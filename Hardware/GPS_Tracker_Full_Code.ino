/*
  ESP32 tracker (configured for your values)
  - GPS: Serial2 (RX=16, TX=17)
  - GSM: Serial1 (RX=4, TX=2)
  - PIR: GPIO13
  - Buzzer: GPIO15
  - Sends SMS and writes to Firebase RTDB (public rules recommended for testing)
  - Libraries: TinyGsm, TinyGPSPlus, ArduinoJson, HTTPClient
*/

#include <TinyGsmClient.h>
#include <TinyGPSPlus.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

// -------- CONFIG (filled) ----------
const char APN[] = "airtelgprs.com";
const char DEVICE_ID[] = "tracker1";
const char PHONE_NUMBER[] = "+919494151022";
const char FIREBASE_HOST[] = "gps-tracker-a7a23-default-rtdb.asia-southeast1.firebasedatabase.app";
const char FIREBASE_AUTH[] = ""; // leave empty for public DB (dev only)

const char FB_LOCATIONS_PATH[] = "/locations";
const char FB_LATEST_LOCATIONS_PATH[] = "/latestLocations";
const char FB_ALERTS_PATH[] = "/alerts";

const bool RELAY_ENABLED = false; // we are not using relay

// Pins
#define MODEM_RX 4
#define MODEM_TX 2
#define GPS_RX 16
#define GPS_TX 17
#define PIR_PIN 13
#define BUZZER_PIN 15

// Behaviour
const unsigned long TRACK_INTERVAL_MS = 30UL * 1000UL; // 30s when tracking
const unsigned long GEOFENCE_POLL_MS  = 60UL * 1000UL; // not used if no geofence set
const unsigned long PIR_DEBOUNCE_MS  = 2000UL;

// TinyGSM modem
#define TINY_GSM_MODEM_SIM800
TinyGsm modem(Serial1);
TinyGsmClient gsmClient(modem);
TinyGsmClientSecure gsmClientSecure(modem);

// GPS
TinyGPSPlus gps;
HardwareSerial SerialGPS(2);

// State
double lastLat = 0.0, lastLon = 0.0;
bool haveFix = false;
bool inTrackingMode = false;
unsigned long lastTrackSend = 0;
unsigned long lastPirTime = 0;

// ---------- helpers ----------
float distanceMeters(double lat1, double lon1, double lat2, double lon2) {
  if (lat1==0 && lon1==0) return 0.0;
  const double R = 6371000.0;
  double dLat = radians(lat2 - lat1);
  double dLon = radians(lon2 - lon1);
  double a = sin(dLat/2)*sin(dLat/2) + cos(radians(lat1))*cos(radians(lat2))*sin(dLon/2)*sin(dLon/2);
  double c = 2*atan2(sqrt(a), sqrt(1-a));
  return (float)(R*c);
}

void toneBeep(int ms=300){
  tone(BUZZER_PIN, 1500);
  delay(ms);
  noTone(BUZZER_PIN);
}

// ---------- Firebase URL helper ----------
String fbUrl(const String &pathWithLeadingSlash) {
  String url = String("https://") + FIREBASE_HOST + pathWithLeadingSlash + ".json";
  if (strlen(FIREBASE_AUTH) > 0) {
    url += "?auth=" + String(FIREBASE_AUTH);
  }
  return url;
}

// ---------- Build JSON helpers ----------
String buildLatestJson(double lat, double lon, bool fix) {
  DynamicJsonDocument d(256);
  d["device"] = DEVICE_ID;
  d["lat"] = lat;
  d["lon"] = lon;
  d["fix"] = fix;
  d["updatedAt"] = (unsigned long)(millis()/1000ULL);
  String out; serializeJson(d,out); return out;
}

String buildPointJson(double lat, double lon, bool fix) {
  DynamicJsonDocument d(256);
  d["lat"] = lat;
  d["lon"] = lon;
  d["fix"] = fix;
  d["ts"] = (unsigned long)(millis()/1000ULL);
  String out; serializeJson(d,out); return out;
}

String buildAlertJson(const char* type, double lat, double lon, bool fix) {
  DynamicJsonDocument d(256);
  d["device"] = DEVICE_ID;
  d["type"] = type;
  d["lat"] = lat;
  d["lon"] = lon;
  d["fix"] = fix;
  d["ts"] = (unsigned long)(millis()/1000ULL);
  String out; serializeJson(d,out); return out;
}

// ---------- GSM & HTTPS helpers ----------
bool ensureNetwork() {
  if (modem.isNetworkConnected()) return true;
  Serial.println("Waiting for network...");
  return modem.waitForNetwork(30);
}
bool ensureGprs() {
  if (modem.isGprsConnected()) return true;
  Serial.println("Connecting GPRS...");
  return modem.gprsConnect(APN, "", "");
}
void endGprsIfNoKeep() {
  if (modem.isGprsConnected()) modem.gprsDisconnect();
}

// HTTPS PATCH (TinyGsmClientSecure)
bool firebasePatch(const String &path, const String &json) {
  if (!ensureNetwork() || !ensureGprs()) return false;
  HTTPClient http;
  String url = fbUrl(path);
  Serial.println("FB PATCH -> " + url);
  // Use secure client; skip cert validation for SIM800 (prototype)
  gsmClientSecure.setInsecure();
  http.begin(gsmClientSecure, url);
  http.addHeader("Content-Type","application/json");
  int code = http.sendRequest("PATCH", (uint8_t*)json.c_str(), json.length());
  String resp = http.getString();
  http.end();
  endGprsIfNoKeep();
  Serial.printf("FB PATCH code=%d resp=%s\n", code, resp.c_str());
  return code >=200 && code < 300;
}

bool firebasePost(const String &path, const String &json) {
  if (!ensureNetwork() || !ensureGprs()) return false;
  HTTPClient http;
  String url = fbUrl(path);
  Serial.println("FB POST -> " + url);
  gsmClientSecure.setInsecure();
  http.begin(gsmClientSecure, url);
  http.addHeader("Content-Type","application/json");
  int code = http.POST(json);
  String resp = http.getString();
  http.end();
  endGprsIfNoKeep();
  Serial.printf("FB POST code=%d resp=%s\n", code, resp.c_str());
  return code >=200 && code < 300;
}

bool firebaseGet(const String &path, String &outJson) {
  if (!ensureNetwork() || !ensureGprs()) return false;
  HTTPClient http;
  String url = fbUrl(path);
  Serial.println("FB GET -> " + url);
  gsmClientSecure.setInsecure();
  http.begin(gsmClientSecure, url);
  int code = http.GET();
  if (code == 200) outJson = http.getString();
  else Serial.printf("FB GET code=%d\n", code);
  http.end();
  endGprsIfNoKeep();
  return code == 200;
}

// ---------- Telemetry ops ----------
bool postTelemetryFirebase(double lat, double lon, bool fix) {
  // latest
  String latestPath = String(FB_LATEST_LOCATIONS_PATH) + "/" + DEVICE_ID;
  String latestJson = buildLatestJson(lat, lon, fix);
  bool okLatest = firebasePatch(latestPath, latestJson);

  // history: push under locations/{device}
  String histPath = String(FB_LOCATIONS_PATH) + "/" + DEVICE_ID;
  String histJson = buildPointJson(lat, lon, fix);
  bool okHist = firebasePost(histPath, histJson);

  return okLatest && okHist;
}
bool logAlertFirebase(const char* type, double lat, double lon, bool fix) {
  String path = String(FB_ALERTS_PATH) + "/" + DEVICE_ID;
  String j = buildAlertJson(type, lat, lon, fix);
  return firebasePost(path, j);
}

// ---------- SMS ----------
void sendSMSAlert(const String &text) {
  Serial.println("Sending SMS...");
  bool ok = modem.sendSMS(PHONE_NUMBER, text.c_str());
  Serial.printf("SMS result: %s\n", ok ? "OK" : "FAILED");
}

// ---------- Events ----------
void onPirTriggered() {
  if (millis() - lastPirTime < PIR_DEBOUNCE_MS) return;
  lastPirTime = millis();
  Serial.println("PIR triggered");
  if (haveFix) {
    String tele = buildPointJson(lastLat, lastLon, haveFix);
    postTelemetryFirebase(lastLat, lastLon, haveFix); // best-effort
    logAlertFirebase("PIR", lastLat, lastLon, haveFix);
    sendSMSAlert(String("PIR ")+DEVICE_ID+" "+String(lastLat,6)+","+String(lastLon,6));
    toneBeep(300);
  } else {
    logAlertFirebase("PIR_NO_GPS", 0, 0, false);
    sendSMSAlert(String("PIR ")+DEVICE_ID+" NO_GPS");
    toneBeep(200);
  }
}

// ---------- Setup ----------
void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("Tracker boot");

  pinMode(PIR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  SerialGPS.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
  delay(50);
  Serial1.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(200);

  Serial.println("Restarting modem...");
  modem.restart();
  Serial.println("Modem: " + String(modem.getModemInfo()));
  // Set modem to SMS text mode
  Serial1.println("AT+CMGF=1");
  delay(200);

  // (Optional) fetch geofence from Firebase if you populated /geofences/{device}
  // skipping in this simplified version — you can add later
}

// ---------- Loop ----------
void loop() {
  // feed GPS parser
  while (SerialGPS.available()) gps.encode(SerialGPS.read());
  if (gps.location.isValid()) {
    haveFix = true;
    lastLat = gps.location.lat();
    lastLon = gps.location.lng();
  } else haveFix = false;

  // PIR check
  if (digitalRead(PIR_PIN) == HIGH) {
    onPirTriggered();
    // small delay to avoid repeat
    delay(500);
  }

  // If in tracking mode (not used here unless you manually set), send periodic
  if (inTrackingMode && millis() - lastTrackSend >= TRACK_INTERVAL_MS) {
    lastTrackSend = millis();
    if (haveFix) {
      postTelemetryFirebase(lastLat, lastLon, haveFix);
      sendSMSAlert(String("TRACK ") + DEVICE_ID + " " + String(lastLat,6) + "," + String(lastLon,6));
    } else {
      sendSMSAlert(String("TRACK_NOPOS ") + DEVICE_ID);
    }
  }

  delay(100);
}
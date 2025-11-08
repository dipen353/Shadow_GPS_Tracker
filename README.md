# SHADOW - GPS Tracking System

**Track what matters, protect what's precious**

SHADOW is a comprehensive real-time GPS tracking solution that combines a modern web dashboard with hardware GPS trackers. Monitor your fleet, assets, and loved ones with live location updates, geofencing, alerts, and historical tracking.

![SHADOW GPS Tracking](https://img.shields.io/badge/Status-Active-success)
![Next.js](https://img.shields.io/badge/Next.js-16.0-black)
![React](https://img.shields.io/badge/React-19.2-blue)
![TypeScript](https://img.shields.io/badge/TypeScript-5.0-blue)
![Firebase](https://img.shields.io/badge/Firebase-Realtime-orange)

## 🌟 Features

### Core Features
- **Real-time Tracking** - Live GPS location updates with minimal latency
- **Device Management** - Add, edit, and manage multiple GPS tracking devices
- **Geofencing** - Create circular and polygon geofences with entry/exit alerts
- **Alert System** - Comprehensive alerts for geofence violations, low battery, offline devices, and more
- **Historical Replay** - Review past journeys and location history
- **Dashboard Analytics** - Overview of active devices, alerts, and tracking statistics
- **Google Maps Integration** - Interactive maps with device markers and real-time updates
- **Multi-Device Support** - Track unlimited devices with grouping capabilities

### Technical Features
- **Real-time Updates** - Firebase Realtime Database for instant data synchronization
- **Responsive Design** - Modern UI built with Tailwind CSS and shadcn/ui
- **Hardware Integration** - Arduino/ESP32 GPS tracker with SIM800 GSM module
- **RESTful API** - Firebase REST API for hardware communication
- **Type Safety** - Full TypeScript support throughout the application

## 📋 Table of Contents

- [Features](#-features)
- [Tech Stack](#-tech-stack)
- [Prerequisites](#-prerequisites)
- [Installation](#-installation)
- [Configuration](#-configuration)
- [Hardware Setup](#-hardware-setup)
- [Usage](#-usage)
- [API Reference](#-api-reference)
- [Development](#-development)
- [Troubleshooting](#-troubleshooting)
- [License](#-license)


## 🛠 Tech Stack

### Frontend
- **Framework**: Next.js 16.0 (App Router)
- **UI Library**: React 19.2
- **Language**: TypeScript 5.0
- **Styling**: Tailwind CSS 4.1
- **UI Components**: shadcn/ui (Radix UI)
- **Icons**: Lucide React
- **Forms**: React Hook Form + Zod
- **Maps**: Google Maps JavaScript API

### Backend
- **Database**: Firebase Realtime Database
- **Authentication**: Firebase (optional)
- **Hosting**: Vercel (recommended)

### Hardware
- **Microcontroller**: ESP32
- **GPS Module**: NEO-6M or similar
- **GSM Module**: SIM800L
- **Sensors**: PIR sensor (optional)
- **Programming**: Arduino IDE

## 📦 Prerequisites

Before you begin, ensure you have the following installed:

- **Node.js** 18.x or higher
- **npm** or **pnpm** or **yarn**
- **Git**
- **Google Cloud Account** (for Maps API)
- **Firebase Account** (for Realtime Database)
- **Arduino IDE** (for hardware setup)

## 🚀 Installation

### 1. Clone the Repository

```bash
git clone <repository-url>
cd shadowww
```

### 2. Install Dependencies

```bash
npm install
```

### 3. Environment Variables

Create a `.env.local` file in the root directory:

```env
# Firebase Configuration
NEXT_PUBLIC_FIREBASE_API_KEY=your_firebase_api_key
NEXT_PUBLIC_FIREBASE_AUTH_DOMAIN=your_firebase_auth_domain
NEXT_PUBLIC_FIREBASE_DATABASE_URL=your_firebase_database_url
NEXT_PUBLIC_FIREBASE_PROJECT_ID=your_firebase_project_id
NEXT_PUBLIC_FIREBASE_STORAGE_BUCKET=your_firebase_storage_bucket
NEXT_PUBLIC_FIREBASE_MESSAGING_SENDER_ID=your_firebase_messaging_sender_id
NEXT_PUBLIC_FIREBASE_APP_ID=your_firebase_app_id
NEXT_PUBLIC_FIREBASE_MEASUREMENT_ID=your_firebase_measurement_id

# Google Maps API
NEXT_PUBLIC_GOOGLE_MAPS_API_KEY=your_google_maps_api_key
NEXT_PUBLIC_GEOLOCATION_API_KEY=your_geolocation_api_key
```

### 4. Firebase Setup

1. Go to [Firebase Console](https://console.firebase.google.com/)
2. Create a new project or select an existing one
3. Enable **Realtime Database**
4. Set database rules (for development, you can use public rules):

```json
{
  "rules": {
    ".read": true,
    ".write": true
  }
}
```

**⚠️ Warning**: Public read/write rules are only for development. For production, implement proper authentication and security rules.

5. Get your Firebase configuration from Project Settings → General → Your apps
6. Copy the configuration values to your `.env.local` file

### 5. Google Maps API Setup

1. Go to [Google Cloud Console](https://console.cloud.google.com/)
2. Create a new project or select an existing one
3. Enable the following APIs:
   - Maps JavaScript API
   - Geolocation API
   - Places API (optional, for address lookup)
4. Create API credentials (API Key)
5. Restrict the API key to your domain (recommended for production)
6. Copy the API key to your `.env.local` file

### 6. Run Development Server

```bash
npm run dev
```

Open [http://localhost:3000](http://localhost:3000) in your browser.

### 7. Build for Production

```bash
npm run build
npm start
# or
pnpm build
pnpm start
```

## ⚙️ Configuration

### Firebase Database Structure

The application uses the following Firebase Realtime Database structure:

```
{
  "devices": {
    "[deviceId]": {
      "id": "deviceId",
      "name": "Device Name",
      "imei": "123456789012345",
      "model": "GPS Tracker Pro",
      "status": "active" | "idle" | "offline",
      "battery": 100,
      "signal": 85,
      "group": "Fleet A",
      "simCard": "SIM-001",
      "firmware": "v2.0.0",
      "icon": "🚗",
      "createdAt": "2024-01-01T00:00:00.000Z",
      "updatedAt": "2024-01-01T00:00:00.000Z"
    }
  },
  "locations": {
    "[deviceId]": {
      "[locationId]": {
        "latitude": 40.7128,
        "longitude": -74.0060,
        "speed": 0,
        "battery": 100,
        "signal": 85,
        "timestamp": "2024-01-01T00:00:00.000Z",
        "accuracy": 10
      }
    }
  },
  "latestLocations": {
    "[deviceId]": {
      "latitude": 40.7128,
      "longitude": -74.0060,
      "speed": 0,
      "battery": 100,
      "signal": 85,
      "timestamp": "2024-01-01T00:00:00.000Z"
    }
  },
  "geofences": {
    "[geofenceId]": {
      "id": "geofenceId",
      "name": "Geofence Name",
      "type": "circle" | "polygon",
      "radius": 1,
      "center": { "lat": 40.7128, "lng": -74.0060 },
      "polygon": [{ "lat": 40.7128, "lng": -74.0060 }],
      "devices": ["deviceId1", "deviceId2"],
      "active": true,
      "alertOnEntry": true,
      "alertOnExit": true,
      "createdAt": "2024-01-01T00:00:00.000Z",
      "updatedAt": "2024-01-01T00:00:00.000Z"
    }
  },
  "alerts": {
    "[alertId]": {
      "id": "alertId",
      "deviceId": "deviceId",
      "deviceName": "Device Name",
      "type": "geofence_entry" | "geofence_exit" | "battery_low" | "offline" | "speeding" | "maintenance",
      "severity": "critical" | "warning" | "info",
      "message": "Alert message",
      "geofenceId": "geofenceId",
      "geofenceName": "Geofence Name",
      "location": { "lat": 40.7128, "lng": -74.0060 },
      "acknowledged": false,
      "acknowledgedAt": "2024-01-01T00:00:00.000Z",
      "timestamp": "2024-01-01T00:00:00.000Z"
    }
  }
}
```

## 🔌 Hardware Setup

### Required Components

- Arduino Uno Development Board
- NEO-6M GPS Module
- SIM800L GSM Module
- Antennas (GPS and GSM)
- SIM Card with data plan
- Voltage regulator (if needed)
- PIR Sensor (optional)
- Buzzer (optional)
- Jumper wires
- Breadboard or PCB

### Wiring Diagram

Refer to the images in the `Hardware/` directory for detailed circuit diagrams.

### Basic Connections

- **GPS Module**:
  - VCC → 3.3V
  - GND → GND
  - TX → GPIO 16
  - RX → GPIO 17

- **GSM Module (SIM800L)**:
  - VCC → 5V (with voltage regulator if needed)
  - GND → GND
  - TX → GPIO 4
  - RX → GPIO 2

- **PIR Sensor** (optional):
  - VCC → 3.3V
  - GND → GND
  - OUT → GPIO 13


### Arduino Code Setup

1. Open Arduino IDE
2. Install the following libraries:
   - `TinyGsm` by Volodymyr Shymanskyy
   - `TinyGPSPlus` by Mikal Hart
   - `ArduinoJson` by Benoit Blanchon
   - `HTTPClient` (included with ESP32)

3. Open `Hardware/GPS_Tracker_Full_Code.ino`
4. Update the following configuration:

```cpp
const char APN[] = "your_apn";  // e.g., "airtelgprs.com"
const char DEVICE_ID[] = "tracker1";
const char PHONE_NUMBER[] = "+1234567890";
const char FIREBASE_HOST[] = "your-project-id-default-rtdb.region.firebasedatabase.app";
```

5. Upload the code to your ESP32
6. Open Serial Monitor (115200 baud) to monitor device status

### Hardware Configuration

1. **SIM Card**: Insert a SIM card with an active data plan
2. **APN Settings**: Configure the correct APN for your carrier
3. **GPS Antenna**: Place the GPS antenna in an area with a clear view of the sky
4. **GSM Antenna**: Attach the GSM antenna for better signal strength
5. **Power**: Connect a stable power source (battery pack or USB power bank)

## 📱 Usage

### Adding a Device

1. Navigate to **Dashboard → Devices**
2. Click **Add Device**
3. Fill in the device information:
   - Device Name
   - IMEI
   - Model
   - SIM Card
   - Group
   - Icon (optional)
4. Click **Add Device**

### Creating a Geofence

1. Navigate to **Dashboard → Geofence**
2. Click **New Geofence**
3. Configure the geofence:
   - Name
   - Type (Circle or Polygon)
   - Radius (for circle)
   - Center coordinates
   - Alert settings (Entry/Exit)
4. Click **Create Geofence**

### Viewing Live Tracking

1. Navigate to **Dashboard → Live Tracking**
2. Select a device from the sidebar
3. View real-time location on the map
4. Monitor device status, speed, battery, and signal strength

### Managing Alerts

1. Navigate to **Dashboard → Alerts**
2. Filter alerts by type or severity
3. Acknowledge or delete alerts as needed
4. View alert history and details

## 🔌 API Reference

### Device Management

```typescript
// Get all devices
getDevices(): Promise<Device[]>

// Get single device
getDevice(deviceId: string): Promise<Device | null>

// Create device
createDevice(device: Omit<Device, "id" | "createdAt" | "updatedAt">): Promise<string>

// Update device
updateDevice(deviceId: string, updates: Partial<Device>): Promise<void>

// Delete device
deleteDevice(deviceId: string): Promise<void>

// Subscribe to devices (real-time)
subscribeToDevices(callback: (devices: Device[]) => void): () => void
```

### Location Tracking

```typescript
// Save location
saveLocation(deviceId: string, location: Omit<LocationData, "deviceId" | "timestamp">): Promise<void>

// Get latest location
getLatestLocation(deviceId: string): Promise<LocationData | null>

// Get location history
getLocationHistory(deviceId: string, limit?: number): Promise<LocationData[]>

// Subscribe to latest locations (real-time)
subscribeToLatestLocations(callback: (locations: Map<string, LocationData>) => void): () => void
```

### Geofence Management

```typescript
// Get all geofences
getGeofences(): Promise<Geofence[]>

// Create geofence
createGeofence(geofence: Omit<Geofence, "id" | "createdAt" | "updatedAt" | "devices">): Promise<string>

// Update geofence
updateGeofence(geofenceId: string, updates: Partial<Geofence>): Promise<void>

// Delete geofence
deleteGeofence(geofenceId: string): Promise<void>

// Assign device to geofence
assignDeviceToGeofence(geofenceId: string, deviceId: string): Promise<void>
```

### Alert Management

```typescript
// Create alert
createAlert(alert: Omit<Alert, "id" | "timestamp" | "acknowledged">): Promise<string>

// Get alerts
getAlerts(limit?: number): Promise<Alert[]>

// Acknowledge alert
acknowledgeAlert(alertId: string): Promise<void>

// Delete alert
deleteAlert(alertId: string): Promise<void>

// Subscribe to alerts (real-time)
subscribeToAlerts(callback: (alerts: Alert[]) => void, limit?: number): () => void
```

## 🧪 Development

### Running Tests

```bash
npm test
```

### Linting

```bash
npm run lint
```

### Type Checking

```bash
npx tsc --noEmit
```

### Building Components

The project uses shadcn/ui for components. To add a new component:

```bash
npx shadcn-ui@latest add [component-name]
```

## 🐛 Troubleshooting

### Firebase Connection Issues

- **Problem**: Firebase not connecting
- **Solution**: 
  - Verify Firebase configuration in `.env.local`
  - Check Firebase Realtime Database rules
  - Ensure Firebase project is active
  - Check network connectivity

### Google Maps Not Loading

- **Problem**: Maps not displaying
- **Solution**:
  - Verify Google Maps API key in `.env.local`
  - Check API key restrictions in Google Cloud Console
  - Ensure Maps JavaScript API is enabled
  - Check browser console for errors

### Device Not Sending Location

- **Problem**: Hardware device not updating location
- **Solution**:
  - Check SIM card and data connection
  - Verify APN settings
  - Check GPS antenna placement
  - Monitor Serial Monitor for errors
  - Verify Firebase host and device ID in Arduino code
  - Check device power supply

### Geofence Alerts Not Triggering

- **Problem**: Geofence alerts not firing
- **Solution**:
  - Verify geofence is active
  - Check device is assigned to geofence
  - Ensure device is sending location updates
  - Check geofence monitoring service is running

### Build Errors

- **Problem**: TypeScript or build errors
- **Solution**:
  - Run `npm install` to update dependencies
  - Check `tsconfig.json` configuration
  - Verify all environment variables are set
  - Clear `.next` folder and rebuild

## 📝 License

This project is licensed under the Apache 2.0 license - see the LICENSE file for details.

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## 🙏 Acknowledgments

-ChatGPT, Cursor, & our brains which was almost on the verge of burnout ;) 
-Sneha Mam for approving the project.
---

**Made with ❤️ by Varun Suvarna, T.Lasya Chowdary, K.Abhiram, P.Sherly Mani Sathwika, M.Sathwik & 353**


# OledDash

OledDash is an ESP32-based modular dashboard system utilizing an SH1106 OLED display (128×64) and a tactile button interface. It features real-time clock integration synced via WiFi and NTP, live weather data, and publishes sensor readings over MQTT to a Spring Boot subscriber for further processing. The project is built with a highly flexible state machine menu system, designed to easily scale into a fully featured smart hub.

---

## Current Features

- **WiFi & NTP Sync:** Automatically fetches and formats local real-time data (+5:30 GMT out of the box) over WiFi.
- **Tactile Navigation:** Uses a simple 3-button configuration (Up, Down, Select) with software-based debouncing locks.
- **Dynamic UI Framework:** Built using the `U8g2` graphics library for crisp rendering on an SH1106 screen.
- **State-Driven UI:** Seamlessly switches pages using clean state structures, keeping the display fast and responsive.
- **Live Weather Integration:** Connects to the OpenWeatherMap API to display metrics for Actual Temperature and Feels Like Temperature, utilizing a built-in 1-minute caching interval to optimize API requests and preserve bandwidth.
- **MQTT Data Publishing:** Publishes live sensor/weather readings (actual temp, feels-like temp) over MQTT to a self-hosted Mosquitto broker, using `PubSubClient`.
- **Spring Boot Subscriber:** A companion server (`server/`) subscribes to the MQTT topic via Spring Integration and processes incoming readings in real time.

---

## Hardware Architecture

- **Microcontroller:** ESP32 (or compatible variant)
- **Display:** SH1106 128×64 OLED (I2C)
- **Control Inputs:**
  - `GPIO 16` — Up Button
  - `GPIO 17` — Down Button
  - `GPIO 18` — Select Button

---

## Project Structure
```
OledDash/
├── (firmware files — PlatformIO project, repo root)
└── server/ # Spring Boot MQTT subscriber
```
**Data flow:** ESP32 → Mosquitto broker → Spring Boot subscriber (`server/`)

---

## Future Roadmap (Expanding the Dash)

- **AWS Dashboard:** Persisting published readings (via the Spring Boot subscriber) into AWS and visualizing them on a live dashboard (CloudWatch/QuickSight).
- **System Monitor:** A PC hardware tracking client via MQTT or Serial (CPU/GPU temps and RAM usage).
- **IoT Smart Toggle:** Remote control relays or Home Assistant integrations directly from the screen.

---

## Getting Started

### Firmware

**Prerequisites** — install these libraries in PlatformIO/Arduino IDE:
- `ArduinoJson` by Benoit Blanchon
- `HttpClient` (Built-in)
- `U8g2` by oliver
- `WiFi` (Built-in)
- `PubSubClient` by Nick O'Leary

**Setup:**

1. Clone the repository:
```bash
   git clone https://github.com/prakhar122808/OledDash.git
```

2. Create a file named `credentials.h` in the same directory as the main sketch, using this template:

```cpp
#pragma once

// Wi-Fi Credentials
#define WIFI_SSID "Your_WiFi_Name"
#define WIFI_PASSWORD "Your_WiFi_Password"

// OpenWeatherMap API Configuration
#define API_KEY "Your_OpenWeatherMap_API_Key"
#define lat Your_Location_Latitude
#define lon Your_Location_Longitude

// MQTT Broker Configuration
#define MQTT_BROKER_IP "Your_Broker_IP"
#define MQTT_TOPIC "esp32/pub"
```

3. Upload and flash as usual via PlatformIO.

### Server (`server/`)

**Prerequisites:**
- Java
- Maven
- A running MQTT broker (e.g. [Mosquitto](https://mosquitto.org/), self-hosted or otherwise)

**Setup:**

1. In `server/src/main/resources/`, create `application-secrets.properties` (gitignored) with:
```properties
url=tcp://your-broker-ip:1883
topic=esp32/pub
```

2. Run the app via IntelliJ, or from the command line:
```bash
cd server
mvn spring-boot:run
```

3. Once the ESP32 is publishing, incoming readings will print to the console. (AWS persistence/dashboarding coming soon — see Roadmap.)

---

# OledDash

OledDash is an ESP32-based modular dashboard system utilizing an SH1106 OLED display ($128 \times 64$) and a tactile button interface. Currently, it features real-time clock integration synced via WiFi and Network Time Protocol (NTP). The project is built with a highly flexible state machine menu system, designed to easily scale into a fully featured smart hub.

---

## Current Features
*   **WiFi & NTP Sync:** Automatically fetches and formats local real-time data (+5:30 GMT out of the box) over WiFi.
*   **Tactile Navigation:** Uses a simple 3-button configuration (Up, Down, Select) with software-based debouncing locks.
*   **Dynamic UI Framework:** Built using the `U8g2` graphics library for crisp rendering on an SH1106 screen.
*   **State-Driven UI:** Seamlessly switches pages using clean state structures, keeping the display fast and responsive.
*   **Live Weather Integration:** Connects to the OpenWeatherMap API to display metrics for Actual Temperature and Feels Like Temperature, utilizing a built-in 1-minute caching interval to optimize API requests and preserve bandwidth.

---

## Hardware Architecture
*   **Microcontroller:** ESP32 (or compatible variant)
*   **Display:** SH1106 $128 \times 64$ OLED (I2C)
*   **Control Inputs:** 
    *   `GPIO 16` - Up Button
    *   `GPIO 17` - Down Button
    *   `GPIO 18` - Select Button

---

## Future Roadmap (Expanding the Dash)
OledDash is engineered to be more than just a desk clock. Upcoming modules slated for future implementation include:

*   **Weather Widget:** Integration with OpenWeatherMap API to display real-time local weather.
*   **System Monitor:** A PC hardware tracking client via MQTT or Serial (CPU/GPU temps and RAM usage).
*   **IoT Smart Toggle:** Remote control relays or Home Assistant integrations directly from the screen.

---

## Getting Started

### Prerequisites
Ensure you have the following libraries installed in your Arduino IDE or PlatformIO project:
*   `ArduinoJson` by Benoit Blanchon
*   `HttpClient` (Built-in)
*   `U8g2` by oliver
*   `WiFi` (Built-in)

### Installation and Configuration
1. Clone the repository:
   ```bash
   git clone https://github.com/prakhar122808/OledDash.git
2. Create a file named `credentials.h` in the same directory as your main sketch file.

3. Populate credentials.h with your configurations following this template:

```C++
#pragma once

// Wi-Fi Credentials
#define WIFI_SSID "Your_WiFi_Name"
#define WIFI_PASSWORD "Your_WiFi_Password"

// OpenWeatherMap API Configuration
#define API_KEY "Your_OpenWeatherMap_API_Key"
#define lat Your_Location_Latitude
#define lon Your_Location_Longitude

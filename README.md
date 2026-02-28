# SENTINEL-X v9.0 (Batch 1) 🛡️

**SENTINEL-X** is an autonomous, ESP32-powered desk assistant and sentry robot designed for real-time target acquisition and environment monitoring using **Time-of-Flight (ToF)** laser technology.

![Sentinel-X Status](https://img.shields.io/badge/Status-Stable-success)
![Platform](https://img.shields.io/badge/Platform-ESP32-blue)
![Language](https://img.shields.io/badge/Language-C++-orange)

---

## 🚀 Key Features
- **Sequential Target Scanning:** Implements a 15-degree step resolution with a 1000ms "Move-then-Read" cycle for maximum data stability.
- **Smart Target Locking:** Automatically calculates the nearest object distance and locks the servo orientation for 3 minutes before re-scanning.
- **HUD (Head-Up Display):** Features a dynamic OLED interface with animated "Blinking Eyes," real-time system clock, and connectivity status.
- **Integrated Prayer Sync:** Synchronizes with the Aladhan API to provide real-time Islamic prayer time countdowns.
- **Environment HUD:** Real-time ambient temperature monitoring via the BMP085 barometric sensor.

---

## 🛠️ Hardware Anatomy (Deep Dive)
Driven by a passion for technical precision, SENTINEL-X is built upon a highly efficient **I2C Shared Bus** architecture.

### 1. The Core
- **ESP32 Lolin32 Lite**: A high-performance dual-core MCU with integrated WiFi 2.4GHz. Chosen for its compact footprint, ideal for 25mm enclosure integration.

### 2. The Optics
- **VL53L0X Time-of-Flight (ToF)**: A premium laser-ranging sensor. Unlike traditional ultrasonic sensors, ToF measures the absolute distance of a target with millimeter precision by calculating the flight time of photons.

### 3. The Actuators
- **MG90S Micro Servo (Metal Gear)**: Provides robust and stable rotational movement for the radar sweep, ensuring high torque and longevity.

### 4. The Interface
- **SSD1306 128x64 OLED**: A monochrome graphical display for the HUD.
- **BMP085 (GY-68)**: High-precision barometric and thermal sensor.

---

## 🧠 Logic Flow & Architecture
SENTINEL-X operates on a **Finite State Machine (FSM)** logic:

1. **BOOT_STATE**: Initializes hardware, syncs WiFi, and performs a 90-degree servo calibration.
2. **SCANNING_STATE**: Performs a 0-180 degree sweep. The system waits for mechanical stabilization (1s) before triggering the ToF sensor to eliminate noise.
3. **LOCKING_STATE**: Evaluates the `minDistance` buffer and re-orients the servo to the `bestAngle` (the closest detected point).
4. **IDLE_WAIT**: Enters a 3-minute cooldown with the ToF laser in sleep mode to optimize power efficiency.



---

## 📂 Installation
1. Clone this repository.
2. Open the project in **VS Code** with **PlatformIO**.
3. Install required libraries: `Adafruit SSD1306`, `Adafruit VL53L0X`, `ESP32Servo`, `ArduinoJson`.
4. Update your WiFi credentials (`ssid` & `password`) in `main.cpp`.
5. Build and Upload to your ESP32 board.

---

## 🛡️ License
Distributed under the **MIT License**. See `LICENSE` for more information.

## 👤 Author
**Imam Falahi**
*B.S. in Informatics Student*
*Universitas Siber Muhammadiyah*

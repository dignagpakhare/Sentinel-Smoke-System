
# AeroGuard IoT: Hybrid Smart Smoke Detection System

A high-performance, dual-processor IoT safety solution using **STM32** for real-time sensor processing and **ESP32** for cloud connectivity and **Telegram Bot** integration.

## 🚀 Key Features
* **Hybrid Architecture:** Uses STM32 (Local Controller) and ESP32 (IoT Gateway) via UART.
* **Trend Analysis:** Intelligently detects if smoke levels are *Rising*, *Falling*, or *Stable*.
* **Interactive Telegram Bot:** Remote monitoring via `/status` command and instant high-alert notifications.
* **Real-time Dashboard:** Local 16x2 I2C LCD display for instant data visualization.
* **Smart Alert Logic:** Progressive buzzing and automated reporting only during critical state changes.

## 🛠️ Hardware Components
* **Microcontrollers:** STM32 (Blue Pill), ESP32 (NodeMCU)
* **Sensor:** MQ-Series Smoke/Gas Sensor
* **Display:** 16x2 I2C LCD Module
* **Notification:** Active Buzzer & Telegram API
* **Communication:** UART (Serial) @ 115200 Baud

## 🔌 Circuit Connection
| Component | STM32 Pin | ESP32 Pin |
| :--- | :--- | :--- |
| Smoke Sensor | PA0 (Analog) | - |
| I2C LCD (SDA/SCL) | PB7 / PB6 | - |
| Buzzer | PA1 | - |
| UART Bridge | PA9 (TX) -> | Pin 16 (RX2) |
| UART Bridge | PA10 (RX) <-| Pin 17 (TX2) |
| Ground | GND | GND (Common) |

## 💻 Software & Libraries
* **Language:** C++ (Arduino Framework)
* **Libraries:** * `UniversalTelegramBot` (for Telegram API)
    * `ArduinoJson` (for Data Parsing)
    * `LiquidCrystal_I2C` (for LCD display)

## 📖 How it Works
1.  **STM32** samples the MQ sensor, filters noise, and updates the LCD and local Buzzer.
2.  Data is sent to **ESP32** every second via Serial Communication.
3.  **ESP32** connects to WiFi and checks the data trend.
4.  If a status change is detected (e.g., Safe to High Alert), a **Telegram notification** is sent.
5.  Users can interact with the bot using the `/status` command to get instant readings.

---
Developed by **Dignag Pakhare** | Electronics & Telecommunication Engineering

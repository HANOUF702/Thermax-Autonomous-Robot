# Thermax: Autonomous Server Room Monitoring Robot 🤖🌡️

## 📌 Overview
Thermax is an autonomous IoT robot co-designed for real-time environmental analysis and 360° obstacle avoidance. It utilizes embedded C++ firmware on Arduino and ESP32 microcontrollers to monitor server room conditions.

## ✨ Key Features
- **Autonomous Navigation:** Features 360° obstacle avoidance for seamless and secure movement.
- **Real-Time Environmental Analysis:** Continuously monitors room temperature to ensure optimal operating conditions.
- **Instant Automated Alerts:** Implements secure WiFi/HTTPS communication via the Telegram Bot API to trigger instant alerts when critical thermal thresholds (>28°C) are breached.

## 🛠️ Technologies & Hardware
- **Microcontrollers:** Arduino, ESP32
- **Programming Languages:** C/C++
- **Communication & APIs:** Telegram Bot API, WiFi/HTTPS Protocols

## 📂 Repository Structure
- `/Thermax_IoT_ESP32` - ESP32 firmware handling secure WiFi connections and Telegram API alerts.
- `/Thermax_Standalone_Arduino` - Core Arduino logic for sensor integration and motor control.
- `/Thermax-Robot` - Main integrated robotic control systems.

# 🔋 Smart Battery Dashboard V1

A smart ESP32-based battery monitoring system that tracks real-time battery voltage, charging status, and battery percentage through a clean web-based dashboard.

The project uses an **18650 battery**, **TP4056 charging module**, and voltage sensing circuits to monitor the battery and display live information on any device connected to the same Wi-Fi network.

---

## ✨ Features

- 🔋 Real-time battery voltage monitoring
- 📊 Live battery percentage calculation
- ⚡ Charging status detection
- 🔌 Battery connected/disconnected detection
- 🟢 Normal, Charging, Low, Full, and No Battery status
- 🌐 Web-based monitoring dashboard
- 📱 Mobile-friendly interface
- 🔄 Automatic dashboard updates
- 📈 Smoothed voltage readings
- 🚀 Optimized for ESP32

---

## 🛠️ Hardware Required

| Component | Quantity |
|-----------|:--------:|
| ESP32 Development Board | 1 |
| 18650 Lithium-Ion Battery | 1 |
| TP4056 Charging Module | 1 |
| 2KΩ Resistor | 1 |
| 1KΩ Resistor | 3 |
| Breadboard | 1 |
| Jumper Wires | As Required |
| USB Cable | 1 |

---

## 🔌 Wiring

### Battery Voltage Monitoring

The battery voltage is connected to the ESP32 through a voltage divider circuit.

| Connection | ESP32 Pin |
|------------|-----------|
| Battery Voltage Sense | GPIO 35 |
| GND | GND |

### Charger Detection

The TP4056 charging output is monitored by the ESP32 through a resistor divider.

| Connection | ESP32 Pin |
|------------|-----------|
| Charger Detection | GPIO 34 |
| GND | GND |

> ⚠️ Do not connect the battery or charger voltage directly to an ESP32 GPIO pin. Use the voltage divider circuit shown in the wiring diagram.

---

## 💻 Software Requirements

Install the following using the **Arduino IDE**:

- ESP32 Board Package
- WiFi
- WebServer

The project uses the ESP32's built-in ADC to measure battery and charger voltage.

---

## 🚀 Getting Started

1. Download or clone this repository.
2. Open the project in Arduino IDE.
3. Open the main `.ino` file.
4. Enter your Wi-Fi name and password in the Wi-Fi configuration section.
5. Build the circuit according to the wiring diagram.
6. Select your ESP32 board and COM Port.
7. Upload the sketch.
8. Open the Serial Monitor at **115200 baud**.
9. Wait for the ESP32 to connect to Wi-Fi.
10. Copy the IP address displayed in the Serial Monitor.
11. Open the IP address in a browser.
12. Monitor your battery in real time.

The ESP32 connects to an existing Wi-Fi network and starts a web server for the battery dashboard. :contentReference[oaicite:2]{index=2}

---

## 📊 Battery Dashboard

The web dashboard displays:

- 🔋 Battery voltage
- 📈 Battery percentage
- ⚡ Charging status
- 🟢 Normal battery status
- 🟠 Low battery warning
- 🔌 Charging indication
- 🚫 No battery detection

The dashboard automatically requests updated battery data at regular intervals for near real-time monitoring. :contentReference[oaicite:3]{index=3} :contentReference[oaicite:4]{index=4}

---

## 🔋 Battery Percentage

The battery percentage is calculated from the measured battery voltage using a voltage-to-percentage mapping.

The system also uses smoothing to reduce sudden voltage fluctuations and provide a more stable battery reading. :contentReference[oaicite:5]{index=5} :contentReference[oaicite:6]{index=6}

---

## ⚡ Charging Detection

The ESP32 monitors the charger voltage through **GPIO 34**.

When the charger voltage crosses the configured threshold, the system updates the battery status to **Charging**. Separate ON and OFF thresholds help prevent unstable status switching. :contentReference[oaicite:7]{index=7}

---

## 📶 Wi-Fi Setup

The ESP32 **does not create its own hotspot**.

It connects to an existing Wi-Fi network, such as:

- 📱 Phone hotspot
- 🏠 Home Wi-Fi
- 💻 Laptop/PC hotspot

After connecting, the ESP32 displays its local IP address in the Serial Monitor.

Open that IP address from a device connected to the **same Wi-Fi network**.

---

## ⚠️ Troubleshooting

| Problem | Solution |
|---------|----------|
| Battery voltage shows 0V | Check the battery connection and voltage divider wiring |
| Incorrect voltage reading | Verify the resistor values and wiring connections |
| Charging status doesn't change | Check the TP4056 sensing connection to GPIO 34 |
| ESP32 doesn't connect | Verify Wi-Fi name and password |
| Web page doesn't open | Make sure both devices are connected to the same Wi-Fi network |
| Battery percentage looks incorrect | Check the battery voltage and calibration values |
| Upload failed | Verify the ESP32 board and COM Port |
| IP address not shown | Open Serial Monitor at 115200 baud |

---

## 📚 Technologies Used

- ESP32
- Arduino IDE
- C++
- Wi-Fi
- WebServer
- HTML
- CSS
- JavaScript
- ESP32 ADC
- 18650 Battery
- TP4056

---

## ⭐ Support

If you found this project helpful, consider giving this repository a **⭐ Star**.

Your support helps **LifeTronix** create more open-source Arduino, ESP32, IoT, and Robotics projects.

---

## 📄 License

This project is licensed under the **MIT License**.

---

<p align="center">
Made with ❤️ by <strong>LifeTronix</strong>
</p>

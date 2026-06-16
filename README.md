# 🤖 ESP32 IoT Companion Bot

The ESP32 IoT Companion Bot is a smart, interactive desktop virtual pet and weather station. Built as part of the Newton School of Technology curriculum, this project brings hardware to life by combining capacitive touch interactions, audio-visual feedback, and real-time IoT data fetching into a single, cohesive embedded system.

Designed for the ESP32 microcontroller, it utilizes a custom non-blocking architecture to ensure fluid physical interactions while continuously fetching live weather data in the background.

✨ Key Features

* 🎭 6 Interactive Personalities: Cycles through distinct emotional states (Happy, Sad, Angry, Calm, Surprised, Winking) via a capacitive touch sensor.
* 🎨 Synchronized Audio-Visuals: Each mood triggers a specific NeoPixel LED color profile and a custom multi-tone buzzer jingle.
* 🌦️ Real-Time Weather Dashboard: Fetches live weather data (Current Temperature, Daily Highs/Lows) for your configured city using the OpenWeatherMap API.
* 🖥️ Dual-Pane OLED UI: Custom graphical interface rendered on a 128x64 OLED display, showing a live dynamic geometric emoji face on the left and a structured weather feed on the right.
* ⏱️ Non-Blocking Timers: Utilizes `millis()` for background network requests (updating weather every 10 minutes) to prevent interface freezing and ensure the touch sensor is always responsive.

---

## 🛠️ Hardware Requirements

If building physically, you will need the following components:

* 1x ESP32 Development Board (NodeMCU-32S or similar)
* 1x 0.96" I2C OLED Display (SSD1306, 128x64)
* 1x WS2812B NeoPixel LED
* 1x Active/Passive Piezo Buzzer
* 1x Capacitive Touch Sensor (e.g., TTP223) or a standard Push Button
* Jumper Wires & Breadboard

### 🔌 Pin Mapping

| Component | ESP32 Pin | Notes |
| --- | --- | --- |
| **Buzzer** | `GPIO 13` | Audio output for mood jingles |
| **NeoPixel (DIN)** | `GPIO 25` | Addressable RGB LED control |
| **Touch Sensor (SIG)** | `GPIO 27` | Input pulled up (`INPUT_PULLUP`) |
| **OLED SDA** | `GPIO 21` | I2C Data Line |
| **OLED SCL** | `GPIO 22` | I2C Clock Line |

---

## 💻 Software Dependencies

To compile and upload this code, ensure you have the ESP32 Board Package installed in your Arduino IDE, along with the following libraries (installable via the Arduino Library Manager):

1. `Adafruit GFX Library`
2. `Adafruit SSD1306`
3. `Adafruit NeoPixel`
4. `ArduinoJson` (v6.x or v7.x)

---

## 🚀 Installation & Setup

### 1. Clone the Repository

```bash
git clone https://github.com/yourusername/esp32-companion-bot.git
cd esp32-companion-bot

```

### 2. API & Network Configuration

Open the `.ino` file in the Arduino IDE. Before compiling, you need to configure your Wi-Fi and API settings:

```cpp
// ── Wi-Fi Configuration ────────────────
const char* ssid     = "YOUR_WIFI_SSID"; 
const char* password = "YOUR_WIFI_PASSWORD";            

// ── OpenWeatherMap API ─────────────────
const String apiKey   = "YOUR_OPENWEATHERMAP_API_KEY";
const String city     = "Delhi"; // Set your city
const String country  = "IN";    // Set your country code

```

*Note: You can get a free API key by signing up at [OpenWeatherMap](https://openweathermap.org/api).

### 3. Flash to ESP32

Select your specific ESP32 board and COM port from the `Tools` menu in the Arduino IDE, then click **Upload**.

---

## 🌐 Running in Wokwi Simulator

This project is fully optimized for the [Wokwi Online Simulator](https://wokwi.com/).
If you are simulating this project rather than building it physically:

1. Copy the code into a new ESP32 Wokwi project.
2. Ensure the `diagram.json` is wired according to the Pin Mapping table above.
3. Leave the Wi-Fi credentials as default:
* `ssid = "Wokwi-GUEST"`
* `password = ""`



---

## 🕹️ Usage

1. Power On: The bot will display a "Connecting Wi-Fi..." screen, attempt to connect to the network, and play a startup jingle.
2. Dashboard Initialization: Once connected, it will fetch the latest weather JSON payload, parse it, and render the default "Happy" face alongside the local temperatures.
3. Interaction:Tap the touch sensor to cycle through the companion's moods. Notice how the screen, LED, and buzzer react instantly without disrupting the background weather timers.

---

## 🤝 Contributing

Contributions, issues, and feature requests are welcome!
Feel free to check [issues page](https://www.google.com/search?q=https://github.com/yourusername/esp32-companion-bot/issues) if you want to contribute.

Potential Future Enhancements:

* Add more complex animations to the OLED eyes (blinking).
* Implement a Web Server interface to change settings via a smartphone.
* Add an LDR (Light Dependent Resistor) to auto-dim the OLED and NeoPixel at night.

## 📝 License

This project is open-source and available under the [MIT License](https://www.google.com/search?q=LICENSE).

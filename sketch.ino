
// Companion Bot 
// Newton School of Technology
// Components: NeoPixel · Buzzer · Touch · OLED · Background Weather
// Simulation Environment: Wokwi
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// ── Wi-Fi & API Configuration ────────────────
const char* ssid     = "Wokwi-GUEST"; // Wokwi Simulator Access Point
const char* password = "";            // No password for simulation AP

const String apiKey   = "5354c05cc56284352a6c1706b425a7ba";
const String city     = "Delhi";
const String country  = "IN";

// ── Pin definitions ─────────────────────────
const int BUZZER_PIN = 13;
const int NEO_PIN    = 25;
const int TOUCH_PIN  = 27;
const int SDA_PIN    = 21;
const int SCL_PIN    = 22;

// ── OLED ────────────────────────────────────
#define SCREEN_W 128
#define SCREEN_H  64
Adafruit_SSD1306 display(SCREEN_W, SCREEN_H, &Wire);

// ── NeoPixel ────────────────────────────────
Adafruit_NeoPixel pixel(1, NEO_PIN, NEO_GRB + NEO_KHZ800);

// ── Global state ────────────────────────────
String moods[]    = {"happy", "sad", "angry", "calm", "surprised", "winking"};
int    moodIndex    = 0;
int    maxMoods     = 6; 
int    lastTouch    = HIGH;

// Weather tracking variables
float currentTemp = 0.0;
float tempMin     = 0.0;
float tempMax     = 0.0;
bool weatherValid = false;

// Non-blocking timers for background data retrieval
unsigned long lastWeatherUpdate = 0;
const unsigned long updateInterval = 600000; // Update weather every 10 minutes

// ────────────────────────────────────────────
// Fetch Weather Data Function (Fixed Type Conversions)
// ────────────────────────────────────────────
void updateWeather() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + country + "&appid=" + apiKey + "&units=metric";
    
    Serial.println("[Weather] Fetching updates from URL...");
    http.begin(url);
    int httpCode = http.GET();
    
    if (httpCode == 200) {
      String payload = http.getString();
      
      // Allocating a 2048-byte buffer ensures all payload properties map without overflow
      StaticJsonDocument<2048> doc;
      DeserializationError error = deserializeJson(doc, payload);
      
      if (!error) {
        // FIXED: Added explicit data type casting so parameters map instead of defaulting to 0
        currentTemp  = doc["main"]["temp"].as<float>();
        tempMin      = doc["main"]["temp_min"].as<float>();
        tempMax      = doc["main"]["temp_max"].as<float>();
        weatherValid = true;
        
        Serial.print("[Weather] Updated Successfully. Live Temp: ");
        Serial.println(currentTemp);
      } else {
        Serial.print("[Weather] JSON parsing error: ");
        Serial.println(error.c_str());
        weatherValid = false;
      }
    } else {
      Serial.print("[Weather] HTTP Request Error Code: ");
      Serial.println(httpCode);
      weatherValid = false;
    }
    http.end();
  } else {
    Serial.println("[Weather] Wi-Fi link down. Skipping update cycle.");
    weatherValid = false;
  }
}

// ────────────────────────────────────────────
// NeoPixel — one colour per mood
// ────────────────────────────────────────────
void setMoodColor(String mood) {
  if      (mood == "happy")     pixel.setPixelColor(0, pixel.Color(255, 200,   0)); // Yellow
  else if (mood == "sad")       pixel.setPixelColor(0, pixel.Color(  0,  50, 255)); // Blue
  else if (mood == "angry")     pixel.setPixelColor(0, pixel.Color(255,   0,   0)); // Red
  else if (mood == "calm")      pixel.setPixelColor(0, pixel.Color(  0, 200, 180)); // Cyan
  else if (mood == "surprised") pixel.setPixelColor(0, pixel.Color(255,  80,   0)); // Orange
  else if (mood == "winking")   pixel.setPixelColor(0, pixel.Color(200,   0, 255)); // Purple
  pixel.show();
}

// ────────────────────────────────────────────
// Buzzer — jingle per mood
// ────────────────────────────────────────────
void beepMood(String mood) {
  if (mood == "happy") {
    tone(BUZZER_PIN, 659, 100); delay(120);
    tone(BUZZER_PIN, 784, 100); delay(120);
    tone(BUZZER_PIN, 880, 150); delay(200);
  }
  else if (mood == "sad") {
    tone(BUZZER_PIN, 294, 400); delay(450);
    tone(BUZZER_PIN, 247, 500); delay(550);
  }
  else if (mood == "angry") {
    for (int i = 0; i < 3; i++) {
      tone(BUZZER_PIN, 330, 80); delay(100);
    }
  }
  else if (mood == "calm") {
    tone(BUZZER_PIN, 440, 300); delay(350);
  }
  else if (mood == "surprised") {
    tone(BUZZER_PIN, 523, 80); delay(90);
    tone(BUZZER_PIN, 1047, 200); delay(220);
  }
  else if (mood == "winking") {
    tone(BUZZER_PIN, 784, 100); delay(120);
    tone(BUZZER_PIN, 988, 120); delay(150);
  }
}

void playStartupJingle() {
  tone(BUZZER_PIN, 523, 120); delay(140);
  tone(BUZZER_PIN, 659, 120); delay(140);
  tone(BUZZER_PIN, 784, 120); delay(140);
  tone(BUZZER_PIN, 1047, 250); delay(400);
}

// ────────────────────────────────────────────
// OLED — Dual Dashboard Rendering
// ────────────────────────────────────────────
void updateScreen(String mood) {
  display.clearDisplay();

  // ── LEFT SIDE: Emoji Face (Centered around X=32, Y=32) ──
  int faceX = 32;
  int faceY = 32;
  display.drawCircle(faceX, faceY, 26, SSD1306_WHITE); 

  if (mood == "happy") {
    display.drawLine(faceX - 12, faceY - 5, faceX - 6, faceY - 11, SSD1306_WHITE);
    display.drawLine(faceX - 6, faceY - 11, faceX, faceY - 5, SSD1306_WHITE);
    display.drawLine(faceX + 6, faceY - 5, faceX + 12, faceY - 11, SSD1306_WHITE);
    display.drawLine(faceX + 12, faceY - 11, faceX + 18, faceY - 5, SSD1306_WHITE);
    display.drawRoundRect(faceX - 10, faceY + 4, 20, 10, 5, SSD1306_WHITE);
    display.fillRect(faceX - 10, faceY + 4, 20, 4, SSD1306_BLACK); 
    display.drawLine(faceX - 10, faceY + 4, faceX + 10, faceY + 4, SSD1306_WHITE);
  }
  else if (mood == "sad") {
    display.drawLine(faceX - 14, faceY - 8, faceX - 4, faceY - 4, SSD1306_WHITE);
    display.drawLine(faceX + 14, faceY - 8, faceX + 4, faceY - 4, SSD1306_WHITE);
    display.fillCircle(faceX - 9, faceY - 2, 2, SSD1306_WHITE);
    display.fillCircle(faceX + 9, faceY - 2, 2, SSD1306_WHITE);
    display.fillTriangle(faceX - 12, faceY + 6, faceX - 9, faceY + 2, faceX - 6, faceY + 6, SSD1306_WHITE);
    display.fillCircle(faceX - 9, faceY + 7, 2, SSD1306_WHITE);
    display.drawCircle(faceX, faceY + 15, 5, SSD1306_WHITE);
    display.fillRect(faceX - 8, faceY + 15, 16, 7, SSD1306_BLACK); 
  }
  else if (mood == "angry") {
    display.drawLine(faceX - 15, faceY - 11, faceX - 3, faceY - 5, SSD1306_WHITE);
    display.drawLine(faceX + 15, faceY - 11, faceX + 3, faceY - 5, SSD1306_WHITE);
    display.fillRect(faceX - 12, faceY - 3, 8, 5, SSD1306_WHITE);
    display.fillRect(faceX + 4, faceY - 3, 8, 5, SSD1306_WHITE);
    display.drawRoundRect(faceX - 8, faceY + 9, 16, 7, 2, SSD1306_WHITE);
    display.drawLine(faceX - 8, faceY + 12, faceX + 8, faceY + 12, SSD1306_WHITE); 
  }
  else if (mood == "calm") {
    display.drawLine(faceX - 12, faceY - 3, faceX - 2, faceY - 3, SSD1306_WHITE);
    display.drawLine(faceX + 2, faceY - 3, faceX + 12, faceY - 3, SSD1306_WHITE);
    display.drawCircle(faceX, faceY + 5, 5, SSD1306_WHITE);
    display.fillRect(faceX - 8, faceY + 2, 16, 5, SSD1306_BLACK); 
  }
  else if (mood == "surprised") {
    display.drawCircle(faceX - 9, faceY - 4, 4, SSD1306_WHITE);
    display.drawCircle(faceX + 9, faceY - 4, 4, SSD1306_WHITE);
    display.fillCircle(faceX - 9, faceY - 4, 1, SSD1306_WHITE);
    display.fillCircle(faceX + 9, faceY - 4, 1, SSD1306_WHITE);
    display.drawCircle(faceX - 9, faceY - 4, 6, SSD1306_WHITE);
    display.drawCircle(faceX + 9, faceY - 4, 6, SSD1306_WHITE);
    display.fillRect(faceX - 17, faceY - 4, 34, 10, SSD1306_BLACK); 
    display.drawCircle(faceX, faceY + 10, 5, SSD1306_WHITE);
  }
  else if (mood == "winking") {
    display.fillCircle(faceX - 9, faceY - 3, 3, SSD1306_WHITE);
    display.drawLine(faceX + 4, faceY - 2, faceX + 9, faceY - 5, SSD1306_WHITE);
    display.drawLine(faceX + 9, faceY - 5, faceX + 14, faceY - 2, SSD1306_WHITE);
    display.drawCircle(faceX - 2, faceY + 6, 8, SSD1306_WHITE);
    display.fillRect(faceX - 12, faceY - 4, 20, 10, SSD1306_BLACK); 
  }

  // ── RIGHT SIDE: Continuous Weather Dashboard ──
  display.drawFastVLine(66, 0, SCREEN_H, SSD1306_WHITE);

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(72, 2);
  display.print(city);

  display.drawFastHLine(66, 13, SCREEN_W - 66, SSD1306_WHITE);

  if (weatherValid) {
    // Current Realtime Temperature
    display.setCursor(72, 18);
    display.print(currentTemp, 1);
    display.print((char)247);
    display.print("C");

    display.drawFastHLine(70, 31, SCREEN_W - 70, SSD1306_WHITE);

    // 24 Hour Parameter readings
    display.setCursor(72, 36);
    display.print("Hi: ");
    display.print(tempMax, 1);
    display.print((char)247);

    display.setCursor(72, 48);
    display.print("Lo: ");
    display.print(tempMin, 1);
    display.print((char)247);
  } else {
    display.setCursor(72, 25);
    display.print("No data");
  }

  display.display();
}

// ────────────────────────────────────────────
// Touch — single tap cycles emoji face only
// ────────────────────────────────────────────
void onTap() {
  moodIndex = (moodIndex + 1) % maxMoods;
  String mood = moods[moodIndex];
  setMoodColor(mood);
  beepMood(mood);
  updateScreen(mood); 
  Serial.println("Face changed to: " + mood);
}

// ────────────────────────────────────────────
// setup() — runs once on power-on
// ────────────────────────────────────────────
void setup() {
  Serial.begin(115200);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(TOUCH_PIN, INPUT_PULLUP);

  pixel.begin();
  pixel.setBrightness(80);
  pixel.clear(); pixel.show();

  Wire.begin(SDA_PIN, SCL_PIN);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 25);
  display.print("Connecting Wi-Fi...");
  display.display();

  WiFi.begin(ssid, password);
  int retryCounter = 0;
  while (WiFi.status() != WL_CONNECTED && retryCounter < 20) {
    delay(500);
    Serial.print(".");
    retryCounter++;
  }
  
  updateWeather();
  lastWeatherUpdate = millis();

  playStartupJingle();
  setMoodColor(moods[moodIndex]);
  updateScreen(moods[moodIndex]);
}

// ────────────────────────────────────────────
// loop() — runs forever
// ────────────────────────────────────────────
void loop() {
  int state = digitalRead(TOUCH_PIN);
  if (state == LOW && lastTouch == HIGH) {
    onTap();
    delay(50);  // debounce
  }
  lastTouch = state;

  if (millis() - lastWeatherUpdate >= updateInterval) {
    updateWeather();
    lastWeatherUpdate = millis();
    updateScreen(moods[moodIndex]); 
  }
}

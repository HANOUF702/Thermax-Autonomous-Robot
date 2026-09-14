#include <WiFi.h>
#include <HTTPClient.h>

// ================= Wi-Fi & API Configurations =================
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

String botToken = "YOUR_TELEGRAM_BOT_TOKEN";
String chatId = "YOUR_CHAT_ID";

// Using Hardware Serial 2 (ESP32 RX: 16, ESP32 TX: 17) to communicate with Arduino
#define RXp2 16
#define TXp2 17

void setup() {
  Serial.begin(115200);
  
  // Start Serial communication with Arduino
  Serial2.begin(9600, SERIAL_8N1, RXp2, TXp2);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");
}

void loop() {
  // Check if Arduino sent an alert over Serial
  if (Serial2.available()) {
    String data = Serial2.readStringUntil('\n');
    data.trim();

    // Parse the data looking for the "ALERT:" keyword
    if (data.startsWith("ALERT:")) {
      String temp = data.substring(6); // Extract the temperature value
      
      String message = "🚨 CRITICAL ALERT: Thermax detected high temperature!\n";
      message += "🌡️ Current Temp: " + temp + " °C\n";
      message += "⚠️ Motor functions halted. Immediate inspection required.";
      
      sendTelegramMessage(message);
    }
  }
}

void sendTelegramMessage(String msg) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "https://api.telegram.org/bot" + botToken + "/sendMessage?chat_id=" + chatId + "&text=" + msg;
    
    http.begin(url);
    int httpResponseCode = http.GET();
    
    if (httpResponseCode > 0) {
      Serial.println("✅ Telegram Alert Sent Successfully!");
    } else {
      Serial.print("❌ Error sending message. HTTP Code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected!");
  }
}
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include "time.h"

const char* ssid = "wifi"; 
const char* password = "12345678";
#define BOT_TOKEN "8695416735:AAEGp7qDVk3-xf8EWajqz12QmwsZLDnaFTI"
#define CHAT_ID "1474524616"

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

unsigned long lastTimeBotRan;
int botRequestDelay = 2000; 

int currentSmokeValue = 0;
int lastSmokeValue = 0;
String serialData = "";
String lastStatus = "SAFE";

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 19800; 

String getTime() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)) return "Time Error";
  char timeStringBuff[50];
  strftime(timeStringBuff, sizeof(timeStringBuff), "%I:%M:%S %p", &timeinfo);
  return String(timeStringBuff);
}

String getTrend() {
  int diff = currentSmokeValue - lastSmokeValue;
  if (abs(diff) < 15) return "STABLE 🟢";
  if (diff > 0) return "RISING 📈";
  return "FALLING 📉";
}

void sendTelegramMsg(String reason) {
  String status = "";
  String emoji = "";
  if(currentSmokeValue < 450) { status = "SAFE"; emoji = "✅"; }
  else if(currentSmokeValue < 650) { status = "NORMAL"; emoji = "⚠️"; }
  else { status = "HIGH ALERT"; emoji = "🚨"; }

  String msg = emoji + " *SMOKE REPORT* " + emoji + "\n\n";
  msg += "*Trigger:* " + reason + "\n";
  msg += "*Status:* " + status + "\n";
  msg += "*Reading:* " + String(currentSmokeValue) + "\n";
  msg += "*Trend:* " + getTrend() + "\n";
  msg += "*Time:* " + getTime();

  bot.sendMessage(CHAT_ID, msg, "Markdown");
}

void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID) continue;
    String text = bot.messages[i].text;
    if (text == "/status") {
      sendTelegramMsg("User Requested");
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, 16, 17); // Pin 16-RX, 17-TX
  client.setInsecure();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  configTime(gmtOffset_sec, 0, ntpServer);
  bot.sendMessage(CHAT_ID, "🛡️ System Online! Use /status for updates.", "");
}

void loop() {
  // 1. STM32 Data Processing
  while (Serial2.available() > 0) {
    char c = Serial2.read();
    if (c == '\n') {
      serialData.trim(); // Faltu spaces hatane ke liye
      if (serialData.length() > 0) {
        lastSmokeValue = currentSmokeValue;
        currentSmokeValue = serialData.toInt();
      }
      serialData = "";
    } else if (isDigit(c)) {
      serialData += c;
    }
  }

  // 2. Automatic Status Logic
  String currentStatus = "";
  if(currentSmokeValue < 450) currentStatus = "SAFE";
  else if(currentSmokeValue < 650) currentStatus = "NORMAL";
  else currentStatus = "HIGH ALERT";

  if (currentStatus != lastStatus) {
    if (currentStatus != "SAFE") {
       sendTelegramMsg("Critical Change Detected");
    } else if (lastStatus != "SAFE") {
       bot.sendMessage(CHAT_ID, "✅ Area is now CLEAR/SAFE.", "");
    }
    lastStatus = currentStatus;
  }

  // 3. Telegram Command Check
  if (millis() - lastTimeBotRan > botRequestDelay) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while(numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}
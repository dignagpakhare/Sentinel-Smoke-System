#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); 

int buzzer = PA1;
int smokeA0 = PA0;

unsigned long lastSendTime = 0;
const int sendInterval = 1000; // Har 1 second mein data transmit hoga

void setup() {
  pinMode(buzzer, OUTPUT);
  pinMode(smokeA0, INPUT);
  
  // PA9(TX), PA10(RX) for ESP32
  Serial1.begin(115200); 

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("  SMOKE SENSOR  ");
  lcd.setCursor(0, 1);
  lcd.print("   BOOTING...   ");
  delay(2000);
  lcd.clear();
}

void loop() {
  // Smoothing (Noise filter)
  long sum = 0;
  for(int i = 0; i < 20; i++) {
    sum += analogRead(smokeA0);
    delay(2);
  }
  int analogSensor = sum / 20;

  // ESP32 ko accurate data bhejna
  if (millis() - lastSendTime >= sendInterval) {
    Serial1.print(analogSensor); 
    Serial1.print('\n'); // ESP32 iska intezar karega packet khatam karne ke liye
    lastSendTime = millis();
  }

  // LCD Update
  lcd.setCursor(0, 0);
  lcd.print("Value: ");
  lcd.print(analogSensor);
  lcd.print("     "); 

  lcd.setCursor(0, 1);
  if (analogSensor < 450) {
    lcd.print("Status: SAFE    ");
    noTone(buzzer);
  } else if (analogSensor < 650) {
    lcd.print("Status: NORMAL  ");
    noTone(buzzer);
  } else {
    lcd.print("Status: HIGH !! ");
    tone(buzzer, 1000, 200);
  }
}
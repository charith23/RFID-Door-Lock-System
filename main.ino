#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

#define SS_PIN 10
#define RST_PIN 9
#define BUZZER 2

MFRC522 rfid(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo lockServo;

// Change this UID to your card UID
byte allowedUID[4] = {0x93, 0xA7, 0x45, 0x1C};

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();

  lcd.init();
  lcd.backlight();

  pinMode(BUZZER, OUTPUT);

  lockServo.attach(3);
  lockServo.write(0); // locked position

  lcd.setCursor(0, 0);
  lcd.print("RFID Door Lock");
  lcd.setCursor(0, 1);
  lcd.print("Scan Card...");
}

void loop() {

  if (!rfid.PICC_IsNewCardPresent())
    return;

  if (!rfid.PICC_ReadCardSerial())
    return;

  Serial.print("UID: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  if (checkUID(rfid.uid.uidByte)) {
    accessGranted();
  } else {
    accessDenied();
  }

  rfid.PICC_HaltA();
}

bool checkUID(byte *uid) {
  for (byte i = 0; i < 4; i++) {
    if (uid[i] != allowedUID[i]) {
      return false;
    }
  }
  return true;
}

void accessGranted() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Access Granted");

  digitalWrite(BUZZER, HIGH);
  delay(200);
  digitalWrite(BUZZER, LOW);

  lockServo.write(90); // unlock
  delay(3000);

  lockServo.write(0); // lock back

  lcd.clear();
  lcd.print("Scan Again...");
}

void accessDenied() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Access Denied");

  for (int i = 0; i < 3; i++) {
    digitalWrite(BUZZER, HIGH);
    delay(200);
    digitalWrite(BUZZER, LOW);
    delay(200);
  }

  lcd.clear();
  lcd.print("Try Again...");
}

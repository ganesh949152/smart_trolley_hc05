#include <SoftwareSerial.h>
#include <SPI.h>
#include <MFRC522.h>

// RFID pins
#define SS_PIN 10
#define RST_PIN 9

// Motor pins (L298N)
#define IN1 7
#define IN2 2
#define IN3 3
#define IN4 4

MFRC522 mfrc522(SS_PIN, RST_PIN);

SoftwareSerial BS(5, 6); // RX, TX
int command;
unsigned long lastCommandTime = 0;
const unsigned long commandDelay = 1000; // Delay in milliseconds

void setup() {
  BS.begin(9600);
  BS.println("Bluetooth On please press 1 or 0 blink LED ..");
  Serial.begin(4800);
  Serial.println("Serial");
  SPI.begin();
  mfrc522.PCD_Init();
  pinMode(7, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
}

void loop() {
  moveMotors(false, false, false, false);
  // Check for Bluetooth commands
  if (BS.available()) {
    moveMotors(false, false, false, false);
    command = BS.read();
    Serial.print("Received command: ");
    Serial.println(command);

    moveMotors(false, false, false, false);
    delay(1000);    
    // Process movement commands if sensor is disabled
        if (command == '1' || command == '0' || command == '3' || command == '2') {
            processMovementCommand(command);
            delay(1000);
            moveMotors(false, false, false, false);
            delay(100);
        }
    
    // Process RFID command
    if (command == '4') {
        processRFIDCommand();
        moveMotors(false, false, false, false);
        delay(100);
    }
    if (millis() - lastCommandTime >= commandDelay) {
        while (BS.available()) {
            BS.read(); // Read and discard any data in the buffer
        }
    }

  }
  
  // Clear the serial buffer if the delay has passed
    
    moveMotors(false, false, false, false);
  delay(100);
  moveMotors(false, false, false, false);
}

void processMovementCommand(int command) {
    if (command == '1') {
      Serial.println("Moving forward");
      moveMotors(true, false, true, false);
      delay(1000);
      moveMotors(false, false, false, false);
    } else if (command == '0') {
      Serial.println("Moving backward");
      moveMotors(false, true, false, true);
      delay(1000);
      moveMotors(false, false, false, false);
    } else if (command == '3') {
      Serial.println("Turning left");
      moveMotors(true, true, true, false);
      delay(1000);
      moveMotors(false, false, false, false);
    } else if (command == '2') {
      Serial.println("Turning right");
      moveMotors(true, false, true, true);
      delay(1000);
      moveMotors(false, false, false, false);
    }
}


void processRFIDCommand() {
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
        String tagUID = "";
        for (byte i = 0; i < mfrc522.uid.size; i++) {
            tagUID += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
            tagUID += String(mfrc522.uid.uidByte[i], HEX);
        }
        Serial.print("RFID Tag: ");
        Serial.println(tagUID);

        const char* itemName = getItemName(tagUID);
        if (itemName != nullptr) {
            Serial.print("Item Name: ");
            Serial.println(itemName);
            delay(1000);
            BS.println(itemName);
            delay(800);
        } else {
            Serial.println("Invalid RFID tag");
        }
        mfrc522.PICC_HaltA();
    }
}

const char* getItemName(String rfidHex) {
    if (rfidHex == "f3706629") {
      return "Milk";
    }
    else if(rfidHex == "f7b43202"){
      return "Bread";
    }
    else if(rfidHex == "532cebe4"){
      return "Cereal";
    }
    else if(rfidHex == "e3d2aa28"){
      return "Butter";
    }
    else{
      return nullptr;
    }
}

void moveMotors(bool motor1Forward, bool motor1Backward, bool motor2Forward, bool motor2Backward) {
  digitalWrite(7, motor1Forward ? HIGH : LOW);
  digitalWrite(2, motor1Backward ? HIGH : LOW);
  digitalWrite(3, motor2Forward ? HIGH : LOW);
  digitalWrite(4, motor2Backward ? HIGH : LOW);
}
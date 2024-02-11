#include <SPI.h>
#include <MFRC522.h>

constexpr uint8_t RST_PIN = 5;
constexpr uint8_t SS_PIN = 53;

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

int relayPin = 22;
int relayState = LOW; // Variable to track relay state
bool cardDetected = false;
unsigned long cardDetectionStartTime = 0;
const unsigned long requiredCardDuration = 2000; // Set the required duration for card detection in milliseconds

void setup() {
  Serial.begin(9600); // Initialize serial communications with the PC
  while (!Serial);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, relayState);

  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  Serial.println(F("RFID PUMP SETUP"));
  delay(1000);
  Serial.println(F("SHOW UR TAG"));
}

void loop() {
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) {
    if (cardDetected && (millis() - cardDetectionStartTime >= requiredCardDuration)) {
      relayState = !relayState; // Toggle relay state
      digitalWrite(relayPin, relayState);
      Serial.println(relayState == HIGH ? F(" PUMP ON") : F(" PUMP OFF"));
      cardDetected = false;
    }
    return;
  }

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Reading from the card
  String tag = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    tag.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    tag.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  tag.toUpperCase();
  Serial.println(tag);

  if ((tag.substring(1) == "9D DD EF 37" || tag.substring(1) == "B3 4A 4F 1C") && !cardDetected) {
    cardDetected = true;
    cardDetectionStartTime = millis();
  }
}

#include <ArduinoBLE.h>

BLEService chatService("180C"); 
BLEStringCharacteristic messageChar("2A56", BLERead | BLENotify, 20);

void setup() {
  Serial.begin(115200);
  BLE.begin();

  BLE.setLocalName("BLE_SENDER");
  BLE.setAdvertisedService(chatService);

  chatService.addCharacteristic(messageChar);
  BLE.addService(chatService);

  // Set initial value
  messageChar.writeValue("hi");

  BLE.advertise();
  Serial.println("Sender advertising...");
}

void loop() {
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Connected to: ");
    Serial.println(central.address());

    // Send “hi” every 2 seconds
    while (central.connected()) {
      messageChar.writeValue("hi");
      Serial.println("Sent: hi");
      delay(2000);
    }

    Serial.println("Disconnected!");
  }
}

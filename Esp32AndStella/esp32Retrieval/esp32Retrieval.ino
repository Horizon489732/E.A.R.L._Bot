#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEClient.h>
#include <BLEAddress.h>
#include <BLEAdvertisedDevice.h>

#define SERVICE_UUID        "180C"
#define CHARACTERISTIC_UUID "2A56"

BLEAddress *tagAddress = nullptr;
bool doConnect = false;
bool connected = false;

BLEClient* pClient;
BLERemoteCharacteristic* pRemoteCharacteristic;

// Callback function to handle BLE notifications
void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify
) {
    Serial.print("Data received (");
    Serial.print(length);
    Serial.print(" bytes): ");
    
    if (length == 3) {  // Our packet format: [anchorID, distanceLow, distanceHigh]
        uint8_t anchorID = pData[0];
        uint16_t distance = pData[1] | (pData[2] << 8);
        
        Serial.print("Anchor ");
        Serial.print(anchorID);
        Serial.print(" - Distance: ");
        Serial.print(distance);
        Serial.println(" mm");

                // Send to Arduino
        Serial2.print("Anchor ");
        Serial2.print(anchorID);
        Serial2.print(" - Distance: ");
        Serial2.print(distance);
        Serial2.println(" mm");
    } else {
        Serial.print("Unexpected format - Raw bytes: ");
        for (int i = 0; i < length; i++) {
            Serial.print(pData[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
    }
}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        if (advertisedDevice.haveName() && advertisedDevice.getName() == "BLE_SENDER") {
            Serial.print("Found BLE_SENDER: ");
            Serial.println(advertisedDevice.toString().c_str());
            tagAddress = new BLEAddress(advertisedDevice.getAddress());
            doConnect = true;
            BLEDevice::getScan()->stop();
        }
    }
};

void setup() {
    Serial.begin(115200);
    Serial.println("ESP32 BLE Receiver starting...");
    Serial.println("Waiting for distance data from 3 anchors...");
    
    BLEDevice::init("");
    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    pBLEScan->start(10, false);

    Serial2.begin(9600, SERIAL_8N1, 3, 1);
}

bool connectToTag() {
    if (tagAddress == nullptr) return false;
    
    pClient = BLEDevice::createClient();
    Serial.println("Connecting to BLE_SENDER...");
    if (!pClient->connect(*tagAddress)) {
        Serial.println("Failed to connect!");
        return false;
    }
    Serial.println("Connected to BLE_SENDER.");
    
    BLERemoteService* pRemoteService = pClient->getService(BLEUUID(SERVICE_UUID));
    if (pRemoteService == nullptr) {
        Serial.println("Failed to find service.");
        pClient->disconnect();
        return false;
    }
    
    pRemoteCharacteristic = pRemoteService->getCharacteristic(BLEUUID(CHARACTERISTIC_UUID));
    if (pRemoteCharacteristic == nullptr) {
        Serial.println("Failed to find characteristic.");
        pClient->disconnect();
        return false;
    }
    
    if(pRemoteCharacteristic->canNotify()) {
        pRemoteCharacteristic->registerForNotify(notifyCallback);
        Serial.println("Registered for notifications - waiting for distance data...");
    }
    
    connected = true;
    return true;
}

void loop() {
    if (!connected && doConnect) {
        doConnect = false;
        if (connectToTag()) {
            Serial.println("BLE notifications enabled.");
        }
    }
    
    if (connected && !pClient->isConnected()) {
        Serial.println("Disconnected! Restarting scan...");
        connected = false;
        delete tagAddress;
        tagAddress = nullptr;
        BLEDevice::getScan()->start(10, false);
    }
    
    delay(1000);
}
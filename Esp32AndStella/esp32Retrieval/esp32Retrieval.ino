#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEClient.h>
#include <BLEAddress.h>
#include <BLEAdvertisedDevice.h>

// UUIDs matching your Arduino sender
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
    Serial.print("Received: ");
    for(int i = 0; i < length; i++) {
        Serial.print((char)pData[i]);
    }
    Serial.println();
}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        // Look for device with name "BLE_SENDER"
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
    Serial.println("ESP32 BLE Central starting...");
    
    BLEDevice::init("");
    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    pBLEScan->start(10, false); // scan for 10 seconds
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
        Serial.println("Registered for notifications");
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
    
    // If disconnected, restart scan
    if (connected && !pClient->isConnected()) {
        Serial.println("Disconnected! Restarting scan...");
        connected = false;
        delete tagAddress;
        tagAddress = nullptr;
        BLEDevice::getScan()->start(10, false);
    }
    
    delay(1000);
}
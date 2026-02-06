#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEClient.h>

#define SERVICE_UUID        "180C"
#define CHARACTERISTIC_UUID "2A56"

BLEAddress* tagAddress = nullptr;
bool doConnect = false;
bool connected = false;
BLEClient* pClient = nullptr;

// ===== Disconnect Handler =====
class MyClientCallback : public BLEClientCallbacks {
    void onConnect(BLEClient* pclient) {}
    void onDisconnect(BLEClient* pclient) {
        connected = false;
        Serial.println("Link Lost! Ready to re-scan...");
    }
};

// ===== Notification Handler =====
void notifyCallback(BLERemoteCharacteristic* pChar,
                    uint8_t* pData,
                    size_t length,
                    bool isNotify) {

    if (length != 6) return;

    uint16_t a1 = pData[0] | (pData[1] << 8);
    uint16_t a2 = pData[2] | (pData[3] << 8);
    uint16_t a3 = pData[4] | (pData[5] << 8);

    // ----- DEBUG (USB Serial) -----
    Serial.printf("Anchor 1 - Distance: %d mm\n", a1);
    Serial.printf("Anchor 2 - Distance: %d mm\n", a2);
    // Serial.printf("Anchor 3 - Distance: %d mm\n", a3);

    // ----- SEND TO MOTOR ARDUINO -----
    Serial2.printf("Anchor 1 - Distance: %d mm\n", a1);
    Serial2.printf("Anchor 2 - Distance: %d mm\n", a2);
    // Serial2.printf("Anchor 3 - Distance: %d mm\n", a3);
}


// ===== Scan Callback =====
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        if (advertisedDevice.getName() == "BLE_SENDER") {
            Serial.println("Stella Found!");
            if (tagAddress != nullptr) delete tagAddress;
            tagAddress = new BLEAddress(advertisedDevice.getAddress());
            doConnect = true;
            BLEDevice::getScan()->stop(); // Stop scanning to connect
        }
    }
};

bool connectToTag() {
    Serial.println("Attempting connection...");
    pClient = BLEDevice::createClient();
    pClient->setClientCallbacks(new MyClientCallback());

    if (!pClient->connect(*tagAddress)) return false;

    BLERemoteService* pRemoteService = pClient->getService(BLEUUID(SERVICE_UUID));
    if (pRemoteService == nullptr) {
        pClient->disconnect();
        return false;
    }

    BLERemoteCharacteristic* pRemoteChar = pRemoteService->getCharacteristic(BLEUUID(CHARACTERISTIC_UUID));
    if (pRemoteChar == nullptr || !pRemoteChar->canNotify()) {
        pClient->disconnect();
        return false;
    }

    pRemoteChar->registerForNotify(notifyCallback);
    connected = true;
    return true;
}

void setup() {
    Serial.begin(115200);
    Serial2.begin(9600, SERIAL_8N1, 16, 17); // Adjust RX/TX pins for your specific ESP32
    
    BLEDevice::init("ESP32_RECEIVER");
    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    Serial.println("Receiver Started. Scanning...");
}

void loop() {
    if (doConnect) {
        doConnect = false;
        if (connectToTag()) {
            Serial.println("Connected Successfully!");
        } else {
            Serial.println("Connection Failed.");
        }
    }

    if (!connected) {
        // This ensures the ESP32 keeps looking whenever it is offline
        Serial.println("The connection is disconnected");
        BLEDevice::getScan()->start(2, false); 
    }

    delay(500);
}
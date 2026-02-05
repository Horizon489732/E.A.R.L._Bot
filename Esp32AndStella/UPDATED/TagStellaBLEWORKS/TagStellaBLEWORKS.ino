#include "StellaUWB.h"
#include <ArduinoBLE.h>

// ===== BLE Setup =====
BLEService distanceService("180C"); 
BLECharacteristic distanceChar("2A56", BLERead | BLENotify, 6);
volatile bool bleConnected = false; //is BLE central connected? true or false
volatile bool newDistanceAvailable = false; // flags new data available to send
uint8_t pendingPacket[6] = {0}; // packet to send to ESP32


uint8_t tagAddr[]     = {0x11, 0x11};
uint8_t anchor1Addr[] = {0x22, 0x22};
uint8_t anchor2Addr[] = {0x33, 0x33};
uint8_t anchor3Addr[] = {0x44, 0x44};

volatile uint16_t distances[3] = {0,0,0};

void rangingHandler(UWBRangingData &rangingData) { // callback that is called whenever UWB data available
    if (rangingData.measureType() != (uint8_t)uwb::MeasurementType::TWO_WAY) return;
    RangingMeasures twr = rangingData.twoWayRangingMeasure();

    bool updated = false;

    for (int i = 0; i < rangingData.available(); i++) { // loops through each anchor's measurements
        if (twr[i].status == 0 && twr[i].distance > 0) {
            uint16_t distanceMm = twr[i].distance;

            // Only update if distance changed
            if (distanceMm != distances[i]) {
                distances[i] = distanceMm;
                updated = true;

                // Serial.print("UWB RX | Anchor ");
                // Serial.print(i + 1);
                // Serial.print(" Distance: ");
                // Serial.print(distanceMm);
                // Serial.println(" mm");


                // // Store packet for BLE sending (first anchor only)
                // pendingPacket[0] = i + 1;               // Anchor number
                // pendingPacket[1] = distanceMm & 0xFF;  // low byte
                // pendingPacket[2] = (distanceMm >> 8) & 0xFF; // high byte
                if (updated){
                  newDistanceAvailable = true; // mark ready to send
                }
         
            }
        }
    }
}


void setup() {
    Serial.begin(115200);
    
    if (!BLE.begin()) {
        while (1);
    }

    BLE.setLocalName("BLE_SENDER");
    distanceService.addCharacteristic(distanceChar);
    BLE.addService(distanceService);
    BLE.setConnectionInterval(16, 32); // Increased for stability
    BLE.advertise();

    // UWB Init
    UWBMacAddress tagMAC(UWBMacAddress::Size::SHORT, tagAddr);
    UWBMacAddress anchor1(UWBMacAddress::Size::SHORT, anchor1Addr);
    UWBMacAddress anchor2(UWBMacAddress::Size::SHORT, anchor2Addr);
    UWBMacAddress anchor3(UWBMacAddress::Size::SHORT, anchor3Addr);

    UWBMacAddressList anchors(UWBMacAddress::Size::SHORT);
    anchors.add(anchor1); anchors.add(anchor2); anchors.add(anchor3);

    UWB.registerRangingCallback(rangingHandler);
    UWB.begin();
    while (UWB.state() != 0) delay(10);

    UWBRangingOneToMany tagSession(0x11223344, tagMAC, anchors);
    UWBSessionManager.addSession(tagSession);
    tagSession.init();
    tagSession.start();
    
    Serial.println("Stella UWB Sender Ready...");
}

void loop() {
    BLE.poll();  // ALWAYS  ; keeps BLE stack running and alive

    static unsigned long lastStatusPrint = 0; // timer for periodic status print

    BLEDevice central = BLE.central(); // check for connection
    bool connectedNow = central && central.connected();

    // Continuous status print
    if (millis() - lastStatusPrint > 1000) {
        if (connectedNow) {
            Serial.print("ESP32 CONNECTED: ");
            Serial.println(central.address());
        } else {
            Serial.println("ESP32 NOT CONNECTED");
        }
        lastStatusPrint = millis();
    }

    // detects connect/ disconnect events
    if (connectedNow && !bleConnected) {
        Serial.println(">>> CONNECT EVENT");
    }

    if (!connectedNow && bleConnected) {
        Serial.println(">>> DISCONNECT EVENT, restarting advertising");
        BLE.advertise(); //if disconnect, restart advertising
    }

    bleConnected = connectedNow;

    // send BLE data only if central is connected and if there is new UWB data
   if (connectedNow && newDistanceAvailable) {
        for (int i = 0; i < 3; i++) {
        pendingPacket[i * 2]     = distances[i] & 0xFF;
        pendingPacket[i * 2 + 1] = (distances[i] >> 8) & 0xFF;
    }
        Serial.print("BLE TX | A1=");
        Serial.print(distances[0]);
        Serial.print(" A2=");
        Serial.print(distances[1]);
        Serial.print(" A3=");
        Serial.println(distances[2]);

        distanceChar.writeValue(pendingPacket, 6); // send the 6-byte packet
        newDistanceAvailable = false;

    }

    delay(20);
}

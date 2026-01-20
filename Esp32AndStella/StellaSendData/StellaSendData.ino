#include "StellaUWB.h"
#include <ArduinoBLE.h>

// BLE Service and Characteristic (increased size to handle multiple distances)
BLEService distanceService("180C"); 
BLECharacteristic distanceChar("2A56", BLERead | BLENotify, 20);  // 20 bytes to send multiple values

// MAC addresses for anchors (fixed) and tag (this device)
uint8_t tagAddr[]     = {0x11, 0x11};
uint8_t anchor1Addr[] = {0x22, 0x22};
uint8_t anchor2Addr[] = {0x33, 0x33};
uint8_t anchor3Addr[] = {0x44, 0x44};

// Handler for ranging notifications
void rangingHandler(UWBRangingData &rangingData) {
    if (rangingData.measureType() != (uint8_t)uwb::MeasurementType::TWO_WAY) return;
    RangingMeasures twr = rangingData.twoWayRangingMeasure();
    
    for (int i = 0; i < rangingData.available(); i++) {
        if (twr[i].status == 0 && twr[i].distance != 0xFFFF) {
            Serial.print("Distance to anchor ");
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(twr[i].distance);
            Serial.println(" mm");
            
            // Keep distance in mm
            uint16_t distanceMm = twr[i].distance;
            
            // Create packet: [anchorID, distanceLow, distanceHigh]
            // anchorID = 1, 2, or 3
            // distance split into 2 bytes (allows up to 65535 mm = 65.5 meters)
            uint8_t packet[3];
            packet[0] = i + 1;  // Anchor ID (1, 2, or 3)
            packet[1] = distanceMm & 0xFF;  // Low byte
            packet[2] = (distanceMm >> 8) & 0xFF;  // High byte
            
            distanceChar.writeValue(packet, 3);
            
            Serial.print("Sent via BLE - Anchor ");
            Serial.print(packet[0]);
            Serial.print(": ");
            Serial.print(distanceMm);
            Serial.println(" mm");
        }
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    // Initialize BLE
    if (!BLE.begin()) {
        Serial.println("Starting BLE failed!");
        while (1);
    }
    
    BLE.setLocalName("BLE_SENDER");
    BLE.setAdvertisedService(distanceService);
    distanceService.addCharacteristic(distanceChar);
    BLE.addService(distanceService);
    
    uint8_t initialData[3] = {0, 0, 0};
    distanceChar.writeValue(initialData, 3);
    BLE.advertise();
    
    Serial.println("BLE advertising started");
    
    // Define MAC addresses for tag and anchors
    UWBMacAddress tagMAC(UWBMacAddress::Size::SHORT, tagAddr);
    UWBMacAddress anchor1(UWBMacAddress::Size::SHORT, anchor1Addr);
    UWBMacAddress anchor2(UWBMacAddress::Size::SHORT, anchor2Addr);
    UWBMacAddress anchor3(UWBMacAddress::Size::SHORT, anchor3Addr);
    
    // Create a list of destination addresses
    UWBMacAddressList anchors(UWBMacAddress::Size::SHORT);
    anchors.add(anchor1);
    anchors.add(anchor2);
    anchors.add(anchor3);
    
    // Register callback to receive distances
    UWB.registerRangingCallback(rangingHandler);
    
    // Initialize UWB
    UWB.begin();
    while (UWB.state() != 0) delay(10);
    
    Serial.println("Starting UWB controller session...");
    
    // Create the one-to-many controller (tag) session
    UWBRangingOneToMany tagSession(0x11223344, tagMAC, anchors);
    
    // Add the session to session manager
    UWBSessionManager.addSession(tagSession);
    
    // Initialize and start the session
    tagSession.init();
    tagSession.start();
    
    Serial.println("Tag session started. Measuring distances to 3 anchors...");
}

void loop() {
    // Poll BLE to keep connection alive
    BLE.poll();
    delay(100);
}
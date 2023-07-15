// Copyright (c) 2023 Rafael Farias
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "PIDestalRemoteBLE.h"

namespace PID_BLE {

BLEService pidService("3e60a07c-235e-11ee-be56-0242ac120002");
BLEStringCharacteristic pidConstsCharacteristic("13078cd8-235e-11ee-be56-0242ac120002", BLERead | BLEWrite, 512);

}  // namespace PID_BLE

String PID_BLE::pidToString(PID pid) {
    return "123,123,123";
}

PIDestalRemoteBLE::PIDestalRemoteBLE(PIDestal& _pidPtr, String deviceName) {
    pidPtr = &_pidPtr;
}

void PIDestalRemoteBLE::initialize() {
#ifndef DO_NOT_USE_BLUETOOTH
    if (!BLE.begin()) {
        Serial.println("starting Bluetooth® Low Energy module failed!");

        while (1)
            delay(200);
    }

    BLE.setLocalName(BLE_DEVICE_NAME);

    BLE.setAdvertisedService(PID_BLE::pidService);
    PID_BLE::pidService.addCharacteristic(PID_BLE::pidConstsCharacteristic);

    BLE.addService(PID_BLE::pidService);

    PID_BLE::pidConstsCharacteristic.writeValue(PID_BLE::pidToString(pidPtr->getPidConsts()));

    BLE.advertise();
    Serial.println("Bluetooth® device active, waiting for connections...");
#endif  // DO_NOT_USE_BLUETOOTH
}

void PIDestalRemoteBLE::process() {
#ifndef DO_NOT_USE_BLUETOOTH
    BLE.poll();

    String receivedValue = PID_BLE::pidConstsCharacteristic.value();

    Serial.println(receivedValue);
#endif  // DO_NOT_USE_BLUETOOTH
}
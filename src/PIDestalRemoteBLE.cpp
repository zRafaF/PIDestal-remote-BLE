// Copyright (c) 2023 Rafael Farias
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "PIDestalRemoteBLE.h"

namespace PID_BLE {

BLEStringCharacteristic pidConstsCharacteristic("13078cd8-235e-11ee-be56-0242ac120002", BLERead | BLEWrite | BLENotify, 512);

String pidToString(PID pid) {
    return (
        String(pid.p, 4) +
        "," +
        String(pid.i, 4) +
        "," +
        String(pid.d, 4));
}
}  // namespace PID_BLE

PIDestalRemoteBLE::PIDestalRemoteBLE(
    PIDestal& _pidPtr,
    const char* deviceUUID) : pidService(deviceUUID) {
    pidPtr = &_pidPtr;
}

void PIDestalRemoteBLE::initialize(const char* deviceName) {
#ifndef DO_NOT_USE_BLUETOOTH
    if (!BLE.begin()) {
        Serial.println("starting Bluetooth® Low Energy module failed!");

        while (1)
            delay(200);
    }

    BLE.setLocalName(deviceName);

    BLE.setAdvertisedService(pidService);
    pidService.addCharacteristic(PID_BLE::pidConstsCharacteristic);

    BLE.addService(pidService);

    PID_BLE::pidConstsCharacteristic.writeValue(getFormattedPackage());

    BLE.advertise();
    Serial.println("Bluetooth® device active, waiting for connections...");
#endif  // DO_NOT_USE_BLUETOOTH
}

void PIDestalRemoteBLE::process() {
#ifndef DO_NOT_USE_BLUETOOTH
    BLE.poll();

    PID_BLE::pidConstsCharacteristic.writeValue(getFormattedPackage());

    String receivedValue = PID_BLE::pidConstsCharacteristic.value();

    Serial.println(receivedValue);
#endif  // DO_NOT_USE_BLUETOOTH
}

String PIDestalRemoteBLE::getFormattedPackage() {
    return (
        PID_BLE::pidToString(
            pidPtr->getPidConsts()) +
        "," +
        extraInfo);
}
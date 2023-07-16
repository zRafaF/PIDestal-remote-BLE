// Copyright (c) 2023 Rafael Farias
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "PIDestalRemoteBLE.h"

namespace PID_BLE {

BLEStringCharacteristic getCharacteristic("0x4e", BLERead | BLENotify, 512);  // Generic User Property Status

BLEStringCharacteristic setCharacteristic("0x4c", BLEWrite, 512);  // Generic User Property Set
}  // namespace PID_BLE

PIDestalRemoteBLE::PIDestalRemoteBLE(
    PIDestal& _pidPtr,
    const char* deviceUUID) : pidService(deviceUUID) {
    pidPtr = &_pidPtr;
    setExtraInfo("");
}

void PIDestalRemoteBLE::initialize(
    const char* deviceName,
    char myPassword[PASSWORD_ARRAY_SIZE]) {
#ifndef DO_NOT_USE_BLUETOOTH
    if (!BLE.begin()) {
        Serial.println("starting Bluetooth® Low Energy module failed!");
        while (1) {
            Serial.println("Unable to connect");
            delay(200);
        }
    }
    if (strlen(myPassword) != PASSWORD_ARRAY_SIZE) {
        while (1) {
            Serial.println("INVALID PASSWORD");
            delay(200);
        }
    }

    BLE.setLocalName(deviceName);

    BLE.setAdvertisedService(pidService);
    pidService.addCharacteristic(PID_BLE::getCharacteristic);
    pidService.addCharacteristic(PID_BLE::setCharacteristic);

    BLE.addService(pidService);

    PID_BLE::getCharacteristic.writeValue(getFormattedPackage());
    PID_BLE::setCharacteristic.writeValue("");
    BLE.advertise();
    Serial.println("Bluetooth® device active, waiting for connections...");
#endif  // DO_NOT_USE_BLUETOOTH
}

void PIDestalRemoteBLE::process() {
#ifndef DO_NOT_USE_BLUETOOTH
    BLE.poll();

    PID_BLE::getCharacteristic.writeValue(getFormattedPackage());

    String receivedValue = PID_BLE::getCharacteristic.value();
    if (lastReceivedValue != receivedValue) {
        Serial.println(receivedValue.length());
    }

    Serial.println(receivedValue);
#endif  // DO_NOT_USE_BLUETOOTH
}

String PIDestalRemoteBLE::getFormattedPackage() {
    return (
        pidToString(
            pidPtr->getPidConsts()) +
        "," +
        extraInfo);
}

String PIDestalRemoteBLE::pidToString(PID pid) {
    return (
        String(pid.p, 4) +
        "," +
        String(pid.i, 4) +
        "," +
        String(pid.d, 4));
}

PID_BLE::ResponseBLE PIDestalRemoteBLE::decodeReceived(String received) {
    PID_BLE::ResponseBLE decoded = {0.0f, 0.0f, 0.0f, "", false};
    if (received.length() < MIN_RECEIVED_SIZE) return decoded;

    return decoded;
}
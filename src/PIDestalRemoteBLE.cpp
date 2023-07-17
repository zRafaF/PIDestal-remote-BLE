// Copyright (c) 2023 Rafael Farias
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "PIDestalRemoteBLE.h"

PIDestalRemoteBLE::PIDestalRemoteBLE(PIDestal& _pidPtr) : pidService("3e60a07c-235e-11ee-be56-0242ac120002"),
                                                          pGetCharacteristic("a5831824-2445-11ee-be56-0242ac120002", BLERead | BLENotify),
                                                          iGetCharacteristic("a5831c2a-2445-11ee-be56-0242ac120002", BLERead | BLENotify),
                                                          dGetCharacteristic("a58322c4-2445-11ee-be56-0242ac120002", BLERead | BLENotify),
                                                          extraGetCharacteristic("a5832454-2445-11ee-be56-0242ac120002", BLERead | BLENotify, EXTRA_INFO_ARRAY_SIZE + PASSWORD_ARRAY_SIZE),
                                                          pSetCharacteristic("a5832594-2445-11ee-be56-0242ac120002", BLEWrite, 32),
                                                          iSetCharacteristic("a58326e8-2445-11ee-be56-0242ac120002", BLEWrite, 32),
                                                          dSetCharacteristic("a58328a0-2445-11ee-be56-0242ac120002", BLEWrite, 32),
                                                          extraSetCharacteristic("a5832a62-2445-11ee-be56-0242ac120002", BLEWrite, EXTRA_INFO_ARRAY_SIZE + PASSWORD_ARRAY_SIZE) {
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
    strcpy(password, myPassword);

    BLE.setLocalName(deviceName);

    BLE.setAdvertisedService(pidService);

    pidService.addCharacteristic(pGetCharacteristic);
    pidService.addCharacteristic(iGetCharacteristic);
    pidService.addCharacteristic(dGetCharacteristic);
    pidService.addCharacteristic(extraGetCharacteristic);
    pidService.addCharacteristic(pSetCharacteristic);
    pidService.addCharacteristic(iSetCharacteristic);
    pidService.addCharacteristic(dSetCharacteristic);
    pidService.addCharacteristic(extraSetCharacteristic);

    BLE.addService(pidService);

    lastPID = pidPtr->getPidConsts();
    lastExtra = getExtraInfo();

    pGetCharacteristic.writeValue(lastPID.p);
    iGetCharacteristic.writeValue(lastPID.i);
    dGetCharacteristic.writeValue(lastPID.d);
    extraGetCharacteristic.writeValue(lastExtra);

    pSetCharacteristic.writeValue("");
    iSetCharacteristic.writeValue("");
    dSetCharacteristic.writeValue("");
    extraSetCharacteristic.writeValue("");
    BLE.advertise();
    Serial.println("Bluetooth® device active, waiting for connections...");
#endif  // DO_NOT_USE_BLUETOOTH
}

void PIDestalRemoteBLE::process() {
#ifndef DO_NOT_USE_BLUETOOTH
    BLE.poll();

    // Updating the getters if something changed
    if (lastPID != pidPtr->getPidConsts()) {
        lastPID = pidPtr->getPidConsts();
        pGetCharacteristic.writeValue(lastPID.p);
        iGetCharacteristic.writeValue(lastPID.i);
        dGetCharacteristic.writeValue(lastPID.d);
    }
    if (lastExtra != getExtraInfo()) {
        lastExtra = getExtraInfo();
        extraGetCharacteristic.writeValue(lastExtra);
    }

    // The received values
    String receivedP = pSetCharacteristic.value();
    String receivedI = iSetCharacteristic.value();
    String receivedD = dSetCharacteristic.value();
    String receivedExtra = extraSetCharacteristic.value();

    Serial.println(receivedP);

    Serial.println(checkValidPassword(receivedP));

#endif  // DO_NOT_USE_BLUETOOTH
}

bool PIDestalRemoteBLE::checkValidPassword(String buffer) {
    Serial.print("myPass: ");
    Serial.println(password);
    Serial.print("ReceivedPass: ");
    Serial.println(buffer);
    if (buffer.length() < 6) return false;
    String receivedPassword = buffer.substring(0, 6);

    return receivedPassword == password;
}

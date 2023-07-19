// Copyright (c) 2023 Rafael Farias
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "PIDestalRemoteBLE.h"

PIDestalRemoteBLE::PIDestalRemoteBLE(
    PIDestal* _pidPtr) : pidService(DEFAULT_SERVICE_UUID),
                         pGetCharacteristic("a5831824-2445-11ee-be56-0242ac120002", BLERead | BLENotify, 32),
                         iGetCharacteristic("a5831c2a-2445-11ee-be56-0242ac120002", BLERead | BLENotify, 32),
                         dGetCharacteristic("665e5ab6-24f7-11ee-be56-0242ac120002", BLERead | BLENotify, 32),
                         extraGetCharacteristic("a5832454-2445-11ee-be56-0242ac120002", BLERead | BLENotify, EXTRA_INFO_ARRAY_SIZE + PASSWORD_ARRAY_SIZE),
                         pSetCharacteristic("a5832594-2445-11ee-be56-0242ac120002", BLEWrite, 32),
                         iSetCharacteristic("a58326e8-2445-11ee-be56-0242ac120002", BLEWrite, 32),
                         dSetCharacteristic("ebf99fc0-24f5-11ee-be56-0242ac120002", BLEWrite, 32),
                         extraSetCharacteristic("a5832a62-2445-11ee-be56-0242ac120002", BLEWrite, EXTRA_INFO_ARRAY_SIZE + PASSWORD_ARRAY_SIZE),
                         ptrArray(_pidPtr ? &_pidPtr : nullptr),
                         ptrArraySize(_pidPtr ? 1 : 0) {
    setExtraInfo("");
}

PIDestalRemoteBLE::PIDestalRemoteBLE(
    PIDestal* _pidArrayPtr[],
    int arraySize = 1) : pidService(DEFAULT_SERVICE_UUID),
                         pGetCharacteristic("a5831824-2445-11ee-be56-0242ac120002", BLERead | BLENotify, 32),
                         iGetCharacteristic("a5831c2a-2445-11ee-be56-0242ac120002", BLERead | BLENotify, 32),
                         dGetCharacteristic("665e5ab6-24f7-11ee-be56-0242ac120002", BLERead | BLENotify, 32),
                         extraGetCharacteristic("a5832454-2445-11ee-be56-0242ac120002", BLERead | BLENotify, EXTRA_INFO_ARRAY_SIZE + PASSWORD_ARRAY_SIZE),
                         pSetCharacteristic("a5832594-2445-11ee-be56-0242ac120002", BLEWrite, 32),
                         iSetCharacteristic("a58326e8-2445-11ee-be56-0242ac120002", BLEWrite, 32),
                         dSetCharacteristic("ebf99fc0-24f5-11ee-be56-0242ac120002", BLEWrite, 32),
                         extraSetCharacteristic("a5832a62-2445-11ee-be56-0242ac120002", BLEWrite, EXTRA_INFO_ARRAY_SIZE + PASSWORD_ARRAY_SIZE),
                         ptrArray(_pidArrayPtr),
                         ptrArraySize(arraySize) {
    setExtraInfo("");
}

void PIDestalRemoteBLE::initialize(
    const char* deviceName,
    const char* myPassword) {
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

    lastPID = getFirstPidConsts();
    lastExtra = getExtraInfo();

    pGetCharacteristic.writeValue(String(lastPID.p, 8));
    iGetCharacteristic.writeValue(String(lastPID.i, 8));
    dGetCharacteristic.writeValue(String(lastPID.d, 8));
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
    updateGetters();
    processReceivedData();

#endif  // DO_NOT_USE_BLUETOOTH
}

void PIDestalRemoteBLE::updateGetters() {
    // Updating the getters if something changed
    if (lastPID != getFirstPidConsts()) {
        lastPID = getFirstPidConsts();
        pGetCharacteristic.writeValue(String(lastPID.p, 8));
        iGetCharacteristic.writeValue(String(lastPID.i, 8));
        dGetCharacteristic.writeValue(String(lastPID.d, 8));
    }
    if (lastExtra != getExtraInfo()) {
        lastExtra = getExtraInfo();
        extraGetCharacteristic.writeValue(lastExtra);
    }
}

void PIDestalRemoteBLE::processReceivedData() {
    String receivedP = pSetCharacteristic.value();
    String receivedI = iSetCharacteristic.value();
    String receivedD = dSetCharacteristic.value();
    String receivedExtra = extraSetCharacteristic.value();

    PID newPID = lastPID;

    if (lastPID.p != receivedP && checkValidPassword(receivedP)) {
        newPID.p = extractStringFromData(receivedP).toFloat();
        lastReceivedP = receivedP;
    }

    if (lastPID.i != receivedI && checkValidPassword(receivedI)) {
        newPID.i = extractStringFromData(receivedI).toFloat();
        lastReceivedI = receivedI;
    }

    if (lastPID.d != receivedD && checkValidPassword(receivedD)) {
        newPID.d = extractStringFromData(receivedD).toFloat();
        lastReceivedD = receivedD;
    }

    if (getExtraInfo() != receivedExtra && checkValidPassword(receivedExtra)) {
        setExtraInfo(extractStringFromData(receivedExtra));
        lastReceivedExtra = receivedExtra;
        Serial.print(receivedExtra);
    }

    if (newPID != lastPID)
        setPidArrayConsts(newPID);
}

bool PIDestalRemoteBLE::checkValidPassword(String buffer) {
    if (buffer.length() < 6) return false;
    String receivedPassword = buffer.substring(0, 6);

    return receivedPassword == password;
}

String PIDestalRemoteBLE::extractStringFromData(String buffer) {
    return buffer.substring(6, buffer.length());
}

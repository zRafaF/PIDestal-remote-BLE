// Copyright (c) 2023 Rafael Farias
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "PIDestalRemoteBLE.h"

PIDestalRemoteBLE::PIDestalRemoteBLE() : pidService(DEFAULT_SERVICE_UUID),
                                         pidGetCharacteristic("a5831824-2445-11ee-be56-0242ac120002", BLERead | BLENotify, 256),
                                         extraGetCharacteristic("a5832454-2445-11ee-be56-0242ac120002", BLERead | BLENotify, EXTRA_INFO_ARRAY_SIZE + PASSWORD_ARRAY_SIZE),
                                         pidSetCharacteristic("a5832594-2445-11ee-be56-0242ac120002", BLEWrite, 256),
                                         extraSetCharacteristic("a5832a62-2445-11ee-be56-0242ac120002", BLEWrite, EXTRA_INFO_ARRAY_SIZE + PASSWORD_ARRAY_SIZE),
                                         callbackIdxSetCharacteristic("ffa97c40-2764-11ee-be56-0242ac120002", BLEWrite, 8),
                                         pidPtrArray(NULL),
                                         pidPtrArraySize(0) {}

PIDestalRemoteBLE::PIDestalRemoteBLE(PIDestal* _pidPtr) : PIDestalRemoteBLE() {
    setPidPtrArray(_pidPtr);
}

PIDestalRemoteBLE::PIDestalRemoteBLE(PIDestal* _pidArrayPtr[], size_t arraySize) : PIDestalRemoteBLE() {
    setPidPtrArray(_pidArrayPtr, arraySize);
}

PIDestalRemoteBLE::~PIDestalRemoteBLE() {
    deleteOldPidArray();
}

void PIDestalRemoteBLE::setPidPtrArray(PIDestal* _pidPtr) {
    deleteOldPidArray();
    pidPtrArray = new PIDestal*[1];
    pidPtrArraySize = 1;
    pidPtrArray[0] = _pidPtr;
    needsToDeleteArray = true;
}

void PIDestalRemoteBLE::setPidPtrArray(PIDestal* _pidArrayPtr[], size_t arraySize) {
    deleteOldPidArray();
    pidPtrArray = _pidArrayPtr;
    pidPtrArraySize = arraySize;
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

    pidService.addCharacteristic(pidGetCharacteristic);
    pidService.addCharacteristic(extraGetCharacteristic);
    pidService.addCharacteristic(pidSetCharacteristic);
    pidService.addCharacteristic(extraSetCharacteristic);
    pidService.addCharacteristic(callbackIdxSetCharacteristic);

    BLE.addService(pidService);

    lastExtra = getExtraInfo();

    pidGetCharacteristic.writeValue(String(""));
    extraGetCharacteristic.writeValue(lastExtra);

    pidSetCharacteristic.writeValue("");
    extraSetCharacteristic.writeValue("");

    callbackIdxSetCharacteristic.writeValue(String(-1));

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
    bool needsToUpdatePID = false;
    for (size_t i = 0; i < pidPtrArraySize; i++) {
        const PID currentPID = pidPtrArray[i]->getPidConsts();
        if (lastPID[i] != currentPID) {
            needsToUpdatePID = true;
        }
        lastPID[i] = currentPID;
    }
    // Updating the getters if something changed
    if (needsToUpdatePID) {
        // Creates an object "pid"
        JsonArray pid = getPidDoc.createNestedArray("pid");
        for (size_t i = 0; i < pidPtrArraySize; i++) {
            // Creates a nested array in pid
            JsonArray pid_array = pid.createNestedArray();
            pid_array.add(lastPID[i].p);
            pid_array.add(lastPID[i].i);
            pid_array.add(lastPID[i].d);
        }
        String serializedString;
        serializeJson(getPidDoc, serializedString);
        pidGetCharacteristic.writeValue(serializedString);
    }
    if (lastExtra != getExtraInfo()) {
        lastExtra = getExtraInfo();
        extraGetCharacteristic.writeValue(lastExtra);
    }
}

void PIDestalRemoteBLE::processReceivedData() {
    String receivedPID = pidSetCharacteristic.value();
    String receivedExtra = extraSetCharacteristic.value();
    String receivedCallbackIdx = callbackIdxSetCharacteristic.value();

    if (lastReceivedPID != receivedPID && checkValidPassword(receivedPID)) {
        lastReceivedPID = receivedPID;
        updatePidArrayConsts(extractStringFromData(receivedPID));
    }

    if (lastReceivedExtra != receivedExtra && checkValidPassword(receivedExtra)) {
        setExtraInfo(extractStringFromData(receivedExtra));
        lastReceivedExtra = receivedExtra;
    }

    if (receivedCallbackIdx.length() >= PASSWORD_ARRAY_SIZE && checkValidPassword(receivedCallbackIdx)) {
        int callbackIdx = extractStringFromData(receivedCallbackIdx).toInt();
        if (callbackIdx >= 0 && callbackIdx < numOfStoredFunctions) {
            runFunctionByIndex(callbackIdx);
        }
        callbackIdxSetCharacteristic.writeValue(String(-1));
    }
}

bool PIDestalRemoteBLE::checkValidPassword(String buffer) {
    if (buffer.length() < 6) return false;
    String receivedPassword = buffer.substring(0, 6);

    return receivedPassword == password;
}

String PIDestalRemoteBLE::extractStringFromData(String buffer) {
    return buffer.substring(6, buffer.length());
}

void PIDestalRemoteBLE::updatePidArrayConsts(String receivedString) {
    DeserializationError error = deserializeJson(setPidDoc, receivedString);
    if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
    }

    JsonArray pidArray = setPidDoc["pid"];

    for (size_t i = 0; i < pidPtrArraySize; i++) {
        JsonArray pid = pidArray[i];

        PID newPID = {
            pid[0],
            pid[1],
            pid[2],
        };

        pidPtrArray[i]->setPidConsts(newPID);  // Accessing the x variable of each PIDestal object
    }
    pidSetCharacteristic.writeValue(receivedString);
}

void PIDestalRemoteBLE::setCallbackFunctions(FunctionPointer funcs[], size_t arraySize) {
    for (size_t i = 0; i < arraySize; ++i) {
        callbackFunctions[i] = funcs[i];
    }
    numOfStoredFunctions = arraySize;
}

void PIDestalRemoteBLE::setCallbackFunctions(FunctionPointer func) {
    callbackFunctions[0] = func;
    numOfStoredFunctions = 1;
}

void PIDestalRemoteBLE::runFunctionByIndex(size_t index) {
    if (index < numOfStoredFunctions && callbackFunctions[index]) {
        callbackFunctions[index]();
    } else {
        Serial.println("Invalid function index!");
    }
}

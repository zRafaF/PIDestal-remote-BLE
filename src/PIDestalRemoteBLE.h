// Copyright (c) 2023 Rafael Farias
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef PIDESTAL_REMOTE_BLE_H
#define PIDESTAL_REMOTE_BLE_H

#include <ArduinoBLE.h>
#include <PIDestal.h>
#include <arduino.h>

#define EXTRA_INFO_ARRAY_SIZE 64

#define PASSWORD_ARRAY_SIZE 6

#define DEFAULT_SERVICE_UUID "3e60a07c-235e-11ee-be56-0242ac120002"

/*
Minimum received package size.

Takes password, pid, commas into account.

Does not care for the extra info
*/
#define MIN_RECEIVED_SIZE (PASSWORD_ARRAY_SIZE + 1 + 6 + 1 + 6 + 1 + 6 + 1)  // = 28

// You can disable the Bluetooth during the preprocesse by definining "DO_NOT_USE_BLUETOOTH"
// #define DO_NOT_USE_BLUETOOTH

class PIDestalRemoteBLE {
   public:
    // Delegation constructor, initializes the array with NULL
    PIDestalRemoteBLE();
    PIDestalRemoteBLE(PIDestal* _pidPtr);
    PIDestalRemoteBLE(PIDestal* _pidArrayPtr[], int arraySize);
    ~PIDestalRemoteBLE();

    // Initialize should be called during setup()
    void initialize(const char* deviceName, const char* myPassword);

    // Process should be called during loop()
    void process();

    char* getExtraInfo() { return extraInfo; }
    // Sets the extra info value, it The array should be of size EXTRA_INFO_ARRAY_SIZE
    void setExtraInfo(const char* info) { strcpy(extraInfo, info); }
    void setExtraInfo(String info) { info.toCharArray(extraInfo, EXTRA_INFO_ARRAY_SIZE); }

    PID getFirstPidConsts();
    void setPidArrayConsts(PID newPID);

    PIDestal** getPidPtrArray() { return pidPtrArray; }
    void setPidPtrArray(PIDestal* _pidPtr);
    void setPidPtrArray(PIDestal* _pidArrayPtr[], int arraySize);

    // Returns the size of the PID array, there is no setter because it may result in unauthorized address access
    // The only way to set the array size is by setting the whole array with `setPidPtrArray(newPidArray, newPidArraySize);`
    int getPidPtrArraySize() { return pidPtrArraySize; }

   private:
    void deleteOldPidArray() {
        if (needsToDeleteArray) {
            delete[] pidPtrArray;
            needsToDeleteArray = false;
        };
    }

    bool checkValidPassword(String buffer);

    String extractStringFromData(String buffer);

    void processReceivedData();
    void updateGetters();

    String lastExtra;
    PID lastPID;

    String lastReceivedP;
    String lastReceivedI;
    String lastReceivedD;
    String lastReceivedExtra;

    String myDeviceName;

    char extraInfo[EXTRA_INFO_ARRAY_SIZE] = "";
    char password[PASSWORD_ARRAY_SIZE];

    PIDestal** pidPtrArray;
    int pidPtrArraySize = 0;
    bool needsToDeleteArray = false;

    BLEService pidService;

    // characteristics
    BLEStringCharacteristic pGetCharacteristic;
    BLEStringCharacteristic iGetCharacteristic;
    BLEStringCharacteristic dGetCharacteristic;
    BLEStringCharacteristic extraGetCharacteristic;

    BLEStringCharacteristic pSetCharacteristic;
    BLEStringCharacteristic iSetCharacteristic;
    BLEStringCharacteristic dSetCharacteristic;
    BLEStringCharacteristic extraSetCharacteristic;
};

#endif  // PIDESTAL_REMOTE_BLE_H
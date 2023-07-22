// Copyright (c) 2023 Rafael Farias
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef PIDESTAL_REMOTE_BLE_H
#define PIDESTAL_REMOTE_BLE_H

#include <ArduinoBLE.h>
#include <PIDestal.h>
#include <arduino.h>

#define ARDUINOJSON_USE_DOUBLE 0
#include "ArduinoJson-v6.21.2.h"

#define EXTRA_INFO_ARRAY_SIZE 64

#define PASSWORD_ARRAY_SIZE 6

#define DEFAULT_SERVICE_UUID "3e60a07c-235e-11ee-be56-0242ac120002"

#define MAX_CALLBACK_FUNCTIONS 16
#define MAX_PIDS 16

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
    // Type def of a function to be called through a callback
    typedef void (*FunctionPointer)();

    // Delegation constructor, initializes the array with NULL
    PIDestalRemoteBLE();
    PIDestalRemoteBLE(PIDestal* _pidPtr);
    PIDestalRemoteBLE(PIDestal* _pidArrayPtr[], size_t arraySize);
    ~PIDestalRemoteBLE();

    // Initialize should be called during setup()
    void initialize(const char* deviceName, const char* myPassword);

    // Process should be called during loop()
    void process();

    /*
        Receives an array of callable functions. The functions stored here can be called through Bluetooth, see  `runFunctionByIndex()`

        Does not pass arguments to these functions.
        ATTENTION: The max number of functions is defined by the constant "MAX_CALLBACK_FUNCTIONS"
    */
    void setCallbackFunctions(FunctionPointer funcs[], size_t arraySize);
    void setCallbackFunctions(FunctionPointer func);

    /*
        Runs the function from "callbackFunctions" at index
    */
    void runFunctionByIndex(size_t index);

    char* getExtraInfo() { return extraInfo; }
    // Sets the extra info value, it The array should be of size EXTRA_INFO_ARRAY_SIZE
    void setExtraInfo(const char* info) { strcpy(extraInfo, info); }
    void setExtraInfo(String info) { info.toCharArray(extraInfo, EXTRA_INFO_ARRAY_SIZE); }

    void updatePidArrayConsts(String receivedString);

    PIDestal** getPidPtrArray() { return pidPtrArray; }
    void setPidPtrArray(PIDestal* _pidPtr);
    void setPidPtrArray(PIDestal* _pidArrayPtr[], size_t arraySize);

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
    PID lastPID[MAX_PIDS];

    String lastReceivedPID;
    String lastReceivedExtra;

    String myDeviceName;

    char extraInfo[EXTRA_INFO_ARRAY_SIZE] = "";
    char password[PASSWORD_ARRAY_SIZE];

    PIDestal** pidPtrArray;
    size_t pidPtrArraySize = 0;
    bool needsToDeleteArray = false;

    size_t numOfStoredFunctions;  // Number of functions stored
    FunctionPointer callbackFunctions[MAX_CALLBACK_FUNCTIONS];

    BLEService pidService;

    /*
    {
        "pid":[
            [1,1,1],
            [1,1,1],
            [1,1,1]...
        ]
    }


    */
    StaticJsonDocument<256> getPidDoc;
    StaticJsonDocument<256> setPidDoc;

    // characteristics
    BLEStringCharacteristic pidGetCharacteristic;
    BLEStringCharacteristic extraGetCharacteristic;

    BLEStringCharacteristic pidSetCharacteristic;
    BLEStringCharacteristic extraSetCharacteristic;

    BLEStringCharacteristic callbackIdxSetCharacteristic;
};

#endif  // PIDESTAL_REMOTE_BLE_H
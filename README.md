<!--
 Copyright (c) 2023 Rafael Farias

 This software is released under the MIT License.
 https://opensource.org/licenses/MIT
-->

# PIDestal Remote BLE

This library was made for the ESP32 and makes use of the Bluetooth Low Energy, it exposes ways to set and get **PID constants** and a parameter **extraInfo** that can be customized by the user.

It is done using [GATT](https://www.bluetooth.com/bluetooth-resources/intro-to-bluetooth-gap-gatt/) with getters and setters. There is a simple validation on the setter that checks if the password is correct. The getters are openly distributed and can be subscribed to.

> **ATTENTION:** The communication is totally **unencrypted** so a middle man attack is very easily done if the attacker knows about the data format being sent.

For utilization examples please check out the `examples` directory.

## Usage

1. Create a new `PIDestalRemoteBLE` object and pass a `PIDestal` object by reference, PIDestalRemoteBLE will modify the PID values and keep the server updated.

2. Call the method `initialize()` in the Arduino's `setup()` function. You also need to provide the device's **NAME** and **PASSWORD**.

> **ATTENTION:** Passwords **MUST** be 6 characters long.

3. Call the method `process()` in the Arduino's `loop()` function.

Example:

```cpp
#include <PIDestal.h>
#include <PIDestalRemoteBLE.h>

PIDestal pid1(0.2, 1, 0.002);

PIDestalRemoteBLE myRemote(pid1);

void setup() {
    Serial.begin(115200);
    myRemote.initialize("ESP_BOARD", "123123");
}

void loop() {
    myRemote.process();
}
```

## Including the library in Arduino IDE

1. Clone this repo into `Documents -> Arduino -> libraries`.
2. Reload the IDE

> If something go wrong check [Add libraries to Arduino IDE](https://support.arduino.cc/hc/en-us/articles/5145457742236-Add-libraries-to-Arduino-IDE)

## Default UUIDs

-   Service UUID: `3e60a07c-235e-11ee-be56-0242ac120002`.

Characteristics:

-   Getters:

    -   Constant P [`float`]: `a5831824-2445-11ee-be56-0242ac120002`.
    -   Constant I [`float`]: `a5831c2a-2445-11ee-be56-0242ac120002`.
    -   Constant D [`float`]: `a58322c4-2445-11ee-be56-0242ac120002`.
    -   Extra [`String`]: `a5832454-2445-11ee-be56-0242ac120002`.

-   Setters:
    -   Constant P [`String`]: `a5832594-2445-11ee-be56-0242ac120002`.
    -   Constant I [`String`]: `a58326e8-2445-11ee-be56-0242ac120002`.
    -   Constant D [`String`]: `a58328a0-2445-11ee-be56-0242ac120002`.
    -   Extra [`String`]: `a5832a62-2445-11ee-be56-0242ac120002`.

### Changing the service UUID

The service UUID defines "in simple terms" the device's id.

By default it's set to `3e60a07c-235e-11ee-be56-0242ac120002`.

> You can generate UUIDs at [https://www.uuidgenerator.net/](https://www.uuidgenerator.net/)

You can modify the default UUID by replacing the `#define DEFAULT_SERVICE_UUID` located at `PIDestalRemoteBLE.h`.

Example `PIDestalRemoteBLE.h`

```cpp
// Define at PIDestalRemoteBLE.h
#define DEFAULT_SERVICE_UUID "3e60a07c-235e-11ee-be56-0242ac120002"
```

## Disabling communication

You can **disable** the communication by uncommenting `DO_NOT_USE_BLUETOOTH` on the library's header file `PIDestalRemoteBLE.h`.

```cpp
// You can disable the Bluetooth during the preprocesse by definining "DO_NOT_USE_BLUETOOTH"
#define DO_NOT_USE_BLUETOOTH
```

## References

-   [Especificação BLE](https://btprodspecificationrefs.blob.core.windows.net/assigned-numbers/Assigned%20Number%20Types/Assigned_Numbers.pdf#page=63&zoom=100,96,94)

Service ( Generic User Property Server )
0x1013

-   Gerador de UUID [https://www.uuidgenerator.net/](https://www.uuidgenerator.net/)

-   Good videos about BLE

    -   [https://www.youtube.com/watch?v=0Yvd_k0hbVs&ab_channel=MoThunderz](https://www.youtube.com/watch?v=0Yvd_k0hbVs&ab_channel=MoThunderz)
    -   [https://www.youtube.com/watch?v=0Yvd_k0hbVs&ab_channel=MoThunderz](https://www.youtube.com/watch?v=0Yvd_k0hbVs&ab_channel=MoThunderz)

-   [ArduinoBLE library with examples](https://github.com/arduino-libraries/ArduinoBLE)

-   Example of sending array "I'm not using" [https://forum.arduino.cc/t/sending-multiple-floats-over-ble/857374](https://forum.arduino.cc/t/sending-multiple-floats-over-ble/857374)

-   Issue about the BLEStringCharacateristic [https://github.com/arduino-libraries/ArduinoBLE/pull/5](https://github.com/arduino-libraries/ArduinoBLE/pull/5)

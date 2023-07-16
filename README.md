<!--
 Copyright (c) 2023 Rafael Farias

 This software is released under the MIT License.
 https://opensource.org/licenses/MIT
-->

# PIDestal Remote BLE

This library was made for the ESP32 and makes use of the Bluetooth Low Energy, it exposes ways to set and get **PID constants** and a parameter **extraInfo** that can be customized by the user.

For utilization examples please check out the `examples` directory.

> Currently only **4 decimal places** are being sent.

## Customization

When initializing the object you need to set the device's **NAME** and **PASSWORD**.

**ATTENTION** Passwords **MUST** be 6 characters long.

> You can generate UUIDs at [https://www.uuidgenerator.net/](https://www.uuidgenerator.net/)

Example

```cpp
#include <PIDestal.h>
#include <PIDestalRemoteBLE.h>

PIDestal pid1(0.2, 1, 0.002);

// You can generate valid UUIDs here https://www.uuidgenerator.net/
PIDestalRemoteBLE myRemote(pid1);

void setup() {
    Serial.begin(115200);
    myRemote.initialize("My Simple ESP32 Connection Name", "123123");
}

void loop() {
    myRemote.process();
}
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

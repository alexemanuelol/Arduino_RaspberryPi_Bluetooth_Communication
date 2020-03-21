# Arduino_Bluetooth_Communication
Bluetooth communication between two arduino uno/ nano

## Door module

- When connected to the raspberry pi via bluetooth, the raspberry pi will send "0x1" indicating that it want to start a connection
- When Arduino Nano detects this event it will enter listen mode for the sensor and send "0x2" when the door opens, "0x3" when the door closes.
- "0x4" can be sent from the raspberry pi to check if the bluetooth connection to the Arduino Nano is still present (Only when Arduino Nano is in "waitForConnectionEstablished" mode).
- "0x4" can also be sent from the Arduino Nano to see if the connection is still present (This happens every 10 seconds).


## HC-05 - Bluetooth Module
[HC-05 - Bluetooth Module link](https://components101.com/wireless/hc-05-bluetooth-module)

[HC-05 Datasheet](docs/HC-05_datasheet.pdf)

## Setup HC-05 for BT communication [here](HC-05_Setup/README.md).


## Software for Breadboard design

[Fritzing Electronic Design](https://github.com/fritzing/fritzing-app)

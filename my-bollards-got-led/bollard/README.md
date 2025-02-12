| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- |

ESP-IDF Combined Bluetooth advertising and scanning
===================================================

Copy of this sample as base for Bollard code
todo edit this document to reflect changes to functionality



Have a fixed bollard number 1-4

Separate tasks to manage Bluetooth comms and display

Bluetooth
Maintain list of Parcel devices within given range (RSSI) over the last five seconds and if they are at the correct bollard or not

Listen for Set/Check/Reset broadcasts from controller. Use a hop counter when rebroadcasting this signal to other bollards to extend the range

On Set call display to enter waiting mode
On Check call correct/incorrect display based on state of Parcel devices seen list
On Reset enter waiting mode

Display
Receive calls to set lights based on game state. Ideally animated sequences




This is a Bluetooth advertising and scanning demo with virtual HCI interface. Send Reset, ADV_PARAM, ADV_DATA and HCI_ADV_ENABLE command for BLE advertising. And SET_EVENT_MASK, SCAN_PARAMS and SCAN_START commands for BLE scanning. Scanned advertising reports from other devices are also displayed.

In this example no host is used. But some of the functionalities of a host are implemented.

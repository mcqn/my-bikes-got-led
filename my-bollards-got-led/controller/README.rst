.. zephyr:code-sample:: ble_peripheral
   :name: Peripheral
   :relevant-api: bt_gatt bluetooth

   Implement basic Bluetooth LE Peripheral role functionality (advertising and exposing GATT services).

Copy of this sample as base for Controller code
todo edit this document to reflect changes to functionality

Manages the game running with button presses

    A Set the game: Broadcast to all bollards to enter waiting mode
    B Check: Broadcast to all the bollards to check status and display correct/incorrect indicator
    A+B Reset Broadcast to all the Parcels to generate a random destination number 1-4 and broadcast to bollards to enter waiting mode





Overview
********

Application demonstrating the Bluetooth LE Peripheral role. It has several well-known and
vendor-specific GATT services that it exposes.


Requirements
************

* BlueZ running on the host, or
* A board with Bluetooth LE support

Building and Running
********************

This sample can be found under :zephyr_file:`samples/bluetooth/peripheral` in the
Zephyr tree.

See :zephyr:code-sample-category:`bluetooth` samples for details.

.. zephyr:code-sample:: ble_peripheral
   :name: Peripheral
   :relevant-api: bt_gatt bluetooth

   Implement basic Bluetooth LE Peripheral role functionality (advertising and exposing GATT services).

Copy of this sample as base for Parcel code
todo edit this document to reflect changes to functionality

Broadcast destination number to bollards
Listen for reset command from Controller and generate a random number (1-4)








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

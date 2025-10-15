# My Bikes got LED

The hardware for MBiGL is based around the [ESP32 C3 Mini](https://www.espressif.com/sites/default/files/documentation/esp32-c3-mini-1_datasheet_en.pdf) 

## ESPC3-mini Pins Used

| Pin | Function | Note |
| ---- | ---- | ----|
| IO0 | GPIO | - |
| IO1 | GPIO | - |
| IO3 | GPIO | Also available as a level shifted data output by bridging JP42 |
| IO4 | GPIO | Also available as a level shifted data output by bridging JP43 |
| IO5 | LED_data | |
| IO6 | LED_Clock | |
| IO7 | WiFi LED | |
| IO9 | Boot pin | Access via J11 |
| IO10 | Reset pin | See note below |


## Interfaces

### Coding

Code can be uploaded via USB serial or with an external UART board via J31 (six pin header).
More details on the [Espressif website](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/get-started/establish-serial-connection.html)

### RGB data outpus

There are four pins available for RGB data output. LED_data (IO5) and LED_clock (IO6) are the primary outputs. IO3 and IO4 are not connected to the level shifter (U4) by default but can be used as additional channels by bridging JP42 and JP43 with solder if required.

**NB IO3/4 are also available as GPIO pins via J10 but cannot be used for both**

### External Crystal

IO0 and IO1 are used by the C3 mini if an external crystal is required. The first revision had footprints for a crystal but this has now been removed as it isn't needed.
These two pins are available via j10 as GPIO

### Additional pins

IO7 is used to drive the Wifi LED indicator D6

IO9 is used to toggle boot modes and can be accessed via J11 See the [C3 technical manual for more details](https://www.espressif.com/sites/default/files/documentation/esp32-c3_technical_reference_manual_en.pdf#bootctrl)

~~IO10 is connected to the Enable pin via JP4, a breakable bridge. This pin is used for the reset usermod. In addition to the bridge pad there are two unpopulated 0603 resistors here in case an RC circuit is required for timing. R17 has GND and float pads R18 3V3 and float.~~

These have been removed from production boards.

### Headers

J5 and J6 both provde 5v 3v3 and Gnd outputs via 2.54 header

J40 provides 5V, Data, and GND access as an alternative light connection to the socket J2



## Battery management fine tuning

R23-26 are connected to the battery management chip [IP5310](http://www.injoinic.com/wwwroot/uploads/files/20200221/a7a6128e03ee193cf35e48b28e3df9df.pdf) Only R26 is populated but all are available for fine tuning of the charging circuit functions

| Resistor | Name | Default status |  Function |
| ---- | ---- | ---- | ---- |
| R23 | VSET | DNP | Battery voltage selection | 
| R24 | VTHS | DNP | Battery platform selection |
| R25 | RSET | DNP | Set the battery impendence |
| R26 | NTC | 51K | Detect battery pack temperature |

A test point labeled Key1 is available to break out the Key pin on the IP5310 if push button functionality is required

D1-4 are connected to give a four light 'State of Charge' indication


## Alternate board conifguarations

If a thinner board footprint is required there are alternate pads provided for the USB socket and JST battery connector.
Only one each of the following is required


USBC1 or USBC2

J51 or J52


## Changes to board to accomodate INMP441

Issue 38 suggests adaptions to the board to accomodate a footprint and headers for an INMP441 microphone board.

The reset pin has been removed entirely, reset can be achieved via SW3 making IO10 available. 

L/R selection doubles up on a level shifted output so the microphone is not available if this output is enabled (JP43)



| Pin | INMP441 | Note |
| ---- | ---- | ----|
| IO10 | SD | Previously reset pin |
| IO0 | SCK | - |
| IO01 | WS | - |
| IO04 | L/R | Alt available as level shifted output |























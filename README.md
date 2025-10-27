# My Bike's Got LED
[My Baby's Got LED](https://github.com/mcqn/my-babys-got-led), bike edition. The same [WLED](https://kno.wled.ge/) fun but without being tied to a wall socket.

![ibal234-KensingtonFog](https://github.com/user-attachments/assets/81efec46-25d8-4d49-ba27-991d3836ec99)

My Bike's got LED is a small, battery powered [ESP32 C3](https://www.espressif.com/en/products/socs/esp32-c3) board designed to drive Neopixel/WS2812 LED strips. The original was created for use on bikes, hence the name, but it works just as well for other wearable/portable light projects. It is intended to be usually powered by lipo batteries via a JST terminal, but an additional power bank source can also be attached via the USB-C socket. 
It is in the process of certification with [OSHWA](https://certification.oshwa.org/) 


Boards are shipped with our [getting started zine](https://github.com/mcqn/my-bikes-got-led/blob/main/electronics/mybikesgotzine.svg) below is a more detailed discription of how to set up and use My Bike's got LED

### Setting up your bike

The board kit we [sell on Tindie](https://www.tindie.com/products/mcqn_ltd/my-bikes-got-led/) comes with a strip of LEDs already connected to an Amphenol Anytek plug suitable for the on board [four way pluggable terminal block](https://uk.farnell.com/amphenol-anytek/oq0412510000g/terminal-block-r-a-header-4way/dp/3810182#anchorTechnicalDOCS) My Bike's got LED has two main LED data outputs, Data and Clk. The default channel of Data is used, but the secondary output of Clk can also be used if required. (In fact it is possible to use four level shifted outputs to drive for independent light strips if required, see the additonal notes on the [electronics page](https://github.com/mcqn/my-bikes-got-led/tree/main/electronics) if you want to know more about this.) If you are using your own lights connect the three wires, 5V, GND and Data to the screw terminals on the green plug as indicated on the board silkscreen.


Next you will need to attached the lights to your bike, we use velcro ties but if you don't have any cable ties work just as well. They key thing to be aware of here is that the plug end of the light strip is within reach of where you will attach the board. In our kits we provide a saddle bag to store the board and battery, so the plug end of the strips should be close to the seat post. Check first that the strip and socket reach where you would like them, before then securing the lights in place with the ties. Be careful to avoid any moving parts when you secure your lights in place.

Once the lights are secure you can connect them to the board with the green plug, and connect the main battery to the board via the white JST connector. Then turn on the board with the on/off switch. The first thirty lights on your strip will illumate amber. This is the default WLED configuration, information on how to change this is shown in the next step.


### Setting up the board

When a board running WLED is first turned on it will attempt to connect to a known wifi network. If it doesn't have information about a network, or it is out of range, it will instead set up it's own acess point for configuration. The boards we ship have a fresh install of WLED with no knowledge of your network so they will always start in access point mode.  

#### Connecting to wifi

When the board is in access point mode you will be able to connect another device (Mobile/latop) to that network. 

- Open your wifi settings an look for the networked called WLED-AP
- Connect to it and log on with password wled1234

You will arrive at a page with two options. 

- To the controls
- Wifi settings

Use 'To the controls' if you just want to change the settings, this may be all you need to get your board set up.  See 'effects' below for more information on setting the light patterns.

The Wifi settings takes you through the process of connecting the board to a local network, this is useful if you want to change settings without having to connect to the board AP. Once the device is connected to your network you will be able to access the settings page on any other device connected to that network. You will need to add the SSID and password of the network you want to connect to. It's also a good idea to change the mDNS address to something you can recognise, the default is WLED.

More details on wifi set up can be found on the [WLED documentation](https://kno.wled.ge/basics/getting-started/#wifi-setup)


#### Effects

#### Setting up presets



### Adding usermods
Aside from the LED data outputs there are other GPIO pins available to extend the capability of the board, with WLED user mods. 

If you have a [UK000073](https://certification.oshwa.org/uk000073.html) board pins 0, 1, 3, 4 and 10 are available on header J10.

On version [UK000083](https://certification.oshwa.org/uk000083.html) 0, 1, 4 and 10 are in the circular 'Microphone' header, designed to work with INMP441 microphone breakout boards. Pin 3 is available as 'Button' next to the power header.

Both configurations offer the ability to add an 'effects button' to cycle through preprogrammed lighting effects and to implement sound reative effects with an audio input.


#### Effects button

There are times when you might want to cycle through the effects you have set up. 

......

More information about setting up buttons is available in the [WLED documentation](https://kno.wled.ge/features/macros/#buttons)

#### Sound reactive

There are more instructions in the [WLED documentation](https://mm.kno.wled.ge/WLEDSR/Digital-Microphone-Hookup/)

### Updating the software

WLED is an active project and new version are released often. If you need to update to a more current version of the software there are a few options available.

One route is to use the [unofficial online complier tool](https://wled-compile.github.io/) NB. this requires Chrome/Edge browsers for the Web USB upload to work.


- In step 1 select 'esp32c3dev' as the Microcontroller type and in step 3 tick the 'Audio reactive' box, everything else can be left as default.
- Compile the code, step 5 and then install with the instructions described in step 6.
- Follow the prompts to connect to your wifi
- Once connected click the link to see the controls

You will need to configure the default pin settings and number of leds as well as the pins for the audio input device you are using. See [electronics](https://github.com/mcqn/my-bikes-got-led/tree/main/electronics) for more details about the pinouts. 

















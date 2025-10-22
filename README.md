# My Bike's Got LED
[My Baby's Got LED](https://github.com/mcqn/my-babys-got-led), bike edition. The same [WLED](https://kno.wled.ge/) fun but without being tied to a wall.

>>>>Image here

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

#### Effects

#### Setting up presets



### Adding usermods

#### Effects button

#### Sound reactive

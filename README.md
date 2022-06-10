# ZBEduino

This is a small program for an arduino

## Supported Arduino
- Wemos D1 (ESP8266 based), R1 is what I have

### Identifying Wemos D1 version
You have a wemos D1 R2 if you have SCL/SDA pins below the A0 pin

### Planned to be supported later
- Wemos D1 (ESP32 based)
- Sparkfun artemis (XXX based)

### CAN Adapter

I'm using a Keyestudio CAN-BUS Shield KS0411 which has pretty bad
documentation. But the good guys @ Zephyr gave ma better pinout than the
official docs, so that's really nice:
https://docs.zephyrproject.org/latest/boards/shields/mcp2515/doc/index.html#keyestudio-can-bus-shield-ks0411

You need to use the corect pins for the SPI CS (D10) and the MCP2515 INT (D8),
on a Wemos D1 R1 this is actually D0 and D15.

## Development

### Setting up env

I'm using arduino-cli pretty much exclusively, but obviously you can use the IDE

```
arduino-cli config init --additional-urls https://arduino.esp8266.com/stable/package_esp8266com_index.json
arduino-cli core update-index
arduino-cli core install esp8266:esp8266
```

Then we need to allow unsafe configs since we will use the spakrfun CAN lib so
use the newly created arduino-cli.yaml in ~/.arduino-15/arduino-cli.yaml and
set enable_unsafe_install to true. Explanation here:
https://arduino.github.io/arduino-cli/0.19/configuration/#configuration-keys

Now we install the 3rd party lib, I don't really understand why you need this one but :D
```
arduino-cli lib install --git-url https://github.com/coryjfowler/MCP_CAN_lib.git
```

### Compiling & Uploading

```
arduino-cli compile --fqbn esp8266:esp8266:d1
arduino-cli upload -p /dev/ttyUSB0 --fqbn esp8266:esp8266:d1_mini:baud=115200

```

#### Useful docs

This is an unstructured list of various things I found helpful:
* https://docs.zephyrproject.org/latest/boards/shields/mcp2515/doc/index.html#keyestudio-can-bus-shield-ks0411
* https://github.com/Longan-Labs/Aruino_CAN_BUS_MCP2515/blob/master/examples/recv/recv.ino
* https://github.com/coryjfowler/MCP_CAN_lib/blob/master/examples/Standard_MaskFilter/Standard_MaskFilter.ino
* https://sundays-ev.com/samurai/new/can-bus
* https://github.com/plapointe6/EspMQTTClient/blob/master/examples/SimpleMQTTClient/SimpleMQTTClient.ino

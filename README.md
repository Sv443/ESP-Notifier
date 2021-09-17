<div align="center" style="text-align: center;">

# ESP-Notifier
WiFi enabled IoT button based on ESP8266 - intended to be used with [Node-Notifier](https://github.com/Sv443/Node-Notifier)

</div>

<br>

## Setup:
1. Clone or download and extract this repository (see [releases](https://github.com/Sv443/ESP-Notifier/releases))
3. Rename the file `config.h.template` to `config.h` and open it with a text editor
3. Set your WiFi settings in `config.h`
6. Start the [desktop client](https://github.com/Sv443/Node-Notifier) and copy its IP address
2. Open the file [`ESP-Notifier.ino`](./ESP-Notifier.ino) in the [Arduino IDE](https://www.arduino.cc/en/software)
3. Edit settings at the top of this file (like the server IP address)
4. Upload the sketch with the Arduino IDE (<kbd>Ctrl</kbd> + <kbd>U</kbd>)
5. Hook up electronics (see [schematic below](#schematic))

<br><br>

## Schematic:

<div align="center" style="text-align: center;">

[![schematic image](./.github/schematic.png)](./.github/schematic.png)

(I used an ESP8266EX development board but this should work on any ESP board, once GPIO pin mappings are changed in the .ino file)

</div>

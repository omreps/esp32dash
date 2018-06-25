[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

# esp32dash
esp32dash enables your ESP32 show the dashboard in web-browser.
For example for the connected sensors values & outputs states & variables and so on. 
It does this by using the Google charts. This code is made for the ESP32 and use the [arduino-esp32](https://github.com/espressif/arduino-esp32).

![alt text](https://github.com/0megaIT/esp32dash/blob/master/img/mock-up4-git.png)

# Hardware
- ESP32-board

![alt text](https://github.com/0megaIT/esp32dash/blob/master/img/ESP32-chip-icon.png)

# Installation instructions
- Install the arduino-core for the ESP32.
- Install the external library: [bbx10/WebServer_tng](https://github.com/bbx10/WebServer_tng)
- Configure IDE: Flash frequency 80 MHz, Flash size 4Mb(32Mb), Flash mode QIO, Upload speed is 115200.
- Configure code - seach for the "<-----" string in esp32dash.ino 
  - Set your Wi-Fi network SSID and PASS in the code. 
  - Set your Wi-Fi network IP, gate & mask, or leave as is for DHCP mode.
- Connect ESP32 board to the PC via the best of your USB cables. 
(it is very ! Important: Use the best USB-cable for the enough power supply.)
- Compile and upload the code to your ESP32.
- Upload the data to SPIFFS (see [here](https://github.com/me-no-dev/arduino-esp32fs-plugin) for more information).
- Open the COM-port monitor if you want.
- Wait until it connects to your Wi-Fi network. If your ESP32 board have the led on the 2-nd pin, it will blink 2 times.
- Go to [http://myESP32.local](http://myESP32.local) and there you'll find the dashboard with a few bilt-in sensors values. 
- Open the code again and add your live values to the dashItems array. Compile and upload the code to your ESP32.
- Refresh the page in the browser. Your data will be shown on the dash.
- We are the champions!

# Status indicators
This firmware use the led on the ESP32 2-nd pin to indicate it's status:
- On startup
  - normal blink for the 2 times: Wi-Fi founded and device connected as a client.
  - fast blink for the 5 times: something wrong, can't connecting to the Wi-Fi.

# Credits
- [Hopperpop](https://github.com/Hopperpop) for his well-structured project [Sattrack](https://github.com/Hopperpop/Sattrack) gave me the idea about how to bring things together making a web-server firmware for the ESP32.


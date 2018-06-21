# Esp32dash
The esp32dash is a code that enables your ESP32-device show in the web-browser its dashboard.
For example for the connected sensors values & outputs state. 
It does this by using the Google charts. This code is made for the ESP32 and use the [arduino-core](https://github.com/espressif/arduino-esp32).
 
# Hardware
- ESP32-board, for example NodeMCU or other

# External libraries
- [bbx10/WebServer_tng](https://github.com/bbx10/WebServer_tng)

# Installation instructions
- Install the arduino-core for the ESP32.
- Install the external libraries.
- Change Flash frequency to 80 MHz and use Flash size 4Mb(32Mb). Set Flash mode QIO.
- Upload speed is 115200.
- Write your Wi-Fi network SSID and password in the code.
- Connect ESP32 board to the PC via the best of your USB cables. 
(it is very ! Important: Use the best USB-cable for the enough power supply.)
- Compile and upload the code to your ESP32.
- Upload the data to SPIFFS (see [here](https://github.com/me-no-dev/arduino-esp32fs-plugin) for more information).
- Open the COM-port monitor if you want.
- Wait until it connects to your Wi-Fi network. If your ESP32 board have the led on the 2-nd pin, it will blink 2 times.
- Go to [http://esp32dash](http://esp32dash) and there you'll find the dashboard with a few bilt-in sensors values. 
- Open the code again and add your live values to the dashItems array. Compile and upload the code to your ESP32.
- Refresh the page in the browser. Your data will be shown on the dash.
- We are the champions!

# Status indicators
This device use the led on the 2-nd pin to indicate it's status:
- On startup
  - normal blink for the 2 times: Wi-Fi founded and device connected as a client.
  - fast blink for the 5 times: something wrong, can't connecting to the Wi-Fi.

# Credits
- [Hopperpop](https://github.com/Hopperpop) for the idea about how to bring things together making web-server firmware for the ESP32


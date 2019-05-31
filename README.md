# esp32cam: OV2640 camera on ESP32-CAM

This library wraps [esp32-camera library](https://github.com/espressif/esp32-camera) in object oriented API.
It has been tested with AI Thinker [ESP32-CAM](http://wiki.ai-thinker.com/esp32-cam) board and OV2640 camera.

In ESP32 Arduino, the board type should be set to "ESP32 WROVER" to enable 4MB external PSRAM.
Otherwise, internal RAM is insufficient to support high resolution.

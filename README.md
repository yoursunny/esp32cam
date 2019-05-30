# OV2640 camera on ESP32-CAM

This library enables access to OV2640 camera on AI Thinker ESP32-CAM module in ESP32 Arduino environment.
It uses [esp32-camera library](https://github.com/espressif/esp32-camera), and has been tested with AI Thinker [ESP32-CAM](http://wiki.ai-thinker.com/esp32-cam) board.

In ESP32 Arduino, the board type should be set to "ESP32 WROVER" to enable 4MB external PSRAM.
Otherwise, internal RAM is insufficient to support resolution higher than 160x120.

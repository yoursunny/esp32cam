# OV2640 camera on ESP32-CAM

This library enables access to OV2640 camera on AI Thinker ESP32-CAM module in ESP32 Arduino environment.
It is ported from [ESP32 Camera Demo](https://github.com/donny681/ESP32_CAMERA_QR), and tested with AI Thinker [ESP32-CAM](http://wiki.ai-thinker.com/esp32-cam) board.

In ESP32 Arduino, the board type should be set to "ESP32 WROVER" to enable 4MB external PSRAM.
Otherwise, internal RAM is insufficient to support resolution higher than 160x120.

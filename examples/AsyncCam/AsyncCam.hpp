#ifndef ASYNCCAM_HPP
#define ASYNCCAM_HPP

// This macro indicates where to send debug logs.
// Delete this macro to disable debug logging.
#define ESP32CAM_ASYNCWEB_LOGGER Serial

#include <esp32cam-asyncweb.h>

#include <ESPAsyncWebServer.h>

extern esp32cam::Resolution initialResolution;
extern esp32cam::Resolution currentResolution;

extern AsyncWebServer server;

void
addRequestHandlers();

#endif // ASYNCCAM_HPP

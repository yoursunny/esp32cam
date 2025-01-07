#ifndef ASYNCCAM_HPP
#define ASYNCCAM_HPP

// It is necessary to include ESPAsyncWebServer.h before esp32cam.h for Arduino builder to
// recognize the dependency.
#include <ESPAsyncWebServer.h>

#include <esp32cam.h>

extern esp32cam::Resolution initialResolution;

extern AsyncWebServer server;

void
addRequestHandlers();

#endif // ASYNCCAM_HPP

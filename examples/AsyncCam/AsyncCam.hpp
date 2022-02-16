#ifndef ASYNCCAM_HPP
#define ASYNCCAM_HPP

#include <esp32cam-asyncweb.h>

#include <ESPAsyncWebServer.h>

extern esp32cam::Resolution initialResolution;
extern esp32cam::Resolution currentResolution;

extern AsyncWebServer server;

void
addRequestHandlers();

#endif // ASYNCCAM_HPP

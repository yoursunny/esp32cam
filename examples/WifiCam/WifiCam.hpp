#ifndef WIFICAM_H
#define WIFICAM_H

#include <esp32cam.h>

#include <WebServer.h>

extern esp32cam::Resolution initialResolution;

extern WebServer server;

void
addRequestHandlers();

#endif // WIFICAM_H

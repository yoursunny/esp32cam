#ifndef WIFICAM_H
#define WIFICAM_H

#include <WebServer.h>
#include <esp32cam.h>

extern esp32cam::Resolution initialResolution;

extern WebServer server;

void
addRequestHandlers();

#endif // WIFICAM_H

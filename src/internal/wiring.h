#ifndef __OMV_PORT_H__
#define __OMV_PORT_H__

#include <esp32-hal.h>

// Some functions to make OpenMV happy
#define systick_sleep(t) delay(t)
#define __disable_irq()
#define __enable_irq()



#endif //__OMV_PORT_H__

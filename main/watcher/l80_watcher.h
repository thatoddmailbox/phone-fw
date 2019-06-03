#ifndef _WATCHER_L80_WATCHER_H
#define _WATCHER_L80_WATCHER_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include <esp_log.h>

#include <sys/time.h>
#include <stdatomic.h>
#include <time.h>

#include "external/minmea.h"

#include "device/l80.h"

void l80_watcher_start();

#endif
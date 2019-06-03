#ifndef _WATCHER_M26_WATCHER_H
#define _WATCHER_M26_WATCHER_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include <esp_log.h>

#include <stdatomic.h>

#include "device/m26.h"

atomic_bool m26_watcher_dirty;
atomic_uint_fast8_t m26_watcher_registration;
atomic_uint_fast8_t m26_watcher_signal;
atomic_uint_fast16_t m26_watcher_voltage;

void m26_watcher_start();

#endif
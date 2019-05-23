#ifndef _DEVICE_M26_TRANSPORT_H
#define _DEVICE_M26_TRANSPORT_H

#include <stdint.h>
#include <string.h>

#include <esp_log.h>
#include <esp_transport_utils.h>
#include <esp_transport.h>

#include "device/m26.h"

esp_transport_handle_t m26_transport_init();

#endif
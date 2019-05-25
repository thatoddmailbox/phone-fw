#ifndef _DEBUG_H
#define _DEBUG_H

#include <stddef.h>
#include <stdint.h>

void debug_init();
void debug_send_buffer(void * buffer, size_t buffer_size);

#endif
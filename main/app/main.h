#ifndef _APP_MAIN_H
#define _APP_MAIN_H

typedef struct {
	const char * name;
	void (*init)();
	void (*start)();
} app_t;

void app_init();

#endif
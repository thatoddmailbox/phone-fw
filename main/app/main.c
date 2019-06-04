#include "app/main.h"

#include "app/launch/main.h"
#include "app/sms/main.h"

void app_init() {
	app_launch.init();
	app_sms.init();
}
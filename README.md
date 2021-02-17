# phone-fw
The firmware portion of a [phone I built](https://github.com/thatoddmailbox/phone). You might also want to see [the hardware](https://github.com/thatoddmailbox/phone-hw).

It is built on top of the ESP32's FreeRTOS SDK, and makes extensive use of FreeRTOS's tasks, semaphores, and queues to synchronize code on the dual-core ESP32.

It also includes a number of custom drivers, to control the various peripherals of the phone, including UART communication with the GPS and cellular modules, I2C communication with the I/O expander (used for the buttons on the phone, among other things), and SPI communication with the display.

There's also a custom UI library capable of drawing text at various font sizes, icons, buttons, lists, and allowing users to navigate through screens with the scrollwheel and buttons of the phone.

## Ideas for further expansion
* Sleep/wake instead of just always being on all the time
* Some sort of text input method
* Bluetooth device setup (specifically for headsets so that audio input/output can happen)
* Volume up/down (needs audio first)
* Ability to send SMS
* Making calls
* Receiving calls
* Connecting to WiFi
* Actually do something with the GPRS data connection (a browser might be too much, but weather? news?)
* Do more with the GPS data than set the time (would a map app be too much?)
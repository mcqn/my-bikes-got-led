// Interface for the LED display

#ifndef APP_LED_H
#define APP_LED_H

#ifdef __cplusplus
extern "C" {
#endif

// Start the LED display task
// (This will also start the Arduino subsystem)
extern int LEDs_Start();

extern int LEDs_Success();
extern int LEDs_Failure();
extern int LEDs_Waiting();
extern int LEDs_Progress(uint8_t aPercentDone);

#ifdef __cplusplus
}
#endif

#endif

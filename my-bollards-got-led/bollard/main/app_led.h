// Interface for the LED display

#ifndef APP_LED_H
#define APP_LED_H

#ifdef __cplusplus
extern "C" {
#endif

enum eLEDPattern
{
    ePatternNone,
    ePatternLarson,
    ePatternAlternate
};

// Start the LED display task
// (This will also start the Arduino subsystem)
extern int LEDs_Start();

extern int LEDs_Success();
extern int LEDs_Failure();
extern int LEDs_Waiting();
extern int LEDs_Progress(uint8_t aPercentDone);
extern int LEDs_Pattern(uint8_t aRed, uint8_t aGreen, uint8_t aBlue, enum eLEDPattern aPattern);

#ifdef __cplusplus
}
#endif

#endif

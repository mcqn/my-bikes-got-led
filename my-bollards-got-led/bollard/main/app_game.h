// Interface for the game controller

#ifndef APP_GAME_H
#define APP_GAME_H

#ifdef __cplusplus
extern "C" {
#endif

// Start the game controller task
// @param aName - name of this bollard, matched against kBollardNames
extern int Game_Start(const char* aName);

extern int Game_MessageReceived(char* aSender, int aRSSI, char* aMessage, int aMessageLen);

#ifdef __cplusplus
}
#endif

#endif

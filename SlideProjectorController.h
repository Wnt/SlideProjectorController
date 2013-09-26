// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef SlideProjectorController_H_
#define SlideProjectorController_H_
#include "Arduino.h"


#include "TimerOne.h"


//end of add your includes here
#ifdef __cplusplus
extern "C" {
#endif
void loop();
void setup();
#ifdef __cplusplus
} // extern "C"
#endif

//add your function definitions for the project SlideProjectorController here

void parseCommands();
void sayHello();
void loadProjectorConfig();
void initProjectorPins();
void printProjectorInfo(struct Projector *);
void tick();



//Do not add code below this line
#endif /* SlideProjectorController_H_ */

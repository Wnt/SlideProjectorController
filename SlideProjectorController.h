// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef analogInSerialOut_H_
#define analogInSerialOut_H_
#include "Arduino.h"
//add your includes for the project analogInSerialOut here


//end of add your includes here
#ifdef __cplusplus
extern "C" {
#endif
void loop();
void setup();
void parseCommands();
void sayHello();
void loadProjectorConfig();
void printProjectorInfo(struct Projector *);
#ifdef __cplusplus
} // extern "C"
#endif

//add your function definitions for the project analogInSerialOut here




//Do not add code below this line
#endif /* analogInSerialOut_H_ */

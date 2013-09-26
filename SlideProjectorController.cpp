/*
* This file is part of SlideProjectorController
 * Copyright (C) 2013 Jonni Nakari <jonni@egarden.fi>
 *
 * SlideProjectorController is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SlideProjectorController is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this SlideProjectorController. If not, see <http://www.gnu.org/licenses/>.
 */

// Do not remove the include below
#include "SlideProjectorController.h"

typedef struct Command {
	int tick;
	int probability;
	int command;
	Command * next;
	Command(int tick_p, int probability_p, int command_p)
	{
		tick = tick_p;
		probability = probability_p;
		command = command_p;
		next = NULL;
	}
} Command;

typedef struct Projector {

	int next_pin;
	int previous_pin;
	int slide_number;
	int loop_number;
	int last_command_tick;
	Command * first_command;
	Command * current_command;

} Projector;


enum CommandType {
	NEXT, PREVIOUS, RESTART
};
enum Probability {
	ALWAYS, SOMETIMES, RARELY
};
//int NEXT = 1;
//int PREVIOUS = 2;
//int RESTART = 3;
//int ALWAYS = 1;
//int SOMETIMES = 5;
//int RARELY = 8;

struct Projector projectors[] = {
//		next_pin	previous_pin	inits rest to zero
		2,			3, 			0, 0, 0, NULL, NULL,
		4,			5, 			0, 0, 0, NULL, NULL,
		6,			7, 			0, 0, 0, NULL, NULL,
		8,			9, 			0, 0, 0, NULL, NULL,
		A1,			A2,			0, 0, 0, NULL, NULL
};
int projector_count = 5;
//int projector_count = 1;
int command_list[] = {
//		proj#	tick	probability		command
		0,		0,		ALWAYS,			NEXT,
		0,		100,	ALWAYS,			PREVIOUS,
		0,		100,	ALWAYS,			RESTART,
		1,		0,		ALWAYS,			NEXT,
		1,		100,	ALWAYS,			NEXT,
		1,		100,	ALWAYS,			PREVIOUS,
		1,		100,	ALWAYS,			PREVIOUS,
		1,		100,	ALWAYS,			RESTART,
		2,		0,		ALWAYS,			PREVIOUS,
		2,		100,	ALWAYS,			NEXT,
		2,		100,	ALWAYS,			PREVIOUS,
		2,		100,	ALWAYS,			NEXT,
		2,		100,	ALWAYS,			RESTART,
		3,		0,		ALWAYS,			NEXT,
		3,		100,	ALWAYS,			NEXT,
		3,		100,	ALWAYS,			PREVIOUS,
		3,		100,	ALWAYS,			PREVIOUS,
		3,		100,	ALWAYS,			RESTART,
		4,		0,		ALWAYS,			NEXT,
		4,		100,	ALWAYS,			NEXT,
		4,		100,	ALWAYS,			PREVIOUS,
		4,		100,	ALWAYS,			PREVIOUS,
		4,		100,	ALWAYS,			RESTART
};

int tick_no = 0;

//The setup function is called once at startup of the sketch
void setup() {
	Serial.begin(115200);
	sayHello();
	parseCommands();
	initProjectorPins();
	for (int i = 0; i < projector_count; ++i) {
		Serial.print("\n\r");
		Serial.print("projector #");
		Serial.print(i);
		Serial.print("\n\r");
		printProjectorInfo(& projectors[i]);
		Serial.print("\n\r");
	}
	delay(500);
	Timer1.initialize(100000); // set a timer of length 100000 microseconds (or 0.1 sec - or 10Hz)
	Timer1.attachInterrupt(tick); // attach the service routine here
}
void tick() {
	struct Command* current_command;
	for (int i = 0; i < projector_count; ++i) {
		current_command = projectors[i].current_command;
		// if projector should run a command now
		if (projectors[i].last_command_tick + current_command->tick == tick_no) {
			// if the command is to restart
			if (current_command->command == RESTART) {
				current_command = projectors[i].first_command;
				projectors[i].loop_number++;
				// TODO should re-check if the current command should run now
			}
			if (current_command->command == NEXT) {
				digitalWrite(projectors[i].next_pin, HIGH);
				projectors[i].slide_number++;
				// TODO should add a timed task to write the pin to low
			}
			else if (current_command->command == PREVIOUS) {
				digitalWrite(projectors[i].previous_pin, HIGH);
				projectors[i].slide_number--;
				// TODO should add a timed task to write the pin to low
			}
			projectors[i].last_command_tick = tick_no;
			projectors[i].current_command = current_command->next;
		}
	}
	tick_no++;
}
// sets the projector pins to digital outputs
void initProjectorPins() {
	for (int i = 0; i < projector_count; ++i) {
		pinMode(projectors[i].next_pin, OUTPUT);
		pinMode(projectors[i].previous_pin, OUTPUT);
	}
}
/**
 * parses the commands from the global int command_list[]
 */
void parseCommands() {
	int projector_index = -1;
	struct Command* previous_parsed_command = NULL;
	// parse commands
	unsigned int command_list_length = sizeof(command_list)/2;
	Serial.print("command_list_length: ");
	Serial.print(command_list_length);
	Serial.print("\n\r");
	for (unsigned int i = 0; i < command_list_length; i = i + 4) {
		Command* parsed_command = NULL;
		parsed_command = new Command(
				command_list[i + 1],
				command_list[i + 2],
				command_list[i + 3]
			);
		Serial.print("parsed_command: ");
		Serial.print(i);
		Serial.print(", ");
		Serial.print(command_list[i]);
		Serial.print(", ");
		Serial.print(command_list[i + 1]);
		Serial.print(", ");
		Serial.print(command_list[i + 2]);
		Serial.print(", ");
		Serial.print(command_list[i + 3]);
		Serial.print("\n\r");
		// if projector hasn't changed
		if (projector_index == command_list[i]) {
			// save the newly parsed command to be next of previously parsed
			previous_parsed_command->next = parsed_command;
		}
		// first command of a new projector
		else {
			// set this to be the first command of the projector
			projectors[command_list[i]].first_command = parsed_command;
			// save the projector index for future comparison
			projector_index = command_list[i];
		}
		previous_parsed_command = parsed_command;
	}
	// reset the projectors to first command
	for (int i = 0; i < projector_count; ++i) {
		projectors[i].current_command = projectors[i].first_command;
	}
}

void printProjectorInfo(struct Projector * projector) {
	Serial.print("next_pin: ");
	Serial.print(projector->next_pin);
	Serial.print("\n\r");
	Serial.print("previous_pin: ");
	Serial.print(projector->previous_pin);
	Serial.print("\n\r");
	Serial.print("slide_number: ");
	Serial.print(projector->slide_number);
	Serial.print("\n\r");
	Serial.print("loop_number: ");
	Serial.print(projector->loop_number);
	Serial.print("\n\r");
	Serial.print("command list: \n\r");
	struct Command* command = projector->first_command;
	if (command != NULL) {
		while (command != NULL) {
			Serial.print("tick: ");
			Serial.print(command->tick);
			Serial.print(" probability: ");
			Serial.print(command->probability);
			Serial.print(" command: ");
			Serial.print(command->command);
			Serial.print("\n\r");
			command = command->next;
		}
	} else {

		Serial.print("no commands!");
		Serial.print("\n\r");
	}

}

void sayHello() {
	Serial.print("\n\r\n\rHello world\n\r");
	Serial.print("This is UNO speaking\n\r\n\r");
}

// The loop function is called in an endless loop
void loop() {
	Serial.print("\n\rAt tick #");
	Serial.print(tick_no);
	struct Command* current_command;
	for (int i = 0; i < projector_count; ++i) {
		Serial.print("\n\rProjector #");
		Serial.print(i);
		Serial.print(" STATUS:");
		Serial.print("\n\rloop #");
		Serial.print(projectors[i].loop_number);
		Serial.print("\n\rat slide #");
		Serial.print(projectors[i].slide_number);
		current_command = projectors[i].current_command;
		Serial.print("\n\rlast_command_tick: ");
		Serial.print(projectors[i].last_command_tick);
		Serial.print("\n\rnext command tick: ");
		Serial.print(current_command->tick);
		Serial.print(" it will be run at tick #");
		Serial.print(projectors[i].last_command_tick + current_command->tick);
		Serial.print("\n\rnext command is: ");
		if (current_command->command == RESTART) {
			Serial.print("RESTART");
		} else if (current_command->command == NEXT) {
			Serial.print("NEXT");
		} else if (current_command->command == PREVIOUS) {
			Serial.print("PREVIOUS");
		}
	}
	Serial.print("\n\r");
	Serial.print("\n\r");
	delay(1000);
}

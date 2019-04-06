#pragma once
#include "elev.h"
/** 
* @file 
* @brief Module for control and interface of the elevator. 
*/



typedef enum elevator_states {IDLE, RUN, WAIT, STOP, INIT} State; 

typedef struct elevator {
	State state;
	unsigned int currentFloor;
	int orderQueue[10]; // 10 different btns
	unsigned int currentOrder;
	int stopBtn;
	elev_motor_direction_t dir;
	unsigned int numOrders;
	int orderInFloor[4];
	int prio_orderList[4];
} Elev;


/*
* @brief Use this function to run the elevator with all functionality
* @param elev Elev struct
*/
void controller_run(Elev *elev);

/*
* @brief Move the elevator to desired floor
* @param floor Floor number between 0 and 3
*/
void controller_go_to(Elev *elev, int floor);

/*
* @brief If stop button is pressed, change to STOP-state
* @param elev Elev struct
*/
void controller_checkStopBtn(Elev *elev);

/*
* @brief Delay the program for s seconds
* @param s seconds
*/
void controller_delay(unsigned int s, Elev *elev);

/*
* @brief Update "currentFloor" in the Elev struct. CurrentFloor is the last sensor which the elevator passed.
* @param elev Elev struct
*/
void controller_updateCurrentFloor(Elev *elev);



void controller_updateLights(Elev *elev);


void controller_btn_listener(Elev *elev);

void controller_turn_off_lights(void);

int controller_arrivedToFloor(void);

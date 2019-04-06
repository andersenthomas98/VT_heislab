#include "controller.h"
#include "elev.h"
#include "fsm.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>



void controller_run(Elev *elev) {
	fsm_init_seq(elev);
	while (1) {
		//printf("currentOrder = %d\n", elev->currentOrder);
		
		for (int i=0; i<=3; i++) {
			printf("%d, ", elev->prio_orderList[i]);
		}
		printf("\n");
		

		controller_checkStopBtn(elev);
		controller_btn_listener(elev);
		if (elev->state == STOP) {
			fsm_stop_seq(elev);
		//	printf("STOP\n");
		}
		else if (elev->state == IDLE) {
			fsm_idle_seq(elev);
		//	printf("IDLE\n");
		}
		else if (elev->state == RUN) {
			fsm_run_seq(elev);
		//	printf("RUN\n");

		}
		else if (elev->state == WAIT) {
			fsm_wait_seq(elev);
		//	printf("WAIT\n");
		}
	}
}


void controller_go_to(Elev *elev, int floor) {

	controller_updateCurrentFloor(elev);

	if (elev->state == INIT) {
		while (elev_get_floor_sensor_signal() != floor) {
			elev_set_motor_direction(DIRN_DOWN);
		}
		elev_set_motor_direction(DIRN_STOP);
		elev->currentFloor = elev_get_floor_sensor_signal();
	} else {
		controller_updateCurrentFloor(elev);
		if (floor == elev->currentFloor && floor == elev_get_floor_sensor_signal()) {
			//printf("DIRN_STOP\n");
			elev_set_motor_direction(DIRN_STOP);
			elev->state = WAIT;
			elev->dir = DIRN_STOP;
		}
		else if (floor < elev->currentFloor) {
			//printf("DIRN_DOWN\n");
			elev_set_motor_direction(DIRN_DOWN);
			elev->dir = DIRN_DOWN;
		}
		else if (floor > elev->currentFloor) {
			//printf("DIRN_UP\n");
			elev_set_motor_direction(DIRN_UP);
			elev->dir = DIRN_UP;
		}
	}
}


void controller_checkStopBtn(Elev *elev) {
	if (elev_get_stop_signal()) {
		elev->state = STOP;
		elev->dir = DIRN_STOP;
	}
}

void controller_delay(unsigned int s, Elev *elev) {
	int ms = 1000 * s;
	int timestamp = time(NULL);
	while (timestamp + ms < time(NULL)) {
		//controller_btn_listener(elev); // Add order to orderQueue if btn is pressed while door open
		printf("delay"); 
		
	}
}

void controller_updateCurrentFloor(Elev *elev) { // including floor indicator lights
	if (elev->currentFloor != elev_get_floor_sensor_signal() && elev_get_floor_sensor_signal() != -1) {
		if (elev->dir == DIRN_DOWN) {
			elev->currentFloor = elev->currentFloor - 1;
		}
		else if (elev->dir == DIRN_UP) {
			elev->currentFloor = elev->currentFloor + 1;
		}
		elev_set_floor_indicator(elev->currentFloor);
	}
}

void controller_btn_listener(Elev *elev) {
	// orderQueue = {BC0, BC1, BC2, BC3, BCU0, BCU1, BCU2, BCU3, BCD1, BCD2, BCD3}   
	// elements in orderQueue equal TRUE (=1) when active (e.g BC0 = 1 when BUTTON_COMMAND for first floor has been pressed)
	// when the order has been executed the element in the orderQueue is set to FALSE (=0)
	
	// Listen for btn press, and add to orderQueue if btn is pressed
	for (int i=0; i<=3; i++) {
		if (elev_get_button_signal(BUTTON_COMMAND, i)) {
			elev->orderQueue[i] = 1;
			elev_set_button_lamp(BUTTON_COMMAND, i, 1);	
		}
	}
	
	for (int i=4; i<=6; i++) {
		if (elev_get_button_signal(BUTTON_CALL_UP, i-4)) {
			elev->orderQueue[i] = 1;	
			elev_set_button_lamp(BUTTON_CALL_UP, i-4, 1);	
		}
	}

	for (int i=7; i<=9; i++) {
		if (elev_get_button_signal(BUTTON_CALL_DOWN, i-6)) {
			elev->orderQueue[i] = 1;
			elev_set_button_lamp(BUTTON_CALL_DOWN, i-6, 1);	
		}	
	}
}

void controller_turn_off_lights(void) {
	for (int i=0; i<=3; i++) {
		if (elev_get_button_signal(BUTTON_COMMAND, i)) {
			elev_set_button_lamp(BUTTON_COMMAND, i, 0);	
		}
	}
	
	for (int i=4; i<=6; i++) {
		if (elev_get_button_signal(BUTTON_CALL_UP, i-4)) {	
			elev_set_button_lamp(BUTTON_CALL_UP, i-4, 0);	
		}
	}

	for (int i=7; i<=9; i++) {
		if (elev_get_button_signal(BUTTON_CALL_DOWN, i-6)) {
			elev_set_button_lamp(BUTTON_CALL_DOWN, i-6, 0);	
		}	
	}
}



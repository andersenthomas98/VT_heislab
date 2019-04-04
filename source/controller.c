#include "controller.h"
#include "elev.h"
#include "fsm.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>



void controller_run(Elev *elev) {
	fsm_init_seq(elev);
	while (1) {
		
		controller_checkStopBtn(elev);
		controller_btn_listener(elev);
		if (elev->state == STOP) {
			fsm_stop_seq(elev);
			printf("STOP\n");
		}
		else if (elev->state == IDLE) {
			fsm_idle_seq(elev);
		}
		else if (elev->state == RUN) {
			fsm_run_seq(elev);

		}
		else if (elev->state == WAIT) {
			fsm_wait_seq(elev);
		}
	}
}


void controller_go_to(Elev *elev, int floor) {
	if (elev->state == INIT) {
		while (elev_get_floor_sensor_signal() != floor) {
			elev_set_motor_direction(DIRN_DOWN);
		}
		elev_set_motor_direction(DIRN_STOP);
		elev->currentFloor = elev_get_floor_sensor_signal();
	} else {
		controller_updateCurrentFloor(elev);
		if (floor == elev->currentFloor) {
			elev_set_motor_direction(DIRN_STOP);
			elev->state = WAIT;
			elev->dir = DIRN_STOP;
		}
		else if (floor < elev->currentFloor) {
			elev_set_motor_direction(DIRN_DOWN);
			elev->dir = DIRN_DOWN;
		}
		else if (floor > elev->currentFloor) {
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

int arrivedToFloor() {
	if (elev_get_floor_sensor_signal() != -1) {
		return 1;
	}
	return 0;
}

void controller_updateCurrentFloor(Elev *elev) { // including floor indicator lights
	if (elev->currentFloor != elev_get_floor_sensor_signal()) {
		if (elev->dir == DIRN_DOWN && arrivedToFloor()) {
			elev->currentFloor = elev->currentFloor - 1;
		}
		else if (elev->dir == DIRN_UP && arrivedToFloor()) {
			elev->currentFloor = elev->currentFloor + 1;
		}
		elev_set_floor_indicator(elev->currentFloor);
	}
}

void controller_btn_listener(Elev *elev) {
	// Listen for btn press, and add to orderQueue if btn is pressed
	// Kan dette gjøres på en annen måte?
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

/*
void controller_updateOrderLights(Elev *elev) {
	for (int i = 0; i <= 3; i++) {
		elev_set_button_lamp(BUTTON_COMMAND, i, 0);  // Skru av alle lys
	}
	int n = sizeof(elev->orderQueue) / sizeof(int);  // Finner antall elementer i orderQueue.
	for (int j = 0; j <= n; j++) {
		elev_set_button_lamp(BUTTON_COMMAND, elev->orderQueue[j], 1); // Skru på lys for hver etasje som er med i orderQueue.
	}

}*/

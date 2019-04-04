#include "controller.h"
#include "elev.h"
#include "fsm.h"
#include <time.h>
#include <stdio.h>


void controller_run(Elev *elev) {
	fsm_init_seq(elev);
	while (1) {
		controller_checkStopBtn(elev);
		controller_btn_listener(elev);

		if (elev->state == STOP) {
			printf("STOP\n");
			fsm_stop_seq(elev);
		}
		else if (elev->state == IDLE) {
			printf("IDLE\n");
			fsm_idle_seq(elev);
		}
		else if (elev->state == RUN) {
			printf("RUN\n");
			fsm_run_seq(elev);

		}
		else if (elev->state == WAIT) {
			printf("WAIT\n");
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
		}
		else if (floor < elev->currentFloor) {
			elev_set_motor_direction(DIRN_DOWN);
		}
		else if (floor > elev->currentFloor) {
			elev_set_motor_direction(DIRN_UP);
		}
	}
}


void controller_checkStopBtn(Elev *elev) {
	if (elev_get_stop_signal()) {
		elev->state = STOP;
	}
}

void controller_delay(unsigned int s, Elev *elev) {
	int ms = 1000 * s;
	clock_t start_time = clock();
	while (clock() < start_time + ms) {
		controller_btn_listener(elev); // Add order to orderQueue if btn is pressed while door open 
		printf("***delay***\n");	
	}
}

void controller_updateCurrentFloor(Elev *elev) { // including floor indicator lights
	if (elev_get_floor_sensor_signal() != -1) {
		//printf("%d\n", elev->currentFloor);
		if (elev->currentFloor != elev_get_floor_sensor_signal()) {
			elev->currentFloor = elev_get_floor_sensor_signal();
			elev_set_floor_indicator(elev->currentFloor);
		//	printf("New floor\n");
		}
	}
}

void controller_btn_listener(Elev *elev) {
	// Listen for btn press, and add to orderQueue if btn is pressed
	// Kan dette gjøres på en annen måte?
	for (int i=0; i<=3; i++) {
		if (elev_get_button_signal(BUTTON_COMMAND, i)) {
			elev->orderQueue[i] = 1;	
		}
	}
	
	for (int i=4; i<=6; i++) {
		if (elev_get_button_signal(BUTTON_CALL_UP, i-4)) {
			elev->orderQueue[i] = 1;	
		}
	}

	for (int i=7; i<=9; i++) {
		if (elev_get_button_signal(BUTTON_CALL_DOWN, i-6)) {
			elev->orderQueue[i] = 1;
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

 #include "fsm.h"
#include "elev.h"
#include "controller.h"
#include <stdio.h>




void fsm_stop_seq(Elev *elev) {
	elev->stopBtn = 1;
	elev_set_motor_direction(DIRN_STOP);
	elev_set_stop_lamp(1);
	printf("Stop btn pressed\n");
	
	// Remove all orders in orderQueue and orderInFloor
	for (int i=0; i<=9; i++) {
		elev->orderQueue[i] = 0;
		if (i<=3) {
			elev->orderInFloor[i] = 0;
		}	
	}
	
	controller_turn_off_lights();
	
	// If elevator at floor, open door (D.3)/*
	if (elev_get_floor_sensor_signal() != -1) {
		elev_set_door_open_lamp(1);
		while (elev_get_stop_signal()) {}
		controller_delay(3, elev);
		elev_set_door_open_lamp(0);
	} else {
		while (elev_get_stop_signal()) {}
	}
	elev_set_stop_lamp(0);
	elev->state = IDLE;
}

int fsm_init_seq(Elev *elev) {
	elev->state = INIT;

	for (int i=0; i<=3; i++) {
		elev->prio_orderList[i] = -1; // No order = -1
	}

	elev->stopBtn = 0;
	// Initialize hardware
	printf("Initializing...");
	if (!elev_init()) {
		printf("Unable to initialize elevator hardware!\n");
		return 1;
	}

	// Kjør til 1. etasje
	controller_go_to(elev, 0);
	elev->currentFloor = 0;
	elev->state = IDLE;
	printf(" complete.\n");
	return 0;
}

void fsm_wait_seq(Elev *elev) {
	elev_set_motor_direction(DIRN_STOP);

	
	// Remove executed order from orderQueue and orderInFloor, and turn off btn lights
	if (elev->currentOrder == 0) {
		elev->orderInFloor[0] = 0;
		elev->orderQueue[0] = 0;
		elev->orderQueue[4] = 0;
		elev_set_button_lamp(BUTTON_COMMAND, 0, 0);
		elev_set_button_lamp(BUTTON_CALL_UP, 0, 0);
	}
	else if (elev->currentOrder == 1) {
		elev->orderInFloor[1] = 0;
		elev->orderQueue[1] = 0;
		elev->orderQueue[5] = 0;
		elev->orderQueue[7] = 0;
		elev_set_button_lamp(BUTTON_COMMAND, 1, 0);
		elev_set_button_lamp(BUTTON_CALL_UP, 1, 0);
		elev_set_button_lamp(BUTTON_CALL_DOWN, 1, 0);
	}
	else if (elev->currentOrder == 2) {
		elev->orderInFloor[2] = 0;
		elev->orderQueue[2] = 0;
		elev->orderQueue[6] = 0;
		elev->orderQueue[8] = 0;
		elev_set_button_lamp(BUTTON_COMMAND, 2, 0);
		elev_set_button_lamp(BUTTON_CALL_UP, 2, 0);
		elev_set_button_lamp(BUTTON_CALL_DOWN, 2, 0);
	}
	else if (elev->currentOrder == 3) {
		elev->orderInFloor[3] = 0;
		elev->orderQueue[3] = 0;
		elev->orderQueue[9] = 0;
		elev_set_button_lamp(BUTTON_COMMAND, 3, 0);
		elev_set_button_lamp(BUTTON_CALL_DOWN, 3, 0);
	}

	// Move all elements in prio_orderList forward, delete executed order
	for (int i=0; i<=2; i++) {
		elev->prio_orderList[i] = elev->prio_orderList[i+1];
	}
	elev->prio_orderList[3] = -1;


	

	
	
	// Open and close doors
	elev_set_door_open_lamp(1);
	controller_delay(3, elev);
	elev_set_door_open_lamp(0);
	
	// Check if there are more orders to be executed
	for (int i=0; i<=9; i++) {
		if (elev->orderQueue[i] == 1) {
			elev->state = RUN;
			return;		
		} 	
	}
	// No more orders
	elev->state = IDLE;
}

// Check if orderQueue is empty, return 1 if empty, else return 0
int fsm_is_orderQueue_empty(Elev *elev) {
	for (int i=0; i<=9; i++) {
		if (elev->orderQueue[i] == 1) {
			return 0; // There are elements in orderQueue
		}
	}
	return 1; // orderQueue is empty
}

void fsm_update_currentOrder(Elev *elev) {
	int num_of_orders = 0;
	//int priority = 0;

	controller_updateCurrentFloor(elev);

	// orderInFloor = {first_floor, second_floor, third_floor, fourth_floor}
	// If first_floor = 1 --> order elevator to first floor. If there is no order --> first_floor = 0
	// If second_floor = 1 --> order elevator to second floor. If there is no order --> second_floor = 0
	// ...
	for (int i=0; i<=9; i++) {
		if (elev->orderQueue[i] == 1) {

			// First floor
			if (i == 0 || i == 4) {
				elev->orderInFloor[0] = 1;
			}
			// Second floor
			else if (i == 1 || i == 5 || i == 7) {
				elev->orderInFloor[1] = 1;
			}
			// Third floor
			else if (i == 2 || i == 6 || i == 8) {
				elev->orderInFloor[2] = 1;
			}
			// Fourth floor
			else if (i == 3 || i == 9) {
				elev->orderInFloor[3] = 1;
				
			}
		}
	}

	fsm_update_dir(elev);

	if (elev->dir == DIRN_UP) {
		for (int i=0; i<= 3; i++) {
			if (elev->orderInFloor[i] == 1) { // If there are any orders
				num_of_orders++;
				elev->prio_orderList[num_of_orders-1] = i;
			}
		}
	}
	else if (elev->dir == DIRN_DOWN) {
		for (int i=3; i>= 0; i--) {
			if (elev->orderInFloor[i] == 1) { // If there are any orders
				num_of_orders++;
				elev->prio_orderList[num_of_orders-1] = i;
			}
		}
	}

	/*
	 ******************************* PROBLEM ************************************
	 Heis på vei opp til 4. etasje, kjører forbi 2. etasje, får bestilling i 2. etasje -----> IKKE BRA...!!!
	*/


	elev->currentOrder = elev->prio_orderList[0];

}

void fsm_update_dir(Elev *elev) {
	if (elev->currentOrder > elev->currentFloor) {
		elev->dir = DIRN_UP;
	}
	else if (elev->currentOrder < elev->currentFloor) {
		elev->dir = DIRN_DOWN;
	}
}

void fsm_run_seq(Elev *elev) {
	

	fsm_update_currentOrder(elev);

	if (elev->prio_orderList[0] != -1) {

		controller_go_to(elev, elev->currentOrder);

	} 
	else {
		elev->state = IDLE;
	}

	/*
	for (int i=0; i<=9; i++) {
		if (elev->orderQueue[i] == 1) {
			// First floor
			if (i == 0 || i == 4) {
				elev->currentOrder = 0;
				controller_go_to(elev, 0);
			}
			// Second floor
			else if (i == 1 || i == 5 || i == 7) {
				elev->currentOrder = 1;
				controller_go_to(elev, 1);
			}
			// Third floor
			else if (i == 2 || i == 6 || i == 8) {
				elev->currentOrder = 2;
				controller_go_to(elev, 2);
			}
			// Fourth floor
			else if (i == 3 || i == 9) {
				elev->currentOrder = 3;
				controller_go_to(elev, 3);
			}
			else {
				//elev->currentOrder = -1; // No orders to be executed
				//elev->state = IDLE;
			}		
		}
	}
	*/



}

void fsm_idle_seq(Elev *elev) {
	elev_set_motor_direction(DIRN_STOP);
	elev->stopBtn = 0;
	controller_btn_listener(elev);
	for (int i=0; i<=9; i++) {
		if (elev->orderQueue[i] == 1) {
			elev->state = RUN;
		} 
	}

}

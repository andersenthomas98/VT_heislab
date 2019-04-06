#include "controller.h"

/*
Stop elevator.
Next state: IDLE
*/
void fsm_stop_seq(Elev *elev);

/*
Initialize hardware and Elev struct, move to floor 0 (first floor).
Next state: IDLE
*/
int fsm_init_seq(Elev *elev);

/*
Waiting for order.
Next state: RUN
*/
void fsm_idle_seq(Elev *elev);


/*
Move elevator to desired floor.
Next state: WAIT
*/
void fsm_run_seq(Elev *elev);


/* 
The elevator has reached the desired floor.
Open door for three seconds, shut door.
New state: IDLE / RUN (depending on if there is another order)
*/
void fsm_wait_seq(Elev *elev);


void fsm_update_currentOrder(Elev *elev);

int fsm_is_orderQueue_empty(Elev *elev);

void fsm_update_dir(Elev *elev);
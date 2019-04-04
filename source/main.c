#include <stdio.h>
#include "controller.h"

Elev myElevator;


int main() {
    
	controller_run(&myElevator);

    return 0;
}

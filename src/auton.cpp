#include "main.h"
#include "ports.h"
#include "display.h"

static int activeAuton = 0;

void set_active_auton(int id) {
	activeAuton = id;
}

void run_active_auton() {
	chassis.cancelAllMotions();
	switch (activeAuton) {
		case 1:
            auton1();
			break;
		case 2:
			auton2();
			break;
		case 3:
            auton3();
			break;
		case 4:
			auton4();
			break;
	}
}

static void auton1() {
    chassis.moveToPoint(0, 20, 4000);
}

static void auton2() {
    chassis.moveToPoint(0, 1, 4000);
}

static void auton3() {
    chassis.turnToHeading(270, 400000000);
}

static void auton4() {
    chassis.turnToHeading(90, 4000);
}

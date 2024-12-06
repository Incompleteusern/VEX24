#include "main.h"
#include "ports.h"
#include "display.h"

static int activeAuton = 0;

// ASSET(RedRightMidRush_txt)
// ASSET(SixRingSoloWP_txt)
// ASSET(RedLeftAWP_txt)

void set_active_auton(int id) {
	activeAuton = id;
}

static void auton1() {
   //  chassis.follow(RedRightMidRush_txt, 20, 10000);
	chassis.waitUntil(45);
	intake.move(127);
}

static void auton2() {
    // chassis.follow(SixRingSoloWP_txt, 20, 10000);
	}

static void auton3() {
    // chassis.follow(RedLeftAWP_txt, 20, 10000);
}

static void auton4() {
    chassis.turnToHeading(90, 4000);
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

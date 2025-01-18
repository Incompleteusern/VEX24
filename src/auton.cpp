#include "main.h"
#include "ports.h"
#include "display.h"

ASSET(RedRightMidRush_txt)
ASSET(SixRingSoloAWP_txt)
ASSET(RedLeftAWP_txt)

static int activeAuton = 1;

static void auton1() {
	// chassis.setPose(lemlib::Pose(0, 0, 0));
	chassis.moveToPoint(0, 12, 2000);
	chassis.waitUntilDone();
	// pros::delay(1400);

	//intake.move(127);

	// chassis.follow(RedRightMidRush_txt, 20, 10000);
	// intake.move(127);
	// lemlib::MoveToPoseParams backwards = {
	// 	.forwards = false
	// };
	// chassis.moveToPoint(30, 0, 2000);


// 	piston.set_value(1);
	// chassis.moveToPoint(0, 500, 2000);
}

static void auton2() {
    // chassis.follow(SixRingSoloAWP_txt, 20, 10000);
}

static void auton3() {
    // chassis.follow(RedLeftAWP_txt, 20, 10000);
}

static void auton4() {
    // chassis.turnToHeading(90, 4000);
}

void set_active_auton(int id) {
	activeAuton = id;
}


void run_active_auton() {
	auton1();


	/*
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
	*/
}

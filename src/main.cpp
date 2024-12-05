#include "main.h"
#include "ports.h"
#include "display.h"
#include "pros/rtos.h"
#include "auton.h"

pros::Controller controller(CONTROLLER_MASTER);

enum class DriveType
{
    Tank = 0,
    Arcade = 1,
	Curvature = 2
};

static DriveType driveType = DriveType::Tank;

static bool intakeActive = false;
static bool pistonActive = true;

// ladybrown setup and control

static bool ladybrownReady = false;
static bool ladybrownTakeIntake = false;

lemlib::PID ladybrown_pid(1, 0, 2, 0);
lemlib::PID intake_pid(1, 0, 0, 0);

static float intake_goal = 0;
static float ladybrown_goal = 0;


#define distance_min 60

const float ladybrown_values[] = {
	0, 30, 150
};

static bool ladyBrownActive = true;

static lemlib::ExitCondition intake_exit(10, 200);
static lemlib::ExitCondition ladybrown_exit(10, 200);

static int lady_brown_id = 0;

void set_ladybrown_goal(int id) {
	lady_brown_id = id;
	ladybrown_goal = ladybrown_values[id];
}

bool run_pid(float goal, float current, lemlib::PID pid, pros::Motor motor, lemlib::ExitCondition exit) {
	float error = goal - current;
	float output = pid.update(error);
	motor.move(output);
	exit.update(error);

	// TODO proper exit conditions
	if (exit.getExit()) {
		pid.reset();
		motor.move(0);
		return true;
	}

	return false;

}


void updateIntake(bool active, bool toggle) {
	if (!active) {
		intake.move(0);
	} else {
		int sign = toggle ? -1 : 1;
		intake.move(sign * 127);
	}

	intakeActive = active;

}

bool currentCheck(MotorGroup* motorgroup) {
	for (int motorCurrentOver : motorgroup->is_over_current_all()) {
		if (motorCurrentOver >= 1) {
			return true;
		}
	}
	return false;
}

void poll_motor_info() {
	bool currentIssue = false;
	bool tempIssue = false; 
	bool connectionIssue = false;

	double maxTemp = -1;
	
	for (Motor motor : motors) {
		currentIssue |= (motor.is_over_current() == 1);
		tempIssue |= (motor.is_over_temp() == 1);
		connectionIssue |= !motor.is_installed();

		double motorTemp = motor.get_temperature();

		if (motorTemp == PROS_ERR_F) {
			continue;
		} else if (motorTemp > maxTemp) {
			maxTemp = motorTemp;
		}
	}	

	set_motor_info(currentIssue, tempIssue, maxTemp, connectionIssue);	
}

double logDrive(double v, double pow = 2) {
    if (v > 0)
    {
        return (std::pow(std::abs(v), pow) / std::pow(127, pow)) * 127;
    }
    else
    {
        return -1 * (std::pow(std::abs(v), pow) / std::pow(127, pow)) * 127;
    }
}



/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are geblocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	// pros::lcd::initialize();
	pros::delay(50); // lvgl racecondition?
	display_init();
	chassis.calibrate();
	// print_type();

	pros::Task render_task{ [] {
        while (true) {
            poll_motor_info();
			display_tick();
			lemlib::Pose pose = chassis.getPose();
			set_imu_info(pose.x, pose.y, pose.theta);
			set_lady_info(ladybrownReady, ladybrownTakeIntake, distance_sensor.get());
			pros::c::task_delay(100);
        }
    } };

	distance_sensor.calibrate();

	/*
	while (true) {
		if (do_auton_hack) {
			do_auton_hack = false;
			run_active_auton();
		} else if (set_auton_id_hack) {
			set_active_auton(set_auton_id_hack);
		}
		pros::delay(25);
	}
	*/

	// pros::lcd::register_btn1_cb(on_center_button);
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {

}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {
	run_active_auton();
}


/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol() {
	int tick = 0;

	while (true) {
		tick++;
		// TODO REPLACE with millis
	
		if (driveType == DriveType::Tank) {
			int leftY = controller.get_analog(ANALOG_LEFT_Y);
			int rightY = controller.get_analog(ANALOG_RIGHT_Y);

			// move the robot
			chassis.tank(logDrive(leftY), logDrive(rightY));
		} else if (driveType == DriveType::Arcade) {
			int leftY = controller.get_analog(ANALOG_LEFT_Y);
			int rightX = controller.get_analog(ANALOG_RIGHT_X);

			// move the robot
			chassis.arcade(logDrive(leftY), logDrive(rightX));
		} else if (driveType == DriveType::Curvature) {
			// get left y and right x positions
			int leftY = controller.get_analog(ANALOG_LEFT_Y);
			int rightX = controller.get_analog(ANALOG_RIGHT_X);

			// move the robot
			chassis.curvature(logDrive(leftY), logDrive(rightX));
		}
		

		if (controller.get_digital_new_press(DIGITAL_A)) {
			pistonActive = !pistonActive;
			piston.set_value(pistonActive ? 1 : 0);
			add_piston_usage();
		}

		if (ladybrownTakeIntake) {
			bool cancel = run_pid(intake_goal, intake.get_position(), intake_pid, intake, intake_exit);
			if (cancel) {
				ladybrownTakeIntake = false;
				ladybrownReady = false;
			}
		} else {
			if (controller.get_digital(DIGITAL_R1)) {
				updateIntake(true, false);
			} else if (controller.get_digital(DIGITAL_R2)) {
				updateIntake(true, true);
			} else if (intakeActive) {
				updateIntake(false, false);
			}
		}
		
		if (controller.get_digital_new_press(DIGITAL_L1)) {
			set_ladybrown_goal(2);
			ladyBrownActive = true;
		} else if (controller.get_digital_new_press(DIGITAL_L2)) {
			set_ladybrown_goal(0);
			ladyBrownActive = true;
		}

		if (ladyBrownActive) {
			bool cancel = run_pid(ladybrown_goal, lady_potentiometer_sensor.get_value(), ladybrown_pid, ladybrown, ladybrown_exit);
			if (cancel) {
				ladyBrownActive = false;
				if (lady_brown_id == 1) {
					ladybrown.brake();
					TODO LOOK AT
				}
			}
		}


		if (controller.get_digital_new_press(DIGITAL_Y)) {
			// move lady brown to starting position
			ladybrownReady = !ladybrownReady;
			if (ladybrownReady) {
				set_ladybrown_goal(1);
			} else {
				set_ladybrown_goal(0);
			}
			ladyBrownActive = true;
		}

		if (ladybrownReady) {
			if (distance_sensor.get() < distance_min && !ladybrownTakeIntake) {
				ladybrownTakeIntake = true;
				intake_goal = intake.get_position() + 220;
			}
		}



		// delay to save resources
		pros::delay(25);
	}
}
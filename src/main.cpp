#include "main.h"
#include "ports.h"
#include "lvgl.h"

pros::Controller controller(CONTROLLER_MASTER);

enum class DriveType
{
    Tank = 0,
    Arcade = 1,
	Curvature = 2
};

const std::string names[] = {
	"Tank Drive", "Arcade", "Curvature"
};

static DriveType driveType = DriveType::Curvature;
static bool intakeActive = false;
static bool intakeToggle = false;

static bool pistonActive = true;

void print_type() {
		pros::lcd::set_text(1, "Control Type: " + names[static_cast<int>(driveType)]);
}

void cycle_drive_type() {
	switch (driveType) {
		case DriveType::Tank: 
			driveType = DriveType::Arcade;
			break;
		case DriveType::Arcade:
			driveType = DriveType::Curvature;
			break;
		case DriveType::Curvature:
			driveType = DriveType::Tank;
			break;
	}
	print_type();
}

std::string onoff(bool b) {
	return b ? "On" : "Off";
}

void print_intake() {
	// pros::lcd::set_text(2, "Intake Toggle: " + onoff(intakeToggle));
	// pros::lcd::set_text(3, "Intake Active: " + onoff(intakeActive));
}

void updateIntake() {
	if (!intakeActive) {
		intake.move(0);
	} else {
		int sign = intakeToggle ? -1 : 1;
		intake.move(sign * 127);
	}

	print_intake();
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
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */
void on_center_button() {

}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are geblocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	// pros::lcd::initialize();
	print_type();
	print_intake();
	catplush();

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
void autonomous() {}

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
	while (true) {
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
		}

		if (controller.get_digital_new_press(DIGITAL_B)) {
			cycle_drive_type();
		}

		if (controller.get_digital_new_press(DIGITAL_L1)) {
			intakeActive = !intakeActive;
			if (!intakeActive) {
				intakeToggle = false;
			}
			updateIntake();
		} else if (controller.get_digital_new_press(DIGITAL_L2)) {
			intakeToggle = !intakeToggle;
			updateIntake();
		}

		// delay to save resources
		pros::delay(25);

	}

}

#include "main.h"

#define track_width 12.6
#define rpm 450
#define wheel_type lemlib::Omniwheel::NEW_275
#define ANALOG_SENSOR_PORT 'A'

#define FIRST_RIGHT_MOTOR_PORT 11
#define SECOND_RIGHT_MOTOR_PORT 12
#define THIRD_RIGHT_MOTOR_PORT -13

#define FIRST_LEFT_MOTOR_PORT 17
#define SECOND_LEFT_MOTOR_PORT -18
#define THIRD_LEFT_MOTOR_PORT -14

#define INTAKE_PORT -20

#define IMU_PORT 1

#define DISTANCE_PORT 10
#define COLOR_PORT 9
#define POTENTIOMETER_PORT 'C'

pros::adi::DigitalOut piston (ANALOG_SENSOR_PORT);

pros::Motor m1(FIRST_LEFT_MOTOR_PORT, pros::MotorGearset::blue);
pros::Motor m2(SECOND_LEFT_MOTOR_PORT, pros::MotorGearset::blue);
pros::Motor m3(THIRD_LEFT_MOTOR_PORT, pros::MotorGearset::blue);
pros::Motor m4(FIRST_RIGHT_MOTOR_PORT, pros::MotorGearset::blue);
pros::Motor m5(SECOND_RIGHT_MOTOR_PORT, pros::MotorGearset::blue);
pros::Motor m6(THIRD_RIGHT_MOTOR_PORT, pros::MotorGearset::blue);

// pros::Distance distance_sensor(DISTANCE_PORT);
// pros::Optical color_sensor(COLOR_PORT);
// pros::ADIAnalogIn lady_potentiometer_sensor (POTENTIOMETER_PORT);

pros::MotorGroup left_motors({FIRST_LEFT_MOTOR_PORT, SECOND_LEFT_MOTOR_PORT, THIRD_LEFT_MOTOR_PORT}, pros::MotorGearset::blue); 
pros::MotorGroup right_motors({FIRST_RIGHT_MOTOR_PORT, SECOND_RIGHT_MOTOR_PORT, THIRD_RIGHT_MOTOR_PORT}, pros::MotorGearset::blue); 

pros::Motor intake(INTAKE_PORT, pros::MotorGearset::blue);

std::vector<pros::Motor> motors = {m1, m2, m3, m4, m5, m6, intake}; //, ladybrown};

lemlib::Drivetrain drivetrain(&left_motors, // left motor group
                              &right_motors, // right motor group
                              track_width, // 10 inch track width
                              wheel_type, // using new 4" omnis
                              rpm, // drivetrain rpm is 360
                              2 // horizontal drift is 2 (for now)
);

pros::Imu imu(IMU_PORT);

lemlib::OdomSensors sensors(nullptr, // vertical tracking wheel 1, set to null
                            nullptr, // vertical tracking wheel 2, set to nullptr as we are using IMEs
                            nullptr, // horizontal tracking wheel 1
                            nullptr, // horizontal tracking wheel 2, set to nullptr as we don't have a second one
                            &imu // inertial sensor
);

// lateral PID controller
lemlib::ControllerSettings lateral_controller(40, // proportional gain (kP)
                                              0, // integral gain (kI)
                                              2, // derivative gain (kD)
                                              0, // anti windup
                                              1, // small error range, in inches
                                              200, // small error range timeout, in milliseconds
                                              3, // large error range, in inches
                                              500, // large error range timeout, in milliseconds
                                              0 // maximum acceleration (slew)
);

// angular PID controller
lemlib::ControllerSettings angular_controller(30, // proportional gain (kP)
                                              0, // integral gain (kI)
                                              5, // derivative gain (kD)
                                              0, // anti windup
                                              1, // small error range, in inches
                                              200, // small error range timeout, in milliseconds
                                              3, // large error range, in inches
                                              500, // large error range timeout, in milliseconds
                                              0 // maximum acceleration (slew)
);

lemlib::Chassis chassis(drivetrain, // drivetrain settings
                        lateral_controller, // lateral PID settings
                        angular_controller, // angular PID settings
                        sensors // odometry sensors
);
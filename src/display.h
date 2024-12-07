#include "liblvgl/lvgl.h"

extern void display_init();
// extern void set_control_text(const std::string text);
extern void add_piston_usage();
extern void display_tick();
extern void set_motor_info(bool currentIssue, bool tempIssue, double maxTemp, bool connectionIssue);
extern void set_imu_info(float x, float y, float theta);
extern void set_lady_info(bool ladybrown, bool ladybrownTake, float distance);
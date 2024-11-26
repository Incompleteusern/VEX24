#include "liblvgl/lvgl.h"

extern void display_init();
extern void set_intake_text(bool on, bool inverse);
// extern void set_control_text(const std::string text);
extern void add_piston_usage();
extern void display_tick();
extern void set_motor_info(bool currentIssue, bool tempIssue, double maxTemp);
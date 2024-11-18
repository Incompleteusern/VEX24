// todo
// auton selection
// stats during actual thing? would be cool

#include "main.h"
#include "_main.h"
#include "pros/apix.h"
#include "liblvgl/lvgl.h"

// From catpuccin
#define cat_rosewater 0xf5e0dc
#define cat_flamingo 0xf2cdcd
#define cat_pink 0xf5c2e7
#define cat_mauve 0xf2cdcd
#define cat_red 0xf38ba8
#define cat_maroon 0xeba0ac
#define cat_peach 0xfab387
#define cat_yellow 0xf9e2af
#define cat_green 0xa6e3a1
#define cat_teal 0x94e2d5
#define cat_sky 0x89dceb
#define cat_sapphire 0x74c7ec
#define cat_blue 0x89b4fa
#define cat_lavender 0xb4befe
#define cat_text 0xcdd6f4
#define cat_subtext1 0xbac2de
#define cat_subtext0 0xa6adc8
#define cat_overlay2 0x9399b2
#define cat_overlay1 0x7f849c
#define cat_overlay0 0x6c7086
#define cat_surface2 0x585b70
#define cat_surface1 0x45475a
#define cat_surface0 0x313244
#define cat_base 0x1e1e2e
#define cat_mantle 0x181825
#define cat_crust 0x11111b


LV_IMG_DECLARE(catplush);

static lv_obj_t * catplush_img;

static lv_style_t style_btn_pressed;
static lv_style_t style_btn_red;

static lv_style_t style_obj;
static lv_style_t style_btn;

static lv_style_t style_base;
static lv_style_t style_mantle;
static lv_style_t style_crust;


static lv_color_t darken(const lv_color_filter_dsc_t * dsc, lv_color_t color, lv_opa_t opa)
{
    LV_UNUSED(dsc);
    return lv_color_darken(color, opa);
}

/*
static void event_cb(lv_event_t * e)
{
    LV_LOG_USER("Clicked");

    cycle_drive_type();
}
*/

// static lv_obj_t * labels[5];
// static bool labels_init = false;

// static lv_obj_t * controlType;
static lv_obj_t * intakeToggle;
static lv_obj_t * intakeActive;
static lv_obj_t * pistonUses;
static lv_obj_t * motorMaxTemp;
static lv_obj_t * motorOverTemp;
static lv_obj_t * motorOverCurrent;

static lv_span_t * intakeToggleOnOff;
static lv_span_t * intakeActiveOnOff;
static lv_span_t * motorMaxTempNum;
static lv_span_t * motorOverTempBool;
static lv_span_t * motorOverCurrentBool;

static lv_span_t * pistonUsesCount;



/*
void init_labels() {
    for (int i = 0; i < 5; i++) {
        lv_obj_t * label = lv_label_create(lv_scr_act());
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 40 + 20*i);
        labels[i] = label;
    }

    labels_init = true;
}
*/

void updateSpanOnOff(lv_span_t* span, bool on) {
    if (on) {
        lv_style_set_text_color(&span->style, lv_color_hex(cat_green));
        lv_span_set_text(span, "On");
    } else {
        lv_style_set_text_color(&span->style, lv_color_hex(cat_red));
        lv_span_set_text(span, "Off");
    }
}

void updateSpanOnCheck(lv_span_t* span, bool issue) {
    if (issue) {
        lv_style_set_text_color(&span->style, lv_color_hex(cat_red));
        lv_span_set_text(span, "Bad");
    } else {
        lv_style_set_text_color(&span->style, lv_color_hex(cat_green));
        lv_span_set_text(span, "Good");
    }
}

void updateSpanTemp(lv_span_t* span, float temp) {
    float maxTemp = 55;
    float minTemp = 20;
    float lerp = std::clamp((temp - maxTemp)/(maxTemp - minTemp), 0.0f, 1.0f);

    lv_style_set_text_color(&span->style, lv_color_hex((int) (cat_red * (1-lerp) + cat_green * lerp)));
    lv_span_set_text(span, (std::to_string(temp) + "C").c_str());
}


/*
void set_control_text(const std::string text) {
    lv_label_set_text(controlType, text.c_str());
    lv_obj_align(controlType, LV_ALIGN_CENTER, 0, 0);
}
*/

void set_intake_text(bool on, bool inverse) {
    updateSpanOnOff(intakeActiveOnOff, on);
    updateSpanOnOff(intakeToggleOnOff, inverse);
    lv_obj_align(intakeActive, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_align(intakeToggle, LV_ALIGN_LEFT_MID, 0, 0);
}

void set_motor_info(bool currentIssue, bool tempIssue, int maxTemp) {
    updateSpanOnCheck(motorOverCurrentBool, currentIssue);
    updateSpanOnCheck(motorOverTempBool, tempIssue);
    updateSpanTemp(motorMaxTempNum, maxTemp);
    lv_obj_align(motorOverCurrent, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_align(motorOverTemp, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_align(motorMaxTemp, LV_ALIGN_LEFT_MID, 0, 0);
}

static int piston_usage = 0;

void add_piston_usage() {
    lv_span_set_text(pistonUsesCount, std::to_string(++piston_usage).c_str());
}


/*
void set_text(int i, const std::string text) {
    if (i > 5 || i < 0 || !labels_init) {
        return;
    }

    lv_label_set_text(labels[i-1], text.c_str());
}
*/

lv_style_t style_bg_color(uint32_t hex) {
    lv_style_t style;
    lv_style_init(&style);
    lv_style_set_bg_color(&style, lv_color_hex(hex));
    return style;
}



void draw_catplush() {
    catplush_img = lv_img_create(lv_scr_act());
    lv_img_set_src(catplush_img, &catplush);
    lv_obj_align(catplush_img, LV_ALIGN_CENTER, 0, -75);
    lv_img_set_zoom(catplush_img, 512);

    // lv_obj_t * img2 = lv_img_create(lv_scr_act());
    // lv_img_set_src(img2, LV_SYMBOL_OK "Accept");
    // lv_obj_align_to(img2, img1, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
}

static void style_init(void)
{
    /*Create a simple button style*/
    lv_style_init(&style_btn);
    lv_style_set_radius(&style_btn, 10);
    lv_style_set_bg_opa(&style_btn, LV_OPA_COVER);
    lv_style_set_bg_color(&style_btn, lv_palette_lighten(LV_PALETTE_GREY, 3));
    lv_style_set_bg_grad_color(&style_btn, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_bg_grad_dir(&style_btn, LV_GRAD_DIR_VER);

    lv_style_set_border_color(&style_btn, lv_color_black());
    lv_style_set_border_opa(&style_btn, LV_OPA_20);
    lv_style_set_border_width(&style_btn, 2);

    lv_style_set_text_color(&style_btn, lv_color_black());

    /*Create a style for the pressed state.
     *Use a color filter to simply modify all colors in this state*/
    static lv_color_filter_dsc_t color_filter;
    lv_color_filter_dsc_init(&color_filter, darken);
    lv_style_init(&style_btn_pressed);
    lv_style_set_color_filter_dsc(&style_btn_pressed, &color_filter);
    lv_style_set_color_filter_opa(&style_btn_pressed, LV_OPA_20);

    /*Create a red style. Change only some colors.*/
    lv_style_init(&style_btn_red);
    lv_style_set_bg_color(&style_btn_red, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_bg_grad_color(&style_btn_red, lv_palette_lighten(LV_PALETTE_RED, 3));
}



void add_buttons() {
        /*Initialize the style*/
    style_init();


    /*Create a button and use the new styles*/
    lv_obj_t * btn = lv_btn_create(lv_scr_act());
    /* Remove the styles coming from the theme
     * Note that size and position are also stored as style properties
     * so lv_obj_remove_style_all will remove the set size and position too */
    // lv_obj_remove_style_all(btn);
    lv_obj_set_pos(btn, 10, 10);
    lv_obj_set_size(btn, 120, 50);
    lv_obj_add_style(btn, &style_btn, 0);
    lv_obj_add_style(btn, &style_btn_pressed, LV_STATE_PRESSED);
    //lv_obj_add_event_cb(btn, event_cb, LV_EVENT_PRESSED, NULL);
    
    /*Add a label to the button*/
    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, "Button");
    lv_obj_center(label);

    /*Create another button and use the red style too*/
    lv_obj_t * btn2 = lv_btn_create(lv_scr_act());
    lv_obj_remove_style_all(btn2);                      /*Remove the styles coming from the theme*/
    lv_obj_set_pos(btn2, 10, 80);
    lv_obj_set_size(btn2, 120, 50);
    lv_obj_add_style(btn2, &style_btn, 0);
    lv_obj_add_style(btn2, &style_btn_red, 0);
    lv_obj_add_style(btn2, &style_btn_pressed, LV_STATE_PRESSED);
    lv_obj_set_style_radius(btn2, LV_RADIUS_CIRCLE, 0); /*Add a local style too*/

    label = lv_label_create(btn2);
    lv_label_set_text(label, "Button 2");
    lv_obj_center(label);
}

/**
 * Create styles from scratch for buttons.
 */
void display_init(void)
{
    draw_catplush();
    // init_labels();

    style_base = style_bg_color(cat_base);
    style_crust = style_bg_color(cat_crust);
    style_mantle = style_bg_color(cat_mantle);

    lv_obj_add_style(lv_scr_act(), &style_base, 0);

    lv_style_init(&style_obj);
    lv_style_set_pad_all(&style_obj, 10);
    lv_style_set_border_color(&style_obj, lv_color_hex(cat_lavender)); // lavender
    lv_style_set_text_color(&style_obj, lv_color_hex(cat_text)); // text

    lv_style_init(&style_btn);
    lv_style_set_border_color(&style_btn, lv_color_hex(cat_lavender)); // lavender
    lv_style_set_text_color(&style_btn, lv_color_hex(cat_text)); // text
    lv_style_set_bg_color(&style_btn, lv_color_hex(cat_surface0));

    int h = 140;
    int d = 25;

    

    lv_obj_t * cont = lv_obj_create(lv_scr_act());
    lv_obj_add_style(cont, &style_mantle, 0);
    lv_obj_add_style(cont, &style_obj, 0);
    lv_obj_set_size(cont, 430, h);
    lv_obj_align(cont, LV_ALIGN_CENTER, 0, h/2-30);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // controlType = lv_label_create(cont);
    // lv_obj_align(controlType, LV_ALIGN_CENTER, -30, 40);

    lv_obj_t * subconts[2];

    for (int i = 0; i < 2; i++) {
        lv_obj_t * subcont = lv_obj_create(cont);
        lv_obj_add_style(subcont, &style_obj, 0);
        lv_obj_add_style(subcont, &style_base, 0);
        lv_obj_set_height(subcont, h-d);
        lv_obj_set_flex_flow(subcont, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(subcont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_flex_grow(subcont, 1);           /*1 portion from the free space*/
        subconts[i] = subcont;
    }

    // lv_obj_set_flex_align(subconts[1], LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);

    static lv_style_t style_span;
    lv_style_init(&style_span);
    lv_style_set_width(&style_span, 160);
    lv_style_set_height(&style_span, 20);

    lv_obj_t * motorInfo = lv_label_create(subconts[0]);
    lv_label_set_text(motorInfo, "Motor Info");
    lv_obj_align(motorInfo, LV_FLEX_ALIGN_CENTER, 0, 0);
    lv_label_set_long_mode(motorInfo, LV_LABEL_LONG_WRAP);

    motorOverTemp = lv_spangroup_create(subconts[0]);
    lv_obj_add_style(motorOverTemp, &style_span, 0);
    lv_obj_align(motorOverTemp, LV_FLEX_ALIGN_CENTER, 0, 0);
    lv_label_set_long_mode(motorOverTemp, LV_LABEL_LONG_WRAP);
    lv_span_t * spanMotorOverTemp = lv_spangroup_new_span(motorOverTemp);
    lv_span_set_text(spanMotorOverTemp, "Motor Temp: ");
    motorOverTempBool = lv_spangroup_new_span(motorOverTemp);
    updateSpanOnCheck(motorOverTempBool, false);

    motorOverCurrent = lv_spangroup_create(subconts[0]);
    lv_obj_add_style(motorOverCurrent, &style_span, 0);
    lv_obj_align(motorOverCurrent, LV_FLEX_ALIGN_CENTER, 0, 0);
    lv_label_set_long_mode(motorOverCurrent, LV_LABEL_LONG_WRAP);
    lv_span_t * spanMotorOverCurrent = lv_spangroup_new_span(motorOverCurrent);
    lv_span_set_text(spanMotorOverCurrent, "Motor Current: ");
    motorOverCurrentBool = lv_spangroup_new_span(motorOverCurrent);
    updateSpanOnCheck(motorOverCurrentBool, false);

    motorMaxTemp = lv_spangroup_create(subconts[1]);
    lv_obj_add_style(motorMaxTemp, &style_span, 0);
    lv_obj_align(motorMaxTemp, LV_FLEX_ALIGN_CENTER, 0, 0);
    lv_label_set_long_mode(motorMaxTemp, LV_LABEL_LONG_WRAP);
    lv_span_t * spanMotorMaxTemp = lv_spangroup_new_span(motorMaxTemp);
    lv_span_set_text(spanMotorMaxTemp, "Max Motor Temp: ");
    motorMaxTempNum = lv_spangroup_new_span(motorMaxTemp);
    lv_span_set_text(motorMaxTempNum, "??");
    lv_style_set_text_color(&motorMaxTempNum->style, lv_color_hex(cat_teal));

    updateSpanTemp(intakeToggleOnOff, false);

    /*
    lv_obj_t * btn = lv_btn_create(subconts[0]);
    lv_obj_set_size(btn, 130, 40);
    lv_obj_add_style(btn, &style_btn, 0);
    lv_obj_add_style(btn, &style_btn_pressed, LV_STATE_PRESSED);
    lv_obj_add_event_cb(btn, event_cb, LV_EVENT_PRESSED, NULL);
    */

    /*Add a label to the button*/
    /*
    controlType = lv_label_create(btn);
    lv_label_set_text(controlType, "");
    lv_obj_center(controlType);
    */

    intakeActive = lv_spangroup_create(subconts[1]);
    lv_obj_add_style(intakeActive, &style_span, 0);
    lv_obj_align(intakeActive, LV_FLEX_ALIGN_CENTER, 0, 0);
    lv_label_set_long_mode(intakeActive, LV_LABEL_LONG_WRAP);
    lv_span_t * spanIntakeActive = lv_spangroup_new_span(intakeActive);
    lv_span_set_text(spanIntakeActive, "Intake Active: ");
    intakeActiveOnOff = lv_spangroup_new_span(intakeActive);
    updateSpanOnOff(intakeActiveOnOff, false);

    intakeToggle = lv_spangroup_create(subconts[1]);
    lv_obj_add_style(intakeToggle, &style_span, 0);
    lv_obj_align(intakeToggle, LV_FLEX_ALIGN_CENTER, 0, 0);
    lv_label_set_long_mode(intakeToggle, LV_LABEL_LONG_WRAP);
    lv_span_t * spanIntakeToggle = lv_spangroup_new_span(intakeToggle);
    lv_span_set_text(spanIntakeToggle, "Intake Toggle: ");
    intakeToggleOnOff = lv_spangroup_new_span(intakeToggle);
    updateSpanOnOff(intakeToggleOnOff, false);

    // color lerping lol

    pistonUses = lv_spangroup_create(subconts[1]);
    lv_obj_add_style(pistonUses, &style_span, 0);
    lv_obj_align(pistonUses, LV_FLEX_ALIGN_CENTER, 0, 0);
    lv_label_set_long_mode(pistonUses, LV_LABEL_LONG_WRAP);
    lv_span_t * spanPistonUses = lv_spangroup_new_span(pistonUses);
    lv_span_set_text(spanPistonUses, "Piston Uses: ");
    pistonUsesCount = lv_spangroup_new_span(pistonUses);
    lv_span_set_text(pistonUsesCount, "0");
    lv_style_set_text_color(&pistonUsesCount->style, lv_color_hex(cat_teal));
}

static int rotation = 0;

void display_tick() {
    rotation += 50;
    lv_img_set_angle(catplush_img, rotation);
}

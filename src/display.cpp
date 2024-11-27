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
static lv_style_t style_span;

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

struct span_label {
    lv_obj_t * spangroup;
    lv_span_t * spanlabel;
    lv_span_t * spanvalue;
};


static span_label intakeToggle;
static span_label intakeActive;
static span_label pistonUses;
static span_label motorMaxTemp;
static span_label motorOverTemp;
static span_label motorOverCurrent;
static span_label motorNotConnected;



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

void updateSpanTemp(lv_span_t* span, double temp) {
    if (temp < 0) {
        lv_style_set_text_color(&span->style, lv_color_hex(cat_peach));
        lv_span_set_text(motorMaxTemp.spanvalue, "???");
        return;
    }
    float maxTemp = 55;
    float minTemp = 30;
    float lerp = std::clamp((temp - minTemp)/(maxTemp - minTemp), 0.0, 1.0);

    lv_style_set_text_color(&span->style, lv_color_hex((int) (cat_green * (1-lerp) + cat_red * lerp)));
    lv_span_set_text(motorMaxTemp.spanvalue, (std::to_string((int) temp) + "C").c_str());
}


/*1
void set_control_text(const std::string text) {
    lv_label_set_text(controlType, text.c_str());
    lv_obj_align(controlType, LV_ALIGN_CENTER, 0, 0);
}
*/

void set_intake_text(bool on, bool inverse) {
    updateSpanOnOff(intakeActive.spanvalue, on);
    updateSpanOnOff(intakeToggle.spanvalue, inverse);
    lv_obj_align(intakeActive.spangroup, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_align(intakeToggle.spangroup, LV_ALIGN_LEFT_MID, 0, 0);
}

void set_motor_info(bool currentIssue, bool tempIssue, double maxTemp, bool connectionIssue) {
    updateSpanOnCheck(motorOverCurrent.spanvalue, currentIssue);
    updateSpanOnCheck(motorOverTemp.spanvalue, tempIssue);
    updateSpanOnCheck(motorNotConnected.spanvalue, connectionIssue);
    updateSpanTemp(motorMaxTemp.spanvalue, maxTemp);
    lv_obj_align(motorOverCurrent.spangroup, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_align(motorOverTemp.spangroup, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_align(motorMaxTemp.spangroup, LV_ALIGN_LEFT_MID, 0, 0);
}

static int piston_usage = 0;

void add_piston_usage() {
    lv_span_set_text(pistonUses.spanvalue, std::to_string(++piston_usage).c_str());
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



/*
static void style_init(void)
{
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

    static lv_color_filter_dsc_t color_filter;
    lv_color_filter_dsc_init(&color_filter, darken);
    lv_style_init(&style_btn_pressed);
    lv_style_set_color_filter_dsc(&style_btn_pressed, &color_filter);
    lv_style_set_color_filter_opa(&style_btn_pressed, LV_OPA_20);

    lv_style_init(&style_btn_red);
    lv_style_set_bg_color(&style_btn_red, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_bg_grad_color(&style_btn_red, lv_palette_lighten(LV_PALETTE_RED, 3));
}
*/


void init_styles() {
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

    lv_style_init(&style_span);
    lv_style_set_width(&style_span, 160);
    lv_style_set_height(&style_span, 18);
    lv_style_set_pad_top(&style_span, 0);
    lv_style_set_pad_bottom(&style_span, 0);
    lv_style_set_border_width(&style_span, 0);

}

span_label create_span_label(const char *text, lv_obj_t* parent) {
    span_label label;
    label.spangroup = lv_spangroup_create(parent);
    lv_obj_add_style(label.spangroup, &style_span, 0);
    lv_obj_align(label.spangroup, LV_FLEX_ALIGN_CENTER, 0, 0);
    lv_label_set_long_mode(label.spangroup, LV_LABEL_LONG_WRAP);
    label.spanlabel = lv_spangroup_new_span(label.spangroup);
    lv_span_set_text(label.spanlabel, text);
    label.spanvalue = lv_spangroup_new_span(label.spangroup);

    return label;
}

void info_display(lv_obj_t* parent) {
    int h = 180;
    int d = 25;

    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_add_style(cont, &style_mantle, 0);
    lv_obj_add_style(cont, &style_obj, 0);
    lv_obj_set_size(cont, 430, h);
    lv_obj_align(cont, LV_ALIGN_CENTER, 0, 5);
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
        lv_obj_set_flex_grow(subcont, 1);        
        subconts[i] = subcont;
    }

    // lv_obj_set_flex_align(subconts[1], LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);

    /*
    lv_obj_t * motorInfo = lv_label_create(subconts[0]);
    lv_label_set_text(motorInfo, "Motor Info");
    lv_obj_align(motorInfo, LV_FLEX_ALIGN_CENTER, 0, 0);
    lv_label_set_long_mode(motorInfo, LV_LABEL_LONG_WRAP);
    */

    motorOverTemp = create_span_label("Temperature: ", subconts[0]);
    updateSpanOnCheck(motorOverTemp.spanvalue, false);

    motorOverCurrent = create_span_label("Current: ", subconts[0]);
    updateSpanOnCheck(motorOverCurrent.spanvalue, false);

    motorNotConnected = create_span_label("Connected: ", subconts[0]);
    updateSpanOnCheck(motorNotConnected.spanvalue, false);

    motorMaxTemp = create_span_label("Max Motor Temp: ", subconts[0]);
    lv_span_set_text(motorMaxTemp.spanvalue, "??");
    lv_style_set_text_color(&motorMaxTemp.spanvalue->style, lv_color_hex(cat_teal));
    updateSpanTemp(motorMaxTemp.spanvalue, false);

    intakeActive = create_span_label("Intake Active: ", subconts[1]);
    updateSpanOnOff(intakeActive.spanvalue, false);

    intakeToggle = create_span_label("Intake Toggle: ", subconts[1]);
    updateSpanOnOff(intakeToggle.spanvalue, false);

    pistonUses = create_span_label("Piston Uses: ", subconts[1]);
    lv_span_set_text(pistonUses.spanvalue, "0");
    lv_style_set_text_color(&pistonUses.spanvalue->style, lv_color_hex(cat_teal));
}

void draw_catplush(lv_obj_t* parent) {
    catplush_img = lv_img_create(parent);
    lv_img_set_src(catplush_img, &catplush);
    lv_obj_align(catplush_img, LV_ALIGN_CENTER, 0, 0);
    lv_img_set_zoom(catplush_img, 1024);

    // lv_obj_t * img2 = lv_img_create(lv_scr_act());
    // lv_img_set_src(img2, LV_SYMBOL_OK "Accept");
    // lv_obj_align_to(img2, img1, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
}


static int rotation = 0;

void display_tick() {
    rotation += 50;
    lv_img_set_angle(catplush_img, rotation);
}

void display_init(void)
{
    // init_labels();

    init_styles();

    lv_obj_t *tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 30);

    lv_obj_t* tab_info = lv_tabview_add_tab(tabview, "Info");
    lv_obj_t* tab_image = lv_tabview_add_tab(tabview, "Fun");
    lv_obj_t* tab_auton = lv_tabview_add_tab(tabview, "Auton");

    draw_catplush(tab_image);
    info_display(tab_info);
}

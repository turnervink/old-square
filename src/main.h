#include <pebble.h>
#pragma once

// Define app keys
#define KEY_TEXT_COLOR 0
#define KEY_INVERT_COLORS 1
#define KEY_TEMPERATURE 2
#define KEY_TEMPERATURE_IN_C 3
#define KEY_CONDITIONS 4
#define KEY_SHAKE_FOR_WEATHER 5
#define KEY_USE_CELSIUS 6
#define KEY_BACKGROUND_COLOR 7
#define KEY_SHOW_WEATHER 8
#define KEY_VIBE_ON_DISCONNECT 9
#define KEY_VIBE_ON_CONNECT 10
#define KEY_REFLECT_BATT 12
#define KEY_DATE_FORMAT 13
#define KEY_LANGUAGE 14
#define KEY_READY 15
#define KEY_LARGE_FONT 16
#define KEY_SHOW_SECONDS 17

Window *main_window;

// Config options
bool ready;
bool invert_colors;
bool use_celsius;
bool shake_for_weather;
bool show_weather;
bool vibe_on_disconnect;
bool vibe_on_connect;
bool reflect_batt;
bool euro_date;
bool large_font;
bool picked_font;
bool show_seconds;

// App message setup
void init_appmessage();
void inbox_received_handler(DictionaryIterator *iter, void *context);
void inbox_dropped_callback(AppMessageResult reason, void *context);
void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context);
void outbox_sent_callback(DictionaryIterator *iterator, void *context);

// Language settings
void sendLang(char* lang);
int lang;
//static int lang = 0; // Hardcoded for testing

// Color setting functions
void set_text_color(int color);
void set_background_color(int bgcolor);
void inverter();

// Weather TextLayers
TextLayer *time_layer, *temp_layer, *conditions_layer, *temp_layer_unanimated, *conditions_layer_unanimated;

// Weather GFonts
GFont weather_font, bt_font, date_font, time_font, small_time_font;

// Update functions
void update_layers();
void update_time();
void update_weather();

void tick_handler(struct tm *tick_time, TimeUnits units_changed);
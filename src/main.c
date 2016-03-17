#include <pebble.h>
#include "languages.h"
#include "main.h"
#include "battery.h"
	
Window *s_main_window;

static TextLayer *s_time_layer, *s_date_layer, *s_bluetooth_layer;
TextLayer *s_temp_layer, *s_conditions_layer, *s_temp_layer_unanimated, *s_conditions_layer_unanimated, *s_charge_layer;

static GFont s_time_font, s_date_font;
GFont s_weather_font, s_bt_font;

static Layer  *s_weather_layer, *s_weather_layer_unanimated;
Layer *s_batt_layer, *s_static_layer;

// Config options
bool ready = 0;
bool invert_colors = 0;
bool use_celsius = 0;
bool shake_for_weather = 1;
bool show_weather = 1;
bool vibe_on_disconnect = 1;
bool vibe_on_connect = 1;
bool reflect_batt = 1;
bool euro_date = 0;
bool large_font = 0;
bool picked_font = 0;

int lang; // User selected language code

void on_animation_stopped(Animation *anim, bool finished, void *context) {
    //Free the memory used by the Animation
    property_animation_destroy((PropertyAnimation*) anim);
}

static void bluetooth_handler(bool connected) {
	if (!connected) {
		layer_set_hidden(text_layer_get_layer(s_bluetooth_layer), false);
		if (vibe_on_disconnect == 1) {
			vibes_long_pulse();
		}
	} else {
		layer_set_hidden(text_layer_get_layer(s_bluetooth_layer), true);
		if (vibe_on_connect == 1) {
			vibes_double_pulse();
		}
	}
}
 
void animate_layer(Layer *layer, GRect *start, GRect *finish, int duration, int delay) {
    //Declare animation
    PropertyAnimation *anim = property_animation_create_layer_frame(layer, start, finish);
 
    //Set characteristics
    animation_set_duration((Animation*) anim, duration);
    animation_set_delay((Animation*) anim, delay);
 
    //Set stopped handler to free memory
    AnimationHandlers handlers = {
        //The reference to the stopped handler is the only one in the array
        .stopped = (AnimationStoppedHandler) on_animation_stopped
    };
    animation_set_handlers((Animation*) anim, handlers, NULL);
 
    //Start animation!
    animation_schedule((Animation*) anim);
}

static void animate_layers() {
	GRect bounds = layer_get_bounds(window_get_root_layer(s_main_window));
	GSize cond_size = text_layer_get_content_size(s_conditions_layer);
	GSize temp_size = text_layer_get_content_size(s_temp_layer);
	
	

	// Weather moves in from bottom
	GRect wins = GRect(0, bounds.size.h + cond_size.h, bounds.size.w, cond_size.h);
	GRect winf = GRect(0, PBL_IF_ROUND_ELSE(bounds.size.h - 55, (bounds.size.h - cond_size.h) - 5), bounds.size.w, cond_size.h);
	
	animate_layer(text_layer_get_layer(s_conditions_layer), &wins, &winf, 1000, 0);

	GRect wouts = GRect(0, PBL_IF_ROUND_ELSE(bounds.size.h - 55, (bounds.size.h - cond_size.h) - 5), bounds.size.w, cond_size.h);
	GRect woutf = GRect(0, bounds.size.h + 10, bounds.size.w, cond_size.h);
	
	animate_layer(text_layer_get_layer(s_conditions_layer), &wouts, &woutf, 1000, 5000);

	// Temp moves in from top
	GRect tins = GRect(0, -32, bounds.size.w, temp_size.h);
	GRect tinf = GRect(0, PBL_IF_ROUND_ELSE(40, 0), bounds.size.w, temp_size.h);
	
	animate_layer(text_layer_get_layer(s_temp_layer), &tins, &tinf, 1000, 0);

	GRect touts = GRect(0, PBL_IF_ROUND_ELSE(40, 0), bounds.size.w, temp_size.h);
	GRect toutf = GRect(0, -32, bounds.size.w, temp_size.h);
	
	animate_layer(text_layer_get_layer(s_temp_layer), &touts, &toutf, 1000, 5000);
}

void update_time() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
	
  static char time_buffer[] = "00:00";
  static char datn_buffer[] = "DD"; // Buffer for date number
  static char date_buffer[] = "WWW MMM DD"; // Buffer for entire date to display
  
  if(clock_is_24h_style() == true) {
    strftime(time_buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    strftime(time_buffer, sizeof("00:00"), "%I:%M", tick_time);
  }
	
	text_layer_set_text(s_time_layer, time_buffer);

	strftime(datn_buffer, sizeof("DD"), "%d", tick_time); // Write current date to buffer
	int month = tick_time->tm_mon; // Get current month as an integer
	int weekday = tick_time->tm_wday; // Get current weekday as an integer (0 is Sunday)
	
	// Select the correct strings from languages.c and write to buffer along with date
	if (euro_date == 1) { // If the user has selected the WWW DD MMM date format
		snprintf(date_buffer, sizeof(date_buffer), "%s %s %s", dayNames[lang][weekday], datn_buffer, monthNames[lang][month]);
	} else {
		snprintf(date_buffer, sizeof(date_buffer), "%s %s %s", dayNames[lang][weekday], monthNames[lang][month], datn_buffer);
	}

	text_layer_set_text(s_date_layer, date_buffer); // Display the date info
}

void update_layers() {
	if (show_weather == 0) {
		layer_set_hidden(s_weather_layer, true);
		layer_set_hidden(s_weather_layer_unanimated, true);
	} else {
		if (shake_for_weather == 0) {
			layer_set_hidden(s_weather_layer, true);
			layer_set_hidden(s_weather_layer_unanimated, false);
		} else {
			layer_set_hidden(s_weather_layer, false);
			layer_set_hidden(s_weather_layer_unanimated, true);
		}
	}

	if (reflect_batt == 1) {
		layer_set_hidden(s_static_layer, true);
		layer_set_hidden(s_batt_layer, false);
  	} else {
		layer_set_hidden(s_static_layer, false);
		layer_set_hidden(s_batt_layer, true);
  	}
}

void set_text_color(int color) {
  #ifdef PBL_COLOR
		GColor text_color = GColorFromHEX(color);
		text_layer_set_text_color(s_time_layer, text_color);
		text_layer_set_text_color(s_date_layer, text_color);
		text_layer_set_text_color(s_temp_layer, text_color);
		text_layer_set_text_color(s_conditions_layer, text_color);
		text_layer_set_text_color(s_temp_layer_unanimated, text_color);
		text_layer_set_text_color(s_conditions_layer_unanimated, text_color);
		text_layer_set_text_color(s_charge_layer, text_color);
		text_layer_set_text_color(s_bluetooth_layer, text_color);
  #endif
}

void set_background_color(int bgcolor) {
	#ifdef PBL_COLOR
		GColor bg_color = GColorFromHEX(bgcolor);
		window_set_background_color(s_main_window, bg_color);
  	#endif
}

void inverter() {
	if (invert_colors == 1) {
	    	window_set_background_color(s_main_window, GColorWhite);
			text_layer_set_text_color(s_time_layer, GColorBlack);
			text_layer_set_text_color(s_date_layer, GColorBlack);
			text_layer_set_text_color(s_temp_layer, GColorBlack);
			text_layer_set_text_color(s_conditions_layer, GColorBlack);
			text_layer_set_text_color(s_temp_layer_unanimated, GColorBlack);
			text_layer_set_text_color(s_conditions_layer_unanimated, GColorBlack);
			text_layer_set_text_color(s_charge_layer, GColorBlack);
			text_layer_set_text_color(s_bluetooth_layer, GColorBlack);
	    } else {
	    	window_set_background_color(s_main_window, GColorBlack);
			text_layer_set_text_color(s_time_layer, GColorWhite);
			text_layer_set_text_color(s_date_layer, GColorWhite);
			text_layer_set_text_color(s_temp_layer, GColorWhite);
			text_layer_set_text_color(s_conditions_layer, GColorWhite);
			text_layer_set_text_color(s_temp_layer_unanimated, GColorWhite);
			text_layer_set_text_color(s_conditions_layer_unanimated, GColorWhite);
			text_layer_set_text_color(s_charge_layer, GColorWhite);
			text_layer_set_text_color(s_bluetooth_layer, GColorWhite);
	    }
}

static void main_window_load(Window *window) {
	GRect bounds = layer_get_bounds(window_get_root_layer(s_main_window));

	// Create fonts
	if (persist_exists(KEY_LARGE_FONT)) {
		APP_LOG(APP_LOG_LEVEL_INFO, "KEY_LARGE_FONT exists!");
		large_font = persist_read_int(KEY_LARGE_FONT);
	}

	s_weather_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SQUARE_18));
	s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SQUARE_50));
	s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SQUARE_26));
	s_bt_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SQUARE_14));

	// Weather parent layers
	s_weather_layer = layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
	s_weather_layer_unanimated = layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
	
	// Battery bar
	s_batt_layer = layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
	layer_set_update_proc(s_batt_layer, batt_layer_draw);

	// Static bar
	s_static_layer = layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
	layer_set_update_proc(s_static_layer, static_layer_draw);

	// Time layer
	s_time_layer = text_layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
	text_layer_set_font(s_time_layer, s_time_font);
	//text_layer_set_text(s_time_layer, "     "); // to get size
	text_layer_set_background_color(s_time_layer, GColorClear);
	text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
	//text_layer_set_text(s_time_layer, "12:34");
	
	// Date layer
	s_date_layer = text_layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
	text_layer_set_font(s_date_layer, s_date_font);
	text_layer_set_text(s_date_layer, "          ");
	text_layer_set_background_color(s_date_layer, GColorClear);
	text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
	//text_layer_set_text(s_date_layer, "THU OCT 15");
	
	update_time();
	
	GSize time_size = text_layer_get_content_size(s_time_layer);
	layer_set_frame(text_layer_get_layer(s_time_layer), GRect(0, ((bounds.size.h / 2) + 5 - time_size.h), bounds.size.w, time_size.h));
	GRect time_frame = layer_get_frame(text_layer_get_layer(s_time_layer));
	
	GSize date_size = text_layer_get_content_size(s_date_layer);
	layer_set_frame(text_layer_get_layer(s_date_layer), GRect(0, (bounds.size.h / 2) + 5, bounds.size.w, bounds.size.h));
	GRect date_frame = layer_get_frame(text_layer_get_layer(s_date_layer));
	
	// Charging status
	s_charge_layer = text_layer_create(GRect(0, (date_frame.origin.y + date_size.h), bounds.size.w, bounds.size.h));
	text_layer_set_background_color(s_charge_layer, GColorClear);
	text_layer_set_font(s_charge_layer, s_bt_font);
	text_layer_set_text_alignment(s_charge_layer, GTextAlignmentCenter);
	text_layer_set_text(s_charge_layer, "CHRG");
	layer_set_hidden(text_layer_get_layer(s_charge_layer), true);

	// Bluetooth status
	s_bluetooth_layer = text_layer_create(GRect(0, time_frame.origin.y - 3, bounds.size.w, bounds.size.h));
	text_layer_set_background_color(s_bluetooth_layer, GColorClear);
	text_layer_set_font(s_bluetooth_layer, s_bt_font);
	text_layer_set_text_alignment(s_bluetooth_layer, GTextAlignmentCenter);
	text_layer_set_text(s_bluetooth_layer, "BT");

	// ========== WEATHER LAYERS ========== //

	// Temperature
	s_temp_layer = text_layer_create(GRect(0, -32, bounds.size.w, 18));
	text_layer_set_background_color(s_temp_layer, GColorClear);
	text_layer_set_text_alignment(s_temp_layer, GTextAlignmentCenter);

	// Conditions
	s_conditions_layer = text_layer_create(GRect(0, 182, bounds.size.w, 14));
	text_layer_set_overflow_mode(s_conditions_layer, GTextOverflowModeWordWrap);
	text_layer_set_background_color(s_conditions_layer, GColorClear);
	text_layer_set_text_alignment(s_conditions_layer, GTextAlignmentCenter);

	// Temperature unanimated
	s_temp_layer_unanimated = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(40, 0), bounds.size.w, 18));
	text_layer_set_background_color(s_temp_layer_unanimated, GColorClear);
	text_layer_set_text_alignment(s_temp_layer_unanimated, GTextAlignmentCenter);

	// Conditions unanimated
	s_conditions_layer_unanimated = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(bounds.size.h - 40, 150), bounds.size.w, 14));
	text_layer_set_overflow_mode(s_conditions_layer_unanimated, GTextOverflowModeWordWrap);
	text_layer_set_background_color(s_conditions_layer_unanimated, GColorClear);
	text_layer_set_text_alignment(s_conditions_layer_unanimated, GTextAlignmentCenter);
	
	if (large_font == 1) {
		APP_LOG(APP_LOG_LEVEL_INFO, "Using large font");
		text_layer_set_font(s_conditions_layer, s_weather_font);
		text_layer_set_font(s_temp_layer, s_weather_font);
		text_layer_set_font(s_conditions_layer_unanimated, s_weather_font);
		text_layer_set_font(s_temp_layer_unanimated, s_weather_font);
	} else {
		APP_LOG(APP_LOG_LEVEL_INFO, "Using small font");
		text_layer_set_font(s_conditions_layer, s_bt_font);
		text_layer_set_font(s_temp_layer, s_bt_font);
		text_layer_set_font(s_conditions_layer_unanimated, s_bt_font);
		text_layer_set_font(s_temp_layer_unanimated, s_bt_font);
	}
	
	// ========== ADD CHILDREN ========== //

	// Main elements
	layer_add_child(window_get_root_layer(window), s_batt_layer);
	layer_add_child(window_get_root_layer(window), s_static_layer);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));

	// Extra elements
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_charge_layer));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_bluetooth_layer));

	// Weather elements
	layer_add_child(window_get_root_layer(window), s_weather_layer);
	layer_add_child(window_get_root_layer(window), s_weather_layer_unanimated);
	layer_add_child(s_weather_layer, text_layer_get_layer(s_temp_layer));
	layer_add_child(s_weather_layer, text_layer_get_layer(s_conditions_layer));
	layer_add_child(s_weather_layer_unanimated, text_layer_get_layer(s_temp_layer_unanimated));
	layer_add_child(s_weather_layer_unanimated, text_layer_get_layer(s_conditions_layer_unanimated));

	// ========== CHECK FOR EXISTING KEYS ========== //
	
	#ifdef PBL_COLOR
		if (persist_exists(KEY_TEXT_COLOR)) {
	    	int text_color = persist_read_int(KEY_TEXT_COLOR);
	    	APP_LOG(APP_LOG_LEVEL_INFO, "KEY_TEXT_COLOR exists! - %d", text_color);
	    	set_text_color(text_color);
	    } else {
	    	set_text_color(0xFFFFFF); // white
	    }

	    if (persist_exists(KEY_BACKGROUND_COLOR)) {
	    	int bg_color = persist_read_int(KEY_BACKGROUND_COLOR);
	    	APP_LOG(APP_LOG_LEVEL_INFO, "KEY_BACKGROUND_COLOR exists! - %d", bg_color);
	    	set_background_color(bg_color);
	    } else {
	    	set_background_color(0x000000); // black
	    }
	#endif


	#ifdef PBL_BW
	    if (persist_exists(KEY_INVERT_COLORS)) {
	    	invert_colors = persist_read_int(KEY_INVERT_COLORS);
	    	APP_LOG(APP_LOG_LEVEL_INFO, "KEY_INVERT_COLORS exists! - %d", invert_colors);
	    }

	    inverter();
	#endif

	if (persist_exists(KEY_USE_CELSIUS)) {
  	  	use_celsius = persist_read_int(KEY_USE_CELSIUS);
  	  	APP_LOG(APP_LOG_LEVEL_INFO, "KEY_USE_CELSIUS exists! - %d", use_celsius);
  	}

  	if (persist_exists(KEY_SHAKE_FOR_WEATHER)) {
  	  	shake_for_weather = persist_read_int(KEY_SHAKE_FOR_WEATHER);
  	  	APP_LOG(APP_LOG_LEVEL_INFO, "KEY_SHAKE_FOR_WEATHER exists! - %d", shake_for_weather);
  	}

  	if (persist_exists(KEY_REFLECT_BATT)) {
  	  	reflect_batt = persist_read_int(KEY_REFLECT_BATT);
  	  	APP_LOG(APP_LOG_LEVEL_INFO, "KEY_REFLECT_BATT exists! - %d", reflect_batt);

  	  	if (reflect_batt == 1) {
  			layer_set_hidden(s_static_layer, true);
  			layer_set_hidden(s_batt_layer, false);
  		} else {
  			layer_set_hidden(s_static_layer, false);
  			layer_set_hidden(s_batt_layer, true);
  		}
  	} else {
  		layer_set_hidden(s_static_layer, true);
  		layer_set_hidden(s_batt_layer, false);
  	}

  	if (persist_exists(KEY_SHOW_WEATHER)) {
  		show_weather = persist_read_int(KEY_SHOW_WEATHER);
  		APP_LOG(APP_LOG_LEVEL_INFO, "KEY_SHOW_WEATHER exists! - %d", show_weather);

  		if (show_weather == 1) {
  			update_layers();
  		} else {
  			layer_set_hidden(s_weather_layer, true);
  			layer_set_hidden(s_weather_layer_unanimated, true);
  		}
  	}

  	if (persist_exists(KEY_DATE_FORMAT)) {
  		euro_date = persist_read_int(KEY_DATE_FORMAT);
  		APP_LOG(APP_LOG_LEVEL_INFO, "KEY_DATE_FORMAT exists! - %d", euro_date);

  	}

  	if (persist_exists(KEY_LANGUAGE)) {
  		lang = persist_read_int(KEY_LANGUAGE);
  		APP_LOG(APP_LOG_LEVEL_INFO, "KEY_LANGUAGE exists! - %s", langCodes[lang]);
  	} else {
  		lang = 0;
  	}

  	bool connected = bluetooth_connection_service_peek();

  	if (!connected) {
 		layer_set_hidden(text_layer_get_layer(s_bluetooth_layer), false);
 	} else {
  		layer_set_hidden(text_layer_get_layer(s_bluetooth_layer), true);
 	}

  	charge_handler(); // Is the battery charging?
}

static void main_window_unload(Window *window) {
	text_layer_destroy(s_time_layer);
	text_layer_destroy(s_date_layer);
	text_layer_destroy(s_charge_layer);
	text_layer_destroy(s_conditions_layer);
	text_layer_destroy(s_conditions_layer_unanimated);
	text_layer_destroy(s_temp_layer);
	text_layer_destroy(s_temp_layer_unanimated);
	layer_destroy(s_batt_layer);
	layer_destroy(s_weather_layer);
	layer_destroy(s_weather_layer_unanimated);

	fonts_unload_custom_font(s_time_font);
	fonts_unload_custom_font(s_date_font);
	fonts_unload_custom_font(s_weather_font);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	update_time();
	if (show_weather == 1) {
		// Update weather every 30 minutes
		if(tick_time->tm_min % 30 == 0) {
			// Begin dictionary
			DictionaryIterator *iter;
			app_message_outbox_begin(&iter);

			// Add a key-value pair
			dict_write_uint8(iter, 4, 0);

			// Send the message!
			app_message_outbox_send();
		}
	}
}

static void update_weather() {
	// Begin dictionary
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);

	// Add a key-value pair
	dict_write_uint8(iter, 4, 0);

	// Send the message!
	app_message_outbox_send();
}

static void tap_handler(AccelAxisType axis, int32_t direction) {
	if (shake_for_weather == 1) {
		if (shake_for_weather == 0) {
			APP_LOG(APP_LOG_LEVEL_INFO, "Tap! Not animating.");
			// Do not animate
		} else {
			APP_LOG(APP_LOG_LEVEL_INFO, "Tap! Animating");
			animate_layers();
		}
	}
}

static void init() {
	s_main_window = window_create();

	window_set_window_handlers(s_main_window, (WindowHandlers) {
		.load = main_window_load,
		.unload = main_window_unload
	});

	window_stack_push(s_main_window, true);
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
	battery_state_service_subscribe(battery_handler);
	accel_tap_service_subscribe(tap_handler);
	bluetooth_connection_service_subscribe(bluetooth_handler);

	
	
	init_appmessage(); // Init appmessage in messaging.c
}

static void deinit() {
  window_destroy(s_main_window);
  tick_timer_service_unsubscribe();
 	battery_state_service_unsubscribe();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
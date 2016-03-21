#include <pebble.h>
#include "languages.h"
#include "main.h"
#include "bar.h"
	
Window *main_window;

static Layer *bluetooth_layer;
TextLayer *time_layer, *date_layer, *temp_layer, *conditions_layer, *temp_layer_unanimated, *conditions_layer_unanimated;

GFont weather_font, bt_font, date_font, time_font, small_time_font;

static Layer  *weather_layer, *weather_layer_unanimated;
Layer *bar_layer, *static_layer;

static GBitmap *bt_icon;

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
bool show_seconds = 0;

int lang; // User selected language code

void on_animation_stopped(Animation *anim, bool finished, void *context) {
    //Free the memory used by the Animation
    property_animation_destroy((PropertyAnimation*) anim);
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

static void bluetooth_handler(bool connected) {
	if (!connected) {
		APP_LOG(APP_LOG_LEVEL_INFO, "BT disconnected");
		layer_set_hidden(bluetooth_layer, false);
		if (vibe_on_disconnect == 1) {
			vibes_long_pulse();
		}
	} else {
		APP_LOG(APP_LOG_LEVEL_INFO, "BT connected");
		layer_set_hidden(bluetooth_layer, true);
		if (vibe_on_connect == 1) {
			vibes_double_pulse();
		}
	}
}

static void draw_bt(Layer *layer, GContext *ctx) {
	graphics_context_set_compositing_mode(ctx, GCompOpSet);
	
	#ifdef PBL_COLOR
		graphics_context_set_fill_color(ctx, GColorBlue);
	#else
		graphics_context_set_fill_color(ctx, GColorBlack);
	#endif
	
	GRect bounds = layer_get_bounds(window_get_root_layer(main_window));
	
	#ifdef PBL_ROUND
		graphics_fill_circle(ctx, GPoint(bounds.size.w / 2, bounds.size.h - 20), 9);
		graphics_draw_bitmap_in_rect(ctx, bt_icon, GRect(bounds.size.w / 2 - 8, bounds.size.h - 29, 18, 18));
	#else
		graphics_fill_circle(ctx, GPoint(bounds.size.w / 2, 35), 9);
		graphics_draw_bitmap_in_rect(ctx, bt_icon, GRect(bounds.size.w / 2 - 8, 26, 18, 18));
	#endif
}

void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	update_time();
	if (show_weather == 1) {
		// Update weather every 30 minutes
		if(tick_time->tm_min % 30 == 0) {
			update_weather();
		}
	}
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

void update_time() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
	
  static char time_buffer[] = "00:00:00";
  static char datn_buffer[] = "DD"; // Buffer for date number
  static char date_buffer[] = "WWW MMM DD"; // Buffer for entire date to display
  
	// Display hour and minute
	if (show_seconds == 1) {
		if(clock_is_24h_style() == true) {
			strftime(time_buffer, sizeof("00:00:00"), "%H:%M:%S", tick_time);
		} else {
			strftime(time_buffer, sizeof("00:00:00"), "%I:%M:%S", tick_time);
		}
	} else {
		if(clock_is_24h_style() == true) {
			strftime(time_buffer, sizeof("00:00:00"), "%H:%M", tick_time);
		} else {
			strftime(time_buffer, sizeof("00:00:00"), "%I:%M", tick_time);
		}
	}
	
	text_layer_set_text(time_layer, time_buffer);

	// Display date
	strftime(datn_buffer, sizeof("DD"), "%d", tick_time); // Write current date to buffer
	int month = tick_time->tm_mon; // Get current month as an integer
	int weekday = tick_time->tm_wday; // Get current weekday as an integer (0 is Sunday)
	
	// Select the correct strings from languages.c and write to buffer along with date
	if (euro_date == 1) { // If the user has selected the WWW DD MMM date format
		snprintf(date_buffer, sizeof(date_buffer), "%s %s %s", dayNames[lang][weekday], datn_buffer, monthNames[lang][month]);
	} else {
		snprintf(date_buffer, sizeof(date_buffer), "%s %s %s", dayNames[lang][weekday], monthNames[lang][month], datn_buffer);
	}

	text_layer_set_text(date_layer, date_buffer); // Display the date info
}

void update_layers() {
	GRect bounds = layer_get_bounds(window_get_root_layer(main_window));
	GSize cond_size = text_layer_get_content_size(conditions_layer);
	if (show_weather == 0) {
		APP_LOG(APP_LOG_LEVEL_INFO, "Hiding weather");
		layer_set_hidden(weather_layer, true);
		layer_set_hidden(weather_layer_unanimated, true);
	} else {
		if (shake_for_weather == 0) {
			APP_LOG(APP_LOG_LEVEL_INFO, "Showing static weather");
			layer_set_hidden(weather_layer, true);
			layer_set_hidden(weather_layer_unanimated, false);
		} else {
			APP_LOG(APP_LOG_LEVEL_INFO, "Showing animated weather");
			layer_set_hidden(weather_layer, false);
			layer_set_hidden(weather_layer_unanimated, true);
		}
	}
	
	layer_mark_dirty(bar_layer);

	/*if (reflect_batt == 1) {
		layer_set_hidden(static_layer, true);
		layer_set_hidden(bar_layer, false);
  } else {
		layer_set_hidden(static_layer, false);
		layer_set_hidden(bar_layer, true);
  }*/
}

void set_text_color(int color) {
  #ifdef PBL_COLOR
		GColor text_color = GColorFromHEX(color);
		text_layer_set_text_color(time_layer, text_color);
		text_layer_set_text_color(date_layer, text_color);
		text_layer_set_text_color(temp_layer, text_color);
		text_layer_set_text_color(conditions_layer, text_color);
		text_layer_set_text_color(temp_layer_unanimated, text_color);
		text_layer_set_text_color(conditions_layer_unanimated, text_color);
  #endif
}

void set_background_color(int bgcolor) {
	#ifdef PBL_COLOR
		GColor bg_color = GColorFromHEX(bgcolor);
		window_set_background_color(main_window, bg_color);
  #endif
}

void inverter() {
	if (invert_colors == 1) {
	    window_set_background_color(main_window, GColorWhite);
			text_layer_set_text_color(time_layer, GColorBlack);
			text_layer_set_text_color(date_layer, GColorBlack);
			text_layer_set_text_color(temp_layer, GColorBlack);
			text_layer_set_text_color(conditions_layer, GColorBlack);
			text_layer_set_text_color(temp_layer_unanimated, GColorBlack);
			text_layer_set_text_color(conditions_layer_unanimated, GColorBlack);
	} else {
	    	window_set_background_color(main_window, GColorBlack);
			text_layer_set_text_color(time_layer, GColorWhite);
			text_layer_set_text_color(date_layer, GColorWhite);
			text_layer_set_text_color(temp_layer, GColorWhite);
			text_layer_set_text_color(conditions_layer, GColorWhite);
			text_layer_set_text_color(temp_layer_unanimated, GColorWhite);
			text_layer_set_text_color(conditions_layer_unanimated, GColorWhite);
	}
}

static void main_window_load(Window *window) {
	GRect bounds = layer_get_bounds(window_get_root_layer(main_window));

	// Create fonts
	if (persist_exists(KEY_LARGE_FONT)) {
		APP_LOG(APP_LOG_LEVEL_INFO, "KEY_LARGE_FONT exists!");
		large_font = persist_read_int(KEY_LARGE_FONT);
	}

	weather_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SQUARE_18));
	time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SQUARE_50));
	small_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SQUARE_34));
	date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SQUARE_26));
	bt_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SQUARE_14));

	// Weather parent layers
	weather_layer = layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
	weather_layer_unanimated = layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
	
	// Battery bar
	bar_layer = layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
	layer_set_update_proc(bar_layer, bar_layer_draw);

	// Static bar
	static_layer = layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
	layer_set_update_proc(static_layer, static_layer_draw);

	// Time layer
	time_layer = text_layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
	text_layer_set_background_color(time_layer, GColorClear);
	text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
	//text_layer_set_text(time_layer, "12:34");
	
	if (persist_exists(KEY_SHOW_SECONDS)) {
		show_seconds = persist_read_int(KEY_SHOW_SECONDS);
		APP_LOG(APP_LOG_LEVEL_INFO, "KEY_SHOW_SECONDS exists! - %d", show_seconds);
		
		if (show_seconds == 1) {
			APP_LOG(APP_LOG_LEVEL_INFO, "Subscribing to seconds");
			tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
		} else {
			APP_LOG(APP_LOG_LEVEL_INFO, "Subscribing to minutes");
			tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
		}
	}
	
	
	// Date layer
	date_layer = text_layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
	text_layer_set_font(date_layer, date_font);
	text_layer_set_background_color(date_layer, GColorClear);
	text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);
	
	update_time();
	
	if (show_seconds == 1) {
		text_layer_set_font(time_layer, small_time_font);
	} else {
		text_layer_set_font(time_layer, time_font);
	}
	
	GSize time_size = text_layer_get_content_size(time_layer);
	layer_set_frame(text_layer_get_layer(time_layer), GRect(0, ((bounds.size.h / 2) + 5 - time_size.h), bounds.size.w, time_size.h));
	GRect time_frame = layer_get_frame(text_layer_get_layer(time_layer));
	
	GSize date_size = text_layer_get_content_size(date_layer);
	layer_set_frame(text_layer_get_layer(date_layer), GRect(0, (bounds.size.h / 2) + 5, bounds.size.w, bounds.size.h));
	GRect date_frame = layer_get_frame(text_layer_get_layer(date_layer));

	// Bluetooth status
	bluetooth_layer = layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
	layer_set_update_proc(bluetooth_layer, draw_bt);
	bt_icon = gbitmap_create_with_resource(RESOURCE_ID_BT_ICON);

	// ========== WEATHER LAYERS ========== //

	// Temperature
	temp_layer = text_layer_create(GRect(0, -32, bounds.size.w, 50));
	text_layer_set_background_color(temp_layer, GColorClear);
	text_layer_set_text_alignment(temp_layer, GTextAlignmentCenter);

	// Conditions
	conditions_layer = text_layer_create(GRect(0, 182, bounds.size.w, 50));
	text_layer_set_overflow_mode(conditions_layer, GTextOverflowModeWordWrap);
	text_layer_set_background_color(conditions_layer, GColorClear);
	text_layer_set_text_alignment(conditions_layer, GTextAlignmentCenter);

	// Temperature unanimated
	temp_layer_unanimated = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(40, 0), bounds.size.w, 50));
	text_layer_set_background_color(temp_layer_unanimated, GColorClear);
	text_layer_set_text_alignment(temp_layer_unanimated, GTextAlignmentCenter);

	// Conditions unanimated
	conditions_layer_unanimated = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(bounds.size.h - 55, 150), bounds.size.w, 50));
	text_layer_set_overflow_mode(conditions_layer_unanimated, GTextOverflowModeWordWrap);
	text_layer_set_background_color(conditions_layer_unanimated, GColorClear);
	text_layer_set_text_alignment(conditions_layer_unanimated, GTextAlignmentCenter);
	
	if (large_font == 1) {
		APP_LOG(APP_LOG_LEVEL_INFO, "Using large font");
		text_layer_set_font(conditions_layer, weather_font);
		text_layer_set_font(temp_layer, weather_font);
		text_layer_set_font(conditions_layer_unanimated, weather_font);
		text_layer_set_font(temp_layer_unanimated, weather_font);
	} else {
		APP_LOG(APP_LOG_LEVEL_INFO, "Using small font");
		text_layer_set_font(conditions_layer, bt_font);
		text_layer_set_font(temp_layer, bt_font);
		text_layer_set_font(conditions_layer_unanimated, bt_font);
		text_layer_set_font(temp_layer_unanimated, bt_font);
	}
	
	// ========== ADD CHILDREN ========== //

	// Main elements
	layer_add_child(window_get_root_layer(window), bar_layer);
	layer_add_child(window_get_root_layer(window), static_layer);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(time_layer));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(date_layer));

	// Extra elements
	layer_add_child(window_get_root_layer(window), bluetooth_layer);

	// Weather elements
	layer_add_child(window_get_root_layer(window), weather_layer);
	layer_add_child(window_get_root_layer(window), weather_layer_unanimated);
	layer_add_child(weather_layer, text_layer_get_layer(temp_layer));
	layer_add_child(weather_layer, text_layer_get_layer(conditions_layer));
	layer_add_child(weather_layer_unanimated, text_layer_get_layer(temp_layer_unanimated));
	layer_add_child(weather_layer_unanimated, text_layer_get_layer(conditions_layer_unanimated));

	// ========== CHECK FOR EXISTING KEYS ========== //
	
	#ifdef PBL_COLOR
		if (persist_exists(KEY_TEXT_COLOR)) {
	    	int text_color = persist_read_int(KEY_TEXT_COLOR);
	    	APP_LOG(APP_LOG_LEVEL_INFO, "KEY_TEXT_COLOR exists! - %d", text_color);
	    	set_text_color(text_color);
	    } else {
	    	set_text_color(0x00ff00); // green
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
  			layer_set_hidden(static_layer, true);
  			layer_set_hidden(bar_layer, false);
  		} else {
  			layer_set_hidden(static_layer, false);
  			layer_set_hidden(bar_layer, true);
  		}
  	} else {
  		layer_set_hidden(static_layer, true);
  		layer_set_hidden(bar_layer, false);
  	}

  	if (persist_exists(KEY_SHOW_WEATHER)) {
  		show_weather = persist_read_int(KEY_SHOW_WEATHER);
  		APP_LOG(APP_LOG_LEVEL_INFO, "KEY_SHOW_WEATHER exists! - %d", show_weather);

  		if (show_weather == 1) {
  			update_layers();
  		} else {
  			layer_set_hidden(weather_layer, true);
  			layer_set_hidden(weather_layer_unanimated, true);
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
 			layer_set_hidden(bluetooth_layer, false);
 		} else {
  		layer_set_hidden(bluetooth_layer, true);
 		}
	
}

static void main_window_unload(Window *window) {
	text_layer_destroy(time_layer);
	text_layer_destroy(date_layer);
	text_layer_destroy(conditions_layer);
	text_layer_destroy(conditions_layer_unanimated);
	text_layer_destroy(temp_layer);
	text_layer_destroy(temp_layer_unanimated);
	layer_destroy(bar_layer);
	layer_destroy(weather_layer);
	layer_destroy(weather_layer_unanimated);

	fonts_unload_custom_font(time_font);
	fonts_unload_custom_font(date_font);
	fonts_unload_custom_font(weather_font);
}

static void init() {
	main_window = window_create();

	window_set_window_handlers(main_window, (WindowHandlers) {
		.load = main_window_load,
		.unload = main_window_unload
	});

	window_stack_push(main_window, true);
	
	battery_state_service_subscribe(battery_handler);
	accel_tap_service_subscribe(tap_handler);
	bluetooth_connection_service_subscribe(bluetooth_handler);
	
	init_appmessage(); // Init appmessage in messaging.c
}

static void deinit() {
  window_destroy(main_window);
  tick_timer_service_unsubscribe();
 	battery_state_service_unsubscribe();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
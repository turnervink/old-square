#include <pebble.h>

#define KEY_TEXT_COLOR 0
#define KEY_INVERT_COLORS 1
	
static Window *s_main_window;
static TextLayer *s_time_layer, *s_date_layer, *s_charge_layer;
static GFont s_time_font, s_date_font;
static Layer *s_batt_layer, *s_scharge_layer;
static bool invert_colors = 0;


static void update_time() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
	
  static char time_buffer[] = "00:00";
	static char date_buffer[] = "WWW MMM DD";
  
  if(clock_is_24h_style() == true) {
    strftime(time_buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    strftime(time_buffer, sizeof("00:00"), "%I:%M", tick_time);
  }
	
	text_layer_set_text(s_time_layer, time_buffer);
	
	strftime(date_buffer, sizeof("WWW MMM DD"), "%a %b %d", tick_time);
	
	text_layer_set_text(s_date_layer, date_buffer);
}

static void battery_handler(BatteryChargeState state) {
	APP_LOG(APP_LOG_LEVEL_INFO, "Battery change registered!");
	layer_mark_dirty(s_batt_layer);
	
	bool charging = state.is_charging;
	
	if(charging == true) {
		layer_set_hidden(s_scharge_layer, false);
	} else {
		layer_set_hidden(s_scharge_layer, true);
	}
}

static void batt_layer_draw(Layer *layer, GContext *ctx) {	
	BatteryChargeState state = battery_state_service_peek();
	int pct = state.charge_percent;

	#ifdef PBL_COLOR // If on basalt
		if (persist_exists(KEY_TEXT_COLOR)) { // Check for existing colour
			int text_color = persist_read_int(KEY_TEXT_COLOR);
    		GColor fill_color = GColorFromHEX(text_color);
			graphics_context_set_fill_color(ctx, fill_color); // Make the remaining battery that colour
		} else {
			graphics_context_set_fill_color(ctx, GColorWhite); // Otherwise, default to white
		}
	#else // If on aplite
		if (persist_exists(KEY_INVERT_COLORS)) { // Check for invert setting
			if (invert_colors == 1) {
				graphics_context_set_fill_color(ctx, GColorBlack); // If inverted, make the remaining battery black
			} else {
				graphics_context_set_fill_color(ctx, GColorWhite); // Otherwise, default to white
			}
		} else {
			graphics_context_set_fill_color(ctx, GColorWhite); // If no invert setting, default to white
		}
	#endif

	graphics_fill_rect(ctx, GRect(2, 92, 140-(((100-pct)/10)*14), 2), 0, GCornerNone); // Draw battery
}

static void set_background_and_text_color(int color) {
  #ifdef PBL_COLOR
		GColor text_color = GColorFromHEX(color);
		text_layer_set_text_color(s_time_layer, text_color);
		text_layer_set_text_color(s_date_layer, text_color);

		window_set_background_color(s_main_window, gcolor_legible_over(text_color));
  #else
		window_set_background_color(s_main_window, GColorBlack);
		text_layer_set_text_color(s_time_layer, GColorWhite);
		text_layer_set_text_color(s_date_layer, GColorWhite);
  #endif
}


static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *text_color_t = dict_find(iter, KEY_TEXT_COLOR);
  Tuple *invert_colors_t = dict_find(iter, KEY_INVERT_COLORS);

  if (text_color_t) {
    int text_color = text_color_t->value->int32;

    persist_write_int(KEY_TEXT_COLOR, text_color);

    set_background_and_text_color(text_color);
  }

  if (invert_colors_t) {
    invert_colors = invert_colors_t->value->int8;

    persist_write_int(KEY_INVERT_COLORS, invert_colors);

  if (invert_colors == 1) {
    #ifdef PBL_COLOR
    	// Do not try to invert
    #else
	    if (invert_colors == 1) {
	    	window_set_background_color(s_main_window, GColorWhite);
			text_layer_set_text_color(s_time_layer, GColorBlack);
			text_layer_set_text_color(s_date_layer, GColorBlack);
	    } else {
	    	window_set_background_color(s_main_window, GColorBlack);
			text_layer_set_text_color(s_time_layer, GColorWhite);
			text_layer_set_text_color(s_date_layer, GColorWhite);
	    }
	#endif
    }
  }
}

static void main_window_load(Window *window) {
	// Create fonts
	s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SQUARE_50));
	s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SQUARE_22));
	
	// Battery bar
	s_batt_layer = layer_create(GRect(0, 0, 144, 168));
	layer_set_update_proc(s_batt_layer, batt_layer_draw);
	
	// Charging status
	s_charge_layer = text_layer_create(GRect(0, 110, 144, 168));
	#ifdef PBL_COLOR
		text_layer_set_text_color(s_charge_layer, GColorGreen);
	#else
		text_layer_set_text_color(s_charge_layer, GColorWhite);
	#endif
		text_layer_set_background_color(s_charge_layer, GColorClear);
	text_layer_set_font(s_charge_layer, s_date_font);
	text_layer_set_text_alignment(s_charge_layer, GTextAlignmentCenter);
	text_layer_set_text(s_charge_layer, "CHRG");
	
	s_scharge_layer = layer_create(GRect(0, 0, 144, 168));
	layer_set_hidden(s_scharge_layer, true);
	
	// Time layer
	s_time_layer = text_layer_create(GRect(0, 40, 144, 168));
	text_layer_set_background_color(s_time_layer, GColorClear);
	text_layer_set_font(s_time_layer, s_time_font);
	text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
	
	// Date layer
	s_date_layer = text_layer_create(GRect(0, 90, 144, 168));
	text_layer_set_background_color(s_date_layer, GColorClear);
	text_layer_set_font(s_date_layer, s_date_font);
	text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
	
	layer_add_child(window_get_root_layer(window), s_batt_layer);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
	layer_add_child(window_get_root_layer(window), s_scharge_layer);
	layer_add_child(s_scharge_layer, text_layer_get_layer(s_charge_layer));
	
	if (persist_exists(KEY_TEXT_COLOR)) {
    	int text_color = persist_read_int(KEY_TEXT_COLOR);
    	APP_LOG(APP_LOG_LEVEL_INFO, "KEY_TEXT_COLOR exists!");
    	set_background_and_text_color(text_color);
    }

    if (persist_exists(KEY_INVERT_COLORS)) {
    	APP_LOG(APP_LOG_LEVEL_INFO, "KEY_INVERT_COLORS exists!");
    	invert_colors = persist_read_int(KEY_INVERT_COLORS);
    }

    #ifdef PBL_COLOR
    	// Do not try to invert
    #else
	    if (invert_colors == 1) {
	    	window_set_background_color(s_main_window, GColorWhite);
			text_layer_set_text_color(s_time_layer, GColorBlack);
			text_layer_set_text_color(s_date_layer, GColorBlack);
	    } else {
	    	window_set_background_color(s_main_window, GColorBlack);
			text_layer_set_text_color(s_time_layer, GColorWhite);
			text_layer_set_text_color(s_date_layer, GColorWhite);
	    }
	#endif

	update_time();
}

static void main_window_unload(Window *window) {
	text_layer_destroy(s_time_layer);
	text_layer_destroy(s_date_layer);
	text_layer_destroy(s_charge_layer);
	layer_destroy(s_batt_layer);
	layer_destroy(s_scharge_layer);
	fonts_unload_custom_font(s_time_font);
	fonts_unload_custom_font(s_date_font);
}


static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
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
	
	BatteryChargeState state = battery_state_service_peek();
	bool charging = state.is_charging;
	
	if(charging == true) {
		layer_set_hidden(s_scharge_layer, false);
	} else {
		layer_set_hidden(s_scharge_layer, true);
	}

	app_message_register_inbox_received(inbox_received_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
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
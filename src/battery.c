#include <pebble.h>
#include "main.h"
#include "battery.h"

void charge_handler() {
	BatteryChargeState state = battery_state_service_peek();
	bool charging = state.is_charging;
	
	if (charging == true) {
		layer_set_hidden(text_layer_get_layer(s_charge_layer), false);
	} else {
		layer_set_hidden(text_layer_get_layer(s_charge_layer), true);
	}
}

void battery_handler(BatteryChargeState state) {
	APP_LOG(APP_LOG_LEVEL_INFO, "Battery change registered!");
	layer_mark_dirty(s_batt_layer);
	charge_handler();
}

void batt_layer_draw(Layer *layer, GContext *ctx) {	
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

	//graphics_fill_rect(ctx, GRect(2, 92, 140-(((100-pct)/10)*14), 2), 0, GCornerNone); // Draw battery

	GRect bounds = layer_get_bounds(window_get_root_layer(s_main_window));

	/*
	graphics_fill_rect(ctx, GRect((bounds.size.w / 2), (bounds.size.h / 2), (140-(((100-pct)/10)*14))/2, 2), 0, GCornerNone); // Centre to right
	graphics_fill_rect(ctx, GRect((bounds.size.w / 2), (bounds.size.h / 2), -(140-(((100-pct)/10)*14))/2, 2), 0, GCornerNone); // Centre to left
	*/

	graphics_fill_rect(ctx, GRect((bounds.size.w / 2), (bounds.size.h / 2) + 8, ((140)-(((100-pct)/10)*14))/2, 2), 0, GCornerNone); // Centre to right
	graphics_fill_rect(ctx, GRect((bounds.size.w / 2), (bounds.size.h / 2) + 8, -((140)-(((100-pct)/10)*14))/2, 2), 0, GCornerNone); // Centre to left
}

void static_layer_draw(Layer *layer, GContext *ctx) {
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

	GRect bounds = layer_get_bounds(window_get_root_layer(s_main_window));

	graphics_fill_rect(ctx, GRect(PBL_IF_ROUND_ELSE(20, 2), (bounds.size.h / 2) + 8, 140, 2), 0, GCornerNone); // Draw static bar
}
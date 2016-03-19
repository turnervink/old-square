#include <pebble.h>
#include "main.h"

void update_weather() {
	if (show_weather == 1) {
		APP_LOG(APP_LOG_LEVEL_INFO, "Updating weather...");
		// Begin dictionary
		DictionaryIterator *iter;
		app_message_outbox_begin(&iter);

		// Add a key-value pair
		dict_write_uint8(iter, 4, 0);

		// Send the message!
		app_message_outbox_send();
	}
}

void animate_layers() {
	GRect bounds = layer_get_bounds(window_get_root_layer(main_window));
	GSize cond_size = text_layer_get_content_size(conditions_layer);
	GSize temp_size = text_layer_get_content_size(temp_layer);
	GRect time_frame = layer_get_frame(text_layer_get_layer(time_layer));
	GSize date_size = text_layer_get_content_size(date_layer);
	GRect date_frame = layer_get_frame(text_layer_get_layer(date_layer));
	
	GRect temp_offscreen = GRect(0, -32, bounds.size.w, temp_size.h);
	GRect temp_onscreen_small = GRect(0, PBL_IF_ROUND_ELSE(time_frame.origin.y - 4, 0), bounds.size.w, temp_size.h);
	GRect temp_onscreen_large = GRect(0, PBL_IF_ROUND_ELSE(time_frame.origin.y - 8, 0), bounds.size.w, temp_size.h);
	
	GRect conditions_offscreen = GRect(0, bounds.size.h + cond_size.h, bounds.size.w, cond_size.h);
	GRect conditions_onscreen_small =GRect(0, PBL_IF_ROUND_ELSE(date_frame.origin.y + date_size.h, bounds.size.h - cond_size.h - 5), bounds.size.w, cond_size.h);
	GRect conditions_onscreen_large = GRect(0, PBL_IF_ROUND_ELSE(date_frame.origin.y + date_size.h, bounds.size.h - cond_size.h - 5), bounds.size.w, cond_size.h);
	
	if (large_font == 0) {
		// Temp moves in from top
		animate_layer(text_layer_get_layer(temp_layer), &temp_offscreen, &temp_onscreen_small, 1000, 0);
		animate_layer(text_layer_get_layer(temp_layer), &temp_onscreen_small, &temp_offscreen, 1000, 5000);

		// Weather moves in from bottom
		animate_layer(text_layer_get_layer(conditions_layer), &conditions_offscreen, &conditions_onscreen_small, 1000, 0);
		animate_layer(text_layer_get_layer(conditions_layer), &conditions_onscreen_small, &conditions_offscreen, 1000, 5000);
	} else {
		// Temp moves in from top
		animate_layer(text_layer_get_layer(temp_layer), &temp_offscreen, &temp_onscreen_large, 1000, 0);
		animate_layer(text_layer_get_layer(temp_layer), &temp_onscreen_large, &temp_offscreen, 1000, 5000);

		// Weather moves in from bottom
		animate_layer(text_layer_get_layer(conditions_layer), &conditions_offscreen, &conditions_onscreen_large, 1000, 0);
		animate_layer(text_layer_get_layer(conditions_layer), &conditions_onscreen_large, &conditions_offscreen, 1000, 5000);
	}
}

void size_weather_layers() {
	GRect bounds = layer_get_bounds(window_get_root_layer(main_window));
	GSize cond_size = text_layer_get_content_size(conditions_layer);
	GSize temp_size = text_layer_get_content_size(temp_layer);
	GRect time_frame = layer_get_frame(text_layer_get_layer(time_layer));
	GSize date_size = text_layer_get_content_size(date_layer);
	GRect date_frame = layer_get_frame(text_layer_get_layer(date_layer));
	
	if (show_weather == 0) {
		return;
	}
	
	if (large_font == 0) { // If we're using the small font
		layer_set_frame(text_layer_get_layer(temp_layer), GRect(0, -32, bounds.size.w, temp_size.h)); // Keep animated layers off screen
		layer_set_frame(text_layer_get_layer(temp_layer_unanimated), GRect(0, PBL_IF_ROUND_ELSE(time_frame.origin.y - 4, 0), bounds.size.w, temp_size.h)); // Place temp at the end of the animation
		
		layer_set_frame(text_layer_get_layer(conditions_layer), GRect(0, bounds.size.h + cond_size.h, bounds.size.w, cond_size.h));
		layer_set_frame(text_layer_get_layer(conditions_layer_unanimated), GRect(0, PBL_IF_ROUND_ELSE(date_frame.origin.y + date_size.h, bounds.size.h - cond_size.h - 5), bounds.size.w, cond_size.h)); // Place conditions at the end of the animation
	} else { // Do the same as above but take into account the larger font
		layer_set_frame(text_layer_get_layer(temp_layer), GRect(0, -32, bounds.size.w, temp_size.h));
		layer_set_frame(text_layer_get_layer(temp_layer_unanimated), GRect(0, PBL_IF_ROUND_ELSE(time_frame.origin.y - 8, 0), bounds.size.w, temp_size.h));
		
		layer_set_frame(text_layer_get_layer(conditions_layer), GRect(0, bounds.size.h + cond_size.h, bounds.size.w, cond_size.h));
		layer_set_frame(text_layer_get_layer(conditions_layer_unanimated), GRect(0, PBL_IF_ROUND_ELSE(date_frame.origin.y + date_size.h, bounds.size.h - cond_size.h - 5), bounds.size.w, cond_size.h));
	}
	
	layer_mark_dirty(text_layer_get_layer(conditions_layer));
	layer_mark_dirty(text_layer_get_layer(conditions_layer_unanimated));
	layer_mark_dirty(text_layer_get_layer(temp_layer));
	layer_mark_dirty(text_layer_get_layer(temp_layer_unanimated));
}

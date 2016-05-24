#include <pebble.h>
#include "main.h"
#include "coloursched.h"
#include "bar.h"

void check_for_night_mode() {
	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);
	int cur_hour = tick_time->tm_hour;
	
	APP_LOG(APP_LOG_LEVEL_INFO, "Hour for colourscheme is %d", cur_hour);
	
	if (cur_hour >= night_mode_start || cur_hour < night_mode_end) {
		APP_LOG(APP_LOG_LEVEL_INFO, "Using night colours");
		set_text_color(night_text_color);
		set_background_color(night_bg_color);
		fill_color = GColorFromHEX(night_text_color);
	} else {
		APP_LOG(APP_LOG_LEVEL_INFO, "Using day colours");
		set_text_color(text_color);
		set_background_color(bg_color);
		fill_color = GColorFromHEX(text_color);
	}
	
	layer_mark_dirty(bar_layer);
}

/*
	Will need to store text and bg colours for both day and night colours, then set text_color and bg_color to the correct one
*/
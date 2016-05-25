#include <pebble.h>
#include "main.h"
#include "premium.h"
#include "kiezelpay.h"

void activate_premium() {
	APP_LOG(APP_LOG_LEVEL_INFO, "Activating premium features");
	// Begin dictionary
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);

	// Add a key-value pair
	dict_write_uint8(iter, 501, 1);

	// Send the message!
	app_message_outbox_send();
}

void load_premium_settings() {
	APP_LOG(APP_LOG_LEVEL_INFO, "Checking premium features");
	int32_t result = kiezelpay_get_status();
	
	if (result & KIEZELPAY_LICENSED) {
		APP_LOG(APP_LOG_LEVEL_INFO, "Premium features unlocked");
		
		if (persist_exists(KEY_MANUAL_GOAL)) {
			manual_goal = persist_read_int(KEY_MANUAL_GOAL);
		}
		
		if (persist_exists(KEY_USE_NIGHT_MODE)) {
			use_night_mode = persist_read_int(KEY_USE_NIGHT_MODE);
		}
		
		if (persist_exists(KEY_NIGHT_TEXT_COLOR)) {
			night_text_color = persist_read_int(KEY_NIGHT_TEXT_COLOR);
		} else {
			night_text_color = 0x00ff00;
		}
	
		if (persist_exists(KEY_NIGHT_BACKGROUND_COLOR)) {
			night_bg_color = persist_read_int(KEY_NIGHT_BACKGROUND_COLOR);
		} else {
			night_bg_color = 0x000000;
		}
	
		if (persist_exists(KEY_NIGHT_START)) {
			night_mode_start = persist_read_int(KEY_NIGHT_START);
		}
	
		if (persist_exists(KEY_NIGHT_END)) {
			night_mode_end = persist_read_int(KEY_NIGHT_END);
		}
		
	} else {
		APP_LOG(APP_LOG_LEVEL_INFO, "Premium features locked");
		// Set everything back to non-premium defaults
		manual_goal = 0;
	}
		
}
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
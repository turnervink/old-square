#include <pebble.h>
#include "main.h"
#include "languages.h"

void sendLang(char* lang) { // Send selected language to JS to fetch weather
	APP_LOG(APP_LOG_LEVEL_INFO, "Sending lang to JS - %s", lang);
	// Begin dictionary
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);

	// Add a key-value pair
	dict_write_cstring(iter, 14, lang); // Key 14 is KEY_LANGUAGE

	// Send the message!
	app_message_outbox_send();
}

void init_appmessage() {
	APP_LOG(APP_LOG_LEVEL_INFO, "Opening app message inbox");
	
	app_message_register_inbox_received(inbox_received_handler);
	app_message_register_inbox_dropped(inbox_dropped_callback);
	app_message_register_outbox_failed(outbox_failed_callback);
	app_message_register_outbox_sent(outbox_sent_callback);
	
	int size_buffer_in = dict_calc_buffer_size(16, sizeof(char), sizeof(int32_t), sizeof(int8_t), sizeof(int32_t), sizeof(int32_t), sizeof(char), 
	sizeof(int8_t), sizeof(int8_t), sizeof(int8_t), sizeof(int32_t), sizeof(int8_t), 
	sizeof(int8_t), sizeof(int8_t), sizeof(char), sizeof(char), sizeof(int8_t));
		
	int size_buffer_out = dict_calc_buffer_size(3, sizeof(int8_t), sizeof(int8_t), sizeof(char));
	
  app_message_open(size_buffer_in, size_buffer_out);
}

void inbox_received_handler(DictionaryIterator *iter, void *contex) {
  static char temp_buffer[15];
  static char temp_c_buffer[15];
  static char conditions_buffer[100];

  Tuple *ready_t = dict_find(iter, KEY_READY); // cstring

  Tuple *text_color_t = dict_find(iter, KEY_TEXT_COLOR); // int32
  Tuple *invert_colors_t = dict_find(iter, KEY_INVERT_COLORS); // int8
  Tuple *temperature_t = dict_find(iter, KEY_TEMPERATURE); // int32
  Tuple *temperature_in_c_t = dict_find(iter, KEY_TEMPERATURE_IN_C); // int32
  Tuple *conditions_t = dict_find(iter, KEY_CONDITIONS); // cstring
  Tuple *shake_for_weather_t = dict_find(iter, KEY_SHAKE_FOR_WEATHER); // int8
  Tuple *show_weather_t = dict_find(iter, KEY_SHOW_WEATHER); // int8
  Tuple *use_celsius_t = dict_find(iter, KEY_USE_CELSIUS); // int8
  Tuple *background_color_t = dict_find(iter, KEY_BACKGROUND_COLOR); // int32
  Tuple *vibe_on_connect_t = dict_find(iter, KEY_VIBE_ON_CONNECT); // int8
  Tuple *vibe_on_disconnect_t = dict_find(iter, KEY_VIBE_ON_DISCONNECT); // int8
  Tuple *reflect_batt_t = dict_find(iter, KEY_REFLECT_BATT); // int8
  Tuple *date_format_t = dict_find(iter, KEY_DATE_FORMAT); // cstring
  Tuple *lang_t = dict_find(iter, KEY_LANGUAGE); // cstring
	Tuple *largefont_t = dict_find(iter, KEY_LARGE_FONT); // int8
	

  if (ready_t) { // Wait for JS to be ready before requesting weather in selected language
  	APP_LOG(APP_LOG_LEVEL_INFO, "JS reports ready");
  	ready = 1;

  	sendLang(langCodes[lang]);
  }

  if (lang_t) {
  	APP_LOG(APP_LOG_LEVEL_INFO, "KEY_LANGUAGE received!");
  	if (strcmp(lang_t->value->cstring, "en") == 0) {
  		APP_LOG(APP_LOG_LEVEL_INFO, "Using English");
  		lang = 0;
  	} else if (strcmp(lang_t->value->cstring, "fr") == 0){
  		APP_LOG(APP_LOG_LEVEL_INFO, "Using French");
  		lang = 1;
  	} else if (strcmp(lang_t->value->cstring, "es") == 0){
  		APP_LOG(APP_LOG_LEVEL_INFO, "Using Spanish");
  		lang = 2;
  	} else if (strcmp(lang_t->value->cstring, "de") == 0){
  		APP_LOG(APP_LOG_LEVEL_INFO, "Using German");
  		lang = 3;
  	} else {
  		lang = 0;
  	}
  	APP_LOG(APP_LOG_LEVEL_INFO, "KEY_LANGUAGE received!");
  	sendLang(lang_t->value->cstring);

  	persist_write_int(KEY_LANGUAGE, lang);
  }

  if (text_color_t) {
    int text_color = text_color_t->value->int32;
    APP_LOG(APP_LOG_LEVEL_INFO, "KEY_TEXT_COLOR received!");

    persist_write_int(KEY_TEXT_COLOR, text_color);
    #ifdef PBL_COLOR
    	set_text_color(text_color);
    #else

    #endif
  }

  if (background_color_t) {
  	int bg_color = background_color_t->value->int32;
  	APP_LOG(APP_LOG_LEVEL_INFO, "KEY_BACKGROUND_COLOR received! - %d", bg_color);

  	persist_write_int(KEY_BACKGROUND_COLOR, bg_color);
  	#ifdef PBL_COLOR
    	set_background_color(bg_color);
    #else

    #endif
  }

  if (invert_colors_t) {
  	APP_LOG(APP_LOG_LEVEL_INFO, "KEY_INVERT_COLORS received!");
    invert_colors = invert_colors_t->value->int8;


    persist_write_int(KEY_INVERT_COLORS, invert_colors);

    #ifdef PBL_BW
    	inverter();
    #endif
  }

  if (use_celsius_t) {
  	APP_LOG(APP_LOG_LEVEL_INFO, "KEY_USE_CELSIUS received!");

  	use_celsius = use_celsius_t->value->int8;

  	persist_write_int(KEY_USE_CELSIUS, use_celsius);
  }

  if (shake_for_weather_t) {
  	APP_LOG(APP_LOG_LEVEL_INFO, "KEY_SHAKE_FOR_WEATHER received!");

  	shake_for_weather = shake_for_weather_t->value->int8;

  	persist_write_int(KEY_SHAKE_FOR_WEATHER, shake_for_weather);
  }

  if (show_weather_t) {
  	APP_LOG(APP_LOG_LEVEL_INFO, "KEY_SHOW_WEATHER received!");

  	show_weather = show_weather_t->value->int8;

  	persist_write_int(KEY_SHOW_WEATHER, show_weather);
  }

  if (temperature_t) {
  	APP_LOG(APP_LOG_LEVEL_INFO, "KEY_TEMPERATURE received!");

  	snprintf(temp_buffer, sizeof(temp_buffer), "%d°", (int)temperature_t->value->int32);
  }

  if (temperature_in_c_t) {
  	APP_LOG(APP_LOG_LEVEL_INFO, "KEY_TEMPERATURE_IN_C received!");

  	snprintf(temp_c_buffer, sizeof(temp_c_buffer), "%d°", (int)temperature_in_c_t->value->int32);
  }

  if (conditions_t) {
		APP_LOG(APP_LOG_LEVEL_INFO, "KEY_CONDITIONS received!");

		snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", conditions_t->value->cstring);
		text_layer_set_text(s_conditions_layer, conditions_buffer);
		text_layer_set_text(s_conditions_layer_unanimated, conditions_buffer);
		//text_layer_set_text(s_conditions_layer, "This is some placeholder text");
		//text_layer_set_text(s_conditions_layer_unanimated, "This is some placeholder text");

		GSize cond_size = text_layer_get_content_size(s_conditions_layer);
		GSize conds_size = text_layer_get_content_size(s_conditions_layer_unanimated);
		GRect bounds = layer_get_bounds(window_get_root_layer(s_main_window));

		layer_set_frame(text_layer_get_layer(s_conditions_layer), GRect(0, 182, bounds.size.w, cond_size.h)); 
		layer_set_frame(text_layer_get_layer(s_conditions_layer_unanimated), GRect(0, PBL_IF_ROUND_ELSE(bounds.size.h - 55, (bounds.size.h - cond_size.h) - 5), bounds.size.w, cond_size.h));
  }

  if (vibe_on_connect_t) {
  	APP_LOG(APP_LOG_LEVEL_INFO, "KEY_VIBE_ON_CONNECT received!");
  	vibe_on_connect = vibe_on_connect_t->value->int8;
  }

  if (vibe_on_disconnect_t) {
  	APP_LOG(APP_LOG_LEVEL_INFO, "KEY_VIBE_ON_DISCONNECT received!");
  	vibe_on_disconnect = vibe_on_disconnect_t->value->int8;
  }

  if (use_celsius == 1) {
		APP_LOG(APP_LOG_LEVEL_INFO, "Using Celsius and getting temp size");
  	text_layer_set_text(s_temp_layer, temp_c_buffer);
  	text_layer_set_text(s_temp_layer_unanimated, temp_c_buffer);
		
		/*GSize temp_size = text_layer_get_content_size(s_temp_layer);
		GSize temps_size = text_layer_get_content_size(s_temp_layer_unanimated);
		GRect bounds = layer_get_bounds(window_get_root_layer(s_main_window));
		APP_LOG(APP_LOG_LEVEL_INFO, "Temp size is %d", temp_size.h);
		
		layer_set_frame(text_layer_get_layer(s_temp_layer), GRect(0, -32, bounds.size.w, 18));
		layer_set_frame(text_layer_get_layer(s_temp_layer_unanimated), GRect(0, PBL_IF_ROUND_ELSE(40, 0), bounds.size.w,  18));*/
  } else {
		APP_LOG(APP_LOG_LEVEL_INFO, "Using Fahrenheit and getting temp size");
  	text_layer_set_text(s_temp_layer, temp_buffer);
  	text_layer_set_text(s_temp_layer_unanimated, temp_buffer);
  }

  if (reflect_batt_t) {
  	APP_LOG(APP_LOG_LEVEL_INFO, "KEY_REFLECT_BATT received!");

  	reflect_batt = reflect_batt_t->value->int8;

  	persist_write_int(KEY_REFLECT_BATT, reflect_batt);
  }

  if (date_format_t) {
  	APP_LOG(APP_LOG_LEVEL_INFO, "KEY_DATE_FORMAT received!");

  	if (strcmp(date_format_t->value->cstring, "edate") == 0) {
  		APP_LOG(APP_LOG_LEVEL_INFO, "Using european date");
  		euro_date = 1;
  		persist_write_int(KEY_DATE_FORMAT, euro_date);
  	} else {
  		APP_LOG(APP_LOG_LEVEL_INFO, "Using standard date");
  		euro_date = 0;
  		persist_write_int(KEY_DATE_FORMAT, euro_date);
  	}
  }
	
	if (largefont_t) {
		large_font = largefont_t->value->int8;
		APP_LOG(APP_LOG_LEVEL_INFO, "KEY_LARGE_FONT received! - %d", large_font);
		
		persist_write_int(KEY_LARGE_FONT, large_font);
		
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

			layer_mark_dirty(text_layer_get_layer(s_conditions_layer));
		}

  update_layers();
  update_time();
}

void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}
#include <pebble.h>
#include "main.h"
#include "languages.h"

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
	
	//static bool weather_needs_update;

  Tuple *ready_tup = dict_find(iter, KEY_READY); // cstring

  Tuple *text_color_tup = dict_find(iter, KEY_TEXT_COLOR); // int32
  Tuple *invert_colors_tup = dict_find(iter, KEY_INVERT_COLORS); // int8
  Tuple *temperature_tup = dict_find(iter, KEY_TEMPERATURE); // int32
  Tuple *temperature_in_c_tup = dict_find(iter, KEY_TEMPERATURE_IN_C); // int32
  Tuple *conditions_tup = dict_find(iter, KEY_CONDITIONS); // cstring
  Tuple *shake_for_weather_tup = dict_find(iter, KEY_SHAKE_FOR_WEATHER); // int8
  Tuple *show_weather_tup = dict_find(iter, KEY_SHOW_WEATHER); // int8
  Tuple *use_celsius_tup = dict_find(iter, KEY_USE_CELSIUS); // int8
  Tuple *background_color_tup = dict_find(iter, KEY_BACKGROUND_COLOR); // int32
  Tuple *vibe_on_connect_tup = dict_find(iter, KEY_VIBE_ON_CONNECT); // int8
  Tuple *vibe_on_disconnect_tup = dict_find(iter, KEY_VIBE_ON_DISCONNECT); // int8
  Tuple *reflect_batt_tup = dict_find(iter, KEY_REFLECT_BATT); // int8
  Tuple *date_format_tup = dict_find(iter, KEY_DATE_FORMAT); // cstring
  Tuple *lang_tup = dict_find(iter, KEY_LANGUAGE); // cstring
	Tuple *largefont_tup = dict_find(iter, KEY_LARGE_FONT); // int8
	Tuple *showseconds_tup = dict_find(iter, KEY_SHOW_SECONDS); // int8
	

  if (ready_tup) { // Wait for JS to be ready before requesting weather in selected language
  	APP_LOG(APP_LOG_LEVEL_INFO, "JS reports ready");
  	ready = 1;

  	//sendLang(langCodes[lang]);
		update_weather();
  }

  

  if (text_color_tup) {
    int text_color = text_color_tup->value->int32;
    APP_LOG(APP_LOG_LEVEL_INFO, "KEY_TEXT_COLOR received!");

    persist_write_int(KEY_TEXT_COLOR, text_color);
    #ifdef PBL_COLOR
    	set_text_color(text_color);
    #endif
  }

  if (background_color_tup) {
  	int bg_color = background_color_tup->value->int32;
  	APP_LOG(APP_LOG_LEVEL_INFO, "KEY_BACKGROUND_COLOR received! - %d", bg_color);

  	persist_write_int(KEY_BACKGROUND_COLOR, bg_color);
  	#ifdef PBL_COLOR
    	set_background_color(bg_color);
    #endif
  }

  if (invert_colors_tup) {
  	APP_LOG(APP_LOG_LEVEL_INFO, "KEY_INVERT_COLORS received!");
    invert_colors = invert_colors_tup->value->int8;


    persist_write_int(KEY_INVERT_COLORS, invert_colors);

    #ifdef PBL_BW
    	inverter();
    #endif
  }

  if (use_celsius_tup) {
  	APP_LOG(APP_LOG_LEVEL_INFO, "KEY_USE_CELSIUS received!");

  	use_celsius = use_celsius_tup->value->int8;

  	persist_write_int(KEY_USE_CELSIUS, use_celsius);
		
		vibes_short_pulse();
  }

  if (shake_for_weather_tup) {
  	APP_LOG(APP_LOG_LEVEL_INFO, "KEY_SHAKE_FOR_WEATHER received!");

  	shake_for_weather = shake_for_weather_tup->value->int8;

  	persist_write_int(KEY_SHAKE_FOR_WEATHER, shake_for_weather);
  }

  if (temperature_tup) {
  	APP_LOG(APP_LOG_LEVEL_INFO, "KEY_TEMPERATURE received!");

  	snprintf(temp_buffer, sizeof(temp_buffer), "%d°", (int)temperature_tup->value->int32);
  }

  if (temperature_in_c_tup) {
  	APP_LOG(APP_LOG_LEVEL_INFO, "KEY_TEMPERATURE_IN_C received!");

  	snprintf(temp_c_buffer, sizeof(temp_c_buffer), "%d°", (int)temperature_in_c_tup->value->int32);
  }

  if (conditions_tup) {
		APP_LOG(APP_LOG_LEVEL_INFO, "KEY_CONDITIONS received!");

		snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", conditions_tup->value->cstring);
		text_layer_set_text(conditions_layer, conditions_buffer);
		text_layer_set_text(conditions_layer_unanimated, conditions_buffer);
		
		size_weather_layers(1);
  }
	
	if (largefont_tup) {
		large_font = largefont_tup->value->int8;
		APP_LOG(APP_LOG_LEVEL_INFO, "KEY_LARGE_FONT received! - %d", large_font);
		
		persist_write_int(KEY_LARGE_FONT, large_font);	
		
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
		
		size_weather_layers(2);
	}

  if (vibe_on_connect_tup) {
  	APP_LOG(APP_LOG_LEVEL_INFO, "KEY_VIBE_ON_CONNECT received!");
  	vibe_on_connect = vibe_on_connect_tup->value->int8;
  }

  if (vibe_on_disconnect_tup) {
  	APP_LOG(APP_LOG_LEVEL_INFO, "KEY_VIBE_ON_DISCONNECT received!");
  	vibe_on_disconnect = vibe_on_disconnect_tup->value->int8;
  }

  if (use_celsius == 1) {
		APP_LOG(APP_LOG_LEVEL_INFO, "Using Celsius and getting temp size");
  	text_layer_set_text(temp_layer, temp_c_buffer);
  	text_layer_set_text(temp_layer_unanimated, temp_c_buffer);
  } else {
		APP_LOG(APP_LOG_LEVEL_INFO, "Using Fahrenheit and getting temp size");
  	text_layer_set_text(temp_layer, temp_buffer);
  	text_layer_set_text(temp_layer_unanimated, temp_buffer);
  }

  if (reflect_batt_tup) {
  	APP_LOG(APP_LOG_LEVEL_INFO, "KEY_REFLECT_BATT received!");

  	reflect_batt = reflect_batt_tup->value->int8;

  	persist_write_int(KEY_REFLECT_BATT, reflect_batt);
  }

  if (date_format_tup) {
  	APP_LOG(APP_LOG_LEVEL_INFO, "KEY_DATE_FORMAT received!");

  	if (strcmp(date_format_tup->value->cstring, "edate") == 0) {
  		APP_LOG(APP_LOG_LEVEL_INFO, "Using european date");
  		euro_date = 1;
  		persist_write_int(KEY_DATE_FORMAT, euro_date);
  	} else {
  		APP_LOG(APP_LOG_LEVEL_INFO, "Using standard date");
  		euro_date = 0;
  		persist_write_int(KEY_DATE_FORMAT, euro_date);
  	}
  }	
	
	if (showseconds_tup) {
		show_seconds = showseconds_tup->value->int8;
		APP_LOG(APP_LOG_LEVEL_INFO, "KEY_SHOW_SECONDS received! - %d", show_seconds);
		
		if (show_seconds == 1) {
			APP_LOG(APP_LOG_LEVEL_INFO, "Subscribing to seconds");
			tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
		} else {
			APP_LOG(APP_LOG_LEVEL_INFO, "Subscribing to minutes");
			tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
		}
		
		if (show_seconds == 1) {
			text_layer_set_font(time_layer, small_time_font);
		} else {
			text_layer_set_font(time_layer, time_font);
		}
		
		persist_write_int(KEY_SHOW_SECONDS, show_seconds);
		
		layer_mark_dirty(text_layer_get_layer(time_layer));
		GRect bounds = layer_get_bounds(window_get_root_layer(main_window));
		GSize time_size = text_layer_get_content_size(time_layer);
		layer_set_frame(text_layer_get_layer(time_layer), GRect(0, ((bounds.size.h / 2) + 5 - time_size.h), bounds.size.w, time_size.h));
	}
	
	if (lang_tup) {
  	APP_LOG(APP_LOG_LEVEL_INFO, "KEY_LANGUAGE received!");
  	if (strcmp(lang_tup->value->cstring, "en") == 0) {
  		APP_LOG(APP_LOG_LEVEL_INFO, "Using English");
  		lang = 0;
  	} else if (strcmp(lang_tup->value->cstring, "fr") == 0){
  		APP_LOG(APP_LOG_LEVEL_INFO, "Using French");
  		lang = 1;
  	} else if (strcmp(lang_tup->value->cstring, "es") == 0){
  		APP_LOG(APP_LOG_LEVEL_INFO, "Using Spanish");
  		lang = 2;
  	} else if (strcmp(lang_tup->value->cstring, "de") == 0){
  		APP_LOG(APP_LOG_LEVEL_INFO, "Using German");
  		lang = 3;
  	} else {
  		lang = 0;
  	}
  	//sendLang(lang_tup->value->cstring);

  	persist_write_int(KEY_LANGUAGE, lang);
  }
	
	if (show_weather_tup) {
  	APP_LOG(APP_LOG_LEVEL_INFO, "KEY_SHOW_WEATHER received!");
		
		bool old_show_weather = show_weather;

  	show_weather = show_weather_tup->value->int8;
		
		if (old_show_weather == 0 && show_weather == 1) {
			update_weather();
		}

  	persist_write_int(KEY_SHOW_WEATHER, show_weather);
  }
	
	APP_LOG(APP_LOG_LEVEL_INFO, "All keys received");

  update_layers();
  update_time();
}

void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped! - %d", reason);
}

void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}
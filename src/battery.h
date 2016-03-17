#include <pebble.h>
#pragma once

TextLayer *s_charge_layer;
Layer *s_batt_layer, *s_static_layer;

void charge_handler();
void battery_handler(BatteryChargeState state);
void batt_layer_draw(Layer *layer, GContext *ctx);
void static_layer_draw(Layer *layer, GContext *ctx);
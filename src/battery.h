#include <pebble.h>
#pragma once

Layer *batt_layer, *static_layer;

void battery_handler(BatteryChargeState state);
void batt_layer_draw(Layer *layer, GContext *ctx);
void static_layer_draw(Layer *layer, GContext *ctx);
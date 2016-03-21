#include <pebble.h>
#pragma once

Layer *bar_layer, *static_layer;

void battery_handler(BatteryChargeState state);
void bar_layer_draw(Layer *layer, GContext *ctx);
void static_layer_draw(Layer *layer, GContext *ctx);

int bar_setting;
int steps;
int step_goal;
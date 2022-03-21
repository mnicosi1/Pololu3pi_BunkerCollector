#ifndef OUR_FUNCTIONS_H
#define OUR_FUNCTIONS_H
#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/io.h>
#include "lcd_driver.h"
#include "port_macros.h"

int speed_pulser();
int bunker_pulser();
int forward_speed(int speed_index, int bunker_count);
void turn_around(int speed_index);
int bunker_sensor(int speed_index);
#endif

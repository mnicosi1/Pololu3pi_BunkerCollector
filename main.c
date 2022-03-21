#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/io.h>
#include "lcd_driver.h"
#include "port_macros.h"
#include "our_functions.h"
#define Buttons PINB
#define one_motor_B PORTB
#define three_motors_D PORTD
#define LEFT_BUTTON 1
#define MIDDLE_BUTTON 4
#define RIGHT_BUTTON 5
#define M1_LEFT 5
#define M1_RIGHT 6
#define M2_LEFT 3
#define M2_RIGHT 3
#define LEFT_SENSOR 0
#define LEFT_MID_SENSOR 1
#define MID_SENSOR 2
#define RIGHT_MID_SENSOR 3
#define RIGHT_SENSOR 4

int main(){

	//Seting buttons and sensors to inputs, and motors to outputs through masking DDRB, DDRD, and DDRC
	DDRB &= (~(1<<LEFT_BUTTON)|~(1<<MIDDLE_BUTTON)|~(1<<RIGHT_BUTTON));
	DDRB |= (1<<M2_RIGHT);
	DDRD |= ((1<<M2_LEFT) | (1<<M1_LEFT) | (1<<M1_RIGHT));
	DDRC &= ~((1<<LEFT_SENSOR) | (1<<LEFT_MID_SENSOR) | (1<<MID_SENSOR) | (1<<RIGHT_MID_SENSOR) | (1<<RIGHT_SENSOR));

	//Setting the buttons and sensors to have pull-up resistors
	PORTB |= (1<<LEFT_BUTTON)|(1<<MIDDLE_BUTTON)|(1<<RIGHT_BUTTON);
	PORTC |= (1<<LEFT_SENSOR) | (1<<LEFT_MID_SENSOR) | (1<<MID_SENSOR) | (1<<RIGHT_MID_SENSOR) | (1<<RIGHT_SENSOR);

	// initialize command arrays, both for custom commands, and the choices for commands, also initialize variables we need later
	char speed[][7] = {"SLOW", "MEDIUM", "FAST"};
	char bunker_number[][3] = {"1", "2", "3", "4", "5"};
	unsigned int last_c_state = 0;
	int lm_sensor_change = 0;
	int bunker_counter = 0;
	int sensor_state = 0;
	
	//Initial state of Robot, asks for speed and number of bunkers
	initialize_LCD_driver();
	LCD_execute_command(TURN_ON_DISPLAY);
	LCD_execute_command(CLEAR_DISPLAY);

	LCD_print_String("Speed:");
	_delay_ms(1000);
	LCD_execute_command(CLEAR_DISPLAY);

	LCD_print_String(speed[0]);
	int speed_index = speed_pulser();
	_delay_ms(1000);

	LCD_execute_command(CLEAR_DISPLAY);
	LCD_print_String("# Bunkers:");
	_delay_ms(1000);
	LCD_execute_command(CLEAR_DISPLAY);

	LCD_print_String(bunker_number[0]);
	int bunker_index = bunker_pulser();
	_delay_ms(1000);

	LCD_execute_command(CLEAR_DISPLAY);
	LCD_print_String("Done");
	LCD_execute_command(CLEAR_DISPLAY);
	
	//Driving code, will take user input and pass through to functions, also checks for victory condition
	while(1){
		if(bunker_counter < bunker_index){
			sensor_state = forward_speed(speed_index, bunker_counter);
			if(sensor_state == 1){
				bunker_counter++;
			}
			else{
				continue;
			}	
		}
		//Victory "dance"
		if(bunker_counter == bunker_index){
			three_motors_D |= ((1<<M1_RIGHT) | (1<<M1_LEFT) | (1<<M2_LEFT));
			one_motor_B |= (1<<M2_RIGHT);
			LCD_execute_command(CLEAR_DISPLAY);
			LCD_print_String("YOU WIN");
			_delay_ms(5000);	
			break;
		}
	}
	return 0;
}

#include "our_functions.h"
#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/io.h>
#include "lcd_driver.h"
#include "port_macros.h"
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

int speed_pulser(){
	//This is the logic for the robot asking for which speed command the user wants to input, utilizes pulser for more accurate button presses
	unsigned int last_left_button_state = (Buttons & (1<<LEFT_BUTTON));
	unsigned int left_button_pressed;
	unsigned int last_middle_button_state = (Buttons & (1<<MIDDLE_BUTTON));
	unsigned int middle_button_pressed;
	unsigned int last_right_button_state = (Buttons & (1<<RIGHT_BUTTON));
	unsigned int right_button_pressed;
	int parameter_index = 0;
	char speed[][7] = {"SLOW", "MEDIUM", "FAST"};

	//Same idea as movement pulser (Left goes down, middle goes up, right returns the value and moves on to asking for time
	while(1){
	//Pulser for Left Button
	if( (Buttons & (1<<LEFT_BUTTON)) != last_left_button_state ){
		if( (Buttons & (1<<LEFT_BUTTON)) == 0 ){
			left_button_pressed=1;
			parameter_index -=1;
			if (parameter_index < 0){
				parameter_index = 2;
			}
			LCD_execute_command(CLEAR_DISPLAY);
			LCD_print_String(speed[parameter_index]);
			}
			last_left_button_state = (Buttons & (1<<LEFT_BUTTON));
		}
		else{
			left_button_pressed=0;
		}
	//Pulser for middle button
	if( (Buttons & (1<<MIDDLE_BUTTON)) != last_middle_button_state ){
		if( (Buttons & (1<<MIDDLE_BUTTON)) == 0 ){
			middle_button_pressed=1;
			parameter_index += 1;
			if (parameter_index > 2){
				parameter_index = 0;
			}
			LCD_execute_command(CLEAR_DISPLAY);
			LCD_print_String(speed[parameter_index]);

		}
		last_middle_button_state = (Buttons & (1<<MIDDLE_BUTTON));
		}
		else{
			middle_button_pressed=0;
		}
	//Pulser for right button
	if( (Buttons & (1<<RIGHT_BUTTON)) != last_right_button_state ){
		if( (Buttons & (1<<RIGHT_BUTTON)) == 0 ){
			right_button_pressed=1;
			LCD_execute_command(CLEAR_DISPLAY);

		}
		last_right_button_state = (Buttons & (1<<RIGHT_BUTTON));
		}
		else{
			right_button_pressed=0;
		}
	if(right_button_pressed == 1){
		return parameter_index;
	}
}
}

int bunker_pulser(){
	//Same pulser as above but this is for the robots initial asking of how many bunkers the robot needs to find
	unsigned int last_left_button_state = (Buttons & (1<<LEFT_BUTTON));
	unsigned int left_button_pressed;
	unsigned int last_middle_button_state = (Buttons & (1<<MIDDLE_BUTTON));
	unsigned int middle_button_pressed;
	unsigned int last_right_button_state = (Buttons & (1<<RIGHT_BUTTON));
	unsigned int right_button_pressed;
	int parameter_index = 0;
	char command_number[][3] = {"1", "2", "3", "4", "5"};

	while(1){
	//Pulser for left button
	if( (Buttons & (1<<LEFT_BUTTON)) != last_left_button_state ){
		if( (Buttons & (1<<LEFT_BUTTON)) == 0 ){
			left_button_pressed=1;
			parameter_index -=1;
			if (parameter_index < 0){
				parameter_index = 4;
			}
			LCD_execute_command(CLEAR_DISPLAY);
			LCD_print_String(command_number[parameter_index]);
			}
			last_left_button_state = (Buttons & (1<<LEFT_BUTTON));
		}
		else{
			left_button_pressed=0;
		}
	//Pulser for middle button
	if( (Buttons & (1<<MIDDLE_BUTTON)) != last_middle_button_state ){
		if( (Buttons & (1<<MIDDLE_BUTTON)) == 0 ){
			middle_button_pressed=1;
			parameter_index += 1;
			if (parameter_index > 4){
				parameter_index = 0;
			}
			LCD_execute_command(CLEAR_DISPLAY);
			LCD_print_String(command_number[parameter_index]);

		}
		last_middle_button_state = (Buttons & (1<<MIDDLE_BUTTON));
		}
		else{
			middle_button_pressed=0;
		}
	//Pulser for right button
	if( (Buttons & (1<<RIGHT_BUTTON)) != last_right_button_state ){
		if( (Buttons & (1<<RIGHT_BUTTON)) == 0 ){
			right_button_pressed=1;
			LCD_execute_command(CLEAR_DISPLAY);

		}
		last_right_button_state = (Buttons & (1<<RIGHT_BUTTON));
		}
		else{
			right_button_pressed=0;
		}
	if(right_button_pressed == 1){
		return (parameter_index + 1);
	}
}
}

void turn_around(int speed_index){
	//This function turns the robot around until the sensors are all not on black
	unsigned int pwm_counter = 0;
	unsigned int duty_cycle = 0;
	unsigned int PWM_TOP = 100;
	unsigned int last_c_state = ((1<<LEFT_MID_SENSOR)| (1<<MID_SENSOR) | (1<<RIGHT_MID_SENSOR));

	if(speed_index == 0) {
		duty_cycle = 45;
	}

	if(speed_index == 1) {
		duty_cycle = 50;
	}

	if(speed_index == 2) {
		duty_cycle = 55;
	}

	while(1){
		if( (PINC & ((1<<LEFT_SENSOR) | (1<<LEFT_MID_SENSOR) | (1<<MID_SENSOR) | (1<<RIGHT_MID_SENSOR) | (1<<RIGHT_SENSOR))) != last_c_state ){
			if( (PINC & ((1<<LEFT_SENSOR) | (1<<LEFT_MID_SENSOR) | (1<<MID_SENSOR) | (1<<RIGHT_MID_SENSOR) | (1<<RIGHT_SENSOR))) == 0){
				break;
			}
		}
		//PWM Counter
		pwm_counter = pwm_counter + 1;
		if( pwm_counter >= PWM_TOP ){
			pwm_counter = 0;
		}
		if( pwm_counter < duty_cycle ){
			three_motors_D |= ((1<<M1_RIGHT) | (1<<M2_LEFT));
			three_motors_D &= ~(1<<M1_LEFT);
			one_motor_B &= ~(1<<M2_RIGHT);
		}
		else{
			three_motors_D &= ~((1<<M1_RIGHT) | (1<<M1_LEFT) | (1<<M2_LEFT));
			one_motor_B &= ~(1<<M2_RIGHT);
		}
	}
	three_motors_D |= ((1<<M1_RIGHT) | (1<<M1_LEFT) | (1<<M2_LEFT));
	one_motor_B |= (1<<M2_RIGHT);

}

int bunker_sensor(int speed_index){
	//This function adjusts the robot in order to determine whether the robot is over the border (0), or a bunker (1). Utilizes the number of sensors that detect black
	unsigned int pwm_counter = 0;
	unsigned int duty_cycle = 0;
	unsigned int PWM_TOP = 100;
	unsigned int last_c_state = ((1<<LEFT_MID_SENSOR)| (1<<MID_SENSOR) | (1<<RIGHT_MID_SENSOR));

	if(speed_index == 0) {
		duty_cycle = 45;
	}

	if(speed_index == 1) {
		duty_cycle = 50;
	}

	if(speed_index == 2) {
		duty_cycle = 55;
	}

	while(1){

		//PWM Counter
		pwm_counter = pwm_counter + 1;
		if( pwm_counter >= PWM_TOP ){
			pwm_counter = 0;
		}
		// This checks if sensor 1 is high, and if so it turns counter clockwise, if other sensors are high it's a border otherwise bunker
		if( pwm_counter < duty_cycle ){
			if((PINC & (1<<LEFT_MID_SENSOR)) ==(1<<LEFT_MID_SENSOR) ){
				while(1){
					three_motors_D |= (1<<M1_LEFT);
					three_motors_D &= ~((1<<M2_LEFT) | (1<<M1_RIGHT));
					one_motor_B |= (1<<M2_RIGHT);
					_delay_ms(30);
					if(PINC & (1<<LEFT_MID_SENSOR)|(1<<MID_SENSOR) == (1<<LEFT_MID_SENSOR)|(1<<MID_SENSOR)){
						if((PINC & (1<<LEFT_SENSOR) == (1<<LEFT_SENSOR)) || (PINC & (1<<RIGHT_SENSOR) == (1<<RIGHT_SENSOR))){
							return 1;
						}
						if((PINC & ((1<<LEFT_SENSOR)|(1<<RIGHT_SENSOR))) == 0){
							return 0;
						}
					}
				}
			}
			// This checks if sensor 3 is high, and if so it turns clockwise, if other sensors are high it's a border otherwise bunker
			if((PINC & (1<<RIGHT_MID_SENSOR)) ==(1<<RIGHT_MID_SENSOR) ){
				while(1){
					three_motors_D &= ~(1<<M1_LEFT);
					three_motors_D |= (1<<M2_LEFT)|(1<<M1_RIGHT);
					one_motor_B &= ~(1<<M2_RIGHT);
					_delay_ms(30);
					if(PINC & (1<<MID_SENSOR)|(1<<RIGHT_MID_SENSOR) == (1<<MID_SENSOR)|(1<<RIGHT_MID_SENSOR)){
						if((PINC & (1<<LEFT_SENSOR) == (1<<LEFT_SENSOR)) || (PINC & (1<<RIGHT_SENSOR) == (1<<RIGHT_SENSOR))) {
							return 1;
						}
						if((PINC & ((1<<LEFT_SENSOR)|(1<<RIGHT_SENSOR))) == 0){
							return 0;
						}
					}
				}
			}
			if((PINC & ((1<<LEFT_SENSOR) | (1<<RIGHT_SENSOR))) == ((1<<LEFT_SENSOR) | (1<<RIGHT_SENSOR))) {
				return 1;
			}
		}
		else{
			three_motors_D &= ~((1<<M1_RIGHT) | (1<<M1_LEFT) | (1<<M2_LEFT));
			one_motor_B &= ~(1<<M2_RIGHT);
		}
	}
	three_motors_D |= ((1<<M1_RIGHT) | (1<<M1_LEFT) | (1<<M2_LEFT));
	one_motor_B |= (1<<M2_RIGHT);

}



int forward_speed(int speed_index, int bunker_count){
	//Main Driving function that uses other functions and returns a number in order to keep track of how many bunkers we have
	unsigned int pwm_counter = 0;
	unsigned int duty_cycle = 0;
	unsigned int PWM_TOP = 100;
	unsigned int last_c_state = 0;
	char bunker_numbers[][2] = {"0", "1", "2", "3", "4", "5"};
	//Check for speed, if higher then longer duty cycle meaning faster pace and vice versa
	if(speed_index == 0) {
		duty_cycle = 50;
	}

	if(speed_index == 1) {
		duty_cycle = 53;
	}

	if(speed_index == 2) {
		duty_cycle = 56;
	}

	while(1){
		if( ((PINC & (1<<LEFT_MID_SENSOR)) || (PINC & (1<<MID_SENSOR)) || (PINC & (1<<RIGHT_MID_SENSOR))) != last_c_state ) {
			three_motors_D |= ((1<<M1_RIGHT) | (1<<M1_LEFT) | (1<<M2_LEFT));
			one_motor_B |= (1<<M2_RIGHT);
			int border_status = bunker_sensor(speed_index);
			//After the robot is stopped and the bunker vs border is determined, a border will lead it to return 0 and turn around
			if(border_status == 1){
				LCD_execute_command(CLEAR_DISPLAY);
				LCD_print_String("Border");
				LCD_print_String(bunker_numbers[bunker_count]);
				three_motors_D |= ((1<<M1_RIGHT) | (1<<M1_LEFT) | (1<<M2_LEFT));
				one_motor_B |= (1<<M2_RIGHT);
				turn_around(speed_index);
				return 0;
			}
			//After the robot is stopped and the bunker vs border is determined, a bunker will lead it to return 1 and turn around
			if(border_status == 0){
				LCD_execute_command(CLEAR_DISPLAY);
				LCD_print_String("Bunker");
				LCD_print_String(bunker_numbers[bunker_count]);
				three_motors_D |= ((1<<M1_RIGHT) | (1<<M1_LEFT) | (1<<M2_LEFT));
				one_motor_B |= (1<<M2_RIGHT);
				_delay_ms(1000);
				turn_around(speed_index);
				return 1;
			}
		}
		//PWM Counter
		pwm_counter = pwm_counter + 1;
		if( pwm_counter >= PWM_TOP ){
			pwm_counter = 0;
		}
		if( pwm_counter < duty_cycle ){
			three_motors_D |= (1<<M1_RIGHT);
			three_motors_D &= ~((1<<M2_LEFT) | (1<<M1_LEFT));
			one_motor_B |= (1<<M2_RIGHT);
		}
		else{
			three_motors_D &= ~((1<<M1_RIGHT) | (1<<M1_LEFT) | (1<<M2_LEFT));
			one_motor_B &= ~(1<<M2_RIGHT);
		}
	}
}

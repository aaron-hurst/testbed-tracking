/*Standard includes*/
#include <math.h>	// sqrt, atan2, pow

/*External libraries*/
#include <opencv2/opencv.hpp>	// cv::getTickFrequency()

/*Project includes*/
#include "config.h"
#include "car.h"
#include "time.h"

#define PI 3.14159265

void Car::update_state(float px_x, float px_y, Config conf, Time time_val)
{
	/*Update position: convert from pixel values to mm*/
	position_new[0] = conf.scale*(px_x - conf.origin[0]);
	position_new[1] = conf.scale*(px_y - conf.origin[1]);
	
	/*Update velocity*/
	if (found_old) {
		/*Calculate velocity based on previous instance's values*/
		velocity_new[0] = (position_new[0] - position_old[0])/(time_val.current - time_val.old)*(cv::getTickFrequency());
		velocity_new[1] = (position_new[1] - position_old[1])/(time_val.current - time_val.old)*(cv::getTickFrequency());
	} else {
		/*No data on previous position, set velocity to zero*/
		velocity_new[0] = 0.0;
		velocity_new[1] = 0.0;
	}
	
	/*Update speed*/
	speed = sqrt(pow(velocity_new[0], 2) + pow(velocity_new[1], 2));
	
	/*Update orientation*/
	if (speed > conf.min_speed)	{
		/*Calculate orientation based on previous instance's values*/
		orientation_new = (int)(90 - 180/PI*atan2(velocity_new[1], velocity_new[0]));
		if (orientation_new < 0) {
			orientation_new += 360;
		}
	} else {
		/*Velocity is too low to reliably estimate orientation, retail previous value*/
		orientation_new = orientation_old;
	}
}

void Car::update_state_lost(void)
{
	position_new[0] = 0;
	position_new[1] = 0;
	velocity_new[0] = 0;
	velocity_new[1] = 0;
	speed			= 0;
	orientation_new = 0;
}

void Car::state_new_to_old(void)
{
	position_old[0] = position_new[0];
	position_old[1] = position_new[1];
	velocity_old[0] = velocity_new[0];
	velocity_old[1] = velocity_new[1];
	orientation_old = orientation_new;
	found_old = found;
}

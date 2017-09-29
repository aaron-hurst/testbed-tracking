#include <math.h>	// sqrt, atan2, pow
#include "car.h"
#include "time.h"

#define PI 		3.14159265


/**********************************************************************************
* Update car state
* 
* Updates car position based on provided position within image and calculates
* veolcity and orientation based on current and past state.
* Should only be called after checking that the car has been found in the current
* frame. If it is not found, call update_state_lost instead.
* 
* px_x		Car x-coordinate in image
* px_y		Car x-coordinate in image
* sys_conf	Config struct containing global configuration parameters
* sys_time	Time struct containing current, previous and start time
*
* return: void
**********************************************************************************/
void Car::update_state(float px_x, float px_y, int origin[2], float scale, int min_speed, Time sys_time)
{
	// Update position, converting from pixel values to mm
	position_new[0] = scale*(px_x - origin[0]);
	position_new[1] = scale*(px_y - origin[1]);
	
	// Update velocity
	if (Car::found_old)
	{
		// Calculate velocity based on previous instance's values
		velocity_new[0] = (position_new[0] - position_old[0])/(sys_time.current - sys_time.old);
		velocity_new[1] = (position_new[1] - position_old[1])/(sys_time.current - sys_time.old);
	}
	else
	{
		// No data on previous position, set velocity to zero
		velocity_new[0] = 0.0;
		velocity_new[1] = 0.0;
	}
	
	// Update speed
	speed = sqrt(pow(velocity_new[0], 2) + pow(velocity_new[1], 2));
	
	// Update orientation
	if (speed > min_speed)
	{
		// Calculate orientation based on previous instance's values
		orientation_new = (int)(90 - 180/PI*atan2(velocity_new[1], velocity_new[0]));
		if (orientation_new < 0)
		{
			orientation_new += 360;
		}
	}
	else
	{
		// Velocity is too low to reliably estimate orientation, set to zero
		orientation_new = 0;
	}
	
	return;
}


/**********************************************************************************
* Set car state to zero
* 
* Call this function if car is not found in a given frame (instance) to set its
* state (position, velocity and orientation) to zero
*
* return: void
**********************************************************************************/
void Car::update_state_lost(void)
{
	position_new[0] = 0;
	position_new[1] = 0;
	velocity_new[0] = 0;
	velocity_new[1] = 0;
	speed			= 0;
	orientation_new = 0;
	return;
}


/**********************************************************************************
* Move new state to old
* 
* Call at the end of processing an image (instance) to copy current state to 'old'
* state for use in the subsequent instance.
*
* return: void
**********************************************************************************/
void Car::state_new_to_old(void)
// Update old variables with new data
{
	position_old[0] = position_new[0];
	position_old[1] = position_new[1];
	velocity_old[0] = velocity_new[0];
	velocity_old[1] = velocity_new[1];
	orientation_old = orientation_new;
	found_old = found;
	return;
}


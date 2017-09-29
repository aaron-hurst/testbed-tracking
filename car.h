#ifndef CARSCV_CAR_H
#define CARSCV_CAR_H

#include <string>
#include "time.h"
#include "config.h"


/**********************************************************************************
* Car Data Structure
**********************************************************************************/
struct Car
{
	// Identifiers
	std::string name;
	std::string mac_add;	// MAC Address
	
	// Colour attributes
	int hue;
	int delta;
	
	// State 
	bool found;
	bool found_old;
	float position_old[2];
	float position_new[2];
	float velocity_old[2];
	float velocity_new[2];
	float speed;
	int orientation_old;
	int orientation_new;
	
	// Default values
	Car () : found(false), found_old(false) {}
	
	//  Member functions
	void update_state(float px_x, float px_y, int origin[2], float scale, int min_speed, Time sys_time);
	void update_state_lost(void);
	void state_new_to_old(void);
};



#endif
#ifndef CARSCV_CAR_H
#define CARSCV_CAR_H

/*General includes*/
#include <string>

/*Forward declarations*/
struct Time;
struct Config;

//=====================================
/*! @struct Car data structure
 * 
 * Struct for storing car configuration and state (kinematic) data
 */
struct Car
{
	/*Identifiers*/
	std::string name;
	std::string mac_add;	// MAC Address
	
	/*Colour attributes*/
	int hue;
	int delta;
	
	/*State*/ 
	bool found;
	bool found_old;
	float position_old[2];
	float position_new[2];
	float velocity_old[2];
	float velocity_new[2];
	float speed;
	int orientation_old;
	int orientation_new;
	
	/*Default values*/
	Car () : found(false), found_old(false) {}
	
	//=====================================
	/*! @brief Update car object state
	 *
	 * Updates car position based on provided position within image 
	 * Calculates veolcity and orientation based on current and
	 * previous state.
	 * Should only be called after checking that the car has been
	 * found in the current frame. If it is not found, call
	 * update_state_lost instead.
	 * 
	 * @param px_x Car x-coordinate in image
	 * @param px_y Car x-coordinate in image
	 * @param conf Struct containing general configuration parameters
	 * @param time Struct containing key timestamps
	 *
	 * @return void
	 */
	void update_state(float, float, Config, Time);

	//=====================================
	/*! @brief Set car object state to zero
	 *
	 * Call this function if car is not found in a given frame
	 * (instance) to set its state (position, velocity and
	 * orientation) to zero.
	 * 
	 * @return void
	 */
	void update_state_lost(void);

	//=====================================
	/*! @brief Move new state to old
	 *
	 * Call this function after processing an image (instance). It 
	 * coppies the car's 'new' state to its 'old' state for use in 
	 * the subsequent instance.
	 * 
	 * @return void
	 */
	void state_new_to_old(void);
};


#endif

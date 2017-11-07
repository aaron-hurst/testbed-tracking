#ifndef CARSCV_CONFIG_H
#define CARSCV_CONFIG_H

/*General includes*/
#include <vector>

/*External libraries*/
#include <opencv2/opencv.hpp>

/*Project includes*/
#include "config.h"

/*Forward declarations*/
struct Car;

//=====================================
/*! @struct Configuration information
 */
struct Config
{
	/*Detection mode*/
	int detect_mode;
	
	/*Background*/
	bool get_new_background;
	int diff_threshold;

	/*Camera*/
	float shutter;
	bool auto_shutter;
	int image_w;
	int image_h;
	int crop_n;
	int crop_e;
	int crop_s;
	int crop_w;
	
	/*Image*/
	int min_sat;
	int min_val;
	int origin[2];
	float scale;
	int car_size_max;
	int car_size_min;

	/*Histogram comparison*/
	float chi2_dist_max;
	float intersect_min;
	
	/*Other*/
	int min_speed;

	//=====================================
	/*! @brief Reads config information from config file
	 *
	 * Reads and parses general configuration information from the 
	 * config file, storing it in a Config struct.
	 * 
	 * @return 0 on success, 1 on failure
	 */
	int read_config(void);

	//=====================================
	/*! @brief Prints config information to the console
	 *
	 * Prints general configuration information to the console.
	 * 
	 * @return void
	 */
	void print_config(void);
};

#endif /*CARSCV_CONFIG_H*/

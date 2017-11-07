#ifndef CARSCV_CONFIG_H
#define CARSCV_CONFIG_H

#include <vector>

#include <opencv2/opencv.hpp>

#include "car.h"
#include "config.h"

/**********************************************************************************
* Configuration Data Structure
**********************************************************************************/
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
	int crop_l;
	int crop_r;
	int crop_t;
	int crop_b;

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
};


/**********************************************************************************
* Get Configuration
* 
* Reads and parses general and vehicle-specific configuration data from config.txt,
* storing it in a Config struct and one or more Car structs as required
* 
* cars_all	Vector for storing one or more Car structs
* sys_conf	Config struct for storing global configuration parameters
*
* return: 0 on success, 1 on failure
**********************************************************************************/
int set_config(std::vector<struct Car> &cars_all, Config &sys_conf);

/**********************************************************************************
* Show Configuration
* 
* Reads configuration data from sys_config struct and prints this to the console
* 
* sys_conf	Config struct containing global configuration parameters
*
* return: 0 on success, 1 on failure
**********************************************************************************/
void show_config(std::vector<struct Car> &cars_all, Config &sys_conf);


/**********************************************************************************
* Get cropping mask
* 
* Generates a mask for cropping camera images to remove the borders of the table.
* The mask can then be used in subsequent functions.
* 
* sys_conf	 Config struct containing global configuration parameters
* crop_mask	 binary mask to be used for applying cropping
*
* return: void
**********************************************************************************/
void get_cropping_mask(struct Config sys_conf, cv::Mat &crop_mask);


#endif
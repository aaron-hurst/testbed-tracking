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

/*Macros*/
#define MODE_LIVE 0
#define MODE_LIVE_CONS 1
#define MODE_LIVE_LOG 2
#define MODE_TEST 3
#define MODE_DEBUG 4

//=====================================
/*! @struct Configuration information
 * 
 * TODO: variable descriptions
 */
struct Config
{
	/*Detection mode*/
	int detect_mode;

	/*Operation*/
	int n_frames;
	int output_mode;
	int delay;
	
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
	/*! @brief Parse command line arguments
	 *
	 * Run this function after checking that the correct number of command
	 * line arguments are provided. It will parse the first three (number of
	 * frames, operation mode and delay) and update the associated config
	 * parameters.
	 * 
	 * @param argc Number of command line arguments
	 * @param argv Array of command line arguments
	 * 
	 * @return 0 on success, 1 on failure
	 */
	int parse_args(int, char**);

	//=====================================
	/*! @brief Print correct usage to console
	 *
	 * Prints instructions for correct usage of executable to console.
	 */
	void print_usage(void);

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

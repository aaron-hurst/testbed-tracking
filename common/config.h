#ifndef CARSCV_CONFIG_H
#define CARSCV_CONFIG_H

/*General includes*/
#include <vector>

/*External libraries*/
#include <opencv2/opencv.hpp>

/*Forward declarations*/
struct Car;
namespace raspicam {
    class RaspiCam_Cv;
}

/*Macros*/
#define MODE_LIVE 0
#define MODE_LIVE_CONS 1
#define MODE_LIVE_LOG 2
#define MODE_TEST 3
#define MODE_DEBUG 4

#define DETECT_MODE_HUE	0
#define DETECT_MODE_HIST 1

//=====================================
/*! @struct Configuration information
 * 
 * 
 */
//TODO: variable descriptions
//TODO: add cam_set flag to config (initialised after camera setup)
struct Config
{
	bool config_set;

	/*Operation*/
	int detect_mode;
	int n_frames;
	int output_mode;
	int delay;
	bool debug;
	
	/*Camera*/
	float shutter;
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
	int back_diff_threshold;

	/*Histogram comparison*/
	float chi2_dist_max;
	float intersect_min;
	
	/*Other*/
	int min_speed;

	/*Default values*/
	Config () : config_set(false) {}

	//=====================================
	/*! @brief Configuration entry-point
	 *
	 * Single entry-point function for configuration; manages calls to each
	 * individual configuration function
	 * 
	 * @param argc Number of command line arguments
	 * @param argv Array of command line arguments
	 * @param Camera Raspicam camera object
	 * @param cars_all Vector of car objects
	 * 
	 * @return 0 on success, 1 on failure
	 */
	int config_master(int, char**,
		raspicam::RaspiCam_Cv&,
		std::vector<struct Car>&);
	
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
	/*! @brief Prints config information
	 *
	 * Prints general configuration information to the specified pointer, which
	 * may be either stdout or a log file. Rudimentary checking is completed
	 * (to ensure pointer is not null); however, it is the calling function's
	 * responsibility to ensure a valid pointer.
	 * 
	 * @param pointer Pointer to either stdout or a log file
	 * 
	 * @return 0 on success, 1 on failure
	 */
	int print_config(FILE*);
};

#endif /*CARSCV_CONFIG_H*/

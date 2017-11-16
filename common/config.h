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
struct Config
{
	/*System operation*/
	int detect_mode;
	int n_frames;
	int output_mode;
	int delay;
	bool debug;

	/*System state*/
	bool config_set;
	int sock;
	int n_cars;

	/*Image processing*/
	int min_sat;
	int min_val;
	int crop_n;
	int crop_e;
	int crop_s;
	int crop_w;
	int car_size_max;
	int car_size_min;

	/*Car state estimation parameters*/
	int min_speed;
	int origin[2];
	float scale;
	
	/*Camera*/
	float shutter;
	int image_w;
	int image_h;

	/*Histogram comparison*/
	float chi2_dist_max;
	float intersect_min;
	int back_diff_threshold;
	
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
	/*! @brief Set output channels
	 *
	 * Depending on the output mode in use, may initialise network socket for
	 * communication to controller program and/or create log file and write
	 * configuration and data headers to it.
	 * 
	 * @param cars_all Vector of car objects
	 * 
	 * @return 0 on success, 1 on failure
	 */
	int output_setup(std::vector<struct Car>);

	//=====================================
	/*! @brief Print correct usage to console
	 *
	 * Prints instructions for correct usage of executable to console.
	 */
	void print_usage(void);

	//=====================================
	/*! @brief Prints config information
	 *
	 * Prints general configuration information to the specified pointer. This
	 * may be either stdout or a log file. Rudimentary checking is complete
	 * (to ensure pointer is not null and config is set); however, it is the
	 * calling function's responsibility to ensure a valid pointer.
	 * 
	 * @param pointer Pointer to either stdout or a log file
	 * 
	 * @return 0 on success, 1 on failure
	 */
	int print_config(FILE*);
};

#endif /*CARSCV_CONFIG_H*/

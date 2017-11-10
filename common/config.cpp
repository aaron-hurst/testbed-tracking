/*General includes*/
#include <iostream>		// cout
#include <string>		// to_string, string
#include <sstream>		// istringstream
#include <fstream>
#include <unistd.h>		// sleep

/*External libraries*/
#include </home/pi/raspicam-0.1.6/src/raspicam_cv.h>

/*Project includes*/
#include "outputs.h"
#include "config.h"
#include "camera.h"
#include "car_config.h"

/*Macros*/
#define FAILURE	1
#define SUCCESS	0

#define MIN_ARGS 4
#define MAX_ARGS 5

int Config::config_master(int argc, char** argv,
	raspicam::RaspiCam_Cv& Camera, std::vector<struct Car>& cars_all)
{
	/*Variables*/
	int ret = 0;	/*track function return values*/
	
	/*Load and set system and car configuration parameters*/
	ret = read_config();
	ret += cars_config_read(cars_all);
	if (ret != SUCCESS) {
		std::cout<<"ERROR CRITICAL: Unable to parse configuration file."<<std::endl;
		return FAILURE;
	}

	/*Parse arguments*/
	ret = parse_args(argc, argv);
	if (ret != SUCCESS) {
		std::cout<<"ERROR CRITICAL: Unable to parse arguments."<<std::endl;
		print_usage();
		return FAILURE;
	}

	/*Camera setup*/
	cam_set(Camera, (*this));
	if (!Camera.open()) {
		std::cout << "Error opening camera" << std::endl;
		return FAILURE;
	}
	sleep(2);	/*wait for camera to warm up*/

	/*Get background image*/
	if (argc == MAX_ARGS) {
		if (strcmp(argv[4], "--back") == 0) {
			get_background(image_h, image_w, Camera);
		}
	} else if (detect_mode == DETECT_MODE_HIST) {
		FILE * background_file = fopen("background.png", "r");
		if (background_file == NULL) { 
			get_background(image_h, image_w, Camera);
		}
	}

	/*Set debug flag*/
	if (output_mode == MODE_DEBUG) {
		debug = true;
	} else {
		debug = false;
	}

	/*Print config to console if in debug mode*/
	if (debug) {
		print_config(stdout);
		cars_config_print(cars_all);
	}

	return SUCCESS;
}

int Config::read_config(void)
{
	/*Variables*/
	std::string line, name, value, tmp;
	
	/*Open config file*/
	std::ifstream conf_file("config.txt");
	if (!conf_file) {
		return FAILURE;
	}
	
	/*Parse config.txt*/
	while (getline(conf_file, line)) {
		std::istringstream line_stream(line);
		line_stream >> name;
		
		/*Skip invalid lines and comments*/
		if (line_stream.fail() || name[0] == '#')	continue;
		
		/*Detection mode*/
		if 		(name == "detect_mode")		line_stream >> detect_mode;
		
		/*Camera*/
		if      (name == "img_width")		line_stream >> image_w;
		else if (name == "img_height")		line_stream >> image_h;
		else if (name == "crop_n")			line_stream >> crop_n;
		else if (name == "crop_e")			line_stream >> crop_e;
		else if (name == "crop_s")			line_stream >> crop_s;
		else if (name == "crop_w")			line_stream >> crop_w;
		else if (name == "shutter")			line_stream >> shutter;
		
		/*Image*/
		else if (name == "min_sat")			line_stream >> min_sat;
		else if (name == "min_val")			line_stream >> min_val;
		else if (name == "origin_x")		line_stream >> origin[0];
		else if (name == "origin_y")		line_stream >> origin[1];
		else if (name == "scale")			line_stream >> scale;
		else if (name == "size_min")		line_stream >> car_size_min;
		else if (name == "size_max")		line_stream >> car_size_max;
		else if (name == "threshold")		line_stream >> back_diff_threshold;
		
		/*Histogram comparison*/
		if      (name == "chi2_dist_max")	line_stream >> chi2_dist_max;
		else if (name == "intersect_min")	line_stream >> intersect_min;
		
		/*Other*/
		if       (name == "min_speed")	line_stream >> min_speed;
	}
	
	config_set = true;	/*record config has been completed*/

	return SUCCESS;
}

int Config::parse_args(int argc, char** argv)
{
	/*Check for minimum number of arguments*/
	if (argc < MIN_ARGS) {
		return FAILURE;
	}

	/*Parse number of frames*/
	n_frames = atoi(argv[1]);
	if (n_frames < 0) {
		return FAILURE;
	}

	/*Parse output mode*/
	output_mode = atoi(argv[2]);
	if (output_mode != MODE_LIVE &&
		output_mode != MODE_LIVE_CONS &&
		output_mode != MODE_LIVE_LOG &&
		output_mode != MODE_TEST &&
		output_mode != MODE_DEBUG
	) {
		std::cout<<"WARNING: Invalid output mode specified."<<std::endl;
		std::cout<<"Using default: Mode 1: LIVE - JSON + console"<<std::endl;
		output_mode = MODE_LIVE_CONS;
	}

	/*Parse delay*/
	delay = atoi(argv[3]);
	if (delay < 0) {
		return FAILURE;
	} else if (delay > 200) {
		std::cout<<"WARNING: selected delay of "<<delay<<" may cause system to run excessively slowly"<<std::endl;
	}

	return SUCCESS;
}

void Config::print_usage(void)
{
	std::cout<<"Correct usage: "<<std::endl;
	std::cout<<"  ./tracker [frames] [mode] [delay] [[new background]]"<<std::endl;
	std::cout<<"where:"<<std::endl;
	std::cout<<"  [frames] number of frames to track (non-negative integer)"<<std::endl;
	std::cout<<"  [mode]   operating mode (0-4)"<<std::endl;
	std::cout<<"  [delay]  delay applied between frames in milliseconds (non-negative integer)"<<std::endl;
	std::cout<<"  [[back]] optional argument to get a new background image"<<std::endl;
	std::cout<<"                     specify --back to get a new background image"<<std::endl;
	std::cout<<"                     (only applicable for histogram detection)"<<std::endl	;
}

int Config::print_config(FILE* pointer)
{
	/*Check input pointer*/
	if (pointer == NULL) {
		return FAILURE;
	}
	
	/*Check config is set*/
	if (!config_set) {
		std::cout<<"ERROR: Cannot print config before it is set"<<std::endl;
		return FAILURE;
	}

	fprintf(pointer,"================================\n");
	fprintf(pointer,"Detection mode:");
	if (detect_mode == 0) {
		fprintf(pointer," hue matching\n\n");
	}
	else if (detect_mode == 1) {
		fprintf(pointer," histogram comparison\n");
		fprintf(pointer," chi2_dist_max:  %d\n", chi2_dist_max);
	}
	else {
		fprintf(pointer," ERROR: detection mode unknown\n");
	}
	fprintf(pointer,"Tracking operation:\n");
	fprintf(pointer," n_frames:      %d\n", n_frames);
	fprintf(pointer," output_mode:   %d\n", output_mode);
	fprintf(pointer," delay:         %d\n", delay);
	fprintf(pointer,"Background:\n");
	fprintf(pointer," threshold:     %d\n", back_diff_threshold);	//TODO: re-organise this
	fprintf(pointer,"Camera:\n");
	fprintf(pointer," img_w:         %d\n", image_w);
	fprintf(pointer," img_h:         %d\n", image_h);
	fprintf(pointer," crop_w:        %d\n", crop_w);
	fprintf(pointer," crop_e:        %d\n", crop_e);
	fprintf(pointer," crop_n:        %d\n", crop_n);
	fprintf(pointer," crop_s:        %d\n", crop_s);
	fprintf(pointer," shutter:       %1.2f\n", shutter);
	fprintf(pointer,"Image:\n");
	fprintf(pointer," min_sat:       %d\n", min_sat);
	fprintf(pointer," min_val:       %d\n", min_val);
	fprintf(pointer," origin_x:      %d\n", origin[0]);
	fprintf(pointer," origin_y:      %d\n", origin[1]);
	fprintf(pointer," scale:         %1.3f\n", scale);
	fprintf(pointer," size_min:      %d\n", car_size_min);
	fprintf(pointer," size_max:      %d\n", car_size_max);	
	fprintf(pointer,"Other:\n");
	fprintf(pointer," min_speed:     %d\n", min_speed);
	fprintf(pointer,"================================\n");

	return SUCCESS;
}

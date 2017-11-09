/*General includes*/
#include <iostream>		// cout
#include <string>		// to_string, string
#include <sstream>		// istringstream
#include <fstream>

/*Project includes*/
#include "outputs.h"
#include "config.h"
#include "camera.h"

/*Macros*/
#define FAILURE	1
#define SUCCESS	0

#define MIN_ARGS 4
#define MAX_ARGS 5

int Config::config_master(int argc, char** argv)
{
	
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
		else if (name == "auto_shutter") 	line_stream >> auto_shutter;
		
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

	/*Check config set*/
	//TODO: do not need this if movind background get out
	if (!config_set) {
		std::cout<<"ERROR: Config not set before parsing arguments"<<std::endl;
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

	//TODO: check return value of get_background
	//TODO(CRITICAL): move to separate function that is called after camera setup
	//TODO: add cam_set flag to config (initialised after camera setup)
	/*Background image - if requested*/
	if (argc == MAX_ARGS && strcmp(argv[4], "--back") == 0) {
		get_background(image_h, image_w);
	}
	/*Background image - if required but not present*/
	else if (detect_mode == DETECT_MODE_HIST) {
		FILE * background_file = fopen("background.png","r");
		if (background_file == NULL) { 
			get_background(image_h, image_w);
		}
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

void Config::print_config(void)
{
	printf("================================\n");
	printf("Detection mode:");
	if (detect_mode == 0) {
		printf(" hue\n\n");
	}
	else if (detect_mode == 1) {
		printf(" histogram\n");
		printf(" chi2_dist_max:  %d\n", chi2_dist_max);
	}
	else {
		printf(" ERROR: detection mode unknown\n");
	}
	printf("Tracking operation:\n");
	printf(" n_frames:      %d\n", n_frames);
	printf(" output_mode:   %d\n", output_mode);
	printf(" delay:         %d\n", delay);
	printf("Background:\n");
	printf(" threshold:     %d\n", back_diff_threshold);	//TODO: re-organise this
	printf("Camera:\n");
	printf(" img_w:         %d\n", image_w);
	printf(" img_h:         %d\n", image_h);
	printf(" crop_w:        %d\n", crop_w);
	printf(" crop_e:        %d\n", crop_e);
	printf(" crop_n:        %d\n", crop_n);
	printf(" crop_s:        %d\n", crop_s);
	printf(" shutter:       %1.2f\n", shutter);
	printf(" force shutter: %d\n", auto_shutter);
	printf("Image:\n");
	printf(" min_sat:       %d\n", min_sat);
	printf(" min_val:       %d\n", min_val);
	printf(" origin_x:      %d\n", origin[0]);
	printf(" origin_y:      %d\n", origin[1]);
	printf(" scale:         %1.3f\n", scale);
	printf(" size_min:      %d\n", car_size_min);
	printf(" size_max:      %d\n", car_size_max);	
	printf("Other:\n");
	printf(" min_speed:     %d\n", min_speed);
	printf("================================\n");
}

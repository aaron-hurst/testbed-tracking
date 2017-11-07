/*General includes*/
#include <iostream>		// cout
#include <string>		// to_string, string
#include <sstream>		// istringstream
#include <fstream>

/*External libraries*/
#include <opencv2/opencv.hpp>

/*Project includes*/
#include "car.h"		// Car struct
#include "config.h"		// Config struct

#define FAILURE		1
#define SUCCESS		0


int Config::read_config(void)
{
	/*Variables*/
	std::string line, name, value, tmp;
	Car car_dummy;
	
	/*Open config file*/
	std::ifstream conf_file("config.txt");
	if (!conf_file) {
		std::cout << "Error: could not load config file: config.txt" << std::endl;
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

		/*Background*/
		if 		(name == "get_new")			line_stream >> get_new_background;
		else if (name == "threshold")		line_stream >> diff_threshold;
		
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
		
		/*Histogram comparison*/
		if      (name == "chi2_dist_max")	line_stream >> chi2_dist_max;
		else if (name == "intersect_min")	line_stream >> intersect_min;
		
		/*Other*/
		if       (name == "min_speed")	line_stream >> min_speed;
		
		/*Cars - populate a dummy struct which is then pushed to cars_all vector*/
		// if (name == "Car") {
		// 	line_stream >> tmp;
		// 	if (tmp == "Y")	{ /*skip if not marked at "Y"*/
		// 		while (getline(conf_file, line)) {
		// 			std::istringstream line_stream(line);
		// 			line_stream >> name;
					
		// 			/*Skip invalid lines and comments*/
		// 			if (line_stream.fail() ||name[0] == '#')   continue;
					
		// 			/*Car parameters*/
		// 			if 		(name == "name")		line_stream >> car_dummy.name;
		// 			else if (name == "MAC_add")		line_stream >> car_dummy.mac_add;
		// 			else if (name == "hue")			line_stream >> car_dummy.hue;
		// 			else if (name == "delta")		line_stream >> car_dummy.delta;
		// 			else if (name == "END")	{ /*signifies end of car config parameters*/
		// 				cars_all.push_back(car_dummy);	/*push newly configured to cars_all vector*/
		// 				break;
		// 			}
		// 		}
		// 	}
		// }
	}
	
	return SUCCESS;
}



void Config::print_config(void)
{
	printf("================================\n");
	printf("Detection modes: ");
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
	printf("Background:\n");
	printf(" get_new:       %d\n", get_new_background);
	printf(" threshold:     %d\n", diff_threshold);
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

	// for (int i = 0; i < cars_all.size(); i++)
	// {
	// 	printf("================================\n");
	// 	std::cout << "Car: "<< cars_all[i].name << std::endl;
	// 	std::cout << " MAC Address: " << cars_all[i].mac_add << std::endl;
	// 	std::cout << " mid hue:     " << cars_all[i].hue << std::endl;
	// 	std::cout << " delta hue:   " << cars_all[i].delta << std::endl;
	// }
	// printf("================================\n\n");
	
	return;
}

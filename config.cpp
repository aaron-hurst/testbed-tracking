#include <iostream>		// cout
#include <string>		// to_string, string
#include <sstream>		// istringstream
#include <fstream>

#include <opencv2/opencv.hpp>

#include "car.h"		// Car struct
#include "config.h"		// Config struct

#define FAIL		1
#define SUCCESS		0


int set_config(std::vector<struct Car> &cars_all, Config &sys_conf)
{
	/*Variables*/
	std::string line, name, value, tmp;
	Car car_dummy;
	
	/*Open config file*/
	std::ifstream conf_file("config.txt");
	if (!conf_file) {
		std::cout << "Error: could not load config file: config.txt" << std::endl;
		return FAIL;
	}
	
	/*Parse config.txt*/
	while (getline(conf_file, line)) {
		std::istringstream line_stream(line);
		line_stream >> name;
		
		/*Skip invalid lines and comments*/
		if (line_stream.fail() || name[0] == '#')	continue;
		
		/*Detection mode*/
		if 		(name == "detect_mode")		line_stream >> sys_conf.detect_mode;

		/*Background*/
		if 		(name == "get_new")			line_stream >> sys_conf.get_new_background;
		else if (name == "threshold")		line_stream >> sys_conf.diff_threshold;
		
		/*Camera*/
		if      (name == "img_width")		line_stream >> sys_conf.image_w;
		else if (name == "img_height")		line_stream >> sys_conf.image_h;
		else if (name == "crop_l")			line_stream >> sys_conf.crop_l;
		else if (name == "crop_r")			line_stream >> sys_conf.crop_r;
		else if (name == "crop_t")			line_stream >> sys_conf.crop_t;
		else if (name == "crop_b")			line_stream >> sys_conf.crop_b;
		else if (name == "shutter")			line_stream >> sys_conf.shutter;
		else if (name == "auto_shutter") 	line_stream >> sys_conf.auto_shutter;
		
		/*Image*/
		else if (name == "min_sat")			line_stream >> sys_conf.min_sat;
		else if (name == "min_val")			line_stream >> sys_conf.min_val;
		else if (name == "origin_x")		line_stream >> sys_conf.origin[0];
		else if (name == "origin_y")		line_stream >> sys_conf.origin[1];
		else if (name == "scale")			line_stream >> sys_conf.scale;
		else if (name == "size_min")		line_stream >> sys_conf.car_size_min;
		else if (name == "size_max")		line_stream >> sys_conf.car_size_max;
		
		/*Histogram comparison*/
		if      (name == "chi2_dist_max")	line_stream >> sys_conf.chi2_dist_max;
		else if (name == "intersect_min")	line_stream >> sys_conf.intersect_min;
		
		/*Other*/
		if       (name == "min_speed")	line_stream >> sys_conf.min_speed;
		
		/*Cars - populate a dummy struct which is then pushed to cars_all vector*/
		if (name == "Car") {
			line_stream >> tmp;
			if (tmp == "Y")	{ /*skip if not marked at "Y"*/
				while (getline(conf_file, line)) {
					std::istringstream line_stream(line);
					line_stream >> name;
					
					/*Skip invalid lines and comments*/
					if (line_stream.fail() ||name[0] == '#')   continue;
					
					/*Car parameters*/
					if 		(name == "name")		line_stream >> car_dummy.name;
					else if (name == "MAC_add")		line_stream >> car_dummy.mac_add;
					else if (name == "hue")			line_stream >> car_dummy.hue;
					else if (name == "delta")		line_stream >> car_dummy.delta;
					else if (name == "END")	{ /*signifies end of car config parameters*/
						cars_all.push_back(car_dummy);	/*push newly configured to cars_all vector*/
						break;
					}
				}
			}
		}
	}
	
	return SUCCESS;
}



void show_config(std::vector<struct Car> &cars_all, Config &sys_conf)
{
	printf("================================\n");
	printf("Detection modes: ");
	if (sys_conf.detect_mode == 0) {
		printf(" hue\n\n");
	}
	else if (sys_conf.detect_mode == 1) {
		printf(" histogram\n");
		printf(" chi2_dist_max:  %d\n", sys_conf.chi2_dist_max);
		printf(" intersect_min:  %d\n\n", sys_conf.intersect_min);
	}
	else {
		printf(" ERROR: detection mode unknown\n\n");
	}
	printf("Background:\n");
	printf(" get_new:       %d\n", sys_conf.get_new_background);
	printf(" threshold:     %d\n", sys_conf.diff_threshold);
	printf("Camera:\n");
	printf(" img_w:         %d\n", sys_conf.image_w);
	printf(" img_h:         %d\n", sys_conf.image_h);
	printf(" crop_l:        %d\n", sys_conf.crop_l);
	printf(" crop_r:        %d\n", sys_conf.crop_r);
	printf(" crop_t:        %d\n", sys_conf.crop_t);
	printf(" crop_b:        %d\n", sys_conf.crop_b);
	printf(" shutter:       %1.2f\n", sys_conf.shutter);
	printf(" force shutter: %d\n", sys_conf.auto_shutter);
	printf("Image:\n");
	printf(" min_sat:       %d\n", sys_conf.min_sat);
	printf(" min_val:       %d\n", sys_conf.min_val);
	printf(" origin_x:      %d\n", sys_conf.origin[0]);
	printf(" origin_y:      %d\n", sys_conf.origin[1]);
	printf(" scale:         %1.3f\n", sys_conf.scale);
	printf(" size_min:      %d\n", sys_conf.car_size_min);
	printf(" size_max:      %d\n", sys_conf.car_size_max);	
	printf("Other:\n");
	printf(" min_speed:     %d\n", sys_conf.min_speed);

	for (int i = 0; i < cars_all.size(); i++)
	{
		printf("================================\n");
		std::cout << "Car: "<< cars_all[i].name << std::endl;
		std::cout << " MAC Address: " << cars_all[i].mac_add << std::endl;
		std::cout << " mid hue:     " << cars_all[i].hue << std::endl;
		std::cout << " delta hue:   " << cars_all[i].delta << std::endl;
	}
	printf("================================\n\n");
	
	return;
}




void get_cropping_mask(struct Config sys_conf, cv::Mat &crop_mask)
{
	cv::Point tl = cv::Point(sys_conf.crop_l, sys_conf.crop_t);
	cv::Point br = cv::Point(sys_conf.image_w - sys_conf.crop_r, sys_conf.image_h - sys_conf.crop_b);
	cv::rectangle(crop_mask, tl, br, 255, -1, 8 /*cv::CV_FILLED*/);
	// cv::imshow("img", crop_mask);
	// cv::waitKey(0);
    return;
}
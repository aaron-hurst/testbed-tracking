//TODO: file prologue

/*Genral includes*/
#include <iostream>		// cout
#include <vector>		// vector
#include <string>		// string
#include <unistd.h>		// sleep

/*External libraries*/
#include <opencv2/opencv.hpp>
#include </home/pi/raspicam-0.1.6/src/raspicam_cv.h>

/*Project includes*/
#include "common/config.h"
#include "common/outputs.h"
#include "common/camera.h"
#include "common/car.h"
#include "common/time.h"
#include "hue/sh_detect.h"
#include "histogram/hist_detect.h"

/*Macros*/
#define FAILURE	1
#define SUCCESS	0

#define THRESHOLD 55	//TODO: replace with config parameter
#define DILATION_ITER 1	//TODO: replace with config parameter

#define BUF_LEN 5 /*length of buffer for detection output*/


int main(int argc, char **argv)
{
	/**************************************************************************
	 * Variables
	 *************************************************************************/
	int ret;							// track function return values
	float buf[BUF_LEN];					// buffer passed to detection algorithm
	int sock;							// network socket
	Config conf;						// configuration struct
	Time sys_time;						// tracks various times
	raspicam::RaspiCam_Cv Camera;		// camera object
	std::vector<Car> cars_all;			// container for car structs
	std::vector<cv::Mat> masks_all;		// TODO: review obselescence
	cv::Mat img, img_hsv, crop_mask;	// images

	std::vector<struct Hist_data> hists_std;		// prototype histograms
	std::vector<struct Hist_data> hists_calc;		// observed histograms
	std::vector<std::vector<cv::Point>> contours;	// detection outlines

	/**************************************************************************
	 * Configuration
	 *************************************************************************/
	ret = conf.config_master(argc, argv, Camera, cars_all);
	if (ret != SUCCESS) {
		std::cout << "ERROR CRITICAL: Unable to perform configuration" << std::endl;
		return FAILURE;
	}

	//TODO: add configuration paremeters and cars info to log file (at top)
	ret = output_setup(conf.output_mode, sock, cars_all.size());
	if (ret != SUCCESS) {
		std::cout << "ERROR CRITICAL: Unable to set up outputs" << std::endl;
		return FAILURE;
	}


	/*Initialise image matrices*/
	img = cv::Mat::zeros(conf.image_h, conf.image_w, CV_8UC3);
	img_hsv = cv::Mat::zeros(conf.image_h, conf.image_w, CV_8UC3);

	/*Initialise mask for cropping table borders out of image*/
	crop_mask = cv::Mat::zeros(conf.image_h, conf.image_w, CV_8UC1);
	cv::Point tl = cv::Point(conf.crop_w, conf.crop_n);		/*north west corner*/
	cv::Point br = cv::Point(conf.image_w - conf.crop_e, conf.image_h - conf.crop_s);	/*south east corner*/
	cv::rectangle(crop_mask, tl, br, 255, -1, 8);
	
	/*Initialise vector with masks for each car*/
	for (int i = 0; i < cars_all.size(); i++) {
		cv::Mat mask = cv::Mat::zeros(conf.image_h, conf.image_w, CV_8UC1);
		masks_all.push_back(mask);
	}
	



	/*Load standard histogram data*/
	//TODO: instead of a loop in main, move the loop to hist_std_init and pass cars_all to it
	//TODO: add car type (plain or marker) to config file and histogram struct
	if (conf.detect_mode == DETECT_MODE_HIST) {
		hists_std.clear();
		for (int car = 0; car < cars_all.size(); car++) {
			ret = hist_std_init(hists_std, cars_all[car].mac_add, car);
			if (ret) {
				std::cout << "ERROR CRITICAL: parsing standard histogram file failed" << std::endl;
				return FAILURE;
			}
		}
	}
	




	/******************************************************************************
	* TRACKING (INCLUDING REPORTING)
	******************************************************************************/
	
	cv::Mat background = cv::Mat::zeros(conf.image_h, conf.image_w, CV_8UC3);
	cv::Mat diff, global_mask;

	background = cv::imread("background.png", CV_LOAD_IMAGE_COLOR);
	cv::imshow("Background image", background);
	cv::waitKey(0);
	
	sys_time.start = cv::getTickCount();
	sys_time.old = sys_time.start;
	
	for (int frame = 0; frame < conf.n_frames; frame++) {
		/*Get image*/
		Camera.grab();
		Camera.retrieve(img);
		sys_time.current = cv::getTickCount();  /*current time*/
		//cv::cvtColor(img, img_hsv, cv::COLOR_RGB2HSV);  /*convert to HSV - only needed for hue*/

		/*Get global mask*/
		//TODO: save background image, allow user to use previous background image (e.g. type Y and press enter)
		cv::absdiff(background, img, diff);
		cv::cvtColor(diff, global_mask, cv::COLOR_BGR2GRAY);
		cv::threshold(global_mask, global_mask, THRESHOLD, 255, cv::THRESH_BINARY);
		global_mask = global_mask & crop_mask;
		cv::dilate(global_mask, global_mask, cv::Mat(), cv::Point(-1, -1), DILATION_ITER); // 3x3 dilation

		cv::cvtColor(diff, img_hsv, cv::COLOR_BGR2HSV);

		if (conf.debug) {
			cv::imshow("Difference image", diff);
			cv::imshow("Difference image mask", global_mask);
			cv::waitKey(0);
		}

		/*Detect cars and calculate histograms over each contour*/
		if (conf.detect_mode == DETECT_MODE_HIST) {
			hist_detect_calc(img_hsv, global_mask, contours, hists_calc,
				conf.car_size_min, conf.car_size_max, frame, conf.debug);
		}

		/*Update all cars*/
		for (int car = 0; car < cars_all.size(); car++)
		{
			/*Detection using selected mode*/
			if (conf.detect_mode == DETECT_MODE_HUE) {
				ret = sh_detect(img_hsv, crop_mask, masks_all[car], cars_all[car], conf, buf);
			}
			else if (conf.detect_mode == DETECT_MODE_HIST) {
				//TODO: do the if statememt for car type within hist_detect
				if (cars_all[car].hue > 0) {
					ret = hist_detect(car, conf.chi2_dist_max, conf.intersect_min, contours, hists_calc, hists_std, 0, buf, conf.debug);
				}
				else {
					ret = hist_detect(car, conf.chi2_dist_max, conf.intersect_min, contours, hists_calc, hists_std, 1, buf, conf.debug);
				}
				
			}
			else {
				std::cout << "ERROR CRITICAL: invalid detection mode" << std::endl;
				return FAILURE;
			}

			/*Update state*/
			if (ret) {
				/*Car not found*/
				cars_all[car].found = false;
				cars_all[car].update_state_lost();
				ret = 0;
			}
			else {
				/*Car found successfully*/
				cars_all[car].found = true;
				cars_all[car].update_state(buf[0], buf[1], conf, sys_time);
			}

			if (conf.debug && conf.detect_mode == DETECT_MODE_HUE) {
				cv::imshow("Object masks (main)", masks_all[car]); /*display each car's mask in debug mode*/
				cv::waitKey(0);
			}
		}/*for all cars*/
		
		/*Outputs*/
		send_outputs(cars_all, conf.output_mode, sock, sys_time, frame + 1);		
		
		/*Update old data values*/
		sys_time.old = sys_time.current;
		for (int car = 0; car < cars_all.size(); car++)
		{
			cars_all[car].state_new_to_old();
		}

		/*Impose user-specified delay (to ensure downstream systems can keep up)*/
		usleep(conf.delay*1000);
	}/*for all frames*/
	
	
	/**************************************************************************
	 * EXIT
	 *************************************************************************/
	sys_time.end = cv::getTickCount();
	sys_time.total = (sys_time.end - sys_time.start) / double (cv::getTickFrequency());
    std::cout << "=========================================================================" << std::endl;
	printf("\n");
	printf("Tracking duration: %-5.3f seconds\n", sys_time.total);
	printf("Total frames:      %d\n", conf.n_frames);
	printf("Time per frame:    %-3.1f ms/frame\n", sys_time.total/conf.n_frames*1000);
	printf("FPS:               %-2.2f\n", conf.n_frames/sys_time.total);
	printf("\n");
	
	Camera.release();
	
	return SUCCESS;
}

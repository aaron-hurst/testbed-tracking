/// Tracking of single-hue objects based on colour and size


/**********************************************************************************
* GENERAL INCLUDES
**********************************************************************************/
#include <iostream>		// cout
#include <vector>		// vector
#include <string>		// string
#include <unistd.h>		// sleep

/**********************************************************************************
* CAMERA INCLUDES
**********************************************************************************/
#include </home/pi/raspicam-0.1.6/src/raspicam_cv.h>

/**********************************************************************************
* OPENCV INCLUDES
**********************************************************************************/
#include <opencv2/opencv.hpp>

/**********************************************************************************
* LOCAL INCLUDES
**********************************************************************************/
#include "config.h"
#include "outputs.h"
#include "camera.h"
#include "car.h"
#include "time.h"
#include "sh_detect.h"
#include "hist_detect.h"

/**********************************************************************************
* MACROS
**********************************************************************************/
#define FAIL		1
#define SUCCESS		0

//TODO: if histogram mode is successful, remove option or move SH detection to a different branch
#define DETECT_MODE__SH		0
#define DETECT_MODE__HIST	1


#define THRESHOLD	55
#define DILATION_ITER 	1


/**********************************************************************************
* MAIN
**********************************************************************************/
int main(int argc,char **argv)
{
	/******************************************************************************
	* SETUP
	******************************************************************************/
	
	//-----------------------------------------------------------------------------
	// Variables
	//-----------------------------------------------------------------------------
	int n_frames, output_mode, delay;	// arguments
	bool debug;							// debug parameter passed to functions
	int ret;							// function return value
	float buf[5];						// buffer passed to detection algorithm
	int sock;							// network socket
	Config sys_conf;					// store config data
	Time sys_time;						// store time variables
	raspicam::RaspiCam_Cv Camera;		// camera object
	std::vector<Car> cars_all;			// store car structs
	std::vector<cv::Mat> masks_all;		// store masks for each car
	cv::Mat img, img_hsv, crop_mask;	// images

	std::vector<struct Hist_data> hists_std;	// standard (prototype) histograms from config
	std::vector<struct Hist_data> hists_calc;	// calculated histograms in each frame
	std::vector<std::vector<cv::Point>> contours;	// contours calculated in each frame

	//-----------------------------------------------------------------------------
	// Arguments
	//-----------------------------------------------------------------------------
	n_frames 	= atoi(argv[1]);	// number of frames to process
	output_mode = atoi(argv[2]);	// output mode
	delay 		= atoi(argv[3]);	// inter-frame delay in milliseconds
	
	// Check and print output mode to the console
	output_mode = output_mode_set(output_mode);
	if (output_mode == MODE_DEBUG) {
		debug = true;
	}
	
	//-----------------------------------------------------------------------------
	// Config
	//-----------------------------------------------------------------------------
	/*Load and set system and car configuration parameters*/
	ret = set_config(cars_all, sys_conf);
	if (ret) {
		std::cout << "ERROR CRITICAL: parsing configuration file failed" << std::endl;
		return FAIL;
	}
	
	/*Load standard histogram data*/
	//TODO: instead of a loop in main, move the loop to hist_std_init and pass cars_all to it
	//TODO: add car type (plain or marker) to config file and histogram struct
	if (sys_conf.detect_mode == DETECT_MODE__HIST) {
		hists_std.clear();
		for (int car = 0; car < cars_all.size(); car++) {
			ret = hist_std_init(hists_std, cars_all[car].mac_add, car);
			if (ret) {
				std::cout << "ERROR CRITICAL: parsing standard histogram file failed" << std::endl;
				return FAIL;
			}
		}
	}

	/*Print config in debug mode*/
	if (debug) {
		show_config(cars_all, sys_conf);
	}

	/*DEBUG: set up histogram log file and print standard histograms to console*/
	if (debug && sys_conf.detect_mode == DETECT_MODE__HIST) {
		hist_log_setup(n_frames);

		printf("STANDARD HISTOGRAMS:\n");
		for (int i = 0; i < hists_std.size(); i++) {
			printf("%-17s: ", cars_all[hists_std[i].car].mac_add.c_str());
			for (int j = 0; j < N_BINS; j++) {
				printf("%1.2f ", hists_std[i].histogram[j]);
			}
			printf("\n");
		}
		printf("\n");
	}

	/*Initialise image matrices*/
	img = cv::Mat::zeros(sys_conf.image_h, sys_conf.image_w, CV_8UC3);
	img_hsv = cv::Mat::zeros(sys_conf.image_h, sys_conf.image_w, CV_8UC3);

	/*Initialise mask for cropping table borders out of image*/
	crop_mask = cv::Mat::zeros(sys_conf.image_h, sys_conf.image_w, CV_8UC1);
	get_cropping_mask(sys_conf, crop_mask);
	
	/*Initialise vector with masks for each car*/
	for (int i = 0; i < cars_all.size(); i++) {
		cv::Mat mask = cv::Mat::zeros(sys_conf.image_h, sys_conf.image_w, CV_8UC1);
		masks_all.push_back(mask);
	}
	
	//-----------------------------------------------------------------------------
	// Camera
	//-----------------------------------------------------------------------------
	/*Open camera*/
	cam_set(Camera, sys_conf);
	if (!Camera.open()) {
        std::cout << "Error opening camera" << std::endl;
        return FAIL;
	}
	sleep(2);	// wait for camera to "warm up"
	
	/*Initialise camera*/
	cam_auto_init(Camera, sys_conf, cars_all.size(), crop_mask, debug);
	
	//-----------------------------------------------------------------------------
	// Outputs
	//-----------------------------------------------------------------------------
	//TODO: add configuration paremeters and cars info to log file (at top)
	ret = output_setup(output_mode, sock, cars_all.size());
	if (ret) {
		std::cout << "ERROR CRITICAL: setting up output modes failed" << std::endl;
		ret = 0;
		return FAIL;
	}
	
	/******************************************************************************
	* TRACKING (INCLUDING REPORTING)
	******************************************************************************/
	sys_time.start = cv::getTickCount();
	sys_time.old = sys_time.start;
	
	//TODO: make function for getting background, imshow in debug
	cv::Mat background = cv::Mat::zeros(sys_conf.image_h, sys_conf.image_w, CV_8UC3);
	cv::Mat diff, global_mask;
	printf("Getting background image, please remove cars and then press enter\n");
	cv::imshow("Background image", background);
	cv::waitKey(0);
	Camera.grab();
	Camera.retrieve(background);
	printf("Background image collected, place cars back on testbed\n");
	cv::imshow("Background image", background);
	cv::waitKey(0);
	

	for (int frame = 0; frame < n_frames; frame++)
	{
		/*Get image*/
		Camera.grab();
		Camera.retrieve(img);
		sys_time.current = cv::getTickCount();  /*current time*/
		//cv::cvtColor(img, img_hsv, cv::COLOR_RGB2HSV);  /*convert to HSV*/

		/*Get global mask*/
		//TODO: save background image, allow user to use previous background image (e.g. type Y and press enter)
		cv::absdiff(background, img, diff);
		cv::cvtColor(diff, global_mask, cv::COLOR_BGR2GRAY);
		cv::threshold(global_mask, global_mask, THRESHOLD, 255, cv::THRESH_BINARY);
		global_mask = global_mask & crop_mask;
		cv::dilate(global_mask, global_mask, cv::Mat(), cv::Point(-1, -1), DILATION_ITER); // 3x3 dilation

		cv::cvtColor(diff, img_hsv, cv::COLOR_BGR2HSV);  /*convert to HSV*/

		if (debug) {
			cv::imshow("Difference image", diff);  /*display source image in debug mode*/
			cv::imshow("Difference image mask", global_mask);  /*display source image in debug mode*/
			cv::waitKey(0);
		}

		/*Detect cars and calculate histograms over each contour*/
		if (sys_conf.detect_mode == DETECT_MODE__HIST) {
			hist_detect_calc(img_hsv, global_mask, contours, hists_calc,
				sys_conf.car_size_min, sys_conf.car_size_max, frame, debug);
		}

		/*Update all cars*/
		for (int car = 0; car < cars_all.size(); car++)
		{
			/*Detection using selected mode*/
			if (sys_conf.detect_mode == DETECT_MODE__SH) {
				ret = sh_detect(img_hsv, crop_mask, masks_all[car], cars_all[car], sys_conf, buf);
			}
			else if (sys_conf.detect_mode == DETECT_MODE__HIST) {
				ret = hist_detect(car, sys_conf.chi2_dist_max, sys_conf.intersect_min,
					contours, hists_calc, hists_std, buf, debug);
			}
			else {
				std::cout << "ERROR CRITICAL: invalid detection mode" << std::endl;
				return FAIL;
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
				cars_all[car].update_state(buf[0], buf[1], sys_conf.origin, sys_conf.scale, sys_conf.min_speed, sys_time);
			}

			if (output_mode == MODE_DEBUG && sys_conf.detect_mode == DETECT_MODE__SH) {
				cv::imshow("Object masks (main)", masks_all[car]); /*display each car's mask in debug mode*/
				cv::waitKey(0);
			}
		}/*for all cars*/
		
		/*Outputs*/
		send_outputs(cars_all, output_mode, sock, sys_time, frame + 1);		
		
		/*Update old data values*/
		sys_time.old = sys_time.current;
		for (int car = 0; car < cars_all.size(); car++)
		{
			cars_all[car].state_new_to_old();
		}

		/*Impose user-specified delay (to ensure downstream systems can keep up)*/
		usleep(delay*1000);
	}/*for all frames*/
	
	
	/******************************************************************************
	* EXIT
	******************************************************************************/
	sys_time.end = cv::getTickCount();
	sys_time.total = (sys_time.end - sys_time.start) / double (cv::getTickFrequency());
    std::cout << "=========================================================================" << std::endl;
	printf("\n");
	printf("Tracking duration: %-5.3f seconds\n", sys_time.total);
	printf("Total frames:      %d\n", n_frames);
	printf("Time per frame:    %-3.1f ms/frame\n", sys_time.total/n_frames*1000);
	printf("FPS:               %-2.2f\n", n_frames/sys_time.total);
	printf("\n");
	
	Camera.release();
	
	return SUCCESS;
}

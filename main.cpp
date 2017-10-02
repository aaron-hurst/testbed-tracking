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

/**********************************************************************************
* MACROS
**********************************************************************************/
#define FAIL		1
#define SUCCESS		0

/**********************************************************************************
* NAMESPACES
**********************************************************************************/
//using namespace std;
//using namespace cv;


int main(int argc,char **argv)
{
	/******************************************************************************
	* SETUP
	******************************************************************************/
	
	//-----------------------------------------------------------------------------
	// Variables
	//-----------------------------------------------------------------------------
	int n_frames, output_mode, delay;	// arguments
	int ret;							// function return value
	float buf[5];						// buffer passed to detection algorithm
	int sock;							// network socket
	Config sys_conf;					// store config data
	Time sys_time;						// store time variables
	raspicam::RaspiCam_Cv Camera;		// camera object
	std::vector<Car> cars_all;			// store car structs
	std::vector<cv::Mat> masks_all;		// store masks for each car
	cv::Mat img, img_hsv, crop_mask;	// images
	
	//-----------------------------------------------------------------------------
	// Arguments
	//-----------------------------------------------------------------------------
	n_frames 	= atoi(argv[1]);	// number of frames to process
	output_mode = atoi(argv[2]);	// output mode
	delay 		= atoi(argv[3]);	// inter-frame delay in milliseconds
	
	// Check and print output mode to the console
	output_mode = output_mode_set(output_mode);
	
	//-----------------------------------------------------------------------------
	// Config
	//-----------------------------------------------------------------------------
	// Load and set system and car configuration parameters
	ret = set_config(cars_all, sys_conf);
	if (ret)
	{
		std::cout << "ERROR CRITICAL: parsing configuration file failed" << std::endl;
		ret = 0;
		return FAIL;
	}
	
	// Print config in debug mode
	if (output_mode == MODE_DEBUG) {
		show_config(cars_all, sys_conf);
	}
	
	// Initialise image matrices
	img = cv::Mat::zeros(sys_conf.image_h, sys_conf.image_w, CV_8UC3);
	img_hsv = cv::Mat::zeros(sys_conf.image_h, sys_conf.image_w, CV_8UC3);

	// Initialise mask for cropping table borders out of image
	crop_mask = cv::Mat::zeros(sys_conf.image_h, sys_conf.image_w, CV_8UC1);
	get_cropping_mask(sys_conf, crop_mask);
	
	// Initialise vector with masks for each car
	for (int i = 0; i < cars_all.size(); i++)
	{
		cv::Mat mask = cv::Mat::zeros(sys_conf.image_h, sys_conf.image_w, CV_8UC1);
		masks_all.push_back(mask);
	}
	
	//-----------------------------------------------------------------------------
	// Camera
	//-----------------------------------------------------------------------------
	// Open camera
	cam_set(Camera, sys_conf);
	if (!Camera.open())
	{
        std::cout << "Error opening camera" << std::endl;
        return FAIL;
	}
	sleep(2);	// wait for camera to "warm up"
	
	// Initialise camera
	if (output_mode == MODE_DEBUG)
		cam_auto_init(Camera, sys_conf, cars_all.size(), crop_mask, 1);
	else
		cam_auto_init(Camera, sys_conf, cars_all.size(), crop_mask, 0);
	
	//-----------------------------------------------------------------------------
	// Outputs
	//-----------------------------------------------------------------------------
	ret = output_setup(output_mode, sock, cars_all.size());
	if (ret)
	{
		std::cout << "ERROR CRITICAL: setting up output modes failed" << std::endl;
		ret = 0;
		return FAIL;
	}
	
	/******************************************************************************
	* TRACKING (INCLUDING REPORTING)
	******************************************************************************/
	sys_time.start = cv::getTickCount();
	sys_time.old = sys_time.start;

	for (int frame = 0; frame < n_frames; frame++)
	{
		// Get image
		Camera.grab();
		Camera.retrieve(img);
		sys_time.current = cv::getTickCount();	// current time
		cv::cvtColor(img, img_hsv, cv::COLOR_RGB2HSV);	// convert to HSV
		
		if (output_mode == MODE_DEBUG) {
			imshow("source", img);	// display source image in debug mode
		}

		// Update all cars
		for (int i = 0; i < cars_all.size(); i++)
		{
			ret = sh_detect(img_hsv, crop_mask, masks_all[i], cars_all[i], sys_conf, buf);
			if (ret) {
				// Car not found
				cars_all[i].found = false;
				cars_all[i].update_state_lost();
				ret = 0;
			}
			else {
				// Car found successfully
				cars_all[i].found = true;
				cars_all[i].update_state(buf[0], buf[1], sys_conf.origin, sys_conf.scale, sys_conf.min_speed, sys_time);
			}

			if (output_mode == MODE_DEBUG) {
				cv:imshow("mask", masks_all[i]);	// display each car's mask in debug mode
				cv::waitKey(0);
			}
		}//for all cars
		
		// Outputs
		send_outputs(cars_all, output_mode, sock, sys_time, frame + 1);		
		
		// Update old data values
		sys_time.old = sys_time.current;
		for (int i = 0; i < cars_all.size(); i++)
		{
			cars_all[i].state_new_to_old();
		}
	}//for all frames
	
	
	/******************************************************************************
	* EXIT
	******************************************************************************/
	sys_time.end = cv::getTickCount();
	sys_time.total = (sys_time.end - sys_time.start) / double (cv::getTickFrequency());
	printf("\n");
	std::cout << "Total time:        " << sys_time.total << " seconds" << std::endl;
	std::cout << "Total frames:      " << n_frames << std::endl;
	std::cout << "Processing time:   " << sys_time.total/n_frames*1000 << " ms/frame" << std::endl;
	std::cout << "Frames per second: " << n_frames/sys_time.total << std::endl;
	
	Camera.release();
	
	return SUCCESS;
}

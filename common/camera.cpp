/*General includes*/
#include <unistd.h>		// usleep
#include <iostream>		// cout

/*External libraries*/
#include </home/pi/raspicam-0.1.6/src/raspicam_cv.h>
#include <opencv2/opencv.hpp>

/*Project includes*/
#include "camera.h"
#include "config.h"

/*Macos*/
#define FAILURE	1
#define SUCCESS	0

#define IMG_BRIGHTNESS		50
#define IMG_CONTRAST		50
#define IMG_SATURATION		50
#define IMG_GAIN			50

//TODO: check shutter values when reviewing auto-shutter function
#define SHUTTER_MIN         1.6
#define SHUTTER_MAX         4
#define SHUTTER_DEFAULT     3.03
#define SHUTTER_INC         0.1
#define SHUTTER_EXTRA       0.4

void cam_set(raspicam::RaspiCam_Cv& Camera, struct Config conf)
{
	Camera.set(CV_CAP_PROP_FRAME_WIDTH, conf.image_w);
    Camera.set(CV_CAP_PROP_FRAME_HEIGHT, conf.image_h);
    Camera.set(CV_CAP_PROP_BRIGHTNESS, IMG_BRIGHTNESS);
    Camera.set(CV_CAP_PROP_CONTRAST, IMG_CONTRAST);
    Camera.set(CV_CAP_PROP_SATURATION, IMG_SATURATION);
    Camera.set(CV_CAP_PROP_GAIN, IMG_GAIN);
	Camera.set(CV_CAP_PROP_EXPOSURE, conf.shutter);
	return;
}

//TODO: check obselescence/build another function for the same result using image filtering (convertTo)
int cam_auto_init(raspicam::RaspiCam_Cv &Camera, struct Config &sys_conf, int n_cars, cv::Mat crop_mask, bool debug)
{
	// Variables
    int px_matched, px_matched_old = 0;     // number of matched pixels
	
	// printf("Debug state:  %d\n", debug);
	// printf("Maximum area: %d\n", n_cars*sys_conf.car_size_max);
	// printf("Minimum area: %d\n", n_cars*sys_conf.car_size_min);
	
	// Image
    cv::Mat img;
	Camera.grab();
    Camera.retrieve(img);
    
	// Masks
	cv::Mat img_hsv = cv::Mat::zeros(img.rows, img.cols, CV_8UC3);
    cv::Mat mask = cv::Mat::zeros(img.rows, img.cols, CV_8UC1);
	
    // Increment shutter speed and check number of matched pixels until termination conditions met
	printf("Starting auto shutter speed camera initialisation...\n");
	sys_conf.shutter = SHUTTER_MIN;
	while (px_matched > n_cars*sys_conf.car_size_max || px_matched < n_cars*sys_conf.car_size_min)
	{
		// Increment shutter speed and update px_matched_old
		sys_conf.shutter += SHUTTER_INC;
		px_matched_old = px_matched;
		
		// Set up camera and get image
		cam_set(Camera, sys_conf);
		usleep(70000);
		Camera.grab();
		Camera.retrieve(img);
		
		// Calculate mask of significant hues
		cv::cvtColor(img, img_hsv, cv::COLOR_RGB2HSV);
		cv::inRange(img_hsv, cv::Scalar(0, sys_conf.min_sat, sys_conf.min_val), cv::Scalar(180, 255, 255), mask);
		mask = mask & crop_mask;
		
		// cv::imshow("mask", mask);
		// cv::waitKey(0);

		// Calculate matched area
		px_matched = cv::countNonZero(mask == 255);
		
        // Terminate if current matched value is less than minimum car size AND was previously greater
        // than maximum car size (both multiplied by the number of cars)
		// i.e. if it jumped from too many pixels to too few in a single increment
		if (px_matched < n_cars*sys_conf.car_size_min && px_matched_old > n_cars*sys_conf.car_size_max) {
			printf("Special termination: jumped over accepted range\n");
			break;
		}
		
		// Printf output if in debug mode
		if (debug) {
			printf("Shutter:  %1.2f    matched pixels:  %5d\n", sys_conf.shutter, px_matched);
		}

		// Terminate when maximum shutter period is reached
		if (sys_conf.shutter > SHUTTER_MAX)	{
			printf("ERROR: Camera Auto Initialisation. Setting default shutter period of %1.2f ms.\n", 3.3*SHUTTER_DEFAULT);
			sys_conf.shutter = SHUTTER_DEFAULT;
			return FAILURE;
		}
	}
	
	// Increment shutter period slightly beyond measured threshold and impose a lower limit
	sys_conf.shutter += SHUTTER_EXTRA;
	cam_set(Camera, sys_conf);
	
    // Print output
	printf("\n==================================\n");
	printf("Auto shutter:     %1.2f (%1.2f ms)\n", sys_conf.shutter, 3.3*sys_conf.shutter);
	printf("Matched area:   %6d\n", px_matched);
	printf("==================================\n");
	
	// cv::imshow("img", img);
	// cv::waitKey(0);
	
	return SUCCESS;
}

void get_background(int height, int width, raspicam::RaspiCam_Cv Camera)
{
	cv::Mat background = cv::Mat::zeros(height, width, CV_8UC3);
	std::cout<<"Getting new background image."<<std::endl;
	std::cout<<"Please remove all cars from testbed and then press enter."<<std::endl;
	getchar();	/*wait for user to press enter*/
	std::cout<<"Capturing background image..."<<std::endl;
	Camera.grab();
	Camera.retrieve(background);
	std::cout<<"Background image collected, return cars on testbed"<<std::endl;
	cv::imwrite("background.png", background);
}

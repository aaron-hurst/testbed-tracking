// OpenCV includes
#include <opencv2/opencv.hpp>

// Local includes
#include "car.h"
#include "config.h"
#include "sh_detect.h"

#define FAIL		1
#define SUCCESS		0

#define DILATION_ITER 	1



void set_buf_zero(float buf[])
{
	for (int i = 0; i < 5; i++) {
		buf[i] = 0;
	}
}


int sh_detect(cv::Mat img_hsv, cv::Mat crop_mask, cv::Mat &mask, struct Car car, struct Config sys_conf, float buf[])
{
	// Variables
	int contour_area, contour_area_max = 0, count = 0, idx = 0;
	cv::Moments mu;

	// Hue-matching mask
	cv::inRange(img_hsv, cv::Scalar(car.hue - car.delta, sys_conf.min_sat, sys_conf.min_val), cv::Scalar(car.hue + car.delta, 255, 255), mask);
	mask = mask & crop_mask;
	cv::dilate(mask, mask, cv::Mat(), cv::Point(-1, -1), DILATION_ITER);	// 3x3 dilation

	cv::Mat mask_use = cv::Mat::zeros(mask.rows, mask.cols, CV_8UC1);
	mask.copyTo(mask_use);	// copy mask since will be modified by findContours

	// Find contours
	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(mask_use, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);	// note: mask_use is modified here

	// Return FAIL if no contours exist
	if (contours.size() < 1) {
		set_buf_zero(buf);
		return FAIL;
	}
	
	// Find correct contour associated with the car
	for (int i = 0; i < contours.size(); i++)
	{
		// Check if contour area is within tolerances for a car
		contour_area = cv::contourArea(contours[i]);
		if (contour_area > sys_conf.car_size_min && contour_area < sys_conf.car_size_max)
		{
			if (contour_area > contour_area_max)
			{
				contour_area_max = contour_area;
				idx = i;	// index of largest contour within allowed size bounds
				count++;	// number of contours within allowed size bounds
			}
		}
	}

	// Report state
	if (count == 0) {
		// No cars found
		set_buf_zero(buf);
		return FAIL;
	}
	else if (count > 1) {
		// Multiple contours of allowable size
		std::cout << "WARNING: multiple contours found for car: " << car.name << std::endl;
	}

	// Save outputs in buffer
	mu = cv::moments(contours[idx], true);	// calculate moment of car's contour		
	buf[0] = mu.m10 / mu.m00;				// x-position of car in pixels
	buf[1] = mu.m01 / mu.m00;				// y-position of car in pixels
	buf[2] = contour_area_max;				// area of car's contour
	
	return SUCCESS;
}

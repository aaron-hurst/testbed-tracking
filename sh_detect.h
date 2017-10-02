#ifndef CARSCV_SH_DETECT_H
#define CARSCV_SH_DETECT_H



void set_buf_zero(float buf[5]);


/**********************************************************************************
* Single Hue Detection
* 
* ... does not edit the car... returns values via a buffer.. set buffer to all zeros at start and no not change if not found
* 
* ...
*
* return: 0 on success (found), 1 on failure (not found)
**********************************************************************************/
int sh_detect(cv::Mat img_hsv, cv::Mat crop_mask, cv::Mat &mask, struct Car car, struct Config sys_conf, float buf[]);



#endif
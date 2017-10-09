#ifndef CARSCV_SH_DETECT_H
#define CARSCV_SH_DETECT_H


/**********************************************************************************
 * @brief Flushes buffer
 * 
 * Sets all values in finite sized buffer to zero
 * 
 * @param buf
 */
void set_buf_zero(float buf[5]);

//TODO: description
/**********************************************************************************
 * @brief Single hue detection
 * 
 * returns values via a buffer, all zeros if not found
 * 
 * @param img_hsv
 * @param crop_mask
 * @param mask
 * @param car
 * @param sys_conf
 * @param buf
 * 
 * return: 0 on success (found), 1 on failure (not found)
 */
int sh_detect(cv::Mat img_hsv, cv::Mat crop_mask, cv::Mat &mask, struct Car car, struct Config sys_conf, float buf[]);



#endif
#ifndef CARSCV_CAMERA_H
#define CARSCV_CAMERA_H

#include "config.h"

/**********************************************************************************
 * Camera settings
 * 
 * Sets standard camera settings: frame width (pixels), frame height (pixels),
 * brightness (??), contrast, saturation, gain (ISO??) and shutter duration.
 * White balance is left on auto and colour capture is used.
 * 
 * Camera    object defining the camera
 * shutter   shutter duration (in units of 3.3ms, i.e. 1 = 3.3ms, 2 = 6.6ms)
 *
 * return: void
 */
void cam_set(raspicam::RaspiCam_Cv &Camera, struct Config sys_conf);


/**********************************************************************************
 * Camera auto initialisation
 * 
 * Sets camera settings (using cam_set) and automatically selects shutter speed
 * such that the background (projection or table) is fully washed out and only the
 * cars show up as having significant saturation and intensity values.
 * The value of sys_conf.shutter is updated by this function.
 * 
 * Camera    object defining the camera
 * sys_conf  struct containing global configuration parameters
 * n_cars    number of cars to expect in the field of view
 * debug     specifies whether to run in normal (0) or debug mode (1)
 *
 * return: 0 on success, 1 on failure
 */
int cam_auto_init(raspicam::RaspiCam_Cv &Camera, struct Config &sys_conf,int n_cars, cv::Mat crop_mask, bool debug);


#endif
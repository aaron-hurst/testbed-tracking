#ifndef CARSCV_CAMERA_H
#define CARSCV_CAMERA_H

/*Forward declarations*/
struct Config;
namespace raspicam {
    class RaspiCam_Cv;
}

//=====================================
/*! @brief Apply camera settings
 *
 * Sets camera image width and height (pixels), brightness, contrast,
 * saturation, gain and shutter duration. White balance is left on auto and
 * colour capture is used.
 * 
 * @param Camera Raspicam camera object
 * @param conf Struct containing general configuration parameters
 * 
 * @return void
 */
void cam_set(raspicam::RaspiCam_Cv&, struct Config);

//TODO: proper function documentation
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

//=====================================
/*! @brief Get a new background image
 *
 * Asks the user to remove all cars from the testbed before taking an image
 * which can be used for background subtraction. Image is saved as
 * background.png in the histogram/ folder.
 * 
 * @param height Image height in pixels
 * @param width Image width in pixels
 * 
 * @return void
 */
void get_background(int, int, raspicam::RaspiCam_Cv);

#endif /*CARSCV_CAMERA_H*/

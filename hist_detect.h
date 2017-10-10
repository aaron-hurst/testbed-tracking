#ifndef CARSCV_HIST_DETECT_H
#define CARSCV_HIST_DETECT_H

#define MAX_HUE		179
#define BIN_WIDTH   10
#define N_BINS		18

// Forward declarations
struct Config;


/**********************************************************************************
 * @brief Histogram data structure
 */
struct Hist_data {
    float histogram [N_BINS];   // calculated or prototype histogram
    int contour_idx;            // index of contours over which the histogram was calculated
    int car;                    // index of car associated with histogram (either from config or based on detection)
    int area;                   // area of mask used to calculate this (-1 for prototype histograms?)
};

/**********************************************************************************
 * @brief Initialise prototype histograms for each car in use
 * 
 * 
 */
int hist_std_init(std::vector<struct Hist_data> &hist_std, std::string mac_add, int car_idx);

/**********************************************************************************
 * @brief Calculate histograms over contours detected in a source HSV image
 * 
 * 
 */
int hist_detect_calc(cv::Mat img_hsv, cv::Mat crop_mask,
    std::vector<std::vector<cv::Point>> &contours,
    std::vector<struct Hist_data> &hist_calc,
    struct Config sys_conf, int frame, bool debug);

/**********************************************************************************
 * @brief Compares two histograms provided as arrays of fixed length
 * 
 * 
 */
float hist_compare(struct Hist_data hist1, struct Hist_data hist2, int len);


/**********************************************************************************
 * @brief Applies histogram comparison to detect and locate known vehicles
 * 
 * 
 */
int hist_detect(int car_idx, int min_quality,
	std::vector<std::vector<cv::Point>> contours,
	std::vector<struct Hist_data> hist_calc,
	std::vector<struct Hist_data> hist_std,
	float buf[]);





#endif /*CARSCV_HIST_DETECT_H*/
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
    int car_type;               // 0: plain, 1: markers
};


int hist_log_setup(int total_frames);


/**********************************************************************************
 * @brief Initialise prototype histograms for each car in use
 * 
 * 
 */
int hist_std_init(std::vector<struct Hist_data> &hist_std, std::string mac_add, int car_idx);

/**********************************************************************************
 * @brief Calculate histograms over contours detected in a source HSV image
 * 
 * adds contour index and contour area to hist struct
 */
int hist_detect_calc(cv::Mat img_hsv, cv::Mat global_mask,
	std::vector<std::vector<cv::Point>> &contours,
	std::vector<struct Hist_data> &hists_calc,
	int size_min, int size_max, int frame, bool debug);

/**********************************************************************************
 * @brief TODO: Compares two histograms provided as arrays of fixed length
 * 
 * Order of histograms matters
 * 
 * return: 
 */
float chi2_dist(float hist_std[], float hist_test[], int len, float min_hist_val);

/**********************************************************************************
 * @brief TODO: Compares two histograms provided as arrays of fixed length
 * 
 * Order of histograms matters
 * 
 * return: 
 */
float intersection(float hist_std[], float hist_test[], int len);

/**********************************************************************************
 * @brief Applies histogram comparison to detect and locate known vehicles
 * 
 * adds car index to histogram struct so that it is not re-compared (time saver and avoids confused detection)
 * 
 * return: 0 on success (found), 1 on failure (not found)
 */
int hist_detect(int car_idx, float chi2_threshold, float int_threshold,
	std::vector<std::vector<cv::Point>> contours,
	std::vector<struct Hist_data> &hists_calc,
    std::vector<struct Hist_data> hists_std,
    int type,
    float buf[],
    bool debug);





#endif /*CARSCV_HIST_DETECT_H*/
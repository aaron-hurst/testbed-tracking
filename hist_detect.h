#ifndef CARSCV_HIST_DETECT_H
#define CARSCV_HIST_DETECT_H

#define MAX_HUE		179
#define BIN_WIDTH   10
#define N_BINS		18

// Forward declarations
struct Config;


struct Hist_data {
    float histogram [N_BINS];   // calculated or prototype histogram
    int contour_idx;            // index of contours over which the histogram was calculated
    int car;                    // index of car associated with histogram (either from config or based on detection)
    int area;                   // area of mask used to calculate this (-1 for prototype histograms?)
};


void hist_detect_flush(/*vector of calculated histogram arrays*/);


void hist_detect_init(void);



int hist_detect_calc(cv::Mat img_hsv, cv::Mat crop_mask,
    std::vector<std::vector<cv::Point>> contours,
    std::vector<struct Hist_data> hist_calc,
    struct Config sys_conf, bool debug);


int hist_detect_compare();


/**********************************************************************************
* Single Hue Detection
* 
* ... does not edit the car... returns values via a buffer.. set buffer to all zeros at start and no not change if not found
* 
* ...
*
* return: 0 on success (found), 1 on failure (not found)
**********************************************************************************/



#endif
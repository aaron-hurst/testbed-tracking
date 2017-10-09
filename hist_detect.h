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



/**********************************************************************************
 * Initialise prototype histograms for each car in use
 * 
 * 
 */
void hist_std_init(/*vector for prototype histograms*/);


/**********************************************************************************
 * Calculate histograms over contours detected in a source HSV image
 * 
 * 
 */
int hist_detect_calc(cv::Mat img_hsv, cv::Mat crop_mask,
    std::vector<std::vector<cv::Point>> &contours,
    std::vector<struct Hist_data> &hist_calc,
    struct Config sys_conf, bool debug);



float hist_compare(struct Hist_data hist1, struct Hist_data hist2);



int hist_detect(
    /* car
     * contours vector for finding position
     * calculated histogram vector
     * prototype histogram vector
     * config for required match quality
     * */
);





#endif /*CARSCV_HIST_DETECT_H*/
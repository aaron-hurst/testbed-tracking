#include <vector>		// vector

// OpenCV includes
#include <opencv2/opencv.hpp>

// Local includes
#include "hist_detect.h"
#include "config.h"

#define FAIL		1
#define SUCCESS		0

#define DILATION_ITER 	1





void hist_std_init(void)
{

}


//TODO: check for erros, return FAILURE
int hist_detect_calc(cv::Mat img_hsv, cv::Mat crop_mask,
	std::vector<std::vector<cv::Point>> &contours,
	std::vector<struct Hist_data> &hist_calc,
	struct Config sys_conf, bool debug)
{
	/*Variables*/
	cv::Mat mask = cv::Mat::zeros(img_hsv.rows, img_hsv.cols, CV_8UC1);
	cv::Mat mask_ctr = cv::Mat::zeros(img_hsv.rows, img_hsv.cols, CV_8UC1);
	int hist_bins = N_BINS;
	float hue_range[] = {0, MAX_HUE};
	const float* range[] = {hue_range};
	int channel[] = {0};
	cv::MatND hist_tmp;
	struct Hist_data hist_data_tmp;
	double max;
	int contour_area;

	/*Clear data vectors*/
	contours.clear();
	hist_calc.clear();

	/*GLobal hue-matching mask*/
	cv::inRange(img_hsv, cv::Scalar(0, sys_conf.min_sat, sys_conf.min_val), cv::Scalar(180, 255, 255), mask);
	mask = mask & crop_mask;
	cv::dilate(mask, mask, cv::Mat(), cv::Point(-1, -1), DILATION_ITER);	// 3x3 dilation

	/*Find contours*/
	cv::findContours(mask, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

	/*Calculate histogram for each contour of sufficient size*/
	for (int i = 0; i < contours.size(); i++) {
		contour_area = cv::contourArea(contours[i]);
		if (contour_area > sys_conf.car_size_min && contour_area < sys_conf.car_size_max) {
			/*Generate contour mask*/
			mask_ctr = cv::Mat::zeros(img_hsv.rows, img_hsv.cols, CV_8UC1);
			if (debug) {
				cv::drawContours(mask_ctr, contours, i, cv::Scalar(255, 255, 255), CV_FILLED, 8);
				imshow("mask_ctr", mask_ctr);
				cv::waitKey(0);
			}
			
			/*Calculate histogram on contour mask*/
			cv::calcHist(&img_hsv, 1, channel, mask_ctr, hist_tmp, 1, &hist_bins, range, true, false);

			/*Get maximum histogram value*/
			cv::minMaxLoc(hist_tmp, NULL, &max, NULL, NULL);

			/*Normalise and copy histogram data to hist_data struct*/
			for (int i = 0; i < hist_bins; i++) {
				hist_data_tmp.histogram[i] = hist_tmp.at<float>(i) / max;
			}
			hist_data_tmp.area = contour_area;
			hist_data_tmp.contour_idx = i;

			/*Push hist_data_tmp to hist_calc vector*/
			hist_calc.push_back(hist_data_tmp);
		}
	}
	
	/*Print output in debug mode*/
	if (debug) {
		printf("Number of vehicles found: %d\n", hist_calc.size());
		printf("Histograms:\n");
		printf("          ");
		for (int i = 0; i < N_BINS; i++) {
			printf("%3d  ", 5+i*BIN_WIDTH);
		}
		printf("\n");
		for (int i = 0; i < hist_calc.size(); i++) {
			printf("Car %2d:  ", i+1);
			for (int j = 0; j < N_BINS; j++) {
				printf("%1.2f ", hist_calc[i].histogram[j]);
			}
			printf("\n");
		}
	}

	return SUCCESS;
}


float hist_compare(struct Hist_data hist1, struct Hist_data hist2)
{

}

int hist_detect(
    /* car number (index in cars_all)
     * contours vector for finding position
     * calculated histogram vector
     * prototype histogram vector
     * minimum quality
	 * buffer
	 * 
	 * do not pass any by reference - don't need to change any of them
     * */
)
{
	float match_quality = 0, best_match = 0;
	int best_idx = -1;

	/*Compare calculated histograms with standard histogram for given car*/
	for (int i = 0; i < hist_calculated.size(); i++) {
		if (1/*calculated histogram not already associated with a car*/) {
			match_quality = hist_compare(hist_calculated[i], hist_std[car]);
			if (match_quality > best_match) {
				best_match = match_quality;
				best_idx = i;
			}
		}
	}

	if (best_match > min_quality) {
		/*Calculate car position*/

		/*Store position and diagnostic data in buffer*/

	}
	else {
		/*No suitable matching histogram found*/
		return FAIL;
	}

	return SUCCESS;
}
#include <vector>		// vector
#include <iostream>
#include <sstream>
#include <string>
#include <math.h>		// pow

// OpenCV includes
#include <opencv2/opencv.hpp>

// Local includes
#include "hist_detect.h"
#include "../common/config.h"

#define FAILURE		1
#define SUCCESS		0

#define MIN_HIST_VAL 0.01


int hist_log_setup(int total_frames)
{
	/*Open file*/
	FILE * hist_log;
	hist_log = fopen("hist_log.csv","w");
	if (hist_log == NULL) {
		return FAILURE;
	}

	/*Write file lines*/
	fprintf(hist_log,"Log of calculated histograms,\n");
	fprintf(hist_log,"%d,",total_frames);
	for (int i = 0; i < N_BINS; i++) {
		fprintf(hist_log, "%d,", 5+i*BIN_WIDTH);
	}
	fprintf(hist_log,"\n");

	fclose(hist_log);

	return SUCCESS;
}

int hist_std_init(std::vector<struct Hist_data> &hist_std, std::string mac_addr, int car_idx)
{
	/*Variables*/
	int idx = 0;
	bool found = false;
	std::string line, token;
	struct Hist_data hist_tmp;
	
	/*Open histogram config file*/
	std::ifstream hist_conf("histogram/hist_config.txt");
	if (!hist_conf) {
		std::cout << "Error: could not load histogram config file: hist_config.txt" << std::endl;
		return FAILURE;
	}

	/*Parse each line*/
	while(std::getline(hist_conf, line)) {
		std::istringstream ss(line);
		/*get MAC address (first item on line) and compare to provided MAC address*/
		std::getline(ss, token, ',');
		if (mac_addr.compare(token) == 0) {
			/*Matching entry found, store a new histogram*/
			idx = 0;
			while(std::getline(ss, token, ',')) {
				hist_tmp.histogram[idx] = std::stof(token);
				idx++;
			}
			hist_tmp.car = car_idx;
			hist_tmp.contour_idx = -1;
			hist_tmp.area = -1;
			hist_std.push_back(hist_tmp);
			found = true;
		}
	}

	/*Return failure if no matching histogram was found for the given MAC address*/
	if (!found) {
		return FAILURE;
	}
	
	return SUCCESS;
}

//TODO: error checking
int hist_detect_calc(cv::Mat img_hsv, cv::Mat global_mask,
	std::vector<std::vector<cv::Point>> &contours,
	std::vector<struct Hist_data> &hists_calc,
	int size_min, int size_max, int frame, bool debug)
{
	/*Variables*/
	cv::Mat mask_ctr = cv::Mat::zeros(img_hsv.rows, img_hsv.cols, CV_8UC1);
	int hist_bins = N_BINS;
	float hue_range[] = {0, MAX_HUE};
	const float* range[] = {hue_range};
	int channel[] = {0};
	cv::MatND hist_tmp_mat;
	struct Hist_data hist_tmp;
	double max;
	int contour_area;

	/*Clear data vectors*/
	contours.clear();
	hists_calc.clear();

	/*Find contours*/
	cv::findContours(global_mask, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

	/*Calculate histogram for each contour of sufficient size*/
	for (int i = 0; i < contours.size(); i++) {
		contour_area = cv::contourArea(contours[i]);
		if (contour_area > size_min && contour_area < size_max) {
			/*Generate contour mask*/
			mask_ctr = cv::Mat::zeros(img_hsv.rows, img_hsv.cols, CV_8UC1);
			cv::drawContours(mask_ctr, contours, i, cv::Scalar(255, 255, 255), CV_FILLED, 8);
			// if (debug) {
			// 	imshow("Contour mask (hist_detect_calc)", mask_ctr);
			// 	cv::waitKey(0);
			// }
			
			/*Calculate histogram on contour mask*/
			cv::calcHist(&img_hsv, 1, channel, mask_ctr, hist_tmp_mat, 1, &hist_bins, range, true, false);

			/*Get maximum histogram value*/
			cv::minMaxLoc(hist_tmp_mat, NULL, &max, NULL, NULL);

			/*Normalise and copy histogram data to temporary hist struct*/
			for (int j = 0; j < hist_bins; j++) {
				hist_tmp.histogram[j] = hist_tmp_mat.at<float>(j) / max;
			}
			hist_tmp.area = contour_area;
			hist_tmp.contour_idx = i;
			hist_tmp.car = -1;

			/*Push hist_tmp to hists_calc vector*/
			hists_calc.push_back(hist_tmp);
		}
	}

	/*Print histograms in debug mode*/
	if (debug) {
		printf("Number of cars found: %d\n", hists_calc.size());
		printf("             ");
		for (int i = 0; i < N_BINS; i++) {
			printf("%3d  ", 5+i*BIN_WIDTH);
		}
		printf("\n");
		for (int i = 0; i < hists_calc.size(); i++) {
			printf("Object %2d:  ", i+1);
			for (int j = 0; j < N_BINS; j++) {
				printf("%1.2f ", hists_calc[i].histogram[j]);
			} /*for each elemeent in histogram*/
			printf("\n");
		} /*for each histogram*/
	}

	/*Write histogram to log file*/
	if (debug) {
		FILE * hist_log;
		hist_log = fopen("hist_log.csv","a");
		for (int i = 0; i < hists_calc.size(); i++) {
			fprintf(hist_log, "%4d,", frame);
			for (int j = 0; j < N_BINS; j++) {
				fprintf(hist_log, "%1.2f,", hists_calc[i].histogram[j]);
			}
			fprintf(hist_log, "\n");
		}
		fclose(hist_log);
	}

	return SUCCESS;
}


float chi2_dist(float hist_std[], float hist_test[], int len, float min_hist_val)
{
	float diff = 0;
	for (int i = 0; i < len; i++) {
		if (hist_std[i] < min_hist_val) { /*standard histogram value too small for reasonable comparison*/
			diff += pow(hist_test[i] - min_hist_val, 2) / min_hist_val;
		}
		else {
			diff += pow(hist_test[i] - hist_std[i], 2) / hist_std[i];
		}
	}

	return diff;
}


float intersection(float hist_std[], float hist_test[], int len)
{
	float intersection = 0, sum = 0;
	for (int i = 0; i < len; i++) {
		if (hist_test[i] < hist_std[i]) {
			intersection += hist_test[i];
		}
		else {
			intersection += hist_std[i];
		}
		sum += hist_std[i];
	}

	return intersection/sum;
}


int hist_detect(int car_idx, float chi2_threshold, float int_threshold,
	std::vector<std::vector<cv::Point>> contours,
	std::vector<struct Hist_data> &hists_calc,
	std::vector<struct Hist_data> hists_std,
	int type,
	float buf[],
	bool debug)
{
	/*Variables*/
	float hist_diff = 9999, best_diff = 9999;
	float hist_int = 0, best_int = 0;
	int best_calc_idx = -1, best_ctr_idx = -1, best_std;
	cv::Moments mu;

	/*Compare calculated histograms with standard histogram(s) for given car*/
	for (int i = 0; i < hists_std.size(); i++) {
		if (hists_std[i].car == car_idx) { /*standard histogram corresponds to car of interest*/
			if (type == 1) { /*car with marker*/
				for (int j = 0; j < hists_calc.size(); j++) {
					if (hists_calc[j].car < 0) { /*calculated histogram not already associated with a car*/
						/*Calculate intersection between histograms*/
						hist_int = intersection(hists_std[i].histogram, hists_calc[j].histogram, N_BINS);
						if(debug) {
							printf("Car %2d:  %6.2f\n", car_idx+1, hist_int);
						}
						/*Check if current intersection is best (largest) yet*/
						if (hist_int > best_int) {
							best_int = hist_int;
							best_calc_idx = j; /*best matching calculated histogram*/
							best_std = i; /*best matching standard histogram*/
						}
					}
				} /*for calculated histogram not already matched to a car*/
			}
			else { /*plain coloured car*/
				for (int j = 0; j < hists_calc.size(); j++) {
					if (hists_calc[j].car < 0) { /*calculated histogram not already associated with a car*/
						/*Calculate modified chi-squared difference between histograms*/
						hist_diff = chi2_dist(hists_std[i].histogram, hists_calc[j].histogram, N_BINS, MIN_HIST_VAL);
						if(debug) {
							printf("Car %2d:  %6.2f\n", car_idx+1, hist_diff);
						}
						/*Check if current difference is best (smallest) yet*/
						if (hist_diff < best_diff) {
							best_diff = hist_diff;
							best_calc_idx = j; /*best matching calculated histogram*/
							best_std = i; /*best matching standard histogram*/
						}
					}
				} /*for calculated histogram not already matched to a car*/
			}
		}
	} /*for each standard histogram associated with car of interest*/

	/*If best difference is small/best intersection is large, assign to car and store data in buffer*/
	if (best_diff < chi2_threshold || best_int > int_threshold) {
		best_ctr_idx = hists_calc[best_calc_idx].contour_idx;
		mu = cv::moments(contours[best_ctr_idx], true);	/*calculate moment of car's contour*/
		buf[0] = mu.m10 / mu.m00;						/*x-position of car in pixels*/
		buf[1] = mu.m01 / mu.m00;						/*y-position of car in pixels*/
		buf[2] = hists_calc[best_calc_idx].area;		/*area of car's contour*/
		buf[3] = best_calc_idx;							/*index of matched histogram*/

		/*Associate car with calculated histogram (so histogram is not re-compared)*/
		hists_calc[best_calc_idx].car = car_idx;
	}
	/*Else, no suitable matching histogram found: set buffer to zero*/
	else {
		for (int i = 0; i < 5; i++) {
			buf[i] = 0;
		}
		return FAILURE;
	}

	return SUCCESS;
}

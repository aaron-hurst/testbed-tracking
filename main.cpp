/// Tracking of single-hue objects based on colour and size



/**********************************************************************************
* LOCAL INCLUDES
**********************************************************************************/
#include "config.h"
#include "car.h"

/**********************************************************************************
* NAMESPACES
**********************************************************************************/
//using namespace std;
//using namespace cv;

int main(int argc,char **argv)
{
	/******************************************************************************
	* SETUP
	******************************************************************************/
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Variables
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	int n_frames, output_mode, delay;	// arguments
	int ret;							// function return value
	vector<Car> cars_all;
	Config sys_config;
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Arguments
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	n_frames 	= atoi(argv[1]);	// number of frames to process
	output_mode = atoi(argv[2]);	// output mode
	delay 		= atoi(argv[3]);	// delay in milliseconds
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Config
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	ret = set_config(cars_all, sys_config);
	if (ret)
	{
		cout << "Error parsing configuration file" << endl;
		return 1;
	}
	
	// Print config in debug mode
	if (output_mode = DEBUG)
	{
		show_config(sys_conf);
	}
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Camera
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Outputs
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	
	/******************************************************************************
	* TRACKING (INCLUDING REPORTING)
	******************************************************************************/
	
	
	
	
	/******************************************************************************
	* EXIT
	******************************************************************************/
	
	double time_total = double ( cv::getTickCount() - time_start ) / double ( cv::getTickFrequency() ); // total time in seconds
	cout << endl;
	cout << "Total time: " << time_total <<" seconds"<<endl;
	cout << "Total frames: " << n_frames <<endl;
    cout << "Average processing speed: " << time_total/n_frames*1000 << " ms/frame (" << n_frames/time_total<< " fps)" <<endl;
	
	Camera.release();
	
	return 0;
}

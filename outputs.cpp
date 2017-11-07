#include <iostream>		// cout
#include <string>		// string, to_string

#include <sys/socket.h>	// socket comms
#include <netinet/in.h>	// struct sockaddr_in
#include <sys/time.h>	// system time for JSON
#include <arpa/inet.h>	// inet_addr

#include <opencv2/opencv.hpp>	// cv::getTickFrequency()

#include "outputs.h"
#include "car.h"
#include "time.h"

#define FAILURE		1
#define SUCCESS		0


int output_mode_set(int output_mode)
{
	if (output_mode == MODE_LIVE) {
		std::cout<<"Output mode: LIVE - JSON only"<<std::endl;
	} else if (output_mode == MODE_LIVE_CONS) {
		std::cout<<"Output mode: LIVE - JSON + console"<<std::endl;
	} else if (output_mode == MODE_LIVE_LOG) {
		std::cout<<"Output mode: LIVE - JSON + console + csv"<<std::endl;
	} else if (output_mode == MODE_TEST) {
		std::cout<<"Output mode: TEST - console + csv"<<std::endl;
	} else if (output_mode == MODE_DEBUG) {
		std::cout<<"Output mode: DEBUG"<<std::endl;
	} else {
		std::cout<<"WARNING: Invalid output mode specified. Using default: Mode 2: LIVE - JSON + console"<<std::endl;
		output_mode = MODE_LIVE_CONS;
	}
	return output_mode;
}


int output_setup(int &output_mode, int &sock, int n_cars)
{
	// Set up network socket connection to server (cars controller program)
	if (output_mode == MODE_LIVE || output_mode == MODE_LIVE_CONS || output_mode == MODE_LIVE_LOG)
	{
        // Create socket
        sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == -1)
		{
			std::cout<< "ERROR: Could not create socket" <<std::endl;
			return FAILURE;
		}
		std::cout << "Socket created" << std::endl;
	
		struct sockaddr_in server;
		server.sin_addr.s_addr = inet_addr("127.0.0.1");
		server.sin_family = AF_INET;
		server.sin_port = htons(1520);
	
		// Connect to remote server
		if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
		{
			std::cout<< "ERROR: Connection failed" << std::endl;
			return FAILURE;
		}
		std::cout << "Connected" << std::endl;
	}
	
	// Set up log file header
	if (output_mode == MODE_LIVE_LOG || output_mode == MODE_TEST || output_mode == MODE_DEBUG)
	{
		FILE * log_csv;
		log_csv = fopen("log.csv","w");		// clear log file
		fprintf(log_csv,"time (s)");        // begin header with time
		for (int i = 0; i < n_cars; i++)
		{
			// Print one set of headers for each car
			fprintf(log_csv,",found,x (mm),y (mm),v_x (mm/s),v_y (mm/s),theta (degrees)");
		}
		fprintf(log_csv,"\n");
		fclose(log_csv);
	}

	return 0;
}


int send_outputs(std::vector<struct Car> cars_all, int output_mode, int sock, struct Time sys_time, int frame)
{
    int ret = 0;
    switch (output_mode) {
    case MODE_LIVE:
        ret += send_json(cars_all, sock, false);
        break;
    case MODE_LIVE_CONS:
		ret += write_console(cars_all, frame);
		ret += send_json(cars_all, sock, false);
        break;
    case MODE_LIVE_LOG:
		ret += write_console(cars_all, frame);
		ret += send_json(cars_all, sock, false);
        ret += write_log(cars_all, sys_time);
        break;
    case MODE_TEST:
        ret += write_console(cars_all, frame);
        ret += write_log(cars_all, sys_time);
        break;
    case MODE_DEBUG:
        ret += write_console(cars_all, frame);
		ret += send_json(cars_all, sock, true);
		ret += write_log(cars_all, sys_time);
        //ret += other debug stuff (e.g. save/display masks, ...)
        break;
    default:
        // Code
        break;
    }

    if (ret > 0) {
        return FAILURE;
    }

    return SUCCESS;
}


int send_json(std::vector<struct Car> cars_all, int sock, bool debug)
{
	// Variables
	std::string json_string;// = "{";
	struct timeval tp;
	unsigned long long time_now;

	// Prepend with system time
	json_string.append("{\"time\":");
	gettimeofday(&tp, NULL);
	time_now = (unsigned long long)(tp.tv_sec) * 1000 + (unsigned long long)(tp.tv_usec) / 1000;
	json_string.append(std::to_string(time_now));
	
	// Reserve memory for the json string (increases efficiency)
	json_string.reserve(cars_all.size()*100);
	
	// Add data for each car found in current frame/instance
	for (int i = 0; i < cars_all.size(); i++)
	{
		if (cars_all[i].found)
		{
			json_string.append(",");					// key:value comma delimiter
			json_string.append("\"");					// leading quote for MAC address						
			json_string.append(cars_all[i].mac_add);	// MAC address
			json_string.append("\":[");				// end quote for MAC address, colon for key:value pairs, opening bracket for data array
			json_string.append("1");					// object type: 1 for cars
			json_string.append(",");
			json_string.append(std::to_string((int)round(cars_all[i].position_new[0])));	// x-position
			json_string.append(",");
			json_string.append(std::to_string((int)round(cars_all[i].position_new[1])));	// y-position
			json_string.append(",");
			json_string.append(std::to_string((int)round(cars_all[i].velocity_new[0])));	// x-velocity
			json_string.append(",");
			json_string.append(std::to_string((int)round(cars_all[i].velocity_new[1])));	// y-velocity
			json_string.append(",");
			json_string.append(std::to_string((int)round(cars_all[i].orientation_new)));	// orientation
			json_string.append(",");
			json_string.append("0");		// spare
			json_string.append(",");
			json_string.append("0");		// spare
			json_string.append("]");		// closing array bracket
		}
	}
	json_string.append("}");
	
	// Send JSON string
	if (debug) {
		// Debug mode: print JSON string to console
		std::cout << "JSON string: " << json_string << std::endl;
	} else {
		// Live mode: send JSON string to controller via socket
		if(send(sock, json_string.c_str(), json_string.size(), 0) < 0) {
			std::cout << "JSON send failed" << std::endl;
			return FAILURE;
		}
	}
	
	return SUCCESS;
}


int write_console(std::vector<struct Car> cars_all, int frame)
{
    std::cout << "=========================================================================" << std::endl;
    std::cout << "FRAME: " << frame << std::endl;
    for (int i = 0; i < cars_all.size(); i++)
    {		
		printf("Car: %-10s", cars_all[i].name.c_str());	// convert to a c string and pad to 10 characters
        if (cars_all[i].found)
        {
            printf(" - (%6.1f, %6.1f) mm,", cars_all[i].position_new[0], cars_all[i].position_new[1]);
            printf(" (%6.1f, %6.1f) mm/s,", cars_all[i].velocity_new[0], cars_all[i].velocity_new[1]);
            printf(" %3d degrees\n", cars_all[i].orientation_new);
        }
        else
        {
            printf(" - NOT DETECTED\n");
        }
	}
	
	return SUCCESS;
}


int write_log(std::vector<struct Car> cars_all, struct Time sys_time)
{
	// Open file
	FILE * log_csv;
	log_csv = fopen("log.csv","a");	// append mode

	// Begin entry with sime since tracking started (in seconds)
	fprintf(log_csv, "%7.3f,", (sys_time.current - sys_time.start)/(cv::getTickFrequency()));
	
	// Add data for each car
	for (int i = 0; i < cars_all.size(); i++) {
		fprintf(log_csv, "%d,", cars_all[i].found);
		fprintf(log_csv, "%6.1f,", cars_all[i].position_new[0]);
		fprintf(log_csv, "%6.1f,", cars_all[i].position_new[1]);
		fprintf(log_csv, "%6.1f,", cars_all[i].velocity_new[0]);
		fprintf(log_csv, "%6.1f,", cars_all[i].velocity_new[1]);
		fprintf(log_csv, "%i,", cars_all[i].orientation_new);
	}
	fprintf(log_csv, "\n");	// end enrty with a new line
	fclose(log_csv);
	
	return SUCCESS;
}

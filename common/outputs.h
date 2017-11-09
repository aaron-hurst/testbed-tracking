#ifndef CARSCV_OUTPUTS_H
#define CARSCV_OUTPUTS_H

/*General includes*/
#include <vector>

/*External libraries*/
#include <sys/socket.h>

/*Project includes*/
#include "time.h"
#include "car.h"

/*Macros*/
#define MODE_LIVE			0
#define MODE_LIVE_CONS		1
#define MODE_LIVE_LOG		2
#define MODE_TEST			3
#define MODE_DEBUG			4

/**********************************************************************************
* Check and report output mode
* 
* Prints description of selected output mode to the console. If specified output
* mode is not one of the available modes, output_mode is set to a default mode.
* 
* output_mode   currently set output mode
*
* return: output mode
**********************************************************************************/
int set_output_mode(int output_mode);


/**********************************************************************************
* Setup required components for system output
* 
* Depending on the output mode selected: (1) set up network socket for communication
* of JSON files to controller program, and (2) set up log file.
* 
* output_mode   currently set output mode
* sock          network socket for inter-program communication
* n_cars        number of cars
*
* return: 0 on success, 1 on failure
**********************************************************************************/
int output_setup(int output_mode, int &sock, int n_cars);


/**********************************************************************************
* Manages program outputs
* 
* Reads the current output mode and calls the appropriate output functions,
* passing them their required inputs.
* 
* cars_all      vector containing car structs (with state data)
* output_mode   current output mode
* sock          network socket for inter-program communication
* sys_time	    Time struct containing relevent timestamps
* frame         current frame (instance)
*
* return: 0 on success, 1 on failure
**********************************************************************************/
int send_outputs(std::vector<struct Car> cars_all, int output_mode, int sock, struct Time sys_time, int frame);


/**********************************************************************************
* Send vehicle states to controller program using a JSON string
* 
* Constructs a JSON string containing the current system timestamp (at the point the
* string is constructed) and state information for each currently found car. This
* string is then send via a network socket to the controller program. In debug mode
* the JSON string is printed to the console but not send to the controller.
*
* The JSON string is formatted as:
*   {"time":<timestamp>, <MAC address>:[x, y, vx, vy, theta, 0, 0], ... }
* Note 1: the two trailing zeros in the data array for each vehicle are spare
* values included for convenient extensibility.
* Note 2: car state information is rounded to the nearest integer before sending.
* 
* cars_all      vector containing car structs (with state data)
* sock          network socket for inter-program communication
* debug 	    boolean value describing whether to run in debug (true) or live
*               (false) mode
*
* return: 0 on success, 1 on failure
**********************************************************************************/
int send_json(std::vector<struct Car> cars_all, int sock, bool debug);


/**********************************************************************************
* Print all vehicles' state information to the console
* 
* Prints the current frame number on one line and the prints each vehicle's state
* information on separate lines. Formatted as:
*   Car: <name> - (x,y) mm, (vx, vy) mm/s, theta degrees
* 
* cars_all      vector containing car structs (with state data)
* output_mode   current output mode
* sock          network socket for inter-program communication
* sys_time	    Time struct containing relevent timestamps
* frame         current frame (instance)
*
* return: 0 on success, 1 on failure
**********************************************************************************/
int write_console(std::vector<struct Car> cars_all, int frame);


/**********************************************************************************
* Write all vehicles' state information to a log file
* 
* Appends current timestamp (in seconds since tracking commenced) and all vehicles'
* state information as a new line in a csv log file. Formatted as:
*   <timestamp>, <car 1 data>, <car 2 data>, ...
* Where <car i data> is formatted as:
*   x, y, vx, vy, theta
* 
* cars_all      vector containing car structs (with state data)
* sys_time	    Time struct containing relevent timestamps
*
* return: 0 on success, 1 on failure
**********************************************************************************/
int write_log(std::vector<struct Car> cars_all, struct Time sys_time);


#endif
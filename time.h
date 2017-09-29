#ifndef CARSCV_TIME_H
#define CARSCV_TIME_H

/**********************************************************************************
* Time Data Structure
*
* Stores various time values:
*	cur		when current image/instance was made in ticks
*	old		when previous image/instance was made in ticks
*	start	when tracking began in ticks
*	end		when tracking ceases in ticks
*	total	duration of tracking in seconds
**********************************************************************************/
struct Time
{
	double current;
	double old;
	double start;
	double end;
	double total;
};


#endif
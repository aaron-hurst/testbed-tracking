/*General includes*/
#include <iostream>		// cout
#include <string>		// to_string, string
#include <sstream>		// istringstream
#include <fstream>
#include <vector>

/*Project includes*/
#include "car.h"
#include "car_config.h"

#define FAILURE		1
#define SUCCESS		0

int cars_read_config(std::vector<struct Car>& cars_all)
{
	/*Variables*/
	std::string line, name, value, tmp;
	Car car_dummy;

	/*Open config file*/
	std::ifstream conf_file("config.txt");
	if (!conf_file) {
		std::cout << "Error loading config file (config.txt) for car data" << std::endl;
		return FAILURE;
	}

	/*Parse config.txt*/
	while (getline(conf_file, line)) {
		std::istringstream line_stream(line);
		line_stream >> name;
		
		/*Skip invalid lines and comments*/
		if (line_stream.fail() || name[0] == '#') {
            continue;
        }
		
		/*For each car, populate a dummy struct and push it to cars_all vector*/
		if (name == "Car") {
			line_stream >> tmp;
			if (tmp == "Y")	{ /*skip if not marked as "Y"*/
				while (getline(conf_file, line)) {
					std::istringstream line_stream(line);
					line_stream >> name;
					
					/*Skip invalid lines and comments*/
					if (line_stream.fail() || name[0] == '#') {
                        continue;
                    }
					
					/*Car parameters*/
					if 		(name == "name")	line_stream >> car_dummy.name;
					else if (name == "MAC_add")	line_stream >> car_dummy.mac_add;
					else if (name == "hue")		line_stream >> car_dummy.hue;
					else if (name == "delta")	line_stream >> car_dummy.delta;
					else if (name == "END")	{ /*signifies end of car config parameters*/
						cars_all.push_back(car_dummy);	/*push to cars_all vector*/
						break;
					}
				}
			}
		}
    }

    return SUCCESS;
}

void cars_print_config(std::vector<struct Car> cars_all)
{
	for (int i = 0; i < cars_all.size(); i++) {
		printf("================================\n");
		std::cout << "Car: "<< cars_all[i].name << std::endl;
		std::cout << " MAC Address: " << cars_all[i].mac_add << std::endl;
		std::cout << " mid hue:     " << cars_all[i].hue << std::endl;
		std::cout << " delta hue:   " << cars_all[i].delta << std::endl;
	}
	printf("================================\n\n");
}
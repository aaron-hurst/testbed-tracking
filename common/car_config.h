#ifndef CARSCV_CAR_CONFIG_H
#define CARSCV_CAR_CONFIG_H

/*Forward declarations*/
struct Car;

//=====================================
/*! @brief Read car config data from config file
 *
 * Reads and parses car-specific configuration information from the
 * config file, storing it in a Config struct.
 * 
 * @param cars_all Vector for storing all car objects
 * 
 * @return 0 on success, 1 on failure
 */
int cars_config_read(std::vector<struct Car>&);

//=====================================
/*! @brief Print car config information
 *
 * Prints car-specific configuration information to the specified pointer. This
 * may be either stdout or a log file. Rudimentary checking is completed (to
 * ensure pointer is not null and config is set); however, it is the calling
 * function's responsibility to ensure a valid pointer.
 * 
 * @param cars_all Vector containing all car objects
 * @param pointer Pointer to either stdout or a log file
 * 
 * @return 0 on success, 1 on failure
 */
int cars_config_print(std::vector<struct Car>, FILE*);

#endif /*CARSCV_CAR_CONFIG_H*/

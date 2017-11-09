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
int cars_read_config(std::vector<struct Car>&);

//=====================================
/*! @brief Print car config data to console
 *
 * @param cars_all Vector containing all car objects
 * 
 * @return void
 */
void cars_config_print(std::vector<struct Car>);

#endif /*CARSCV_CAR_CONFIG_H*/

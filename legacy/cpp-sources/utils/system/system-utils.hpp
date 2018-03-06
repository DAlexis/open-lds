/*
 * system.hpp
 *
 *  Created on: 29 марта 2016 г.
 *      Author: dalexies
 */

#ifndef UTILS_CONFIGURATION_SYSTEM_UTILS_HPP_
#define UTILS_CONFIGURATION_SYSTEM_UTILS_HPP_

#include <string>
#include <vector>

namespace SystemUtils
{
	std::string homeDir();
	std::string replaceTilta(const std::string& source);
	std::string probeFiles(const std::vector<std::string>& variants, const std::string& suffix = "");
}

#endif /* UTILS_CONFIGURATION_SYSTEM_UTILS_HPP_ */

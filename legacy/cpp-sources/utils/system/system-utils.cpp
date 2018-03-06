/*
 * system.cpp
 *
 *  Created on: 29 марта 2016 г.
 *      Author: dalexies
 */

#include "system-utils.hpp"

#include <boost/filesystem.hpp>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>


namespace SystemUtils
{

std::string homeDir()
{
	const char *homedir;

	if ((homedir = getenv("HOME")) == NULL) {
	    homedir = getpwuid(getuid())->pw_dir;
	}
	return std::string(homedir);
}

std::string replaceTilta(const std::string& source)
{
	std::string result = source;
	size_t pos = result.find(std::string("~"));
	if (pos != result.npos)
		result.replace(pos, 1, homeDir());

	return result;
}

std::string probeFiles(const std::vector<std::string>& variants, const std::string& suffix)
{
	for (auto& it : variants)
	{
		std::string fullName = it + suffix;

		if (boost::filesystem::exists(fullName))
		{
			return fullName;
		}
	}
	return "";
}

}

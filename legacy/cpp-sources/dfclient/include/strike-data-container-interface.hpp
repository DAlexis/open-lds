/*
 * strike-data-container-interface.hpp
 *
 *  Created on: 25 марта 2016 г.
 *      Author: dalexies
 */

#ifndef DFCLIENT_STRIKE_DATA_CONTAINER_INTERFACE_HPP_
#define DFCLIENT_STRIKE_DATA_CONTAINER_INTERFACE_HPP_

#include "serialization.hpp"
#include <string>

class IStrikeDataContainer
{
public:
	virtual ~IStrikeDataContainer() {}
	virtual std::string getMySQLQuery() const = 0;
	virtual void setDevExpId(int deviceId, int experimentId) = 0;
};



#endif /* DFCLIENT_STRIKE_DATA_CONTAINER_INTERFACE_HPP_ */

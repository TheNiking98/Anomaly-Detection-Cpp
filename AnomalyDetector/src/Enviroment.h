#ifndef ENVIROMENT_H
#define ENVIROMENT_H

#include <vector>
#include <string>
#include <utility> // Per std::pair
#include "SensorData.h"
#include "RedisManager.h"
#include "../../con2db/pgsql.h"
#include <unordered_map>
#include <boost/functional/hash.hpp>

std::tuple<int, int, std::vector<SensorData>,std::vector<std::pair<int, std::string>>,std::unordered_map<std::pair<int, int>, double, boost::hash<std::pair<int, int>>>> 
processCSVAndSendToRedis(const std::string &filename, RedisManager &redis, Con2DB &db1);

#endif // ENVIROMENT_H


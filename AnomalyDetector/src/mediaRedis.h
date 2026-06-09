#ifndef MEDIAREDIS_H
#define MEDIAREDIS_H

#include <string>
#include <unordered_map>
#include <vector> 
#include <hiredis/hiredis.h>

std::unordered_map<int, std::pair<double, int>> calcMediaRedis(redisContext* context, const std::string& streamKey);
std::vector<std::pair<int, std::string>> generaMedia(std::unordered_map<int, std::pair<double, int>>& sensorMap);


#endif // MEDIAREDIS_H

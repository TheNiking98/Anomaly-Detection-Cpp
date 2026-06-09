#ifndef MEDIA_TO_DB_H
#define MEDIA_TO_DB_H

#include <unordered_set>
#include <vector>
#include <string>
#include <utility>
#include "../../con2db/pgsql.h"
#include "Enviroment.h"
#include <unordered_map>
#include <boost/functional/hash.hpp> 

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2>& pair) const {
        // Combina gli hash del primo e del secondo elemento della coppia
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
};

void mediaToDB(Con2DB db1, const std::vector<std::pair<std::string, double>>& sensorAverages);

void insertNullAverages(Con2DB &db1, const std::vector<std::string>& nullSensors);

void insertSensorData(Con2DB &db1, const std::vector<SensorData>& sensorData);

void covarianceToDB(Con2DB db1, const std::unordered_map<std::pair<int, int>, double, boost::hash<std::pair<int, int>>>& covarianceMap);

void anomaliesToDB(Con2DB &db1, const std::unordered_set<int>& anomalies);

#endif // MEDIA_TO_DB_H

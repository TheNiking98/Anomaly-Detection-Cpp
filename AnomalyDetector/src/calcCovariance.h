#ifndef CALC_COVARIANCE_H
#define CALC_COVARIANCE_H

#include <unordered_map>
#include <string>
#include <utility>
#include <boost/functional/hash.hpp>
#include <hiredis/hiredis.h>

// Dichiarazione della funzione per calcolare le covarianze tra sensori
std::unordered_map<std::pair<int, int>, double, boost::hash<std::pair<int, int>>> calculateCovariances(
    redisContext* context,
    const std::string& streamKey,
    const std::unordered_map<int, std::pair<double, int>>& sensorMap,
    int W
);

// Dichiarazione della funzione per convertire stringa in double, tenendo conto dei "NULL"
bool convertValue(const std::string& str, double& outValue);

#endif // CALC_COVARIANCE_H

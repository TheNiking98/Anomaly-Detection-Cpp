#include "mediaToDB.h"
#include "Enviroment.h"
#include "main.h"
#include <cstdio>
#include <iostream>
#include <cmath>

void anomalyDetection(
    const std::vector<std::pair<int, std::string>>& w1avg,
    const std::unordered_map<std::pair<int, int>, double, boost::hash<std::pair<int, int>>>& w1cov,
    const std::vector<std::pair<int, std::string>>& w2avg,
    const std::unordered_map<std::pair<int, int>, double, boost::hash<std::pair<int, int>>>& w2cov,
    double sigmaMultiplier, Con2DB &db1);
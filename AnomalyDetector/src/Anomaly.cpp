#include "mediaToDB.h"
#include "Enviroment.h"
#include "main.h"
#include <cstdio>
#include <iostream>
#include <cmath>
#include <numeric>
#include <unordered_set>

// Funzione per convertire valori stringa in double, tenendo conto dei "NULL"
bool convertValue2(const std::string& str, double& outValue) {
    if (str == "NULL") {
        return false;
    }
    try {
        outValue = std::stod(str);
        return true;
    } catch (...) {
        return false;
    }
}

// Funzione per calcolare la deviazione standard
double calculateStandardDeviation(const std::vector<double>& values) {
    if (values.size() <= 1) return 0.0;

    double mean = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
    double sq_sum = std::inner_product(values.begin(), values.end(), values.begin(), 0.0,
                                       std::plus<>(), [mean](double a, double b) {
                                           return (a - mean) * (b - mean);
                                       });
    return std::sqrt(sq_sum / (values.size() - 1));
}

// Funzione principale per l'anomaly detection
void anomalyDetection(
    const std::vector<std::pair<int, std::string>>& w1avg,
    const std::unordered_map<std::pair<int, int>, double, boost::hash<std::pair<int, int>>>& w1cov,
    const std::vector<std::pair<int, std::string>>& w2avg,
    const std::unordered_map<std::pair<int, int>, double, boost::hash<std::pair<int, int>>>& w2cov,
    double sigmaMultiplier, Con2DB &db1) // Output vector for anomalies
{
    std::unordered_set<int> anomalies; // Vettore per ID sensori anomali

    // Costruzione vettori delle medie e covarianze per W1
    std::vector<double> w1MeanValues, w1CovValues;

    for (const auto& [sensorID, meanStr] : w1avg) {
        double mean;
        if (convertValue2(meanStr, mean)) {
            w1MeanValues.push_back(mean);
        }
    }

    for (const auto& [sensorPair, cov] : w1cov) {
        w1CovValues.push_back(cov);
    }

    // Calcola deviazioni standard per le soglie di W1
    double meanThreshold = sigmaMultiplier * calculateStandardDeviation(w1MeanValues);
    double covarianceThreshold = sigmaMultiplier * calculateStandardDeviation(w1CovValues);

    // Mappa per le medie di W1 per accesso rapido
    std::unordered_map<int, double> w1MeanMap;
    for (const auto& [sensorID, meanStr] : w1avg) {
        double mean;
        if (convertValue2(meanStr, mean)) {
            w1MeanMap[sensorID] = mean;
        }
    }

    // Controlla anomalie delle medie in W2
    for (const auto& [sensorID, meanStr] : w2avg) {
        double mean;
        if (convertValue2(meanStr, mean) && w1MeanMap.count(sensorID)) {
            if (std::abs(mean - w1MeanMap[sensorID]) > meanThreshold) {
                anomalies.insert(sensorID);
            }
        }
    }

    // Controlla anomalie delle covarianze in W2
    for (const auto& [sensorPair, cov] : w2cov) {
        if (w1cov.count(sensorPair)) {
            if (std::abs(cov - w1cov.at(sensorPair)) > covarianceThreshold) {
                anomalies.insert(sensorPair.first);  // Aggiungi solo uno dei due sensori della coppia
                anomalies.insert(sensorPair.second);
            }
        }
    }
    
    anomaliesToDB(db1, anomalies);

    // Stampa i risultati (o restituire il vettore se richiesto)
    std::cout << "Sensori Anomali: ";
    for (int sensorID : anomalies) {
        std::cout << sensorID << " ";
    }
    std::cout << std::endl;
}
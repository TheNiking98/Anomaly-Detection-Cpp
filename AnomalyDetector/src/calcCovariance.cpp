#include <unordered_map>
#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include "SensorData.h"
#include <hiredis/hiredis.h>
#include "../../con2db/pgsql.h"
#include "calcCovariance.h"
#include <boost/functional/hash.hpp> 

// Funzione per calcolare le covarianze tra sensori usando le medie da sensorMap
std::unordered_map<std::pair<int, int>, double, boost::hash<std::pair<int, int>>> calculateCovariances(
    redisContext* context, const std::string& streamKey,
    const std::unordered_map<int, std::pair<double, int>>& sensorMap,
    int W) 
{
    // Mappa per memorizzare la somma dei prodotti delle deviazioni e il conteggio dei valori validi
    std::unordered_map<std::pair<int, int>, std::pair<double, int>, boost::hash<std::pair<int, int>>> covarianceMap; 
    redisReply* reply;

    reply = (redisReply*)redisCommand(context, "XRANGE %s - +", streamKey.c_str());

    // Itera sui dati filtrati per calcolare la somma dei prodotti delle deviazioni dalla media
    for (size_t i = 0; i < reply->elements; ++i) {
        for (size_t j = i + 1; j < reply->elements; ++j) {
            redisReply* entry1 = reply->element[i];
            redisReply* entry2 = reply->element[j];

            // Se i sensori sono diversi
            if (entry1->element[1]->element[3]->str != entry2->element[1]->element[3]->str && entry1->element[1]->element[1]->str == entry2->element[1]->element[1]->str) {
                double value1, value2;
                bool valid1 = convertValue(entry1->element[1]->element[5]->str, value1);
                bool valid2 = convertValue(entry2->element[1]->element[5]->str, value2);

                if (valid1 && valid2) {
                    // Ottieni le medie dai dati di sensorMap
                    double mean1 = sensorMap.at(std::stoi(entry1->element[1]->element[3]->str)).first;
                    double mean2 = sensorMap.at(std::stoi(entry2->element[1]->element[3]->str)).first;

                    // Calcola il prodotto delle deviazioni dalla media
                    double deviationProduct = (value1 - mean1) * (value2 - mean2);

                    // Aggiorna la mappa delle covarianze
                    auto& [sumProducts, count] = covarianceMap[{std::stoi(entry1->element[1]->element[3]->str), std::stoi(entry2->element[1]->element[3]->str)}];
                    sumProducts += deviationProduct;
                    count++;
                } else {
                    // Se uno dei due valori è "NULL", la covarianza sarà NULL
                    covarianceMap[{std::stoi(entry1->element[1]->element[3]->str), std::stoi(entry2->element[1]->element[3]->str)}] = {0, 0};
                }
            }
        }
    }

    // Normalizzazione finale per ottenere le covarianze
    std::unordered_map<std::pair<int, int>, double, boost::hash<std::pair<int, int>>> finalCovarianceMap;
    for (const auto& [sensorPair, sumAndCount] : covarianceMap) {
        const auto& [sumProducts, count] = sumAndCount;

        if (count <= 1) {
            finalCovarianceMap[sensorPair] = 0; // Covarianza è "NULL" se i dati non sono sufficienti
        } else {
            finalCovarianceMap[sensorPair] = sumProducts / (count - 1); // Calcolo della covarianza
        }
    }

    return finalCovarianceMap;
}

// Funzione per convertire stringa in double, tenendo conto dei "NULL"
bool convertValue(const std::string& str, double& outValue) {
    if (str == "NULL") {
        return false; // Indica che il valore è "NULL"
    }
    try {
        outValue = std::stod(str); // Converti la stringa in double
        return true;
    } catch (...) {
        return false; // Gestione di errori in conversione
    }
}


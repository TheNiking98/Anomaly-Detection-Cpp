#include "mediaRedis.h"
#include <iostream>
#include <vector>
#include <map>
#include <utility>
#include <hiredis/hiredis.h>
#include <unordered_map>

std::unordered_map<int, std::pair<double, int>> calcMediaRedis(redisContext* context, const std::string& streamKey) {
    std::unordered_map<int, std::pair<double, int>> sensorData;
    redisReply* reply;

    
    reply = (redisReply*)redisCommand(context, "XRANGE %s - +", streamKey.c_str());


    for (size_t i = 0; i < reply->elements; ++i) {
        redisReply* entry = reply->element[i];

        if (sensorData.find(std::stoi(entry->element[1]->element[3]->str)) == sensorData.end()) {
            sensorData[std::stoi(entry->element[1]->element[3]->str)] = std::make_pair(0.0, 0);
        }
    }
    

    if (reply->type == REDIS_REPLY_ARRAY) {
        // Itera su ogni entry della stream
        for (size_t i = 0; i < reply->elements; ++i) {
            redisReply* entry = reply->element[i];

            std::string value(entry->element[1]->element[5]->str);

            if (value != " NULL") {

                double value = std::stod(entry->element[1]->element[5]->str);
                sensorData[std::stoi(entry->element[1]->element[3]->str)].first += value; // Aggiungi alla somma
                sensorData[std::stoi(entry->element[1]->element[3]->str)].second += 1;    // Incrementa il conteggio dei valori non NULL
            }
        }
    }

    freeReplyObject(reply);
    return sensorData;
}


    

// Funzione per generare una unordered_map di risultati con media o 'NULL'
std::vector<std::pair<int, std::string>> generaMedia(std::unordered_map<int, std::pair<double, int>>& sensorMap) {
    std::vector<std::pair<int, std::string>> results;
    
    for (const auto& entry : sensorMap) {
        int sensorID = entry.first;
        double sum = entry.second.first;
        int count = entry.second.second;
        //std::cout << sensorID << " somma: " << sum << "count:" << count <<std::endl;
        if (count == 0) {
            results.emplace_back(sensorID, "NULL");
            sensorMap[entry.first].first = 0.0;
        } else {
            double average = sum / count;
            results.emplace_back(sensorID, std::to_string(average));
            sensorMap[entry.first].first = average;
        }
    }

    return results;
}

#include "Enviroment.h"
#include "SensorData.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "mediaRedis.h"
#include <unordered_map>
#include <vector>
#include <utility>
#include "mediaToDB.h"
#include "main.h"
#include "calcCovariance.h"
#include <cmath>
#include <boost/functional/hash.hpp> 

// Funzione per leggere i dati dal CSV
std::vector<SensorData> readCSV(const std::string &filename) {
    std::vector<SensorData> data;
    std::ifstream file(filename);
    std::string line, word;

    // Legge l'intestazione
    std::getline(file, line);

    // Legge i dati riga per riga
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        SensorData entry;
        
        try {
            std::getline(ss, word, ',');
            entry.sampleTime = std::stoi(word);
            
            std::getline(ss, word, ',');
            entry.sensorID = std::stoi(word);
            
            std::getline(ss, word, ',');
            if (word ==" NULL") { 
                entry.value = word.empty() ? "NULL" : word; 
            } else {
                entry.value = std::to_string(std::stod(word));
            }
            
            
        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid data format in CSV: " << line << std::endl;
            continue;
        } catch (const std::out_of_range& e) {
            std::cerr << "Data out of range in CSV: " << line << std::endl;
            continue;
        }

        data.push_back(entry);
    }

    return data;
}

// Funzione per processare il CSV e inviare i dati a Redis
std::tuple<int, int, std::vector<SensorData>,std::vector<std::pair<int, std::string>>,std::unordered_map<std::pair<int, int>, double, boost::hash<std::pair<int, int>>>> 
processCSVAndSendToRedis(const std::string &filename, RedisManager &redis, Con2DB &db1) {
    // Legge i dati dal CSV
    std::vector<SensorData> data = readCSV(filename);
    if (data.empty()) {
        std::cerr << "Il CSV non contiene dati o non è stato letto correttamente." << std::endl;
    } else {
        std::cout << "Letti " << data.size() << " record dal CSV." << std::endl;
    }

    // Seleziona un tempo casuale
    int W = 0;
    std::cout << "Inserisci la finestra temporale W: ";
    std::cin >> W;
    int maxTime = (data.back().sampleTime);
    int randomTime = (std::rand() % (maxTime - W + 1));
    std::cout << "RandomTime: " << randomTime << std::endl;

    // Filtra i dati in base alla finestra temporale
    std::vector<SensorData> filteredData;
    for (const auto& entry : data) {
        if (entry.sampleTime >= randomTime && entry.sampleTime <= randomTime + W - 1) {
            filteredData.push_back(entry);
        }
    }

    // Invia i dati filtrati a Redis
    if (!redis.sendToRedis(filteredData, randomTime, W)) {
        std::cerr << "Failed to send data to Redis" << std::endl;
    }
    std::cout << "dati per test inviati a redis." << std::endl;

    // Inserisci i dati filtrati nella tabella SensorData
    insertSensorData(db1, filteredData);

    const std::string streamKey = "sensor_data";

    // Calcola la media dei valori dei sensori
    auto sensorMap = calcMediaRedis(redis.getContext(), "sensor_data");
    std::cout << "Media per testing calcolata." << std::endl;


    // Genera i risultati
    std::vector<std::pair<int, std::string>> sensorResults = generaMedia(sensorMap);
    auto covarianceMap = calculateCovariances (redis.getContext(), "sensor_data", sensorMap, W);

    std::cout << "Covarianza per testing calcolata." << std::endl;


    // Converti i risultati in std::pair<std::string, double> per mediaToDB
    std::vector<std::pair<std::string, double>> sensorAverages;
    std::vector<std::string> nullSensors;  // Per memorizzare gli ID dei sensori con valori NULL

    for (const auto& entry : sensorResults) {
        std::string sensorID = std::to_string(entry.first);  // Converte l'ID del sensore da int a string

        // Se la media è NULL, aggiungi l'ID del sensore a nullSensors
        if (entry.second == "NULL") {
            nullSensors.push_back(sensorID);  // Aggiungi il sensore con valore NULL
            sensorAverages.emplace_back(sensorID, 0);
        } else {
            // Converte la media da string a double se non è NULL
            double average = std::stod(entry.second);
            sensorAverages.emplace_back(sensorID, average);  // Inserisci la media nel vettore
        }
    }

    // Chiamata alla funzione mediaToDB per inserire le medie nel database
    mediaToDB(db1, sensorAverages);

    // Chiamata per inserire i sensori con valore NULL nella tabella NullAverages
    insertNullAverages(db1, nullSensors);
    covarianceToDB(db1, covarianceMap);
    
    redisCommand(redis.getContext(), "FLUSHALL");


    return {randomTime + W, W, data, sensorResults, covarianceMap};
}

 /*
for (const auto& result : sensorResults) {
        std::cout << "Sensore ID: " << result.first << ", Media valori: " << result.second << std::endl;
    }
*/   

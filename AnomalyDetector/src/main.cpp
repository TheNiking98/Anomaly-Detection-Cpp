#include <iostream>
#include <cstdlib>
#include <ctime>
#include "RedisManager.h"
#include "Enviroment.h"
#include "../../con2db/pgsql.h"
#include "mediaToDB.h"
#include "mediaRedis.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>
#include <unistd.h>
#include <sys/stat.h>
#include "calcCovariance.h"
#include "Anomaly.h"

bool fileExists(const std::string& filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

int main() {
    // Cambiare directory e eseguire script
    if (chdir("/home/luca/Desktop/Progetto/AnomalyDetector/db-scripts") != 0) {
        std::cerr << "Errore nel cambiare la directory" << std::endl;
        return 1;
    }
    const char* command = "./create.sh";
    int result = std::system(command);
    if (result == 0) {
        std::cout << "Script eseguito con successo!" << std::endl;
    } else {
        std::cerr << "Errore nell'esecuzione dello script. Codice di ritorno: " << result << std::endl;
    }

    // Connettersi al DB
    Con2DB db1("localhost", "5432", "anomalydec", "47002", "logdb_anomalydec");
    std::srand(std::time(nullptr));

    // Connettersi a Redis
    RedisManager redis;
    if (!redis.connect("127.0.0.1", 6379)) {
        std::cerr << "Failed to connect to Redis" << std::endl;
        return 1;
    }

    // Verifica del file CSV
    std::string filename = "/home/luca/Desktop/Progetto/AnomalyDetector/src/data/Sensordataset.csv";
    if (!fileExists(filename)) {
        std::cerr << "File does not exist: " << filename << std::endl;
        return 1;
    }

    // Processare il CSV e determinare randomTime, W e data
    auto [randomTime, W, data, w1avg, w1cov] = processCSVAndSendToRedis(filename, redis, db1);
    std::cout << "randomTime: " << randomTime << ", W: " << W << std::endl;

    

    std::vector<SensorData> newData;
    for (const auto& entry : data) {
        if (entry.sampleTime >= randomTime && entry.sampleTime <= randomTime + W - 1) {
            newData.push_back(entry);
        }
    }

    if (!redis.sendToRedis(newData, randomTime, W)) {
        std::cerr << "Failed to send data to Redis" << std::endl;
    }

    auto sensorMap = calcMediaRedis(redis.getContext(), "sensor_data");

    std::vector<std::pair<int, std::string>> sensorAverages = generaMedia(sensorMap);
    for (const auto& result : sensorAverages) {
        std::cout << "Sensore ID: " << result.first << ", Media valori: " << result.second << std::endl;
    }
    
    auto covarianceMap = calculateCovariances(redis.getContext(), "sensor_data", sensorMap, W);
    std::cout << "Covarianze calcolate." << std::endl;

    anomalyDetection(w1avg, w1cov, sensorAverages, covarianceMap, 3.0, db1);
    
    
    redisCommand(redis.getContext(), "FLUSHALL");
    redis.disconnect();
    return 0;
}



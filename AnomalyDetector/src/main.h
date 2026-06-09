#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#include <cstdlib>
#include <ctime>
#include "RedisManager.h"
#include "Enviroment.h"
#include "calcCovariance.h"
//#include "../../con2db/pgsql.h"

// Funzione per il processo di invio dati da CSV a Redis a PostgreSQL
std::vector<SensorData> processCSVAndSendToRedis(const std::string& csvFile, RedisManager& redis);

#endif // MAIN_H

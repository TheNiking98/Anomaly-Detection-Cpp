#ifndef REDISMANAGER_H
#define REDISMANAGER_H

#include <string>
#include <vector>
#include <hiredis/hiredis.h>
#include "SensorData.h"

class RedisManager {
public:
    bool connect(const std::string &hostname, int port);
    void disconnect();
    bool sendToRedis(const std::vector<SensorData> &data, int startTime, int W);
    bool command(const std::string &cmd);
    redisContext* getContext() const; // Aggiungi questo prototipo
private:
    redisContext* context = nullptr; // Assicurati che ci sia questo membro
};
#endif // REDISMANAGER_H

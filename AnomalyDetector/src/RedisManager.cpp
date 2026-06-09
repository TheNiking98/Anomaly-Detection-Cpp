#include "RedisManager.h"
#include <iostream>

bool RedisManager::connect(const std::string &hostname, int port) {
    context = redisConnect(hostname.c_str(), port);
    if (context == nullptr || context->err) {
        if (context) {
            std::cerr << "Error: " << context->errstr << std::endl;
            redisFree(context);
        } else {
            std::cerr << "Can't allocate redis context" << std::endl;
        }
        return false;
    }
    return true;
}

void RedisManager::disconnect() {
    if (context) {
        redisFree(context);
        context = nullptr;
    }
}

bool RedisManager::sendToRedis(const std::vector<SensorData> &data, int startTime, int W) {
    if (context == nullptr) {
        std::cerr << "No connection to Redis" << std::endl;
        return false;
    }

    std::string streamName = "sensor_data";

    for (const auto &entry : data) {
        if (entry.sampleTime >= startTime && entry.sampleTime < startTime + W) {
            redisReply *reply = (redisReply *)redisCommand(context,
                "XADD %s * SampleTime %d SensorID %d Value %s",
                streamName.c_str(), entry.sampleTime, entry.sensorID, entry.value.c_str());
            if (reply == nullptr) {
                std::cerr << "Failed to execute XADD command" << std::endl;
                redisFree(context);
                return false;
            }
            freeReplyObject(reply);
        }
    }
    return true;
}

bool RedisManager::command(const std::string &cmd) {
    if (context == nullptr) {
        std::cerr << "No connection to Redis" << std::endl;
        return false;
    }

    redisReply *reply = (redisReply *)redisCommand(context, cmd.c_str());
    if (reply == nullptr) {
        std::cerr << "Failed to execute Redis command" << std::endl;
        return false;
    }
    freeReplyObject(reply);
    return true;
}

redisContext* RedisManager::getContext() const {
    return context; // Restituisce il contesto di Redis
}
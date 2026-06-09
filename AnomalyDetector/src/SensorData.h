#ifndef SENSORDATA_H
#define SENSORDATA_H

#include <string>

struct SensorData {
    int sampleTime; 
    int sensorID;   
    std::string value;           
};

#endif // SENSORDATA_H
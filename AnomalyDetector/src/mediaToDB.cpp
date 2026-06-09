#include "mediaToDB.h"
#include "Enviroment.h"
#include "main.h"
#include <cstdio>
#include <unordered_set>
#include <iostream>
#include <cmath>

// Funzione per inserire le medie nel database
void mediaToDB(Con2DB db1, const std::vector<std::pair<std::string, double>>& sensorAverages) {
    PGresult *res;
    char sqlcmd[512]; // Buffer per il comando SQL

    // Inizio della transazione
    sprintf(sqlcmd, "BEGIN");
    res = db1.ExecSQLcmd(sqlcmd);
    PQclear(res);

    // Inserimento dei dati nella tabella SensorAverage
    for (const auto& sensor : sensorAverages) {
        if (sensor.second == 0) {
            // Inserisci NULL per varvalue se la media è NULL
            sprintf(sqlcmd, 
                "INSERT INTO SensorAverages (SensorID, varvalue) VALUES ('%s', NULL) "
                "ON CONFLICT (SensorID) DO UPDATE SET varvalue = EXCLUDED.varvalue",
                sensor.first.c_str());
        } else {
            // Inserisci la media calcolata normalmente
            sprintf(sqlcmd, 
                "INSERT INTO SensorAverages (SensorID, varvalue) VALUES ('%s', %f) "
                "ON CONFLICT (SensorID) DO UPDATE SET varvalue = EXCLUDED.varvalue",
                sensor.first.c_str(),
                sensor.second);
        }

        res = db1.ExecSQLcmd(sqlcmd);
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            std::cerr << "Errore nell'inserimento: " << PQerrorMessage(db1.GetConn()) << std::endl;
        }
        PQclear(res);
    }

    // Fine della transazione
    sprintf(sqlcmd, "COMMIT");
    res = db1.ExecSQLcmd(sqlcmd);
    PQclear(res);

#if (DEBUG > 0)
    // Controllo dei dati inseriti
    sprintf(sqlcmd, "SELECT * FROM SensorAverages");
    res = db1.ExecSQLtuples(sqlcmd);
    int rows = PQntuples(res);
    for (int i = 0; i < rows; i++) {
        fprintf(stderr, "mediaToDB(): inserted in SensorAverages ('%s', %f)\n",
                PQgetvalue(res, i, PQfnumber(res, "SensorID")),
                atof(PQgetvalue(res, i, PQfnumber(res, "varvalue"))));
    }
    PQclear(res);
#endif
}

void insertNullAverages(Con2DB &db1, const std::vector<std::string>& nullSensors) {
    PGresult *res;
    char sqlcmd[512];  // Buffer per il comando SQL

    // Inizio della transazione
    sprintf(sqlcmd, "BEGIN");
    res = db1.ExecSQLcmd(sqlcmd);
    PQclear(res);

    // Inserisci gli ID dei sensori con media NULL nella tabella NullAverages
    for (const auto& sensorID : nullSensors) {
        sprintf(sqlcmd, 
            "INSERT INTO NullAverages (SensorID) VALUES ('%s') "
            "ON CONFLICT (SensorID) DO NOTHING",  // Evita duplicati
            sensorID.c_str());

        res = db1.ExecSQLcmd(sqlcmd);
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            std::cerr << "Errore nell'inserimento di NullAverages: " << PQerrorMessage(db1.GetConn()) << std::endl;
        }
        PQclear(res);
    }

    // Fine della transazione
    sprintf(sqlcmd, "COMMIT");
    res = db1.ExecSQLcmd(sqlcmd);
    PQclear(res);
}

// Funzione per inserire i dati nella tabella SensorData
void insertSensorData(Con2DB &db1, const std::vector<SensorData>& sensorData) {
    PGresult *res;
    char sqlcmd[512];  // Buffer per il comando SQL

    // Inizio della transazione
    sprintf(sqlcmd, "BEGIN");
    res = db1.ExecSQLcmd(sqlcmd);
    PQclear(res);

    // Inserisci i dati grezzi nella tabella SensorData
    for (const auto& data : sensorData) {
        sprintf(sqlcmd, 
            "INSERT INTO SensorData (SensorID, SampleTime, SensorValue) VALUES ('%s', to_timestamp(%d), %s)",
            std::to_string(data.sensorID).c_str(),
            data.sampleTime,
            data.value == "NULL" ? "NULL" : data.value.c_str());

        res = db1.ExecSQLcmd(sqlcmd);
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            std::cerr << "Errore nell'inserimento dei dati del sensore: " << PQerrorMessage(db1.GetConn()) << std::endl;
        }
        PQclear(res);
    }

    // Fine della transazione
    sprintf(sqlcmd, "COMMIT");
    res = db1.ExecSQLcmd(sqlcmd);
    PQclear(res);
}

void covarianceToDB(Con2DB db1, const std::unordered_map<std::pair<int, int>, double, boost::hash<std::pair<int, int>>>& covarianceMap) {
    PGresult *res;
    char sqlcmd[512]; // Buffer per il comando SQL

    // Inizio della transazione
    sprintf(sqlcmd, "BEGIN");
    res = db1.ExecSQLcmd(sqlcmd);
    PQclear(res);

    // Inserimento dei dati nella tabella SensorCovariance
    for (const auto& entry : covarianceMap) {
        const auto& sensorPair = entry.first;
        int sensorID1 = sensorPair.first;
        int sensorID2 = sensorPair.second;
        double covariance = entry.second;

        if (covariance == 0) {
            // Inserisci NULL per covariance se la covarianza è 0
            sprintf(sqlcmd, 
                "INSERT INTO SensorCovariance (SensorID1, SensorID2, Covariance) VALUES ('%d', '%d', NULL) "
                "ON CONFLICT (SensorID1, SensorID2) DO UPDATE SET Covariance = EXCLUDED.Covariance",
                sensorID1, sensorID2);
        } else {
            // Inserisci la covarianza calcolata normalmente
            sprintf(sqlcmd, 
                "INSERT INTO SensorCovariance (SensorID1, SensorID2, Covariance) VALUES ('%d', '%d', %f) "
                "ON CONFLICT (SensorID1, SensorID2) DO UPDATE SET Covariance = EXCLUDED.Covariance",
                sensorID1, sensorID2,
                covariance);
        }

        // Esegui il comando SQL
        res = db1.ExecSQLcmd(sqlcmd);
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            std::cerr << "Errore nell'inserimento: " << PQerrorMessage(db1.GetConn()) << std::endl;
        }
        PQclear(res);
    }

    // Fine della transazione
    sprintf(sqlcmd, "COMMIT");
    res = db1.ExecSQLcmd(sqlcmd);
    PQclear(res);

#if (DEBUG > 0)
    // Controllo dei dati inseriti (opzionale per il debug)
    sprintf(sqlcmd, "SELECT * FROM SensorCovariance");
    res = db1.ExecSQLtuples(sqlcmd);
    int rows = PQntuples(res);
    for (int i = 0; i < rows; i++) {
        fprintf(stderr, "covarianceToDB(): inserted in SensorCovariance ('%s', '%s', %f)\n",
                PQgetvalue(res, i, PQfnumber(res, "SensorID1")),
                PQgetvalue(res, i, PQfnumber(res, "SensorID2")),
                atof(PQgetvalue(res, i, PQfnumber(res, "Covariance"))));
    }
    PQclear(res);
#endif
}

void anomaliesToDB(Con2DB &db1, const std::unordered_set<int>& anomalies) {
    PGresult *res;
    char sqlcmd[128];  // Buffer per il comando SQL

    // Inizio della transazione
    sprintf(sqlcmd, "BEGIN");
    res = db1.ExecSQLcmd(sqlcmd);
    PQclear(res);

    // Inserisci gli ID dei sensori anomali nella tabella SensorAnomalies
    for (const int sensorID : anomalies) {
        sprintf(sqlcmd, "INSERT INTO SensorAnomalies (SensorID) VALUES (%d) ON CONFLICT (SensorID) DO NOTHING", sensorID);

        res = db1.ExecSQLcmd(sqlcmd);
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            std::cerr << "Errore nell'inserimento dell'ID del sensore anomalo " << sensorID << ": " << PQerrorMessage(db1.GetConn()) << std::endl;
        }
        PQclear(res);
    }

    // Fine della transazione
    sprintf(sqlcmd, "COMMIT");
    res = db1.ExecSQLcmd(sqlcmd);
    PQclear(res);
}

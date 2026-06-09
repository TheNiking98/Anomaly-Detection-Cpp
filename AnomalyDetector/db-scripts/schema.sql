-- Connessione al database esistente
\c logdb_anomalydec

-- Creazione dei domini per i tipi di dati
CREATE DOMAIN TimePoint AS timestamp;
CREATE DOMAIN String4VarName AS VARCHAR(30);
CREATE DOMAIN String4Info AS VARCHAR(200);
CREATE DOMAIN VarType AS double precision;

-- Creazione della tabella per i dati grezzi del sensore
CREATE TABLE IF NOT EXISTS SensorData (
    SensorID String4VarName,  -- ID del sensore (VARCHAR 30)
    SampleTime TimePoint,     -- Istante di tempo (VARCHAR 30)
    SensorValue VarType       -- Valore del sensore (può essere NULL)
);

-- Creazione della tabella per le medie dei sensori
CREATE TABLE IF NOT EXISTS SensorAverages (
    SensorID String4VarName PRIMARY KEY,       -- ID del sensore
    varvalue VarType                       -- Valore medio del sensore (può essere NULL)
);

-- Creazione della tabella per i sensori con media NULL
CREATE TABLE IF NOT EXISTS NullAverages (
    SensorID String4VarName PRIMARY KEY        -- ID del sensore con media NULL
);

-- Creazione della tabella per la covarianza tra sensori
CREATE TABLE IF NOT EXISTS SensorCovariance (
    SensorID1 String4VarName,       -- ID del primo sensore
    SensorID2 String4VarName,       -- ID del secondo sensore
    Covariance VarType,             -- Covarianza tra i due sensori
    PRIMARY KEY (SensorID1, SensorID2)
);

-- Creazione della tabella per le anomalie
CREATE TABLE IF NOT EXISTS SensorAnomalies (
    SensorID INT PRIMARY KEY       -- ID del sensore
);

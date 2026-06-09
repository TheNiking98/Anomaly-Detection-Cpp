\c logdb_anomalydec postgres

-- user already exists
GRANT ALL PRIVILEGES ON DATABASE logdb_anomalydec to anomalydec ;


ALTER TABLE SensorData OWNER TO anomalydec ;
ALTER TABLE SensorAverages OWNER TO anomalydec ;
ALTER TABLE NullAverages OWNER TO anomalydec ;
ALTER TABLE SensorCovariance OWNER TO anomalydec ;
ALTER TABLE SensorAnomalies OWNER TO anomalydec ;

-- grant all privileges on all tables in schema public to :username ;
-- grant all privileges on all sequences in schema public to :username ;

GRANT ALL ON SCHEMA public TO anomalydec ;
GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA public TO anomalydec ;



-- This creates the database :dbname

drop database if exists logdb_anomalydec ;
CREATE DATABASE logdb_anomalydec ;


-- This creates the user :username

\c logdb_anomalydec postgres

-- erase role if already created
REASSIGN OWNED BY anomalydec TO postgres ;
REVOKE ALL PRIVILEGES ON ALL TABLES IN SCHEMA public FROM anomalydec ;
REVOKE ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA public FROM anomalydec ;
REVOKE ALL PRIVILEGES ON ALL FUNCTIONS IN SCHEMA public FROM anomalydec ;
DROP OWNED BY anomalydec;
DROP USER IF EXISTS anomalydec ;
CREATE USER anomalydec WITH ENCRYPTED PASSWORD '47002' ;
-- user created




\c logdb_anomalydec postgres

-- grant privileges to user

GRANT ALL PRIVILEGES ON DATABASE logdb_anomalydec to anomalydec ;

GRANT ALL ON SCHEMA public TO anomalydec ;


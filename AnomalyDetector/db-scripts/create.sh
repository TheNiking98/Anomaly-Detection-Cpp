#!/bin/bash

whoami

set -x

sudo -u postgres psql postgres -f /home/luca/Desktop/Progetto/AnomalyDetector/db-scripts/parameters.sql \
                          -f /home/luca/Desktop/Progetto/AnomalyDetector/db-scripts/create-db-user.sql \
                          -f /home/luca/Desktop/Progetto/AnomalyDetector/db-scripts/schema.sql \
                          -f /home/luca/Desktop/Progetto/AnomalyDetector/db-scripts/grant.sql


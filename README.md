# Anomaly Detection System - Ingegneria del Software

Questo software è un sistema di **Anomaly Detection** sviluppato in **C++** per l'esame di *Ingegneria del Software* (Ottobre 2024). Il sistema analizza i dati provenienti da una serie di sensori, calcola metriche statistiche e identifica anomalie sfruttando la deviazione standard come discriminante principale.

## Stack Tecnologico
* **Linguaggio:** C++
* **Database Relazionale:** PostgreSQL (per lo storage a lungo termine e le relazioni)
* **In-Memory Data Store:** Redis (utilizzato per la gestione rapida e la cache dei dati filtrati)
* **Input dei dati:** File in formato CSV

---

## Funzionamento del Sistema

Il software opera ricevendo in input un file CSV e una **finestra temporale ($W_e$)** specificata dall'utente. Il flusso di elaborazione principale segue questi step:

1. **Filtraggio:** Il sistema filtra i dati del CSV in base alla finestra temporale inserita e li invia a Redis.
2. **Calcolo Statistico:** Vengono calcolate le medie e le covarianze dei rispettivi sensori all'interno di quella determinata finestra.
3. **Rilevamento Anomalie:** Il sistema determina quali valori si discostano significativamente dai trend correnti utilizzando la **deviazione standard**.
4. **Persistenza:** Vengono create e popolate 5 tabelle specifiche sul database PostgreSQL per memorizzare i risultati analizzati.

### Struttura del Database (Tabelle)
* `sensordata`: Tutti i sensori appartenenti alla finestra temporale con i rispettivi valori.
* `nullaverages`: Sensori nulli (che non hanno registrato valori nella finestra temporale).
* `sensoraverages`: Tutte le medie calcolate per sensore.
* `sensorcovariance`: Tutte le covarianze calcolate.
* `sensoranomalies`: Tutti gli ID dei sensori rilevati come anomalie.

---

## Architettura dei Componenti

Il sistema è suddiviso in moduli logici ben definiti:

* **Main:** Avvia l'applicazione, esegue gli script SQL iniziali per preparare l'ambiente, stabilisce le connessioni a Redis/PostgreSQL e orchestra le fasi di calcolo.
* **Environment:** Gestisce l'acquisizione del file CSV, riceve l'input dell'utente, filtra i dati temporali e coordina l'ordine di esecuzione delle analisi.
* **RedisManager:** Si occupa strettamente della gestione della connessione con Redis e della pulizia della cache (`Flush All`).
* **mediaRedis:** Calcola le medie dei sensori.
* **CalcCovariances:** Calcola le covarianze dei sensori.
* **Anomaly:** Identifica le anomalie basandosi sulla deviazione standard.
* **MediaToDB:** Si occupa dell'interfacciamento con PostgreSQL per il caricamento massivo dei dati nelle rispettive tabelle.

### Script del Database (`db-scripts/`)
Nella directory dedicata sono presenti i file SQL per la gestione automatica del database:
* `create-db-user.sql`: Resetta/Crea il database, l'utente dedicato e ne configura i privilegi.
* `schema.sql`: Definisce i domini e la struttura delle tabelle.
* `grant.sql`: Assegna i giusti permessi di lettura/scrittura all'utente sulle tabelle create.

---

## 📊 Requisiti Utente e d'Uso
L'utente interagisce con il sistema potendo:
1. Inserire la finestra temporale di analisi.
2. Visionare direttamente sul database PostgreSQL l'esito delle elaborazioni (valori filtrati, medie, covarianze, sensori nulli e anomalie individuate).

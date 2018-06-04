# Sistemi Operativi

### Progetto 2 - Statistiche comandi ed analisi

Implementazione di una shell _(non interattiva)_ in grado di comprendere, eseguire e loggare comandi.

## Compilazione

Aprire un terminale nella directory corrente _(vedere `cd --help` per posizionarsi correttamente se necessario)_ ed eseguire:

```
make build
```

Nella cartella `./bin` verranno creati i file oggetto che saranno linkati a creare l'eseguibile `xlogger`.

## Esecuzione

Posizionarsi nella directory locale `./bin` ed eseguire:

```
./xlogger [OPTION]... "SCRIPT"
```

Per maggiori informazioni:

```
./xlogger --help
```
_Ulteriore documentazione in `./documents`._

## Componenti del gruppo
- `[182003]` **Federico Favotto**
- `[186354]` **Tommaso Bonomo**
- `[186520]` **Andrea Zanotto**
- `[186633]` **Francesco Cadei**

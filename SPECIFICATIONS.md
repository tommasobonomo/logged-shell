# Program flow
```bash
$ ./xlog --logfile="/tmp/run.log" --format=csv "ls | wc"
$ ./xlog --logfile="/tmp/run.log" --format=csv "ls; wc"
$ ./xlog --logfile="/tmp/run.log" --outpath="/tmp/out.txt" --outoverride --format=csv "ls; wc"
```

## Esecuzione comandi
- Analizzare il comando
- Eseguire in ordine i comandi
- creare una struttura con:
  - ID
  - stringa del comando totale
  - path del file di log
  - formato del file di log
  - log file mode: crea o errore (default crea)
  - < output | error > stream mode (scarta, stampaAVideo, scriviSuLog, fileOverwrite, fileAppend) _implementare con la redirezione(usare /dev/null per scartare)_
  - creare un array di strutture di n posti, dove n è il numero di sub-comandi
    - nome del sub-comando
    - parametri del sub-comando eseguito
    - **Inserisci statistiche**

## Ottenere un'istanza del logger
- se non esiste creare il demone
- comunicare con il demone passandogli la struttura _(fare con file? cos'è più efficiente)_

## Demone
- formatta i dati ricevuti
- aprire il file di log
  - gestire errori particolari I/O
  - se non esite
    - flag se crearlo o errore (default crea)
  - se esiste
    - **attenzione a non mischiare formati diversi**

-----

# Specifiche
## Shell & Parser
- [ ] Creare il processo come demone
- [ ] Spezzare il comando da eseguire in token
  - [ ] `|`
  - [ ] `;`
  - [ ] `>` e `<`
  - [ ] `&` (sia con comando dopo che no)
  - [ ] `''`

## Argomenti
- [ ] Creare un help molto descrittivo
- [ ] Creare argomenti
  - [ ] normali
    - [ ] `-f` formato del file di log
    - [ ] `-hr` time human readable
    - [ ] `-e` non crea il file di log se non esiste, ma restituisce un errore
  - [ ] mnemonici
    - [ ] `--format` _alias_ `-f`

## Output
- [ ] output del comando su log o a console
- [ ] Diversi tipi di file di output:
  - [ ] txt
  - [ ] csv (delimitatore a scelta)
  - [ ] ~~excel~~
  - [ ] ~~pdf~~
- [ ] cancellare il file di log (controlla se non ci sono processi collegati)
- [ ] rotazione del log con cancellazione dell'ultimo o creazione di più file (log1, log2, ...) (occhio al fatto che il file dovrebbe essere univoco)

## Statistiche
- [ ] tempo di avvio
- [ ] tempo di conclusione
- [ ] durata del comando in secondi
- [ ] codice di ritorno del comando
- [ ] numero di righe di output
- [ ] numero di righe di errori
- [ ] pid, gruppo e utente con cui viene eseguito ogni comando
- [ ] la priorità del processo
- [ ] CPU usata  (avg, max)
- [ ] RAM usata (avg, max)
- [ ] CPU, I/O e wait time
- [ ] Quanti file ha aperto (avg, max)
- [ ] quali altri processi vengono chiamati all'interno di un comando _(possibile?)_

## Makefile
- [ ] compila
- [ ] clean
- [ ] usare i file obj
- [ ] usare guardie di compilazione `#IFDEF nomefile`

## Special effects
- [ ] aggiungere una entry su man con un help esteso
- [ ] gestire il comando sudo
- [ ] stare attenti a comandi che chiedono input
- [ ] creare un wrapper per ogni chiamata di sistema che gestisca gli errori
- [ ] usare parallelizzazione hardware

-----

# Domande
- creiamo un demone per ogni file di log? o uno per tutti?
  lo possiamo clonare?

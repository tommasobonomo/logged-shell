# Program flow
```
$ ./xlog --logfile="/tmp/run.log" --format=csv "ls | wc"
$ ./xlog --logfile="/tmp/run.log" --format=csv "ls; wc"
```

## Ottenere un'istanza del logger
- se non esiste creare il demone
- comunicare con il demone passandogli in una struttura:
  - array con tutti i comandi da eseguire
  - array con le priorità associate ad ogni comando (stessa priorità = esecuzione concorrente)
    - [1,2,3,4] pipe
    - [1000,2000,3000] comandi indipendenti
    - [1,1,1] comandi concorrenti
    - [-5] comando in backgruond
  - path del file di log
  - log file mode: append o overwrite
  - stream < output | error | ~~log~~ > mode (scarta, video, log, fileOverwrite, fileAppend)
  - stream < output | error | ~~log~~ > path
  - **TODO ogni altro parametro**

## aprire il file di log
- gestire errori particolari I/O
- se non esite
  - flag se crearlo o errore (default crea)
- se esiste
  - append o overwrite (**attenzione a non mischiare formati diversi**)

-----

# Specifiche
## Features
- [ ] Creare il processo come demone
- [ ] Spezzare il comando da eseguire in token
  - [ ] `&` (sia con comando dopo che no)
  - [ ] `>` e `<`
  - [ ] `|`
  - [ ] `;`
  - [ ] `''`

## Argomenti
- [ ] Creare un help molto descrittivo
- [ ] Creare argomenti
  - [ ] normali
    - [ ] `-f` formato del file di log
    - [ ] `-hr` time human readable
    - [ ] `-e` non crea il file di log se non esiste, ma restituisce un errore
    - [ ] `-a` appende in coda l'output al file di log se esiste
  - [ ] mnemonici
    - [ ] `--format` _alias_ `-f`

## Output
- [ ] output con append o overwrite
- [ ] output del comando su log o a console
- [ ] Diversi tipi di file di output:
  - [ ] txt
  - [ ] csv (delimitatore a scelta)
  - [ ] ~~excel~~
  - [ ] ~~pdf~~

## Statistiche
- [ ] tempo di avvio
- [ ] tempo di conclusione
- [ ] durata del comando in secondi
- [ ] codice di ritorno del comando
- [ ] stampa la catena di comandi in ordine
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
- [ ] creare un wrapper per ogni chiamata di sistema che gestisca gli errori
- [ ] usare parallelizzazione hardware

-----

# Domande
- creiamo un demone per ogni file di log? o uno per tutti?
  lo possiamo clonare?

# Documentazione Xlogger

## Usage

__`./bin/xlogger [OPTION]... "[SCRIPT]"`__

Informazioni di compilazione ed esecuzione a fine documento.

---

## Features

Alcune delle principali features implementate:

- ### Logger
    Il processo che si occupa di scrivere le statistiche raccolte su file (logger) è implementato tramite demone custom, cioè non si utilizza una libreria per crearlo. Inoltre, il logger è univoco e comune a qualsiasi istanza del tool xlogger, pertanto se su uno stesso sistema sono in esecuzione più istanze del tool, esse comunicheranno le statistiche raccolte al logger che scriverà su file.

    Il logger stesso viene lanciato all'esecuzione della prima istanza del tool e chiuso al termine dell'ultima istanza, evitando di rimanere in background alla chiusura di tutte le istanze.

- ### Operatori
    Vengono gestiti i seguenti operatori:
    - di controllo: `&&`, `||`, `;`, `|`
    - di ridirezione: `>`, `<`, `>>`
    Utilizzabili ed intercambiabili più volte nello stesso comando.
    Gli operatori logici (`&&`, `||`) sono "lazy evaluated", aspettando l'esecuzione del sottocomando di sinistra prima di eseguire quello di destra.

- ### Esecuzione dei sottoprocessi e piping
    Il processo principale genera un processo per ogni sottocomando, successivamente eseguito tramite `execvp`. In seguito, le statistiche vengono raccolte dal processo principale, che pertanto aspetta la fine dell'esecuzione di ogni sottocomando.
    In caso di operatore `|`, il processo principale utilizzerà dei threads per raccogliere le statistiche: si avrà quindi la continua esecuzione dei sottocomandi, evitando pertanto un comportamento bloccante.

- ### Gestione di sottocomandi
    Se lo `SCRIPT` contiene sottocomandi (es. `ls | wc` o `echo 1; ls`), questi vengono eseguiti separatamente, con statistiche raccolte in modo individuale e raggruppate alla stampa a seconda del comando di appartenenza.

- ### Builtins (`cd`)
    Viene gestito il sottocomando `cd`, che effettua il cambio della directory corrente per l'intero ambiente ambiente di scripting (caratterizzando quindi il comportamento di tutti i successivi sottocomandi della sequenza). Essendo comunque un sottocomando valido, vengono raccolte e loggate le sue statistiche.

- ### Statistiche
    Raccolta di una serie di statistiche, visualizzabili in un normale file di log. La raccolta avviene tramite il riempimento di una struttura di libreria `rusage`, fornendo statistiche come:

```
    ############################## STATS ###########################
    #
    #      2018-05-27 21:32:40
    #
    #      whole command: pacman -Syu
    #      n° subcommand: 1
    #           username: root (0)
    #
    #    ========================================================
    #
    #                 subcommand: pacman -Syu
    #
    #                        PID:         14374
    #                       PGRP:         14371
    #                        SID:          3924
    #                exit status:             0
    #               elapsed time:      0.758215 s
    #              CPU time used:      0.016561 s
    #               max ram size:         27612 kB
    #           soft page faults:          5287
    #           hard page faults:             0
    #                      swaps:             0
    #           signals received:             0
    #      vol. context switches:            13
    #      inv. context switches:            17
    #
    #################################################################

```

---

## Compilazione

È necessario eseguire il tool `make` nella cartella del progetto. A partire da i file `.c`, verranno creati i file oggetto salvati nella cartella temporanea `/bin` e linkati a creare l'eseguibile `xlogger`.

La regola `make clean` rimuove la directory `/bin` e eventualmente la cartella del tool creata nella directory `/tmp` nella root.

---

## Esecuzione

L'eseguibile `xlogger` nella cartella `/bin` si occupa di eseguire un programma `SCRIPT` definito dall'utente e di raccogliere statistiche riguardanti il programma stesso. Le statistiche vengono mandate a un processo "logger", implementato come un demone, che scrive la statistiche su file.

Vi sono una varietà di flag passabili al tool per modificare il suo comportamento:

- `-a`, `-appout=FILE`: permette di appendere l'output dello `SCRIPT` ad un `FILE` a discrezione dell'utente;
- `-A`, `-apperr=FILE`: identico al flag precedente, eccetto che appende gli errori invece che l'output;
- `-e`, `--printerr`: stampa gli errori a video, comportamento di default;
- `-f`, `--format=FORMAT`: specifica il formato del file di log contenente le statistiche. Può essere `"txt"`(default) o `"csv"`;
- `--nerr` e/o `--nout`: non stampa errori e/o output dei sottocomandi, rispettivamente, mentre eventuali errori e/o output del tool sono stampati normalmente;
- `-o`, `--printout`: stampa l'output dei sottocomandi a video;
- `-p`, `--path=FILE`: specifica il file dove scrivere le statistiche;
- `-q`, `--quiet`: non stampare assolutamente niente a video, nemmeno errori o output del tool;
- `-s`, `--show`: stampa le statistiche contenute nel file di log direttamente a video nel terminal;
- `-S`, `--cshow=FILE`: stampa le statistiche contenute nel file di log personalizzato direttamente a video;
- `-x`, `--overout=FILE`: sovrascrivi nel FILE stampando gli output dei sottocomandi;
- `-X`, `--overerr=FILE`: sovrascrivi nel FILE stampando gli errori dei sottocomandi;
- `--help`: mostra la schermata di help e termina;
- `--version`: stampa la versione corrente del tool e termina.

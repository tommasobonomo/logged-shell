# Documentazione Xlogger

## Compilazione

È necessario eseguire il tool `make` nella cartella del progetto. A partire da i file `.cpp`, verranno creati i file oggetto, salvati nella cartella temporanea `/bin` e linkati a creare l'eseguibile `xlogger`.

La regola `make clean` rimuove la directory `/bin` e eventualmente la cartella del tool creata nella directory `/tmp` nella root.

## Esecuzione

L'eseguibile `xlogger` nella cartella `/bin` si occupa di eseguire un programma definito dall'utente (`SCRIPT`) e di raccogliere statistiche riguardanti il programma stesso. Le statistiche vengono mandare a un processo "logger", implementato come un demone, che scrive la statistiche su file. 

Vi sono una varietà di flag passabili al tool per modificare il suo comportamento:

- `-a`, `-appout=FILE`: permette di appendere l'output dello `SCRIPT` ad un `FILE` a discrezione dell'utente;
- `-A`, `-apperr=FILE`: identico al flag precedente, eccetto che appende gli errori invece che l'output;
- `-e`, `--printerr`: stampa gli errori a video, comportamento di default;
-  `-f`, `--format=FORMAT`: specifica il formato del file di log contenente le statistiche. Può essere `"txt"`(default) o `"csv"`.
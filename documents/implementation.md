# Scelte implementative

## Struttura

### Logged shell
Unico processo multithread (gestione non bloccante per la memorizzazione delle statistiche di processi in piping).

### operatori
Ogni SCRIPT passato alla logged_shell viene scomposto in sottocomandi opportunamente valutati rispettando eventuali operatori:
 - control operator: `&&`, `||`
 - redirection operator

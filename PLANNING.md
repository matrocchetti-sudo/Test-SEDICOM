# Piano di lavoro

## Obiettivo

Portare il progetto da specifica a applicazione CLI C++17 funzionante per la gestione delle spese, con persistenza locale su `expenses.csv`, architettura modulare e comandi coerenti con il README.

## Principi del piano

- Ogni task deve produrre un avanzamento reale e verificabile.
- I task sono piccoli, ordinati per dipendenze e con scope limitato.
- La persistenza CSV e la CLI minima vengono completate prima di filtri e report avanzati.

## Task incrementali

### 1. Inizializzare la struttura del progetto

Creare `CMakeLists.txt` e le directory `src/`, `include/` e `tests/`.

Risultato atteso: il repository ha una struttura compilabile e pronta ad accogliere i moduli previsti.

### 2. Definire il modello `Expense`

Creare la struct `Expense` con i campi `id`, `amount`, `category`, `date`, `description`.

Risultato atteso: esiste un tipo dati centrale condiviso da tutti i moduli.

### 3. Definire le interfacce dei moduli principali

Creare gli header di `ExpenseManager`, `DataHandler` e `CLI_Interface`, fissando responsabilità e API minime.

Risultato atteso: l'architettura descritta nel README è codificata in interfacce stabili.

### 4. Creare il punto di ingresso dell'applicazione

Implementare `main` con bootstrap dei moduli e parsing base del comando principale.

Risultato atteso: il programma parte e riconosce almeno i comandi previsti, anche con stub temporanei.

### 5. Implementare il caricamento CSV vuoto o inesistente

Far sì che `DataHandler` apra `expenses.csv` se presente e restituisca una collezione vuota se il file non esiste.

Risultato atteso: l'app può avviarsi senza dati preesistenti e senza errori inutili.

### 6. Implementare il salvataggio CSV

Scrivere in `expenses.csv` intestazione e record nel formato definito dal README.

Risultato atteso: i dati in memoria possono essere persistiti in modo portabile e leggibile.

### 7. Collegare `ExpenseManager` alla persistenza

Fare in modo che il manager carichi i dati all'avvio e li salvi dopo ogni modifica.

Risultato atteso: la persistenza tra sessioni è realmente funzionante.

### 8. Implementare la generazione di ID univoci

Calcolare il prossimo ID disponibile a partire dai record caricati.

Risultato atteso: ogni nuova spesa è eliminabile in modo affidabile tramite ID.

### 9. Implementare il comando `add`

Gestire `--amount`, `--cat`, `--date` e `--description`, creare la spesa e salvarla.

Risultato atteso: l'utente può registrare una spesa completa da CLI.

### 10. Validare l'input del comando `add`

Verificare importo positivo, categoria non vuota, data nel formato `YYYY-MM-DD`, descrizione gestibile.

Risultato atteso: i dati salvati sono coerenti e gli errori sono espliciti.

### 11. Implementare il comando `list`

Mostrare tutte le spese in formato tabellare leggibile da terminale.

Risultato atteso: l'utente può consultare rapidamente lo storico delle spese.

### 12. Ordinare l'output di `list`

Stabilire un ordinamento coerente, ad esempio per ID o data, e mantenerlo stabile.

Risultato atteso: l'output è prevedibile e semplice da verificare.

### 13. Implementare il comando `delete`

Gestire `--id`, rimuovere la spesa corrispondente e salvare il file aggiornato.

Risultato atteso: l'utente può correggere o rimuovere record errati.

### 14. Gestire i casi di cancellazione non valida

Restituire messaggi chiari per ID assente, non numerico o file inconsistente.

Risultato atteso: la cancellazione fallisce in modo sicuro e comprensibile.

### 15. Implementare il totale complessivo delle spese

Aggiungere in `ExpenseManager` il calcolo del totale aggregato.

Risultato atteso: la base del comando `stats` è pronta.

### 16. Implementare il comando `stats`

Visualizzare almeno il totale complessivo speso.

Risultato atteso: il comando richiesto dal README produce un primo riepilogo utile.

### 17. Aggiungere il totale per categoria

Calcolare e mostrare i totali raggruppati per categoria.

Risultato atteso: la reportistica diventa realmente utile per l'analisi delle spese.

### 18. Implementare il filtro per categoria

Consentire il filtraggio delle spese tramite categoria, integrandolo in `list` o in un comando dedicato.

Risultato atteso: l'utente può isolare velocemente un sottoinsieme di spese.

### 19. Implementare il filtro per intervallo di date

Permettere di cercare spese tra due date valide.

Risultato atteso: il requisito di filtraggio temporale è coperto.

### 20. Riutilizzare la logica di filtro in manager e CLI

Separare il calcolo dei filtri dalla sola formattazione a video.

Risultato atteso: la logica resta testabile e coerente con l'architettura modulare.

### 21. Aggiungere statistiche estese

Implementare almeno media delle spese e totale mensile, come suggerito dalla roadmap del README.

Risultato atteso: il modulo di reportistica copre anche la parte evolutiva prevista.

### 22. Gestire correttamente CSV malformati

Rilevare righe incomplete, numeri non validi e intestazioni errate con messaggi chiari.

Risultato atteso: il programma non corrompe i dati e non fallisce in modo opaco.

### 23. Migliorare l'help CLI

Mostrare uso corretto, sintassi dei comandi e opzioni supportate quando l'input è incompleto o errato.

Risultato atteso: l'app è autoesplicativa da terminale.

### 24. Aggiungere test unitari per `ExpenseManager`

Verificare aggiunta, cancellazione, calcolo totale, filtri e generazione ID.

Risultato atteso: la logica di business principale è protetta da regressioni.

### 25. Aggiungere test per `DataHandler`

Verificare lettura e scrittura CSV, inclusi file mancanti e righe invalide.

Risultato atteso: la persistenza è coperta nei casi normali e negli edge case più importanti.

### 26. Aggiungere test end-to-end minimi della CLI

Validare almeno i flussi `add`, `list`, `delete`, `stats` usando file temporanei.

Risultato atteso: i principali comandi funzionano insieme, non solo isolatamente.

### 27. Rifinire output e messaggi d'errore

Uniformare testo, codici di uscita e formattazione tabellare.

Risultato atteso: l'applicazione è più robusta e gradevole da usare.

### 28. Aggiornare README con comandi reali e istruzioni build/test

Allineare la documentazione all'implementazione definitiva.

Risultato atteso: il progetto è eseguibile e comprensibile senza conoscenza implicita.

## Milestone suggerite

### Milestone 1: Progetto compilabile

Task 1-4.

### Milestone 2: Persistenza base funzionante

Task 5-8.

### Milestone 3: Flusso CRUD minimo

Task 9-14.

### Milestone 4: Analisi e filtri

Task 15-21.

### Milestone 5: Robustezza e chiusura progetto

Task 22-28.

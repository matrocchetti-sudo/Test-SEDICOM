# Gestione spese

## 1. Obiettivo del Progetto

Realizzare un'applicazione da riga di comando (CLI) che permetta all'utente di registrare, visualizzare, eliminare e analizzare le proprie spese quotidiane. Il programma deve garantire la persistenza dei dati tra una sessione e l'altra.

## 2. Requisiti Funzionali

Il programma dovrà supportare le seguenti operazioni principali:

* **Aggiunta Spesa:** Inserimento di importo, categoria (es. Cibo, Trasporti), data e una breve descrizione.
* **Visualizzazione:** Elenco di tutte le spese registrate in formato tabellare.
* **Filtraggio:** Ricerca spese per categoria o per intervallo di date.
* **Cancellazione:** Rimozione di una spesa tramite un ID univoco.
* **Reportistica:** Calcolo del totale speso complessivo o suddiviso per categoria.
* **Persistenza:** Salvataggio automatico dei dati su un file (CSV o JSON).

## 3. Architettura del Sistema

L'applicazione seguirà un approccio modulare per separare la logica di business dall'interfaccia utente:

1.  **Model (`Expense`):** Classe/Struct che rappresenta la singola spesa.
2.  **Controller (`ExpenseManager`):** Gestisce la logica (aggiunta, rimozione, calcoli).
3.  **Persistence (`DataHandler`):** Gestisce la lettura e scrittura su file.
4.  **View (`CLI_Interface`):** Gestisce l'input dell'utente e la formattazione dell'output a video.

---

## 4. Specifiche Tecniche e Struttura Dati

### 4.1. Lo Stack Tecnico

* **Linguaggio:** C++17 o superiore (per utilizzare `std::filesystem` e `std::optional`).
* **Build System:** CMake.
* **Librerie consigliate:**
    * `nlohmann/json` (se scegli il formato JSON per il salvataggio).
    * `FTXUI` o semplicemente `iostream` per la CLI.
    * `chrono` per la gestione delle date.

### 4.2. Definizione della Struttura Dati

```cpp
struct Expense {
    int id;
    double amount;
    std::string category;
    std::string date; // Formato YYYY-MM-DD
    std::string description;
};
```

---

## 5. Interfaccia Utente (Design dei Comandi)

Il programma interagirà con l'utente tramite argomenti da linea di comando. Ecco un esempio della sintassi ipotizzata:

| Comando | Descrizione | Esempio |
| :--- | :--- | :--- |
| `add` | Aggiunge una nuova spesa | `./expense add --amount 15.50 --cat Cibo` |
| `list` | Mostra tutte le spese | `./expense list` |
| `stats` | Mostra il riepilogo totale | `./expense stats` |
| `delete` | Rimuove una spesa tramite ID | `./expense delete --id 5` |

---

## 6. Persistenza dei Dati

Per semplicità e portabilità, i dati verranno salvati in un file `expenses.csv` nella stessa cartella dell'eseguibile.

**Esempio di formato CSV:**
```csv
id,amount,category,date,description
1,12.50,Pranzo,2024-03-20,Pizza con colleghi
2,45.00,Benzina,2024-03-21,Pieno auto
```

---

## 7. Roadmap di Sviluppo

1.  **Fase 1:** Definizione della classe `Expense` e gestione dell'input base.
2.  **Fase 2:** Implementazione della logica di memorizzazione in memoria (`std::vector<Expense>`).
3.  **Fase 3:** Implementazione dei moduli di lettura/scrittura su file CSV.
4.  **Fase 4:** Aggiunta di filtri di ricerca e statistiche (media, totale mensile).
5.  **Fase 5 (Bonus):** Gestione degli errori (input non validi, file corrotti).

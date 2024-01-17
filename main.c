#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#define TABLE_SIZE 5000  // Dimensione dell'array hash table

typedef struct {
    int distanza;
    int nAuto;
    int* autonomie;
} Stazione;

typedef struct {
    Stazione *stazione;
    struct Nodo *next;
    struct Nodo *prev;
}Nodo;

typedef struct {
    Nodo *head;  // Puntatore alla testa della lista
    Nodo *tail;  // Puntatore alla coda della lista
} Bucket;

typedef struct {
    Bucket table[TABLE_SIZE];
    int size;  // Numero di elementi nella hash table
} HashTable;

typedef struct {
    int km;
    int autonomiaMax;
    struct NodoList *next;
    struct NodoList *prev;
}NodoList;

typedef struct ListaStazioni {
    int lunghezza;
    NodoList * testa;
    NodoList * coda;
} ListaStazioni;

typedef struct{
    int km;
    int numTappe;
    int StazionePrec;
}StatoStazione;

void ottieniStazioniIntermedieOrdinate(ListaStazioni *percorso, int partenza, int arrivo, HashTable* autostrada);

void initHashTable(HashTable *hashTable) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        hashTable->table[i].head = NULL;
        hashTable->table[i].tail = NULL;
    }
    hashTable->size = 0;
}

int hash(int key) {
    return key % TABLE_SIZE;
}

void insertStation(HashTable *hashTable, Stazione *station) {
    int index = hash(station->distanza);

    Nodo *newNode = (Nodo *)malloc(sizeof(Nodo));
    newNode->stazione = station;
    newNode->next = NULL;
    newNode->prev = NULL;

    // Inserisci nella lista
    if (hashTable->table[index].head == NULL) {
        hashTable->table[index].head = newNode;
        hashTable->table[index].tail = newNode;
    } else {
        hashTable->table[index].tail->next = (struct Nodo *) newNode;
        newNode->prev = (struct Nodo *) hashTable->table[index].tail;
        hashTable->table[index].tail = newNode;
    }

    hashTable->size++;
}

Stazione* findStation(HashTable *hashTable, int distance) {
    int index = hash(distance);

    Nodo *current = hashTable->table[index].head;
    while (current != NULL) {
        if (current->stazione->distanza == distance) {
            return current->stazione;
        }
        current = (Nodo *) current->next;
    }

    return NULL;  // Stazione non trovata
}

void stampaStazioni(HashTable *table) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        Nodo *current = table->table[i].head;
        while (current != NULL) {
            Stazione *stazione = current->stazione;
            printf("Distanza: %d\n", stazione->distanza);
            printf("Numero Auto: %d\n", stazione->nAuto);
            printf("Autonomia max: %d\n", stazione->autonomie[0]);
            printf("Autonomie: ");
            for (int j = 1; j < stazione->nAuto + 1; j++) {
                printf("%d ", stazione->autonomie[j]);
            }
            printf("\n");

            current = (Nodo *) current->next;
        }
    }
}

ListaStazioni* creaLista() {
    ListaStazioni* lista = (ListaStazioni*)malloc(sizeof(ListaStazioni));
    lista->testa = NULL;
    lista->coda = NULL;
    lista->lunghezza = 0;
    return lista;
}

void liberaLista(ListaStazioni* lista) {
    NodoList * current = lista->testa;

    if(current==NULL){
        free(lista);
        printf("lista vuota liberata\n");
        return;
    }

    while (current != NULL) {
        NodoList * temp = current;
        current = (NodoList *) current->next;
        //free(temp->stazione->autonomie);
        //free(temp->stazione);
        free(temp);
    }
    free(lista);
}

void aggiungiInTesta(ListaStazioni* lista, int stazione) {
    NodoList* nuovoNodo = (NodoList*)malloc(sizeof(NodoList));
    if (nuovoNodo == NULL) {
        // Gestisci l'errore di allocazione di memoria
        printf("Errore di allocazione di memoria nuovoNodo\n");
        return; // o un altro tipo di gestione dell'errore
    }

    nuovoNodo->km = stazione;
    nuovoNodo->next = (struct NodoList *) lista->testa;
    nuovoNodo->prev = NULL;

    if (lista->testa != NULL) {
        lista->testa->prev = (struct NodoList*)nuovoNodo;
    } else {
        lista->coda = nuovoNodo;
    }

    lista->testa = nuovoNodo;
    lista->lunghezza++;
}

void stampaLista(ListaStazioni* lista) {
    NodoList * current = lista->testa;
    while (current != NULL) {
        printf("%d", current->km);
        current = (NodoList *) current->next;

        if(current!=NULL){
            printf(" ");
        }else{
            printf("\n");
        }
    }
}

void riempiDistanze(ListaStazioni *lista, int arrivoKm ,int* distanze){
    //int arrivo = percorso->percorso->coda->km;
    int start = lista->testa->km;
    //int numStazioni = percorso->lunghezza;

    NodoList * stazione = lista->testa;

    int i = 0;
    int fine = 0;

    while(!fine){
        distanze[i] = stazione->km - start;
        if(stazione->km==arrivoKm){
            fine = 1;
        }else {
            stazione = (NodoList *) stazione->next;
            i++;
        }
    }

}

void riempiDistanzeBack(ListaStazioni *lista, int arrivoKm, int *distanze) {
    if (lista == NULL || lista->coda == NULL || distanze == NULL) {
        // Verifica che le strutture siano valide
        return;
    }

    int start = lista->coda->km;

    NodoList *stazione = lista->coda;
    int i = 0;

    while (stazione != NULL) {
        distanze[i] = abs(stazione->km - start);
        //printf("distanze i: %d\n", distanze[i]);
        //printf("stazioneKm: %d\n", stazione->km);
        if (stazione->km == arrivoKm) {
            break;
        }
        stazione = (NodoList *)stazione->prev;
        //printf("stazioneSucc: %d\n", stazione->km);
        i++;
    }
}


// Funzione per ricostruire il percorso minimo dai dati precedenti
ListaStazioni* estraiPercorsoMinimo(StatoStazione* precedenti, int numStazioni) {
    ListaStazioni* percorsoMinimo = creaLista();

    int indiceStazione = numStazioni - 1;
    int kmArrivo = precedenti[indiceStazione].km;

    // Aggiungi la stazione di arrivo al percorso minimo
    aggiungiInTesta(percorsoMinimo, kmArrivo);

    int kmStazionePrec = precedenti[indiceStazione].StazionePrec;
    int stazionePrec = indiceStazione - 1;

    if(kmStazionePrec==-1){
        //printf("nessun percorso\n");
        return NULL;
    }

    // Aggiungi la stazione precedente alla prima iterazione
    aggiungiInTesta(percorsoMinimo, kmStazionePrec);

    // Costruisci il percorso minimo retrocedendo dal percorso precedenti
    while (stazionePrec > 0) {
        if (precedenti[stazionePrec].km == kmStazionePrec) {
            kmStazionePrec = precedenti[stazionePrec].StazionePrec;
            aggiungiInTesta(percorsoMinimo, kmStazionePrec);
        }
        stazionePrec--;
    }

    return percorsoMinimo;
}

//implementazione della variante dell'algoritmo di Dijkstra
void dijkstra2(HashTable * graph, int partenzaKm, int arrivoKm){
    //inizializzazione strutture dati

    ListaStazioni* lista = creaLista();
    lista->testa=NULL;
    lista->coda=NULL;
    ottieniStazioniIntermedieOrdinate(lista, partenzaKm, arrivoKm, graph);

    int numStazioni = lista->lunghezza;
    //printf("numStazioni: %d\n", numStazioni);
    NodoList* partenza =lista->testa;

    int distanza = arrivoKm - partenzaKm;

    //vediamo se si puo fare in una mossa
    if(partenza->autonomiaMax >= distanza){
        printf("%d %d\n", partenzaKm, arrivoKm);
        return;
    }

    int* distanze = (int*)malloc( numStazioni* sizeof(int));

    //printf("numStazioni: %d\n", numStazioni);

    //bool* visitati = (bool*)malloc(numStazioni * sizeof(bool));
    StatoStazione* precedenti = (StatoStazione *)malloc(numStazioni * sizeof(StatoStazione));

    //inizializza strutture dati
    for(int i=0; i<numStazioni;i++){
        distanze[i]=-1;
        precedenti[i].km = -1;
        precedenti[i].StazionePrec = -1;
        precedenti[i].numTappe = 0;
    }

    //in distanze inserisco in corrispondenza di i la distanza dall'inizio
    riempiDistanze(lista, arrivoKm, distanze);

    NodoList * tappa = partenza;

    precedenti[0].km = tappa->km;

    if(tappa->autonomiaMax < distanze[1]){
        printf("nessun percorso\n");
        return;
    }


    //ciclo principale di Dijkstra
    for(int i=0; i < numStazioni - 1; i++){

        //printf("stazione presa in considerazione: %d\n", distanze[i] + kmPartenza);
        int numTappe = precedenti[i].numTappe + 1;
        int autonomia = tappa->autonomiaMax;
        //printf("autonomia: %d\n", autonomia);

        for(int j=i+1; j < numStazioni; j++){


            int distanzaStazioni = abs(distanze[i]-distanze[j]);
            int stazioneAttuale = distanze[j] + partenzaKm;

            //vuol dire che abbiamo raggiunto una stazione che non è raggiungibile da altre
            if(i!=0 && numTappe==0) break;

            if(autonomia < distanzaStazioni) break;

            //printf("distanza: %d, numTappe: %d, numTappePrec: %d\n", distanzaStazioni, numTappe,precedenti[j].numTappe);

            if(autonomia >= distanzaStazioni && (numTappe < precedenti[j].numTappe || precedenti[j].numTappe==0)){

                precedenti[j].numTappe = numTappe;
                precedenti[j].km = stazioneAttuale;
                precedenti[j].StazionePrec = tappa->km;
                //printf("tappa attuale: %d\n", precedenti[j].km);
                //printf("tappa prec: %d\n", precedenti[j].StazionePrec);
            }

        }

        tappa = (NodoList *) tappa->next;
    }

    if(precedenti[numStazioni-1].StazionePrec==-1){
        printf("nessun percorso\n");
        return;
    }

    //adesso devo estrarre a ritroso dalla fine tutte le stazioni presenti in StazionePrec per formare il percorso minimo
    ListaStazioni* percorsoMinimo = estraiPercorsoMinimo(precedenti, numStazioni);

    if(percorsoMinimo==NULL){
        printf("nessun percorso\n");
    }else {
        if(percorsoMinimo->testa->km != partenza->km){
            printf("nessun percorso\n");
            liberaLista(percorsoMinimo);
            return;
        }
        stampaLista(percorsoMinimo);
        liberaLista(percorsoMinimo);
    }

    // Libera la memoria delle strutture dati
    free(distanze);
    free(precedenti);
    liberaLista(lista);
}

//implementazione della variante dell'algoritmo di Dijkstra
void dijkstra2Back(HashTable * graph, int partenzaKm, int arrivoKm){
    //inizializzazione strutture dati

    ListaStazioni* lista = creaLista();
    lista->testa=NULL;
    lista->coda=NULL;
    ottieniStazioniIntermedieOrdinate(lista, arrivoKm, partenzaKm, graph);

    NodoList * partenza = lista->coda;

    int numStazioni = lista->lunghezza;
    //printf("numStazioni: %d\n", numStazioni);

    int distanza = abs(arrivoKm - partenzaKm);

    //vediamo se si puo fare in una mossa
    if(partenza->autonomiaMax >= distanza){
        printf("%d %d\n", partenzaKm, arrivoKm);
        return;
    }

    int* distanze = (int*)malloc( numStazioni* sizeof(int));

    //printf("arrivoKm: %d\n", arrivoKm);

    //printf("numStazioni: %d\n", numStazioni);

    //bool* visitati = (bool*)malloc(numStazioni * sizeof(bool));
    StatoStazione* precedenti = (StatoStazione *)malloc(numStazioni * sizeof(StatoStazione));

    //inizializza strutture dati
    for(int i=0; i<numStazioni;i++){
        distanze[i] = -1;
        precedenti[i].km = -1;
        precedenti[i].StazionePrec = -1;
        precedenti[i].numTappe = 0;
    }

    //in distanze inserisco in corrispondenza di i la distanza dall'inizio
    riempiDistanzeBack(lista, arrivoKm, distanze);

    NodoList * tappa = partenza;

    precedenti[0].km = tappa->km;

    if(tappa->autonomiaMax < distanze[1]){
        printf("nessun percorso\n");
        return;
    }

    if(tappa->autonomiaMax >= distanze[numStazioni-1]){
        //printf("autonomia: %d, dist: %d\n", tappa->autonomiaMax, distanze[5]);
        printf("%d %d\n", partenzaKm, arrivoKm);
        return;
    }

    //ciclo principale di Dijkstra
    for(int i=0; i < numStazioni - 1; i++){

        //printf("stazione presa in considerazione: %d\n", distanze[i] + kmPartenza);
        int numTappe = precedenti[i].numTappe + 1;
        int autonomia = tappa->autonomiaMax;
        //printf("autonomia: %d\n", autonomia);

        for(int j=i+1; j < numStazioni; j++){

            int distanzaStazioni = abs(distanze[i]-distanze[j]);
            int stazioneAttuale = abs(partenzaKm - distanze[j]);

            //vuol dire che abbiamo raggiunto una stazione che non è raggiungibile da altre
            if(i!=0 && numTappe==0) break;

            if(autonomia < distanzaStazioni) break;

            //printf("distanza: %d, numTappe: %d, numTappePrec: %d\n", distanzaStazioni, numTappe,precedenti[j].numTappe);

            if(autonomia >= distanzaStazioni && (numTappe < precedenti[j].numTappe || precedenti[j].numTappe==0)){

                precedenti[j].numTappe = numTappe;
                precedenti[j].km = stazioneAttuale;
                precedenti[j].StazionePrec = tappa->km;
                //printf("tappa attuale: %d\n", precedenti[j].km);
                //printf("tappa prec: %d\n", precedenti[j].StazionePrec);
            }

            if(autonomia >= distanzaStazioni && numTappe == precedenti[j].numTappe
               && precedenti[j].StazionePrec > tappa->km){

                //printf("numTappe attuale: %d\n", precedenti[j].numTappe);
                //printf("numTappe da assegnare: %d\n", numTappe);

                precedenti[j].numTappe = numTappe;
                precedenti[j].km = stazioneAttuale;
                precedenti[j].StazionePrec = tappa->km;
                //printf("tappa attuale: %d\n", precedenti[j].km);
                //printf("tappa prec: %d\n", precedenti[j].StazionePrec);
            }

        }

        tappa = (NodoList *) tappa->prev;
    }

    if(precedenti[numStazioni-1].StazionePrec==-1){
        printf("nessun percorso\n");
        return;
    }

    //adesso devo estrarre a ritroso dalla fine tutte le stazioni presenti in StazionePrec per formare il percorso minimo
    ListaStazioni* percorsoMinimo = estraiPercorsoMinimo(precedenti, numStazioni);

    if(percorsoMinimo==NULL){
        printf("nessun percorso\n");
    }else {
        if(percorsoMinimo->testa->km != partenza->km){
            printf("nessun percorso\n");
            liberaLista(percorsoMinimo);
            return;
        }
        stampaLista(percorsoMinimo);
        liberaLista(percorsoMinimo);
    }

    // Libera la memoria delle strutture dati
    free(distanze);
    //free(visitati);
    free(precedenti);
    liberaLista(lista);
}

void aggiungiStazioneOrdinata(ListaStazioni* lista, Stazione* nuovaStazione) {
    NodoList* nuovoNodo = malloc(sizeof(NodoList));
    nuovoNodo->km = nuovaStazione->distanza;
    nuovoNodo->autonomiaMax = nuovaStazione->autonomie[0];

    lista->lunghezza++;

    if (lista->testa == NULL) {
        // Lista vuota, il nuovo nodo diventa la testa e la coda
        nuovoNodo->prev = NULL;
        nuovoNodo->next = NULL;
        lista->testa = nuovoNodo;
        lista->coda = nuovoNodo;
    } else {
        // Inserimento ordinato per distanza
        NodoList* current = lista->testa;
        NodoList* precedente = NULL;

        while (current != NULL && current->km < nuovaStazione->distanza) {
            precedente = current;
            current = (NodoList *) current->next;
        }

        if (precedente == NULL) {
            // Inserimento alla testa della lista
            nuovoNodo->prev = NULL;
            nuovoNodo->next = (struct NodoList *) lista->testa;
            lista->testa->prev = (struct NodoList *) nuovoNodo;
            lista->testa = nuovoNodo;
        } else {
            // Inserimento in mezzo alla lista o alla fine
            nuovoNodo->prev = (struct NodoList *) precedente;
            nuovoNodo->next = (struct NodoList *) current;
            precedente->next = (struct NodoList *) nuovoNodo;
            if (current != NULL) {
                current->prev = (struct NodoList *) nuovoNodo;
            } else {
                // Inserimento in coda
                lista->coda = nuovoNodo;
            }
        }
    }
}


// Funzione per ottenere una lista di stazioni intermedie tra due stazioni dati i loro nodi di partenza e arrivo (ORDINATA per distanza)
void ottieniStazioniIntermedieOrdinate(ListaStazioni* percorso, int partenza, int arrivo, HashTable* autostrada) {

    // Seleziona la distanza di partenza e arrivo
    int distanzaPartenza = partenza;
    int distanzaArrivo = arrivo;

    // Attraversa l'hash table per trovare stazioni intermedie
    for (int i = 0; i < TABLE_SIZE; i++) {
        Nodo* current = autostrada->table[i].head;
        while (current != NULL) {
            int distanzaStazione = current->stazione->distanza;
            if (distanzaStazione >= distanzaPartenza && distanzaStazione <= distanzaArrivo) {
                aggiungiStazioneOrdinata(percorso, current->stazione);
            }
            current = (Nodo *) current->next;
        }
    }

}


int getMaxFromAutonomie(const int *pInt, int remove, int size) {
    int secondMax = 0;

    for(int i=1; i < size + 1 ; i++){

        if(i==remove){
            continue;
        }

        if(pInt[i]> secondMax){
            secondMax = pInt[i];
        }

    }
    //printf("%d\n", secondMax);
    return secondMax;
}

void removeAuto(HashTable *graph,  int toRemove, int distanza) {

    Stazione *stazione = findStation(graph, distanza);

    if(stazione==NULL){
        printf("non rottamata\n");
        return;
    }
    //printf("%d\n", current->stazione->distanza);

    // Se la stazione viene trovata, aggiungi la nuova autonomia
    if (stazione != NULL) {
        int indexToRemove = -1;

        // Cerca l'indice dell'autonomia da rimuovere
        for (int i = 1; i < stazione->nAuto + 1; i++) {
            if (stazione->autonomie[i] == toRemove) {
                indexToRemove = i;
                break;
            }
        }

        //se ho trovato l'indice
        if (indexToRemove != -1) {

            stazione->nAuto--;

            if (stazione->nAuto == 0) {
                stazione->autonomie[0] = 0;
                int *nuoveAutonomie = (int *) realloc(stazione->autonomie, (stazione->nAuto + 1) * sizeof(int));
                stazione->autonomie = nuoveAutonomie;
                printf("rottamata\n");
                return;
            }

            if (stazione->autonomie[0] == stazione->autonomie[indexToRemove]) {
                stazione->autonomie[0] = getMaxFromAutonomie(stazione->autonomie, indexToRemove, stazione->nAuto);
            }

            for (int i = indexToRemove; i < stazione->nAuto + 1; i++) {
                stazione->autonomie[i] = stazione->autonomie[i + 1];
            }

            int *nuoveAutonomie = (int *) realloc(stazione->autonomie, (stazione->nAuto + 1) * sizeof(int));

            if (nuoveAutonomie != NULL || stazione->nAuto != 0) {
                stazione->autonomie = nuoveAutonomie;
            } else {
                printf("Errore di allocazione della memoria.\n");
            }

            printf("rottamata\n");

        } else {
            printf("non rottamata\n");
        }

    } else {
        printf("non rottamata\n");
    }


}

void addAuto(HashTable *graph, int newAutonomia, int distanza) {

    Stazione * stazione = findStation(graph, distanza);

    if(stazione==NULL){
        printf("non aggiunta\n");
        return;
    }

    // Se la stazione viene trovata, aggiungi la nuova autonomia
    if (stazione != NULL && stazione->nAuto < 512) {

        int *nuoveAutonomie = (int *) realloc(stazione->autonomie, (stazione->nAuto + 2) * sizeof(int));
        if (nuoveAutonomie != NULL) {
            stazione->autonomie = nuoveAutonomie;
            //stazione->nAuto++;
            stazione->autonomie[stazione->nAuto + 1] = newAutonomia;
            stazione->nAuto++;

            // se l'autonomia da aggiungere rappresenta il nuovo max lo sostituisco in prima pos.
            if (newAutonomia > stazione->autonomie[0]) stazione->autonomie[0] = newAutonomia;

            printf("aggiunta\n");

        } else {
            printf("Errore di allocazione della memoria.\n");
        }
    } else {
        printf("non aggiunta\n");
    }

}

// Funzione per rimuovere una stazione dalla hash table
int removeStazione(HashTable *table, int distanza) {
    // Calcola l'indice del bucket in cui cercare la stazione
    int indice = hash(distanza);

    Nodo *current = table->table[indice].head;
    Nodo *previous = NULL;

    // Cerca la stazione nel bucket corrispondente
    while (current != NULL) {
        if (current->stazione->distanza == distanza) {
            // Rimuovi la stazione
            if (previous == NULL) {
                // La stazione è la testa del bucket
                table->table[indice].head = (Nodo *) current->next;
            } else {
                previous->next = current->next;
            }

            // Verifica se la stazione rimossa è anche la coda della lista
            if (current == table->table[indice].tail) {
                table->table[indice].tail = previous;
            }

            free(current->stazione->autonomie); // Libera la memoria delle autonomie
            free(current->stazione); // Libera la memoria della stazione
            free(current); // Libera la memoria del nodo

            table->size--; // Decrementa la dimensione della hash table
            return 1; // Stazione rimossa con successo
        }

        previous = current;
        current = (Nodo *) current->next;
    }

    return 0; // Stazione non trovata
}


//funzione per otterene le autonomie delle auto dalla stringa di input
int *getAutonomie(char *input, int numero_auto) {
    int *autonomie = (int *) malloc(sizeof(int) * (numero_auto + 1));
    int indice = 1;
    int maxAutonomia = 0;

    // Ottiene un puntatore al primo valore dopo numero_auto
    char *token = strtok(input, " ");
    for (int i = 0; i < 3; i++) {
        token = strtok(NULL, " ");
    }

    // Salva le autonomie delle auto nell'array autonomie
    while (token != NULL && indice < numero_auto + 1) {
        int autonomia = atoi(token);
        if (autonomia > maxAutonomia) {
            maxAutonomia = autonomia;
        }

        autonomie[indice] = autonomia;
        indice++;
        token = strtok(NULL, " ");
    }

    // Assegna l'autonomia più grande alla posizione autonomie[0]
    autonomie[0] = maxAutonomia;

    return autonomie;
}

//controller
void makeMove(char input[], HashTable *graph) {
    char comando[20];

    if (strstr(input, "aggiungi-stazione") != NULL) {
        int km, nVeicoli;
        int *autonomie;

        // Esegui le istruzioni per aggiungi-stazione
        sscanf(input, "%s %d %d", comando, &km, &nVeicoli);

        /*
        printf("Primo comando: %s\n", comando);
        printf("Km di distanza: %d\n", km);
        printf("Numero di veicoli: %d\n", nVeicoli);*/

        //bisogna memorizzare tutti i dettagli da qualche parte
        autonomie = NULL;
        if (nVeicoli != 0 && nVeicoli <= 512) {
            autonomie = getAutonomie(input, nVeicoli);
        } else if (nVeicoli == 0) {
            autonomie = (int *) malloc(sizeof(int) * (nVeicoli + 1));
            autonomie[0] = 0;
        }

        /*
        for (int i = 0; i < nVeicoli; i++) {
            printf("%d ", autonomie[i]);
            printf("\n");
        }*/

        //bisogna fare il check se esiste già una stazione a questa distanza in km
        if (findStation(graph, km)!=NULL || nVeicoli > 512 || km < 0) {
            printf("non aggiunta\n");

        } else {
            //bisogna creare e aggiungere la stazione (se va a buon fine stampo 'aggiunta' )
            Stazione *stazione = (Stazione *) malloc(sizeof(Stazione));
            stazione->distanza = km;
            stazione->nAuto = nVeicoli;
            stazione->autonomie = autonomie;


            insertStation(graph, stazione);
            printf("aggiunta\n");
        }

    }

    if (strstr(input, "demolisci-stazione") != NULL) {
        int distanza;

        // Esegui le istruzioni per il demolisci-stazione
        sscanf(input, "%s %d", comando, &distanza);

        /*
        printf("Primo comando: %s\n", comando);
        printf("Km di distanza: %d\n", distanza);*/

        int ok = removeStazione(graph, distanza);
        if(ok){
            printf("demolita\n");
        }else{
            printf("non demolita\n");
        }

    }

    if (strstr(input, "aggiungi-auto") != NULL) {
        int distanza, autonomia;

        // Esegui le istruzioni per il aggiungi-auto
        sscanf(input, "%s %d %d", comando, &distanza, &autonomia);

        /*
        printf("Primo comando: %s\n", comando);
        printf("Km di distanza: %d\n", distanza);
        printf("Autonomi auto da aggiungere: %d\n", autonomia);*/


        addAuto(graph, autonomia, distanza);


    }

    if (strstr(input, "rottama-auto") != NULL) {
        int distanza, autonomia;

        // Esegui le istruzioni per il rottama-auto
        sscanf(input, "%s %d %d", comando, &distanza, &autonomia);

        /*
        printf("Primo comando: %s\n", comando);
        printf("Km di distanza: %d\n", distanza);
        printf("Autonomi auto da demolire: %d\n", autonomia);*/

       removeAuto(graph, autonomia, distanza);

    }

    if (strstr(input, "pianifica-percorso") != NULL) {
        int partenza, arrivo;

        // Esegui le istruzioni per il pianifica-percorso
        sscanf(input, "%s %d %d", comando, &partenza, &arrivo);

        /*
        printf("Primo comando: %s\n", comando);
        printf("Stazione di partenza: %d\n", partenza);
        printf("Stazione di arrivo: %d\n", arrivo);*/


        //Nodo *start = getNodeIndex(graph, partenza);
        //Nodo *end = getNodeIndex(graph, arrivo);

        //printf("%d ", start->distanza);
        //printf("%d\n", end->distanza);


        if(partenza == arrivo){
            printf("%d %d\n", partenza, arrivo);
            return;
        }


        //pianifica percorso passandogli le stazioni di partenza e arrivo
        if (arrivo > partenza) {

            dijkstra2(graph,  partenza, arrivo);


        } else {

            dijkstra2Back(graph,  partenza, arrivo);

        }


    }

    if (strstr(input, "stampa") != NULL) {
        stampaStazioni(graph);
    }

    if (strstr(input, "print-autonomia") != NULL) {
        int d;

        sscanf(input, "%s %d", comando, &d);

        Stazione *nodo = findStation(graph, d);
        printf("autonomia max: %d\n", nodo->autonomie[0]);
        for (int i = 1; i < nodo->nAuto + 1; i++) {
            printf("%d\n", nodo->autonomie[i]);
        }
    }

}

int main() {
    char input[20000];

    HashTable *autostrada=(HashTable *)malloc(sizeof(HashTable));
    initHashTable(autostrada);

    while (1) {
        // Esegui le operazioni desiderate con la stringa letta
        if (fgets(input, sizeof(input), stdin) != NULL) {
            // Rimuovi il carattere di newline finale
            input[strcspn(input, "\n")] = '\0';
            // Verifica se la stringa è vuota
            if (strcmp(input, "") == 0) break;

            makeMove(input, autostrada);
            //printf("Stringa letta: %s\n", input);
        } else {
            return 0;
        }
    }

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define rows 5
#define columns 6

/*
	Enumeratore contenente gli stati
	che le bolle possono assumere
*/
enum states
{
	exploded = 0,
	empty,
	half,
	full,
	statesCount
};

/*
	Struttura che definisce le coordinate
	applicate dall'algoritmo quando calcola
	il percorso più efficiente
*/
struct status
{
	int x;
	int y;
};

/*
	Struttura che definisce le coordinate
	sugli assi cartesiani relative alla
	prossima propagazione della bolla
*/
struct deltas
{
	int dx;
	int dy;
};

/*
	Funzione che imposta lo stato iniziale (random)
	e il carattere visuale della bolla ricevuta come
	parametro in input
*/
int getState()
{
	return (enum states)(rand() % statesCount);
}

/*
	Funzione che ritorna il carattere visuale
	in bae allo stato della bolla ricevuto
	come parametro in input
*/
char getVisualForState(enum states state)
{
	switch (state)
	{
		case empty: return ' ';
		case half: return 'O';
		case full: return '0';
		case exploded: return 'X';
	}
}

void cleanInputBuffer()
{
	char c;
	while ((c = getchar()) != '\n' && c != EOF);
}

/*
	Funzione che genera la matrice di gioco
	principale e ne copia il contenuto nella
	matrice usata dal programma
*/
int generateField(int playerField[rows][columns], int computerField[rows][columns])
{
	// Per ogni riga
	for (int r = 0; r < rows; r++)
	{
		// Per ogni colonna
		for (int c = 0; c < columns; c++)
		{
			// Popolo la bolla
			playerField[r][c] = getState();
			//playerField[r][&c] = getState();
		}
	}

	// Copio prima matrice in una seconda matrice
	memcpy(computerField, playerField, sizeof(int) * rows * columns);
}

/*
	Funzione che stampa a video la matrice
	ricevuto come parametro in input
*/
void printPlayerField(int playerField[rows][columns])
{
	// Svuoto la console
	system("cls");

	// Per ogni riga
	for (int r = 0; r < rows; r++)
	{
		// Per ogni colonna
		for (int c = 0; c < columns; c++)
		{
			// Stampo il carattere visuale della bolla
			printf("%c ", getVisualForState(playerField[r][c]));
		}
		printf("\n");
	}
	printf("\n");
}

/*
	Funzione che riceve in input dal giocatore
	le coordinate di una bolla e valorizza di
	conseguenza i puntatori ricevuti in input
	come parametro
*/
void askPlayer(int *x, int *y, int playerField[rows][columns])
{
	do
	{
		*x = -1;
		*y = -1;

		// Finchè la colonna non è valida
		while (*x == -1)
		{
			printf("Seleziona una colonna [1-%d]", columns);
			// Recupero valore colonna in input
			scanf("%d", x);
			cleanInputBuffer();

			// Se colonna non è presente nella matrice
			// TRUE: Invalido l'input del giocatore
			// FALSE: Correggo l'input dell'utente e modifico il valore del puntatore
			if (*x < 1 || *x > columns)
			{
				*x = -1;
				printf("Non e' una colonna valida!\n");
			}
			else
			{
				*x -= 1;
			}
		}

		// Finchè la riga non è valida
		while (*y == -1)
		{
			printf("Seleziona una riga [1-%d]", rows);
			// Recupero valore riga in input
			scanf("%d", y);
			cleanInputBuffer();

			// Se riga non è presente nella matrice
			// TRUE: Invalido l'input del giocatore
			// FALSE: Correggo l'input dell'utente e modifico il valore del puntatore
			if (*y < 1 || *y > rows)
			{
				*y = -1;
				printf("Non e' una riga valida!\n");
			}
			else
			{
				*y -= 1;
			}
		}

		if (playerField[*y][*x] == exploded)
		{
			printf("Non puoi scegliere una bolla gia' esplosa!\n");
		}
	} while (playerField[*y][*x] == exploded);
}

/*
	Funzione che calcola il delta delle
	coordinate cartesiane in relazione alla direzione
	ricevuta come parametro in input
	0: sinistra
	1: su
	2: destra
	3: giù
*/
struct deltas getDeltas(int x, int y, int propagation)
{
	struct deltas ris;
	ris.dx = x + (propagation == 0 ? -1 : propagation == 2 ? 1 : 0);
	ris.dy = y + (propagation == 1 ? -1 : propagation == 3 ? 1 : 0);
	return ris;
}

/*
	Funzione ricorsiva che aggiorna lo stato della
	bolla e se necessario propaga l'evento alle
	bolle adiacenti
*/
void updateNode(int matrix[rows][columns], int x, int y, int propagation)
{
	// Se le coordinate ricevute come parametro in input non sono comprese nella matrice
	// VERO: ritorno senza compiere ulteriori azioni
	if (x < 0 || y < 0 || x >= columns || y >= rows)
	{
		return;
	}

	struct deltas delta;

	// Se la bolla è già esplosa
	if (matrix[y][x] == exploded)
	{
		// Se il valore di propagazione(direzione esplosione) è valido
		if (propagation != -1)
		{
			// Recupero i delta delle coordinate ricevute in input come parametro aggiornate con la direzione
			delta = getDeltas(x, y, propagation);
			// Aggiorno la bolla risultante
			updateNode(matrix, delta.dx, delta.dy, propagation);
		}
	}
	// Altrimenti se la bolla sta per esplodere 
	else if (matrix[y][x] == full)
	{
		// Aggiorno lo stato ad esplosa
		matrix[y][x] = exploded;

		// Per ogni possibile direzione cartesiana
		for (int i = 0; i < 4; i++)
		{
			// Recupero i delta delle coordinate ricevute in input come parametro aggiornate con la direzione corrente
			delta = getDeltas(x, y, i);
			// Aggiorno la bolla risultante
			updateNode(matrix, delta.dx, delta.dy, i);
		}
	}
	else
	{
		// Altrimenti, incremento lo stato della bolla
		matrix[y][x] += 1;
	}

	// Aggiorno il valore visuale della bolla appena modificata
	//matrix[y][x].visual = getVisualForState(matrix[y][x].state);
}

/*
	Funzione che salva in memoria la matrice
	ricevuta come parametro in input
*/
char* saveMatrix(int matrix[rows][columns])
{
	int dim = sizeof(int) * rows * columns;
	// Alloco in memoria e ottengo l'indirizzo relativo
	char* ris = malloc(dim);
	// Copio il valore della matrice nell'indirizzo appena occupato
	memcpy(ris, matrix, dim);
	return ris;
}

/*
	Funzione che recupera da un buffer in ingresso
	il valore della matrice presente
*/
void restoreMatrix(int matrix[rows][columns], char* buf)
{
	int dim = sizeof(int) * rows * columns;
	// Copio il valore della matrice contenuta nell'indirizzo del buffer sulla matrice ricevuta come parametro in input
	memcpy(matrix, buf, dim);
	// Dealloco l'indirizzo che occupava il buffer
	free(buf);
}

/*
	Funzione che controlla se il gioco è
	stato completato
*/
int isCompleted(int matrix[rows][columns])
{
	//if (numMoves >= minMoves) gridCompleted = lost;
	// Per ogni riga
	for (int r = 0; r < rows; r++)
	{
		// Per ogni colonna
		for (int c = 0; c < columns; c++)
		{
			// Se la bolla corrente ha stato diverso da esplosa
			if (matrix[r][c] != exploded)
			{
				// Il gioco non è stato completato
				return 0;
			}
		}
	}

	// Il gioco è stato completato
	//gridCompleted = win;
	return 1;
}

/*
	Funzione ricorsiva che valuta tutte le possibili scelte
	che si possono fare e ne deriva il miglior percorso
	per vincere la partita
*/
int tree(FILE *stream, int *numMoves, int *minMoves, int x, int y, int computerField[rows][columns], struct status statusStack[1000])
{
	// Se il numero di mosse è maggiore di quello minimo già trovato oppure la bolla corrente è già esplosa
	if (*numMoves >= *minMoves - 1 || computerField[y][x] == exploded)
	{
		// Ritorno, evitando azioni inutili
		return;
	}

	// Salvo lo stato corrente della matrice in memoria
	char* stack = saveMatrix(computerField);

	// Aggiorno lo stack delle mosse (rispetto alla mossa corrente) con le coordinate della bolla corrente
	statusStack[*numMoves].x = x;
	statusStack[*numMoves].y = y;

	// Incremento il numero di mosse compiute
	*numMoves = *numMoves + 1;

	// Aggiorno lo stato della bolla corrente
	updateNode(computerField, x, y, -1);

	// Se la griglia presenta solo bolle esplose (gioco completato)
	//if(gridCompleted == win)
	if(isCompleted(computerField))
	{ 
		// Se il numero di mosse minimo già trovato è maggiore del numero di mosse compiute
		if (*minMoves > *numMoves)
		{
			// Aggiorno il valore minimo con il numero corrente di mosse
			*minMoves = *numMoves;
		}

		fprintf(stream, "Finito ramo con %d mosse totali:\n", *numMoves);

		for (int i = 0; i < *numMoves; i++)
		{
			fprintf(stream, "%d: (%d, %d)\n", i+1, statusStack[i].x+1, statusStack[i].y+1);
		}
		fprintf(stream, "\n");

		// Decremento il numero di mosse
		numMoves--;
		// Ripristino la matrice allo stato precedente che avevo salvato in memoria
		restoreMatrix(computerField, stack);

		return;
	}

	// Per ogni riga
	for (int r = 0; r < rows; r++)
	{	
		// Per ogni colonna
		for (int c = 0; c < columns; c++)
		{
			// Calcolo il percorso migliore
			tree(stream, numMoves, minMoves, c, r, computerField, statusStack);
		}
	}
	
	// Decremento il numero di mosse
	*numMoves = *numMoves - 1;
	// Ripristino la matrice allo stato precedente che avevo salvato in memoria
	restoreMatrix(computerField, stack);
}

/*
	Funzione che si occupa del calcolo delle mosse
	minime necessarie a completare la griglia
	generata all'inzio della partita
*/
int calcMinMoves(int computerField[rows][columns])
{
	int minMoves = INT_MAX;
	// Apro stream file
	FILE *stream = fopen("moves.txt", "w");

	// Se lo stream non può essere aperto
	if (stream == NULL)
	{
		printf("Impossibile aprire il file %s\n", "moves.txt");
		// Attendo un input dell'utente
		system("pause");
		// Termino il programma con un codice di errore
		exit(1);
	}

	fprintf(stream, "Algoritmo per il calcolo del numero minimo di mosse necessario a completare la seguente griglia:\n\n");

	for (int c = 0; c < columns; c++)
	{
		if (c == 0) fprintf(stream, "   ");
		fprintf(stream, "%d ", c + 1);
	}
	fprintf(stream, "\n");

	for (int r = 0; r < rows; r++)
	{
		fprintf(stream, "%d  ", r + 1);

		for (int c = 0; c < columns; c++)
		{
			
			fprintf(stream, "%c ", getVisualForState((enum states)computerField[r][c]));
		}
		fprintf(stream, "\n");
	}
	fprintf(stream, "\n");

	struct status statusStack[1000];
	// Per ogni riga
	for (int r = 0; r < rows; r++)
	{
		// Per ogni colonna
		for (int c = 0; c < columns; c++)
		{
			fprintf(stream, "-------------------------------\n\nCalcolo rami possibili partendo da (%d, %d)\n\n", c + 1, r + 1);
			int numMoves = 0;
			// Calcolo il ramo migliore relativo alla bolla di partenza corrente
			tree(stream, &numMoves, &minMoves, c, r, computerField, &statusStack);
		}
	}

	fprintf(stream, "===============================\n\nIl ramo più efficiente impiega solamente %d mosse per completare la griglia.", minMoves);

	fflush(stream);
	fclose(stream);

	return minMoves;
}

/*
	Funzione principale del programma
*/
int main()
{
	// Inizializzo funzione rand() con il seed
 	srand(time(0));
	// Dichiaro variabili utili al gioco
	int numMoves = 0,	// Numero corrente di mosse del giocatore
		status = 0,		// Boolean per verificare se il giocatore ha vinto o perso
		ended = 0;		// Boolean per verificare quando il gioco è concluso
	
	int playerField[rows][columns];		// Matrice contenente le bolle del campo di gioco per il giocatore
	int computerField[rows][columns];	// Matrice contenente le bolle del campo di gioco usato per calcolare il numero minimo di mosse

	// Genero la matrice del giocatore e ne copio il valore all'iterno della matrice usata dall'algoritmo
	generateField(playerField, computerField);
	// Calcolo le mosse minime
	int minMoves = calcMinMoves(computerField);
	
	// Finchè il gioco non è completato
	while (ended == 0)
	{
		// Stampo la matrice allo stato corrente
		printPlayerField(&playerField);

		printf("Mosse rimanenti: %d\n\n", minMoves - numMoves);

		int x = -1, y = -1;
		// Recupero da input del giocatore le coordinate cartesiane
		askPlayer(&x, &y, &playerField);

		// Aggiorno la bolla relativa alle coordinate memorizzate precedentemente
		updateNode(playerField, x, y, -1);

		// Aumento il numero di mosse
		numMoves++;

		// Controllo se la griglia è stata completata
		if (isCompleted(playerField))
		{
			ended = 1;
			status = 1;
		}
		else if (numMoves >= minMoves)
		{
			ended = 1;
		}
	}

	// Stampo la matrice allo stato finale
	printPlayerField(playerField);

	// Se l'esito è valido stampo la vincita
	// Altrimenti la sconfitta
	printf("\n%s\n", (status ? "HAI VINTO" : "HAI PERSO"));
	// Attendo un input dell'utente
	system("pause");
	// Termino il programma
	exit(0);
}
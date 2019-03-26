#include <stdio.h>
#include <stdlib.h>
#include "BubbleBlast.h"
#include <time.h>

int generateRandBubbleState()
{
	return (enum BubbleStates)(rand() % bubbleStatesCount);
}

char getVisualCharForBubbleState(enum BubbleStates bubbleState)
{
	switch (bubbleState)
	{
		case stateEmpty: return ' ';
		case stateHalf: return 'O';
		case stateFull: return '0';
		case stateExploded: return 'X';
	}
}

void cleanInputBuffer()
{
	char c;
	while ((c = getchar()) != '\n' && c != EOF);
}

void avoidProgramExit()
{
	system("pause");
	exit(-1);
}

int generateGameField(int gameField[FIELD_ROWS][FIELD_COLUMNS])
{
	for (int r = 0; r < FIELD_ROWS; r++)
	{
		for (int c = 0; c < FIELD_COLUMNS; c++)
		{
			gameField[r][c] = generateRandBubbleState();
		}
	}
}

void printGameFieldToPlayer(int gameField[FIELD_ROWS][FIELD_COLUMNS])
{
	system("cls");

	for (int c = 0; c < FIELD_COLUMNS; c++)
	{
		if (c == 0) printf("  ");
		printf("%d ", c + 1);
	}
	printf("\n");

	for (int r = 0; r < FIELD_ROWS; r++)
	{
		printf("%d ", r + 1);

		for (int c = 0; c < FIELD_COLUMNS; c++)
		{
			printf("%c ", getVisualCharForBubbleState(gameField[r][c]));
		}
		printf("\n");
	}
	printf("\n");
}

void askPlayerNextMoveCoords(int *x, int *y, int gameField[FIELD_ROWS][FIELD_COLUMNS])
{
	int xTemp = *x;
	int yTemp = *y;

	do
	{
		while (yTemp == -1)
		{
			printf("Seleziona una riga [1-%d]", FIELD_ROWS);
			scanf("%d", &yTemp);
			cleanInputBuffer();

			if (yTemp < 1 || yTemp > FIELD_ROWS)
			{
				yTemp = -1;
				printf("Non e' una riga valida!\n");
			}
			else
			{
				yTemp--;
			}
		}

		while (xTemp == -1)
		{
			printf("Seleziona una colonna [1-%d]", FIELD_COLUMNS);
			scanf("%d", &xTemp);
			cleanInputBuffer();

			if (xTemp < 1 || xTemp > FIELD_COLUMNS)
			{
				xTemp = -1;
				printf("Non e' una colonna valida!\n");
			}
			else
			{
				xTemp--;
			}
		}

		if (gameField[yTemp][xTemp] == stateExploded)
		{
			xTemp = -1;
			yTemp = -1;
			printf("Non puoi scegliere una bolla gia' esplosa!\n");
		}
	} while (gameField[yTemp][xTemp] == stateExploded);

	*x = xTemp;
	*y = yTemp;
}

struct BubbleExplosionDeltas calcBubbleExplosionDeltas(int x, int y, int explosionPropagationDirection)
{
	struct BubbleExplosionDeltas deltas;
	deltas.dx = x + (explosionPropagationDirection == directionLeft ? -1 : explosionPropagationDirection == directionRight ? 1 : 0);
	deltas.dy = y + (explosionPropagationDirection == directionUp ? -1 : explosionPropagationDirection == directionDown ? 1 : 0);
	return deltas;
}

void updateBubbleState(int gameField[FIELD_ROWS][FIELD_COLUMNS], int x, int y, int explosionPropagationDirection)
{
	if (x < 0 || y < 0 || x >= FIELD_COLUMNS || y >= FIELD_ROWS)
	{
		return;
	}

	struct BubbleExplosionDeltas deltas;

	if (gameField[y][x] == stateExploded)
	{
		if (explosionPropagationDirection != -1)
		{
			deltas = calcBubbleExplosionDeltas(x, y, explosionPropagationDirection);
			updateBubbleState(gameField, deltas.dx, deltas.dy, explosionPropagationDirection);
		}
	}
	else if (gameField[y][x] == stateFull)
	{
		gameField[y][x] = stateExploded;

		for (int i = 0; i < 4; i++)
		{
			enum BubbleExplosionDirectionPropagation directionPropagation = (enum BubbleExplosionDirectionPropagation)i;
			deltas = calcBubbleExplosionDeltas(x, y, directionPropagation);
			updateBubbleState(gameField, deltas.dx, deltas.dy, directionPropagation);
		}
	}
	else
	{
		gameField[y][x] += 1;
	}
}

char* saveCalcMovesStatus(int movesField[FIELD_ROWS][FIELD_COLUMNS])
{
	int dim = sizeof(int) * FIELD_ROWS * FIELD_COLUMNS;
	char* ris = malloc(dim);
	memcpy(ris, movesField, dim);
	return ris;
}

void restoreCalcMovesStatus(int movesField[FIELD_ROWS][FIELD_COLUMNS], char* buffer)
{
	int dim = sizeof(int) * FIELD_ROWS * FIELD_COLUMNS;
	memcpy(movesField, buffer, dim);
	free(buffer);
}

int isGameCompleted(int gameField[FIELD_ROWS][FIELD_COLUMNS])
{
	for (int r = 0; r < FIELD_ROWS; r++)
	{
		for (int c = 0; c < FIELD_COLUMNS; c++)
		{
			if (gameField[r][c] != stateExploded)
			{
				return statusPlaying;
			}
		}
	}

	return statusWon;
}

int recursiveBestPathTree(FILE *stream, int *numMoves, int *minMoves, int x, int y, int gameField[FIELD_ROWS][FIELD_COLUMNS], struct MovesStackStatus movesStatusStack[1000])
{
	if (*numMoves >= *minMoves - 1 || gameField[y][x] == stateExploded)
	{
		return;
	}

	char* movesStatusRef = saveCalcMovesStatus(gameField);
	movesStatusStack[*numMoves].x = x;
	movesStatusStack[*numMoves].y = y;

	*numMoves += 1;

	updateBubbleState(gameField, x, y, noDirection);

	if(isGameCompleted(gameField))
	{ 
		if (*minMoves > *numMoves)
		{
			*minMoves = *numMoves;
		}

		fprintf(stream, "Finito ramo con %d mosse totali:\n", *numMoves);

		for (int i = 0; i < *numMoves; i++)
		{
			fprintf(stream, "%d: [%d][%d]\n", i + 1, movesStatusStack[i].y + 1, movesStatusStack[i].x + 1);
		}
		fprintf(stream, "\n");

		numMoves--;
		restoreCalcMovesStatus(gameField, movesStatusRef);

		return;
	}

	for (int r = 0; r < FIELD_ROWS; r++)
	{	
		for (int c = 0; c < FIELD_COLUMNS; c++)
		{
			recursiveBestPathTree(stream, numMoves, minMoves, c, r, gameField, movesStatusStack);
		}
	}
	
	*numMoves -= 1;
	restoreCalcMovesStatus(gameField, movesStatusRef);
}

int calcMinMovesForGameField(int gameField[FIELD_ROWS][FIELD_COLUMNS])
{
	int minMoves = INT_MAX;
	FILE *logStream = fopen("moves.txt", "w");

	int* placeholderFieldRef = malloc(sizeof(int) * FIELD_ROWS * FIELD_COLUMNS);
	memcpy(placeholderFieldRef, gameField, sizeof(int) * FIELD_ROWS * FIELD_COLUMNS);

	if (logStream == NULL)
	{
		printf("Impossibile aprire il file %s\n", "moves.txt");
		avoidProgramExit();
	}

	fprintf(logStream, "Algoritmo per il calcolo del numero minimo di mosse necessario a completare la seguente griglia:\n\n");

	for (int c = 0; c < FIELD_COLUMNS; c++)
	{
		if (c == 0) fprintf(logStream, "   ");
		fprintf(logStream, "%d ", c + 1);
	}
	fprintf(logStream, "\n");

	for (int r = 0; r < FIELD_ROWS; r++)
	{
		fprintf(logStream, "%d  ", r + 1);

		for (int c = 0; c < FIELD_COLUMNS; c++)
		{
			
			fprintf(logStream, "%c ", getVisualCharForBubbleState((enum BubbleStates)*(placeholderFieldRef + (r * FIELD_ROWS) + c)));
		}
		fprintf(logStream, "\n");
	}
	fprintf(logStream, "\n");

	struct MovesStackStatus movesStatusStack[1000];
	for (int r = 0; r < FIELD_ROWS; r++)
	{
		for (int c = 0; c < FIELD_COLUMNS; c++)
		{
			fprintf(logStream, "-------------------------------\n\nCalcolo rami possibili partendo da [%d][%d]\n\n", r + 1, c + 1);
			int numMoves = 0;
			recursiveBestPathTree(logStream, &numMoves, &minMoves, c, r, placeholderFieldRef, &movesStatusStack);
		}
	}

	fprintf(logStream, "===============================\n\nIl ramo più efficiente impiega solamente %d mosse per completare la griglia.", minMoves);

	fflush(logStream);
	fclose(logStream);
	free(placeholderFieldRef);

	return minMoves;
}

int main()
{
 	srand(time(0));
	int numMoves = 0,
		status = 0;
	int gameField[FIELD_ROWS][FIELD_COLUMNS];

	generateGameField(gameField);
	int minMoves = calcMinMovesForGameField(gameField);

	while (status == 0)
	{
		printGameFieldToPlayer(gameField);

		printf("Mosse rimanenti: %d\n\n", minMoves - numMoves);

		int x = -1, y = -1;
		askPlayerNextMoveCoords(&x, &y, gameField);

		updateBubbleState(gameField, x, y, noDirection);
		numMoves++;

		status = isGameCompleted(gameField);
		if (status == 0 && numMoves >= minMoves)
		{
			status = statusLost;
		}
	}

	printGameFieldToPlayer(gameField);
	printf("\n%s\n", (status == 1 ? "HAI VINTO" : "HAI PERSO"));
	avoidProgramExit();
}
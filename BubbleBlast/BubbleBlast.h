#ifndef BUBBLE_BLAST_HEADER

#define BUBBLE_BLAST_HEADER
#define FIELD_ROWS 5
#define FIELD_COLUMNS 6

enum GameStatus
{
	statusLost = -1,
	statusPlaying = 0,
	statusWon = 1
};

enum BubbleStates
{
	stateExploded = 0,
	stateEmpty,
	stateHalf,
	stateFull,
	bubbleStatesCount
};

enum BubbleExplosionDirectionPropagation
{
	noDirection = -1,
	directionLeft = 0,
	directionUp,
	directionRight,
	directionDown
};

struct MovesStackStatus
{
	int x;
	int y;
};

struct BubbleExplosionDeltas
{
	int dx;
	int dy;
};

#endif
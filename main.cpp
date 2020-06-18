/*
	Refer to this layout guide for the Grid when tracing through generateGrid().

	Grid layout:
		top-Left	top-Mid		top-Right

		mid-Left	mid-Mid		mid-Right

		bot-Left	bot-Mid		bot-Right

	The squares from mid-Left onwards will not be specifically generated, but will
	be filled in iteratively by using the following as a reference(SQ = Square):

	- top-Left (SQ)
	- top-Mid (SQ)
	- top-Right (SQ)
	- first column of mid-Left
	- first column of bot-Left

	This will be done by using the solveSudoku() function.

	Credits for grid-filling optimizations:
	https://dlbeer.co.nz/articles/sudoku.html
*/

#include <vector>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define GRID_SIZE 9

using namespace std;

struct Vector2
{
	int x;
	int y;
};

//	make the grid a global variable
int grid[GRID_SIZE][GRID_SIZE] = {
	{0, 0, 0,	0, 0, 0,	0, 0, 0} ,
	{0, 0, 0,	0, 0, 0,	0, 0, 0} ,
	{0, 0, 0,	0, 0, 0,	0, 0, 0} ,

	{0, 0, 0,	0, 0, 0,	0, 0, 0} ,
	{0, 0, 0,	0, 0, 0,	0, 0, 0} ,
	{0, 0, 0,	0, 0, 0,	0, 0, 0} ,

	{0, 0, 0,	0, 0, 0,	0, 0, 0} ,
	{0, 0, 0,	0, 0, 0,	0, 0, 0} ,
	{0, 0, 0,	0, 0, 0,	0, 0, 0}
};

vector<int> values{ 1, 2, 3, 4, 5, 6, 7, 8, 9 }; //	array of all possible integers

// functions related to the solver
void solveSudoku(vector<Vector2> emptyPositions, unsigned int emptyCount);
vector<Vector2> returnEmptyPositions();
bool checkSquare(Vector2 elementPosition);
bool checkRow(Vector2 elementPosition);
bool checkColumn(Vector2 elementPosition);

// functions related to the grid generator
void generateGrid();
int addIntegerToGrid(vector<int> integers, Vector2 position);
void generateTopLeft(vector<int> integers);
vector<int> filterRow(vector<int> integers, int times, int row, int exception);
vector<int> filterColumn(vector<int> integers, int times, int column, int exception);
vector<int> filterUsedValues(vector<int> usedValues, vector<int> values);
vector<Vector2> generateDefaultEmpties();

// functions related to puzzle generator
vector<Vector2> generatePuzzle();
vector<Vector2> getAllPositions();
vector<Vector2> shufflePositions(vector<Vector2> allCellPositions);
bool confirmPosition(Vector2 pos, vector<Vector2> emptyPositions);
vector<int> filterSquare(vector<int> integers, Vector2 pos);

void outputGrid();

// displays the Grid and each cell with their respective elements in the console
void outputGrid()
{
	cout << "\n";

	int offset{ 0 };
	bool offsetting;

	for (int i = 0; i <= GRID_SIZE + 1; i++)
	{

		if (i == 3 || i == 7)
		{
			offset += 1;
			offsetting = true;
		}
		else
		{
			offsetting = false;
		}

		for (int j = 0; j <= GRID_SIZE - 1; j++)
		{
			if (j % 3 == 0 && j != 0) cout << "|";

			if (offsetting)
			{
				cout << "---";
			}
			else
			{
				cout << " " << grid[i - offset][j] << " ";
			}

			if (j == GRID_SIZE - 1) cout << "\n";
		}
	}
}

//	returns a list of all empty cells and their coordinates
vector<Vector2> returnEmptyPositions()
{
	vector<Vector2> positions;

	for (int i = 0; i <= GRID_SIZE - 1; i++)
	{
		for (int j = 0; j <= GRID_SIZE - 1; j++)
		{
			if (grid[i][j] == 0)
			{
				positions.push_back(Vector2{ i, j });
			}
		}
	}

	return positions;
}

//	for the sake of CheckSquare; 
//	the coordinate of the top left cell in the respective Square is needed
Vector2 returnTopLeftSquare(Vector2 emptyPosition)
{
	Vector2 topLeftPosition{ 0, 0 };

	if (emptyPosition.x <= 2) topLeftPosition.x = 0;
	else if (emptyPosition.x <= 5) topLeftPosition.x = 3;
	else topLeftPosition.x = 6;

	if (emptyPosition.y <= 2) topLeftPosition.y = 0;
	else if (emptyPosition.y <= 5) topLeftPosition.y = 3;
	else topLeftPosition.y = 6;

	return topLeftPosition;
}

//	performs a check to see if a same value exists in the Square the element exists in
bool checkSquare(Vector2 elementPosition)
{
	Vector2 topLeftPosition{ returnTopLeftSquare(elementPosition) };

	for (int i = topLeftPosition.x; i <= topLeftPosition.x + 2; i++)
	{
		for (int j = topLeftPosition.y; j <= topLeftPosition.y + 2; j++)
		{

			if (grid[i][j] == grid[elementPosition.x][elementPosition.y]
				&& !(i == elementPosition.x && j == elementPosition.y))
			{
				return false;
			}

		}
	}

	return true;
}

//	performs a check to see if a same value exists on the same row
bool checkRow(Vector2 elementPosition)
{
	for (int i = 0; i <= GRID_SIZE - 1; i++)
	{
		if (grid[elementPosition.x][i] == grid[elementPosition.x][elementPosition.y]
			&& i != elementPosition.y)
		{
			return false;
		}
	}
	return true;
}

//	performs a check to see if a same value exists on the same column
bool checkColumn(Vector2 elementPosition)
{
	for (int i = 0; i <= GRID_SIZE - 1; i++)
	{
		if (grid[i][elementPosition.y] == grid[elementPosition.x][elementPosition.y]
			&& i != elementPosition.x)
		{
			return false;
		}
	}
	return true;
}

/*	performs the solving of the Sudoku by looping through all empty positions
and applying the Check-xyz() methods to each cell									*/
void solveSudoku(vector<Vector2> emptyPositions, unsigned int emptyCount)
{
	for (int i = 0; i < emptyCount; i++)
	{
		if (grid[emptyPositions[i].x][emptyPositions[i].y] == 0
			|| grid[emptyPositions[i].x][emptyPositions[i].y] <= 9)
		{
			grid[emptyPositions[i].x][emptyPositions[i].y] += 1;

			bool failed{ false };

			// if the maximum limit has been reached, go back to previous cell
			if (grid[emptyPositions[i].x][emptyPositions[i].y] == 10)
			{
				grid[emptyPositions[i].x][emptyPositions[i].y] = 0;
				i -= 2;
				failed = true;
			}

			if (!failed && !checkRow(emptyPositions[i]))
			{
				i -= 1;
				failed = true;
			}

			if (!failed && !checkColumn(emptyPositions[i]))
			{
				i -= 1;
				failed = true;
			}

			if (!failed && !checkSquare(emptyPositions[i]))
			{
				i -= 1;
				failed = true;
			}
		}
	}
}

int addIntegerToGrid(vector<int> integers, Vector2 position)
{
	unsigned int number = 0 + (rand() % integers.size());

	grid[position.x][position.y] = integers[number];

	return integers[number];
}

/*	For the sake of optimization, the top left square of the Sudoku is generated
	at random, as no "Sudoku rules" need to be applied yet							*/
void generateTopLeft(vector<int> integers)
{
	for (int i = 0; i <= 2; i++)
	{
		for (int j = 0; j <= 2; j++)
		{
			//	takes random index between the current available ints (betweeen 0-9)
			unsigned int index = 0 + (rand() % integers.size()); 

			grid[i][j] = integers[index];
			integers.erase(integers.begin() + index); //	removes chosen int
		}
	}
}

//	filters out values present in the same row
vector<int> filterRow(vector<int> integers, int times, int row, int exception)
{
	int counter{ 0 };

	while (counter <= times)
	{
		for (int j = counter * 3; j <= counter * 3 + 2; j++)
		{
			if (count(integers.begin(), integers.end(), grid[row][j]) && j != exception)
			{
				integers.erase(remove(integers.begin(), integers.end(), grid[row][j]), integers.end());
			}
		}

		counter += 1;
	}

	return integers;
}

//	filters out values present in the same column
vector<int> filterColumn(vector<int> integers, int times, int column, int exception)
{
	int counter{ 0 };

	while (counter <= times)
	{
		for (int j = counter * 3; j <= counter * 3 + 2; j++)
		{
			if (count(integers.begin(), integers.end(), grid[j][column]) && j != exception)
			{
				integers.erase(remove(integers.begin(), integers.end(), grid[j][column]), integers.end());
			}
		}

		counter += 1;
	}

	return integers;
}

//	filters out already used values in a square (during grid generation)
vector<int> filterUsedValues(vector<int> usedValues, vector<int> values)
{
	for (int i = 0; i <= usedValues.size() - 1; i++)
	{
		if (count(values.begin(), values.end(), usedValues[i]))
			values.erase(remove(values.begin(), values.end(), usedValues[i]), values.end());
	}

	return values;
}

/*	returns a vector containing the coordinates of the remaining empty cells
	(used by generateGrid)															*/
vector<Vector2> generateDefaultEmpties()
{
	vector<Vector2> positions{ };
	for (int i = 3; i <= 8; i++)
	{
		for (int j = 1; j <= 8; j++)
		{
			positions.push_back(Vector2{ i, j });
		}
	}

	return positions;
}

//	creates a grid to act as a base for a Sudoku puzzle
void generateGrid()
{
	generateTopLeft(values); //	generates top-Left

	vector<int> usedValues{ };

	// generates top-Mid
	for (int i = 0; i <= 2; i++)
	{
		vector<int> potentials{ }; // stores all the potential values for a cell
		int chosenValue{ };

		for (int j = 3; j <= 5; j++)
		{

			potentials = filterRow(values, 0, i, j);

			if (usedValues.size() > 0)
				potentials = filterUsedValues(usedValues, potentials);

			if (potentials.size() == 0)
			{
				i = -1;
				usedValues = { };
			}
			else
			{
				// chooses int from potentials and places it in its respective cell
				chosenValue = addIntegerToGrid(potentials, Vector2{ i, j }); 

				// removes the chosenValue from the list of potentials
				potentials.erase(remove(potentials.begin(), potentials.end(), chosenValue), potentials.end());
				usedValues.push_back(chosenValue);
			}
		}
	}

	usedValues = { };

	// generates top-Right
	for (int i = 0; i <= 2; i++)
	{
		vector<int> potentials{ };
		int chosenValue{ };

		for (int j = 6; j <= 8; j++)
		{
			potentials = filterRow(values, 1, i, j);

			if (usedValues.size() > 0)
				potentials = filterUsedValues(usedValues, potentials);

			if (potentials.size() == 0)
			{
				i = -1;
				usedValues = { };
			}
			else
			{
				chosenValue = addIntegerToGrid(potentials, Vector2{ i, j });

				potentials.erase(remove(potentials.begin(), potentials.end(), chosenValue), potentials.end());
				usedValues.push_back(chosenValue);
			}
		}
	}

	usedValues = { };

	// generates the entire first column
	for (int i = 3; i <= 8; i++)
	{
		vector<int> potentials{ };
		int chosenValue{ };

		potentials = filterColumn(values, 0, 0, i);

		if (usedValues.size() > 0)
			potentials = filterUsedValues(usedValues, potentials);

		if (potentials.size() == 0)
		{
			i = 2;
			usedValues = { };
		}
		else 
		{
			chosenValue = addIntegerToGrid(potentials, Vector2{ i, 0 });

			potentials.erase(remove(potentials.begin(), potentials.end(), chosenValue), potentials.end());
			usedValues.push_back(chosenValue);
		}
	}

	/*	
		After filling all of the "top" Squares, the first column can be fully
		filled with the remaining 6 values. Rest of the empty cells need to be
		iteratively filled in, and this has been done by utilising solveSudoku(),
		which was originally made to solve a given Sudoku. Which, it technically
		is doing when generating a grid.
	*/ 

	solveSudoku(generateDefaultEmpties(), 48);
}

//	returns all positions in a grid from {0, 0} to {8, 8}
vector<Vector2> getAllPositions()
{
	vector<Vector2> positions{ };

	for (int i = 0; i <= 8; i++)
	{
		for (int j = 0; j <= 8; j++)
		{
			positions.push_back(Vector2{ i, j });
		}
	}

	return positions;
}

//	shuffles around all the positions obtained from getAllPositions()
vector<Vector2> shufflePositions(vector<Vector2> allCellPositions)
{
	int size = allCellPositions.size();

	for (int i = 0; i < (size - 1); i++)
	{
		int index = i + rand() % (size - i);
		if (index > (size - 1)) index = size - 1;
		swap(allCellPositions[i], allCellPositions[index]);
	}

	return allCellPositions;
}

//	filters out values present in the square (during puzzle generation)
vector<int> filterSquare(vector<int> integers, Vector2 pos)
{
	Vector2 topLeftPosition{ returnTopLeftSquare(pos) };

	for (int i = topLeftPosition.x; i <= topLeftPosition.x + 2; i++)
	{
		for (int j = topLeftPosition.y; j <= topLeftPosition.y + 2; j++)
		{
			if (count(integers.begin(), integers.end(), grid[i][j]))
			{
				integers.erase(remove(integers.begin(), integers.end(), grid[i][j]), integers.end());
			}
		}
	}

	return integers;
}


/*
	Checks if a given cell-position has a unique value by applying all the
	filterXYZ() functions to that position. If the final size of Solutions is
	1, then the position has a unique solution, hence can be used for the
	puzzle.
*/
bool confirmPosition(Vector2 pos, vector<Vector2> emptyPositions)
{
	bool valid{ true };

	int value = grid[pos.x][pos.y]; //	store value incase cell position isn't valid

	grid[pos.x][pos.y] = 0; 
	
	vector<int> solutions = filterRow(values, 2, pos.x, pos.y);

	solutions = filterColumn(solutions, 2, pos.x, pos.y);

	solutions = filterSquare(solutions, pos);

	if (solutions.size() != 1)
	{
		valid = false;
		grid[pos.x][pos.y] = value;
	}

	return valid;
}

//	creates a puzzle for the user and solver to work on
vector<Vector2> generatePuzzle()
{
	//	stores all grid positions, shuffled
	vector<Vector2> allPositions{ shufflePositions(getAllPositions()) };

	vector<Vector2> emptyPositions{ };

	for (int i = 0; i <= allPositions.size() - 1; i++)
	{
		// if position has unique solution
		if (confirmPosition(allPositions[i], emptyPositions)) 
		{
			emptyPositions.push_back(allPositions[i]);
		}
	}

	return emptyPositions;
}

int main()
{
	srand(unsigned(time(0)));

	generateGrid();	//	generates a grid to make a puzzle on

	outputGrid();

	//	while generating puzzle, stores the empty positions for Solver to use
	vector<Vector2> emptyPositions{ };

	while (emptyPositions.size() < 34) // only accept puzzles with < 46 clues
	{
		emptyPositions = generatePuzzle();
	}

	outputGrid();

	//solveSudoku(emptyPositions, emptyPositions.size());

	cout << "\n";
	cout << "Number of empty positions: " << emptyPositions.size() << "\n";

	//outputGrid();

	return 0;
}
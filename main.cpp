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

vector<Vector2> returnEmptyPositions();
bool checkSquare(Vector2 elementPosition);
bool checkRow(Vector2 elementPosition);
bool checkColumn(Vector2 elementPosition);
void solveSudoku(vector<Vector2> emptyPositions, unsigned int emptyCount);

void generateGrid();
void generateTopLeft(vector<int> integers);
vector<int> generateTopMiddle(vector<int> integers, int row);
vector<int> generateMiddleLeft(vector<int> integers, int column);
void generateTopRight(vector<int> integers);
vector<int> removePreviousRow(vector<int> integers, int row);
vector<int> removePreviousColumn(vector<int> integers, int column);
vector<int> removeTopRowValues(vector <int> usedIntegers, vector<int> integers);

void outputGrid();

// displays the Grid and each cell with their respective elements in the console
void outputGrid()
{
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

//	performs the solving of the Sudoku by looping through all empty positions and applying the Check-xyz() methods to each cell
void solveSudoku(vector<Vector2> emptyPositions, unsigned int emptyCount)
{
	for (int i = 0; i < emptyCount; i++)
	{
		if (grid[emptyPositions[i].x][emptyPositions[i].y] == 0
			|| grid[emptyPositions[i].x][emptyPositions[i].y] <= 9)
		{
			grid[emptyPositions[i].x][emptyPositions[i].y] += 1;

			bool failed{ false };

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

// creates a grid for the user to play Sudoku on
void generateGrid()
{
	vector<int> values {1, 2, 3, 4, 5, 6, 7, 8, 9}; //	array of all possible integers

	generateTopLeft(values); //	creates the top left square of the grid

	vector<int> usedValues{ };

	for (int i = 0; i <= 2; i++)
	{
		vector<int> potentials{ };
		switch(i)
		{
		case 0:
			potentials = removePreviousRow(values, i);
			usedValues = generateTopMiddle(potentials, i);
			break;
		case 1:
			potentials = removeTopRowValues(usedValues, values);
			potentials = removePreviousRow(potentials, i);
			potentials = generateTopMiddle(potentials, i);
			usedValues.insert(usedValues.end(), potentials.begin(), potentials.end());
			break;
		case 2:
			potentials = removeTopRowValues(usedValues, values);
			potentials = removePreviousRow(potentials, i);

			if (potentials.size() <= 2) //	if the number of potentials is less then 3, then the Square needs to be re-randomized
			{
				i = -1; //reset the for loop
			}
			else
				generateTopMiddle(potentials, i);

			usedValues = { };
			break;
		}
	}

	generateTopRight(values);


	for (int i = 0; i <= 0; i++)
	{
		vector<int> potentials{ };
		switch(i)
		{
		case 0:
			potentials = removePreviousColumn(values, i);
			usedValues = generateMiddleLeft(potentials, i);
			break;
		}
	}

	outputGrid();
}

//	for the sake of optimization, the top left square of the Sudoku is generated
//	at random, as no "Sudoku rules" need to be applied yet
void generateTopLeft(vector<int> integers)
{
	for(int i = 0; i <= 2; i++)
	{
		for (int j = 0; j <= 2; j++)
		{
			unsigned int index = 0 + (rand() % integers.size());

			grid[i][j] = integers[index];
			integers.erase(integers.begin() + index);
		}
	}
}

vector<int> generateTopMiddle(vector<int> integers, int row)
{
	vector<int> chosenValues{ };

	for (int i = 3; i <= 5; i++)
	{
		unsigned int index = (0 + (rand() % integers.size()));

		grid[row][i] = integers[index];
		chosenValues.push_back(integers[index]);
		integers.erase(integers.begin() + index);
	}

	return chosenValues;
}

void generateTopRight(vector<int> integers)
{
	for (int i = 0; i <= 2; i++)
	{
		vector<int> values = integers;
		for (int j = 0; j <= 5; j++)
		{
			if (count(values.begin(), values.end(), grid[i][j]))
			{
				values.erase(remove(values.begin(), values.end(), grid[i][j]), values.end());
			}
		}

		for (int k = 6; k <= 8; k++)
		{
			unsigned int index = (0 + (rand() % values.size()));

			grid[i][k] = values[index];
			values.erase(values.begin() + index);
		}
	}
}


vector<int> generateMiddleLeft(vector<int> integers, int column)
{
	vector<int> chosenValues{ };

	for (int i = 3; i <= 5; i++)
	{
		unsigned int index = (0 + rand() % integers.size());

		grid[i][column] = integers[index];
		chosenValues.push_back(integers[index]);
		integers.erase(integers.begin() + index);
	}

	return chosenValues;
}

//	removes the elements from the list of potentials found on row already
vector<int> removePreviousRow(vector<int> integers, int row)
{
	vector<int> allAllowedIntegers{ };

	int removed{ 0 };
	int counter{ 0 };
	while (removed < 3 && counter < integers.size())
	{
		for (int j = 0; j <= 2; j++)
		{
			if (integers[counter] == grid[row][j])
			{
				integers.erase(integers.begin() + (counter));
				removed += 1;
				counter = -1;
				break;
			}
		}

		counter += 1;
	}

	allAllowedIntegers = integers;

	return allAllowedIntegers;
}

vector<int> removePreviousColumn(vector<int> integers, int column)
{
	vector<int> allAllowedIntegers{ };

	int removed{ 0 };
	int counter{ 0 };
	while (removed < 3 && counter < integers.size())
	{
		for (int j = 0; j <= 2; j++)
		{
			if (integers[counter] == grid[j][column])
			{
				integers.erase(integers.begin() + counter);
				removed += 1;
				counter -= 1;
				break;
			}
		}

		counter += 1;
	}

	allAllowedIntegers = integers;

	return allAllowedIntegers;
}

vector<int> removeTopRowValues(vector <int> usedIntegers, vector<int> integers)
{
	vector<int> allowedValues{ integers };

	int removed{ 0 };
	for (int i = 0; i <= usedIntegers.size() - 1; i++)
	{
		for (int j = 0; j <= allowedValues.size() - 1; j++)
		{
			if (usedIntegers[i] == allowedValues[j])
			{
				allowedValues.erase(allowedValues.begin() + j);
				removed -= 1;
			}
		}
	}

	return allowedValues;
}

int main()
{
	srand(time(0));

	generateGrid();

	//	storing all empty cell positions in a vector-list of Vector2's
	//vector<Vector2> emptyPositions { returnEmptyPositions() };

	//unsigned int emptyCount{ emptyPositions.size() };


	//solveSudoku(emptyPositions, emptyCount);

	//cout << "\n";
	//cout << "Number of empty positions: " << emptyCount << "\n";
	//cout << "\n";

	//outputGrid();

	return 0;
}
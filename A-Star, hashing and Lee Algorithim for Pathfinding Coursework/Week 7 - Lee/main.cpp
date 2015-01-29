#include <iostream>
#include <windows.h>
#include <deque>

using namespace std;

int board[8][8];

struct coord
{
	int x, y; //board coordinates
};

bool found = false; //have I found the target?
bool back_found = false; //have I found the start?
int path_length; //current path length 

deque <coord> working_set; //used for filling the board
deque <coord> w_set; //used for backtracing

void display()
{
	for(int x = 0; x < 8; x++)
	{
		for(int y = 0; y < 8; y++)
		{
			cout << board[x][y] << "   ";
		}
		cout << endl;
	}
	cout << endl;
}

void move(coord target) //single_step from gpsa
{
	//check neighbours for -1 (get coords from working set)
	coord temp, position;
	temp = working_set.front();
	working_set.pop_front();

	path_length = board[temp.x][temp.y];
	
	for (int x = -1; x < 2; x++)
	{
		for (int y = -1; y < 2; y++)
		{
			//discount diagonals from loop
			if (x == y) continue;
			if (-x == y) continue;
			if (x == -y) continue;

			//board limits
			if ((temp.x + x) < 0 || (temp.x + x) > 7) continue;
			if ((temp.y + y) < 0 || (temp.y + y) > 7) continue;

			//blockages
			if ((temp.x + x) == -9 || (temp.y + y) == -9) continue;

			//check if the target has been found, otherwise run the standard loop
			if (board[temp.x][temp.y] == board[target.x][target.y])
			{
				found = true;
				back_found = true;
				w_set.push_back(target);
			}
			else if ((board[temp.x + x][temp.y + y]) == -1)
			{
				position.x = (temp.x + x);
				position.y = (temp.y + y);
				board[temp.x + x][temp.y + y] = path_length + 1;
				working_set.push_back(position);
			}
		}
	}
}

void backtrace(coord start)
{
	while(back_found)
	{
		coord back_temp, path;
		back_temp = w_set.back();

		path_length = board[back_temp.x][back_temp.y];

		for (int x = -1; x < 2; x++)
		{
			for (int y = -1; y < 2; y++)
			{
				if (x == y) continue;
				if (-x == y) continue;
				if (x == -y) continue;

				if ((back_temp.x + x) < 0 || (back_temp.x + x) > 7) continue;
				if ((back_temp.y + y) < 0 || (back_temp.y + y) > 7) continue;

				if ((back_temp.x + x) == -9 || (back_temp.y + y) == -9) continue;

				if (board[back_temp.x][back_temp.y] == board[start.x][start.y])
				{
					back_found = false;
				}
				else if ((board[back_temp.x + x][back_temp.y]) == (path_length - 1))
				{
					path.x = (back_temp.x + x);
					path.y = back_temp.y;
					path_length = board[back_temp.x + x][back_temp.y];
					w_set.push_back(path);
				}
				else if ((board[back_temp.x][back_temp.y + y]) == (path_length - 1))
				{
					path.x = back_temp.x;
					path.y = (back_temp.y + y);
					path_length = board[back_temp.x + x][back_temp.y];
					w_set.push_back(path);
				}
			}
		}
	}

	for(int x = 0; x < 8; x++)
	{
		for(int y = 0; y < 8; y++)
		{
			board[x][y] = 0; //-1 = empty
		}
	}
	
	while(!w_set.empty())
	{
		coord ty = w_set.front();
		w_set.pop_front();

		board[ty.x][ty.y] = 1;
	}
}

void main()
{
	coord start;
	coord target;
	coord current;

	start.x = 3;
	start.y = 3;

	target.x = 7;
	target.y = 7;

	working_set.push_back(start);

	for(int x = 0; x < 8; x++)
	{
		for(int y = 0; y < 8; y++)
		{
			board[x][y] = -1; //-1 = empty
		}
	}

	board[start.x][start.y] = 0; //initialises start position distance value to 0

	while(!found) 
	{
		move(target);
	}

	display();

	backtrace(start);
	display();

	cin.get();
}


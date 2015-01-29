#include <iostream>
#include <windows.h>
#include <queue>
#include <deque>
#include <math.h>
#include <vector>
#include <string>

using namespace std;

const int board_x = 8;
const int board_y = 8;

class Node
{
public:
	int x, y;
	int g, h;
	bool block; //is this node available to visit?
	bool visited; //has this node been visited?

	Node* parent; //pointer to current nodes parent

	bool operator>(const Node &a)const 
	{
		//set lowest f-cost to be at head of priority queue
		return f() > a.f();
	}

	void set(int set_x, int set_y)
	{
		x = set_x;
		y = set_y;
	}

	int f() const
	{	
		return g + h;
	}
};

bool found = false; //have I found the target?
bool back_found = false; //have I found the start?

Node board[board_x][board_y];
priority_queue< Node, vector<Node>, greater<Node> > OpenList;

void display()
{
	for(int x = 0; x < 8; x++)
	{
		for(int y = 0; y < 8; y++)
		{
			cout << board[x][y].g << "\t";
		}
		cout << endl;
	}
	cout << endl;
}

void move(Node target) //single_step from gpsa
{
	//check neighbours for -1 (get coords from working set)
	Node current, temp;
	current = OpenList.top();
	OpenList.pop();
	board[current.x][current.y].visited = true;
	board[current.x][current.y].h = abs(target.x - current.x) + abs(target.y - current.y);

	for (int x = -1; x < 2; x++)
	{
		for (int y = -1; y < 2; y++)
		{	
			//board limits
			if ((current.x + x) < 0 || (current.x + x) > 7) continue;
			if ((current.y + y) < 0 || (current.y + y) > 7) continue;
			
			//discount diagonals from loop
			if (x == y) continue;
			if (-x == y) continue;
			if (x == -y) continue;

			temp = board[current.x + x][current.y + y]; 
			if (board[temp.x][temp.y].visited == true) continue;

			//blockages
			if (temp.block) continue;

			int distance = board[current.x][current.y].g;

			board[temp.x][temp.y].h = abs(target.x - temp.x) + abs(target.y - temp.y);
			board[temp.x][temp.y].g = distance + 1;

			if ((current.x == target.x) && (current.y == target.y))
			{
				board[target.x][target.y].parent = &current;
				found = true;
				back_found = true;
			}
			else if (board[temp.x][temp.y].h <= board[current.x][current.y].h)
			{
				board[temp.x][temp.y].parent = &board[current.x][current.y];
				OpenList.push(temp);
			}
		}
	}
}

void backtrace(Node start, Node target)
{
	Node* back_step;
	back_step = board[target.x][target.y].parent;

	if(back_step)
	{
		for(int x = 0; x < board_x; x++)
		{
			for(int y = 0; y < board_y; y++)
			{
				board[x][y].g = 1; //only for display purposes, makes the path stand out
			}
		}

		board[back_step->x][back_step->y].g = 9;

		bool start_found = false;
		while(!start_found)
		{
			if (back_step->x == start.x && back_step->y == start.y)
			{
				start_found = true;
			}
			else 
			{
				//follow nodes parents
				back_step = back_step->parent;
			}

			board[back_step->x][back_step->y].g = 9; //again, making path stand out
			board[target.x][target.y].g = 9;
		}
	}
}

void main()
{
	Node start;
	Node target;

	//set start and target positions
	start.set(3, 3);
	target.set(7, 7); 

	for(int x = 0; x < board_x; x++)
	{
		for(int y = 0; y < board_y; y++)
		{
			board[x][y].x = x;
			board[x][y].y = y;
			board[x][y].visited = false;
			board[x][y].block = false;
			board[x][y].g = 0;
		}
	}

	board[start.x][start.y].g = 0;
	board[start.x][start.y].visited = true;
	board[start.x][start.y].parent = &start;

	//determine blockages
	/*board[4][4].block = true;
	board[4][5].block = true;
	board[4][6].block = true;
	board[4][7].block = true;
*/
	OpenList.push(start);

	while(!OpenList.empty() && !found) 
	{
		move(target);
	}

	display();

	backtrace(start, target);

	display();

	cin.get();
}


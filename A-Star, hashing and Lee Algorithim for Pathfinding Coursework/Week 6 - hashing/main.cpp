#include <iostream>
#include <windows.h>
#include <time.h>

using namespace std;

struct  node_rec  
{
	int node_number;		// nodes kept in order
	int  dat;
	struct node_rec * left;    		// doubly-linked    
	struct node_rec * right;
};

typedef  struct  node_rec  node;
typedef  struct  node_rec  *  pointer;
enum game {win, loss, draw, undecided, illegal};

struct position 
{
	int bd[3][3];
	game g;
	int npieces;
};

position wset[800];
bool check[20000] = {false};

int head = 0, rear = 0, dlist1 = 0;

void main() 
{
	unsigned long t;
	double time;

	position temp;		// used to initialise data structures
	int i, j;			// loop counters

	void check_wset();
	void single_step();
	game is_win_or_draw(position p, int xo);
	int is_duplicate(int p[3][3], int q[3][3]);
	void rotation(int b[3][3]);
	void reflection(int b[3][3]);
	void print_status();
	void add_to_wset(position p);
	int wset_not_empty();
	void add_to_dlist(position p);
	void printp(position p);
	void printg(game gg);

	t = clock();
	
	//  Set up initial position 
	for(i=0; i<3; i++)
		for(j=0; j<3; j++)
			temp.bd[i][j] = 0; 
	temp.g = undecided;
	temp.npieces = 0;

	//and initialise wset and dlist
	add_to_wset(temp);
	add_to_dlist(temp);

	while(wset_not_empty()) 
	{
		single_step();
	}
	print_status();
	Sleep(2000);
	//check_wset();

	time = ((double)(clock() - t)) / CLOCKS_PER_SEC;
	cout << "Time taken = " << time << endl;

	cin.get();
}
 
void check_wset() 
{
	int i=0;							// loop counter
	pointer head, tail;				// constant pointers to dummy nodes
	void  check_list( pointer h, pointer t );	//  prototype
	int min_boardval(position p);
	void  insert( int v, int num, pointer h );

	head = (pointer) malloc(sizeof(node));	// dummy head node
	tail = (pointer) malloc(sizeof(node));	// dummy tail node	
	head->left = NULL;
	head->right = tail;
	tail->left = head;
	tail->right = NULL;				//  empty list

	head->node_number = 0;			// min value		
	tail->node_number = (1<<30) - 1 + (1<<30);		// max value		

	while (i<765)
	{
		insert( i+1, min_boardval(wset[i]), head);
		if (i%20 == 19) 
			Sleep (500);
		i++;
	}
}

void  insert( int v, int num, pointer h )
{
	static int ncount = 0;		// Node counter
	//	insert immediately to the left of node_number num

	pointer   temp, runner = h->right;
	temp = (pointer) malloc ( sizeof ( node ) );

	// Could guard against memory failure here, trap temp == NULL

	temp->dat = v;				// initialise new node
	temp->node_number = num;		// key on board value

	//  Now search to find new right hand neighbour node

	while (runner->node_number < num) 
	runner = runner->right;

	// Now insert node "temp" to the left of node "runner"

	if(runner->node_number==num)
		cout<<"insert   clash  "<< num<<endl;

	temp->right = runner;
	temp->left = runner->left;
	temp->left->right = temp;
	temp->right->left = temp;

	cout << "insert   " << ++ncount << "\t" << num << endl;
}

 
int boardval(int b[3][3])
{
	int i, j;						// loop counters
	int total;						// accumulate ternary number

	for(total=0, i=0; i<3; i++)
		for(j=0; j<3; j++)
			total = 3*total + b[i][j];

	return total;
}


int min_boardval(position p) 
{

	int min=20000,					// minimum so far, initialised high
		temp,						// temporary store
		k;							// loop counter

	int boardval(int b[3][3]);
	void rotation(int b[3][3]);
	void reflection(int b[3][3]);

	// First the four rotations
	for(k=0; k<4; k++)
	{
		rotation(p.bd);
		temp = boardval(p.bd);
		if (temp<min) min = temp;
	}

	// Now the mirror images
	reflection(p.bd);
	for(k=0; k<4; k++) 
	{
		rotation(p.bd);
		temp = boardval(p.bd);
		if (temp<min) min = temp;
	}

	return min;
}

void single_step() 
{
	position temp, possible;  
	int i, j, k, xo;

	void add_to_wset(position p);
	void add_to_dlist(position p);
	void rotation(int b[3][3]);
	void reflection(int b[3][3]);
	int is_duplicate(int p[3][3], int q[3][3]);
	position get_from_wset();
	int search_dlist(position p);
	void print_status();
	game is_win_or_draw(position p, int xo);

//	take position from wset
	temp = get_from_wset();
	if(temp.g!=undecided)		//  game over already
		return;					//  skip the rest
//	generate nine new positions.  
//	Could do this in one step (setting some to illegal).
//	I plan to process them in full one after the other.
	temp.npieces++;
	xo = ((temp.npieces%2)==1)? 1 : 2;
	for(i=0; i<3; i++) 
	{
		for(j=0; j<3; j++)
		{
			possible = temp;
//	skip illegals
			if(possible.bd[i][j]==0)
			{			// move legal
				possible.bd[i][j] = xo;
//	check for duplicates
				if (search_dlist(possible)==0) 
				{	// not found
//	add survivors to wset
					// first set g field
					possible.g = is_win_or_draw(possible, xo);	
					add_to_wset(possible);
//	add all 8 equivalents to dlist. Yes. "if not already present" - No

/*	This is really crude, some positions will be added multiple times
	Could either create "add_if_new()" rather than use add_to_dlist()	
	or could modify is_duplicate() to spot symmetry copies.	
	For now I'll just leave this - it is simple at least!			*/

					// First the four rotations
					for(k=0; k<4; k++) 
					{
						rotation(possible.bd);
						add_to_dlist(possible);
					}
					// Now the mirror images
					reflection(possible.bd);
					for(k=0; k<4; k++) 
					{
						rotation(possible.bd);
						add_to_dlist(possible);
					}
				} // if ... not duplicate
			} // if ...  move legal
		} // for j ...
	} // for i ..
}
 
int search_dlist(position p) 
{
	//Use min_boardval function with large bool array
	return check[min_boardval(p)];	
}


game is_win_or_draw(position p, int xo)
{
	//	Checking after a move can only lead to 
	//		a win, return win if xo == 1, loss if xo == 2
	//		a draw (if the board is now full, or
	//		(still) undecided).

	//	There are 8 columns / rows / diagonals patterns leading to
	//	a decisive result:

	if( (xo==p.bd[0][0])&&(xo==p.bd[0][1])&&(xo==p.bd[0][2]) ||
		(xo==p.bd[1][0])&&(xo==p.bd[1][1])&&(xo==p.bd[1][2]) ||
		(xo==p.bd[2][0])&&(xo==p.bd[2][1])&&(xo==p.bd[2][2]) ||
		(xo==p.bd[0][0])&&(xo==p.bd[1][0])&&(xo==p.bd[2][0]) ||
		(xo==p.bd[0][1])&&(xo==p.bd[1][1])&&(xo==p.bd[2][1]) ||
		(xo==p.bd[0][2])&&(xo==p.bd[1][2])&&(xo==p.bd[2][2]) ||
		(xo==p.bd[0][0])&&(xo==p.bd[1][1])&&(xo==p.bd[2][2]) ||
		(xo==p.bd[0][2])&&(xo==p.bd[1][1])&&(xo==p.bd[2][0]) ) 
		if (xo==1) return win;
		else return loss;
	else if (p.npieces==9) return draw;
	else return undecided;
}	
	
	
int is_duplicate(int p[3][3], int q[3][3])
{
	int i, j, same = 1;

	for(i=0; i<3; i++)
		for(j=0; j<3; j++)
			if(p[i][j] != q[i][j]) same = 0;
	return same;
}


void swap(int &a, int &b) 
{
    int  temp = b;
    b = a;
    a = temp;
}

 
void rotation(int b[3][3]) 
{
	//	Rotate 90 degrees clockwise
	int temp = b[0][0];

	b[0][0] = b[2][0];
	b[2][0] = b[2][2];
	b[2][2] = b[0][2];
	b[0][2] = temp;
	temp = b[1][0];
	b[1][0] = b[2][1];
	b[2][1] = b[1][2];
	b[1][2] = b[0][1]; 
	b[0][1] = temp;
}


void reflection(int b[3][3]) 
{
	//	Reflect through vertical, middle column
	int i;
	void swap(int &a, int &b);

	for(i=0; i<3; i++)
		swap(b[0][i], b[2][i]);
}


void print_status()
{
	cout << endl << "Working set size     = " << rear;
	cout << "\tDuplicates list size = " << dlist1 << endl << endl;
}


void add_to_wset(position p)
{
	void printp(position p);

	wset[rear++] = p;
}


int wset_not_empty() 
{
	return head < rear;
}


position get_from_wset() 
{
	return wset[head++];
}


void add_to_dlist(position p)
{
	check[min_boardval(p)] = true; 
}



 
void printp(position p) 
{
	int i,j;

	void printg(game gg);
 
    for (i = 2; i >= 0; i--) 
	{
        for (j = 0; j < 3; j++)
            cout <<  p.bd[j][i];
        cout << endl;
    }
	printg(p.g);
	cout << endl;
}


void printg(game gg) 
{
	switch (gg) 
	{
		case win:       cout<<"win"      <<endl; break;
		case loss:      cout<<"loss"     <<endl; break;
		case draw:      cout<<"draw"     <<endl; break;
		case undecided: cout<<"undecided"<<endl; break;
		case illegal:   cout<<"illegal"  <<endl; break;
	}
	cout << endl;
}




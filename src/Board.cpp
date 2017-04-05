#include "Board.hpp"

Board::Board(void)
{
}

Board::~Board(void)
{
	freeTabs();
}

void
Board::freeTabs(void)
{
	if (cell != 0)
		delete [] cell;
	if (graph_cell != 0)
		delete [] graph_cell;
	if (n_tab != 0)
		delete [] n_tab;
}

void
Board::initSolved(int side_size)
{
	this->size_x = side_size;
	this->size_y = side_size;
	if (side_size < 3)
	{
		this->size_x = 3;
		this->size_y = 3;
	}
	if (side_size > 120)
	{
		this->size_x = 120;
		this->size_y = 120;
	}
	size = size_x * size_y;
	max = size - 1;
	n_tab = new int[size];
	cell = new Cell[size];
	graph_cell = new Cell[size];
	for (int i = 0; i < size; i++)
	{
		cell[i].value = i;
		cell[i].neighbor[0] = -1;
		cell[i].neighbor[1] = -1;
		cell[i].neighbor[2] = -1;
		cell[i].neighbor[3] = -1;
		cell[i].index = i;
		cell[i].movable = 1;
		cell[i].x = -1;
		cell[i].y = -1;
		cell[i].dist_from_start = size;
		cell[i].dist_from_end = size;
		cell[i].movable = 1;
		cell[i].visited = 0;
	}
	init_status = 1;
	swap_queue_status = EMPTY;
	findNeighborsAndPosition();
	replicateCell();
}

void
Board::changeSize(int side_size)
{
	freeTabs();
	initSolved(side_size);
	reset();
}

void
Board::reset(void)
{
	_is_solved = NO;
	clearSwapList();
	resetGraphCell();
	resetCell();
}

void
Board::resetCell(void)
{
	for (int i = 0; i < size; i++)
	{
		cell[i].value = i;
	}
}

void
Board::resetGraphCell(void)
{
	for (int i = 0; i < size; i++)
	{
		graph_cell[i].value = i;
	}
}

int
Board::isItSolved(void)
{
	if (_is_solved == YES)
		return (YES);
	for (int i = 0; i < size; i++)
	{
		if (cell[i].value != cell[i].index)
			return (NO);
	}
	_is_solved = YES;
	return (YES);
}

void
Board::cellSwap(int index_a, int index_b)
{
	int		val_tmp;

	if (index_a < size && index_b < size)
	{
		val_tmp = cell[index_b].value;
		cell[index_b].value = cell[index_a].value;
		cell[index_a].value = val_tmp;
	}
	else
	{
		std::cerr << "board.cellSwap() failed, one or both index are < size" << std::endl;
	}
}

void
Board::graphCellSwap(int index_a, int index_b)
{
	int		val_tmp;

	if (index_a < size && index_b < size)
	{
		val_tmp = graph_cell[index_b].value;
		graph_cell[index_b].value = graph_cell[index_a].value;
		graph_cell[index_a].value = val_tmp;
	}
	else
	{
		std::cerr << "board.graphCellSwap() failed, one or both index are < size" << std::endl;
	}
}

bool
Board::isNeighbor(int cell_index, int possible_neighbor_index)
{
	for (int i = 0; i < 4; i++)
	{
		if (cell[cell_index].neighbor[i] == possible_neighbor_index)
			return (true);
	}
	return (false);
}

bool
Board::isValidCellSwap(int index_a, int index_b)
{
	if ((cell[index_a].value == max || cell[index_b].value == max) && isNeighbor(index_a, index_b))
	{
		return (true);
	}
	else
	{
		std::cerr << "board.validCellSwap(" << index_a << ", " << index_b << ") invalid move" << std::endl;
		return (false);
	}
}

void
Board::validSwap(int index_a, int index_b)
{
	if (isValidCellSwap(index_a, index_b))
	{
		cellSwap(index_a, index_b);
		graphCellSwap(index_a, index_b);
	}
}

void
Board::validShuffle(void)
{
	int a = max;
	int b = -1;
	for (int i = 0; i < size * size * 0.5; i++)
	{
		while (b == -1)
		{
			b = cell[a].neighbor[rand() % 4];
		}
		validSwap(a, b);
		a = b;
		b = -1;
	}
}

void
Board::randomShuffle(int power)
{
	int a = -1;
	int b = -1;
	for (int i = 0; i < size * power; i++)
	{
		a = rand() % max;
		b = rand() % max;
		cellSwap(a, b);
		graphCellSwap(a, b);
	}
}

void
Board::findAndSwap(int direction)
{
	int i;

	for (i = 0; i < size; i++)
	{
		if (graph_cell[i].value == max)
		{
			break;
		}
	}
	if (graph_cell[i].neighbor[direction] != -1)
	{
		graphCellSwap(i, graph_cell[i].neighbor[direction]);
		cellSwap(i, graph_cell[i].neighbor[direction]);
	}
}

void
Board::replicateCell(void)
{
	for (int i = 0; i < size; i++)
	{
		graph_cell[i].value = cell[i].value;
		graph_cell[i].neighbor[0] = cell[i].neighbor[0];
		graph_cell[i].neighbor[1] = cell[i].neighbor[1];
		graph_cell[i].neighbor[2] = cell[i].neighbor[2];
		graph_cell[i].neighbor[3] = cell[i].neighbor[3];
		graph_cell[i].index = cell[i].index;
		graph_cell[i].x = cell[i].x;
		graph_cell[i].y = cell[i].y;
	}
}

void
Board::clearSwapList(void)
{
    while (!swap_queue.empty())
    {
        swap_queue.pop();
    }
    swap_queue_status = EMPTY;
}

void
Board::findNeighborsAndPosition(void)
{
	int		i_x = 0;
	int		i_y = 0;
	int		j = 0;

	// PLACE CELL INDEX IN N_TAB
	for (int i = 0; i < size; i++)
		n_tab[i] = -1;
	j = 0;
	n_tab[i_y * size_y + i_x] = cell[j].index;
	cell[j].x = i_x;
	cell[j].y = i_y;
	while (j < size - 1)
	{
		// right
		while (i_x + 1 < size_x && n_tab[i_y * size_y + (i_x + 1)] == -1)
		{
			i_x++;
			j++;
			n_tab[i_y * size_y + i_x] = cell[j].index;
			cell[j].x = i_x;
			cell[j].y = i_y;
		}
		// down
		while (i_y + 1 < size_y && n_tab[(i_y + 1) * size_y + i_x] == -1)
		{
			i_y++;
			j++;
			n_tab[i_y * size_y + i_x] = cell[j].index;
			cell[j].x = i_x;
			cell[j].y = i_y;
		}
		// left
		while (i_x - 1 >= 0 && n_tab[i_y * size_y + (i_x - 1)] == -1)
		{
			i_x--;
			j++;
			n_tab[i_y * size_y + i_x] = cell[j].index;
			cell[j].x = i_x;
			cell[j].y = i_y;
		}
		// up
		while (i_y - 1 >= 0 && n_tab[(i_y - 1) * size_y + i_x] == -1)
		{
			i_y--;
			j++;
			n_tab[i_y * size_y + i_x] = cell[j].index;
			cell[j].x = i_x;
			cell[j].y = i_y;
		}
	}

/*	// Print n_tab
	std::cout << "n_tab: " << std::endl;
	for (i_y = 0; i_y < size_y; i_y++)
	{
		for (i_x = 0; i_x < size_x; i_x++)
		{
			std::cout << "[" << n_tab[i_y * size_y + i_x] << "]";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;

	// Print cell coord
	for (int i = 0; i < size; ++i)
	{
		std::cout << i << "[" << cell[i].x << "," << cell[i].y << "]";
	}
	std::cout << std::endl << std::endl;
*/
	int index = 0;
	// store neighbor
	for (i_y = 0; i_y < size_y; i_y++)
	{
		for (i_x = 0; i_x < size_x; i_x++)
		{
			//  right
			if ((i_x + 1) < size_x)
			{
				cell[n_tab[index]].neighbor[0] = n_tab[i_y * size_y + (i_x + 1)];
			}
			//  down
			if ((i_y + 1) < size_y)
			{
				cell[n_tab[index]].neighbor[1] = n_tab[(i_y + 1) * size_y + i_x];
			}
			//  left
			if ((i_x - 1) >= 0)
			{
				cell[n_tab[index]].neighbor[2] = n_tab[i_y * size_y + (i_x - 1)];
			}
			//  up
			if ((i_y - 1) >= 0)
			{
				cell[n_tab[index]].neighbor[3] = n_tab[(i_y - 1) * size_y + i_x];
			}
			index++;
		}
	}
/*
	// Print neighbor
	std::cout << "Cell index[], value[], neighbor()()()()" << std::endl;
	for (int i = 0; i < size; i++)
	{
		std::cout << "[" << cell[i].index << "][" << cell[i].value << "] ";
		for (int j = 0; j < 4; j++)
		{
			std::cout << "(" << cell[i].neighbor[j] << ")";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
	*/
}

int
Board::getInitStatus(void) const
{
	return (init_status);
}

bool
Board::cellCmp(Cell &cell1, Cell &cell2)
{
	if (cell1.index != -1 &&
		cell1.value == cell2.value &&
		cell1.neighbor[0] == cell2.neighbor[0] &&
		cell1.neighbor[1] == cell2.neighbor[1] &&
		cell1.neighbor[2] == cell2.neighbor[2] &&
		cell1.neighbor[3] == cell2.neighbor[3] &&
		cell1.index == cell2.index &&
		cell1.x == cell2.x  &&
		cell1.y == cell2.y)
		return (0);
	return (1);
}

std::ostream
&operator<<(std::ostream &o, Board const &my_board)
{
	if (my_board.getInitStatus() == 1)
	{
		o << "--------------------- BOARD --------------------" << std::endl;
		// o << "Size = " << my_board.size << std::endl << std::endl;
		o << std::endl;
		for (int i = 0; i < my_board.size; i++)
		{
			o 	<< "[" << my_board.cell[i].index << "]";
		}
		o << " <- Index" << std::endl;
		for (int i = 0; i < my_board.size; i++)
		{
			o 	<< "[" << my_board.cell[i].value << "]";
		}
		o << " <- Board" << std::endl;
		for (int i = 0; i < my_board.size; i++)
		{
			o 	<< "[" << my_board.graph_cell[i].value << "]";
		}
		o << " <- Graph" << std::endl;
		o << std::endl;

		int		i_x = 0;
		int		i_y = 0;
		o << "Index: " << std::endl;
		for (i_y = 0; i_y < my_board.size_y; i_y++)
		{
			for (i_x = 0; i_x < my_board.size_x; i_x++)
			{
				o << "[" << my_board.cell[my_board.n_tab[i_y * my_board.size_y + i_x]].index << "]";
			}
			o << std::endl;
		}
		o << std::endl;
		o << "Board: " << std::endl;
		for (i_y = 0; i_y < my_board.size_y; i_y++)
		{
			for (i_x = 0; i_x < my_board.size_x; i_x++)
			{
				o << "[" << my_board.cell[my_board.n_tab[i_y * my_board.size_y + i_x]].value << "]";
			}
			o << std::endl;
		}
		o << std::endl;
		o << "Graph: " << std::endl;
		for (i_y = 0; i_y < my_board.size_y; i_y++)
		{
			for (i_x = 0; i_x < my_board.size_x; i_x++)
			{
				o << "[" << my_board.graph_cell[my_board.n_tab[i_y * my_board.size_y + i_x]].value << "]";
			}
			o << std::endl;
		}
		o << "------------------------------------------------" << std::endl;
	}
	else
	{
		o << "Need to init(x, y) Board class." << std::endl;
	}
	return (o);
}


Board::Cell::Cell()
{
}

Board::Cell::Cell(Cell const &cell)
{
	*this = cell;
}

Board::Cell::~Cell()
{
}

Board::Cell		&Board::Cell::operator=(Cell const &cell)
{

	value = cell.value;
	neighbor[0] = cell.neighbor[0];
	neighbor[1] = cell.neighbor[1];
	neighbor[2] = cell.neighbor[2];
	neighbor[3] = cell.neighbor[3];
	index = cell.index;
	x = cell.x;
	y = cell.y;
	dist_from_start = cell.dist_from_start;
	dist_from_end = cell.dist_from_end;
	movable = cell.movable;
	visited = cell.visited;
	return (*this);
}

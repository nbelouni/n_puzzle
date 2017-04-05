#include "Algo.hpp"
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <stdlib.h>

Algo::Algo(void)
{
	board = NULL;
	heuristic = &Algo::ManhattanDist;
	numeric.path = NULL;
	blank.path = NULL;
	tmp_path = NULL;
}

Algo::~Algo(void)
{
}

void
Algo::setBoard(Board *b)
{
	if (numeric.path)
		delete [] numeric.path;
	if (blank.path)
		delete [] blank.path;
	if (tmp_path)
		delete [] tmp_path;
	board = b;
	numeric.path = new Board::Cell[board->size];
	numeric.path_i = 0;
	for (int i = 0; i < board->size; i++)
		initCell(numeric.path[i]);
	blank.path = new Board::Cell[board->size];
	blank.path_i = 0;
	for (int i = 0; i < board->size; i++)
		initCell(blank.path[i]);
	tmp_path = new Board::Cell[board->size];
	tmp_path_i = 0;
	for (int i = 0; i < board->size; i++)
		initCell(tmp_path[i]);
}

void
Algo::initCell(Board::Cell &cell)
{
	cell.value = -1;
	cell.neighbor[0] = -1;
	cell.neighbor[1] = -1;
	cell.neighbor[2] = -1;
	cell.neighbor[3] = -1;
	cell.index = -1;
	cell.movable = 1;
	cell.x = -1;
	cell.y = -1;
	cell.dist_from_start = board->size;
	cell.dist_from_end = board->size;
	cell.movable = 1;
	cell.visited = 0;
}

void		Algo::clear(Board::Cell *array, int &max)
{
	for (int i = 0; i < max; i++)
	{
		initCell(array[i]);
	}
	max = 0;
}

void
Algo::insert(Board::Cell cell, int place)
{
	int			j;
	Board::Cell tmp = tmp_path[place];
	Board::Cell tmp2;

	tmp_path_i += 1;
	if (tmp_path[place].value == tmp_path[place + 1].value)
		return ;
	for (j = place + 1; j < tmp_path_i; j++)
	{
		tmp2 = tmp_path[j];
		tmp_path[j] = tmp;
		tmp = tmp2;
	}
	tmp_path[place] = cell;
}

void
Algo::erase(int place, Board::Cell *array, int &max)
{
	int j;

	for (j = place; j < max; j++)
	{
		array[j] = array[j + 1];
	}
	initCell(array[max -1]);
	max -= 1;
}

void
Algo::eraseFromBegin(int place, Board::Cell *array, int &max)
{
	Board::Cell tmp;
	int			j;
	int			i = 0;

	if (place < 0)
		return;
	for (j = place, i = 0; i < max - place; j++, i++)
	{
		array[i] = array[j];
	}
	while (i < j)
	{
		initCell(array[i]);
		max -= 1;
		i++;
	}
}

void
Algo::pushBack(Board::Cell *cell, Board::Cell *array, int &max)
{
	array[max] = *cell;
	max += 1;
}

void
Algo::popBack(Board::Cell *array, int &max)
{
	initCell(array[max -1]);
	max -= 1;
}

void
Algo::setHeuristic(int h)
{
	if (h == MANHATTAN1)
		heuristic = &Algo::ManhattanDist;
	if (h == MANHATTAN2)
		heuristic = &Algo::ManhattanDist2;
	if (h == HAMMING)
		heuristic = &Algo::HammingDist;
	if (h == PERMUTATION)
		heuristic = &Algo::PermutationsDist;
	if (h == MATRIX)
		heuristic = &Algo::MatrixDist;
}

bool
Algo::isResolved(void)
{
	for (int i = 0; i < board->size; i++)
	{
		if (i != board->cell[i].value - 1)
			return (0);
	}
	return (1);
}

Board::Cell
Algo::findPerValue(int curr_n)
{
	for (int i = 0; i < board->size; i++)
	{
		if (board->cell[i].value == curr_n)
			return (board->cell[i]);
	}
	return (board->cell[0]);
}

void
Algo::Swap(Board::Cell &c1, Board::Cell &c2)
{
	int index = c1.index;
	int	m;

	board->swap_queue.push(index);
	board->swap_queue.push(c2.index);
	board->cellSwap(index, c2.index);
	m = board->cell[index].movable;
	board->cell[index].movable = board->cell[c2.index].movable;
	board->cell[c2.index].movable = m;
	c1 = board->cell[c2.index];
}

void
Algo::Move(void)
{
	int	i = 0;

	while (i < blank.path_i)
	{
		if (board->isNeighbor(blank.start.index, blank.path[i].index))
			Swap(blank.start, blank.path[i]);
		i += 1;
	}
	clear(blank.path, blank.path_i);
}

void
Algo::printBoard()
{
	int i_y, i_x;
	for (i_y = 0; i_y < board->size_y; i_y++)
	{
		for (i_x = 0; i_x < board->size_x; i_x++)
		{
			std::cout << "[" << board->cell[board->n_tab[i_y * board->size_y + i_x]].value << "]";
		}
		std::cout << std::endl;
	}
		std::cout << std::endl;
	for (i_y = 0; i_y < board->size_y; i_y++)
	{
		for (i_x = 0; i_x < board->size_x; i_x++)
		{
			std::cout << "[" << board->cell[board->n_tab[i_y * board->size_y + i_x]].movable << "]";
		}
		std::cout << std::endl;
	}
		std::cout << std::endl;
}

void
Algo::initValues(int curr_n)
{
	(void)curr_n;
	for (int i = 0; i < board->size; i++)
	{
		board->cell[i].dist_from_end = board->size;
		board->cell[i].dist_from_start = board->size;
		board->cell[i].visited = 0;
		if (board->cell[i].value <= curr_n)
			board->cell[i].movable = 0;
		else
			board->cell[i].movable = 1;
	}
}

bool
Algo::canMove(Board::Cell cell)
{
	int		cant_move = 0;
	for (int i = 0; i < 4; i++)
	{
		if (cell.neighbor[i] == -1 ||
			board->cell[cell.neighbor[i]].movable == 0 ||
			board->cell[cell.neighbor[i]].visited == 1)
			cant_move += 1;
	}
	if (cant_move == 4)
		return (0);
	return (1);
}

void
Algo::key1(void)
{
	Board::Cell	end_4 = board->cell[blank.end.index - 4];
	Board::Cell	end_3 = board->cell[blank.end.index - 3];
	Board::Cell	end_2 = board->cell[blank.end.index - 2];
	Board::Cell	end_1 = board->cell[blank.end.index - 1];
	Board::Cell end_0 = board->cell[blank.end.index];
	Board::Cell n = board->cell[numeric.start.index];
	Board::Cell l;

	for (int i = 0; i < 4; i++)
	{
		if (end_4.neighbor[i] != -1 && board->cell[end_4.neighbor[i]].movable == 1)
			blank.end = board->cell[end_4.neighbor[i]];
	}

	clear(blank.path, blank.path_i);
	clear(tmp_path, tmp_path_i);
	blank.start.dist_from_end = 0;
	blank.start.dist_from_start = 0;
	AStar(blank.start, blank);
	cutPath(blank);
	Move();

	for (int i = 0; i < 4; i++)
	{
		if (end_3.neighbor[i] != -1 && board->cell[end_3.neighbor[i]].movable == 1)
			l = board->cell[end_3.neighbor[i]];
	}

	Swap(blank.start, end_4);
	end_4 = findPerValue(end_4.value);

	Swap(blank.start, end_3);
	end_3 = findPerValue(end_3.value);

	Swap(blank.start, end_2);
	end_2 = findPerValue(end_2.value);

	Swap(blank.start, end_1);
	end_1 = findPerValue(end_1.value);

	Swap(blank.start, end_0);
	end_0 = findPerValue(end_0.value);

	Swap(blank.start, n);
	n = findPerValue(n.value);

	Swap(blank.start, l);
	l = findPerValue(l.value);

	Swap(blank.start, end_0);
	end_0 = findPerValue(end_0.value);

	Swap(blank.start, end_1);
	end_1 = findPerValue(end_1.value);

	Swap(blank.start, end_2);
	end_2 = findPerValue(end_2.value);

	Swap(blank.start, end_3);
	end_3 = findPerValue(end_3.value);

	Swap(blank.start, end_4);
	end_4 = findPerValue(end_4.value);
}

void
Algo::key2(void)
{
	std::vector<Board::Cell>::iterator	it;
	Board::Cell	end_2 = board->cell[blank.end.index - 2];
	Board::Cell	end_1 = board->cell[blank.end.index - 1];
	Board::Cell end_0 = board->cell[blank.end.index];
	Board::Cell n = board->cell[numeric.start.index];
	Board::Cell l;

	for (int i = 0; i < 4; i++)
	{
		if (end_2.neighbor[i] != -1 && board->cell[end_2.neighbor[i]].movable == 1)
			blank.end = board->cell[end_2.neighbor[i]];
	}

	clear(blank.path, blank.path_i);
	clear(tmp_path, tmp_path_i);
	blank.start.dist_from_end = 0;
	blank.start.dist_from_start = 0;
	AStar(blank.start, blank);
	cutPath(blank);
	Move();

	for (int i = 0; i < 4; i++)
	{
		if (end_1.neighbor[i] != -1 && board->cell[end_1.neighbor[i]].movable == 1 &&
			board->cell[end_1.neighbor[i]].value != end_0.value)
			l = board->cell[end_1.neighbor[i]];
	}

	Swap(blank.start, end_2);
	end_2 = findPerValue(end_2.value);

	Swap(blank.start, end_1);
	end_1 = findPerValue(end_1.value);

	Swap(blank.start, end_0);
	end_0 = findPerValue(end_0.value);

	Swap(blank.start, n);
	n = findPerValue(n.value);

	Swap(blank.start, l);
	l = findPerValue(l.value);

	Swap(blank.start, end_0);
	end_0 = findPerValue(end_0.value);

	Swap(blank.start, end_1);
	end_1 = findPerValue(end_1.value);

	Swap(blank.start, end_2);
	end_2 = findPerValue(end_2.value);
}

void
Algo::cutPath(starterPack &pack)
{
	int i = pack.path_i - 1;

	if (pack.path_i < 2)
		return;
	while (--i >= 0)
	{
		if (!board->isNeighbor(pack.path[i].index, pack.path[i + 1].index))
			erase(i, pack.path, pack.path_i);
		else if (board->isNeighbor(pack.path[i].index, pack.start.index))
		{
			eraseFromBegin(i, pack.path, pack.path_i);
			return ;
		}
		
	}

}

void
Algo::resolve(void)
{
	int			curr_n = 0;
	int			i;
	Board::Cell	tmp_num_start;
	Board::Cell	tmp_blank;
	std::vector<Board::Cell>::iterator	it;
	int	can_solve = isSolvable();

	total_cells_tmp_path = 0;
	max_cells_all_path = 0;
	n_swap = 0;
	if (!can_solve)
	{
		board->swap_queue_status = UNSOLVABLE;
		return;
	}

	while (curr_n < board->size)
	{
		initValues(curr_n);
		numeric.start = findPerValue(curr_n);
		tmp_num_start = numeric.start;
		numeric.end = board->cell[curr_n];
		numeric.start.dist_from_end = (this->*heuristic)(numeric.start, numeric.end);

		if (board->cellCmp(numeric.start, numeric.end))
		{
			clear(tmp_path, tmp_path_i);
			clear(numeric.path, numeric.path_i);
			clear(tmp_path, tmp_path_i);
			numeric.start.dist_from_end = 0;
			numeric.start.dist_from_start = 0;

			AStar(numeric.start, numeric);
			cutPath(numeric);

			numeric.start = tmp_num_start;
			initValues(curr_n);
			i = 0;
			while (i < numeric.path_i)
			{
				blank.start = findPerValue(board->max);
				blank.end = board->cell[numeric.path[i].index];
				tmp_blank = blank.start;
				board->cell[numeric.start.index].movable = 0;
				blank.start.dist_from_end = (this->*heuristic)(blank.start, blank.end);
            
				initValues(curr_n);
				if (board->cellCmp(blank.start, blank.end))	
				{
					if (!canMove(blank.end))
					{
						if (!canMove(board->cell[blank.end.index - 2]))
						{
							key1();
							clear(numeric.path, numeric.path_i);
							clear(blank.path, blank.path_i);
							break;
						}
						else
						{
							key2();
							clear(numeric.path, numeric.path_i);
							clear(blank.path, blank.path_i);
							break;
						}
					}
					clear(tmp_path, tmp_path_i);
					clear(blank.path, blank.path_i);
					blank.start = tmp_blank;
					clear(tmp_path, tmp_path_i);
					blank.start.dist_from_end = 0;
					blank.start.dist_from_start = 0;

					AStar(blank.start, blank);
					cutPath(blank);
					Move();
					clear(blank.path, blank.path_i);
				}
				if (board->isNeighbor(blank.start.index, numeric.start.index))
					Swap(blank.start, numeric.start);
				numeric.start = board->cell[blank.end.index];

				i += 1;
			}
			clear(numeric.path, numeric.path_i);
		}

		initValues(curr_n);
		curr_n += 1;

	}
	n_swap = static_cast<int>(board->swap_queue.size());
    std::cout << std::endl;
	std::cout << "Complexity in time (Total number of states ever selected in the \"opened\" set) : " << total_cells_tmp_path << std::endl;
	std::cout << "Complexity in size (Maximum number of states ever represented in memory at the same time during the search) : " << max_cells_all_path << std::endl;
	std::cout << "Number of moves : " << n_swap / 2 << std::endl;
	board->swap_queue_status = READY;
}

bool
Algo::isChecked(int value)
{
	for (int i = 0; i < tmp_path_i; i++)
	{
		if (value == tmp_path[i].value)
			return (1);
	}
	return (0);
}

void
Algo::addN(Board::Cell *cell)
{
	int				i;
	bool			is_added = 0;

	if (isChecked(cell->value))
		return ;
	if (tmp_path_i == 0)
		pushBack(cell, tmp_path, tmp_path_i);
	else 
	{
		for (i = 0; i < tmp_path_i; i++)
		{
			if (cell->dist_from_start > tmp_path[i].dist_from_start)
				
			{
				insert(*cell, i);
				is_added = 1;
				break;
			}
		}
		if (is_added == 0)
			pushBack(cell, tmp_path, tmp_path_i);
	}
}

void
Algo::AStar(Board::Cell curr, starterPack &pack)
{
	if (curr.value == -1)
	{
		std::cout << "FAIL" << std::endl;
		exit(1);
	}
	Board::Cell *n; //= new Board::Cell;
	board->cell[curr.index].visited = 1;
	if (curr.index == pack.end.index)
		return;
	else
	{
		for (int i = 0; i < 4; i++)
		{
			if (curr.neighbor[i] > -1)
			{
				n = &(board->cell[curr.neighbor[i]]);
				if (board->cell[curr.neighbor[i]].visited == 0 && board->cell[curr.neighbor[i]].movable == 1)
				{
					board->cell[curr.neighbor[i]].dist_from_end = pack.start.dist_from_end + (this->*heuristic)(board->cell[curr.neighbor[i]], curr);
					board->cell[curr.neighbor[i]].dist_from_start = board->cell[curr.neighbor[i]].dist_from_end + (this->*heuristic)(board->cell[curr.neighbor[i]], pack.end); 
					addN(&(board->cell[curr.neighbor[i]]));

					total_cells_tmp_path += 1;
				}
			}
		}
		pushBack(&(tmp_path[tmp_path_i - 1]), pack.path, pack.path_i);
		if (tmp_path_i + pack.path_i > max_cells_all_path)
			max_cells_all_path = tmp_path_i + pack.path_i;
		popBack(tmp_path, tmp_path_i);
		return (AStar(pack.path[pack.path_i - 1], pack));
	}
}

void
Algo::printCell(Board::Cell c)
{
	std::cout << "value : " <<  c.value << std::endl;
	std::cout << "neighbor[0] : " << c.neighbor[0] << std::endl;
	std::cout << "neighbor[1] : " << c.neighbor[1] << std::endl;
	std::cout << "neighbor[2] : " << c.neighbor[2] << std::endl;
	std::cout << "neighbor[3] : " << c.neighbor[3] << std::endl;
	std::cout << "index  : " << c.index << std::endl;
	std::cout << "x : " << c.x << std::endl;
	std::cout << "y : " << c.y << std::endl;
	std::cout << "dist_from_start : " << c.dist_from_start << std::endl;
	std::cout << "dist_from_end : " << c.dist_from_end << std::endl;
	std::cout << "movable : " << c.movable << std::endl;
	std::cout << "visited : " << c.visited << std::endl;
}

double
Algo::ManhattanDist(Board::Cell start, Board::Cell end)
{
	return (abs(end.x - start.x) + abs(end.y - start.y));	
}

double
Algo::ManhattanDist2(Board::Cell start, Board::Cell end)
{
	return (abs(end.x - start.x + end.y - start.y));	
}

double
Algo::MatrixDist(Board::Cell start, Board::Cell end)
{
	return (abs(end.x - start.x) * abs(end.y - start.y));
}

double
Algo::HammingDist(Board::Cell start, Board::Cell end)
{
	return (abs((end.y - start.y) + abs(end.x - start.x)));	
}

double
Algo::PermutationsDist(Board::Cell start, Board::Cell end)
{
	return (abs(end.y - start.x + end.x - start.y));	
}

bool
Algo::canSolveArray(std::vector<int> array, int row)
{
	int		inversions = 0;

	for (int i = 0; i < static_cast<int>(array.size()); i++)
	{
		for	(int j = i + 1; j < static_cast<int>(array.size()); j++)
		{
			if (array[j] < array[i])
				inversions++;
		}
	}
	if (board->size_x % 2 == 1 && inversions % 2 == 0)
		return (1);
	else if (board->size_x % 2 == 0)
	{
		if ((row) % 2 == 0 && inversions % 2 == 1)
			return (1);
		if ((row) % 2 == 1 && inversions % 2 == 0)
			return (1);
	}
	return (0);
}

bool
Algo::isSolvable(void)
{
	int		i_x = 0;
	int		i_y = 0;
	std::vector<int>	index;
	std::vector<int>	values;
	int					index_row = 0;
	int					values_row = 0;

	for (i_y = 0; i_y < board->size_y; i_y++)
	{
		for (i_x = 0; i_x < board->size_x; i_x++)
		{
			if (board->cell[board->n_tab[i_y * board->size_y + i_x]].index < board->max)
				index.push_back(board->cell[board->n_tab[i_y * board->size_y + i_x]].index);
			else
				index_row = board->size_x - board->cell[board->n_tab[i_y * board->size_y + i_x]].y + 1;
			if (board->cell[board->n_tab[i_y * board->size_y + i_x]].value < board->max)
				values.push_back(board->cell[board->n_tab[i_y * board->size_y + i_x]].value);
			else
				values_row = board->size_x -  board->cell[board->n_tab[i_y * board->size_y + i_x]].y + 1;
		}
	}
	if (canSolveArray(index, index_row) == canSolveArray(values, values_row))
		return (1);
	return (0);
}

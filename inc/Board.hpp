#ifndef BOARD_HPP
# define BOARD_HPP

#include <iostream>
#include <queue>

enum swap_queue_status
{
	EMPTY,
	FILLING,
	UNSOLVABLE,
	READY
};

enum slove_status
{
	NO,
	YES
};

class Board
{

private:

	int		init_status = 0;
	int		_is_solved = NO;

public:

	struct Cell
	{
		int		value;
		int		neighbor[4];
		int		index;
		int		x;
		int		y;
		double	dist_from_start;
		double	dist_from_end;
		int		movable;
		int		visited;

		Cell();
		Cell(Cell const &);
		~Cell();

		Cell		&operator=(Cell const &);

	};

	Cell			*cell;
	Cell			*graph_cell;
	int				*n_tab;
	int				size_x;
	int				size_y;
	int				size;
	int				max;

	std::queue<int> swap_queue;
	int				swap_queue_status;

	Board(void);
	~Board(void);

	void			freeTabs(void);

	void			initSolved(int side_size);
	void			changeSize(int side_size);

	void			reset(void);
	void			resetCell(void);
	void			resetGraphCell(void);

	int				isItSolved(void);

	void			cellSwap(int index_a, int index_b);
	void			graphCellSwap(int index_a, int index_b);
	bool			isNeighbor(int cell_index, int possible_neighbor_index);
	bool			isValidCellSwap(int index_a, int index_b);
	void			replicateCell(void);
	void			validSwap(int index_a, int index_b);
	void			validShuffle(void);
	void			randomShuffle(int power = 5);

	void			findAndSwap(int direction);

	void			clearSwapList(void);
	void			findNeighborsAndPosition(void);

	int				getInitStatus(void) const;

	bool			cellCmp(Cell &cell1, Cell &cell2);
};

std::ostream		&operator<<(std::ostream &o, Board const &i);

#endif

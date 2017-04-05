#ifndef ALGO_HPP
# define ALGO_HPP

#include "Board.hpp"
#include <limits>

enum heuristic
{
	MANHATTAN1,
	MANHATTAN2,
	HAMMING,
	PERMUTATION,
	MATRIX
};

class Algo
{
public:
	Board				*board;

	typedef struct		s_starterPack
	{
		Board::Cell		start;
		Board::Cell		end;
		Board::Cell		*path;
		int				path_i;
	}					starterPack;

	starterPack			numeric;
	starterPack			blank;
	Board::Cell			*tmp_path;
	int					tmp_path_i;
	int					state;
	double					(Algo::*heuristic)(Board::Cell start, Board::Cell end);

	/*
	 *	Variables a afficher (sujet)
	 */
	int								total_cells_tmp_path = 0; // "Total number of states ever selected in the "opened" set (complexity in time)"
	int								max_cells_all_path = 0; // "Maximum number of states ever represented in memory at the same time during the search (complexity in size)"
	int								n_swap = 0; // number of moves (== board->swap_queue.size())

	Algo(void);
	Algo(Board *b);
	~Algo(void);

	/*
	 *	Initialize
	 */
	void		setBoard(Board *b);
	void		setHeuristic(int h);
	void		initValues(int curr_n);

	/*
	 *	Heuristics
	 */
	double			ManhattanDist(Board::Cell start, Board::Cell end);
	double			ManhattanDist2(Board::Cell start, Board::Cell end);
	double			HammingDist(Board::Cell start, Board::Cell end);
	double			PermutationsDist(Board::Cell start, Board::Cell end);
	double			MatrixDist(Board::Cell start, Board::Cell end);

	/*
	 *	Solvable
	 */
	bool		canSolveArray(std::vector<int> array, int row);
	bool		isSolvable(void);

	/*
	 * 	Path finding
	 */
	bool		isChecked(int value);
	void		cutPath(starterPack &pack);
	void		addN(Board::Cell *cell);
	void		AStar(Board::Cell, starterPack &);

	/*
	 *	Moves
	 */
	void		key1(void);
	void		key2(void);
	void		Swap(Board::Cell &c1, Board::Cell &c2);
	void		Move(void);

	/*
	 *	Resolution
	 */
	Board::Cell	findPerValue(int curr_n);
	bool		isResolved(void);
	bool		canMove(Board::Cell cell);
	void		resolve(void);

	/*
	 *	Array fctions
	 */
	void		initCell(Board::Cell &cell);
	void		clear(Board::Cell *array, int &max);
	void		insert(Board::Cell cell, int place);
	void		pushBack(Board::Cell *cell, Board::Cell *array, int &max);
	void		popBack(Board::Cell *array, int &max);
	void		erase(int place, Board::Cell *array, int &max);
	void		eraseFromBegin(int place, Board::Cell *array, int &max);

	/*
	 *	Debug
	 */
	void		printCell(Board::Cell c);
	void		printBoard();
};

#endif

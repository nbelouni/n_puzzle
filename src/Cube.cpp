#include "Cube.hpp"

Cube::Cube(void)
{
    return;
}

Cube::~Cube(void)
{
    return;
}

void
Cube::setDefaultValues(void)
{
    move_speed = 0.02f;
    move_counter = 0.0f;
    space_btw_cube = 1.0f;
}

void
Cube::init(Board *board)
{
    this->board = board;
    matrix_array = new CubeMat[board->size + 1];
    setDefaultValues();
}

void
Cube::increaseMoveSpeed(void)
{
    if (move_speed < 0.49f)
        move_speed += 0.01f;
}

void
Cube::decreaseMoveSpeed(void)
{
    if (move_speed > 0.02f)
        move_speed -= 0.01f;
}

void
Cube::initMatrixArray(void)
{
    float adjust = (board->size_x - 1.0f) / 2.0f;
    int cur_cube;

    for (int i = 0; i < board->size; i++)
    {
        cur_cube = board->graph_cell[i].value;
        matrix_array[cur_cube].matrix.setIdentity();
        matrix_array[cur_cube].init_transf.setIdentity();
        matrix_array[cur_cube].anim_transf.setIdentity();
        matrix_array[cur_cube].move.setIdentity();
        matrix_array[cur_cube].init_transf.translate(
                board->graph_cell[i].x * space_btw_cube,
                board->graph_cell[i].y * -space_btw_cube,
                0.0f);
        matrix_array[cur_cube].init_transf.translate(
                -adjust * space_btw_cube,
                adjust * space_btw_cube,
                0.0f);
    }
}

void
Cube::findAndApplyMoveDirection(void)
{
    int x;
    int y;

    x = board->graph_cell[current_swap[0]].x - board->graph_cell[current_swap[1]].x;
    y = board->graph_cell[current_swap[0]].y - board->graph_cell[current_swap[1]].y;

    if (x == 0 && y == -1)
    {
/*		std::cout 	<< "Index: " <<  current_swap[0] << " go down"
					<< ", "
					<< "Index: " <<  current_swap[1] << " go up"
					<< std::endl;
*/        matrix_array[board->graph_cell[current_swap[0]].value].move.translate(0.0f, -move_speed, 0.0f);
        matrix_array[board->graph_cell[current_swap[1]].value].move.translate(0.0f, move_speed, 0.0f);
    } else if (x == 0 && y == 1)
    {
/*		std::cout 	<< "Index: " <<  current_swap[0] << " go up"
					<< ", "
					<< "Index: " <<  current_swap[1] << " go down"
					<< std::endl;
*/        matrix_array[board->graph_cell[current_swap[0]].value].move.translate(0.0f, move_speed, 0.0f);
        matrix_array[board->graph_cell[current_swap[1]].value].move.translate(0.0f, -move_speed, 0.0f);
    } else if (x == -1 && y == 0)
    {
/*		std::cout 	<< "Index: " <<  current_swap[0] << " go right"
					<< ", "
					<< "Index: " <<  current_swap[1] << " go left"
					<< std::endl;
*/        matrix_array[board->graph_cell[current_swap[0]].value].move.translate(move_speed, 0.0f, 0.0f);
        matrix_array[board->graph_cell[current_swap[1]].value].move.translate(-move_speed, 0.0f, 0.0f);
    } else if (x == 1 && y == 0)
    {
/*		std::cout 	<< "Index: " <<  current_swap[0] << " go left"
					<< ", "
					<< "Index: " <<  current_swap[1] << " go right"
					<< std::endl;
*/        matrix_array[board->graph_cell[current_swap[0]].value].move.translate(-move_speed, 0.0f, 0.0f);
        matrix_array[board->graph_cell[current_swap[1]].value].move.translate(move_speed, 0.0f, 0.0f);
    } else
    {
        std::cout << "WTF?" << std::endl;
    }
}

int
Cube::createMoveAnimationFromSwapList(void)
{
    if (move_counter == 0.0f)
    {
        if (!(board->swap_queue.empty()))
        {
            current_swap[0] = board->swap_queue.front();
            board->swap_queue.pop();
            current_swap[1] = board->swap_queue.front();
            board->swap_queue.pop();
            findAndApplyMoveDirection();
            move_counter += move_speed;
        }
        else
        {
            board->swap_queue_status = EMPTY;
            return (4);
        }
    }
    return (2);
}

float
Cube::applyTransformation(void)
{
    if (move_counter >= move_speed)
    {
        move_counter += move_speed;
    }
    if (move_counter >= space_btw_cube)
    {
        move_counter = 0.0f;
        board->graphCellSwap(current_swap[0], current_swap[1]);
        swapInitTransformation(board->graph_cell[current_swap[0]].value, board->graph_cell[current_swap[1]].value);
        matrix_array[board->graph_cell[current_swap[0]].value].matrix.setIdentity();
        matrix_array[board->graph_cell[current_swap[0]].value].anim_transf.setIdentity();
        matrix_array[board->graph_cell[current_swap[0]].value].move.setIdentity();

        matrix_array[board->graph_cell[current_swap[1]].value].matrix.setIdentity();
        matrix_array[board->graph_cell[current_swap[1]].value].anim_transf.setIdentity();
        matrix_array[board->graph_cell[current_swap[1]].value].move.setIdentity();
    }
    for (int i = 0; i < board->size; i++)
    {
        matrix_array[i].matrix.setIdentity();
        matrix_array[i].matrix.multiply(matrix_array[i].init_transf);
        matrix_array[i].anim_transf.multiply(matrix_array[i].move);
        matrix_array[i].matrix.multiply(matrix_array[i].anim_transf);
    }
    return (move_counter);
}

int
Cube::hyperSpeed(int power)
{
    for (int i = 0; i < power; i++)
    {
        if (!(board->swap_queue.empty()))
        {
            current_swap[0] = board->swap_queue.front();
            board->swap_queue.pop();
            current_swap[1] = board->swap_queue.front();
            board->swap_queue.pop();
            board->graphCellSwap(current_swap[0], current_swap[1]);
            swapInitTransformation(board->graph_cell[current_swap[0]].value,
                                   board->graph_cell[current_swap[1]].value);
        }
    }
    for (int i = 0; i < board->size; i++)
    {
        matrix_array[i].matrix.setIdentity();
        matrix_array[i].matrix.multiply(matrix_array[i].init_transf);
        matrix_array[i].anim_transf.multiply(matrix_array[i].move);
        matrix_array[i].matrix.multiply(matrix_array[i].anim_transf);
    }
    if (board->swap_queue.empty())
    {
        board->swap_queue_status = EMPTY;
        return (4);
    }
    return (2);
}

void
Cube::swapInitTransformation(int ia, int ib)
{
    if (ia >= 0 && ib >= 0 && ia <= board->size && ib <= board->size)
    {
        Mat4 <GLfloat> tmp;

        tmp = matrix_array[ia].init_transf;
        matrix_array[ia].init_transf = matrix_array[ib].init_transf;
        matrix_array[ib].init_transf = tmp;
    }
}

void
Cube::reset(void)
{
    setDefaultValues();
    initMatrixArray();
    applyTransformation();
}

void
Cube::generateModel(void)
{
    model.v_num_elem = 8;
    model.f_num_elem = 36;
    model.n_num_elem = 8;
    model.v_array = new Vec3<GLfloat>[model.v_num_elem];
    model.f_array = new GLubyte[model.f_num_elem];
    model.n_array = new Vec3<GLfloat>[model.v_num_elem];
    model.v_mem_size = sizeof(GLfloat) * 3 * model.v_num_elem;
    model.f_mem_size = sizeof(GLubyte) * model.f_num_elem;
    model.n_mem_size = sizeof(GLfloat) * 3 * model.v_num_elem;

    //////////////////////////////////////////////////////////////////
    //                                           glFrontFace(GL_CW);//
    //          4----5		        ^  Y	                     	//
    //         /|   /|              |                     .------.  //
    //       0----1  |              |                    /       |  //
    //       |  7-|--6              |----->  X             .->   )  //
    //       |/   | /              /                      |     /   //
    //       3____2               /                        \___/    //
    //                          |,  Z                               //
    //                                           DEFAULT is GL_CCW  //
    //////////////////////////////////////////////////////////////////

    model.v_array[0] = {-0.5f, 0.5f, 0.5f};
    model.v_array[1] = {0.5f, 0.5f, 0.5f};
    model.v_array[2] = {0.5f, -0.5f, 0.5f};
    model.v_array[3] = {-0.5f, -0.5f, 0.5f};
    model.v_array[4] = {-0.5f, 0.5f, -0.5f};
    model.v_array[5] = {0.5f, 0.5f, -0.5f};
    model.v_array[6] = {0.5f, -0.5f, -0.5f};
    model.v_array[7] = {-0.5f, -0.5f, -0.5f};

    model.n_array[0] = {-0.57735f, 0.57735f, 0.57735f};
    model.n_array[1] = {0.57735f, 0.57735f, 0.57735f};
    model.n_array[2] = {0.57735f, -0.57735f, 0.57735f};
    model.n_array[3] = {-0.57735f, -0.57735f, 0.57735f};
    model.n_array[4] = {-0.57735f, 0.57735f, -0.57735f};
    model.n_array[5] = {0.57735f, 0.57735f, -0.57735f};
    model.n_array[6] = {0.57735f, -0.57735f, -0.57735f};
    model.n_array[7] = {-0.57735f, -0.57735f, -0.57735f};

    model.f_array[0] = 1;
    model.f_array[1] = 0;
    model.f_array[2] = 3;

    model.f_array[3] = 1;
    model.f_array[4] = 3;
    model.f_array[5] = 2;

    model.f_array[6] = 5;
    model.f_array[7] = 1;
    model.f_array[8] = 2;

    model.f_array[9] = 5;
    model.f_array[10] = 2;
    model.f_array[11] = 6;

    model.f_array[12] = 4;
    model.f_array[13] = 5;
    model.f_array[14] = 6;

    model.f_array[15] = 4;
    model.f_array[16] = 6;
    model.f_array[17] = 7;

    model.f_array[18] = 0;
    model.f_array[19] = 4;
    model.f_array[20] = 7;

    model.f_array[21] = 0;
    model.f_array[22] = 7;
    model.f_array[23] = 3;

    model.f_array[24] = 5;
    model.f_array[25] = 4;
    model.f_array[26] = 0;

    model.f_array[27] = 5;
    model.f_array[28] = 0;
    model.f_array[29] = 1;

    model.f_array[30] = 2;
    model.f_array[31] = 3;
    model.f_array[32] = 7;

    model.f_array[33] = 2;
    model.f_array[34] = 7;
    model.f_array[35] = 6;
}

/**
 *
 * @file    main.c
 * @author  Eric Rock
 * @date    Jan 12, 2016
 * @brief   sbp, A sliding block puzzle solver.
 *
 */

// stat
#include <sys/stat.h>

// open
#include <fcntl.h>

// XXX ?
#include <unistd.h>

// errno
#include <errno.h>

// PATH_MAX
#include <limits.h>

// fabs
#include <math.h>

// gettimeofday, timeval
#include <sys/time.h>

///
/// SK Library Includes
///

// bool
#include "sk_types/sk_types.h"

// parse_double, parse_int
#include "sk_types/sk_types_parse.h"

// str_concat
#include "sk_types/sk_types_concat.h"

// sk_str
#include "sk_str/sk_str.h"

// str_compare
#include "sk_str/c_str.h"

// ALLOC
#include "utils.h"

// Printer
#include "printer.h"

// interpret_keyword_args
#include "sk_clargs.h"

// argv_const_begin, null_terminated_array_const_begin
#include "sk_iterator_utils.h"

// sk_list, sk_list_append, sk_list_size, sk_list_remove
#include "sk_list/sk_list.h"

// sk_vector, sk_vector_init, sk_vector_add, sk_vector_destroy
#include "sk_vector/sk_vector.h"

// sk_random
#include "sk_random/sk_random.h"

// sk_random_default_init
#include "sk_random/sk_random_default.h"



// Program Debug Message Severities
#define DEBUG_ALL           5
#define DEBUG_DETAILS       4
#define DEBUG_EVENTS        3
#define DEBUG_HIGH_LEVEL    2
#define DEBUG_NONE          1


struct global_state;
typedef struct global_state global_state;

struct board_state;
typedef struct board_state board_state;

enum direction;
typedef enum direction direction;

struct move;
typedef struct move move;

struct global_state
{
    printer_t *printer;

    sk_random random;

    const char *input_file_path;    // Default file path for input file
    sk_str *resolved_path;          // Path to resolved input file

    bool random_walk;
    UINT_64 N;
    bool depth_first;
    bool breadth_first;

    board_state *game_state;
};

struct board_state
{
    UINT_64 width;
    UINT_64 height;

    SINT_64 **tiles;
};

enum direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

struct move
{
    SINT_64 piece;
    direction dir;
};

#define GOAL    -1
#define CLEAR    0
#define WALL     1
#define MASTER   2

global_state state;


/**
 * @brief                   Loads the game state from file with given filename
 *
 * @param[in] file_name     - Path to file to load.
 *
 * @return                  True if game state was successfully loaded.
 */
bool loadGameState(const char *file_name);

/**
 * @brief                   Prints the global game state to the screen.
 */
void outputGameState();

/**
 * @brief                   Clones game state \c source into \c dest.
 *
 * @param[in] source        - State to clone
 * @param[out] dest         - State to populate with clone
 *
 * @return                  true if clone succeeded.
 */
bool cloneGameState(board_state *source, board_state *dest);

/**
 * @brief                   Determines whether the current game state is solved.
 *
 * @param[in] source        - State to check
 *
 * @return                  true if board state is solved.
 */
bool gameStateSolved(board_state *source);

/**
 * @brief                   Given a board state \c source and a piece \c piece, populates \c moves
 *                              with all possible moves for \c piece.
 *
 * @param[in] source        - Board state to check
 * @param[in] piece         - Piece to observe
 * @param[out] moves        - Pointer to list to construct and populate
 */
void allMovesHelp(board_state *source, SINT_64 piece, sk_list *moves);

/**
 * @brief                   Given a board state \c source, populates \c moves with all
 *                                  possible moves.
 *
 * @param[in] source        - Board state to check
 * @param[out] moves        - Pointer to list to construct and populate
 */
void allMoves(board_state *source, sk_list *moves);

/**
 * @brief                   Given a board state \c source, applies \c next_move to state in place.
 *
 * @param[in] source        - Board state to apply move to
 * @param[in] next_move     - Move to apply
 */
void applyMove(board_state *source, move next_move);

/**
 * @brief                   Given a board state \c source, applies \c next move to
 *                              a clone of \c source, storing new state into \c dest.
 *
 * @param[in] source        - State to apply move to
 * @param[in] next_move     - Move to apply
 * @param[out] dest         - State to populate with cloned new state
 */
void applyMoveCloning(board_state *source, move next_move, board_state *dest);

/**
 * @brief                   Determines whether state \c a is exactly equal to state \c b
 *
 * @param[in] a             - State to compare
 * @param[in] b             - State to compare to
 *
 * @return                  true if both states are exactly equal.
 */
bool stateEqual(board_state *a, board_state *b);

/**
 * @brief                   Given board state \c source, refactors state internals into
 *                              equivalent normal form.
 *
 * @param[in] source        - State to refactor
 */
void normalizeState(board_state *source);

void destroy_board_state(void *p);

/**
 * @brief                   Applies up to \c N random moves to \c source, halting if the
 *                              board is solved after any move.
 *
 * @param[in] source        - Starting board state
 * @param[in] N             - Maximum number of moves to apply.
 */
void randomWalks(board_state *source, UINT_64 N);

/**
 * @brief                   Searches for a solution to the given board state using a depth-first strategy.
 *
 * @param[in] source        - Board state to solve
 * @param[out] states       - List of board states traversed in this branch
 * @param[out] soln         - List to populate with solution moves
 * @param[out] nodes_visited- Number of nodes visited in the search
 *
 * @return                  true if the sequence of moves up to this point leads to a solution.
 *                              If a solution is found, the sequence of moves leading to solution will be stored in \c soln.
 */
bool uninformedDepthFirst(board_state *source, sk_list *closed, sk_list *soln, UINT_64 *nodes_visited);

/**
 * @brief                   Searches for a solution to the given board state using a breadth-first strategy.
 *
 * @param[in] source        - Board state to solve
 * @param[out] closed       - List of board states traversed
 * @param[in] open          - Instantiated list of states to populate with the frontier
 * @param[out] soln         - List to populate with solution moves
 * @param[out] nodes_visited- Number of nodes visited in the search
 *
 * @return                  true if the sequence of moves up to this point leads to a solution.
 *                              If a solution is found, the sequence of moves leading to solution will be stored in \c soln.
 */
bool uninformedBreadthFirst(board_state *source, sk_list *closed, sk_list *open, sk_list *soln, UINT_64 *nodes_visited);

/**
 * @brief                       Debugging print function.
 *                                  Prints output to standard error if debug level is high enough.
 *
 * @param[in] out               - Printer structure containing open file descriptors
 * @param[in] level             - the level of detail of the message
 * @param[in] fmt               - Format string to pass to printf
 * @param[in] va_args           - Replacement pointers to pass to printf
 */
void app_debug(printer_t *out, int level, char *fmt, ...);

/**
 * @brief                       Keyword "f" handler.
 *                                  Arguments: 1
 *                                  args[1] :
 *
 * @param[in] match             - Input argument string which matches registered keyword
 * @param[in] args              - Arguments following the keyword
 * @param[in] handle            - Handle to common state
 */
void handle_f(sk_str *match, sk_str **args, void *handle);

/**
 * @brief                       Keyword "r" handler.
 *                                  Arguments: 1
 *                                  args[1] :
 *
 * @param[in] match             - Input argument string which matches registered keyword
 * @param[in] args              - Arguments following the keyword
 * @param[in] handle            - Handle to common state
 */
void handle_r(sk_str *match, sk_str **args, void *handle);

/**
 * @brief                       Keyword "d" handler.
 *                                  Arguments: 1
 *                                  args[1] :
 *
 * @param[in] match             - Input argument string which matches registered keyword
 * @param[in] args              - Arguments following the keyword
 * @param[in] handle            - Handle to common state
 */
void handle_d(sk_str *match, sk_str **args, void *handle);

/**
 * @brief                       Keyword "b" handler.
 *                                  Arguments: 1
 *                                  args[1] :
 *
 * @param[in] match             - Input argument string which matches registered keyword
 * @param[in] args              - Arguments following the keyword
 * @param[in] handle            - Handle to common state
 */
void handle_b(sk_str *match, sk_str **args, void *handle);

/**
 * @brief                       Keyword "verbose" handler.
 *                                  Arguments: 0
 *                                  Raises the debug level of the program to maximum.
 *
 * @param[in] match             - Input argument string which matches registered keyword
 * @param[in] args              - Arguments following the keyword
 * @param[in] handle            - Handle to common state
 */
void handle_verbose(sk_str *match, sk_str **args, void *handle);

/**
 * @brief                       Keyword miss handler.
 *                                  Handles arguments not matching any registered keyword.
 *
 * @param[in] match             - Input argument string which matches registered keyword
 * @param[in] args              - Arguments following the keyword
 * @param[in] handle            - Handle to common state
 */
void handle_miss(sk_str *match, sk_str **args, void *handle);

/**
 * @brief                       Parses the input argument array for configuration parameters.
 *
 * @param[in] state             - State to update with configuration
 * @param[in] argc              - Number of command line arguments
 * @param[in] argv              - Argument array
 *
 * @return                      true if the argument vector was successfully processed.
 */
bool interpret_input_arguments(global_state *state, int argc, char **argv);

/**
 * @brief                       Validates the global state against program assumptions.
 *
 * @param[in] state             - State to check.
 *
 * @return                      true if the passed state is valid for program execution.
 */
bool validate_global_state(global_state *state);

/**
 * @brief                       Performs memory cleanup on global state \c p.
 *
 * @param[in] p                 - Pointer to destroy
 */
void destroy_global_state(void *p);

/**
 * @brief                   Attempts to expand \c filename into an absolute path.
 *                              Checks for the existence of a file at that path.
 *
 * @param[in] filename      - a filename to search for.
 *                              Can either be an absolute path or a single filename.
 * @param[out] resolved     - a reference into which to store the absolute path to the found file.
 *
 * @return                  true if a matching file was found
 */
bool resolve_input_file(const char *filename, sk_str **resolved);

/**
 * @brief                   File existential query function.
 *
 * @param path              - a path to check for file existence over.
 *
 * @returns                 true if a file exists that is represented by the given path
 */
bool exists(sk_str *path);

/**
 * @brief                   Reliably reads \c n bytes from file \c fd into buffer \c dst.
 *
 * @param[in] fd            - file descriptor of file to read.
 * @param[out] dst          - destination buffer
 * @param[in] n             - number of bytes to read in
 *
 * @return                  the number of bytes successfully read.
 *                          -1 on error.
 */
SINT_64 r_fread(int fd, void *dst, UINT_64 n);

/**
 * @brief                   Retrieves a string from the given file.
 *                              Input halts at newline, and skips over '\0' characters.
 *
 * @param[out] s            - Destination buffer
 * @param[in] n             - Max number of characters to read in
 * @param[in] fd            - file descriptor of file to read
 *
 * @return                  The number of characters read.
 */
UINT_64 _ngets(char *s, UINT_64 n, int fd);

/**
 * @brief                   Retrieves a line from the given file.
 *
 * @param[in] fd            - File descriptor of file to read.
 * @param[out] dest         - Destination string.
 *
 * @return                  true if a line was able to be retrieved from the file.
 */
bool fd_get_line(int fd, sk_str *dest);

int main (int argc, char **argv)
{
    printer_t *printer = Printer();
    printer->debug = app_debug;
    printer->debug_level = DEBUG_NONE;

    sk_random_default_init(&state.random);

    int retval = EXIT_SUCCESS;

    state.printer = printer;
    state.input_file_path = "./assets/SBP-level0.txt";
    state.resolved_path = NULL;
    state.random_walk = false;
    state.breadth_first = false;
    state.depth_first = false;
    state.game_state = NULL;


    // Step 0: Retrieve program configuration from command line
    if (!interpret_input_arguments(&state, argc, argv))
    {
        state.printer->error(state.printer, "Failed to interpret input arguments. Aborting.\n");
        retval = EXIT_FAILURE;
        goto cleanup;
    }

    // Step 0: Ensure that program configuration is valid.
    if (!validate_global_state(&state))
    {
        state.printer->error(state.printer, "Program configuaration invalid. Aborting.\n");
        retval = EXIT_FAILURE;
        goto cleanup;
    }

    loadGameState(state.resolved_path->string);
    normalizeState(state.game_state);

    if (state.random_walk)
    {
        randomWalks(state.game_state, state.N);
    }
    else if (state.depth_first)
    {
        sk_list soln;
        sk_list states;
        UINT_64 nodes = 0;
        UINT_64 soln_size = 0;
        sk_list_init(&soln, NULL);
        sk_list_init(&states, NULL);

        struct timeval start, stop;
        gettimeofday(&start, NULL);
        if (!uninformedDepthFirst(state.game_state, &states, &soln, &nodes))
        {
            printf("No solution found!\n");
        }
        else
        {
            gettimeofday(&stop, NULL);
            soln_size = sk_list_size(&soln);

            outputGameState();

            move *next_move;
            sk_iterator it;
            sk_list_begin(&it, &soln);
            while (it.has_next(&it))
            {
                next_move = it.next(&it);
                applyMove(state.game_state, *next_move);
                switch (next_move->dir)
                {
                case UP:
                    printf("(%ld, up)\n", next_move->piece);
                    break;
                case DOWN:
                    printf("(%ld, down)\n", next_move->piece);
                    break;
                case LEFT:
                    printf("(%ld, left)\n", next_move->piece);
                    break;
                case RIGHT:
                    printf("(%ld, right)\n", next_move->piece);
                    break;
                }

                free(next_move);
                sk_list_remove(&it);
            }
            it.destroy(&it);

            printf("\n");
            outputGameState();
            printf("\n");

            printf("Nodes Visited: %lu\n", nodes);
            printf("Search time = %0.4f s\n",
                    (float)(stop.tv_sec - start.tv_sec + (stop.tv_usec - start.tv_usec)/(float)1000000));
            printf("Solution Size: %lu moves\n", soln_size);
        }

        sk_list_destroy(&states);
        sk_list_destroy(&soln);
    }
    else if (state.breadth_first)
    {
        sk_list soln;
        sk_list closed;
        sk_list open;
        UINT_64 nodes = 0;
        UINT_64 soln_size = 0;
        sk_list_init(&soln, NULL);
        sk_list_init(&closed, NULL);
        sk_list_init(&open, NULL);

        struct timeval start, stop;
        gettimeofday(&start, NULL);
        if (!uninformedBreadthFirst(state.game_state, &closed, &open, &soln, &nodes))
        {
            printf("No solution found!\n");
        }
        else
        {
            gettimeofday(&stop, NULL);
            soln_size = sk_list_size(&soln);

            outputGameState();

            move *next_move;
            sk_iterator it;
            sk_list_begin(&it, &soln);
            while (it.has_next(&it))
            {
                next_move = it.next(&it);
                applyMove(state.game_state, *next_move);
                switch (next_move->dir)
                {
                case UP:
                    printf("(%ld, up)\n", next_move->piece);
                    break;
                case DOWN:
                    printf("(%ld, down)\n", next_move->piece);
                    break;
                case LEFT:
                    printf("(%ld, left)\n", next_move->piece);
                    break;
                case RIGHT:
                    printf("(%ld, right)\n", next_move->piece);
                    break;
                }

                free(next_move);
                sk_list_remove(&it);
            }
            it.destroy(&it);

            printf("\n");
            outputGameState();
            printf("\n");

            printf("Nodes Visited: %lu\n", nodes);
            printf("Search time = %0.4f s\n",
                    (float)(stop.tv_sec - start.tv_sec + (stop.tv_usec - start.tv_usec)/(float)1000000));
            printf("Solution Size: %lu moves\n", soln_size);

            sk_list_begin(&it, &closed);
            while (it.has_next(&it))
            {
                board_state *next = it.next(&it);
                destroy_board_state(next);
                free(next);
                sk_list_remove(&it);
            }
            it.destroy(&it);
        }

        sk_list_destroy(&closed);
        sk_list_destroy(&open);
        sk_list_destroy(&soln);
    }
    else
    {
        randomWalks(state.game_state, 3);
    }

cleanup:
    destroy_printer(printer);
    destroy_global_state(&state);

    exit(retval);
}

bool loadGameState(const char *file_name)
{
    bool retval = true;

    sk_str *resolved_path = NULL;
    if (!resolve_input_file(file_name, &resolved_path))
    {
        state.printer->error(state.printer, "Error: Failed to resolve input file path : %s.\n", file_name);
        retval = false;
        goto resolve_file_fail;
    }

    int input_fd = 0;
    if (resolved_path && (input_fd = open(resolved_path->string, O_RDONLY)) < 0)
    {
        state.printer->error(state.printer, "Error: Failed to open input file : %s.\n", strerror(errno));
        retval = false;
        goto open_fail;
    }

    UINT_64 width;
    UINT_64 height;

    sk_iterator token_it;
    char *token;
    sk_str str_token;
    sk_str line;

    if (!fd_get_line(input_fd, &line))
    {
        retval = false;
        goto dimensions_fail;
    }

    sk_str_split(&token_it, &line, ',');
    int t_width;
    int t_height;

    token = token_it.next(&token_it);
    sk_str_init(&str_token, token, 10);
    parse_int(&str_token, 10, &t_width);
    sk_str_destroy(&str_token);

    token = token_it.next(&token_it);
    sk_str_init(&str_token, token, 10);
    parse_int(&str_token, 10, &t_height);
    sk_str_destroy(&str_token);

    state.printer->debug(state.printer, DEBUG_DETAILS,
                            "Found width %d  height %d.\n",
                            t_width,
                            t_height);

    token_it.destroy(&token_it);

    sk_str_destroy(&line);

    if (t_width <= 0 || t_height <= 0)
    {
        goto dimensions_fail;
    }
    width = t_width;
    height = t_height;

    SINT_64 **tiles = ALLOC(*tiles, height);
    UINT_64 i;
    for (i = 0; i < height; ++i)
    {
        tiles[i] = ALLOC(*(tiles[i]), width);
    }

    UINT_64 j;
    for (i = 0; i < height; i++)
    {
        if (!fd_get_line(input_fd, &line))
        {
            goto matrix_fill_fail;
        }

        sk_str_split(&token_it, &line, ',');

        for (j = 0; j < width; ++j)
        {
            if (!token_it.has_next(&token_it))
            {
                token_it.destroy(&token_it);
                sk_str_destroy(&line);
                retval = false;
                goto matrix_fill_fail;
            }
            int val;
            token = token_it.next(&token_it);
            sk_str_init(&str_token, token, 10);
            parse_int(&str_token, 10, &val);
            sk_str_destroy(&str_token);
            tiles[i][j] = val;
        }

        token_it.destroy(&token_it);

        sk_str_destroy(&line);
    }

    state.game_state = ALLOC(*(state.game_state), 1);
    state.game_state->width = width;
    state.game_state->height = height;
    state.game_state->tiles = tiles;

    sk_str_destroy(resolved_path);
    free(resolved_path);
    close(input_fd);
    return retval;

matrix_fill_fail:

    for (i = 0; i < height; ++i)
    {
        free(tiles[i]);
    }
    free(tiles);
dimensions_fail:

    if (input_fd > 0)
    {
        close(input_fd);
    }
open_fail:

    sk_str_destroy(resolved_path);
    free(resolved_path);
resolve_file_fail:

    return retval;
}

void outputGameState()
{
    if (!state.game_state)
    {
        return;
    }

    printf("%lu,%lu,\n",
            state.game_state->width,
            state.game_state->height);

    UINT_64 i;
    UINT_64 j;
    for (i = 0; i < state.game_state->height; ++i)
    {
        for (j = 0; j < state.game_state->width; ++j)
        {
            printf("%ld,", state.game_state->tiles[i][j]);
        }

        printf("\n");
    }
}

void printGameState(board_state *game_state)
{
    if (!game_state)
    {
        return;
    }

    state.printer->debug(state.printer, DEBUG_DETAILS,
                        "%lu,%lu,\n",
                        game_state->width,
                        game_state->height);

    char *buffer = ALLOC(*buffer, game_state->width * 10);
    char *curr = buffer;


    UINT_64 i;
    UINT_64 j;
    for (i = 0; i < game_state->height; ++i)
    {
        curr = buffer;
        for (j = 0; j < game_state->width; ++j)
        {
            curr += sprintf(curr, "%ld,", game_state->tiles[i][j]);
        }

        state.printer->debug(state.printer, DEBUG_DETAILS,
                            "%s\n",
                            buffer);
    }

    free(buffer);
}

bool cloneGameState(board_state *source, board_state *dest)
{
    if (!source || !dest)
    {
        return false;
    }

    dest->width = source->width;
    dest->height = source->height;
    dest->tiles = ALLOC(*(dest->tiles), dest->height);

    UINT_64 i, j;
    for (i = 0; i < dest->height; ++i)
    {
        dest->tiles[i] = ALLOC(*(dest->tiles[i]), dest->width);
        for (j = 0; j < dest->width; ++j)
        {
            dest->tiles[i][j] = source->tiles[i][j];
        }
    }
    return true;
}

bool gameStateSolved(board_state *source)
{
    if (!source)
    {
        return false;
    }

    UINT_64 i, j;
    for (i = 0; i < source->height; ++i)
    {
        for (j = 0; j < source->width; ++j)
        {
            if (source->tiles[i][j] == -1)
            {
                return false;
            }
        }
    }
    return true;
}

void allMovesHelp(board_state *source, SINT_64 piece, sk_list *moves)
{
    if (!moves)
    {
        return;
    }

    sk_list_init(moves, NULL);

    if (!source)
    {
        return;
    }

    UINT_64 i, j;
    bool move_possible = true;

    // UP
    for (i = 0; i < source->height; ++i)
    {
        for (j = 0; j < source->width; ++j)
        {
            if (source->tiles[i][j] == piece)
            {
                if ( !(  CLEAR == source->tiles[i - 1][j]
                      || piece == source->tiles[i - 1][j]
                      || (  MASTER == piece
                         && GOAL == source->tiles[i - 1][j]
                         )
                      )
                   )
                {
                    move_possible = false;
                }
            }
        }
    }

    if (move_possible)
    {
        move *next_move = ALLOC(*next_move, 1);
        next_move->piece = piece;
        next_move->dir = UP;
        sk_list_append(moves, next_move);
    }

    // DOWN
    move_possible = true;

    for (i = source->height - 1; /*i >= 0*/; --i)
    {
        for (j = 0; j < source->width; ++j)
        {
            if (source->tiles[i][j] == piece)
            {
                if ( !(  CLEAR == source->tiles[i + 1][j]
                      || piece == source->tiles[i + 1][j]
                      || (  MASTER == piece
                         && GOAL == source->tiles[i + 1][j]
                         )
                      )
                   )
                {
                    move_possible = false;
                }
            }
        }

        // Avoid unsigned infinite loop
        if (i == 0)
        {
            break;
        }
    }

    if (move_possible)
    {
        move *next_move = ALLOC(*next_move, 1);
        next_move->piece = piece;
        next_move->dir = DOWN;
        sk_list_append(moves, next_move);
    }

    // LEFT
    move_possible = true;

    for (i = 0; i < source->height; ++i)
    {
        for (j = 0; j < source->width; ++j)
        {
            if (source->tiles[i][j] == piece)
            {
                if ( !(  CLEAR == source->tiles[i][j - 1]
                      || piece == source->tiles[i][j - 1]
                      || (  MASTER == piece
                         && GOAL == source->tiles[i][j - 1]
                         )
                      )
                   )
                {
                    move_possible = false;
                }
            }
        }
    }

    if (move_possible)
    {
        move *next_move = ALLOC(*next_move, 1);
        next_move->piece = piece;
        next_move->dir = LEFT;
        sk_list_append(moves, next_move);
    }

    // RIGHT
    move_possible = true;

    for (i = 0; i < source->height; ++i)
    {
        for (j = source->width - 1; /*j >= 0*/; --j)
        {
            if (source->tiles[i][j] == piece)
            {
                if ( !(  CLEAR == source->tiles[i][j + 1]
                      || piece == source->tiles[i][j + 1]
                      || (  MASTER == piece
                         && GOAL == source->tiles[i][j + 1]
                         )
                      )
                   )
                {
                    move_possible = false;
                }
            }

            // Avoid unsigned infinite loop
            if (j == 0)
            {
                break;
            }
        }
    }

    if (move_possible)
    {
        move *next_move = ALLOC(*next_move, 1);
        next_move->piece = piece;
        next_move->dir = RIGHT;
        sk_list_append(moves, next_move);
    }

}

void allMoves(board_state *source, sk_list *moves)
{
    if (!moves)
    {
        return;
    }

    sk_list_init(moves, NULL);

    if (!source)
    {
        return;
    }

    SINT_64 max = 3;

    UINT_64 i, j;
    for (i = 0; i < source->height; ++i)
    {
        for (j = 0; j < source->width; ++j)
        {
            if (source->tiles[i][j] > max)
            {
                max = source->tiles[i][j];
            }
        }
    }

    for (i = 2; i < (UINT_64)max + 1; ++i)
    {
        sk_list new_moves;
        allMovesHelp(source, i, &new_moves);
        sk_iterator it;
        move *next_move = NULL;
        sk_list_begin(&it, &new_moves);
        while (it.has_next(&it))
        {
            next_move = it.next(&it);
            sk_list_append(moves, next_move);
            sk_list_remove(&it);
        }
        it.destroy(&it);
        sk_list_destroy(&new_moves);
    }
}

void applyMove(board_state *source, move next_move)
{
    if (!source)
    {
        return;
    }

    UINT_64 i, j;
    bool move_possible = true;

    switch (next_move.dir)
    {
    case UP:
        for (i = 0; i < source->height; ++i)
        {
            for (j = 0; j < source->width; ++j)
            {
                if (move_possible && source->tiles[i][j] == next_move.piece)
                {
                    if ( !(  CLEAR == source->tiles[i - 1][j]
                          || next_move.piece == source->tiles[i - 1][j]
                          || (  MASTER == source->tiles[i][j]
                             && GOAL == source->tiles[i - 1][j]
                             )
                          )
                       )
                    {
                        move_possible = false;
                    }
                }
            }
        }
        if (!move_possible)
        {
            break;
        }
        for (i = 0; i < source->height; ++i)
        {
            for (j = 0; j < source->width; ++j)
            {
                if (source->tiles[i][j] == next_move.piece)
                {
                    source->tiles[i - 1][j] = next_move.piece;
                    source->tiles[i][j] = CLEAR;
                }
            }
        }
        break;
    case DOWN:
        for (i = 0; i < source->height; ++i)
        {
            for (j = 0; j < source->width; ++j)
            {
                if (move_possible && source->tiles[i][j] == next_move.piece)
                {
                    if ( !(  CLEAR == source->tiles[i + 1][j]
                          || next_move.piece == source->tiles[i + 1][j]
                          || (  MASTER == source->tiles[i][j]
                             && GOAL == source->tiles[i + 1][j]
                             )
                          )
                       )
                    {
                        move_possible = false;
                    }
                }
            }
        }
        if (!move_possible)
        {
            break;
        }
        for (i = source->height - 1; /* i >= 0 */; --i)
        {
            for (j = 0; j < source->width; ++j)
            {
                if (source->tiles[i][j] == next_move.piece)
                {
                    source->tiles[i + 1][j] = next_move.piece;
                    source->tiles[i][j] = CLEAR;
                }
            }
            // Avoid unsigned infinite loop
            if (i == 0)
            {
                break;
            }
        }
        break;
    case LEFT:
        for (i = 0; i < source->height; ++i)
        {
            for (j = 0; j < source->width; ++j)
            {
                if (move_possible && source->tiles[i][j] == next_move.piece)
                {
                    if ( !(  CLEAR == source->tiles[i][j - 1]
                          || next_move.piece == source->tiles[i][j - 1]
                          || (  MASTER == source->tiles[i][j]
                             && GOAL == source->tiles[i][j - 1]
                             )
                          )
                       )
                    {
                        move_possible = false;
                    }
                }
            }
        }
        if (!move_possible)
        {
            break;
        }
        for (i = 0; i < source->height; ++i)
        {
            for (j = 0; j < source->width; ++j)
            {
                if (source->tiles[i][j] == next_move.piece)
                {
                    source->tiles[i][j - 1] = next_move.piece;
                    source->tiles[i][j] = CLEAR;
                }
            }
        }
        break;
    case RIGHT:
        for (i = 0; i < source->height; ++i)
        {
            for (j = 0; j < source->width; ++j)
            {
                if (move_possible && source->tiles[i][j] == next_move.piece)
                {
                    if ( !(  CLEAR == source->tiles[i][j + 1]
                          || next_move.piece == source->tiles[i][j + 1]
                          || (  MASTER == source->tiles[i][j]
                             && GOAL == source->tiles[i][j + 1]
                             )
                          )
                       )
                    {
                        move_possible = false;
                    }
                }
            }
        }
        if (!move_possible)
        {
            break;
        }
        for (i = 0; i < source->height; ++i)
        {
            for (j = source->width - 1; /*j >= 0*/; --j)
            {
                if (source->tiles[i][j] == next_move.piece)
                {
                    source->tiles[i][j + 1] = next_move.piece;
                    source->tiles[i][j] = CLEAR;
                }
                // Avoid unsigned infinite loop
                if (j == 0)
                {
                    break;
                }
            }
        }
        break;
    default:
        break;
    }
}

void applyMoveCloning(board_state *source, move next_move, board_state *dest)
{
    if (!source || !dest)
    {
        return;
    }

    cloneGameState(source, dest);
    applyMove(dest, next_move);
}

bool stateEqual(board_state *a, board_state *b)
{
    if (!a || !b || a->width != b->width || a->height != b->height)
    {
        return false;
    }


    UINT_64 i, j;
    for (i = 0; i < a->height; ++i)
    {
        for (j = 0; j < a->width; ++j)
        {
            if (a->tiles[i][j] != b->tiles[i][j])
            {
                return false;
            }
        }
    }

    return true;
}

void tile_swap(board_state *source, SINT_64 target, SINT_64 replacement)
{
    if (!source)
    {
        return;
    }

    UINT_64 i, j;
    for (i = 0; i < source->height; ++i)
    {
        for (j = 0; j < source->width; ++j)
        {
            if (source->tiles[i][j] == target)
            {
                source->tiles[i][j] = replacement;
            }
        }
    }
}

void normalizeState(board_state *source)
{
    if (!source)
    {
        return;
    }

    UINT_64 i, j;
    // shift all tile numbers to be larger than any possible cell number
    for (i = 0; i < source->height; ++i)
    {
        for (j = 0; j < source->width; ++j)
        {
            switch (source->tiles[i][j])
            {
            case WALL:
            case CLEAR:
            case GOAL:
            case MASTER:
                break;
            default:
                source->tiles[i][j] += (source->height * source->width + 3);
                break;
            }
        }
    }

    // replace each block number with the smallest cell number it covers
    for (i = 0; i < source->height; ++i)
    {
        for (j = 0; j < source->width; ++j)
        {
            if (  WALL == source->tiles[i][j]
               || CLEAR == source->tiles[i][j]
               || GOAL == source->tiles[i][j]
               || MASTER == source->tiles[i][j]
               )
            {
                continue;
            }
            if (source->tiles[i][j] > (SINT_64)((i * source->width) + j + 3))
            {
                tile_swap(source, source->tiles[i][j], (i * source->width) + j + 3);
            }
        }
    }

    // Condense block indices into regularly increasing indices starting at 3
    UINT_64 current_index = 3;
    for (i = 0; i < source->height; ++i)
    {
        for (j = 0; j < source->width; ++j)
        {
            if (source->tiles[i][j] == (SINT_64)((i * source->width) + j + 3))
            {
                tile_swap(source, source->tiles[i][j], current_index++);
            }
        }
    }
}

void destroy_board_state(void *p)
{
    if (!p)
    {
        return;
    }

    board_state *state = p;

    UINT_64 i;
    for (i = 0; i < state->height; ++i)
    {
        free(state->tiles[i]);
    }
    free(state->tiles);
}

void randomWalks(board_state *source, UINT_64 N)
{
    if (!source)
    {
        return;
    }

    UINT_64 i, j;
    sk_list moves;
    sk_iterator it;
    UINT_64 num_moves;
    UINT_64 move_idx;
    move *next_move;
    for (i = 0; i <= N; ++i)
    {
        outputGameState();
        if (i == N)
        {
            break;
        }

        if (gameStateSolved(source))
        {
            break;
        }

        allMoves(source, &moves);
        num_moves = sk_list_size(&moves);
        if (num_moves == 0)
        {
            printf("Error! No moves found for given board state!\n");
            outputGameState();
            break;
        }
        move_idx = state.random.rand_64bit(&state.random) % num_moves;

        state.printer->debug(state.printer, DEBUG_DETAILS,
                                "Selected move %lu / %lu\n",
                                move_idx + 1,
                                num_moves);

        j = 0;
        sk_list_begin(&it, &moves);
        while (it.has_next(&it))
        {
            next_move = it.next(&it);

            if (j++ == move_idx)
            {
                printf("\n");
                switch (next_move->dir)
                {
                case UP:
                    printf("(%ld, up)\n", next_move->piece);
                    break;
                case DOWN:
                    printf("(%ld, down)\n", next_move->piece);
                    break;
                case LEFT:
                    printf("(%ld, left)\n", next_move->piece);
                    break;
                case RIGHT:
                    printf("(%ld, right)\n", next_move->piece);
                    break;
                }
                printf("\n");

                applyMove(source, *next_move);
            }
            else
            {
                state.printer->debug(state.printer, DEBUG_DETAILS,
                                        "Potential move (%d,%d)\n",
                                        next_move->piece,
                                        next_move->dir);
            }
            free(next_move);
            sk_list_remove(&it);
        }
        it.destroy(&it);
        sk_list_destroy(&moves);

        normalizeState(source);
    }

    printf("\n");
    printf("Random walk terminated after %lu moves.\n", i);

}

UINT_64 numZeros(board_state *source)
{
    UINT_64 i, j;
    UINT_64 zeros = 0;

    for (i = 0; i < source->height; ++i)
    {
        for (j = 0; j < source->width; ++j)
        {
            if (source->tiles[i][j] == 0)
            {
                zeros++;
            }
        }
    }
    return zeros;
}

bool uninformedDepthFirst(board_state *source, sk_list *closed, sk_list *soln, UINT_64 *nodes_visited)
{
    if (!source || !closed || !soln)
    {
        return false;
    }

    sk_list moves;
    sk_iterator it;
    sk_iterator check_it;
    move *next_move;
    board_state next_state;
    board_state *normalized_next_state;
    board_state *next_check_state;
    bool found_match = false;
    bool solved = false;

    if (gameStateSolved(source))
    {
        return true;
    }

    allMoves(source, &moves);
    if (sk_list_size(&moves) == 0)
    {
        printf("Error! No moves found for given board state!\n");
        sk_list_destroy(&moves);
        return false;
    }

    sk_list_begin(&it, &moves);
    while (it.has_next(&it))
    {
        next_move = it.next(&it);
        applyMoveCloning(source, *next_move, &next_state);

        normalized_next_state = ALLOC(*normalized_next_state, 1);
        cloneGameState(&next_state, normalized_next_state);

        found_match = false;
        sk_list_begin(&check_it, closed);
        while(check_it.has_next(&check_it))
        {
            next_check_state = check_it.next(&check_it);
            if (stateEqual(normalized_next_state, next_check_state))
            {
                found_match = true;
                break;
            }
        }
        check_it.destroy(&check_it);

        if (!found_match)
        {
            state.printer->debug(state.printer, DEBUG_DETAILS,
                                "Considering:\n");
            printGameState(&next_state);
            (*nodes_visited)++;

            sk_list_append(closed, normalized_next_state);

            solved = uninformedDepthFirst(&next_state, closed, soln, nodes_visited);

            destroy_board_state(&next_state);

            if (solved)
            {
                sk_list_prepend(soln, next_move);

                while (it.has_next(&it))
                {
                    next_move = it.next(&it);
                    free(next_move);
                }
                it.destroy(&it);

                return true;
            }
        }
        else
        {
            state.printer->debug(state.printer, DEBUG_DETAILS,
                                "FOUND DUPLICATE:\n");

            destroy_board_state(&next_state);
            destroy_board_state(normalized_next_state);
            free(normalized_next_state);
        }

        free(next_move);
        sk_list_remove(&it);
    }
    it.destroy(&it);
    sk_list_destroy(&moves);

    sk_list_begin(&it, closed);
    while (it.has_next(&it))
    {
        board_state *next = it.next(&it);
        destroy_board_state(next);
        free(next);
        sk_list_remove(&it);
    }
    it.destroy(&it);

    return false;
}

bool uninformedBreadthFirst(board_state *source, sk_list *closed, sk_list *open, sk_list *soln, UINT_64 *nodes_visited)
{
    if (!source || !closed || !open || !soln)
    {
        return false;
    }

    struct breadth_node;
    typedef struct breadth_node breadth_node;

    struct breadth_node
    {
        board_state *state;
        sk_list move_list;
    };

    breadth_node *root = ALLOC(*root, 1);
    root->state = ALLOC(*(root->state), 1);
    cloneGameState(source, root->state);
    sk_list_init(&root->move_list, NULL);
    sk_list_append(open, root);
    (*nodes_visited)++;

    // Iterator across the open list
    sk_iterator open_it;
    // Currently considered node
    breadth_node *current;
    // Next node to add to the open list
    breadth_node *next;
    // Normalized state of \c next to check for repeated states
    board_state *normalized_next_state;
    // Iterator across closed list
    sk_iterator check_it;
    // Next state in the check list to consider
    board_state *next_check_state;
    // Whether we found a match in the closed list for \c normalized_next_state
    bool found_match = false;
    // List of moves possible in the current state
    sk_list moves;
    // Iterator across the move list
    sk_iterator move_it;
    // Next move to consider
    move *next_move;

    sk_list_begin(&open_it, open);
    while (open_it.has_next(&open_it))
    {
        current = open_it.next(&open_it);
        sk_list_remove(&open_it);

        allMoves(current->state, &moves);
        if (sk_list_size(&moves) == 0)
        {
            printf("Error! No moves found for given board state!\n");
            sk_list_destroy(&moves);

            destroy_board_state(current->state);
            free(current->state);
            sk_iterator soln_it;
            sk_list_begin(&soln_it, &current->move_list);
            while (soln_it.has_next(&soln_it))
            {
                free(soln_it.next(&soln_it));
                sk_list_remove(&soln_it);
            }
            soln_it.destroy(&soln_it);
            sk_list_destroy(&current->move_list);
            free(current);
            return false;
        }

        sk_list_begin(&move_it, &moves);
        while (move_it.has_next(&move_it))
        {
            next_move = move_it.next(&move_it);
            next = ALLOC(*next, 1);
            next->state = ALLOC(*(next->state), 1);
            applyMoveCloning(current->state, *next_move, next->state);

            if (gameStateSolved(next->state))
            {
                // Destroy the new node we created
                destroy_board_state(next->state);
                free(next->state);
                free(next);

                // Populate the solution list with the winning moves
                sk_iterator soln_it;
                sk_list_begin(&soln_it, &current->move_list);
                while (soln_it.has_next(&soln_it))
                {
                    move *next_move = soln_it.next(&soln_it);
                    move *cloned = ALLOC(*cloned, 1);
                    *cloned = *next_move;
                    sk_list_append(soln, cloned);
                }
                soln_it.destroy(&soln_it);
                move *cloned = ALLOC(*cloned, 1);
                *cloned = *next_move;
                sk_list_append(soln, cloned);

                // Destroy the move list
                free(next_move);
                sk_list_remove(&move_it);
                while (move_it.has_next(&move_it))
                {
                    free(move_it.next(&move_it));
                    sk_list_remove(&move_it);
                }
                move_it.destroy(&move_it);
                sk_list_destroy(&moves);

                // Free all elements from the open list
                while (open_it.has_next(&open_it))
                {
                    current = open_it.next(&open_it);
                    destroy_board_state(current->state);
                    free(current->state);
                    sk_iterator soln_it;
                    sk_list_begin(&soln_it, &current->move_list);
                    while (soln_it.has_next(&soln_it))
                    {
                        free(soln_it.next(&soln_it));
                        sk_list_remove(&soln_it);
                    }
                    soln_it.destroy(&soln_it);
                    sk_list_destroy(&current->move_list);
                    free(current);
                }
                open_it.destroy(&open_it);

                return true;
            }

            normalized_next_state = ALLOC(*normalized_next_state, 1);
            cloneGameState(next->state, normalized_next_state);

            found_match = false;
            sk_list_begin(&check_it, closed);
            while(check_it.has_next(&check_it))
            {
                next_check_state = check_it.next(&check_it);
                if (stateEqual(normalized_next_state, next_check_state))
                {
                    found_match = true;
                    break;
                }
            }
            check_it.destroy(&check_it);

            // If we haven't visited an equivalent state yet, add the normalized
            //      state to the closed list and the new state to the open list
            if (!found_match)
            {
                sk_list_append(closed, normalized_next_state);

                state.printer->debug(state.printer, DEBUG_DETAILS,
                                    "Considering:\n");
                printGameState(next->state);
                (*nodes_visited)++;

                // Update the next node with the list of moves required to reach it
                sk_list_init(&next->move_list, NULL);
                sk_iterator soln_it;
                sk_list_begin(&soln_it, &current->move_list);
                while (soln_it.has_next(&soln_it))
                {
                    move *next_move = soln_it.next(&soln_it);
                    move *cloned = ALLOC(*cloned, 1);
                    *cloned = *next_move;
                    sk_list_append(&next->move_list, cloned);
                }
                soln_it.destroy(&soln_it);

                move *cloned = ALLOC(*cloned, 1);
                *cloned = *next_move;
                sk_list_append(&next->move_list, cloned);

                sk_list_append(open, next);
                //
                // -- HACK --
                //  If the open list iterator reached the end too soon, reinitialize it.
                //
                if (sk_list_size(open) != 0 && !open_it.has_next(&open_it))
                {
                    open_it.destroy(&open_it);
                    sk_list_begin(&open_it, open);
                }
            }
            else
            {
                state.printer->debug(state.printer, DEBUG_DETAILS,
                                    "FOUND DUPLICATE\n");

                destroy_board_state(next->state);
                free(next->state);
                free(next);

                destroy_board_state(normalized_next_state);
                free(normalized_next_state);
            }

            free(next_move);
            sk_list_remove(&move_it);
        }
        move_it.destroy(&move_it);
        sk_list_destroy(&moves);

        // Destroy the node we just visited
        destroy_board_state(current->state);
        free(current->state);
        sk_iterator soln_it;
        sk_list_begin(&soln_it, &current->move_list);
        while (soln_it.has_next(&soln_it))
        {
            free(soln_it.next(&soln_it));
            sk_list_remove(&soln_it);
        }
        soln_it.destroy(&soln_it);
        sk_list_destroy(&current->move_list);
        free(current);
    }

    return false;
}

void app_debug(struct printer *out, int level, char *fmt, ...)
{
    if (!out || out->debug_level < level || !fmt)
    {
        return;
    }

    // Debug messages are printed on standard output only
    va_list ap;
    va_start(ap, fmt);
    standard_error(out, "[DEBUG] ");
    standard_error_va(out, fmt, ap);
}

void handle_f(sk_str *match, sk_str **args, void *handle)
{
    global_state *state = handle;
    sk_str *arg = args[1];
    if (!match || !args || !arg || !handle)
    {
        return;
    }

    if (!resolve_input_file(arg->string, &state->resolved_path))
    {
        state->printer->error(state->printer, "Failed to parse filename: %s\n", arg->string);
    }
    else
    {
        state->printer->debug(state->printer, DEBUG_DETAILS,
                                "Resolved file path to %s\n",
                                state->resolved_path->string);
    }
}

void handle_r(sk_str *match, sk_str **args, void *handle)
{
    global_state *state = handle;
    sk_str *arg = args[1];
    if (!match || !args || !arg || !handle)
    {
        return;
    }

    int N = 0;
    if (!parse_int(arg, 0, &N))
    {
        state->printer->error(state->printer, "Failed to parse as int: %s\n", arg->string);
    }
    else
    {
        state->printer->debug(state->printer, DEBUG_DETAILS,
                                "Updating random walk step count to %d\n",
                                N);
        state->N = N;
        state->random_walk = true;
    }
}

void handle_d(sk_str *match, sk_str **args, void *handle)
{
    global_state *state = handle;
    if (!match || !args || !handle)
    {
        return;
    }

    state->printer->debug(state->printer, DEBUG_DETAILS,
                            "Updating to depth first search mode.\n");
    state->depth_first = true;
}

void handle_b(sk_str *match, sk_str **args, void *handle)
{
    global_state *state = handle;
    if (!match || !args || !handle)
    {
        return;
    }

    state->printer->debug(state->printer, DEBUG_DETAILS,
                            "Updating to breadth first search mode.\n");
    state->breadth_first = true;
}

void handle_verbose(sk_str *match, sk_str **args, void *handle)
{
    global_state *state = handle;
    if (!match || !args || !handle)
    {
        return;
    }

    state->printer->debug_level = DEBUG_ALL;

    state->printer->debug(state->printer, DEBUG_DETAILS,
                            "Setting debug level to maximum\n");
}

void handle_miss(sk_str *match, sk_str **args, void *handle)
{
    global_state *state = handle;
    if (!match || !args || !handle)
    {
        return;
    }

    state->printer->error(state->printer, "Error processing command line argument: %s\n", match->string);
}

bool interpret_input_arguments(global_state *state, int argc, char **argv)
{
    bool retval = true;
    keyword_entry cl_handlers[] =
    {
        // Input file
        {
            .keyword = "f",
            .argc = 1,
            .handler = handle_f
        },
        // Random walk mode
        {
            .keyword = "r",
            .argc = 1,
            .handler = handle_r
        },
        // Depth first search mode
        {
            .keyword = "d",
            .argc = 0,
            .handler = handle_d
        },
        // Breadth first search mode
        {
            .keyword = "b",
            .argc = 0,
            .handler = handle_b
        },
        // Verbose mode
        {
            .keyword = "verbose",
            .argc = 0,
            .handler = handle_verbose
        },
        {}
    };

    sk_const_iterator args_it;
    sk_const_iterator entries_it;
    if (!argv_const_begin(&args_it, argc, argv))

    {
        state->printer->error(state->printer, "Error: Failed to initialize argument vector iterator.\n");
        retval = false;
        goto argv_const_begin_fail;
    }

    if (!null_terminated_array_const_begin(&entries_it, (void **)&cl_handlers, sizeof(*cl_handlers)))
    {
        state->printer->error(state->printer, "Error: Failed to initialize entry array iterator.\n");
        retval = false;
        goto null_terminated_array_const_begin_fail;
    }

    // Eat the leading path argument
    args_it.next(&args_it);

    if (CLARGS_SUCCESS != interpret_keyword_args(&args_it, &entries_it, handle_miss, state))
    {
        state->printer->error(state->printer, "Error: Failed to interpret provided arguments.\n");
        retval = false;
    }

    entries_it.destroy(&entries_it);
null_terminated_array_const_begin_fail:

    args_it.destroy(&args_it);
argv_const_begin_fail:

    return retval;
}

bool validate_global_state(global_state *state)
{
    if (!state)
    {
        return false;
    }

    if (!state->resolved_path && !resolve_input_file(state->input_file_path, &state->resolved_path))
    {
        state->printer->error(state->printer, "Error: Failed to resolve input file path : %s.\n", state->input_file_path);
        return false;
    }

    if (state->random_walk && state->breadth_first)
    {
        state->printer->error(state->printer, "Error: Conflicting solution algorithms selected.");
        return false;
    }
    else if (state->random_walk && state->depth_first)
    {
        state->printer->error(state->printer, "Error: Conflicting solution algorithms selected.");
        return false;
    }
    else if (state->depth_first && state->breadth_first)
    {
        state->printer->error(state->printer, "Error: Conflicting solution algorithms selected.");
        return false;
    }

    return true;
}

void destroy_global_state(void *p)
{
    if (!p)
    {
        return;
    }
    global_state *state = p;

    if (state->resolved_path)
    {
        sk_str_destroy(state->resolved_path);
        free(state->resolved_path);
        state->resolved_path = NULL;
    }

    if (state->game_state)
    {
        UINT_64 i;
        for (i = 0; i < state->game_state->height; ++i)
        {
            free(state->game_state->tiles[i]);
        }
        free(state->game_state->tiles);
        free(state->game_state);
        state->game_state = NULL;
    }
}

bool resolve_input_file(const char *filename, sk_str **resolved)
{
    if (!filename || !resolved)
    {
        return false;
    }

    sk_str *ret = NULL;

    // Fully resolved path
    char final[PATH_MAX];

    // If filename represents an absolute path, don't expand it.
    if (*filename == '/')
    {
        // Check whether the path resolves to a file entry
        errno = 0;
        if (NULL == realpath(filename, final) && !errno)
        {
            return false;
        }

        ret = ALLOC(*ret, 1);
        sk_str_init(ret, final, 32);

        // Make sure the file exists
        if (exists(ret))
        {
            *resolved = ret;
            return true;
        }
        else
        {
            sk_str_destroy(ret);
            free(ret);
            return false;
        }
    }
    else
    {
        sk_str expanded;

        // Initialize path relative to current directory
        sk_str_init(&expanded, "./", 32);
        str_concat(filename, &expanded);

        // Determine whether path exists
        errno = 0;
        if (NULL == realpath(expanded.string, final) && !errno)
        {
            sk_str_destroy(&expanded);
            return false;
        }

        sk_str_destroy(&expanded);

        ret = ALLOC(*ret, 1);
        sk_str_init(ret, final, 32);

        // Make sure file exists
        if (exists(ret))
        {
            *resolved = ret;
            return true;
        }
        else
        {
            sk_str_destroy(ret);
            free(ret);
            return false;
        }
    }

    return false;
}

bool exists(sk_str *path)
{
    struct stat buffer;
    return !(stat(path->string, &buffer) < 0);
}

SINT_64 r_fread(int fd, void *dst, UINT_64 n)
{
    // Read bytes
    SINT_64 nread;
    // Remaining bytes
    UINT_64 left = n;
    // Output position
    char *out = dst;

    // While there are characters to read
    while (left > 0)
    {
        // Read as many bytes as possible
        if ((nread = read(fd, out, left)) >= 0)
        {
            // If nothing was read, return and report EOF.
            // FIXME find a better way to inform users of EOF.
            if (0 == nread)
            {
                errno = EOF;
                break;
            }
            left -= nread;
            out += nread;
        }
        else if (EINTR != errno)
        {
            // EINTR is expected, all other errors are fatal.
            return -1;
        }
    }

    return n - left;
}

UINT_64 _ngets(char *s, UINT_64 n, int fd)
{
    // number of read bytes from reliable read
    register SINT_64 num_read;
    // number of valid characters read
    register UINT_64 i = 0;

    while (--n > 0)
    {
        num_read = r_fread(fd, s, 1);
        if (num_read <= 0 || '\n' == *s)
        {
            break;
        }
        else if ('\0' == *s)
        {
            ;
        }
        else
        {
            ++s;
            ++i;
        }
    }

    // Null-terminate the string
    *s = '\0';
    return i;
}

bool fd_get_line(int fd, sk_str *dest)
{
    if (!fd || !dest)
    {
        return false;
    }

    // Allocate blocks of 256 characters at a time
    UINT_64 buffer_size = 256;
    UINT_64 curr_size = buffer_size;
    UINT_64 len = 0;
    UINT_64 max_read = buffer_size - 1;

    // Allocate a buffer of chars to read input from.
    char *input = ALLOC(*input, buffer_size);
    //char *pos = input;

    // Read in at most 255 valid characters, then null-terminate
    UINT_64 read = _ngets(input, buffer_size, fd);

    // Catch EOF
    if (0 == read && EOF == errno)
    {
        free(input);
        return false;
    }

    len += read;

    // Buffer the next line in until we have read it all.
    while (read == max_read)
    {
        max_read = buffer_size;

        //pos = pos + read;
        curr_size += buffer_size;

        // Reallocate the buffer to hold a larger string
        input = REALLOC(input, *input, curr_size);

        // The next position to start reading into is the current position of the null character
        read = _ngets(input + curr_size - buffer_size - 1, buffer_size + 1, fd);
        len += read;
    }

    dest->string = input;
    dest->len = len;
    dest->size = curr_size;

    // FIXME check output
    sk_str_trim(dest, dest);
    return true;
}

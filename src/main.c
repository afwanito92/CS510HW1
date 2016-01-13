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


bool loadGameState(const char *file_name);

void outputGameState();

bool cloneGameState(board_state *source, board_state *dest);

bool gameStateSolved(board_state *source);

void allMovesHelp(board_state *source, SINT_64 piece, sk_list *moves);

void allMoves(board_state *source, sk_list *moves);

void applyMove(board_state *source, move next_move);

void applyMoveCloning(board_state *source, move next_move, board_state *dest);

bool stateEqual(board_state *a, board_state *b);

void normalizeState(board_state *source);

void randomWalks(board_state *source, UINT_64 N);


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
    outputGameState();

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

    for (i = source->height; /*i >= 0*/; --i)
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
        for (j = source->width; /*j >= 0*/; --j)
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

    for (i = 3; i < (UINT_64)max; ++i)
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
        for (i = source->height; /* i >= 0 */; --i)
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
            for (j = source->width; /*j >= 0*/; --j)
            {
                if (source->tiles[i][j] == next_move.piece)
                {
                    source->tiles[i][j - 1] = next_move.piece;
                    source->tiles[i][j] = CLEAR;
                }
                // Avoid unsigned infinite loop
                if (i == 0)
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
    for (i = 0; i < N; ++i)
    {
        outputGameState();
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

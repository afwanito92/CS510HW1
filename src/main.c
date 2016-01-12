/**
 *
 * @file    main.c
 * @author  Eric Rock
 * @date    Apr 2, 2015
 * @brief   CG_hw2, adding polygon parsing, clipping, transformations, and scan conversion onto CG_hw1.
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




// Program Debug Message Severities
#define DEBUG_ALL           5
#define DEBUG_DETAILS       4
#define DEBUG_EVENTS        3
#define DEBUG_HIGH_LEVEL    2
#define DEBUG_NONE          1


struct global_state;
typedef struct global_state global_state;

struct global_state
{
    printer_t *printer;

    const char *input_file_path;    // Default file path for input file
    sk_str *resolved_path;          // Path to resolved input file

    board_state game_state;
};

struct board_state;
typedef struct board_state board_state;

struct board_state
{
    UINT_64 width;
    UINT_64 height;

    SINT_64 **tiles;
};

enum direction;
typedef enum direction direction;

enum direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

struct move;
typedef struct move move;

struct move
{
    SINT_64 piece;
    direction dir;
};



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

    int retval = EXIT_SUCCESS;

    state.printer = printer;
    state.input_file_path = "./assets/SBP-level0.txt";
    state.resolved_path = NULL;


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


cleanup:
    destroy_printer(printer);
    destroy_global_state(&state);

    exit(retval);
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

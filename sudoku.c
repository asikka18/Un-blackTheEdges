/*
 *      sudoku.c
 *      by: Armaan Sikka & Nate Pfeffer
 *      utln: asikka01 & npfeff01
 *      date: 9/25/24
 *      assignment: iii
 *
 *      summary: This file will read in a pgm file and determine if it is a 
 *              valid sudoku grid.
 *              The program stores the numbers from the sudoku grid in our
 *              implementation of UArray2. It then maps over columns, rows, 
 *              and four squares and checks whether these groups of nine are 
 *              all unique. 
 *              The program exits with EXIT_FAILURE if they are not valid but
 *              EXIT_SUCCESS if they are valid.
 *      
 */

#include <stdlib.h>
#include <stdio.h>
#include "except.h"
#include "pnmrdr.h"
#include "uarray2.h"
#include "set.h"

#define T Pnmrdr_T
typedef struct Set_n_Size *S;

struct Set_n_Size {
        Set_T group;
        int attemptedAdds;
};

Except_T FileOpenErr = { "Could not open file" };
Except_T IncorrectArgc = { "Incorrect number of arguments given" };
Except_T NonPGM = { "pgm file not provided" };
Except_T MallocFail = { "Malloc Failed" };

/********** intcmp ********
 *
 *      compares two given ints
 *
 *      Parameters:
 *              const void *x: pointer to first int
 *              const void *y: pointer to second int
 *
 *      Return: 
 *              int determining if first was less, greater, or equal to second
 *
 *      Expects:
 *              to be used for hashing in the set of ints
 *
 *      Notes:
 *              Used with the hanson set exclusively
 *      
 ******************************/
int intcmp(const void *x, const void *y) 
{
        if (*(int *)x < *(int *)y)
                return -1; 
        else if (*(int *)x > *(int *)y)
                return 1;  
        else
                return 0; 
}

/********** inthash ********
 *
 *      returns the given int
 *
 *      Parameters:
 *              const void *x: pointer to an int being added to set
 *
 *      Return: 
 *              unsigned int
 *
 *      Expects:
 *              to be used for hashing in the set of ints
 *
 *      Notes:
 *              Used with the hanson set exclusively
 *      
 ******************************/
unsigned inthash(const void *x) 
{
        return *(int *)x;
}

/********** print_pix ********
 *
 *      assigns the current spot to the value in the sudoku grid
 *
 *      Parameters:
 *              void *elem: the pointer to the element spot in the 2D array
 *              void *rdr: the reader that gets the number from the pgm file
 *
 *      Return: 
 *              nothing
 *
 *      Expects:
 *              to be called in one of the UArray2 mapping functions
 *
 *      Notes:
 *              all parameters except the current bit are void,
 *              but still provided as needed by the mapping function
 *              throws CRE if the number is not 1-9
 *      
 ******************************/
void assign(int i, int j, UArray2_T a, void *elem, void *rdr) 
{
        (void) i;
        (void) j;
        (void) a;
        
        /* assigns the current spot to the value in the sudoku grid */
        *(int *)elem = Pnmrdr_get((T)rdr);
        
        if (*(int *)elem < 1 || *(int *)elem > 9)
                exit(EXIT_FAILURE);
}

/********** pgm_to_uarray2 ********
 *
 *      converts the provided pgm file to a UArray2 object to work with
 *
 *      Parameters:
 *              FILE *fp: the pointer to the given pgm file
 *
 *      Return: 
 *              the filled out 2D array representing the puzzle grid
 *
 *      Expects:
 *              an open pgm file to be given
 *
 *      Notes:
 *              
 *      
 ******************************/
UArray2_T pgm_to_uarray2(FILE *fp)
{
        /* creates a pnm reader and map data objects*/
        T rdr = Pnmrdr_new(fp);
        Pnmrdr_mapdata data = Pnmrdr_data(rdr);

        /* checks that the width, height, and max value are correct */
        if (data.width != 9 || data.height != 9 || data.denominator != 9) {
                exit(EXIT_FAILURE);
        }

        /* create a uarray2 */
        UArray2_T a;
        a = UArray2_new(data.height, data.width, sizeof(int));

        /* assigns values from the pgm to location in the 2D array */
        UArray2_map_row_major(a, assign, rdr);

        Pnmrdr_free(&rdr);
        return a;
}

/********** check_nine ********
 *
 *      adds a new number to the set and checks if the last 9 attempted
 *      adds were all unique and thus valid
 *
 *      Parameters:
 *              void *elem: a pointer to the current number in the grid
 *              void *set_struct: the current set and counter
 *
 *      Return: 
 *              nothing
 *
 *      Expects:
 *              that it can be called in one of the UArray2 mapping functions,
 *                      but it is not a requirement
 *
 *      Notes:
 *              all parameters except the current bit are void 
 *              per the map function
 *              The new set made after 9 adds, is freed the next time
 *              nine add attempts occur.
 *      
 ******************************/
void check_nine(int i, int j, UArray2_T a, void *elem, void *set_struct) 
{
        (void) i;
        (void) j;
        (void) a;
        
        S group_n_adds = (S)set_struct;

        /* attempts to add current number to the set */
        Set_put(group_n_adds->group, elem);
        group_n_adds->attemptedAdds++;

        /* check validity if at end of row, col, or square */
        if (group_n_adds->attemptedAdds == 9) {

                /* if set not full by end of row, col, or square, invalid */
                if (Set_length(group_n_adds->group) != 9)
                        exit(EXIT_FAILURE);

                /* frees just used set, creates new set, resets add count*/
                Set_free(&(group_n_adds->group));
                Set_T new = Set_new(9, intcmp, inthash);
                group_n_adds->group = new;
                group_n_adds->attemptedAdds = 0;
        }
}

/********** check_square ********
 *
 *      used to check one 3x3 square of the puzzle
 *
 *      Parameters:
 *              int col: one of the columns that is in the square to check
 *              int row: one of the rows that is in the square to check
 *              S set_struct: The set and counter
 *              UArray2_T a: The 9x9 array of numbers
 *
 *      Return: 
 *              nothing
 *
 *      Expects:
 *              a col and row within the bounds of the grid
 *              a cleared set_struct
 *
 *      Notes:
 *              Will call the check_nine function on all numbers in the square
 *      
 ******************************/
void check_square(int col, int row, S set_struct, UArray2_T a)
{
        /* indices of the first cell in a square */
        int sq_col = 3 * (col / 3);
        int sq_row = 3 * (row / 3);

        /* iterates through the square, checking validity */
        for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                        check_nine( i, 
                                j, 
                                a, 
                                UArray2_at(a, sq_col + i, sq_row + j), 
                                set_struct);
                }
        }
}

/********** check_puzz ********
 *
 *      assesses the rows, columns, and four squares of the puzzle to check
 *              if it is a valid sudoku grid
 *
 *      Parameters:
 *              UArray2_T a: the 2D array that holds the 9x9 grid of numbers
 *              void *set_struct: the struct that holds a set and a counter
 *                              for checking the puzzle.
 *
 *      Return: 
 *              nothing
 *
 *      Expects:
 *              a fully assigned 9x9 array of numbers
 *              an allocated set, and counter at 0
 *
 *      Notes:
 *              all parameters except the current bit are void
 *      
 ******************************/
void check_puzz(UArray2_T a, void *set_struct)
{
        S group_n_adds = (S)set_struct;

        /* check if every columns and row is valid */
        UArray2_map_row_major(a, check_nine, group_n_adds);
        UArray2_map_col_major(a, check_nine, group_n_adds);

        /* check if four squares are correct */
        check_square(0, 0, group_n_adds, a);
        check_square(3, 3, group_n_adds, a);
        check_square(6, 6, group_n_adds, a);
        check_square(0, 3, group_n_adds, a);

        Set_free(&(group_n_adds->group));
        free(group_n_adds);
}

/********** sudoku ********
 *
 *      runs the sudoku program
 *
 *      Parameters:
 *              FILE *fp: the input file stream pointer
 *
 *      Return: 
 *              nothing
 *
 *      Expects:
 *              non-null fp
 *
 *      Notes:
 *              raised exception if malloc fails
 *              group_n_adds is freed in check_puzz
 *      
 ******************************/
void sudoku(FILE *fp)
{
        UArray2_T a = pgm_to_uarray2(fp);

        /* create a hanson set */
        Set_T set = Set_new(9, intcmp, inthash);
        
        /* create and initialize the Set_n_size struct with */
        /* empty set and attempted adds of 0 */
        S group_n_adds = malloc(sizeof(*group_n_adds));
        if (group_n_adds == NULL)
                RAISE(MallocFail);
        group_n_adds->attemptedAdds = 0;
        group_n_adds->group = set;

        check_puzz(a, group_n_adds);

        UArray2_free(&a);
}

/********** open_or_abort ********
 *
 *      tries to open a file and returns the open file stream only if
 *      the file is a .pgm
 *
 *      Parameters:
 *              char *fname: the name of the file to the opened
 *              char *mode: the mode to open the file with
 *
 *      Return: 
 *              the opened file stream pointer
 *
 *      Expects:
 *              a non-null fname pointer and mode pointer
 *              a pgm file
 *
 *      Notes:
 *              raises an exception if the file given is not in .pgm format
 *              if the file could not be opened/fp is null, raises an exception
 *      
 ******************************/
static FILE *open_or_abort(char *fname, char *mode)
{
        /* checks for pgm correct file */
        char *end = strrchr(fname, '.');
        if (strcmp(end, ".pgm") != 0)
                RAISE(NonPGM);

        FILE *fp = fopen(fname, mode);
        if (fp == NULL)
                RAISE(FileOpenErr);
        
        return fp;
}

/* main function: raised exception if incorrect number of arguments is given */
int main(int argc, char *argv[])
{
        if (argc == 1) {
                sudoku(stdin);

        } else if (argc == 2) {
                FILE *fp = open_or_abort(argv[1], "rb");
                sudoku(fp);

                fclose(fp);
        } else {
                RAISE(IncorrectArgc);
        }

        exit(EXIT_SUCCESS);
}
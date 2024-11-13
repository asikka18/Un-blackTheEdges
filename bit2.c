/*
 *      bit2.c
 *      by: Armaan Sikka & Nate Pfeffer
 *      utln: asikka01 & npfeff01
 *      date: 9/25/24
 *      assignment: iii
 *
 *      summary:
 *      
 */

#include "bit2.h"
#include <except.h>

Except_T Malloc_Fail = { "Malloc Failed" };
Except_T Invalid_P = { "NULL Pointer to Array" };

/********** Bit2_new ********
 *
 *      Creates a new 2-D array of bits
 *
 *      Parameters: 
 *              int rows: number of rows for the array
 *              int columns: number of columns for the array
 *
 *      Return: 
 *              returns a pointer to the array
 *
 *      Expects:
 *              valid ints passed in the parameters
 *
 *      Notes:
 *              check if memory allocated correctly
 *      
 ******************************/
Bit2_T Bit2_new(int columns, int rows)
{
        T bitArr = Bit_new(rows * columns);
        Bit2_T bit2 = malloc(sizeof(*bit2));

        if (bit2 == NULL) {
                RAISE(Malloc_Fail);
        }

        bit2->theArray = bitArr;
        bit2->numRows = rows;
        bit2->numCols = columns;
        return bit2;
}

/********** Bit2_width ********
 *
 *      gets the width of the 2-D bit array
 *
 *      Parameters:
 *              UArray2 arr: the pointer to 2D bit array
 *
 *      Return: 
 *              the width of bits as an int
 *
 *      Expects:
 *              a pointer that is non-null to a proper bit array
 *
 *      Notes:
 *              if pointer to array is null, exit with checked runtime error
 *      
 ******************************/
int Bit2_width(Bit2_T bitArr)
{
        if (bitArr == NULL) {
                RAISE(Invalid_P);
        }
        return bitArr->numCols;
}

/********** Bit2_height ********
 *
 *      get the height of the 2-D bit array
 *
 *      Parameters:
 *              T bitArr: the pointer to the bit array
 *
 *      Return: 
 *              an int of the height of the 2D bit array
 *
 *      Expects:
 *              a pointer that is non-null to a proper bit array
 *
 *      Notes:
 *              if pointer to array is null, exit with checked runtime error
 *      
 ******************************/
int Bit2_height(Bit2_T bitArr)
{
        if (bitArr == NULL) {
                        RAISE(Invalid_P);
                }
        return bitArr->numRows;
}

/********** Bit2_put ********
 *
 *      sets the bit at the index given
 *
 *      Parameters:
 *              T bitArr: the pointer to the 2D array
 *              int rowIdx: desired row index
 *              int colIdx: desired column index 
 *              int bit: either a 0 or 1 to place in the coordinates provided
 *
 *      Return: 
 *              1 if bit is already set, 0 otherwise
 *
 *      Expects:
 *              index that is within the bounds the array
 *              bit to be either 0 or 1
 *
 *      Notes:
 *              if pointer to bit array is null, exit with checked runtime error
 *      
 ******************************/
int Bit2_put(Bit2_T bitArr, int colIdx, int rowIdx, int bit)
{
        if (bitArr == NULL) {
                RAISE(Invalid_P);
        }
        
        return Bit_put(bitArr->theArray, 
                        ((colIdx * bitArr->numRows) + rowIdx), 
                        bit);
}

/********** Bit2_get ********
 *
 *      gets the bit at the index given
 *
 *      Parameters:
 *              T bitArr: the pointer to the 2D array
 *              int rowIdx: desired row index
 *              int colIdx: desired column index 
 *
 *      Return: 
 *              value of the bit at the given index
 *
 *      Expects:
 *              index that is within the bounds the array
 *              bit to be either 0 or 1
 *
 *      Notes:
 *              if pointer to bit array is null, exit with checked runtime error
 *      
 ******************************/
int Bit2_get(Bit2_T bitArr, int colIdx, int rowIdx)
{
        if (bitArr == NULL) {
                RAISE(Invalid_P);
        }

        return Bit_get(bitArr->theArray, ((colIdx * bitArr->numRows) + rowIdx));
}

/********** Bit2_map_col_major ********
 *
 *      maps over the bit array with columns getting priority, applying the 
 *      apply() function to each bit
 *
 *      Parameters:
 *              T bitArr: a pointer to the 2D bit array
 *              apply(): the function to be applied to each bit
 *              void *cl: to check whether the function was applied to all
 *                        bits
 *
 *      Return: 
 *              nothing
 *
 *      Expects:
 *              apply() to be a valid function with no errors that works
 *              on bits
 *              
 *      Notes:
 *              
 *      
 ******************************/
void Bit2_map_col_major(Bit2_T a, 
                        void apply(int i, int j, Bit2_T a, int num, void *cl), 
                        void *cl)
{
        if (a == NULL) {
                RAISE(Invalid_P);
        }

        for (int i = 0; i < a->numCols; i++) {
                for (int j = 0; j <  a->numRows; j++) {
                        apply(i, j, a, Bit2_get(a, i, j), cl);
                }
        }
}

/********** Bit2_map_row_major ********
 *
 *      maps over the bit array with rows getting priority, applying the 
 *      apply() function to each bit
 *
 *      Parameters:
 *              T bitArr: a pointer to the 2D bit array
 *              apply(): the function to be applied to each bit
 *              void *cl: to check whether the function was applied to all
 *                        bits
 *
 *      Return: 
 *              nothing
 *
 *      Expects:
 *              apply() to be a valid function with no errors that works
 *              on bits
 *              
 *      Notes:
 *              
 *      
 ******************************/
void Bit2_map_row_major(Bit2_T a, 
                        void apply(int i, int j, Bit2_T a, int num, void *cl), 
                        void *cl)
{
        if (a == NULL) {
                RAISE(Invalid_P);
        }

        for (int j = 0; j < a->numRows; j++) {
                for (int i = 0; i <  a->numCols; i++) {
                        apply(i, j, a, Bit2_get(a, i, j), cl);
                }
        }
}
/********** Bit2_free ********
 *
 *      frees all bits stored in the array
 *
 *      Parameters:
 *              T *bitArr: a pointer to the bit array
 *
 *      Return: 
 *              nothing
 *
 *      Expects:
 *              none of the bits have been freed previously
 *
 *      Notes:
 *              exit with a checked runtime error if bitArr is null
 *      
 ******************************/
void Bit2_free(Bit2_T *bitArr)
{
        if (bitArr == NULL) {
                RAISE(Invalid_P);
        }

        Bit_free(&((*bitArr)->theArray));
        free(*bitArr);
}
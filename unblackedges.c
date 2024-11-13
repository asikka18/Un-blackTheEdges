/*
 *      unblackedges.c
 *      by: Armaan Sikka & Nate Pfeffer
 *      utln: asikka01 & npfeff01
 *      date: 9/26/24
 *      assignment: iii
 *
 *      summary:
 *              This file will read in a pbm file and remove any blacked edges.
 *              The program stores the bit values of the file in our
 *              implementation of the Bit2 array. It then traverses the edges
 *              the 2D array and checks whether the bits and neighboring bits 
 *              are black per the guidlines in the spec. The bit array is then 
 *              outputted to stdout in a valid plain pgm format with a header 
 *              and a raster.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include "except.h"
#include "pnmrdr.h"
#include "bit2.h"
#include "stack.h"

#define T Pnmrdr_T

/* Hanson Exceptions used in the program */
Except_T FileOpenErr = { "Could not open file" };
Except_T NonPBM = { "pbm file not provided" };
Except_T IncorrectArgc = { "Invalid number of arguments provided" };
Except_T FailedMalloc = { "Malloc Failed" };
Except_T NullPtr = { "Null pointer in unexpected location" };

/********** print_pix ********
 *
 *      prints the given pixel bit
 *
 *      Parameters:
 *              int num: the bit of current pixel
 *
 *      Return: 
 *              nothing
 *
 *      Expects:
 *              to be called in one of the Bit2 mapping functions
 *
 *      Notes:
 *              all parameters except the current bit are void
 *      
 ******************************/
void print_pix(int i, int j, Bit2_T b, int num, void *rdr) 
{
        (void) i;
        (void) j;
        (void) b;
        (void) rdr;
        
        printf("%d", num);
}

/********** ez_push ********
 *
 *      creates an int array with 2 elements which denote the index of the
 *      pixel in question and pushes onto the stack for tracking
 *
 *      Parameters:
 *              int col: the column integer
 *              int row: the row integer
 *              Stack_T stack: the stack containing the indices of the black
 *                              edges
 *
 *      Return: 
 *              nothing
 *
 *      Expects:
 *              a row and col within bounds (checked in check_neighbors), 
 *              as well as an initialized and non-null stack because Hanson
 *              checks for that upon initialisation
 *
 *      Notes:
 *              CRE raised if malloc fails
 *              dfs() will free the allocated array (idx)
 *      
 ******************************/
void ez_push(int col, int row, Stack_T stack)
{
        int *idx = malloc(sizeof(int) * 2);
        if (idx == NULL)
                RAISE(FailedMalloc);
        
        idx[0] = col;
        idx[1] = row;
        Stack_push(stack, idx);
}

/********** check_neighbors ********
 *
 *      takes the current bit of the given black edge at the index specified
 *      and a certain index and checks the bits at the indices to the top,
 *      bottom, and both sides to see whether they are also black and part of
 *      the group of black edges to be changed
 *      
 *
 *      Parameters:
 *              Stack_T stack: the stack of the stored indices of black edges
 *              int *idx: pointer to the array that stores the index of the
 *                              black edge
 *              Bit2_T b: the bit array containing the image
 *
 *      Return: 
 *              nothing
 *
 *      Expects:
 *              the bit array to not be empty and the idx to be within bounds
 *
 *      Notes:
 *              checks whether idx is null and CREs if so
 *      
 ******************************/
void check_neighbors(Stack_T stack, int *idx, Bit2_T b)
{
        if (idx == NULL)
                RAISE(NullPtr);
        
        /* checking four possible sides of the current bit for black edges */
        if ((idx[0] - 1) >= 0) {
                if (Bit2_get(b, (idx[0] - 1), idx[1]))
                        ez_push((idx[0] - 1), idx[1], stack);
        }
        if ((idx[0] + 1) < Bit2_width(b)) {
                if (Bit2_get(b, (idx[0] + 1), idx[1]))
                        ez_push((idx[0] + 1), idx[1], stack);
        }
        if ((idx[1] - 1) >= 0) {
                if (Bit2_get(b, idx[0], (idx[1] - 1)))
                        ez_push(idx[0], idx[1] - 1, stack);
        }
        if ((idx[1] + 1) < Bit2_height(b)) {
                if (Bit2_get(b, idx[0], (idx[1] + 1))) 
                        ez_push(idx[0], idx[1] + 1, stack);
        }
}

/********** dfs ********
 *
 *      does a depth-first search on the image to check for black edges
 *
 *      Parameters:
 *              Bit2_T b: the bit array of the image
 *              int col: the column integer
 *              int row: the row integer
 *
 *      Return: 
 *              nothing
 *
 *      Expects:
 *              values of row and col to not be out-of-bounds
 *              bit2 operations to work/CRE if necessary
 *
 *      Notes:
 *              
 *      
 ******************************/
void dfs(Bit2_T b, int col, int row)
{       
        Stack_T stack = Stack_new();
        
        /* if the bit pulled is a 1, call ez_push */
        if (Bit2_get(b, col, row))
                ez_push(col, row, stack);
                
        /* 
         *      while the stack isn't empty, check if there are any neighboring
         *      black edges and put them into the bit array as bits with value
         *      0
         */
        while (!Stack_empty(stack)) {
                int *idx = (int *)Stack_pop(stack);
                Bit2_put(b, idx[0], idx[1], 0);
                check_neighbors(stack, idx, b);

                free(idx);
        }

        Stack_free(&stack);
}

/********** white_edges ********
 *
 *      iterates through the edge of the image and calls dfs to do the depth-
 *      first search for more black edges
 *
 *      Parameters:
 *              Bit2_T b: the bit array of the image
 *
 *      Return: 
 *              nothing
 *
 *      Expects:
 *              bit2 operations to work/CRE if necessary
 *
 *      Notes:
 *              
 *      
 ******************************/
void white_edges(Bit2_T b)
{
        /* first loop to iterate through top row */
        for (int i = 0; i < Bit2_width(b); i++)
                dfs(b, i, 0);

        /* second loop to iterate through first column */
        for (int i = 0; i < Bit2_height(b); i++)
                dfs(b, 0, i);

        /* third loop to iterate through last column */
        for (int i = 0; i < Bit2_height(b); i++)
                dfs(b, (Bit2_width(b) - 1), i);

        /* fourth loop to iterate through bottom row */
        for (int i = 0; i < Bit2_width(b); i++)
                dfs(b, i, (Bit2_height(b) - 1));
}

/********** assign ********
 *
 *      places the bit read from the file into the bit array sequentially
 *
 *      Parameters:
 *              Bit2_T b: the bit array of the image
 *              int num: the value of the bit to be placed in the bit2 array
 *              void *rdr: the pnm reader
 *
 *      Return: 
 *              nothing
 *
 *      Expects:
 *              bit2 operations to work/CRE if necessary
 *              pnmrdr operations to work/CRE if necessary
 *
 *      Notes:
 *              exits with value failure if any values in the image are not
 *              either 0 or 1
 *      
 ******************************/
void assign(int i, int j, Bit2_T b, int num, void *rdr) 
{
        (void) i;
        (void) j;
        
        /* adding image to bit2 array */
        Bit2_put(b, i, j, Pnmrdr_get((T)rdr));
        
        if (num < 0 || num > 1)
                exit(EXIT_FAILURE);
}

/********** pbm_to_bit2 ********
 *
 *      converts the given pbm to a bit2 array
 *
 *      Parameters:
 *              FILE *fp: pointer to the input stream whether that be a file or
 *                              stdin
 *
 *      Return: 
 *              a Bit2_T array with the image
 *
 *      Expects:
 *              a non null file pointer
 *              the pnmrdr operation to work/CRE if necessary
 *
 *      Notes:
 *              fp closed in main
 *      
 ******************************/
Bit2_T pbm_to_bit2(FILE *fp)
{
        /* creates a pnm reader and map data objects*/
        T rdr = Pnmrdr_new(fp);
        Pnmrdr_mapdata data = Pnmrdr_data(rdr);

        /* checks that the width and height are correct */
        if (data.width == 0 || data.height == 0 || data.type != Pnmrdr_bit)
                exit(EXIT_FAILURE);

        /* create a bit2 */
        Bit2_T b;
        b = Bit2_new(data.width, data.height);

        Bit2_map_row_major(b, assign, rdr);
        Pnmrdr_free(&rdr);

        return b;
}

/********** print_plain ********
 *
 *      prints the pbm in a plain pgm format to stdout
 *
 *      Parameters:
 *              Bit2_T b: the bit array with the image
 *
 *      Return: 
 *              nothing
 *
 *      Expects:
 *              
 *
 *      Notes:
 *              raised exception if malloc failed
 *      
 ******************************/
void print_plain(Bit2_T b)
{
        /* set the width length and height length of the raster */
        int wl = snprintf(NULL, 0, "%d", Bit2_width(b));
        int hl = snprintf(NULL, 0, "%d", Bit2_height(b));

        int head_l = 6 + wl + hl;

        /* malloc space for the header */
        char *head = malloc(sizeof(char) * (head_l));
        if (head == NULL)
                RAISE(FailedMalloc);

        /* print header */
        snprintf (head, head_l, "P1\n%d %d\n", Bit2_width(b), Bit2_height(b));
        printf ("%s", head);

        /* print raster */
        Bit2_map_row_major(b, print_pix, NULL);

        free(head);
}

/********** open_or_abort ********
 *
 *      tries to open a file and returns the open file stream
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
 *
 *      Notes:
 *              raises an exception if the file given is not in .pbm format
 *              if the file could not be opened/fp is null, raises an exception
 *      
 ******************************/
static FILE *open_or_abort(char *fname, char *mode)
{
        char *end = strrchr(fname, '.');
        if (strcmp(end, ".pbm") != 0) {
                RAISE(NonPBM);
        }

        FILE *fp = fopen(fname, mode);

        if (fp == NULL) {
                RAISE(FileOpenErr);
        }
        
        return fp;
}

/********** unblackedges ********
 *
 *      the function that runs the unblackedges program
 *
 *      Parameters:
 *              FILE *fp: a pointer to the opened file stream or stdin
 *
 *      Return: 
 *              nothing
 *
 *      Expects:
 *              fp to be non-null
 *
 *      Notes:
 *              the Bit2_T functions to work/CRE if necessary
 *      
 ******************************/
void unblackedges(FILE *fp)
{
        Bit2_T b = pbm_to_bit2(fp);
        white_edges(b);
        print_plain(b);
        Bit2_free(&b);
}

/* main function: raises an exception if more than two arguments are given*/
int main(int argc, char *argv[])
{
        /* checks the number of arguments and runs the program as specified */
        if (argc == 1) {
                unblackedges(stdin);

        } else if (argc == 2) {
                FILE *fp = open_or_abort(argv[1], "rb");

                unblackedges(fp);
                
                fclose(fp);
        } else {
                RAISE(IncorrectArgc);
        }

        return 0;
}

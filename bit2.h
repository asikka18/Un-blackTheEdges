/*
 *      bit2.c
 *      by: Armaan Sikka & Nate Pfeffer
 *      utln: asikka01 & npfeff01
 *      date: 9/24/24
 *      assignment: iii
 *
 *      summary:
 *      
 */

#include "bit.h"
#include <stdlib.h>

typedef struct Bit_T *T;
typedef struct Bit2_T *Bit2_T;

struct Bit2_T {
        Bit_T theArray;
        int numRows;
        int numCols;
};

Bit2_T Bit2_new(int rows, int columns);
int Bit2_width(Bit2_T bitArr);
int Bit2_height(Bit2_T bitArr);
int Bit2_put(Bit2_T bitArr, int colIdx, int rowIdx, int bit);
int Bit2_get(Bit2_T bitArr, int colIdx, int rowIdx);
void Bit2_map_col_major(Bit2_T bitArr, 
                        void apply(int i, int j, Bit2_T a, int num, void *cl), 
                        void *cl);
void Bit2_map_row_major(Bit2_T bitArr, 
                        void apply(int i, int j, Bit2_T a, int num, void *cl), 
                        void *cl);
void Bit2_free(Bit2_T *bitArr);


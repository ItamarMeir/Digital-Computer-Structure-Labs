#include <stdio.h>
#include <string.h>
#define M 10 
#define N 10
#define CEIL 100 
#define max(x,y) x>y ? x : y  // MACRO function

int ComputeTrace(int Mat[M][M]);
void DiagonalsMax(int Mat1[M][M], int Mat2[M][M]);
void Transpose(int Mat1[M][M], int Mat2[M][M]);
void AddMatrix(int Mat1[M][M], int Mat2[M][M]);
void FillMatrix(unsigned int Mat[M][M], int);
void show_matrix(unsigned int Mat[M][M]);
void copy_diag(int Mat1[M][M], int Mat2[M][M]);
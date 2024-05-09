#include "func.h"

//---------------------------------------------------------------
//                  Trace Computation
//---------------------------------------------------------------
int ComputeTrace(int Mat[M][M]){
	int Trace=0,i;
	for(i=0 ; i<M ; i++) Trace += Mat[i][i]; 
	return Trace;
}
//---------------------------------------------------------------
//                  DiagonalsMax Computation
//---------------------------------------------------------------
void DiagonalsMax(int Mat1[M][M], int Mat2[M][M]){
	int Max1=0, Max2=0, i;
		for(i=0 ; i<M ; i++){
			Max1 = max(Mat1[i][i],Max1);
			Max2 = max(Mat2[i][i],Max2);
		} 
		if (Max1 >= Max2) printf("mat1 has the maximal diagonal element value: %d\n", Max1);
        else printf("mat2 has the maximal diagonal element value: %d\n", Max2);
}
//---------------------------------------------------------------
//                  Copy Diag Computation
//---------------------------------------------------------------
void copy_diag(int Mat1[M][M], int Mat2[M][M]){
    int i, tmp;
    for(i=0 ; i<M ; i++){
        tmp = Mat1[i][i];
        Mat2[i][i] = Mat1[i][N-1-i];
        Mat2[i][N-1-i] = tmp;
    }
}
//---------------------------------------------------------------
//                  Matrix Transpose 
//---------------------------------------------------------------
void Transpose(int Mat[M][M], int aux[M][M]) {
	int i, j;
	for (i = 0; i < M; i++) {
		for (j = 0; j < M; j++) {
			aux[i][j] = Mat[j][i];
		}
	}
}
//---------------------------------------------------------------
//                  Add Matrix
//---------------------------------------------------------------
void AddMatrix(int Mat1[M][M], int Mat2[M][M]) {
	int i, j;
    int Mat3[M][M];
    for (i = 0; i < M; i++) {
        for (j = 0; j < M; j++) {
            Mat3[i][j] = Mat1[i][j] + Mat2[i][j];
        }
    }
    show_matrix(Mat3);
	
}
//---------------------------------------------------------------
//                  Fill Matrix
//---------------------------------------------------------------
void FillMatrix(unsigned int Mat[M][M], int offset) {
	int i, j;
	for (i = 0; i < M; i++) {
		for (j = 0; j < M; j++) {
			Mat[i][j] = (offset + i * M + j) % CEIL;
		}
	}
}
//---------------------------------------------------------------
//                  show matrix
//---------------------------------------------------------------
void show_matrix(unsigned int Mat[M][M]) {
	int i, j;
	printf("[\n");
	for (i = 0; i < M; i++) {
		for (j = 0; j < M; j++)
			printf("%d, ", Mat[i][j]);
		printf("\n");
	}
	printf("]\n\n");
	return;
}
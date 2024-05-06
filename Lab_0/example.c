#include <stdio.h> 

#define M 10 
#define N 10
#define CEIL 100 
#define max(x,y) x>y ? x : y  // MACRO function

void show_menu(char** strlst) {
	int i;
	for (i = 0; strlst[i]; i++) 
		printf("%s\n", strlst[i]);
	return;
}

char** menu[] = {
	" Menu \n\r-------",
	"1) Create 10x10 matrix Mat Show using the next expression: ",
	"   for i,j = {0-9}: Mat[i][j] = (offset + i * 10 + j) % 100",
	"2) Show the Mat diagonals maximal element",
	"3) Compute and show the Mat trace value",
	"4) Compute and show the transposed Mat",
	"5) Show Mat",
	"6) Show menu",
	"7) exit",
	NULL
};

int ComputeTrace(int Mat[M][M]);
int DiagonalsMax(int Mat[M][M]);
void Transpose(int Mat1[M][M], int Mat2[M][M]);
void FillMatrix(unsigned int Mat[M][M], int);
void show_matrix(unsigned int Mat[M][M]);
//-------------- Global variables ------------
  int maxTrace,maxDiag;
//--------------------------------------------
int main(){
        
  int Mat[M][M], auxMat[M][M];
  int matTrace,maxDiag,offset=0;
  char Selector = '0', ch, str[3];
  
  show_menu(menu);
  
  while(1) {  	   
    
    printf("insert a menu clause number: ");
	Selector = getchar();
    getchar(); // get the enter key
    
    switch(Selector){
    case '0': 
      break; 

    case '1': 
	  do{
		  printf("Insert an offset value between 0-100: ");
		  gets(str);
		  sscanf_s(str,"%d",&offset);
		  if (offset >= 0 && offset <= 100) break;

	  } while (1);
	  FillMatrix(Mat, offset);
      break;

    case '2': 
	  maxDiag = DiagonalsMax(Mat);
	  printf("The maximal diagonal element = %d\n", maxDiag);
      break;

    case '3': 
	  matTrace = ComputeTrace(Mat);
	  printf("The mat Trace value = %d\n", matTrace);
	  break;

    case '4': 
	  Transpose(Mat,auxMat);
	  show_matrix(auxMat);
	  break;

	case '5':
	 show_matrix(Mat);
	 break;

	case '6':
	  show_menu(menu);
	  break;

	case '7':
	 return 0;

	default:
	 printf("Try agian, You have inserted wrong number\n");
	 break;
  }
 }	
}
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
int DiagonalsMax(int Mat[M][M]){
	int Max=0,i;
		for(i=0 ; i<M ; i++){
			Max = max(Mat[i][i],Max);
			Max = max(Mat[i][N-1-i],Max);
		} 
		return Max;
}
//---------------------------------------------------------------
//                  Matrix Transpose 
//---------------------------------------------------------------
void Transpose(int Mat1[M][M], int Mat2[M][M]) {
	int i, j;
	for (i = 0; i < M; i++) {
		for (j = 0; j < M; j++) {
			Mat2[i][j] = Mat1[j][i];
			Mat2[j][i] = Mat1[i][j];
		}
	}
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
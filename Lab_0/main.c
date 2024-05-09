#include "func.h"

void show_menu(char** strlst) {
	int i;
	for (i = 0; strlst[i]; i++) 
		printf("%s\n", strlst[i]);
	return;
}

char* menu[] = {
	" --------------------- Menu --------------------------------",
    "-----------------------------------------------------------",
	"1) Create two 10x10 matrices Mat1,Mat2 using the next expression: for i,j = {0-9}: Mat[i][j] = (offset + i * 10 + j) % 100",
     "2) Compute and show the matrix Mat1+Mat2",
    "3) Compute and show the maximal element between Mat1 and Mat2",
"4) Compute and show the maximal Trace element matrices Mat1 and Mat2",
"5) Copy mat1 right diagonals to mat2 left diagonals",
"6) Compute and show the transposed of a given Matrix",
"7) Show a given Matrix",
"8) Show menu",
"9) exit",
	NULL
};




//-------------- Global variables ------------
  int maxTrace,maxDiag;
//--------------------------------------------
int main(){
        
  int Mat1[M][M], Mat2[M][M], auxMat[M][M], trace1, trace2;
  int matTrace,maxDiag,offset1=0, offset2=0;
  char Selector = '0', str[3];
  char choice[3];
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
		  sscanf(str,"%d",&offset1);
		  if (offset1 >= 0 && offset1 <= 100) break;
            
      } while (1);
	  FillMatrix(Mat1, offset1);
    
    do{
		  printf("Insert an offset value between 0-100: ");
          
		  gets(str);
		  sscanf(str,"%d",&offset2);
		  if (offset2 >= 0 && offset2 <= 100) break;    
	  
      } while (1);
      FillMatrix(Mat2, offset2);
      break;

    case '2': 
	  AddMatrix(Mat1, Mat2);
      break;

    case '3': 
      DiagonalsMax(Mat1, Mat2);
      break;
	

    case '4': 
	  trace1 = ComputeTrace(Mat1);
        trace2 = ComputeTrace(Mat2);
        if (trace1 >= trace2) printf("mat1 has the maximal Trace value: %d\n", trace1);
        else printf("mat2 has the maximal Trace value: %d\n", trace2);
	  
	  break;

	case '5':
	 copy_diag(Mat1, Mat2);
	 break;

	case '6':
	  do {
        printf("Insert the wanted matrix [Mat1 or Mat2]: ");
        
		  gets(str);
		  if (strcmp(str, "Mat1") == 0 || strcmp(str, "Mat2") == 0) break;
    } while (1);

	  if (strcmp(str, "Mat1") == 0) Transpose(Mat1, auxMat);
      else Transpose(Mat2, auxMat);
      show_matrix(auxMat);
      break;

case '7':
    do {
        printf("Insert the wanted matrix [Mat1 or Mat2]: ");
        
		  gets(str);
		  if (strcmp(str, "Mat1") == 0 || strcmp(str, "Mat2") == 0) break;
    } while (1);
    if (strcmp(str, "Mat1") == 0) show_matrix(Mat1);
    else show_matrix(Mat2);
    break;
            
    case '8':
        show_menu(menu);
        break;

    case '9':
        return 0;
    
	default:
    
	 printf("Try agian, You have inserted wrong number\n");
	 break;
  }
 }	
}

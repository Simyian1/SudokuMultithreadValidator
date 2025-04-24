// Sudoku puzzle verifier and solver
// COMPILE: gcc sudoku.c  -lm -pthread
// RUN: ./a.out text.tx

#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// takes puzzle size and grid[][] representing sudoku puzzle
// and two booleans to be assigned: complete and valid.
// row-0 and column-0 is ignored for convenience, so a 9x9 puzzle
// has grid[1][1] as the top-left element and grid[9]9] as bottom right
// A puzzle is complete if it can be completed with no 0s in it
// If complete, a puzzle is valid if all rows/columns/boxes have numbers from 1
// to psize For incomplete puzzles, we cannot say anything about validity

void deleteSudokuPuzzle(int psize, int **grid);

/* structure for passing data to threads */
typedef struct {
  int row;
  int column;
  int psize;
  int** grid;
  int lenGrid;
  int** validGrid;
} parameters;

bool validNum(int num)
{
  return (num > 0 && num <= 9);
}

bool existsInArray(int num, int* arr, int size)
{
  for(int i = 0; i < size; i++)
  {
    if(arr[i] == num) return true;
  }

  return false;
}

void* checkRow(void* args)
{
  parameters* data = (parameters*)args;
  int row = data->row;
  int psize = data->psize;
  int** grid = data->grid;
  int** validGrid = data->validGrid;
   //create amd initialize mem buffer
  int size = psize + 1;
  int* nums = (int *)malloc((size) * sizeof(int));
  for(int i = 0; i < size; i++)
  {
    nums[i] = -1;
  }
  //go through each collumn in the row
  for(int i = 1; i < size; i++)
  {
    if(!existsInArray(grid[row][i], nums, size) && validGrid[row][i] != -1)
      {
        validGrid[row][i] = 1;
      }
      else
      {
        validGrid[row][i] = -1;
      }
    nums[i] = grid[row][i];
  }
  //free
  free(data);
  free(nums);
  pthread_exit(NULL);
}


void* checkCol(void* args)
{
  parameters* data = (parameters*)args;
  int col = data->column;
  int psize = data->psize;
  int** grid = data->grid;
  int** validGrid = data->validGrid;
   //create amd initialize mem buffer
  int size = psize + 1;
  int* nums = (int *)malloc((size) * sizeof(int));
  for(int i = 0; i < size; i++)
  {
    nums[i] = -1;
  }
  //go through each row in the column
  for(int i = 1; i < size; i++)
  {
    if(!existsInArray(grid[i][col], nums, size) && validGrid[i][col] != -1)
      {
        validGrid[i][col] = 1;
      }
      else
      {
        validGrid[i][col] = -1;
      }
    nums[i] = grid[i][col];
  }
  //free
  free(data);
  free(nums);
  pthread_exit(NULL);
}

void* checkGrid(void* args)
{
  parameters* data = (parameters*)args;
  int row = data->row;
  int col = data->column;
  int psize = data->psize;
  int** grid = data->grid;
  int lenGrid = data->lenGrid;
  int** validGrid = data->validGrid;
  //create amd initialize mem buffer
  int size = psize + 1;
  int* nums = (int *)malloc((size) * sizeof(int));
  for(int i = 0; i < size; i++)
  {
    nums[i] = -1;
  }

  
  int endRow = row + lenGrid;
  int endCol = col + lenGrid;

  for(int i = row; i < endRow && i < size; i++)
  {
    for(int j = col; j < endCol && j < size; j++)
    {
      if(!existsInArray(grid[i][j], nums, size) && validGrid[i][j] != -1)
      {
        validGrid[row][col] = 1;
      }
      else
      {
        validGrid[row][col] = -1;
      }
      nums[i] = grid[i][j];
    }
  }
  //free
  free(data);
  free(nums);
  pthread_exit(NULL);
}

void checkPuzzle(int psize, int **grid, bool *complete, bool *valid) {
  //make valid grid, set every valid pos to 1, if any 0 left invalid
  int** validGrid = (int **)malloc((psize + 1) * sizeof(int *));
  for (int row = 1; row <= psize; row++) {
    validGrid[row] = (int *)malloc((psize + 1) * sizeof(int));
    for (int col = 1; col <= psize; col++) {
      validGrid[row][col] = 0;
    }
  }
  
  int lenGrid = sqrt((double)psize);
  int size = psize + 1;
  *valid = true;
  *complete = true;
  
  //thread index
  int threadI = 0;
  //number of threads is psize * 3 (rows and cols and grid checks),
  pthread_t* threads = (pthread_t*)malloc((sizeof(pthread_t) * (psize*3)));

  for(int i = 1; i < size; i++)
  {
    //data params
    parameters* dataR = (parameters*) malloc(sizeof(parameters));
    dataR->row = i;
    dataR->column = i;
    dataR->psize = psize;
    dataR->grid = grid;
    dataR->validGrid = validGrid;

    parameters* dataC = (parameters*) malloc(sizeof(parameters));
    dataC->row = i;
    dataC->column = i;
    dataC->psize = psize;
    dataC->grid = grid;
    dataC->validGrid = validGrid;

    //spawn threads to validate each column and row
    pthread_create(&threads[threadI++], NULL, checkCol, (void*)dataC);

    pthread_create(&threads[threadI++], NULL, checkRow, (void*)dataR);

    for(int j = 1; j < size; j++)
    {
      
      //maybe modulo row or column by lenGrid, and when remainder == 1 then spawn thread for grid validation
      if(i % lenGrid == 1 && j % lenGrid == 1)
      {
        //data params
        parameters* dataGrid = (parameters*) malloc(sizeof(parameters));
        dataGrid->row = i;
        dataGrid->column = j;
        dataGrid->psize = psize;
        dataGrid->grid = grid;
        dataGrid->validGrid = validGrid;
        dataGrid->lenGrid = lenGrid;

        //spawn threads to validate each column and row
        pthread_create(&threads[threadI++], NULL, checkGrid, (void*)dataGrid);
      }
    }
  }

  //join threads
  for(int i = 0; i < psize * 3; i++)
  {
    pthread_join(threads[i], NULL);
  }

  for(int i = 1; i < size; i++)
  {
    for(int j = 1; j < size; j++)
    {
      //check complete
      if(grid[i][j] == 0) *complete = false;
      //check valid
      if(validGrid[i][j] == 0 || validGrid[i][j] == -1)
      {
        *valid = false;
      }
    }
  }

  free(threads);

  //free valid grid
  deleteSudokuPuzzle(psize, validGrid);
  
}



// takes filename and pointer to grid[][]
// returns size of Sudoku puzzle and fills grid
int readSudokuPuzzle(char *filename, int ***grid) {
  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("Could not open file %s\n", filename);
    exit(EXIT_FAILURE);
  }
  int psize;
  fscanf(fp, "%d", &psize);
  int **agrid = (int **)malloc((psize + 1) * sizeof(int *));
  for (int row = 1; row <= psize; row++) {
    agrid[row] = (int *)malloc((psize + 1) * sizeof(int));
    for (int col = 1; col <= psize; col++) {
      fscanf(fp, "%d", &agrid[row][col]);
    }
  }
  fclose(fp);
  *grid = agrid;
  return psize;
}

// takes puzzle size and grid[][]
// prints the puzzle
void printSudokuPuzzle(int psize, int **grid) {
  printf("%d\n", psize);
  for (int row = 1; row <= psize; row++) {
    for (int col = 1; col <= psize; col++) {
      printf("%d ", grid[row][col]);
    }
    printf("\n");
  }
  printf("\n");
}

// takes puzzle size and grid[][]
// frees the memory allocated
void deleteSudokuPuzzle(int psize, int **grid) {
  for (int row = 1; row <= psize; row++) {
    free(grid[row]);
  }
  free(grid);
}

// expects file name of the puzzle as argument in command line
int main(int argc, char **argv) {
  if (argc != 2) {
    printf("usage: ./sudoku puzzle.txt\n");
    return EXIT_FAILURE;
  }
  // grid is a 2D array
  int **grid = NULL;
  // find grid size and fill grid
  int sudokuSize = readSudokuPuzzle(argv[1], &grid);
  bool valid = false;
  bool complete = false;
  checkPuzzle(sudokuSize, grid, &complete, &valid);
  printf("Complete puzzle? ");
  printf(complete ? "true\n" : "false\n");
  if (complete) {
    printf("Valid puzzle? ");
    printf(valid ? "true\n" : "false\n");
  }
  printSudokuPuzzle(sudokuSize, grid);
  deleteSudokuPuzzle(sudokuSize, grid);
  return EXIT_SUCCESS;
}
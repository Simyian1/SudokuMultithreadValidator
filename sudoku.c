// Sudoku puzzle verifier and solver

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

/* structure for passing data to threads */
typedef struct {
  int row;
  int column;
  int psize;
  int** grid;
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

bool checkRow(int row, int psize, int **grid)
{
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
    if(existsInArray(grid[row][i], nums, size))
    {
      return false;
    }
    nums[i] = grid[row][i];
  }
  //free
  free(nums);
  return true;
}


bool checkCol(int col, int psize, int **grid)
{
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
    if(existsInArray(grid[i][col], nums, size))
    {
      return false;
    }
    nums[i] = grid[i][col];
  }
  //free
  free(nums);
  return true;
}

bool checkGrid(int row, int col, int psize, int **grid, int lenGrid)
{
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
      if(existsInArray(grid[i][j], nums, size))
      {
        return false;
      }
      nums[i] = grid[i][j];
    }
  }

  return true;
}

void checkPuzzle(int psize, int **grid, bool *complete, bool *valid) {
  int lenGrid = sqrt((double)psize);
  int size = psize + 1;
  *valid = true;
  *complete = true;
  /*
  parameters* data = (parameters*) malloc(sizeof(parameters));
  data->row = 1;
  data->column = 1;
  data->psize = psize;
  data->grid = grid;

  pthread_t = thread;
  pthread_create(&thread, NULL, func, (void*)data);
*/
  for(int i = 1; i < size; i++)
  {
    //spawn threads to validate each column and row
    if(!checkCol(i, psize, grid) || !checkRow(i, psize, grid))
    {
      *valid = false;
      break;
    }
    for(int j = 1; j < size; j++)
    {
      //check complete
      if(grid[i][j] == 0) *complete = false;
      if(!validNum(grid[i][j]))
      {
        *valid = false;
        break;
      }
      //maybe modulo row or column by lenGrid, and when remainder == 0 then spaen thread for grid validation
      if(i % lenGrid == 1 && j % lenGrid == 1)
      {
        if(!checkGrid(i, j, psize, grid, lenGrid))
        {
          *valid = false;
          break;
        }
      }
    }
  }

  
  
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
/*
 * Copyright 2021. Heekuck Oh, all rights reserved
 * 이 프로그램은 한양대학교 ERICA 소프트웨어학부 재학생을 위한 교육용으로 제작되었습니다.
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <string.h>

typedef struct {
  int row;
  int column;
} location_t;  /** sudoku의 행/열 번호를 나타내는 구조체*/

// #define DEBUG  /** Log를 표시할 때 사용, 표시하지 않을때 주석처리*/

/*
 * 기본 스도쿠 퍼즐
 */
int sudoku[9][9] = {{6,3,9,8,4,1,2,7,5},{7,2,4,9,5,3,1,6,8},{1,8,5,7,2,6,3,9,4},{2,5,6,1,3,7,4,8,9},{4,9,1,5,8,2,6,3,7},{8,7,3,4,6,9,5,2,1},{5,4,2,3,9,8,7,1,6},{3,1,8,6,7,5,9,4,2},{9,6,7,2,1,4,8,5,3}};

/*
 * valid[0][0], valid[0][1], ..., valid[0][8]: 각 행이 올바르면 1, 아니면 0
 * valid[1][0], valid[1][1], ..., valid[1][8]: 각 열이 올바르면 1, 아니면 0
 * valid[2][0], valid[2][1], ..., valid[2][8]: 각 3x3 그리드가 올바르면 1, 아니면 0
 */
int valid[3][9];

/*
 * 스도쿠 퍼즐의 각 행이 올바른지 검사한다.
 * 행 번호는 0부터 시작하며, i번 행이 올바르면 valid[0][i]에 1을 기록한다.
 */
void *check_rows(void *arg)
{
  int contain_number[10];  /** 1부터 9까지 숫자가 포함되었는지 세는 배열*/

  #ifdef DEBUG
    printf("  [+]check_rows started.\n");
  #endif

  /* 모든 행마다, 각 행을 순회하여 중복되는 숫자가 있는지 검사한다*/
  for (int i = 0; i < 9; i++) {
    /* i번째 행 검사를 위한 배열 초기화*/
    memset(contain_number, 0, sizeof(contain_number));
    valid[0][i] = 1;
    
    /* i번째 행에서 각 숫자가 하나씩만 들어있는지 검사*/
    for (int j = 0; j < 9 && valid[0][i]; j++) {
      valid[0][i] *= (contain_number[sudoku[i][j]]++ == 0);  /* 검사 결과 업데이트*/
    }
  }

  #ifdef DEBUG
    printf("  [-]check_rows finished.\n");
  #endif

  pthread_exit(NULL);
}

/*
 * 스도쿠 퍼즐의 각 열이 올바른지 검사한다.
 * 열 번호는 0부터 시작하며, j번 열이 올바르면 valid[1][j]에 1을 기록한다.
 */
void *check_columns(void *arg)
{
  int contain_number[10]; /** 1부터 9까지 숫자가 포함되었는지 세는 배열*/

  #ifdef DEBUG
    printf("  [+]check_columns started.\n");
  #endif

  /* 모든 열마다, 각 열을 순회하여 중복되는 숫자가 있는지 검사한다*/
  for (int i = 0; i < 9; i++) {
    /* i번째 열 검사를 위한 배열 초기화*/
    memset(contain_number, 0, sizeof(contain_number));
    valid[1][i] = 1;
    
    /* i번째 열에서 각 숫자가 하나씩만 들어있는지 검사*/
    for (int j = 0; j < 9 && valid[1][i]; j++) {
      valid[1][i] *= (contain_number[sudoku[j][i]]++ == 0);  /* 검사 결과 업데이트*/
    }
  }

  #ifdef DEBUG
    printf("  [-]check_columns finished.\n");
  #endif

  pthread_exit(NULL);
}

/*
 * 스도쿠 퍼즐의 각 3x3 서브그리드가 올바른지 검사한다.
 * 3x3 서브그리드 번호는 0부터 시작하며, 왼쪽에서 오른쪽으로, 위에서 아래로 증가한다.
 * k번 서브그리드가 올바르면 valid[2][k]에 1을 기록한다.
 */
void *check_subgrid(void *arg)
{
  int contain_number[10],  /** 1부터 9까지 숫자가 포함되었는지 세는 배열*/
      row_begin,  /** 서브그리드 시작 행 인덱스*/
      col_begin,  /** 서브그리드 시작 열 인덱스*/
      subgrid_index;  /** 몇번째 서브그리드인지 표현*/
  location_t *locate = (location_t*)arg;

  /* 서브그리드의 인덱스 초기화*/
  row_begin = locate->row;
  col_begin = locate->column;
  subgrid_index = row_begin + col_begin / 3;

  /* 서브그리드 검사를 위한 배열 초기화*/
  memset(contain_number, 0, sizeof(contain_number));
  valid[2][subgrid_index] = 1;
  
  #ifdef DEBUG
    printf("  [+]check_subgrid index=%d started.\n", subgrid_index);
  #endif

  /* locate를 기준으로 3x3크기의 서브그리드를 검사한다.*/
  for (int i = row_begin; i < row_begin + 3; i++) {
    for (int j = col_begin; j < col_begin + 3 && valid[2][subgrid_index]; j++) {
      /* 검사 결과 업데이트*/
      valid[2][subgrid_index] *= (contain_number[sudoku[i][j]]++ == 0);
    }
  }

  #ifdef DEBUG
    printf("  [-]check_subgrid index=%d finished.\n", subgrid_index);
  #endif

  pthread_exit(NULL);
}

/*
 * 스도쿠 퍼즐이 올바르게 구성되어 있는지 11개의 스레드를 생성하여 검증한다.
 * 한 스레드는 각 행이 올바른지 검사하고, 다른 한 스레드는 각 열이 올바른지 검사한다.
 * 9개의 3x3 서브그리드에 대한 검증은 9개의 스레드를 생성하여 동시에 검사한다.
 */
void check_sudoku(void)
{
  int i, j;
  pthread_t tids[11];  /** 스레드 ID들을 담는 배열*/
  location_t *data;  /** 스레드에 전달할 스도쿠 인덱스*/
  
  /*
   * 검증하기 전에 먼저 스도쿠 퍼즐의 값을 출력한다.
   */
  for (i = 0; i < 9; ++i) {
    for (j = 0; j < 9; ++j)
      printf("%2d", sudoku[i][j]);
    printf("\n");
  }
  printf("---\n");
  /*
   * 스레드를 생성하여 각 행을 검사하는 check_rows() 함수를 실행한다.
   */
  if (pthread_create(&tids[9], NULL, check_rows, NULL) != 0) {
    printf("ERROR, check_rows thread creation error.\n");
    exit(-1);
  }
  /*
   * 스레드를 생성하여 각 열을 검사하는 check_columns() 함수를 실행한다.
   */
  if (pthread_create(&tids[10], NULL, check_columns, NULL) != 0) {
    printf("ERROR, check_colums thread creation error.\n");
    exit(-1);
  }
  /*
   * 9개의 스레드를 생성하여 각 3x3 서브그리드를 검사하는 check_subgrid() 함수를 실행한다.
   * 3x3 서브그리드의 위치를 식별할 수 있는 값을 함수의 인자로 넘긴다.
   * 이때 스레드 ID는 순서대로 tids[0] ~ tids[8]에 담긴다.
   */
  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      /* 인자로 보내줄 서브그리드의 시작 위치를 지정한다*/
      data = (location_t *)malloc(sizeof(location_t));
      data->row = 3*i;
      data->column = 3*j;

      /* 스레드를 생성하여 해당 서브그리드를 검사하는 check_subgrid() 함수를 실행한다.*/
      if (pthread_create(&tids[3*i + j], NULL, check_subgrid, data) != 0) {
        printf("ERROR, check_subgrid thread creation error.\n");
        exit(-1);
      }
    }
  }
  /*
   * 11개의 스레드가 종료할 때까지 기다린다.
   */
  for (i = 0; i < 11; i++) {
    if (pthread_join(tids[i], NULL) != 0) {
      printf("ERROR, pthread_join pid=%d error.\n", i);
    }
  }
  /*
   * 각 행에 대한 검증 결과를 출력한다.
   */
  printf("ROWS: ");
  for (i = 0; i < 9; ++i)
    printf(valid[0][i] == 1 ? "(%d,YES)" : "(%d,NO)", i);
  printf("\n");
  /*
   * 각 열에 대한 검증 결과를 출력한다.
   */
  printf("COLS: ");
  for (i = 0; i < 9; ++i)
    printf(valid[1][i] == 1 ? "(%d,YES)" : "(%d,NO)", i);
  printf("\n");
  /*
   * 각 3x3 서브그리드에 대한 검증 결과를 출력한다.
   */
  printf("GRID: ");
  for (i = 0; i < 9; ++i)
    printf(valid[2][i] == 1 ? "(%d,YES)" : "(%d,NO)", i);
  printf("\n---\n");
}

/*
 * 스도쿠 퍼즐의 값을 3x3 서브그리드 내에서 무작위로 섞는 함수이다.
 */
void *shuffle_sudoku(void *arg)
{
  int i, tmp;
  int grid;
  int row1, row2;
  int col1, col2;

  #ifdef DEBUG
    printf("[*] shuffle_sudoku started.\n");
  #endif

  srand(time(NULL));
  for (i = 0; i < 100; ++i) {
    /*
     * 0부터 8번 사이의 서브그리드 하나를 무작위로 선택한다.
     */
    grid = rand() % 9;
    /*
     * 해당 서브그리드의 좌측 상단 행열 좌표를 계산한다.
     */
    row1 = row2 = (grid/3)*3;
    col1 = col2 = (grid%3)*3;
    /*
     * 해당 서브그리드 내에 있는 임의의 두 위치를 무작위로 선택한다.
     */
    row1 += rand() % 3; col1 += rand() % 3;
    row2 += rand() % 3; col2 += rand() % 3;
    /*
     * 홀수 서브그리드이면 두 위치에 무작위 수로 채우고,
     */
    if (grid & 1) {
      sudoku[row1][col1] = rand() % 8 + 1;
      sudoku[row2][col2] = rand() % 8 + 1;
    }
    /*
     * 짝수 서브그리드이면 두 위치에 있는 값을 맞바꾼다.
     */
    else {
      tmp = sudoku[row1][col1];
      sudoku[row1][col1] = sudoku[row2][col2];
      sudoku[row2][col2] = tmp;
    }
  }

  #ifdef DEBUG
    printf("[*] shuffle_sudoku finished.\n");
  #endif

  pthread_exit(NULL);
}

/*
* 메인 함수는 위에서 작성한 함수가 올바르게 동작하는지 검사하기 위한 것으로 수정하면 안 된다.
*/
int main(void)
{
  int tmp;
  pthread_t tid;
  
  /*
   * 기본 스도쿠 퍼즐을 출력하고 검증한다.
   */
  check_sudoku();
  /*
   * 기본 퍼즐에서 값 두개를 맞바꾸고 검증해본다.
   */
  tmp = sudoku[5][3]; sudoku[5][3] = sudoku[6][2]; sudoku[6][2] = tmp;
  check_sudoku();
  /*
   * 기본 스도쿠 퍼즐로 다시 바꾼 다음, shuffle_sudoku 스레드를 생성하여 퍼즐을 섞는다.
   */
  tmp = sudoku[5][3]; sudoku[5][3] = sudoku[6][2]; sudoku[6][2] = tmp;
  if (pthread_create(&tid, NULL, shuffle_sudoku, NULL) != 0) {
    fprintf(stderr, "pthread_create error: shuffle_sudoku\n");
    exit(-1);
  }
  /*
   * 무작위로 섞는 중인 스도쿠 퍼즐을 검증해본다.
   */
  check_sudoku();
  /*
   * shuffle_sudoku 스레드가 종료될 때까지 기다란다.
   */
  pthread_join(tid, NULL);
  /*
   * shuffle_sudoku 스레드 종료 후 다시 한 번 스도쿠 퍼즐을 검증해본다.
   */
  check_sudoku();
  exit(0);
}

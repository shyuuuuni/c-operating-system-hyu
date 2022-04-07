/*
 * Copyright 2021. Heekuck Oh, all rights reserved
 * 이 프로그램은 한양대학교 ERICA 소프트웨어학부 재학생을 위한 교육용으로 제작되었습니다.
 */
#include <pthread.h>
#include <stdio.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>
#include <stdlib.h>
#include "threadpool.h"

/*
 * 스레드 풀의 FIFO 대기열 길이와 일꾼 스레드의 갯수를 지정한다.
 */
#define QUEUE_SIZE 10
#define NUMBER_OF_BEES 3

/*
 * Worker 스레드의 실행 상태를 지정한다.
 */
#define RUNNING 1

/*
 * 스레드 풀 대기열의 front index (나가는 위치) / back index (넣는 위치)
 * 현재 대기열의 수를 저장 (task_size)
 */
static int front;
static int back;
int task_size;

/*
 * 스레드를 통해 실행할 작업 함수와 함수의 인자정보 구조체 타입
 */
typedef struct {
    void (*function)(void *p);
    void *data;
} task_t;

/*
 * 스레드 풀의 FIFO 대기열인 worktodo 배열로 원형 버퍼의 역할을 한다.
 */
static task_t worktodo[QUEUE_SIZE];

/*
 * mutex는 대기열을 조회하거나 변경하기 위해 사용하는 상호배타 락이다.
 */
static pthread_mutex_t mutex;

/*
 * bee는 작업을 수행하는 일꾼 스레드의 ID를 저장하는 배열이다.
 * 세마포 sem은 카운팅 세마포로 그 값은 대기열에 입력된 작업의 갯수를 나타낸다.
 */
static pthread_t bee[NUMBER_OF_BEES];
static sem_t *sem;

/*
 * 대기열에 새 작업을 넣는다.
 * enqueue()는 성공하면 0, 꽉 차서 넣을 수 없으면 1을 리턴한다.
 */
static int enqueue(task_t t)
{
  int result; /* 성공 여부를 저장하는 변수 */

  pthread_mutex_lock(&mutex); /* Mutual Exclusion Lock 획득 */

  if (task_size < QUEUE_SIZE) {  /* 대기열에 자리가 있으므로 큐에 입력함 */
    worktodo[back] = t;
    back = (back + 1) % QUEUE_SIZE;
    result = 0;
    task_size++;
  } else {  /* 대기열에 자리가 없으므로 1을 리턴 */
    result = 1;
  }

  pthread_mutex_unlock(&mutex); /* Lock 해제 */

  return result;
}

/*
 * 대기열에서 실행을 기다리는 작업을 꺼낸다.
 * dequeue()는 성공하면 0, 대기열에 작업이 없으면 1을 리턴한다.
 * 성공하면 실행 할 task를, 실패하면 NULL을 리턴한다.
 */
static task_t* dequeue(void)
{
  task_t* task;
  pthread_mutex_lock(&mutex); /* Mutual Exclusion Lock 획득 */

  /* 세마포어로 꺼내올 수 있다는 것을 확인했으므로, front위치에서 Task를 꺼낸다. */
  task = &worktodo[front];
  front = (front + 1) % QUEUE_SIZE;
  task_size--;

  pthread_mutex_unlock(&mutex); /* Lock 해제 */

  return task;
}

/*
 * 풀에 있는 일꾼 스레드로 FIFO 대기열에서 기다리고 있는 작업을 하나씩 꺼내서 실행한다.
 */
static void *worker(void *param)
{
  task_t *task;
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,NULL);

  while (1) {
    sem_wait(sem);  /* 대기열에서 꺼낼 작업이 있을 때 까지 기다림. */
    task = dequeue(); /* 대기열에 작업이 있으므로, Queue에서 작업을 꺼내옴*/

    if (task != NULL) {
      task->function(task->data); /* 작업 실행 */
    }
  }

  pthread_exit(NULL); /* 스레드 종료 */
}

/*
 * 스레드 풀에서 실행시킬 함수와 인자의 주소를 넘겨주며 작업을 요청한다.
 * pool_submit()은 작업 요청이 성공하면 0을, 그렇지 않으면 1을 리턴한다.
 */
int pool_submit(void (*f)(void *p), void *p)
{
  task_t *task;
  int result;

  task = (task_t*) malloc(sizeof(task_t)); /* 실행시킬 작업의 구조체 할당 */
  task->function = f;
  task->data = p;

  if (enqueue(*task) == 0) {
    result = 0;  /* 대기열 추가 성공 */
    sem_post(sem); /* 대기열에서 대기하는 Task 수를 1 증가시킨다. */
  } else {
    result = 1;  /* 대기열 추가 실패 */
  }

  return result;
}

/*
 * 각종 변수, 락, 세마포, 일꾼 스레드 생성 등 스레드 풀을 초기화한다.
 */
void pool_init(void)
{
  /*
   * 전역 변수 초기화
   */
  front = 0;
  back = 0;

  /* named semaphore로 초기화한다. 초기값은 0으로 초기화한다. */
  sem = sem_open("sem_name", O_CREAT, 0777, 0);

  pthread_mutex_init(&mutex, NULL);  /* 상호배타용 뮤텍스 락 초기화 */

  for (int i = 0; i < NUMBER_OF_BEES; i++) {
    pthread_create(&bee[i], NULL, worker, NULL);
  }
}

/*
 * 현재 진행 중인 모든 일꾼 스레드를 종료시키고, 락과 세마포를 제거한다.
 */
void pool_shutdown(void)
{
  /*
   * 모든 일꾼 스레드 종료를 기다림
   */
  for (int i = 0; i < NUMBER_OF_BEES; i++) {
    pthread_cancel(bee[i]);
  }
  for (int i = 0; i < NUMBER_OF_BEES; i++) {
    pthread_join(bee[i], NULL);
  }

  sem_unlink("sem_name"); /* 세마포어 제거 */
  pthread_mutex_destroy(&mutex);  /* 상호배타용 뮤텍스 락 제거 */
}

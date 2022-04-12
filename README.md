# hyu-operating-system

2021년 3학년 1학기에 한양대학교 ERICA에서 수강한 운영체제론(COM2005) 과목의 프로젝트 과제를 정리한 저장소입니다.

## 구성

학기동안 총 4개의 프로젝트를 진행하였고, 각 프로젝트별로 아래와 같은 구조를 가지고 있습니다.

```bash
project
├── README.md           # 프로젝트 소개
├── img                 # 문서 작성에 필요한 이미지
│   └── *.png
├── requirement.md      # 프로젝트 요구사항
├── report.md           # 프로젝트 정리 및 느낀점
└── src
    ├── Makefile        # 컴파일을 위한 Makefile
    ├── execute_file    # 실행 파일
    └── project_file.c  # 원본 프로젝트 소스코드
```

## Project1 Simple UNIX Shell

첫번째 프로젝트로 진행한 [Simple UNIX Shell](project1-simple-unix-shell/README.md) 구현  프로젝트입니다.

쉘의 기본적인 기능인 명령어 실행, 리다이렉팅, 파이프를 활용한 입출력 전환 등의 기능을 구현한 간단한 쉘을 구현하는 프로젝트입니다.

## Project2 Sudoku Solution Validator

두번째 프로젝트로 진행한 [Sudoku Solution Validator](project2-sudoku-solution-validator/README.md) 구현 프로젝트 입니다.

여러 스레드를 사용하여 동시에 읽기와 쓰기가 진행되는 경우를 테스트하여 스레드 사용 시 주의사항을 학습하는 프로젝트입니다.

## Project3 Reader Writer Problem

세번째 프로젝트로 진행한 [Reader Writer Problem](project3-reader-writer-problem/README.md) 구현 프로젝트 입니다.

여러 유저가 위험구역(Critical Section)에 접근하는 문제점을 설명하는 대표적인 예시인 독자-저자 문제(Reader-Writer Problem)를 해결하는 여러 방법을 구현하는 프로젝트입니다.

## Project4 Thread Pool

네번째 프로젝트로 진행한 [Thread Pool](project4-thread-pool/README.md) 구현 프로젝트입니다.

큐(Queue)에 Task들을 저장하고, 스레드풀로 사용하는 3개의 Worker 스레드가 큐에 접근하여 Task를 가져와서 처리하는 프로그램을 구현하여 스레드풀을 학습하는 프로젝트입니다.
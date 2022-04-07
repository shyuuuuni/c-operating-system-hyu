# Reader-Writer Problem


## 소개

2021년도 1학기 한양대학교 ERICA 운영체제론 수업에서 진행한 세번째 프로젝트입니다. [프로젝트 요구사항](requirement.md)에 맞추어 공유 자원을 여럿이서 동시에 접근할 때 발생하는 문제를 Writer 선호 방식, 공정한 Reader Writer 방식으로 구현하여 비교해보는 실험입니다.

## 기능

본 프로그램은 두 개의 실행파일 ```writer_prefer```, ```pair_reader_writer``` 로 구성되어 있습니다.

실험의 조건은 다음과 같습니다.
    
    1. Reader는 10명 존재하며, 한 Reader당 하나의 알파뱃 대문자를 8192개를 출력한다.
    2. Writer는 3명 존재하며, 문자로 저장된 사람 얼굴 이미지를 출력한다.
    3. 위의 Reader와 Writer가 총 13개의 스레드에서 동시에 Critical Section에 접근하므로 충돌을 제어해 주어야 한다.
    4. Reader끼리는 중복해서 접근할 수 있다.

위 조건을 Writer 선호 방식과 공정한 Reader Writer 방식으로 진행한 결과를 각각 출력합니다.

## 실행방법

저장소의 파일들을 받은 후 다음 명령어를 입력합니다. 이후 13개의 스레드가 설정한 조건에 맞추어 문자를 출력한 뒤 프로그램을 종료합니다.

```shell
./writer_prefer # writer 선호

./fair_reader_writer # 공정한 reader writer
```

프로그램을 수정하고 컴파일 하기 위해서는 `gcc` 컴파일러 설치가 필요합니다.

> 만약 **Windows** 운영체제의 경우 **`pthread.h`** 라이브러리를 사용하기 위해 추가적인 설정이 필요할 수 있습니다.
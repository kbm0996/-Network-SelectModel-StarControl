# 네트워크 프로그래밍  모델
## 📢 개요
 Select 모델을 사용하여 하나의 서버에 다수의 플레이어가 참여할 수 있는 네트워크 프로그램. 각각의 클라이언트는 자신의 캐릭터(*)를 방향키로 움일 수 있고 다른 클라이언트의 캐릭터 또한 볼 수 있다.

  ![capture](https://github.com/kbm0996/SimpleShootingGame-Procedural-/blob/master/GIF.gif?raw=true)
  
  **figure 1. Star Control(animated)*

## 📌 주요 레퍼런스
### 1. select(int nfds, fd_set FAR * readfds, fd_set FAR * writefds, fd_set FAR * exceptfds, const struct timeval FAR * timeout);
동기 I/O를 수행하기 위해 대기하는 하나 이상의 소켓의 상태를 판정한다
 

>        WSADATA wsa;
>        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) 
>        // 매크로 함수 MAKEWORD(a, b) 인자의 두 BYTE를 하나의 WORD로 병합
>          err_quit(L"WSAStartup()"); // 예외 처리

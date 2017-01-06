# TheServer
C Socket Server, (also C++ image receiving socket server is included)

git push --set-upstream origin celine


   RPi     ->     PC
(clinet)       (Server)
jpg image    img process


the received image is processed in realtime in PC
in image processing program, location.txt is opened everytime newly.
the text file is located in TheServer directory.
The testserver exe file reads location.txt and hand it over to client
client receive char array and convert it into inteager.
if converted inteager is not in between 0~9, it's dropped.

Server Sends the refined serial location packet to Client 
2 bits for each x, y positions, 8 bits in total.



서버 status
B: 서버랑 클라이언트가 멍때리고 있다가 (클라이언트가 자신의 시리얼넘버만 보내는 상태)
M: 모바일에서 사용자 관련 정보들을 입력하면 서버에서 클라이언트한테 사용자 정보를 입력한다.
D: 클라이언트에서 다익스트라를 돌린 다음에 거리값을 반환한다. 
R:그런 다음에 서버에서 모든 거리값들을 비교한 다음에 서버에서 거리 반환한 모든 클라이언트들에게 선택된 클라이언트의 고유코드를 반환한다.
W:핸드폰으로부터 사용자가 차에 탔다는 신호를 기다린다.
A:만약 선택되면 선택된 차는 모터를 활성화 시키고 서버로부터 지속적으로 자신의 위치와 다른 자동차의 위치를 받는다. 
F:만약 자동차가 사용자의 목적지에 도착한다면 활성화 변수를 중단 시키고 서버한테 다시 멍때리는 상태가 된다. 

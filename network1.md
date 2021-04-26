# ARP poisoning 

### 0. What is ARP ??
![이미지](./picture.PNG)

- ARP는 주소를 해결하기 위한 일종의 프로토콜이다 수신측은 IP 주소를 기반으로 수신측의 호스트의 MAC주소를 알아낼 수 있다. 
- ARP table에 수신측의 IP주소에 대응하는 MAC주소가 있다면 table은 그 정보를 반환한다. 
#### 동작 과정 

- 상황 > 호스트 A가 호스트 B IP 주소를 알고 MAC주소를 알고 싶은 상황 (앞으로 호스트 A는 공격당하는 자로 칭함)
  1. 호스트 A는 네트워크 대역에 위의 그림처럼 '이 IP주소를 가지고 있는 장비있니?' 라는 ARP  request정보를 송신한다. 
  2. 네트워크 대역에 있는 해당 IP를 가지고 있는 장비는 ARP reply를 보낸다. (자신의 MAC 주소를 전송)
  3. ARP reply를 받은 호스트 A는 <IP , MAC> 정보를 자신의 ARP 테이블에 저장을 한다. 이후 통신은 이 ARP table을 참고한 후 통신이 진행이 된다. 
  4. 이 table의 경우 일정 시간이 지나면 없어지므로 , 없어진 순간 다시 1번 과정부터 다시 실행이 된다. (table 형식은 아래 그림참고)

![이미지](./arp table)


#### ARP 패킷의 형식 

![이미지](./arppacket)


### 1. ARP cache table poisoning

 - ARP protocol은 reply 정보가 들어오면 그 해당 패킷을 보낸 사용자의 신원을 확인 하지 않고 자신의 ARP table을 수정을 한다. 이럴 경우에 패킷을 받은 호스트 A(수신자)는 자신의 Table을 최신으로 
들어온 정보로 수정하게 한다. 



#### 공격 시나리오 
  - 호스트 A는 호스트 B의 MAC 주소를 알기 위해서 해당 네트워크 대역에 Broadcast를 한다. 
  - 해당 Packet을 받은 Attacer는 fake <IP , MAC> 주소를 호스트 A(공격당하는 자)를 보낸다.
  - 이 경우 호스트 A는 자신의 ARP table에 호스트 B의 정보를 fake <IP , MAC>으로 수정을 하게 된다. 
  - 호스트 A는 호스트 B(서버라고 가정)에 접속을 시도 하게 되면 Attacker가 만든 fake <IP, MAC>으로 접속을 하게 된다.  



- 이럴 경우에 다음의 나열된 공격을 시도 할 수 있다. 

  1. DoS attack
  2. Host impersonation 
  3. Man-in-the-middle
  4. etc

  #### 1. DoS attack
  - Attack는 호스트 A의 ARP table의 정보를 의 fake <IP, MAC>으로 수정을 하게 되면 실제로 가야하는 목적지에 패킷이 똑바로 전달 되지 못하게 된다. 
  
  #### 2. Host impersonation
  - 1번 공격과 다르게 실제로 통신을 해야하는 호스트 B가 아닌 Attacker가 호스트 B를 가장해서 정보를 빼낼 수 있다. 
  
  #### 3. Man-in-the-middle
  - 동시에 두개의 호스트를 poisoning했을 경우에 두개의 호스트는 자신끼리 통신을 하고 있다고 생각하지만 중간의 공격자가 패킷을 받고 수신하면서 안의 내용을 볼 수 있다. (구조 아래 그림참고)

![이미지](./arppacket)

  #### 4. DDOS(작성자 생각)
  - Attacker가 네트워크의 대역에 있는 모든 호스트들의 ARP table을 호스트 A의 <IP , MAC>으로 감염시키고, 특정 행동을 했을 때 계속해서 호스트 A에게 접속을 하게 되고 이 경우, 네트워크
    규모가 클 경우, 하나의 사용자는 계속해서 많은 패킷을 받기 때문에 정상적으로 작동하기 어려울 것이다. 



### 2. ARP cache table poisoning 실습


#### vm의 환경 구축

![이미지](./환경구축)

위의 그림을 보면 Kali랑 Windows 7이 통신을 하려하는 데 Attacker가 중간에 ARP spoofing을 하여 Windows 7의 table을 공격한다. 

- 초기 Windows 7 arp table 화면 

![이미지](./WindowsArptable)











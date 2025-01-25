## QoS服務質量

### 解決問題

* Lack of bandwidth
  * ![image-20200223202528567](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200223202528567.png)
  * 瓶頸是256kbps
  * upgrade the link
  * forward the important packets first(QoS)
  * compress the payload of layer 2 frames(it takes time)
  * compress IP packet headers
* End-to-end delay(fixed and variable)
* Type of delay
  * processing delay: the time ti tales for a router to tale the packet from an input interface, examine it, and put it into the output queue of the output interface
  * queuing delay : The time a packet resides in the output queue of a router
  * serialization delay : the time it takes to place the "bits on the wire"
  * propagation delay : the time it takes for the packet to cross the link from one end to the other
  * 總延遲: 所有延遲的總和
* Variation of delay(jitter)
  * 解決jitter使用緩衝技術
* Packet loss
  * telephone call
  * teleconferencing
  * publishing company
  * call center
  * Tail drop: 當queue滿了  鏈路壅塞就會造成尾丟棄
    * 解決:
    * 升級鏈路
    * 對敏感的封包做保證頻寬(QoS)
    * 防止較少的丟棄率在重要的 封包(QoS)

#### 哪些應用程式需要QoS

![image-20200223210754263](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200223210754263.png)

#### QoS定義

> The ability of the network to provide better or "special" service to a set of users or applications or both to the detriment of others applications or both



#### QoS Models

* Best effort : No QoS is applied t packets 
* IntServ : 集成的, 終端發出通知，預先讓路由器預留資源給應用
* DiffServ: 區分的
  * 流量進入: 分類 + 標記
  * queue : 決定發送順序

![image-20200224082020787](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200224082020787.png)

![image-20200224082117099](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200224082117099.png)

![image-20200224082232263](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200224082232263.png)

### MQC(Module QoS CLI):

1. 流量分類
2. 策略
3. 在接口調用QoS策略

![image-20200224082730644](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200224082730644.png)

#### Classification

* Incoming interface
* IP precedence
* DSCP
* source or destination address
* application
* 使用ACL分類

##### Marking

* Link layer:
  * CoS(ISL, 802.1p)
    * tag 的PRI 字段作為CoS值
    * CoS range 0-7
    * 0 : 最低優先值, 5 : 最高優先級, 6-7保留給通信協議使用
  * MPLS EXP bits
    * 3bits
  * Frame Relay
* Network layer:
  * IPv4: ToS Byte
  * ![image-20200224083921531](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200224083921531.png)
  * DSCP : 6個bits, 可分64個等級
  * IP precedence : 8個等級
  * IP precedence及DSCP是可兼容的



#### Per Hop Behaviors

* Default PHB
* EF PHB 1個 快速轉發 例如: 語音 值為DSCP101110
* AF PHB 12個 保證轉發級別 guarantee
  * AF1: 001
    * AF11 001010
    * AF12 001100
    * AF13 001110
  * AF2:010
    * AF21 010010
    * AF22 010100
    * AF23 010110
  * AF3 : 011
    * AF31 011010
    * AF32 011100
    * AF33 011110
  * AF4 : 100
    * AF41 100010
    * AF42 100100
    * AF43 100110

#### DSCP summary

![image-20200224101654247](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200224101654247.png)



### Mapping CoS to Network Layer QoS

![image-20200224102102362](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200224102102362.png)

### Trust Boundaries 信任邊界

![image-20200224102225477](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200224102225477.png)



#### ACL 區別流量

需求:

> 1. 對VOIP流量設定IP優先級為5
> 2. 對5.5.5.0過來的流量 訪問2.2.2.0的流量設定IP優先級為2
> 3. 對6.6.6.0過來的流量 訪問2.2.2.0的流量設定IP優先級為1

#### PBR 做標記

```
R1

access-list 100 permit udp any any range 16384 32767
access-list 101 permit ip 5.5.5.0 0.0.0.255 2.2.2.0 0.0.0.255
access-list 102 permit ip 6.6.6.0 0.0.0.255 2.2.2.0 0.0.0.255
!
route-map WOLF permit 10
 match ip address 100
 set ip precedence critical
!
route-map WOLF permit 20
 match ip address 101
 set ip precedence immediate
!
route-map WOLF permit 30
 match ip address 102
 set ip precedence priority
!
!input interface
interface Ethernet0/1     
 ip address 5.5.5.1 255.255.255.0
 ip policy route-map WOLF

```

### CBMariking

1. class-map
2. route-map
3. 調用

>1. 對VOIP流量 給予IP優先級5
>2. 對於telnet流量 給予IP優先級4
>3. 對於來自172.16.1.0的流量給予IP優先級2

```
access-list 100 permit udp any any range 16384 32767
access-list 103 permit tcp any any eq telnet
access-list 104 permit ip 172.16.1.0 0.0.0.255 any
!
!
class-map match-all VOIP
 match access-group 100
class-map match-all telnet
 match access-group 103
class-map match-all network172
 match access-group 104
!
!
policy-map WOLF
 class VOIP
  set ip precedence 5
 class telnet
  set ip precedence 4
 class network172
  set ip precedence 2
!
interface e0/1
 service-map input WOLF

```

#### NABR(Network-based Application Recognition)

> 基於第七層訊息對封包做分類
>
> * Statically assigned TCP and UDP port numbers
> * Non-UDP and non-TCP IP protocols
> * Dynamically assigned TCP and UDP port numbers negotiated during connection establishment

![image-20200224114107709](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200224114107709.png)

#### NBAR配置

1. 開啟CEF

   ```
   ip cef
   ```

2. 定義類別映射表

   ```
   class-map [maptch-any|match-all] TELNET
   	match protocol telnet 
   ```

3. 定義策略映射表

   ```
   policy-map XWX
   	class TELNET
   	 set ip precedence 3
   ```

4. 調用

   ```
   interface s0
   	ip nbar protocol-discovery
   	service-policy input XWX
   ```

#### PDLM(packet description language module)

需要先下載進flash 在進行加載 不需要更換IOS或重啟路由器

```
ip nbar pdlm br.pdlm

class-map match-any DROP
 match protocol pdlm bt
 
policy-map XWX
 class DROP
  drop
  
int s0
 
```



### Queuing

#### Congestion and Queuing

* congestion can occur at any point in the network where there are points of speed mismatched or aggregation
* Queuing manages congestion to provide bandwidth and delay guarantees.



#### Why cause congestion?

* Speed Mismatch
* Aggregation
* ![image-20200224142901923](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200224142901923.png)

### Queue

> Definition:
>
> Queuing is designed to accommodate temporary congestion on an interface of a network device by storing excess packets in buffers until bandwidth becomes available.

* FIFO first in first out 
* PQ priority queuing
* CQ custom queuing
* WFQ weighted fair queuing
* WRRQ weighted round robin queuing
* IP RTP 
* CBWFQ
* LLQ



packet--> software queuing system --> Hardware queuing----> output interface

​                                                                      (always FIFO)

#### Queuing Components

![image-20200224150059857](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200224150059857.png)

* Each queuing mechanism has three main components that define it:
  * Classification(select the class)
  * Insertion policy(determining whether a packet can be enqueued)
  * Service policy(scheduling packets to be put into the hardware queue)

##### FIFO先進先出

![image-20200224150607252](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200224150607252.png)

* 一個隊列
* 不分類
* 加隊 : 先到的封包先加隊列
* 調度 : 先進先出
* FIFO >= 2M, WFO < 2M

* 配置 : 

```
interface e0/0
 no fair-queue
! change to FIFO, WFQ eq fair-queue
```

```
show inter e0/0
  Queueing strategy: fifo
  Output queue: 0/40 (size/max)
```

修改隊列長度:

```
inter e0/0
hold-queue 50 out
```

##### Priority Queuing

![image-20200224162644262](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200224162644262.png)

* classify to 4 class
* Append packet to 4 FIFO queues
* Low delay transportation in high priority traffic
* low priority traffic is going to starving
* classification
  * source interface
  * packet size
  * TCP source or destination
  * UDP source or destination

* 配置:

1. 分類

```
access-list 100 permit icmp any any
```

2. 定義PQ

```
priority-list 1 protocol ip high tcp telnet
priority-list 1 protocol ip medium list 100
```

3. 調用

```
inter e0/0
priority-group 1
```

#### 調適

```
debug priority
```

version 15

```
access-list 100 permit icmp any any
class-map ICMP
 match access-group 100
 
policy-map type qos PQ1
 class ICMP
 priority level 2
 
interface e0/0
service-policy output PQ1
```





### Custom Queuing

![image-20200224220541419](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200224220541419.png)

* 16個隊列
* 輪流發送不同數量的bytes，只要quota足夠就會將封包全部發送，多的部分會在下一循環扣除
* 無法保證低延遲，只能保證頻寬。解決方法為增加一個編號為0的隊列，擁有最高優先級。(Pre-emptive Queues)

![image-20200224234153488](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200224234153488.png)

* 如果優先級隊列不足 可以再將循環隊列加入優先級隊列

* 配置

```
access-list 100 permit udp any any range 16384 32767
access-list 101 permit icmp any any

!定義CQ
queue-list 1 protocol ip 0 list 100
queue-list 1 protocol ip 1 tcp telnet
queue-list 1 protocol ip 2 list 101
!更改隊列傳送bytes數
queue-list 1 queue 1 byte-count 3000
!更改隊列長度
queue-list 1 queue 1 limit 40
!調用
inter e0/0
custome-queue-list 1

```

調適

```
debug custom-queue
```

##### 更改優先級隊列

```
queue-list 1 lowest-custom 1 
!從第二個隊列開始往下是循環隊列 以上為優先隊列
```





### CBWFQ

![image-20200225090138409](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200225090138409.png)

* 全自動配置 只要在路由器接口啟用即可
* 預設有256個隊列 最多可以達到4096個隊列
* 分類 分流 依照 來源 目的 端口號包含ToS等六個元素來分流 做hash運算
* ![image-20200225090756511](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200225090756511.png)
* 每個隊列的緩存長度在每個時間點都不相同 由調度中心調配
* WFQ的丟棄策略有兩種
  * Early dropping when the congestive discard threshold (CDT) is reached
    * 當達到壅塞丟棄閥值 進入最長隊列的類別將會被丟棄 
    * 進入非最長隊列的類別不會丟棄
    * 預設64
  * Aggressive dropping when the Hold-Queue Out limit (HQO) is reached
    * 當緩存中的封包達到HQO時
    * 封包進入最長隊列時 將被丟棄
    * 封包進入非最長隊列時 丟棄現在最長隊列的尾端封包
    * 預設1000
* WFQ scheduling
  * finish time
  * 序列號 = 輪次 + 虛擬包大小 = 輪次 + [實際包大小*(4096/(IP 優先級+1)]

![image-20200225155816175](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200225155816175.png)

* priority 0 4096
* priority 1 2048
* priority 2 1032



##### 配置

```
inter s0/1
fair-queue <1-4096> <16-4096>
!          CDT       DCQ
hold-queue 2000 out
!更改隊列長度
```



### CB-WFQ

![image-20200225161134655](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200225161134655.png)

#### 配置

> 在路由器出接口 使用CBWFQ為以下流量保證帶寬
>
> VOIP 30%
>
> HTTP 20%
>
> Other 25%

```
!access list
access-list 100 permit tcp any any eq www
!
!class map
class-map VOIP
match ip rtp 16384 16383
exit
!
!
class-amp HTTP
match access-group 100
!
!policy map
policy-map CBWFQ1
class VOIP
bandwidth percent 30
exit
class HTTP
bandidth percent 20
class default
bandwidth percent 25
```

```
!釋出所有保留帶寬
max-reserved-bandwidth 100
```

* 不能保證低延遲傳輸

### LLQ

* 為了減少延遲，增加一個LLQ(low latency queue)

![image-20200225163657651](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200225163657651.png)

> 在路由器出接口 使用CBWFQ為以下流量保證帶寬
>
> VOIP 30% LLQ
>
> HTTP 20%
>
> Other 25% WFQ

```
!access list
access-list 100 permit tcp any any eq www
!
!class map
class-map VOIP
match ip rtp 16384 16383
exit
!
!
class-amp HTTP
match access-group 100
!
!policy map
policy-map CBWFQ1
class VOIP
priority percent 30
exit
class HTTP
bandidth percent 20
class default
fair-queue
```

### IP RTP Prioritization

* IP RTP prioritization provides low-latency queuing when used in combination with WFQ or CBWFQ
* It can be used only for UDP traffic with predictable port numbers.
* It is usually used for VoIP traffic 
* IP RTP Prioritization is limited to prevent starvation of other traffic

```
inter e0/0
!可與CBWFQ共同使用
ip rtp priority 16384 16383 64
```







## 交換機上的排隊技術

#### WRRQ( Weighted Round-Robin)

* WRRQ在交換機上啟用後，每一個端口下都有四個隊列 每個隊列默認占用25%的帶寬 採用輪詢的方式來調度。可以把第四個隊列配置成絕對優先隊列，只有絕對優先隊列中的數據處理完成後，才會傳輸其他隊列的數據



* 配置實例:
  1. 將優先級為0 1的偵放入隊列一
  2. 將優先級為2 3的偵放入隊列二
  3. 將優先級為4 5的偵放入隊列三
  4. 將優先級為6 7的偵放入隊列四

```
mls qos
inter port
sw(config-if)# wrr-queue cos-map 1 0 1
sw(config-if)# wrr-queue cos-map 2 2 3
sw(config-if)# wrr-queue cos-map 3 4 5
sw(config-if)# wrr-queue cos-map 4 6 7 
```

* 更改帶寬所映射的權值

```
inter e0/0
wrr-queue bandwidth 1 2 3 4
```

1. 隊列一得到10%的帶寬
2. 隊列二得到20%的帶寬
3. 隊列三得到30%的帶寬
4. 隊列四得到40%的帶寬

* 啟用絕對優先隊列，固定為4號隊列

```
priority-queue out
```





### 限速(shaping & policy)

* Central to remote site speed mismatch
* Remote to central site oversubscription



#### Policing 流量監管

![image-20200225172103692](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200225172103692.png)

* incoming and outgoing directions
* Out-of-profile packets are dropped
* Dropping causes TCP retransmits
* Policing supports packet marking or re-marking

#### Shaping 流量碩塑型

![image-20200225172113679](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200225172113679.png)

* Outgoing direction only
* Out-of-profile packets are queued until a buffer gets full
* Buffering minimizes TCP retransmits
* Marking or re-marking not supported
* Shaping supports interaction with Frame Relay congestion indication



#### 令牌桶理論

![image-20200225184708408](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200225184708408.png)

* 封包通過時，需要領取令牌才能到出接口，領取後 令牌會消耗
* 每單位時間的補充的令牌數是受限制的
* 沒有令牌則會將封包丟棄
* Bc : is normal burst size, bits
* Tc : time interval (ms) 間隔時間
* CIR : 執行的資訊速率, Bc/Tc

![image-20200225185039224](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200225185039224.png)

* 為了使接口帶寬利用率更高，新增另一個Be桶(excess)，可以儲存令牌
* 在偶爾有流量的接口上，忽然出現流量，可以短暫的輸出大流量

![image-20200225190605225](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200225190605225.png)

* Shaping mechanisms:

  * Generic traffic shaping(GTS)

    * ![image-20200225191034446](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200225191034446.png)
    * 針對不同class做限速
    * token足夠時，進入硬體Q處理
    * token不足時，封包進入shaping Q進行處理
    * GTS is multiprotocol
    * GTS uses WFQ for the shaping queue
    * GTS can be implemented in combination with any queuing mechanisms:FIFO PQ CQ WFQ
    * GTS works on output interface
    * 配置:

  * ```
    interface e0/0
    traffic-shape rate 64000
    
    !分類限速
    access-list 100 permit udp any any range 1000 32768
    access-list 101 permit tcp any any eq www
    inter e0/0
    traffic-shape group 100 64000 8000 8000 1000
    traffic-shape group 101 256000 7936 7936 1000
    ```

  * Frame Relay traffic shaping(FRTS)

    * 偵中繼適應性流量塑型

  * Class-based shaping

    * Two shaping methods:
      * average rate
      * peak rate

  * 配置:

```
!1. 對VOICE流量 設定平均速率為64K
!2. 對於1.1.1.0網段訪問2.2.2.0網段的流量 設定最大速率為32k
access-list 100 permit ip 1.1.1.0 0.0.0.255 2.2.2.0 0.0.0.255

class-map VOIP
match  ip rtp 16384 16383
exit

class-map NET
match access-group 100
exit

policy-map CBR1
class VOIP
shape average 64000
exit
class NET
shape peak 32000
exit
!
inter e0/0
service-policy output CBR1
```

* Policing mechanisms:

  * Committed access rate(CAR)
    * ![image-20200225205912226](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200225205912226.png)
    * action : drop, transmit, continue
    * continue : 繼續往下匹配其他策略
    * 配置

  ```
  !1. 
  !
  inter e0/0
  rate-limit output 256000  2000      5000  conform-action transmit exceed-action drop
  !                         不可小於MTU Bc+Be
  ```

  * 對流量分類

  ```
  access-list 100 permit udp any any range 16384 32767
  access-list 101 permit tcp any any eq telnet
  
  inter e0/0
  rate-limit output access-group 100 64000 2000 5000 conform-action transmit exceed-action drop
  rate0limit output access-group 101 32000 2000 5000 conform-action transmit exceed-action drop
  ```

  * Class-based policing

    * 將流量分三種 拿令牌 借令牌 沒牌的

    * Conforms : 有令牌

    * Exceed : 借令牌

    * Violates : 沒令牌

    * 配置

    * ```
      !1. 對於VOIP流量 設定傳輸速率為64k 拿到令牌的設為EF優先級並傳輸, exceed 做普通包傳輸, violate drop
      !
      class-map VOIP
      match ip rtp 16384 16383
      exit
      
      policy-map WOLF
      class VOIP
      police cir 64000 conform-action set dscp-transmit ef exceed transmit violate-action drop
      !
      inter e0/0
      service-policy output WOLF
      ```

    * 

## WRED (Weighted Random Early Detection)

#### Tail Drop

> Tail drop should be avoided because it contains significant flaws:
>
> * TCP synchronization
> * TCP starvation
> * No differentiated drop

* TCP synchronization means that session may drop at the same time
  * TCP sessions restart at the same time

* TCP starvation

### 解決尾丟棄問題

* RED is a mechanism that randomly drops packets before a queue is full
* RED increases drop rate as the average queue size increases
* RED只能搭配FIFO
* three method:
  * No drop
  * random drop
  * drop all
* <最小閥值: No drop
* \>最小閥值, <最大閥值: Random drop
* \>最大閥值 Tail drop

![image-20200225214633600](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200225214633600.png)

* WRED
* use multiple different RED profiles
* Each profile is identified by:
  * Minimum threshold
  * Maximum threshold
  * Mark probability denominator
* WRED profile selection is based on :
  * IP precedence (8 profiles)
  * DSCP (64 profiles)
* WRED drops less important packets more aggressively than more important packets
* WRED can be applied at the interface, VC or class level 

![image-20200225215610697](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200225215610697.png)

* 在隊列滿載之前，依照壅塞程度 丟棄不同優先級的封包

* 在隊列滿了之後 tail drop

* 配置:

* ```
  inter e0/0
  random-detect
  
  show queueing random-detect
  !
  !修改class 的閥值
  inter e0/0
  random-detect precedence 5 36 40 20 
  
  !
  !基於dscp值丟棄
  inter e0/0
  random-dectect dscp-based
  ```

* CB-WRED

  * ```
    class-map GOLD
    match ip precedence 3 4
    class-map SILVER
    match ip precedence 1 2
    !
    policy-map POLICY1
    class GOLD 
    bandwidth percent 30
    random-detect
    random-detect precedence 3 20 40 10 
    random-detect precedence 4 20 40 10
    class SILVER 
    bandwidth percent 20
    random-detect
    random-detect precedence 1 15 35 10 
    random-detect precedence 2 20 35 10
    ```

  * 
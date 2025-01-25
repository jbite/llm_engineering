## 組播--PIM-DM協議原理

### 協議無關組播PIM

* 表示組播依靠單播的路由可以由靜態路由 OSPF IS-IS BGP等提供，組播路由和酖播路由協議無關，只要單播路由協議能產生所需路由表項，如RPF檢查即可。
* 協議號：103
* PIM路由器組播地址為：224.0.0.13
* PIM
  * DM --> 源樹(S, G)
  * SM --> 共享樹(*, G)
* ![image-20200304110216807](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200304110216807.png)

#### 組播路由協議PIM-DM

> Protocol independent Multicast
>
> 其含意是在做RPF檢查以及發送特定的協議單播報文的時候利用已有的單播路由表 與具體採用何種單播路由協議獲得此單播路由無關; DM, 即Dense mode 密集模式(用戶分布相對集中)

* 擴散
* ![image-20200304110440011](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200304110440011.png)
* 往上修剪
* 新加入的接收者，發送嫁接報文來加入組
* Prune Delay on multiaccess Network
  * ![image-20200304112636654](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200304112636654.png)
  * 上層會等待3秒
  * ![image-20200304112737234](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200304112737234.png)
  * 當層次多的時候，延遲的秒數就會變得非常高
  * RPF檢查
  * Assert機制
    * 多個同層路由器會自行選舉出一個主要路由器來發送組播包
    * ![image-20200304113450253](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200304113450253.png)
  * PIM-DM評價
    * 對於小型網路非常有效
      * 接收者密集非常適合
    * 優勢
      * 容易配置
      * 實現機制簡單(擴散 剪枝 嫁接)
    * 潛在問題
      * 擴散剪枝過程效率低
      * 複雜Assert機制
      * 控制和數據平面混合導致網路內部的所有路由器上都有(s,G)表項存在
      * 不支持共享樹
* Lab

![image-20200304120132747](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200304120132747.png)

* R1 R2 R3 R4 

* ```
  ip multicast-routing
  inter e0/1
  ip pim dense-mode
  inter e0/0
  ip pim dense-mode
  inter lo0
  ip pim dense-mode
  ```

* R5

* ```
  inter e0/0
  ip igmp join-group 238.1.1.10
  ```

  

```
R3#sh ip mroute
IP Multicast Routing Table
Flags: D - Dense, S - Sparse, B - Bidir Group, s - SSM Group, C - Connected,
       L - Local, P - Pruned, R - RP-bit set, F - Register flag,
       T - SPT-bit set, J - Join SPT, M - MSDP created entry, E - Extranet,
       X - Proxy Join Timer Running, A - Candidate for MSDP Advertisement,
       U - URD, I - Received Source Specific Host Report,
       Z - Multicast Tunnel, z - MDT-data group sender,
       Y - Joined MDT-data group, y - Sending to MDT-data group,
       V - RD & Vector, v - Vector
Outgoing interface flags: H - Hardware switched, A - Assert winner
 Timers: Uptime/Expires
 Interface state: Interface, Next-Hop or VCD, State/Mode

(*, 238.1.1.10), 00:01:01/stopped, RP 0.0.0.0, flags: D
  Incoming interface: Null, RPF nbr 0.0.0.0
  Outgoing interface list:
    Ethernet0/1, Forward/Dense, 00:01:01/stopped
    Ethernet0/0, Forward/Dense, 00:01:01/stopped

(1.1.1.1, 238.1.1.10), 00:01:01/00:01:58, flags: T
  Incoming interface: Ethernet0/0, RPF nbr 10.1.13.1
  Outgoing interface list:
    Ethernet0/1, Forward/Dense, 00:01:01/stopped, A

(*, 224.0.1.40), 00:04:41/00:02:22, RP 0.0.0.0, flags: DCL
  Incoming interface: Null, RPF nbr 0.0.0.0
  Outgoing interface list:
    Ethernet0/1, Forward/Dense, 00:04:32/stopped
    Ethernet0/0, Forward/Dense, 00:04:41/stopped
```

* 什麼情況下生成(*,G)表項

1. 共享樹
2. 最後一跳路由器收到IGMP的回覆後
3. 源樹每生成一個(S, G)就會生成一個(*, G)
4. 先有組播流量，才有組播表項

##### PIM 排錯

```
show ip pim interface [type][num]
!
show ip pimg neighbor
Mode: B - Bidir Capable, DR - Designated Router, N - Default DR Priority,
      P - Proxy Capable, S - State Refresh Capable, G - GenID Capable
Neighbor          Interface                Uptime/Expires    Ver   DR
Address                                                            Prio/Mode
10.1.12.2         Ethernet0/0              01:30:51/00:01:29 v2    1 / DR S P G
10.1.13.3         Ethernet0/1              01:30:51/00:01:24 v2    1 / DR S P G

!
R2
show ip rpf
RPF information for ? (1.1.1.1)
  RPF interface: Ethernet0/1
  RPF neighbor: ? (10.1.12.1)
  RPF route/mask: 1.1.1.1/32
  RPF type: unicast (ospf 1)
  Doing distance-preferred lookups across tables
  RPF topology: ipv4 multicast base, originated from ipv4 unicast base
!
!
R4
show ip igmp interface
!
show ip igmp groups
!

show ip mroute
```

* debug

```
debug ip mrouting

debug ip mpacket
```

#### PIM-DM TS思路

* IGP
* ip multicast-routing ip pimg dense-mode
* RPF
* ACL

#### 組播路由協議PIM-SM

* 
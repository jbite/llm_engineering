### MPLS VPN

#### VPN分類

![image-20200215145707656](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200215145707656.png)

Overlay VPN的本質是一種靜態VPN，這好比靜態路由

* 全部的配置與部屬需要手工完成，如果某客戶的VPN新增了一個節點，則需要完成如下工作
  * 在這個節點新增節點上建立與所有已存在的N個節點的隧道及相關的路由
  * 對於已存在的N個節點，需要在每個節點上都建立一個與新增節點之間的隧道及相關的路由
* 靜態VPN，無法反映網絡的實時變化

<div style="color:red">如果隧道建立在CE上，則必須由用戶維護，如果建立在PE上，則又無法解決地址衝突問題</div>
> CE(customer Edge) : 直接與服務提供商相連的用戶設備
>
> PE(Provider Edge Router): 指骨幹網上的邊緣路由器，與CE相連，主要負責VPN業務的接入
>
> P(Provider Router): 指骨幹網上的核心路由器，主要完成路由和快速轉發功能。由於網路規模不同，網路中可能不存在P路由器。PE路由器也可能同時是P路由器

![image-20200215150806360](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200215150806360.png)

#### Peer-to-Peer VPN 共享PE方式

(1) 地址衝突

(2) 提供一種動態建立的隧道技術

MPLS中的LSP正是一種天然的通道，而且這種隧道的建立是基於LDP協議，又恰恰是一種動態的標籤生成協議

(3) 動態建立隧道乘載的協議

IS-IS EIGRP BGP



BGP

1. 網絡中VPN路由數目可能非常大，BGP是唯一支持大量路由的路由協議
2. BGP是基於TCP建立連接，可以不直接相連的路由器間交換信息，這使得P路由器中無須包含VPN路由信息
3. BGP可以運載負加在路由器後的任何信息，作為可選的BGP屬性，任何不了解這些屬性的BGP路由器都將透明的轉發他們，這使在PE路由器間傳播路由非常簡單。



#### MPLS VPN概述

![image-20200215153345666](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200215153345666.png)

##### L3 MPLS/VPN模型特點

* 隧道承建：客戶設備透明 運營商設備維護
* 路油維護
* VPN數據封裝:MPLS標籤報頭

 ##### L3 MPLS VPN模型的優勢與劣勢

* 優勢:由運營商維護客戶路由 降低管理成本
* 劣勢: 路由信息被運營商獲取，且訊息沒有加密 

![image-20200215154010550](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200215154010550.png)

#### MPLS VPN 功能組件

![image-20200215154505718](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200215154505718.png)

 ##### VRF(Virtual ROuting and Forwarding)

* 每個VRF可以理解為一台虛擬邏輯路由器

* 每台支持VRF的路由器可以創建多個VRF
* 默認情況下，VRF之間，VRF與主路由器之間是物理隔離
* 一台PE路由器分配多個VRF來連接不同的客戶設備
* 實現同一PE下的不同網絡訊息的隔離需求



##### LAB

VRF下，各種動態路由協議配置

* RIP

```
router rip 
address family ipv4 vrf VRFNAME
version 2
network 0.0.0.0
```

* OSPF

```cisco
router ospf 1 vrf VRFNAME
router-id 1.1.1.1
network 0.0.0.0 0.0.0.0 a 0
```

* EIGRP

```
router eigrp 1234 # 任意數值皆可
address-family ipv4 vrf GREEN autonomous-system 47 # 需要跟CE上的EIGRP編號相同
network x.x.x.x wildcard
```

* BGP

```
ip vrf YELLOW
 rd 2:2
 route-target import 16:18
 route-target export 18:16
!
router bgp 1234
 bgp router-id 4.4.4.4
 no bgp default ipv4-unicast
 !
 address-family ipv4 vrf YELLOW
  neighbor 18.1.1.8 remote-as 8
  neighbor 18.1.1.8 activate
 exit-address-family
```


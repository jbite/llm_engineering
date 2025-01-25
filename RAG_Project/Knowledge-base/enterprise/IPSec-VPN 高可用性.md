## IPSec-VPN 高可用性

1. Reverse Route Injection(RRI)
2. Dead Peer Detection(DPD)
3. 高可用性VPN(鏈路備援)
4. 高可用性VPN(設備備援)
5. 高可用性VPN工程推薦設計

#### RRI(反向路由注入)示意圖

![image-20200228163628978](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200228163628978.png)

> RRI能夠為遠端隧道終點保護的網路和主機 在路由表內動態地插入相應的靜態路由 這些被保護的主機和網路也叫做remote proxy

* 無法進行鏈路負載

* 動態路由協議切換閘道

* 配置

* ```
  crypto map cry-map 10 ipsec-iskmap
  set reverse-route tag 10
  reverse-route static
  ```



#### DPD(dead peer detection)

* SA預設的消失時間為一小時
* 當peer掛掉 SA會持續存在active路由器上
* active路由器會一直注入靜態路由
* 需要主動偵測IPSec掛掉 清除dead SA讓standby 路由器起來
* HQ會使用RRI 及DPD兩種機制
* 週期性(periodic)
* 按需發送(on-demand)
* HQ及BR都需要設定DPD 因為會BRANCH會咬住原本的SA
* 兩邊的keepalive時間不需要一致
* 完成配置不會立即生效，需要clear crypto isakmp



#### 鏈路備援

![image-20200228174949448](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200228174949448.png)

* 包含的技術
  * DPD
  * RRI
  * Defualt peer config
  * Idle Timers(optional) : SA消失的時間



* 配置
* 所有IPSec設備都需要設定keepalive
* Branch 
  * 在第二階段中，設定兩個peer

```
crypto isakmp policy 10
 hash md5
 authentication pre-share
 group 2
crypto isakmp key cisco address 61.128.1.1
crypto isakmp key cisco address 28.9.3.1
crypto isakmp keepalive 10 periodic
!
!
crypto ipsec transform-set Tran esp-des esp-md5-hmac
!
crypto map CRYMAP 10 ipsec-isakmp
 set peer 61.128.1.1 default
 set peer 28.9.3.1
 set security-association idle-time 60
 set transform-set Tran
 match address VPN
!
!
interface Loopback0
 ip address 1.1.1.1 255.255.255.255
!
interface Ethernet0/0
 ip address 202.100.1.1 255.255.255.0
 crypto map CRYMAP
 no shutdown
!
ip route 0.0.0.0 0.0.0.0 202.100.1.10
!
ip access-list extended VPN
 permit ip 1.1.1.0 0.0.0.255 2.2.2.0 0.0.0.255
```

* Active
  * Active中設置reverse route
  * 將reverse route重發布至OSPF

```
crypto isakmp policy 10
 hash md5
 authentication pre-share
 group 2
crypto isakmp key cisco address 202.100.1.1
crypto isakmp keepalive 10 periodic
!
!
crypto ipsec transform-set Tran esp-des esp-md5-hmac
!
!
crypto map CRYMAP 10 ipsec-isakmp
 set peer 202.100.1.1
 set transform-set Tran
 set reverse-route tag 10
 match address VPN
 reverse-route
!
!
interface Ethernet0/0
 ip address 10.1.1.10 255.255.255.0
 ip nat inside
 no shutdown
!
interface Ethernet0/1
 ip address 61.128.1.1 255.255.255.0
 ip nat outside
 no shutdown
 crypto map CRYMAP
!
router ospf 1
 redistribute static subnets route-map TAG
 network 10.1.1.0 0.0.0.255 area 0
 network 0.0.0.0 255.255.255.255 area 0
!
!
ip nat inside source list NAT interface Ethernet0/1 overload
ip route 0.0.0.0 0.0.0.0 61.128.1.10
!
ip access-list extended NAT
 deny   ip host 2.2.2.2 host 1.1.1.1
 permit ip any any
ip access-list extended VPN
 permit ip 2.2.2.0 0.0.0.255 1.1.1.0 0.0.0.255
!
!
route-map TAG permit 10
 match tag 10

```

* Standby
  * standby中設置reverse route
  * 將reverse route重發布至OSPF

```
crypto isakmp policy 10
 hash md5
 authentication pre-share
 group 2
crypto isakmp key cisco address 202.100.1.1
crypto isakmp keepalive 10 periodic
!
!
crypto ipsec transform-set Tran esp-des esp-md5-hmac
!
!
!
crypto map CRYMAP 10 ipsec-isakmp
 set peer 202.100.1.1
 set transform-set Tran
 set reverse-route tag 10
 match address VPN
 reverse-route
!
!
interface Ethernet0/0
 ip address 28.9.3.1 255.255.255.0
 ip nat outside
 crypto map CRYMAP
 no shutdown
!
interface Ethernet0/1
 ip address 10.1.1.20 255.255.255.0
 ip nat inside
 no shutdown
!
router ospf 1
 redistribute static subnets route-map TAG
 network 10.1.1.0 0.0.0.255 area 0
 network 0.0.0.0 255.255.255.255 area 0
!
!
no ip http server
no ip http secure-server
ip nat inside source list NAT interface Ethernet0/0 overload
ip route 0.0.0.0 0.0.0.0 28.9.3.10
!
ip access-list extended NAT
 deny   ip host 2.2.2.2 host 1.1.1.1
 permit ip any any
ip access-list extended VPN
 permit ip 2.2.2.0 0.0.0.255 1.1.1.0 0.0.0.255
!
route-map TAG permit 10
 match tag 10

```

* Inside

```
interface Loopback0
 ip address 2.2.2.2 255.255.255.255
!
interface Ethernet0/0
 ip address 10.1.1.1 255.255.255.0
 no shutdown
!
router ospf 1
 network 2.2.2.2 0.0.0.0 area 0
 network 10.1.1.0 0.0.0.255 area 0

```

* Internet

```
interface Ethernet0/0
 ip address 61.128.1.10 255.255.255.0
 no shutdown
!
interface Ethernet0/1
 ip address 202.100.1.10 255.255.255.0
 no shutdown
!
interface Ethernet0/2
 ip address 28.9.3.10 255.255.255.0
 no shutdown
```



#### 高可用性VPN(設備備援)

![image-20200301102119445](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200301102119445.png)

* 使用HSRP來做設備冗餘
* Branch

```
crypto isakmp policy 10
 hash md5
 authentication pre-share
 group 2
crypto isakmp key cisco address 61.128.1.3
crypto isakmp keepalive 10 periodic
!
!
crypto ipsec transform-set Tran esp-des esp-md5-hmac
!
crypto map CRYMAP 10 ipsec-isakmp
 set peer 61.128.1.3
 set security-association idle-time 60
 set transform-set Tran
 match address VPN
!
!
interface Loopback0
 ip address 1.1.1.1 255.255.255.255
!
interface Ethernet0/0
 ip address 202.100.1.1 255.255.255.0
 crypto map CRYMAP
 no shutdown
!
ip route 0.0.0.0 0.0.0.0 202.100.1.10
!
ip access-list extended VPN
 permit ip 1.1.1.0 0.0.0.255 2.2.2.0 0.0.0.255
```

* Active

```
crypto isakmp policy 10
 hash md5
 authentication pre-share
 group 2
crypto isakmp key cisco address 202.100.1.1
crypto isakmp keepalive 10 periodic
!
!
crypto ipsec transform-set Tran esp-des esp-md5-hmac
!
!
crypto map CRYMAP 10 ipsec-isakmp
 set peer 202.100.1.1
 set transform-set Tran
 set reverse-route tag 10
 match address VPN
 reverse-route
 reverse-route static
!
!
interface Ethernet0/0
 ip address 10.1.1.10 255.255.255.0
 ip nat inside
 no shutdown
!
interface Ethernet0/1
 ip address 61.128.1.1 255.255.255.0
 ip nat outside
 standby 1 ip 61.128.1.3
 standby 1 preempt
 standby 1 name Redun
 no shutdown
 crypto map CRYMAP redundancy Redun
!
router ospf 1
 redistribute static subnets route-map TAG
 network 10.1.1.0 0.0.0.255 area 0
 network 0.0.0.0 255.255.255.255 area 0
!
!
ip nat inside source list NAT interface Ethernet0/1 overload
ip route 0.0.0.0 0.0.0.0 61.128.1.10
!
ip access-list extended NAT
 deny   ip host 2.2.2.2 host 1.1.1.1
 permit ip any any
ip access-list extended VPN
 permit ip 2.2.2.0 0.0.0.255 1.1.1.0 0.0.0.255
!
route-map TAG
 match tag 10
```

* Standby

```
crypto isakmp policy 10
 hash md5
 authentication pre-share
 group 2
crypto isakmp key cisco address 202.100.1.1
crypto isakmp keepalive 10 periodic
!
!
crypto ipsec transform-set Tran esp-des esp-md5-hmac
!
!
!
crypto map CRYMAP 10 ipsec-isakmp
 set peer 202.100.1.1
 set transform-set Tran
 set reverse-route tag 10
 match address VPN
 reverse-route
 reverse-route static
!
!
interface Ethernet0/0
 ip address 61.128.1.2 255.255.255.0
 ip nat outside
 crypto map CRYMAP redundancy Redun
 standby 1 ip 61.128.1.3
 standby 1 preempt
 standby 1 name Redun
 no shutdown
!
interface Ethernet0/1
 ip address 10.1.1.20 255.255.255.0
 ip nat inside
 no shutdown
!
router ospf 1
 redistribute static subnets route-map TAG
 network 10.1.1.0 0.0.0.255 area 0
 network 0.0.0.0 255.255.255.255 area 0
!
!
ip nat inside source list NAT interface Ethernet0/0 overload
ip route 0.0.0.0 0.0.0.0 61.128.1.10
!
ip access-list extended NAT
 deny   ip host 2.2.2.2 host 1.1.1.1
 permit ip any any
ip access-list extended VPN
 permit ip 2.2.2.0 0.0.0.255 1.1.1.0 0.0.0.255
!
route-map TAG permit 10
 match tag 10
```

* Inside

```
interface Loopback0
 ip address 2.2.2.2 255.255.255.255
!
interface Ethernet0/0
 ip address 10.1.1.1 255.255.255.0
 no shutdown
!
router ospf 1
 network 2.2.2.2 0.0.0.0 area 0
 network 10.1.1.0 0.0.0.255 area 0
```

### 高可用性VPN工程推薦設計

* GRE over IPSec
* 動態路由

![image-20200302101225810](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200302101225810.png)
## DMVPN

傳統VPN 的高擴展性問題

* 多次加密
* 每個客戶端須要維護過多的IPSec SA
* 每個客戶端都需要固定IP



#### DMVPN

特點:

* 簡單的HUB spoke配置提供了full mash連通性
* 支持spoke動態地址
* 增加新的spoke不需要改變hub配置
* spoke和spoke動態產生隧道觸發IPSEC加密

![image-20200302115244627](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200302115244627.png)

* 動態建立hub-to-spoke和spoke-to-spoke的IPSEC隧道
* 優化網路性能
* 降低實時運用的延時
* 減少hub路由器的配置 在不改變hub配置的情況下動態增加多個spoke隧道
* 0丟包
* 支持spoke路由器動態地址 HUB必須是固定IP地址
* 動態建立spoke-to-spoke IPSEC隧道 這些流量無須穿越hub
* 支持動態路由協議
* 支持hub和spoke組播 只支持hub到spoke
* 支持MPLS的VRF
* 擁有自癒能力 可有存在兩個hub
* 支持多個VPN中心設備的負載均衡

※ASA不支持DMVPN GRE SVTI GETVPN



#### DMVPN的組件

* MGRE
  * 點到多點
* NHRP
  * 二層的客戶服務解析協議 用於映射隧道地址到一個NBMA地址 功能類似於ARP和偵中繼的反向ARP 支持靜態映射和動態映射
* 動態路由協議
* IPSEC VPN

#### DMVPN拓樸

![image-20200302120406561](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200302120406561.png)

* 配置

  * Hub

    * 建立tunnel

    * ```
    interface Tunnel0
       ip address 172.16.1.100 255.255.255.0
     tunnel source 202.100.1.100
       tunnel mode gre multipoint
       tunnel key 123
      
      ```
  
    * 設定nhrp
  
    * ```
      inter tunn0
      ip nhrp authentication cisco
      ip nhrp map multicast dynamic
      ip nhrp network-id 10
      ip nhrp redirect
    ```
  
  * 解決分支接點流量經過hub
  
    ```
      !1.
      inter tunn 0
       no ip split-horizon eigrp 90
       no ip next-hop-self eigrp 90
       
      !2.
      !hub
      inter tunn 0
       no ip split-horizon eigrp 90
       ip nhrp redirect
      
       
      !需要先流量觸發
      !D   % 192.168.2.0/24 [90/28288000] via 172.16.1.100, 00:00:43, Tunnel0
      !D     192.168.100.0/24 [90/27008000] via 172.16.1.100, 00:00:43, Tunnel0
      ```
  
    * IPSec
  
    * ```
      crypto isakmp policy 10
       hash md5
       authentication pre-share
       group 2
      crypto isakmp key cisco address 0.0.0.0
      crypto ipsec transform-set Tran esp-des esp-md5-hmac
      crypto ipsec profile IPSECPROF
       set transform-set Tran
       
      inter tunn0
       tunnel protection ipsec profile IPSECPROF
      ```
  
    * 
  
  * Spoke
  
  * ```
    crypto isakmp policy 10
     hash md5
     authentication pre-share
     group 2
    crypto isakmp key cisco address 0.0.0.0
    crypto ipsec transform-set Tran esp-des esp-md5-hmac
    crypto ipsec profile IPSECPROF
     set transform-set Tran
     
    inter tunn0
     tunnel protection ipsec profile IPSECPROF
    interface Tunnel0
     ip address 172.16.1.1 255.255.255.0
     ip nhrp authentication cisco
     ip nhrp map 172.16.1.100 202.100.1.100
     ip nhrp map multicast 202.100.1.100
     ip nhrp network-id 10
     ip nhrp nhs 172.16.1.100
     ip nhrp shortcut
     tunnel source 202.100.1.1
     tunnel mode gre multipoint
     tunnel key 123
     tunnel	protection ipsec profile IPSECPROF
    ```
  
  * Spoke2
  
  * ```
    interface Tunnel0
     ip address 172.16.1.2 255.255.255.0
     no ip redirects
     ip nhrp authentication cisco
     ip nhrp map 172.16.1.100 202.100.1.100
     ip nhrp map multicast 202.100.1.100
     ip nhrp network-id 10
     ip nhrp nhs 172.16.1.100
     ip nhrp shortcut
     tunnel source 202.100.1.1
     tunnel mode gre multipoint
     tunnel key 123
    
    ```
  
  * 此例可使用AH來封裝IPSec
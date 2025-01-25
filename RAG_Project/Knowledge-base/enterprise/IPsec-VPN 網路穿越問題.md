## IPsec-VPN 網路穿越問題

* IPSec Profile

  * GRE over IPSec 使用IPsec Profile

  * ```
    !Site1
    !
    inter tun 0
    tunnel source 202.100.1.1
    tunnel destination 61.128.1.1
    ip add 100.1.1.1 255.255.255.0
    !
    !第一階段
    crypto isakmp policy 10
    encry des
    hash md5
    group 2
    authentication pre-share
    lifetime 3600
    crypto isakmp key 6 cisco address 203.1.1.3
    !
    !第二階段
    crypto ipsec transform-set Tran esp-des esp-md5-hmac
    exit
    !
    !定義IPsec profile
    crypto ipsec profile IPSECPROF
    set transform-set Tran
    !
    !在tunnel 中的流量皆是感興趣流
    int tun 0
    !直接套用IPsec profile到tunnel中
    tunnel protection ipsec profile IPSECPROF
    ```

  * 觀察加密封包

  * ```
    sh crypto engine connec active
    ```

* ISAKMP Profile

  * ISAKMP QoS運用

  * VRF VPN

  * 證書認證管理

  * 解決一個站點和多個站點，建立多種類型VPN的時候。普遍運用於一個設備和不同站點配置多個IPSEC隧道，並且每個站點需要不同第一階段策略的場合

  * IPSec use ISAKMP profile 配置

  * ```
    !Site1
    !
    !第一階段
    crypto isakmp policy 10
    encryption des
    hash md5
    group 2
    authentication pre-share
    lifetime 3600
    !
    !使用keyring來關聯profile
    crypto keyring site1
     pre-shared-key address 61.128.1.1 key cisco
    !
    !第二階段
    crypto ipsec transform-set Tran esp-des esp-md5-hmac
    mode tunnel
    exit
    !
    !定義ISAMP profile
    crypto isakmp profile ISAPROF
     match identity address 61.128.1.1 255.255.255.255
     keyring site1
     exit
    !
    !定義感興趣流
    ip access-list extend VPN
     permit ip host 1.1.1.1 host 2.2.2.2
     exit
    !
    !定義isakmp 
    crypto map CRYMAP 10 ipsec-isakmp
     set peer 61.128.1.1
     match address VPN
     set isakmp-profile ISAPROF
     set transform-set Tran
     exit
    !
    !在實體接口調用
    inter e0/1
     crypto map CRYMAP
    !
    ```

* ISAKMP profile QoS

  * ```
    crypto isakmp profile QOS
    keyring L2Lkey
    match identity
    qos group 2
    ```

* ISAKMP Profile 證書認證管理

  * ```
    crtypto pki certificate map cert.control 10
     issuer-name eq cn = ca.atom.org, o=atom, ou = atomguo
     
    crypto isakmp identity dn
    crypto isakmp profile isaprof
     ca trust point CA
     match certificate cert.control
    ```

* ISAKMP Profile VRF 應用

  * ```
    crypto isakmp profile VRF
    keyring L2LKey
    match identity address 202.100.1.1 255.255.255.255
    vrf SITE1
    ```

  * 實現不同VPN站點發過來的數據解密後放到不一樣的VRF中

  * ```
    !Tunnel
    !
    crypto isakmp policy 10
    auth
    hash
    encry
    group 
    !
    crypto keyring SITE2 
    pre-shared-key address x.x.x.x key cisco
    !
    !
    crypto isakmp profile ISAPROF
     match identity address 202.100.1.1
     keyring SITE2
    !
    crypto ipsec transform-set Tran esp-des esp-md5-hmac
    mode tunnel
    !
    crypto ipsec profile IPSECPROF
    set isakmp profile ISAPROF
    set transform-set Tran
    
    inter tunnel 0
    tunnel protection ipsec profile IPSECPROF
    ```

    

### 動態地址VPN解決方案

##### 動態MAP VS 靜態MAP

> 適用場合:
>
> HQ使用CISCO設備 且Brabch不是固定IP地址的情況，
>
> branch為CISCO使用EZVPN
>
> branch非cisco使用

### 動態VPN解決方案

![image-20200226155602502](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200226155602502.png)

Site1

```
crypto isakmp policy 10
 hash md5
 authentication pre-share
 group 2
crypto isakmp key cisco address 0.0.0.0
!
!
crypto ipsec transform-set Tran esp-des esp-md5-hmac
!
!
crypto dynamic-map DYMAP 999
 set transform-set Tran
!
!
crypto map CRYMAP 10 ipsec-isakmp dynamic DYMAP
!動態VPN不需要設定感興趣流，任何跟他建立VPN關係的即為感興趣流
!
interface Loopback0
 ip address 1.1.1.1 255.255.255.255
!
interface Ethernet0/0
 ip address 102.1.1.1 255.255.255.0
 crypto map CRYMAP

```

Site2

```
crypto isakmp policy 10
 hash md5
 authentication pre-share
 group 2
crypto isakmp key cisco address 102.1.1.1
!
!
crypto ipsec transform-set Tran esp-des esp-md5-hmac
!
!
!
crypto map CRYMAP 10 ipsec-isakmp
 set peer 102.1.1.1
 set transform-set Tran
 match address VPN
!
!
interface Loopback0
 ip address 2.2.2.2 255.255.255.255
!
interface Ethernet0/1
 ip address dhcp
 crypto map CRYMAP

```



### 動態域名解析技術

> 適用場合:
>
> 中心和分支都是動態獲取



### VTI

> IPSec自帶的隧道技術
>
> 可以隧道口上運用動態路由協議 並且不需要額外的4個bytes的GRE頭部 降低發送加密數據的帶寬

* SVTI用於L2L-VPN
* DVTI用於遠程撥號VPN





### IPSec VPN 網路穿越

#### 路由器NAT

* 出方向流量先經過NAT才會做VPN 進方向先解密後NAT回去
* 解決方法: 做NAT時，把感興趣流deny 不做NAT

```
ip access-list extend NAT
deny ip 感興趣流network
```

#### 中間設備NAT

* 防火牆將網路分為兩個區域



* 在PAT沒有設定DNAT下 只能由Site2先發起通道建立
* PAT有建立映射即可由Site1主動發起 udp 500 和udp 4500
* NAT-T的port號為4500

* ```
  crypto ipsec nat-transparency udp-encapsuplation
  ```

* 設定DNAT
  
  ```
  !                             inside local     inside global
ip nat inside source static udp 10.1.1.1 500 203.1.1.3 500
  ip nat inside source static udp 10.1.1.1 4500 203.1.1.3 4500
  ```
  



### IPSec VPN下 訪問控制的設定

> 1. Inside啟用telnet 並限制只允許2.2.2.0/24 網絡訪問1.1.1.0/24網路
> 2. P啟用HTTP

* 12.4以後，接口入方向的AVL的處理過程
  * layer 2的解封裝
  * Reverse crypto map ACL 
  * Inbounbd ACL 
  * IPSec解密
  * Inbound access crypto nmap ACL
  * Packet forwarding

* 12.4以後，接口出方向的ACL的處理過程

  * crypto to map ACL
  * Outbound access crypto map ACL
  * IPSec 加密
  * Outbound ACL
  * Layer2 封裝

* 在物理接口下配置ACL

* ```
  Extended IP access list VPNIn
      10 permit udp host 61.128.1.1 host 202.100.1.1 eq isakmp
      20 permit esp host 61.128.1.1 host 202.100.1.1
  ```

* 在crypto map 下配置ACL

* ```
  Extended IP access list crymap-inbound
      10 permit tcp 2.2.2.0 0.0.0.255 1.1.1.0 0.0.0.255 eq telnet
      20 permit tcp 2.2.2.0 0.0.0.255 eq www 1.1.1.0 0.0.0.255
  Extended IP access list crymap-outbound
      10 permit tcp 1.1.1.0 0.0.0.255 2.2.2.0 0.0.0.255 eq www
      20 permit tcp 1.1.1.0 0.0.0.255 eq telnet 2.2.2.0 0.0.0.255
  !
  !在解密策略下配置ACL
  crypto map CRYMAP 10 ipsec-isakmp
   set ip access-group crymap-inbound in
   set ip access-group crymap-outbound out
  ```

* 
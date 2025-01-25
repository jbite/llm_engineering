## PIM-SM

* 稀疏模式 主要用於組成員分布相對分散 分散較廣 大規模的網路
* PIM-SM實現組播轉發的核心任務是構造並維護兩棵樹
* 共享樹RPT和源樹SPT
* 共享樹選擇PIM域中某一路由器做為公用樹根，稱為匯聚點RP(Rendezvous Point) ，葉結點是最後一跳路由器
* 源樹選擇第一跳路由器做為某個組播組的樹根 葉結點是RP
* PIM-SM不依賴於特定的單播路由協議 而是使用現存的單播路由表進行RPF檢查
* RPF檢查根據樹的種類進行:
  * 使用共享樹進行數據接收轉發時，使用RP地址做為檢測地址
  * 使用源樹進行數據接收轉發時 使用組播源地址為檢測地址
* 組播數據沿著源樹轉發到RP 然後沿共享樹接收者轉發
* 共享樹的形成 : 接收者發送IGMP的join，由DR(最後一跳路由器)創建(*,G)項並向RP方向發送PIM join消息
* 源樹的形成 : 組播源發送組播流 通過DR(第一跳路由器)向RP方向轉發register報文 RP收到register報文後 向源發送PIM(S, G)join消息

![image-20200305084635276](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200305084635276.png)

RP選舉有動態及靜態方法

![image-20200305140148367](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200305140148367.png)

優點:

* RP 協助第一個封包加入組播
* RP負責與接收者建立(*, G)表項 所以適合接收者稀疏的環境
* SPT切換可以設定閥值 超過閥值後才切換

#### LAB

![image-20200305140751814](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200305140751814.png)

* 每一台路由器均靜態設置RP 包含R4自己

* RP 使用lookback 當作RP位址

  ```
  ip pim rp-address 4.4.4.4
  ```



### 組播路由協議PIM-SM RP形成

* 靜態RP

* ```
  ip pim rp-address 4.4.4.4 
  !
  !使用ACL控制不同組的RP
  ip pim rp-address 4.4.4.4 ACL
  !
  !使得靜態RP覆蓋動態RP
  ip pim rp-address 4.4.4.4 overrides
  ```

* Auto-RP

  * cisco私有

  * 路由器腳色

    * Candi-RP : announces its candidacy to the multicast address 224.0.1.39
    * MA(Mapping Agent) : collects the information about candidate router and announces it to the CIsco-Discovery multicast address 224.0.1.40
    * RP 選舉規則 : 
      * highest C-RP IP address
    * C-RP 可存在多個，MA也可以存在多個

  * 步驟:

    * 選MA

    * ```
      ip pim send-rp-discovery scope 10
      ```

    * 路由器向224.0.1.239發送競選消息

    * 宣告給所有MA，透過224.0.1.40傳遞，並透過group-list過濾加入的組

    * ```
      ip pim send-rp-announce [interface-type number] scope ttl [group-list acl-number] [interval seconds]
      ```

  * 遭遇問題 : 選舉RP需要傳送multicast，但是沒有RP無法傳送multicast

    * 解法: 
      * Sparse-dense-mode
      * ip pim autorp listen : 使auto-rp選舉去往224.0.1.39 和224.0.1.40的流量用SM發送 
      * 當MA及RP相鄰時，不需要

  * LAB

  ![image-20200305165606053](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200305165606053.png)

  ```
  R2#
  access-list 10 permit 239.1.1.1
  ip pim send-rp-announce Loopback0 scope 10 group-list 10
  ip pim send-rp-discovery scope 10
  
  R3#
  access-list 10 permit 239.1.1.1
  ip pim send-rp-announce Loopback0 scope 10 group-list 10
  ip pim send-rp-discovery scope 10
  ```

  

  ### BSR Bootstrap

> * 工業標準
> * C-RP/BSR
> * 兩次選舉
>   * 選Active-BSR
>   * 所有路由器選舉RP
> * 使用224.0.0.13當作hello包地址

#### BSR工作原理

1. C-BSR通過224.0.0.13組播地址 選出Active-BSR
2. 選出的BSR也會用224.0.0.13組播向外通告BSR地址
3. C-RP收到通告，會用單播把消息發向BSR
4. BSR收集到所有C-RP消息後，通過224.0.0.13通告出去
5. 所有葉路由器按照選舉規則選出RP

選舉規則:

* C-BSR priority最大的獲勝
* priority一樣時，IP值大的獲勝

葉路由器選舉:

1. 最長匹配
2. 最低優先級獲勝
3. hash值
4. 最高的IP地址



![image-20200305214121141](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200305214121141.png)

DR: 選擇誰來發(*, G)

1. priority
2. IP

DM的Assert: 發給下一層的路由器中，選擇誰來發組播

1. AD, Metric

BSR : 

1. priority
2. IP



### BSR LAB

```
ip pim bsr-candidate
ip pim rp-candidate
```

![image-20200305215059062](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200305215059062.png)

* RP流量控制

#### Controlling RP Acceptance

```
ip pim accept-rp {address | auto-rp } [group-access-lis-number]
```

* Configure

```
ip pim rp-announce-filter rp-list access-list-number group-list access-list-number
```

* Filter incoming Auto-RP announcement messages coming from a bogus Candidate RP
  * rp-list access-list

```
ip pim accept-register [list access-list-number] | [router-map map-name]
```

* Filter incoming Register messages


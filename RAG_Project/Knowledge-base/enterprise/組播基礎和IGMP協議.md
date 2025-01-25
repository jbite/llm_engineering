## 組播基礎和IGMP協議

1. 組播基礎
   * ![image-20200303112133963](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200303112133963.png)
   * 組播: 發送端只要發送一個封包，就可以讓特定接收者接收封包
   * 從服務器角度 : 減輕伺服器壓力 用戶數目的增加對server無影響 
   * 從應用角度，使多點應用成為可能
   * 從傳送網路角度 減少冗餘流量 網路中同一鍊路上同樣的組播數據只有一份
   * 劣勢: 
     * 基於UDP
     * 盡力而為--可靠組播方式解決
     * 沒有壅塞避免--有運行在有運行在TCP的組播
     * 報文重複--應用程式解決
     * 報文失序--應用程式解決
   * 組播地址結構
     * 組播IP地址
       * 組播地址沒有mask
       * 組播地址不會配在接口上
       * 組播地址沒有來源IP的概念
       * well-known 224.0.0.0-224.0.0.255
         * 224.0.0.1- all multicast systems on subnet
         * 224.0.0.2 - all routers on subnet
         * 224.0.0.4 - all DVMRP routers
         * 224.0.0.13 - all PIMv2 routers
         * 224.0.0.5, 224.0.0.6, 224.0.0.9, 224.0.0.10 used by unicast routing protocols
       * global range : 224.0.1.0- 238.255.255.255
         * 可在網際網路中傳遞的組播地址
       * limited :239.0.0.0/8 無法在互聯網使用
       * RFC 2770 GLOP addressing 233/8 申請AS時使用
       * SSM(Source Specific Multicast addressing) 232.0.0.0/8
     * 組播MAC地址
       * 組播MAC地址 第一個字節的最後一位為1
       * ![image-20200303195206964](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200303195206964.png)
       * 單播MC地址，第一個字節的最後一位為0
       * ![image-20200303195254445](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200303195254445.png)
       * 
     * 組播地址映射
2. IGMP協議原理
3. 組播分發樹
   1. 什麼是組播分發樹
      * 源樹 : 最短路徑樹，由組播源到用戶間的最短路徑構成
      * ![image-20200303200724030](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200303200724030.png)
        * 必須要有單播路由表才能有組播樹
        * 一個源 一棵樹
        * 路徑最短
        * 缺點 需要維護過多的樹
      * 共享樹
        * ![image-20200303203118055](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200303203118055.png)
        * 有一台路由器被選出來當作匯集點(RP, Rendezvous Point)，性能要好一點
        * 組播樹表項較少
        * 非最短路徑
        * 源到RP之間，還是源樹
        * RP到目的端之間是共享樹

#### RPF

> 逆向路徑轉發(Reverse Path Forwarding)

* RPF檢查

  * 從接口收到報文時，檢查源IP在本地路由表中出去，就由該接口接收組播封包

  * RPF校驗規則:

    * IP mroute 組播靜態路由 組播靜態路由只用來做RPF校驗
    * mp-bgp
    * IGP
      * AD
      * metric
      * IP

    以上規則由上至下校驗

* 組播不通 大部分原因為路由器沒接收組播包 需要確認IGP搞通



## IGMP協議原理

![image-20200303211818091](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200303211818091.png)

* Source進入第一個路由器叫做first hop
* 組播群最後一個路由器叫做Last hop
* 組播路由協議 PIM
  * 源樹或是共享樹
* 確保組播包不重複
* IGP
* LH到指定最後接收者 透過IGMP控制



### IGMP概述

* 用途 : IGMP協議是主機跟路由器之間的控制協議
* 主機通過IGMP協議向組播路由器報告自己想加入的組
* 路由器通過IGMP協議查詢網段上是否還有特定組的成員
* 當前IGMP對IPv4有三個版本
  * v1
  * v2
  * v3
* 對IPv6有兩版本
  * MLD v1
  * MLD v2

### IGMP v1

* 報文格式
* ![image-20200304083753462](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200304083753462.png)
  * 版本
  * 類型
    * 成員關係查詢(0x11)
    * 成員關係報告(0x12)
  * 校驗和
  * 組地址
    * 當一個成員關係報告正被發送時 組地址字段包含組播地址
    * 幫用於成員關係查詢時 本字段為0 並被主機忽略
  * v1組成員加入過程
    * 當一個主機希望接收一個組播組的數據 則發送成成員加入報告給組播組
    * 路由器RTA週期性的查詢 直第三次才發現組內有成員離開

### IGMPv2

* 格式
  * ![image-20200304084653168](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200304084653168.png)
  * 類型
    * 成員關係查詢(0x11)
      * 常規查詢 : 用於確定那些組播組是有活躍的 即該組是否還有成員在使用 常規查詢地址由全零表示
      * 特定組查詢 : 用於查詢某具體組播組是否還有組成員
    * 版本2成員關係報告(0x16)
    * 版本1成員關係報告(0x12)
    * 離開組消息(0x17)
  * 加入過程
    * 路由器發送查詢消息
    * 主機加入組，發送一個或多個版本2的成員關係報告給組播組
    * 離組時，發送一個離組消息
    * 指定組查詢
  * V2 IGMP封包類型
    * ![image-20200304091143001](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200304091143001.png)
    * 普通查詢
      * Dest IP : 224.0.0.1
      * Group : 0.0.0.0
    * 回復
      * Dest IP : 225.1.1.10 PC的組地址
      * Group : 225.1.1.10
    * 離開消息
      * Dest IP : 224.0.0.2
      * Group : 225.1.1.10
    * 指定組查詢
      * Dest IP : 225.1.1.10
      * Group : 225.1.1.10

## IGMPv3

![image-20200304092741962](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200304092741962.png)

* 多了一個指定源的查詢
* 服務於source 

![image-20200304092928737](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200304092928737.png)

### 三個版本比較

![image-20200304093017028](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200304093017028.png)

* 透過IGMP協議查詢，地址最小的成為查詢器

```
!發送端
ip multicast-routing
intger e0/0
ip pim sparse-dense-mode
ip igmp version 3
!接收端
inter e0/0
ip add 
ip igmp join-group 225.1.1.1
```

```
LH#sh ip igmp groups
IGMP Connected Group Membership
Group Address    Interface                Uptime    Expires   Last Reporter   Group Accounted
225.1.1.1        Ethernet0/0              00:01:36  00:02:38  10.1.1.1
!所有運行組播路由協議的路由器都會自行加入224.0.1.40
224.0.1.40       Ethernet0/0              00:03:27  00:02:35  10.1.1.2
```


### MPLS LDP

#### LDP( Label distribute protocol)

#### 協議概述

* LDP/TDP(cisco propriate)
* RSVP
* MP-BGP
* 每台運行LDP的LSR都會進行本地綑綁 也就是說為IPV4前綴分配標籤 再發送給LSR鄰居。這些訊息會儲存在LIB
* 所有綑綁某一特定前綴的remote label中，LSR只使用其中一個標籤來確定該前綴的出站標籤

#### LDP packets

##### Hello

* IP header  |  UDP header  |  LDP Hello MSG
* LDP鄰居建立首先發送Hello包(基於UDP 源目端口都是646)
* LDP ID為6個字節(4字節的IP加2字節的LABEL SpaceID)
* 兩個路由器建立LDP鄰居 要保證雙方的LDP ID三層可達
* transport addr除非手工指定，否則一般等於LDP ID
* LDP ID的選舉和OSPF routerID一樣

#### LDP鄰居關係建立過程

* LDP鄰居發現

![image-20200212113727974](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200212113727974.png)

接口IP

傳輸IP

router ID

>  建議手動配置router-ID

* LDP會話建立

![image-20200212114329971](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200212114329971.png)

* LDP標籤映射消息交互

![image-20200212115043528](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200212115043528.png)

#### PHP機制

* 次末跳彈出 
* 解決末端路由器查找本地路由 查一次LFIB 一次RIB表浪費資源的問題
* PHP直接在次末跳將標籤彈出，避免末端路由器多查一次
* expl-null 標籤值為0 會保留exp值 做QOS
* imp-null 標籤值3

#### Loop Detection

* LDP的環路檢測機制依賴於IGP協議
* 如果出現的環路(一般是IGO出現問題)，標籤中的TTL將防止標籤包無止盡的被轉發
* 標籤頭，中的TTL與IP頭中的TTL是一樣的通常拷貝自IP頭中的TTL值，這是TTL propagation
* TTLpropagation會有MPLS暴露的問題
* 關閉TTL propatation
  * no ip mpls ip propagation-ttl local

#### Frame-mode的標籤分配

* 貞模式的MPLS網絡標籤分配遵循

1. 建構IP路由表
2. 分配並分發標籤、維護LIB
3. 維護LFIB



#### 路由匯總對MPLS的影響

![image-20200213183808884](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200213183808884.png)

#### 保留的標籤

![image-20200213183750228](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200213183750228.png)

#### CISCO IOS platform switching mechanism

* process switching
* Fast switching
* CEF

#### 帶標籤報文的負載均衡



#### MPLS下的BGP

![image-20200214214827780](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200214214827780.png)

AS1234 MPLS backbone

R2 R3 沒有運行BGP

* 當R1R5網段及R4R6網段沒有透過OSPF通告，會造成R2做出PHP，而使得5.5.5.5及6.6.6.6的兩個網段不可達，因此解決方案就是使用R1及R4就是使用R1及R4的lookback接口建立IBGP鄰居關係
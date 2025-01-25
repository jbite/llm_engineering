#### MPLS

1. 透過IGP建立RIB![image-20200205223547368](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200205223547368.png)
2. LDP標籤交換協議位路由表在本地綑綁一個標籤，並傳遞給鄰居。將這些訊息記錄成LIB標籤資訊庫![image-20200205223651598](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200205223651598.png)

3. MPLS傳遞標籤交換訊息 建立LFIB。不可關閉CEF功能，否則LFIB無法建立

![image-20200205222111920](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200205222111920.png)

#### 小結

透過MPLS 可避免IP路由逐跳轉發情況 減少對數據包的深入分析

數據包在進入MPLS網路的入口路由器上被進行一次三層查找 而在此後的LSR只是進行簡單的標籤交換動作 無須進一步分析三層訊息

每個LSR必須在數據轉發之前建立好LIB及LFIB。當LSR收到一個標籤數據貞時 將數據貞的標籤在LFIB表中進行查找，再根據LFIB表中只是的相關動作對標籤進行壓入、彈出、交換、移除等動作



#### MPLS架構

##### control plane

* 交換三層路由訊息(如OSPF ISIS BGP等) 及標籤(如TDP LDP BGPv4 RSVP)
* Data plane: 基於標籤進行數據轉發
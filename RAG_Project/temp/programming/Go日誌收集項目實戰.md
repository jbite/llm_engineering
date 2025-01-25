









# Go 日誌收集項目實戰

![image-20200605221615214](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200605221615214.png)

### etcd特點

* 完全複製
* 高可用性:
* 一致性:
* 包括一個定義良好 面向用戶的API(gRPC)
* 快速:



## etcd應用場景

### 服務發現

分布式系統中當有新服務起來之後 需要設備自行發現新的服務並加入

伺服器與register需要有心跳來溝通 以保持最新狀態

![image-20200604140006435](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200604140006435.png)

### 配置中心

將配置訊息放到etcd上進行集中管理

etcd也可以使用watcher來通知訂閱者配置有更新

### 分布式鎖

使用raft算法可以保持etcd數據的一致性 實現方式有兩種 一是保持獨占 二是控制時序

* 保持獨占代表最終只有一個用戶可以得到
* 控制時序 所有想要獲得鎖的用戶都會被安排執行 但是獲得鎖的順序也是全局唯一的 同時也解決的執行順序

![image-20200604174232540](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200604174232540.png)

### etcd架構

![image-20200604174726902](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200604174726902.png)

### etcd集群

多使用3、5、7等奇數，使選舉較易 主要是透過raft來完成



### 使用etcd優化日誌收集項目

## 本周任務

1. Raft協議: 
   1. 選舉
   2. 日誌複製機制
   3. 異常處理(腦裂)
   4. zookeeper 的zad協議的區別


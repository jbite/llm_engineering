## Feature Netflow

> 概述: 不僅能記錄數據的數量 還可以記錄協議等信息。共有四個版本V1/V5/V7/V9 建議用V9
>
> Netflow對每個流量進行統計和分析，在每台設備獨立進行 不需要所有設備開啟



#### 配置條件

1. 必須開啟路由功能
2. CEF必須開啟
3. 消耗額外的CPU和記憶體



以下七元素相同 Netflow認為是同一個流

1. source IP
2. Destination IP
3. Source port
4. Destination port
5. Layer 3 protocol type
6. Type of Service
7. Input logical interface

> Netflow 透過UDP9991向遠端主機發送統計數據



#### Lab

![image-20200303083611180](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200303083611180.png)

* 配置

  * ```
    ip cef
    flow-sampler-map FLOW
     mode random one-out-of 10
    !
    !
    ip access-list extend ssh-acl
    permit tcp any any eq ssh
    !
    class-map match-all ssh-c
     match access-group name ssh-acl
    ! 
    !
    policy-map ssh-p
     class ssh-c
      netflow-sampler FLOW
    !  
    interface e0/0
     service-policy input ssh-p
    ! 
    !
    ip flow-capture vlan-id
    ip flow-capture mac-address
    ip flow-export version 9
    ip flow-top-talkers
     top 5
     sort-by packets
     match class-map ssh-c
     
    !Netflow 外部伺服器
    ip flow-export source e0/0
    ip flow-export destination 8.8.8.8 9991
    ```

  * 
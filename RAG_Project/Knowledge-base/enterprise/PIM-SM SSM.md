## PIM-SM SSM

Source Specific Multicast

* SSM 不需要RP
* 有一個特定的組播組範圍 232.0.0.0/8
* 透過PIM-SM及IGMPv3達成SSM

![image-20200306161434580](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200306161434580.png)

當receiver不支援IGMPv3 就需要LH來做映射

### LAB

![image-20200306161628262](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200306161628262.png)



Receiver 1#(只支援V3)

```
interface Ethernet0/0
 ip address 10.1.135.4 255.255.255.0
 ip igmp join-group 225.1.1.1 source 11.1.1.1
 ip igmp version 3

```

Receiver 2#(只支援V2)

```
interface Ethernet0/0
 ip address 10.1.135.5 255.255.255.0
 ip igmp version 2
 ip igmp join-group 225.1.1.3
 ip igmp join-group 225.1.1.2
```



R3#

```
!--------------V3----------
access-list 10 permit 225.1.1.1
access-list 10 permit 225.1.1.3
access-list 10 permit 225.1.1.2
ip pim ssm range 10
!
interface Ethernet0/0
 ip address 10.1.23.3 255.255.255.0
 ip pim sparse-mode

!----------V2---------------
ip pim ssm range 10
ip igmp ssm-map enable
no ip igmp ssm-map query dns
ip igmp ssm-map static 1 11.1.1.1
access-list 1 permit 225.1.1.2
access-list 1 permit 225.1.1.3

```




#### Prefix-list

* 路由匹配工具

>背景

![image-20200204225834642](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200204225834642.png)

```
access-list 1 deny 172.16.32.0
access-list 1 permit any

route-map test per 10
 match ip address 1
 
=======================================

access-list 1 permit 172.6.32.0
access-list 2 permit any

route-map test deny 10
 match ip address 1
route-map test permit 20
```



##### 如何用最精簡 最精確的標準ACL匹配下列路由

* 192.168.8.0/24
* 192.168.9.0/24
* 192.168.10.0/24
* 192.168.11.0/24

A:192.168.X.0 0.0.X.0

00001000

00001001

00001010

00001011

192.168.8.0  0.0.3.0



##### 使用擴展ACL匹配路由及掩碼



* 前綴列表的可控性比訪問列表高得多 支持增量修改更為靈活
* 判斷路由前綴與前綴列表中的前綴是否匹配
* 前綴列表包含序列號 從最小的開始匹配 默認序列為5 以5增加
* 如果前綴不與前綴列表中的任何條目匹配 將被拒絕

#### prefix-list的配置

> router(config)#ip prefix-list {list-name [seq number] {deny|permit} network/length[ge ge-value][le le-value]}



00000100

00000101

00000110

00000111

192.168.4.0/22 ge 24 le 24

* 匹配所有主機: ip prefix-list list 1 permit 0.0.0.0/0 ge 32
* 匹配所有路由: ip prefix-list list 1 permit 0.0.0.0/0 le 32


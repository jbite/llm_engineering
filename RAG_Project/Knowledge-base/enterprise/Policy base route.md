#### Policy base route 策略路由



##### 傳統路由

* 只看目的位址做判斷

##### 策略路由

* 同時比對來源及目的做判斷
* 應用
* 協議類型
* 報文大小
* 比傳統路由能力更強 更易控制數據流

![image-20200710170305111](D:\google\電子書\學習筆記\image-20200710170305111.png)

設定步驟:

1. 設定ACL

   ```
   ip access-list standard 2
   permit 172.16.1.0 0.0.0.127
   
   ip access-list standard 3
   permit 172.16.1.128 0.0.0.127
   ```

2. 套用route-map並設定下一跳位址

   ```
   route-map pbr-1 permit 10
   match ip add 2
   set ip next-hop 12.12.12.2
   route-map pbr-1 permit 20
   match ip add 3
   set ip next-hop 13.13.13.2
   ```

3. 套用至流量進入接口

   ```
   inter e0/2
   ip policy route-map pbr-1
   ```



在全局使本地始發的流量生效

ip local policy route-map map-tag



為了確保連通性 可使用set ip next-hop ip verify-available 此特性需有CDP支援(不建議)



較好的方法為SLA track

```
ip sla monitor responder
ip sla monitor 1
type echo protocol ipIcmpEcho 10.1.1.2 source-ipaddr 10.1.1.1
frequency 10

ip sla monior schedule 1 life forever start-time now
track 1 rtr 1 reachability
```



set ip recursive next-hop 



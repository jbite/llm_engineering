#### offset-list



##### 路徑控制

* 網絡實現冗餘 
  * 彈性:實現鏈路的主動切換同時被用鏈路可用於負載均衡
  * 可用性:從主鏈路切換到被用鏈路的時間
  * 自適應: 主鏈路壅塞時也可以使用冗餘路徑
  * 性能: 提高戴寬的使用率
* 路徑控制工具
  * 妥善的編址方案
  * 重分發和路由協議的特徵
  * passive-inteface
  * distribute-list
  * prefix-list
  * AD的把控
  * route-map
  * 路由標記
  * offset-list
  * cisco IOS IP SLAs
  * PBR

##### offset-list

用於在入站或出站時增大通過EIGRP或RIP獲悉的路由度量值

```
router(config-router)#offset-list {access-list-number|name}{in|out} offset [interface-type interface-number]
```

![image-20200205092258400](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200205092258400.png)

D路由器的配置

```
access-list 1 permit 10.1.1.0
router rip 
offset-list 1 out 2 serial 0/0
```

需求: 透過B為主要線路 D為備用線路

方法: D路由器透過offset-list增加跳數即可
#  LVS高併發解決方案

LB load balance

* 集群功能分類
  * LB
    * 軟體型: LVS HAproxy Nginx
    * 硬體型: F5 citrix netsclaer A10
  * HA: keepalive
  * HPC: 高性能集群，hadoop分布式計算集群
* LVS-DR
* LVS-NAT

http重定向

* 原理: 單純將http請求重定向到真實伺服器，所以用戶端又需要重新發送一次請求
* 優點: 比較簡單
* 缺點: 性能差

DNS負載均衡:

* 透過DNS解析一個網域名稱到多個IP之中，達到負載均衡

* 優點:省去設備建置的麻煩，透過運營商的DNS解決負載均衡的問題
* 缺點: 彈性不足

反向代理負載均衡:

* nignx
* squid

IP負載均衡

* LVS-NAT: 會將封包的目的位址改成真實的伺服器IP，透過NAT的技術實現
* 效率高，因為只看到封包頭的目的位址。但無法處理更高級的請求

硬體負載均衡

* BIG-IP提供12種算法將所有流量均衡的分配到各個伺服器

四層和七層負載

* 四層: LVS
* 七層: Nginx haproxy

代理:

* 正向代理: 內網到外網的代理
* 反向代理: 外網到內網的代理

### LVS(Linux Virtual Server)

> LVS工作在一台server上提供負載均衡的功能 本身並不提供服務 只是把特定的請求轉發給對應的realserver，從而實現集群環境中的負載均衡

#### LVS 工作模式

#### LVS輪詢算法

#### LVS-NAT

設定LVS

```
echo 1 > /proc/sys/net/ipv4/ip_forward
vi /etc/sysctl.conf
net.ipv4.ip_forward = 1
sysctl -p
yum install -y ipvsadm
ipvsadm -A -t 192.168.56.34:80 -s rr
        位址 tcp                算法 round-robin
ipvsadm -a -t 192.168.56.34:80 -r 192.168.142.137 -m
ipvsadm -a -t 192.168.56.34:80 -r 192.168.142.138 -m
touch /etc/sysconfig/ipvsadm
ipvsadm-save
cat /etc/sysconfig/ipvsadm
-A -t 192.168.56.34:80 -s rr
-a -t 192.168.56.34:80 -r 192.168.142.137:80 -m -w 1
-a -t 192.168.56.34:80 -r 192.168.142.138:80 -m -w 1
sysgtemctl restart ipvsadm
```

限制: 伺服器只能少於20台



#### LVS工作模式

* NAT轉發模式
  * 優點:
    *  網路隔離更安全
    * 節約IP地址
  * 缺點: 
    * 性能瓶頸在LVS上
* DR直接路由模式
  * LVS使用MAC地址來負載均衡請求，不會變更封包的SIPDIP
  * 當真實伺服器收到LVS轉進來的客戶端請求後，使用VIP當作SIP回覆客戶端請求。真實伺服器上面會使用LVS的VIP當作SIP，這個IP放在自己的lo0
  * LVS轉發請求給realserver時，使用MAC地址來區分不同主機，DIP不變 
* TUN-IP隧道模式
* FULL-NAT

#### LVS輪詢算法

* static schedule method
  * RR
  * WRR 加權輪詢
  * DH 目標地址hash
  * SH 源地址hash
* dynamic schedule method
  * LC 最少連接
  * WLC 加權最少連接
  * LBLC 基於本地的最少連接
  * LBLCR 帶複製的基於本地的最少連接

#### LVS-DR mode Lab

主機

* LVS: 

  * RIP 192.168.56.34 VIP 192.168.56.40

  * vi /etc/sysconfig/network-script/ifcfg-enp0s8

    ```
    IPADDR1=192.168.56.34
    IPADDR2=192.168.56.40
    NETMASK1=255.255.255.0
    NETMAKS2=255.255.255.255
    ```

  * 設定使用192.168.56.40處理封包

    ```
    route add -host 192.168.56.40 dev enp0s8
    ```

  * vi /etc/sysctl.conf

    ```
    net.ipv4.ip_forward = 1
    net.ipv4.conf.all.send_redirects = 0
    net.ipv4.conf.enp0s8.send_redirects = 0
    net.ipv4.conf.default.send_redirects = 0
    
    sysctl -p
    ```

  * 設置負載均衡設定

  * ```
    ipvsadm -A -t 192.168.56.40:80 -s rr
    ipvsadm -a -t 192.168.56.40:80 -r 192.168.56.37:80 -g
    ipvsadm -a -t 192.168.56.40:80 -r 192.168.56.38:80 -g
    touch /etc/sysconfig/ipvsadm
    ipvsadm-save 
    systemctl -restart ipvsadm
    ```

* webserver

  * web1: RIP 192.168.56.37 VIP 192.168.56.40
  * web2: RIP 192.168.56.38 VIP 192.168.56.40

* webserver配置lo網卡

  ```
  vi /etc/sysconfig/network-script/ifcfg-lo
  IPADDR=192.168.56.40
  NETMASK=255.255.255.255
  ```

* 配置web server內核參數 使得webserver忽略arp詢問

  ```
  echo "1" >/proc/sys/net/ipv4/conf/lo/arp_ignore
  echo "2" >/proc/sys/net/ipv4/conf/lo/arp_announce
  echo "1" >/proc/sys/net/ipv4/conf/all/arp_ignore
  echo "2" >/proc/sys/net/ipv4/conf/all/arp_announce
  ```

* ```
  vi /etc/sysctl.conf
  net.ipv4.conf.all.arp_ignore = 1
  net.ipv4.conf.all.arp_announce = 2
  net.ipv4.conf.default.arp_ignore = 1
  net.ipv4.conf.default.arp_announce = 2
  net.ipv4.conf.lo.arp_ignore = 1
  net.ipv4.conf.lo.arp_announce = 2
  ```

* 
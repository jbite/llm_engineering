## iptables及firewalld加固服務器安全

### 資訊安全概述

* 系統安全
  * 安全策略: SSH端口修改 SUDO 禁用管理員登陸
  * 文件指紋: 數據安全 selinux 自身程式用於本地文件的訪問 修改刪除監控
  * Linux軟體防火牆: 
    * 前身: 
* 網路安全
  * 網路防火牆: 
    * 應用防火牆:
      * WAF
    * 網路防火牆
    * 防毒牆:
      * 垃圾郵件病毒過濾
  * 上網行為管理
    * 分析應用程序 
  * 入侵檢測
    * IDS IPS
  * 傳輸安全
    * VPN
* Linux防火牆的建議

### iptables

* 主機型: 保護自己

* 網路型: 

* 缺點:

  * 防火牆雖然可以阻擋網際網路上的封包 卻無法過濾內部網路的封包
  * 電腦本身的操作系統也可能一些系統漏洞使入侵者可以利用這些漏洞繞過防火牆過濾
  * 防火牆無法有效阻擋病毒攻擊 尤其是隱藏在數據中的病毒
  * 防火牆會造成網路瓶頸

* table 及chain

  * 四個表: filter/nat/mangle/raw
    * 主要是filter和nat
  * 五個鏈:
    * prerouting(預路由)
    * postrouting(已路由)
    * input(入站)
    * output(出站)
    * forward(轉發)
  * 策略
    * drop 
    * accept
    * deny
  * 表的應用順序:
    * raw->mangle->nat->filter

* 語法

  * 基本語法

    * iptables -t 表名 管理選項 鏈名 匹配 匹配條件 [-j 控制類型]
    * -t 每個表都可以用 不寫預設是filter表
    * 管理選項: 操作方式-I -A -D -C等 
    * 鏈名: INPUT OUTPUT FORWARD 全部要大寫
    * 匹配條件: -p 協定 -d 目的位址 -s 來源地址
      * --sport/--dport
    * 控制類型: ACCEPT REJECT DROP LOG

  * 新增規則`iptables -t filter -I INPUT -p icmp -j DROP`

  * ```
    #拒絕單一端口訪問
    iptables -t filter -I INPUT -p tcp --dport 80 -s 192.168.56.10 -j REJECT
    ```

  * 標記位匹配(SYN RST ACK) --tcp-flags SYN|RST|ACK

  * icmp:

    * 0 icmp reply

    * 8 icmp request

    * ```
      iptables -I INPUT -p icmp --icmp-type 8 -j DROP
      ```

  * 企業真實防火牆政策:

    * ```
      IPT="/usr/sbin/iptables"
      $IPT --delete-chain
      $IPT --flush
      $IPT -P INPUT DROP #1
      $IPT -P FORWARD DROP #1
      $IPT -P OUTPUT DROP #1
      $IPT -A INPUT -m state --state RELATED,ESTABLISHED -j ACCEPT #2
      $IPT -A INPUT -p tcp -m tcp --dport 80 -j ACCEPT
      $IPT -A INPUT -p tcp -m tcp --dport 22 -j ACCEPT
      $IPT -A INPUT -p tcp -m tcp --dport 21 -j ACCEPT
      $IPT -A INPUT -p tcp -m tcp --dport 873 -j ACCEPT
      $IPT -A INPUT -i lo -j ACCEPT
      $IPT -A INPUT -p icmp -m icmp --icmp-type 8 -j ACCEPT
      $IPT -A INPUT -p icmp -m icmp --icmp-type 11 -j ACCEPT
      $IPT -A OUTPUT -m state --state RELATED,ESTABLISHED -j ACCEPT
      $IPT -A OUTPUT *p udp --dport 53 -j ACCEPT
      $IPT -A OUTPUT -p tcp --dport 80 -j ACCEPT
      $IPT -A OUTPUT -p icmp -m --icmp-type 8 -j ACCEPT
      $IPT -A OUTPUT -p icmp -m --icmp-type 11 -j ACCEPT
      
      ```

啟動轉發功能

```
echo "net.ipv4.ip_forwaed=1">>/usr/lib/sysctl.d/50-default.conf
echo 1 > /proc/sys/net/ipv4/ip_forward
sysctl -a | grep ip_forward
source /usr/lib/sysctl.d/50-default.conf
```

SNAT

```
iptables -FXZL
iptables -t nat -A POSTROUTING -s 192.168.100.0/24 -o out_interface -j SNAT --to-source 192.168.200.10
```

### firewalld

使用zone來管理

* trusted: 在這邊的流量全部放行
* home/internal
* work
* public: 僅允許部分服務通過
* external
* dmz
* block
* drop

常用端口

* http 80/http https 443/tcp ssh 22/tcp
* telnet 23/tcp
* dns 53/udp

firewall配置實戰

* 查看預設區域`firewall-cmd --get-default-zone`

* firewall-cmd --list-all-zone

  ```
  public (active)
    target: default
    icmp-block-inversion: no
    interfaces: enp0s3 enp0s8
    sources:
    services: ssh dhcpv6-client
    ports:
    protocols:
    masquerade: no
    forward-ports:
    source-ports:
    icmp-blocks:
    rich rules:
  ```

設定預設區域

```
firewall-cmd --set-default-zone=trusted
```

將http加入public區域

```
firewall-cmd --permanent --add-service=http --zone=public
```

將http移除public區域

```
firewall-cmd --permanent --remove-service=http --zone=public
```

認識普通區域

* DMZ
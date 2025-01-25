# Keepalive高可用集群

HA cluster 減少服務中斷時間為目的的集群

衡量標準: 使用MTTF mean time to failure 平均故障時間，平均修復時間(Mean time to restoration)

自動偵測: 由主機軟體中的功能通過多餘的偵測線 

brain seperate: 多台服務主機間的心跳線斷開時，會產生多台主機認為自己是master的狀況 會造成搶奪資源或是同時啟用服務的狀況



### LAB

webserver

* web1: 192.168.56.37

* web2: 192.168.56.38

* ```
  yum install -y keepalived
  ```

* 配置web1 master:

  ```
  vi /etc/keepalive/keepalive.conf
  ! Configuration File for keepalived
  
  global_defs {
     router_id 1
  }
  
  vrrp_instance VI_1 {
      state MASTER
      interface enp0s8
      mcast_src_ip 192.168.56.37
      virtual_router_id 51
      priority 100
      advert_int 1
      authentication {
          auth_type PASS
          auth_pass 123456
      }
      virtual_ipaddress {
          192.168.56.40
      }
  }
  
  ```

* 配置web2 backup

  ```
  ! Configuration File for keepalived
  
  global_defs {
     router_id 2 #兩台伺服器必須設置不一樣id
  }
  
  vrrp_script chk_nginx {
    script "/etc/keepalived/ck_ng.sh"  #偵測執行的shell路徑
    interval 2                         #檢查間隔
    weight -5                          #檢查失敗時，所要增減的priority,exit0為成功，exit1為失敗
    fall 3
  }
  
  vrrp_instance VI_1 {
      state BACKUP
      interface enp0s8
      mcast_src_ip 192.168.56.38
      virtual_router_id 51
      priority 99
      advert_int 1
      authentication {
          auth_type PASS
          auth_pass 123456
      }
      virtual_ipaddress {
          192.168.56.40
      }
      track_script {
        chk_nginx
      }
  }
  ```

#### 偵測nginx服務是否啟動

使用監控腳本監控nginx

```
vi /etc/keepalive/ck_ng.sh
#!/bin/bash

ps -C nginx --no-heading > /dev/null


#if [ $? -eq 1 ];then
#  systemctl start nginx
#  sleep 5
#  counter=$(ps -C nginx --no-heading|wc -l)
#  if [ $counter -eq 0 ];then
#    exit 1
# else
#    exit 0
#fi

```

### Keepalived + LVS

* LB

  * 192.168.56.34 master

  * 192.168.56.35 

  * VIP 192.168.56.36

  * 安裝keepalived ipvsadm，ipvsadm安裝但是不配置

  * 配置keepalived.conf

    ```
    ! Configuration File for keepalived
    
    global_defs {
       router_id Director1 #兩台伺服器必須設置不一樣id
    }
    
    vrrp_instance VI_1 {
        state MASTER
        interface enp0s8
        virtual_router_id 51
        priority 150
        advert_int 1
        authentication {
            auth_type PASS
            auth_pass 123456
        }
        virtual_ipaddress {
            192.168.56.36/24 dev enp0s8
        }
        
    }
    
    virtual_server 192.168.56.36 80 {
        delay_loop 3              # 服務輪詢的時間間隔
        lb_algo rr                # LVS調度算法
        lb_kind DR                #
        persistence_timeout 50
        protocol TCP
    
        real_server 192.168.56.37 80 {
            weight 1
            TCP_CHECK {
               connection_timeout 3
            }
        }
        real_server 192.168.56.38 80 {
            weight 1
            TCP_CHECK {
               connection_timeout 3
            }
        }
    }
    
    ```

  * 複製配置到lvs2 並修改priority及state設定

  * 啟動

    ```
    systemctl start keepalived
    systemctl enable keepalived
    ```

  * 

* webserver

  * 192.168.56.37 web1

  * 192.168.56.38 web2

  * 安裝網站服務httpd 或是nginx

  * arp 新增設定忽略配置

  * ```
    vi /etc/sysctl.conf
    net.ipv4.conf.all.arp_ignore = 1
    net.ipv4.conf.all.arp_announce = 2
    net.ipv4.conf.default.arp_ignore = 1
    net.ipv4.conf.default.arp_announce = 2
    net.ipv4.conf.lo.arp_ignore = 1
    net.ipv4.conf.lo.arp_announce = 2
    ```

  * 路由配置 寫到開機執行

    ```
    vi /etc/rc.local
    /sbin/route add -host 192.168.56.36 dev lo:0
    ```

  * reboot

觀察


### Kubernetes

### K8S簡介

容器的分布式架構方案 為容器化的應用提供部屬運行 資源調度 服務發現和動態伸縮等一系列功能

舉有集群管理能力 多層次的安全防護和准入機制 透明的服務註冊和發現機制 內建智能負載均衡器 強大的故障發現和自我修復能力 服務滾動升級和在線擴容能力

#### 特性

* 自動裝箱
  * 購建於容器之上 機於資源依賴及其他
* 自我修復
  * 故障後自動重啟 節點故障後重新調度容器
* 水平擴展
  * 通過命令行或是UI手動水平擴展
* 服務發現和負載均衡
* 自動發布和回滾
* 存儲編排
* 批量處理執行

### 四組基本概念

* Pod/Pod 控制器：
  * k8s中能夠被運行的最小邏輯單元
  * 一個Pod中可以運行多個容器，它們共享UTS+NET+IPC名稱空間
  * 可以理解為豌豆莢，而同一pod內的每個容器是一顆顆豌豆
  * Pod控制器是pod啟動的一種模板，用來保証在k8s裡啟動的pod應始終按照人們的預期運行
  * k8s內提供了眾多的pod控制器，常用的有以下幾種：
    * Deployment
    
    * DaemonSet：
    
      * 用來確保全部或一些Node上運行一個Pod的副本。當有Node加入集群時，也會為他們新增一個Pod。當有Node從集群移除時，這些Pod也會被回收。刪除DaemonSet將會刪除它創建的所有Pod
    
        使用DaemonSet的典型用法：
    
        * 運行集群存儲daemon，例如在每個Node上運行glusterd、ceph
        * 在每個Node上運行日誌收集daemon，例如fluentd、logstash
        * 在每個Node上運行監控daemon，例如prometheus Node Exporter
    
    * ReplicaSet：
    
      * ReplicationController ：用來確保容器應用的副本數始終保持在用戶定義的副本數，即如果有容器異常退出，會自動創建新的Pod來替代，而如果異常多出的容器也會自動回收。
    
        在新版本中，建議使用RelicaSet來取代ReplicationController
    
        
    
        ReplicaSet跟ReplicationController沒有本質上的不同，只是名字不一樣，並且ReplicaSet支持集合式的selector
    
        
    
        雖然ReplicaSet可以獨立使用，但一般還是建議使用Deployment來自動管理ReplicaSet，這樣就無需擔心跟其他機制的不兼容問題（比如ReplicaSet不支持rollingupdate但deployment支持）
    
    * StatefulSet：
    
      * 是為了解決有狀態服務的問題（對應Deployment和ReplcaSets是為無狀態服務而設計），其應用場景包括：
        * 穩定的持久化存儲，即Pod重新調颯後還是能訪問到相同的持久化數據，基於PVC來實現
        * 穩定的網路標識，即Pod重新調度後其PodName和HostName不變，基於Headless service來實現。
        * 有序部署，有序擴展
        * 有序收縮，有序刪除
    
    * Job：負責批處理任務，即僅執行一次的任務，它保證批處理任務的一個或多個Pod成功結束
    
    * Cronjob：管理基於時間的job
* Name/Namespace：
  * k8s使用資源來定義每一種邏輯概念（功能），故每種資源都應該有自己的名稱
  * 資源有api版本、類別、元數據、定義清單、狀態等配置信息
  * 名稱通常定義在資源的元數據信息裡。
  * namespace
    * 隨著項目增多、人員增加、集群規模的擴大，需要一種能夠隔離k8s內各種資源的方法，這就是名稱空間
    * 名稱空間可以理解為k8s內部的虛擬集群組
    * 不同名稱空間內的資源，名稱可以相同，相同名稱空間內的同種資源名稱不能相同
    * 合理的使用k8s的名稱空間，使得集群管理員能夠更好的對交付到k8s裡的服務進行分類管理和瀏覽
    * k8s裡默認存在的名稱空間有，default、kube-system、kube-public
    * k8s裡特定資源要帶上相應的名稱空間
* Label/Label選擇器：
  * Label
    * 標籤是k8s特色的管理方式，便於分類管理資澦對象
    * 一個標籤可以對應多個資源，一個資源也可以有多個標籤，它們是多對多的關係
    * 一個資源擁有多個標籤，可以實現不同維度的管理
    * 標籤的組成：key=value
    * 與標籤類似的，還有一種注解（annotations）
  * label選擇器
    * 給資源打上標籤後，可以使匆標籤選擇器過濾指定的標籤
    * 標籤選擇器目前有兩個：基於等值關系（等於、不等於）和基於集合關系（屬於、不屬於、存在）
    * 許多資源支持內嵌標籤選擇器字段
      * matchLabels
      * matchExpressions
* Service/Ingress
  * Service
    * 在k8s的世界裡，雖然每個pod都會被分配一個單獨的IP地址，但這個IP地址會隨著pod的銷毀而消失
    * service（服務）就是用來解決這個問題的核心概念
    * 一個service可以看作一組提供相同服務的pod的對外訪問接口
    * service作用於哪些pod是通過標籤選擇器來定義的
  * ingress
    * Ingress是k8s集群裡工作在OSI網路參考模型下，第七層的應用，對外暴露的接口。
    * service只能進行L4流量調度，表現形式是ip+port
    * ingress則可以調度不同業務域，不同URL訪問路徑的業務流量

### K8S集群組件

典型的集群由

* 多個工作節點

* 一個主節點
* 一個集群狀態存儲系統(ETCD)組成：鍵值對資料庫，儲存k8s集群的重要資料

Master

>  負責管理工作 為集群提供管理接口 並監控和編排集群中的各工作節點
>
>  MASTER也可以多副本的方式同時運行於多個節點提供HA 
>
>  是統一訪問入口

API server

> 負責RESTFUL風格的api，提供集群管理的restfulAPI接口，負責其他模塊之間的數據交互，承擔通信樞鈕功能。也是資源配額控制的入口。提供完備的集群安全機制

集群狀態存儲

> 基於raft協議開發的

控制器（kube-controller-manager）

> * 由一系列控制器組成，通過apiserver監控整個集群的狀態，並確保集群處於預期的工作狀態
> * Node controller
> * Deployment controller
> * service controller
> * volume controller
> * endpoint controller
> * garbage controller
> * namespace controller
> * job controller
> * resource quta controller

調度器(kube-scheduler)

> 分配資源到適合的運算節點上
>
> 預算策略
>
> 優選策略

Node組件(運算節點)

> 提供運行容器的各種依賴環境 並接收master的管理

Node內又有許多組件

* `kuberlet `: 定時向 API server註冊節點並匯報容器運行時的環境 : 提供鏡像並運行，負責鏡象和容器的清理工作，保證節點上鏡象不會占滿磁碟空間，退出的容器不會占用太多資源
* `kube-proxy `: 按需為service生成iptables規則 並轉發至正確的目標。負責建立和刪除包括更新調度規則、通知apiserver自己的更新，或者從apiserver中獲取其他kube-proxy的調度規則變化來更新自己的。



### 核心附件

* CNI網路插件 flannel/calico
* 服務發現用插件 coredns
* 服務暴露用插件 traefik
* GUI管理插件 dashboard
* Frederation：提供一個可以跨集群中心多k8s統一管理功能
* Prometheus：提供k8s集群的監控能力
* ELK：提供k8s日誌統一分析介入平台

#### HPA（HorizontalPodAutoScale）

監控POD資源利用率來擴容縮容

## 設置免密碼登入

```
ssh-copy-id -i ~/.ssh/id_rsa.pub "root@k8s-master1"
ssh-copy-id -i ~/.ssh/id_rsa.pub "root@k8s-node1"
ssh-copy-id -i ~/.ssh/id_rsa.pub "root@k8s-node2"
```



## 安裝簽發證書服務

HDSS7-200

```
wget https://pkg.cfssl.org/R1.2/cfssl_linux-amd64 -O /usr/bin/cfssl

wget https://pkg.cfssl.org/R1.2/cfssljson_linux-amd64 -O /usr/bin/cfssl-json

wget https://pkg.cfssl.org/R1.2/cfssl-certinfo_linux-amd64 -O /usr/bin/cfssl-certinfo

chmod +x /usr/bin/cfssl*
```

創建ca請求文件

```
mkdir /opt/certs -p
cd /opt/certs/
cat>>/opt/certs/ca-csr.json<<EOF
{
    "CN": "example.com",
	"hosts": [],
	"key": {
		"algo": "rsa",
		"size": 2048
	},
	"names": [
		{
			"C": "TW",
			"ST": "Taipei",
			"L": "Taipei",
			"O": "od",
			"OU": "ops"
		}
	],
	"ca": {
		"expiry": "175200h"
	}
}
EOF

```

生成證書

```
cfssl gencert -initca /opt/certs/ca-csr.json | cfssl-json -bare ca
```

查看證書

```
ls 
ca-csr.json  ca-key.pem  ca.csr  ca.pem

cat ca-key.pem
```

### 部署docker環境

21 22 200

```
curl -fsSL https://get.docker.com |bash -s docker

mkdir /etc/docker
vi /etc/docker/daemon.json
{
	"graph": "/data/docker",
	"storage-driver": "overlay2",
	"insecure-registries": ["registry.access.redhat.com","quay.io","harbor.od.com"],
	"registry-mirrors": ["https://q2gr04ke.mirror.aliyuncs.com"],
	"bip": "172.7.21.1/24",
	"exec-opts": ["native.cgroupdriver=systemd"],
	"live-restore": true
}
```

### 部署docker私有鏡像倉庫harbor

https://github.com/goharbor/harbor/releases

```
mkdir -p /opt/src && cd /opt/src
wget https://github.com/goharbor/harbor/releases/download/v2.1.0/harbor-offline-installer-v2.1.0.tgz

tar xvf harbor-offline-installer-v2.1.0.tgz -C /opt
ln -s /opt/harbor-v2.1.0 /opt/harbor
yum install docker-compose -y
```

```
#配置harbor.yml
vi /opt/harbor/harbor.yml
hostname: harbor.od.com
http:
	port: 180
	
https:
  # https port for harbor, default is 443
  port: 443
  # The path of cert and key files for nginx
  certificate: /opt/certs/ca.pem
  private_key: /opt/certs/ca-key.pem

location: /data/harbor/logs
data_volume: /data/harbor
```

執行./installer.sh

**查看**

`docker-compose ps`

### 安裝nginx

```
yum install nginx -y
```

vi /etc/nginx/conf.d/harbor.od.com.conf

```
server {
	listen    80;
	server_name hub.atguigu.com;
	client_max_body_size  1000m;
	location / {
		proxy_pass http://192.168.56.200:80;
	}
}
```

新建項目 

* public
* 公開

使用docker登入私有倉庫

```
docker images | grep 1.7.9
nginx                         1.7.9               84581e99d807        5 years ago         91.7MB
 docker tag 84581e99d807 hub.atguigu.com/public/nginx:v1.7.9
 docker login hub.atguigu.com
 docker push hub.atguigu.com/public/nginx:v1.7.9
```

## K8S 手動安裝

### 環境規畫

單master架構

```
master
	k8s-master  192.168.56.11
worker
	k8s-node1   192.168.56.21
	k8s-node2   192.168.56.22
```

高可用架構

```
master
    k8s-master1
    k8s-master2
worker
	k8s-node1
	k8s-node2
lvm
	l
```

k8s版本 1.16

安裝方式 離線，二進制

操作系統7.7

### 部署docker環境

```
curl -fsSL https://get.docker.com |bash -s docker

mkdir /etc/docker
vi /etc/docker/daemon.json
{
	"graph": "/data/docker",
	"storage-driver": "overlay2",
	"insecure-registries": ["registry.access.redhat.com","quay.io","harbor.od.com"],
	"registry-mirrors": ["https://q2gr04ke.mirror.aliyuncs.com"],
	"bip": "172.7.21.1/24",
	"exec-opts": ["native.cgroupdriver=systemd"],
	"live-restore": true
}
```

### 架構圖

![image-20201031103526251](https://i.imgur.com/DIued7o.jpg)

### 創建安裝目錄

```
mkdir /opt/etcd/{bin,cfg,certs} -p 
mkdir /opt/kubernetes/{bin,cfg,certs} -p
```

### 部署etcd 

填寫表單，寫明etcd所在的IP

向證書頒發機構申請證書

使用master1 node1 node2 部署etcd，生產環境需使用獨立的etcd設備。

創建證書頒發機構HDSS7-200

```
cd /opt/certs
cat>>/opt/certs/ca-config.json<<EOF
{
	"signing": {
		"expiry": "175200h"
	},
	"profiles": {
		"server": {
			"expiry": "175200h",
			"usages": [
				"signing",
				"key encipherment",
				"server auth"
			]
		},
        "client": {
            "expiry": "175200h",
            "usages": [
                "signing",
                "key encipherment",
                "client auth"
            ]
        },
        "peer": {
            "expiry": "175200h",
            "usages": [
                "signing",
                "key encipherment",
                "client auth"
            ]
        }
	}
}
EOF

```

證書申請文件

```
vi /opt/certs/etcd-peer-csr.json
{
	"CN": "etcd",
	"hosts": [
		"192.168.56.31",
		"192.168.56.32",
		"192.168.56.33"
	],
	"key": {
		"algo": "rsa",
		"size": 2048
	},
	"names": [
		{
			"C": "TW",
			"ST": "TW",
			"L": "Taipei",
			"O": "od",
			"OU": "ops"
		}
	]
}
```

生成證書（HDSS7-200）

```
mkdir /opt/certs/etcd
cd /opt/certs/etcd

cfssl gencert -ca=ca.pem -ca-key=ca-key.pem -config=ca-config.json -profile=peer etcd-peer-csr.json | cfssl-json -bare etcd-peer 
```

12 21 22創建需要的資料夾及用戶

```
mkdir /opt/src -p && /opt/src
mkdir -p /data/logs/etcd-server
mkdir -p /data/etcd 

#創建etcd用戶
useradd -s /sbin/nologin -M etcd
```

下載etcd安裝檔

```
#12 21 22下載etcd.tar.gz
tar xvf etcd -C /opt
ln -s etcd-vXXX etcd 

mkdir /opt/etcd/certs
cd /opt/etcd/certs
scp root@hp56-11:/opt/certs/ca.pem . 
scp root@hp56-11:/opt/certs/etcd-peer.pem . 
scp root@hp56-11:/opt/certs/etcd-peer-key.pem . 
```

授權

```
chown -R etcd.etcd /data/logs/etcd-server
chown -R etcd.etcd /data/etcd
chown -R etcd.etcd /opt/etcd/certs/*.pem
```

建立etcd設定檔

```
cat>>/opt/etcd/cfg/etcd<<EOF
#[Member]
ETCD_NAME="etcd-server1
ETCD_DATA_DIR="/data/etcd/default.etcd"
ETCD_LISTEN_PEER_URLS="https://192.168.56.31:2380"
ETCD_LISTEN_CLIENT_URLS="https://192.168.56.31:2379"

#[Clustering]
ETCD_INITIAL_ADVERTISE_PEER_URLS="https://192.168.56.31:2380"
ETCD_ADVERTISE_CLIENT_URLS="https://192.168.56.31:2379"
ETCD_INITIAL_CLUSTER="etcd1=https://192.168.56.31:2380,etcd2=https://192.168.56.32:2380,etcd3=https://192.168.56.33:2380"
ETCD_INITIAL_CLUSTER_TOKEN="etcd-cluster"
ETCD_INITIAL_CLUSTER_STATE="new"

#[Certs]
ETCD_CA="/opt/etcd/certs/ca.pem"
ETCD_KEY="/opt/etcd/certs/etcd-peer-key.pem"
ETCD_CERT="/opt/etcd/certs/etcd-peer.pem"
EOF
```

創建etcd的etcd.service文件

```
vi /usr/lib/systemd/system/etcd.service
[Unit]
Description=Etcd Server
After=network.target
After=network-online.target
Wants=network-online.target

[Service]
User=etcd
Group=etcd
Type=notify
EnvironmentFile=/opt/etcd/cfg/etcd
ExecStart=/opt/etcd/bin/etcd \
--name=${ETCD_NAME} \
--data-dir=${ETCD_DATA_DIR} \
--listen-peer-urls=${ETCD_LISTEN_PEER_URLS} \
--listen-client-urls=${ETCD_LISTEN_CLIENT_URLS},http://127.0.0.1:2379 \
--advertise-client-urls=${ETCD_ADVERTISE_CLIENT_URLS} \
--initial-advertise-peer-urls=${ETCD_INITIAL_ADVERTISE_PEER_URLS} \
--initial-cluster=${ETCD_INITIAL_CLUSTER} \
--initial-cluster-token=${ETCD_INITIAL_CLUSTER_TOKEN} \
--initial-cluster-state=new \
--cert-file=${ETCD_CERT} \
--key-file=${ETCD_KEY} \
--peer-cert-file=${ETCD_CERT} \
--peer-key-file=/${ETCD_KEY} \
--trusted-ca-file=${ETCD_CA} \
--peer-trusted-ca-file=${ETCD_CA} \
--log-output stdout \
--peer-client-cert-auth \
--quota-backend-bytes 8000000000
Restart=on-failure
LimitNOFILE=65536

[Install]
WantedBy=multi-user.target

```

測試etcd集群

```
etcdctl -ca-file=/opt/etcd/certs/ca.pem --cert-file=/opt/etcd/certs/etcd-peer.pem --key-file=/opt/etcd/certs/etcd-peer-key.pem --endpoints="https://192.168.56.11:2379,https://192.168.56.21:2379,https://192.168.56.22:2379" cluster-health

2020-11-03 08:52:59.244932 I | warning: ignoring ServerName for user-provided CA for backwards compatibility is deprecated
2020-11-03 08:52:59.245321 I | warning: ignoring ServerName for user-provided CA for backwards compatibility is deprecated
member a654cb086123e8d9 is healthy: got healthy result from https://192.168.56.21:2379
member bf624c9e82dced96 is healthy: got healthy result from https://192.168.56.11:2379
member c0df55494e9634b3 is healthy: got healthy result from https://192.168.56.22:2379
cluster is healthy
```

(改用systemctl)安裝supervisor進程後台運行(12 21 22)

```
yum install -y supervisor  # 可以將進程放在後台運行
systemctl start supervisord
systemctl enable supervisord
```

```
vi  /opt/etcd/etcd-server-start.sh
#!/bin/sh 
./etcd --name etcd-server-7-11 \
	   --data-dir /data/etcd/etcd-server \
	   --listen-peer-urls https://192.168.56.11:2380 \
	   --listen-client-urls https://192.168.56.11:2379,http://127.0.0.1:2379 \
	   --quota-backend-bytes 8000000000 \
	   --initial-advertise-peer-urls https://192.168.56.11:2380 \
	   --advertise-client-urls https://192.168.56.11:2379,http://127.0.0.1:2379 \
	   --initial-cluster etcd-server-7-11=https://192.168.56.11:2380,etcd-server-7-21=https://192.168.56.21:2380,etcd-server-7-22=https://192.168.56.22:2380 \
	   --ca-file /opt/etcd/certs/ca.pem \
	   --cert-file /opt/etcd/certs/etcd-peer.pem \
	   --key-file /opt/etcd/certs/etcd-peer-key.pem \
	   --client-cert-auth \
	   --trusted-ca-file /opt/etcd/certs/ca.pem \
	   --peer-ca-file /opt/etcd/certs/ca.pem \
	   --peer-cert-file /opt/etcd/certs/etcd-peer.pem \
	   --peer-key-file /opt/etcd/certs/etcd-peer-key.pem \
	   --peer-client-cert-auth \
	   --peer-trusted-ca-file /opt/etcd/certs/ca.pem \
	   --log-output stdout
```

supervisor配置文件(12 21 22)

```
vi /etc/supervisord.d/etcd-server.ini
[program:etcd-server-7-22]
command=/opt/etcd/etcd-server-start.sh 
numprocs=1
directory=/opt/etcd
autostart=true
autorestart=true
startsecs=30
startretries=3
exitcodes=0,2
stopsignal=QUIT
stopwaitsecs=10
user=etcd
redirect_stderr=true
stdout_logfile=/data/logs/etcd-server/etcd.stdout.log
stdout_logfile_maxbytes=64MB
stdout_logfile_backups=4
stdout_capture_maxbytes=1MB
stdout_events_enabled=false
```

啟動supervisor

```
supervisorctl update
supervisorctl status
etcd-server-7-12                 STARTING
etcd-server-7-12                 RUNNING   pid 14450, uptime 0:03:57

netstat -lntup | grep etcd 
tcp        0      0 192.168.56.12:2379      0.0.0.0:*               LISTEN      14451/./etcd        
tcp        0      0 127.0.0.1:2379          0.0.0.0:*               LISTEN      14451/./etcd        
tcp        0      0 192.168.56.12:2380      0.0.0.0:*
```

### 部署flanneld網路

安裝docker

```
yum install -y yum-utils device-mapper-persistent-data lvm2 && yum-config-manager --add-repo https://download.docker.com/linux/centos/docker-ce.repo && yum list docker-ce --showduplicates | soft -r &&  yum install docker-ce -y 

systemctl start docker && systemctl enable docker

```

向master寫入集群Pod網段信息(etcd主節點上操作)

```
cd /opt/etcd/certs/
/opt/etcd/bin/etcdctl \
--ca-file=/opt/etcd/certs/ca.pem --cert-file=/opt/etcd/certs/etcd-peer.pem \
--key-file=/opt/etcd/certs/etcd-peer-key.pem \
--endpoints="https://192.168.56.11:2379,https://192.168.56.21:2379,https://192.168.56.22:2379" \
set /coreos.com/network/config  '{ "Network": "172.18.0.0/16", "Backend": {"Type": "vxlan"}}'
```

下載flanneld二進制檔

```
wget https://github.com/coreos/flannel/releases/download/v0.11.0/flannel-v0.11.0-linux-amd64.tar.gz

# https://github.com/coreos/flannel/releases/tag/v0.11.0
```

解壓安裝

```
tar xvf flannel
mv flanneld mk-docker-opts.sh /opt/kubernetes/bin/
```

配置flannel

```
cat>>/opt/kubernetes/cfg/flanneld<< EOF
FLANNEL_OPTIONS="--etcd-endpoints=https://192.168.56.11:2379,https://192.168.56.21:2379,https://192.168.56.22:2379 -etcd-cafile=/opt/etcd/certs/ca.pem -etcd-certfile=/opt/etcd/certs/etcd-peer.pem -etcd-keyfile=/opt/etcd/certs/etcd-peer-key.pem"
EOF
```

創建flanneld的flanneld.service文件，配置所有node節點

```
vi /usr/lib/systemd/system/flanneld.service
[Unit]
Description=Flanneld overlay address etcd agent
After=network-online.target network.target
Before=docker.service
 

[Service]
Type=notify
EnvironmentFile=/opt/kubernetes/cfg/flanneld
ExecStart=/opt/kubernetes/bin/flanneld --ip-masq $FLANNEL_OPTIONS
ExecStartPost=/opt/kubernetes/bin/mk-docker-opts.sh -k DOCKER_NETWORK_OPTIONS -d /run/flannel/subnet.env
Restart=on-failure

[Install]
WantedBy=multi-user.target
```

配置Docker啟動指定子網段，所有node節點

```
vi /usr/lib/systemd/system/docker.service 
[Unit]
Description=Docker Application Container Engine
Documentation=https://docs.docker.com
After=network-online.target firewalld.service
Wants=network-online.target

[Service]
Type=notify
EnvironmentFile=/run/flannel/subnet.env
ExecStart=/usr/bin/dockerd $DOCKER_NETWORK_OPTIONS
ExecReload=/bin/kill -s HUP $MAINPID
LimitNOFILE=infinity
LimitNPROC=infinity
LimitCORE=infinity
TimeoutStartSec=0
Delegate=yes
KillMode=process
Restart=on-failure
StartLimitBurst=3
StartLimitInterval=60s

[Install]
WantedBy=multi-user.target
```



### 部署apiserver

下載kubernetes至/opt/src，並解壓至/opt/下

```

```

首先，簽發client證書

HDSS7-200

```
vi /opt/certs/client-csr.json
{
	"CN": "k8s-node",
	"hosts": [
	],
	"key": {
		"algo": "rsa",
		"size": 2048
	},
	"names": [
		{
			"C": "TW",
			"ST": "TW",
			"L": "Taipei",
			"O": "od",
			"OU": "ops"
		}
	]
}
```

生成證書（HDSS7-200）

```
cfssl gencert -ca=ca.pem -ca-key=ca-key.pem -config=ca-config.json -profile=client client-csr.json | cfssl-json -bare client
```

給apiserver簽發證書(HDSS7-200)

```
cat<<EOF| tee /opt/certs/apiserver-csr.json
{
	"CN": "k8s-apiserver",
	"hosts": [
		"127.0.0.1",
		"192.168.0.1",
		"kubernetes.default",
		"kubernetes.default.svc",
		"kubernetes.default.svc.cluster",
		"kubernetes.default.svc.cluster.local",
		"192.168.56.10",
		"192.168.56.11",
		"192.168.56.12",
		"192.168.56.21",
		"192.168.56.22",
		"192.168.56.23"
	],
	"key": {
		"algo": "rsa",
		"size": 2048
	},
	"names": [
		{
			"C": "TW",
			"ST": "TW",
			"L": "Taipei",
			"O": "od",
			"OU": "ops"
		}
	]
}
EOF
```

```
cfssl gencert -ca=ca.pem -ca-key=ca-key.pem -config=ca-config.json -profile=server apiserver-csr.json | cfssl-json -bare apiserver
```

創建kubernetes Proxy證書

```
cat << EOF | tee kube-proxy-csr.json
{
  "CN": "system:kube-proxy",
  "hosts": [],
  "key": {
    "algo": "rsa",
    "size": 2048
  },
  "names": [
    {
      "C": "TW",
      "ST": "TW",
      "O": "od",
      "OU": "ops"
    }
  ]
}
EOF

cfssl gencert -ca=/opt/certs/ca.pem -ca-key=/opt/certs/ca-key.pem -config=/opt/certs/ca-config.json -profile=server kube-proxy-csr.json | cfssl-json -bare kube-proxy

```

```
tar -xvf kubernetes-server-linux-amd64.tar.gz 
cd kubernetes/server/bin/
cp kube-scheduler kube-apiserver kube-controller-manager kubectl /opt/kubernetes/bin/

cp ca.pem ca-key.pem apiserver.pem apiserver-key.pem /opt/kubernetes/certs/

```

### 部署kube-apiserver組件

創建TLS Bootstrapping token

```
# 生成随机字符串
head -c 16 /dev/urandom | od -An -t x | tr -d ' '
2366a641f656a0a025abb4aabda4511b
```

```
vi /opt/kubernetes/cfg/token.csv
642b4351a397e55df44971b136751044,kubelet-bootstrap,10001,"system:kubelet"
```

創建apiserver配置文件

```
vi /opt/kubernetes/cfg/kube-apiserver 
KUBE_APISERVER_OPTS="--logtostderr=true \
--v=4 \
--etcd-servers=https://192.168.56.11:2379,https://192.168.56.21:2379,https://192.168.56.22:2379 \
--bind-address=192.168.56.11 \
--secure-port=6443 \
--advertise-address=192.168.56.11 \
--allow-privileged=true \
--service-cluster-ip-range=10.0.0.0/24 \
--enable-admission-plugins=NamespaceLifecycle,LimitRanger,SecurityContextDeny,ServiceAccount,ResourceQuota,NodeRestriction \
--authorization-mode=RBAC,Node \
--enable-bootstrap-token-auth \
--token-auth-file=/opt/kubernetes/cfg/token.csv \
--service-node-port-range=30000-50000 \
--tls-cert-file=/opt/kubernetes/certs/apiserver.pem  \
--tls-private-key-file=/opt/kubernetes/certs/apiserver-key.pem \
--client-ca-file=/opt/kubernetes/certs/ca.pem \
--service-account-key-file=/opt/kubernetes/certs/ca-key.pem \
--etcd-cafile=/opt/etcd/certs/ca.pem \
--etcd-certfile=/opt/etcd/certs/etcd-peer.pem \
--etcd-keyfile=/opt/etcd/certs/etcd-peer-key.pem"

```

**参数说明(\* 号代表通配符说明参数相同的有多个)：**

- `--advertise-address`：apiserver 对外通告的 IP（kubernetes 服务后端节点 IP）；
- `--default-*-toleration-seconds`：设置节点异常相关的阈值；
- `--max-*-requests-inflight`：请求相关的最大阈值；
- `--etcd-*`：访问 etcd 的证书和 etcd 服务器地址；
- `--bind-address`： https 监听的 IP，不能为 `127.0.0.1`，否则外界不能访问它的安全端口 6443；
- `--secret-port`：https 监听端口；
- `--insecure-port=0`：关闭监听 http 非安全端口(8080)；
- `--tls-*-file`：指定 apiserver 使用的证书、私钥和 CA 文件；
- `--audit-*`：配置审计策略和审计日志文件相关的参数；
- `--client-ca-file`：验证 client (kue-controller-manager、kube-scheduler、kubelet、kube-proxy 等)请求所带的证书；
- `--enable-bootstrap-token-auth`：启用 kubelet bootstrap 的 token 认证；
- `--requestheader-*`：kube-apiserver 的 aggregator layer 相关的配置参数，proxy-client & HPA 需要使用；
- `--requestheader-client-ca-file`：用于签名 `--proxy-client-cert-file` 和 `--proxy-client-key-file` 指定的证书；在启用了 metric aggregator 时使用；
- `--requestheader-allowed-names`：不能为空，值为逗号分割的 `--proxy-client-cert-file` 证书的 CN 名称，这里设置为 "aggregator"；
- `--service-account-key-file`：签名 ServiceAccount Token 的公钥文件，kube-controller-manager 的 `--service-account-private-key-file` 指定私钥文件，两者配对使用；
- `--runtime-config=api/all=true`： 启用所有版本的 APIs，如 autoscaling/v2alpha1；
- `--authorization-mode=Node,RBAC`、`--anonymous-auth=false`： 开启 Node 和 RBAC 授权模式，拒绝未授权的请求；
- `--enable-admission-plugins`：启用一些默认关闭的 plugins；
- `--allow-privileged`：运行执行 privileged 权限的容器；
- `--apiserver-count=3`：指定 apiserver 实例的数量；
- `--event-ttl`：指定 events 的保存时间；
- `--kubelet-*`：如果指定，则使用 https 访问 kubelet APIs；需要为证书对应的用户(上面 kubernetes*.pem 证书的用户为 kubernetes) 用户定义 RBAC 规则，否则访问 kubelet API 时提示未授权；
- `--proxy-client-*`：apiserver 访问 metrics-server 使用的证书；
- `--service-cluster-ip-range`： 指定 Service Cluster IP 地址段；
- `--service-node-port-range`： 指定 NodePort 的端口范围；

創建kube-apiserver的kube-apiserver.service文件

```
vi /usr/lib/systemd/system/kube-apiserver.service 
[Unit]
Description=Kubernetes API Server
Documentation=https://github.com/kubernetes/kubernetes

[Service]
EnvironmentFile=/opt/kubernetes/cfg/kube-apiserver
ExecStart=/opt/kubernetes/bin/kube-apiserver $KUBE_APISERVER_OPTS
Restart=on-failure

[Install]
WantedBy=multi-user.target
```



下載證書至HDSS7-21 /opt/certs並授權

`chown -R etcd.etcd /opt/k8s/server/bin/certs/`

配置/opt/kubernetes/server/bin/conf/audit.yaml

```
apiVersion: audit.k8s.io/v1beta1 # This is required.
kind: Policy
# Don't generate audit events for all requests in RequestReceived stage.
omitStages:
  - "RequestReceived"
rules:
  # Log pod changes at RequestResponse level
  - level: RequestResponse
    resources:
    - group: ""
      # Resource "pods" doesn't match requests to any subresource of pods,
      # which is consistent with the RBAC policy.
      resources: ["pods"]
  # Log "pods/log", "pods/status" at Metadata level
  - level: Metadata
    resources:
    - group: ""
      resources: ["pods/log", "pods/status"]

  # Don't log requests to a configmap called "controller-leader"
  - level: None
    resources:
    - group: ""
      resources: ["configmaps"]
      resourceNames: ["controller-leader"]

  # Don't log watch requests by the "system:kube-proxy" on endpoints or services
  - level: None
    users: ["system:kube-proxy"]
    verbs: ["watch"]
    resources:
    - group: "" # core API group
      resources: ["endpoints", "services"]

  # Don't log authenticated requests to certain non-resource URL paths.
  - level: None
    userGroups: ["system:authenticated"]
    nonResourceURLs:
    - "/api*" # Wildcard matching.
    - "/version"

  # Log the request body of configmap changes in kube-system.
  - level: Request
    resources:
    - group: "" # core API group
      resources: ["configmaps"]
    # This rule only applies to resources in the "kube-system" namespace.
    # The empty string "" can be used to select non-namespaced resources.
    namespaces: ["kube-system"]

  # Log configmap and secret changes in all other namespaces at the Metadata level.
  - level: Metadata
    resources:
    - group: "" # core API group
      resources: ["secrets", "configmaps"]

  # Log all other resources in core and extensions at the Request level.
  - level: Request
    resources:
    - group: "" # core API group
    - group: "extensions" # Version of group should NOT be included.

  # A catch-all rule to log all other requests at the Metadata level.
  - level: Metadata
    # Long-running requests like watches that fall under this rule will not
    # generate an audit event in RequestReceived.
    omitStages:
      - "RequestReceived"
```

查看apiserver

```
netstat -lntup | grep kube-api
tcp        0      0 127.0.0.1:8080          0.0.0.0:*               LISTEN      17481/./kube-apiser 
tcp6       0      0 :::6443                 :::*                    LISTEN      17481/./kube-apiser 
```

#### 配置反向代理

在11,12上，虛擬IP為10

```
yum install -y nginx

vi /etc/nginx/nginx.conf
stream {
	upstream kube-apiserver {
		server 192.168.56.21:6443  max_fails=3 fail_timeout=20s;
		server 192.168.56.22:6443  max_fails=3 fail_timeout=20s;
	}
	server {
		listen 7443;
		proxy_connect_timeout 2s;
		proxy_timeout 900s;
		proxy_pass kube-apiserver;
	}
}
```

```
systemctl enable nginx 
systemctl start nginx
```



安裝keepalive

`yum install keepalived -y`

```
vi /etc/keepalived/check_port.sh
#!/bin/bash
CHK_PORT=$1
if [ -n "$CHK_PORT" ];then
	PORT_PROCESS=`ss -lnt|grep $CHK_PORT|wc -l`
	if [ $PORT_PROCESS -eq 0 ];then
		echo "Port $CHK_PORT Is Not Used, End."
		exit 1
	fi
else
	echo "Check Port Can't Be Empty！"
fi
```

```
chmod +x /etc/keepalived/check_port.sh
```

keepalived 主

```
vi /etc/keepalive/keepalive.conf
! Configuration File for keepalived

global_defs {
   router_id 192.168.56.12
}
vrrp_script chk_nginx {
  script "/etc/keepalived/check_port.sh 7443"  
  interval 2                         
  weight -5                          
  fall 3
}
vrrp_instance VI_1 {
    state MASTER
    interface enp0s8
    mcast_src_ip 192.168.56.12
    virtual_router_id 51
    priority 100
    advert_int 1
    nopreempt
    authentication {
        auth_type PASS
        auth_pass 123456
    }
    virtual_ipaddress {
        192.168.56.10
    }
    track_script {
      chk_nginx
    }
}
```

keepalived從

```
! Configuration File for keepalived
global_defs {
   router_id 192.168.56.12 #兩台伺服器必須設置不一樣id
}

vrrp_script chk_nginx {
  script "/etc/keepalived/check_port.sh 7443"  #偵測執行的shell路徑
  interval 2                         #檢查間隔
  weight -5                          #檢查失敗時，所要增減的priority,exit0為成功，exit1為失敗
  fall 3
}

vrrp_instance VI_1 {
    state BACKUP
    interface enp0s8
    mcast_src_ip 192.168.56.12
    virtual_router_id 51
    priority 99
    advert_int 1
    authentication {
        auth_type PASS
        auth_pass 123456
    }
    virtual_ipaddress {
        192.168.56.10
    }
    track_script {
      chk_nginx
    }
}
```

```
systemctl start keepalived
systemctl enable keepalived
```

### 部署controller-manager

創建kube-controller-manager配置文件

```
vi /opt/kubernetes/cfg/kube-controller-manager
KUBE_CONTROLLER_MANAGER_OPTS="--logtostderr=true \
--v=4 \
--master=127.0.0.1:8080 \
--leader-elect=true \
--address=127.0.0.1 \
--service-cluster-ip-range=10.0.0.0/24 \
--cluster-name=kubernetes \
--cluster-signing-cert-file=/opt/kubernetes/certs/apiserver.pem \
--cluster-signing-key-file=/opt/kubernetes/certs/apiserver-key.pem  \
--root-ca-file=/opt/kubernetes/certs/ca.pem \
--service-account-private-key-file=/opt/kubernetes/certs/ca-key.pem"

# 证书配置这块使用的是apiserver的证书进行连接集群
```

**配置参数详解：**

- `--port=0`：关闭监听非安全端口（http），同时 `--address` 参数无效，`--bind-address` 参数有效；
- `--secure-port=10252`、`--bind-address=0.0.0.0`: 在所有网络接口监听 10252 端口的 https /metrics 请求；
- `--kubeconfig`：指定 kubeconfig 文件路径，kube-controller-manager 使用它连接和验证 kube-apiserver；
- `--authentication-kubeconfig` 和 `--authorization-kubeconfig`：kube-controller-manager 使用它连接 apiserver，对 client 的请求进行认证和授权。`kube-controller-manager` 不再使用 `--tls-ca-file` 对请求 https metrics 的 Client 证书进行校验。如果没有配置这两个 kubeconfig 参数，则 client 连接 kube-controller-manager https 端口的请求会被拒绝(提示权限不足)。
- `--cluster-signing-*-file`：签名 TLS Bootstrap 创建的证书；
- `--experimental-cluster-signing-duration`：指定 TLS Bootstrap 证书的有效期；
- `--root-ca-file`：放置到容器 ServiceAccount 中的 CA 证书，用来对 kube-apiserver 的证书进行校验；
- `--service-account-private-key-file`：签名 ServiceAccount 中 Token 的私钥文件，必须和 kube-apiserver 的 `--service-account-key-file` 指定的公钥文件配对使用；
- `--service-cluster-ip-range` ：指定 Service Cluster IP 网段，必须和 kube-apiserver 中的同名参数一致；
- `--leader-elect=true`：集群运行模式，启用选举功能；被选为 leader 的节点负责处理工作，其它节点为阻塞状态；
- `--controllers=*,bootstrapsigner,tokencleaner`：启用的控制器列表，tokencleaner 用于自动清理过期的 Bootstrap token；
- `--horizontal-pod-autoscaler-*`：custom metrics 相关参数，支持 autoscaling/v2alpha1；
- `--tls-cert-file`、`--tls-private-key-file`：使用 https 输出 metrics 时使用的 Server 证书和秘钥；
- `--use-service-account-credentials=true`: kube-controller-manager 中各 controller 使用 serviceaccount 访问 kube-apiserver；

創建kube-controller-manager的kube-controller-manager.service

```
vi /usr/lib/systemd/system/kube-controller-manager.service
[Unit]
Description=Kubernetes Controller Manager
Documentation=https://github.com/kubernetes/kubernetes

[Service]
EnvironmentFile=-/opt/kubernetes/cfg/kube-controller-manager
ExecStart=/opt/kubernetes/bin/kube-controller-manager $KUBE_CONTROLLER_MANAGER_OPTS
Restart=on-failure

[Install]
WantedBy=multi-user.target
```

```
systemctl daemon-reload
systemctl enable kube-controller-manager
systemctl restart kube-controller-manager
```

### 部署kube-scheduler

創建kube-scheduler配置文件

```
vi /opt/kubernetes/cfg/kube-scheduler
KUBE_SCHEDULER_OPTS="--logtostderr=true --v=4 --master=127.0.0.1:8080 --leader-elect=true"
```

**参数说明：**

```
--address：在 127.0.0.1:10251 端口接收 http /metrics 请求；kube-scheduler 目前还不支持接收 https 请求；
--master 连接本地apiserver
--kubeconfig：指定 kubeconfig 文件路径，kube-scheduler 使用它连接和验证 kube-apiserver；
--leader-elect=true：集群运行模式，启用选举功能；被选为 leader 的节点负责处理工作，其它节点为阻塞状态；当该组件启动多个时，自动选举（HA）
```

創建kube-scheduler的kube-scheduler.service文件

```
vi /usr/lib/systemd/system/kube-scheduler.service 
[Unit]
Description=Kubernetes Scheduler
Documentation=https://github.com/kubernetes/kubernetes

[Service]
EnvironmentFile=-/opt/kubernetes/cfg/kube-scheduler
ExecStart=/opt/kubernetes/bin/kube-scheduler $KUBE_SCHEDULER_OPTS
Restart=on-failure

[Install]
WantedBy=multi-user.target
```

### 將可執行文件路徑添加到PATH變量中

```
vi /etc/profile
PATH=/opt/kubernetes/bin:$PATH:$HOME/bin
source /etc/profile
```

查看kube cluster

`kubectl get cs `

```
kubectl get cs
Warning: v1 ComponentStatus is deprecated in v1.19+
NAME                                 STATUS    MESSAGE              ERROR
componentstatus/scheduler            Healthy   ok                   
componentstatus/controller-manager   Healthy   ok                   
componentstatus/etcd-2               Healthy   {"health": "true"}   
componentstatus/etcd-1               Healthy   {"health": "true"}   
componentstatus/etcd-0               Healthy   {"health": "true"}
```



### 部署kubelet，在node節點上

將kubelete及kube-proxy二進制文件複制到node上

```
scp kubelet root@k8s-node1:/opt/kubernetes/bin
scp kube-proxy root@k8s-node1:/opt/kubernetes/bin
```

創建kubelet bootstrap kubeconfig文件

```
cd /opt/kubernetes/certs/

vi environment.sh
BOOTSTRAP_TOKEN=642b4351a397e55df44971b136751044

KUBE_APISERVER="https://192.168.56.11:6443"
# kuber-apiserver启动参数中的token.csv和kubelet启动参数中指定的bootstrap文件bootstrap.kubeconfig中的token值是否一致，此外该token必须为实际数值，不能使用变量代替

# 设置集群参数
kubectl config set-cluster kubernetes \
  --certificate-authority=./ca.pem \
  --embed-certs=true \
  --server=${KUBE_APISERVER} \
  --kubeconfig=bootstrap.kubeconfig

# 设置客户端认证参数

kubectl config set-credentials kubelet-bootstrap \
  --token=${BOOTSTRAP_TOKEN} \
  --kubeconfig=bootstrap.kubeconfig

# 设置上下文参数

kubectl config set-context default \
  --cluster=kubernetes \
  --user=kubelet-bootstrap \
  --kubeconfig=bootstrap.kubeconfig

# 设置默认上下文

kubectl config use-context default --kubeconfig=bootstrap.kubeconfig

#----------------------
# 创建kube-proxy kubeconfig文件
kubectl config set-cluster kubernetes \
  --certificate-authority=./ca.pem \
  --embed-certs=true \
  --server=${KUBE_APISERVER} \
  --kubeconfig=kube-proxy.kubeconfig

kubectl config set-credentials kube-proxy \
  --client-certificate=./kube-proxy.pem \
  --client-key=./kube-proxy-key.pem \
  --embed-certs=true \
  --kubeconfig=kube-proxy.kubeconfig

kubectl config set-context default \
  --cluster=kubernetes \
  --user=kube-proxy \
  --kubeconfig=kube-proxy.kubeconfig

kubectl config use-context default --kubeconfig=kube-proxy.kubeconfig

```

在master中運行environment.sh

```
cd /opt/kubernetes/certs
bash environment.sh
```

將bootstrap.kubeconfig kube-proxy.kubeconfig 文件拷貝到所有node節點

```
scp bootstrap.kubeconfig kube-proxy.kubeconfig root@k8s-node1:/opt/kubernetes/cfg

scp bootstrap.kubeconfig kube-proxy.kubeconfig root@k8s-node2:/opt/kubernetes/cfg

cp bootstrap.kubeconfig kube-proxy.kubeconfig /opt/kubernetes/cfg/
history

```

#### node節點配置kubelet

創建kubelet參配置模板文件，需注意每個node節點的IP要修改

```
vi /opt/kubernetes/cfg/kubelet.config
kind: KubeletConfiguration
apiVersion: kubelet.config.k8s.io/v1beta1
address: 192.168.56.22
port: 10250
readOnlyPort: 10255
cgroupDriver: cgroupfs
clusterDNS: 
- 10.0.0.2
clusterDomain: cluster.local
failSwapOn: false
authentication:
  anonymous:
    enabled: true
  webhook:
    cacheTTL: 2m0s
    enabled: true
  x509:
    clientCAFile: /opt/kubernetes/certs/ca.pem
authorization:
  mode: Webhook
  webhook:
    cacheAuthorizedTTL: 5m0s
    cacheUnauthorizedTTL: 30s
evictionHard:
  imagefs.available: 15%
  memory.available: 100Mi
  nodefs.available: 10%
  nodefs.inodesFree: 5%
maxOpenFiles: 1000000
maxPods: 110
```

創建kubelet配置文件

```
vi /opt/kubernetes/cfg/kubelet
KUBELET_OPTS="--logtostderr=true \
--v=4 \
--hostname-override=192.168.56.22 \
--network-plugin=cni \
--kubeconfig=/opt/kubernetes/cfg/kubelet.kubeconfig \
--bootstrap-kubeconfig=/opt/kubernetes/cfg/bootstrap.kubeconfig \
--config=/opt/kubernetes/cfg/kubelet.config \
--cert-dir=/opt/kubernetes/certs \
--pod-infra-container-image=rgcr.io/google_containers/pause-amd64:3.0"
```

創建kubelet的kubelet.service文件

```
vi /usr/lib/systemd/system/kubelet.service 
[Unit]
Description=Kubernetes Kubelet
After=docker.service
Requires=docker.service

[Service]
EnvironmentFile=/opt/kubernetes/cfg/kubelet
ExecStart=/opt/kubernetes/bin/kubelet $KUBELET_OPTS
Restart=on-failure
KillMode=process

[Install]
WantedBy=multi-user.target
```

將kubelete-bootstrap用戶綁定到系統集群角色master執行

```
/opt/kubernetes/bin/kubectl create clusterrolebinding kubelet-bootstrap \
--clusterrole=system:node-bootstrapper \
--user=kubelet-bootstrap
```

重啟kubelet服務

```
systemctl daemon-reload
systemctl enable kubelet
systemctl restart kubelet
```

**至master查看註冊請求**

```
kubectl get csr
NAME                                                   AGE   SIGNERNAME                                    REQUESTOR           CONDITION
node-csr-bNu4CS9bELJohevr1EJjczDzg96lX8yzNTM6P1gmWRU   68s   kubernetes.io/kube-apiserver-client-kubelet   kubelet-bootstrap   Pending
node-csr-h3LtxmKsVm0GsesE4WKB5_H3iQWtCTFGKa2mt4C_7QA   95s   kubernetes.io/kube-apiserver-client-kubelet   kubelet-bootstrap   Pending
```

**master同意node的註冊**

```
kubectl certificate approve node-csr-bNu4CS9bELJohevr1EJjczDzg96lX8yzNTM6P1gmWRU

kubectl certificate approve node-csr-h3LtxmKsVm0GsesE4WKB5_H3iQWtCTFGKa2mt4C_7QA
```

```
kubectl get csr 
NAME                                                   AGE   SIGNERNAME                                    REQUESTOR           CONDITION
node-csr-bNu4CS9bELJohevr1EJjczDzg96lX8yzNTM6P1gmWRU   10m   kubernetes.io/kube-apiserver-client-kubelet   kubelet-bootstrap   Approved,Issued
node-csr-h3LtxmKsVm0GsesE4WKB5_H3iQWtCTFGKa2mt4C_7QA   11m   kubernetes.io/kube-apiserver-client-kubelet   kubelet-bootstrap   Approved,Issued

```

### 部署kube-proxy

kube-proxy

```
vi /opt/kubernetes/cfg/kube-proxy
KUBE_PROXY_OPTS="--logtostderr=false \
--v=4 \
--hostname-override=192.168.56.21 \
--cluster-cidr=10.0.0.0/24 \
--proxy-mode=ipvs \
--masquerade-all=true \
--log-dir=/opt/kubernetes/logs \
--kubeconfig=/opt/kubernetes/cfg/kube-proxy.kubeconfig"
```

創建kube-proxy systemd 文件

```
vi /usr/lib/systemd/system/kube-proxy.service
[Unit]
Description=Kubernetes Proxy
After=network.tarsget

[Service]
EnvironmentFile=/opt/kubernetes/cfg/kube-proxy
ExecStart=/dopt/kubernetes/bin/kube-proxy $KUBE_PROXY_OPTS
Restart=on-failure

[Install]
WantedBy=multi-user.target
```

啟動服務

```
systemctl daemon-reload
systemctl enable kube-proxy
systemctl restart kube-proxy
```



### 錯誤回報

```
F1103 17:53:45.239189    3689 server.go:265] failed to run Kubelet: unable to load bootstrap kubeconfig: invalid configuration: no server found for cluster "kubernetes"

```



----------------------------------------------------------------------------------------------------------------------------

## K8S kubeadm安裝

### 前期準備

k8s-master01

k8s-node01

k8s-node02

Harbor

1. 關閉防火牆

2. 關閉SELINUX

3. 關閉swap分區

   ```
   swapoff -a && sed -ri '/ swap / s/^(.*)$/#\1/g' /etc/fstab
   
   cat /etc/fstab
   ```

4. 配置時間同步

   ```
   yum install chrony -y
   ```

5. 

安裝依賴包

```
yum install -y conntrack ntpdate ntp ipvsadm ipset jq iptables curl sysstat libseccomp wget net-tools git
```

設置iptable為防火牆並設置空規則

```
yum -y install iptables-services && systemctl start iptables && systemctl enable iptables && iptables -F && service iptables save
```

設定iptables相關功能

```
systemctl disable firewalld && systemctl stop firewalld

echo 1 > /proc/sys/net/ipv4/ip_forward

echo "net.ipv4.ip_forward = 1" >> /etc/sysctl.conf

echo "net.bridge.bridge-nf-call-iptables = 1" >> /etc/sysctl.conf

modprobe br_netfilter

echo "br_netfilter" > /etc/modules-load.d/br_netfilter.conf

sysctl -p

lsmod | grep br_netfilter
br_netfilter           22256  0 
bridge                151336  1 br_netfilter
```

調整內核參數

```
cat>/etc/sysctl.d/kubernetes.conf<<EOF
net.bridge.bridge-nf-call-iptables=1
net.bridge.bridge-nf-call-ip6tables=1
net.ipv4.ip_forward=1
net.ipv4.tcp_tw_recycle=0
vm.swappiness=0
vm.overcommit_memory=1
vm.panic_on_oom=0
fs.inotify.max_user_instances=8192
fs.inotify.max_user_watches=1048576
fs.file-max=52706063
fs.nr_open=52706963
fs.ipv6.conf.all.disable_ipv6=1
net.netfilter.nf_conntrack_max=2310720
EOF

sysctl -p /etc/sysctl.d/kubernetes.conf
```

kube-proxy 開啟ipvs的前置條件

```
modprobe br_netfilter

cat > /etc/sysconfig/modules/ipvs.modules <<EOF
#!/bin/bash
modprobe -- ip_vs
modprobe -- ip_vs_rr
modprobe -- ip_vs_wrr
modprobe -- ip_vs_sh
modprobe -- nf_conntrack_ipv4
EOF
chmod 755 /etc/sysconfig/modules/ipvs.modules && bash /etc/sysconfig/modules/ipvs.modules && lsmod | grep -e ip_vs -e nf_conntrack_ipv4
```

### 安裝docker container runtime interface

```
yum install -y yum-utils device-mapper-persistent-data lvm2 

yum-config-manager --add-repo https://download.docker.com/linux/centos/docker-ce.repo 

yum install docker-ce -y


systemctl start docker && systemctl enable docker
-----------------------------------------------------------
yum install -y \
  docker-ce-18.09* \
  docker-ce-cli-18.09*
```

配置docker daemon

```
mkdir /etc/docker -p
cat >/etc/docker/daemon.json<<EOF
{
	"exec-opts": ["native.cgroupdriver=systemd"],
	"log-driver": "json-file",
	"log-opts": {
		"max-size": "100m"
	},
	"insecure-registries": ["https://harbor.example.com"]
}
EOF

mkdir -p /etc/systemd/system/docker.service.d

systemctl daemon-reload && systemctl restart docker && systemctl enable docker 

```

### 安裝kubernetes（主從配置）

```
cat>/etc/yum.repos.d/kubernetes.repo<<EOF
[kubernetes]
name=Kubernetes
baseurl=https://packages.cloud.google.com/yum/repos/kubernetes-el7-x86_64
enabled=1
gpgcheck=1
repo_gpgcheck=1
gpgkey=https://packages.cloud.google.com/yum/doc/yum-key.gpg https://packages.cloud.google.com/yum/doc/rpm-package-key.gpg
EOF

yum clean all && yum repolist

yum -y install kubelet-1.15.2 kubectl-1.15.2 kubeadm-1.15.2 
systemctl enable kubelet.service
```

#### 初始化master node

api-server-advertise指定為master的內部IP，pod-network-cidr與service-cidr都採預設值，也可以變更為其他

```
kubeadm config print init-defaults > ~/install-k8s/core/kubeadm-config.yaml

apiVersion: kubeadm.k8s.io/v1beta2
bootstrapTokens:
- groups:
  - system:bootstrappers:kubeadm:default-node-token
  token: abcdef.0123456789abcdef
  ttl: 24h0m0s
  usages:
  - signing
  - authentication
kind: InitConfiguration
localAPIEndpoint:
  advertiseAddress: 192.168.56.10
  bindPort: 6443
nodeRegistration:
  criSocket: /var/run/dockershim.sock
  name: k8s-master01
  taints:
  - effect: NoSchedule
    key: node-role.kubernetes.io/master
---
apiServer:
  timeoutForControlPlane: 4m0s
apiVersion: kubeadm.k8s.io/v1beta2
certificatesDir: /etc/kubernetes/pki
clusterName: kubernetes
controllerManager: {}
dns:
  type: CoreDNS
etcd:
  local:
    dataDir: /var/lib/etcd
imageRepository: k8s.gcr.io
kind: ClusterConfiguration
kubernetesVersion: v1.15.2
networking:
  dnsDomain: cluster.local
  podSubnet: 10.244.0.0/16
  serviceSubnet: 10.96.0.0/12
scheduler: {}
---
apiVersion: kubeproxy.config.k8s.io/v1alpha1
kind: KubeProxyConfiguration
featureGates:
  SupportIPVSProxyMode: true
mode: ipvs
```



```
kubeadm init --config=$HOME/install-k8s/core/kubeadm-config.yaml --upload-certs | tee kubeadm-init.log
-----------------------------------------------------------
Your Kubernetes control-plane has initialized successfully!

To start using your cluster, you need to run the following as a regular user:

  mkdir -p $HOME/.kube
  sudo cp -i /etc/kubernetes/admin.conf $HOME/.kube/config
  sudo chown $(id -u):$(id -g) $HOME/.kube/config

You should now deploy a pod network to the cluster.
Run "kubectl apply -f [podnetwork].yaml" with one of the options listed at:
  https://kubernetes.io/docs/concepts/cluster-administration/addons/

Then you can join any number of worker nodes by running the following on each as root:

kubeadm join 192.168.56.10:6443 --token abcdef.0123456789abcdef \
    --discovery-token-ca-cert-hash sha256:0e5aeef6db74b4056fe052925d572bab85997e9de9431db9d25f2387632b6054 
```



安裝完成要求依照他的指示操作

```
mkdir -p $HOME/.kube

sudo cp -i /etc/kubernetes/admin.conf $HOME/.kube/config

sudo chown $(id -u):$(id -g) $HOME/.kube/config
```

Node加入

```
kubeadm join 192.168.56.10:6443 --token abcdef.0123456789abcdef \
    --discovery-token-ca-cert-hash sha256:0e5aeef6db74b4056fe052925d572bab85997e9de9431db9d25f2387632b6054 
```



安裝通用的 flannel容器網路介面[CNI](https://jimmysong.io/kubernetes-handbook/concepts/cni.html)（Container Network Interface）元件

```
mkdir ~/install-k8s
mv kubeadm-init.log install-k8s 
mv kubeadm-config.yaml install-k8s
cd ~/install-k8s
mkdir core
mv * core/
kubectl apply -f https://raw.githubusercontent.com/coreos/flannel/master/Documentation/kube-flannel.yml

kubectl get pod -n kube-system
NAME                                            READY   STATUS    RESTARTS   AGE
coredns-7d947ddc6b-bbxwz                        1/1     Running   0          4h12m
coredns-7d947ddc6b-jdnds                        1/1     Running   0          4h12m
etcd-localhost.localdomain                      1/1     Running   0          4h12m
kube-apiserver-localhost.localdomain            1/1     Running   0          4h12m
kube-controller-manager-localhost.localdomain   1/1     Running   2          4h13m
kube-flannel-ds-grwkj                           1/1     Running   0          3m35s
kube-proxy-n9dzk                                1/1     Running   0          4h12m
kube-scheduler-localhost.localdomain            1/1     Running   1          4h12m

kubectl get node
[root@k8s-master01 flannel]# kubectl get node 
NAME                    STATUS   ROLES    AGE     VERSION
localhost.localdomain   Ready    master   4h14m   v1.15.2

```

Node加入集群，在node節點中輸入以下指令

```
kubeadm join 192.168.101.28:6443 --token gny70m.2v41qsd2t3jllxk  --discovery-token-ca-cert-hash sha256:f25d9d5d03fe993976daa053f23c546fa946cb6faa92c82c5c1946806aa57932
```

k8s-master01

```
kubectl run nginx-deployment --image=hub.atguigu.com/public/nginx@sha256:416d511ffa63777489af47f250b70d1570e428b67666567085f2bece3571ad83 --port=80 --replicas=1
kubectl get deployment
kubectl get rs
kubectl get pod
kubectl get pod -o wide 
```

## K8S WEB dashboard

安裝

```
kubectl apply -f https://raw.githubusercontent.com/kubernetes/dashboard/v2.0.0/aio/deploy/recommended.yaml
```

查看安裝

```
kubectl get pods --namespace=kubernetes-dashboard -o wide
NAME                                         READY   STATUS    RESTARTS   AGE   IP           NODE        NOMINATED NODE   READINESS GATES
dashboard-metrics-scraper-7b59f7d4df-cgp24   1/1     Running   0          56m   10.244.2.4   k8s-node2   <none>           <none>
kubernetes-dashboard-665f4c5ff-szngb         1/1     Running   0          56m   10.244.1.4   k8s-node1   <none>           <none>
```

查看pod日誌

```
kubectl describe pod dashboard-metrics-scraper-566cddb686-6p8tb --namespace=kubernetes-dashboard
Namespace:    kubernetes-dashboard
Priority:     0
Node:         k8s-node2/192.168.56.22
Start Time:   Tue, 27 Oct 2020 17:09:59 +0800
Labels:       k8s-app=dashboard-metrics-scraper
              pod-template-hash=7b59f7d4df
Annotations:  seccomp.security.alpha.kubernetes.io/pod: runtime/default
Status:       Running
IP:           10.244.2.4
IPs:
  IP:           10.244.2.4
Controlled By:  ReplicaSet/dashboard-metrics-scraper-7b59f7d4df
Containers:
  dashboard-metrics-scraper:
    Container ID:   docker://982c6ed9cf5000214299f3882a12194a545e11148469ed87e1ea7e538ce28697
    Image:          kubernetesui/metrics-scraper:v1.0.4
    Image ID:       docker-pullable://kubernetesui/metrics-scraper@sha256:555981a24f184420f3be0c79d4efb6c948a85cfce84034f85a563f4151a81cbf
    Port:           8000/TCP
    Host Port:      0/TCP
    State:          Running
      Started:      Tue, 27 Oct 2020 17:10:43 +0800
    Ready:          True
    Restart Count:  0
    Liveness:       http-get http://:8000/ delay=30s timeout=30s period=10s #success=1 #failure=3
    Environment:    <none>
    Mounts:
      /tmp from tmp-volume (rw)
      /var/run/secrets/kubernetes.io/serviceaccount from kubernetes-dashboard-token-j85kq (ro)
Conditions:
  Type              Status
  Initialized       True 
  Ready             True 
  ContainersReady   True 
  PodScheduled      True 
Volumes:
  tmp-volume:
    Type:       EmptyDir (a temporary directory that shares a pod's lifetime)
    Medium:     
    SizeLimit:  <unset>
  kubernetes-dashboard-token-j85kq:
    Type:        Secret (a volume populated by a Secret)
    SecretName:  kubernetes-dashboard-token-j85kq
    Optional:    false
QoS Class:       BestEffort
Node-Selectors:  kubernetes.io/os=linux
Tolerations:     node-role.kubernetes.io/master:NoSchedule
                 node.kubernetes.io/not-ready:NoExecute op=Exists for 300s
                 node.kubernetes.io/unreachable:NoExecute op=Exists for 300s
Events:          <none>
```

使用NodePort來訪問

```
kubectl --namespace=kubernetes-dashboard edit service kubernetes-dashboard
```

將Type更改為NodePort並重新查看

```
kubectl --namespace=kubernetes-dashboard get service kubernetes-dashboard
NAME                   TYPE       CLUSTER-IP      EXTERNAL-IP   PORT(S)         AGE
kubernetes-dashboard   NodePort   10.99.173.215   <none>        443:30251/TCP   169m
```



使用`nodeIP:port`訪問

```
https://192.168.56.21:30251
```

生成證書

```
#新建目录：
mkdir key && cd key

#生成证书
openssl genrsa -out dashboard.key 2048 

#我这里写的自己的node1节点，因为我是通过nodeport访问的；如果通过apiserver访问，可以写成自己的master节点ip
openssl req -new -out dashboard.csr -key dashboard.key -subj '/CN=192.168.56.21'
openssl x509 -req -in dashboard.csr -signkey dashboard.key -out dashboard.crt 

#删除原有的证书secret
kubectl delete secret kubernetes-dashboard-certs -n kubernetes-dashboard

#创建新的证书secret
kubectl create secret generic kubernetes-dashboard-certs --from-file=dashboard.key --from-file=dashboard.crt -n kubernetes-dashboard

#查看pod
kubectl get pod -n kubernetes-dashboard

#重启pod
kubectl delete pod kubernetes-dashboard-7b5bf5d559-gn4ls  -n kubernetes-dashboard
```

產生token

```
# 創建Service Account
cat <<EOF | kubectl apply -f -
apiVersion: v1
kind: ServiceAccount
metadata:
  name: admin-user
  namespace: kubernetes-dashboard
EOF

cat <<EOF | kubectl apply -f -
apiVersion: rbac.authorization.k8s.io/v1
kind: ClusterRoleBinding
metadata:
  name: admin-user
roleRef:
  apiGroup: rbac.authorization.k8s.io
  kind: ClusterRole
  name: cluster-admin
subjects:
- kind: ServiceAccount
  name: admin-user
  namespace: kubernetes-dashboard
EOF

# 取得bearer token
kubectl -n kubernetes-dashboard describe secret $(kubectl -n kubernetes-dashboard get secret | grep admin-user | awk '{print $1}')
Name:         admin-user-token-zb85b
Namespace:    kubernetes-dashboard
Labels:       <none>
Annotations:  kubernetes.io/service-account.name: admin-user
              kubernetes.io/service-account.uid: 34e12570-0a17-4cc8-9705-b2105b4d4ce5

Type:  kubernetes.io/service-account-token

Data
====
ca.crt:     1066 bytes
namespace:  20 bytes
token:      eyJhbGciOiJSUzI1NiIsImtpZCI6Ik5vRWE0NTNYNksxRG5iU2ExUEc5eU1jS2NFb0JzeTZSS0t4TDBwX1ItREkifQ.eyJpc3MiOiJrdWJlcm5ldGVzL3NlcnZpY2VhY2NvdW50Iiwia3ViZXJuZXRlcy5pby9zZXJ2aWNlYWNjb3VudC9uYW1lc3BhY2UiOiJrdWJlcm5ldGVzLWRhc2hib2FyZCIsImt1YmVybmV0ZXMuaW8vc2VydmljZWFjY291bnQvc2VjcmV0Lm5hbWUiOiJhZG1pbi11c2VyLXRva2VuLXpiODViIiwia3ViZXJuZXRlcy5pby9zZXJ2aWNlYWNjb3VudC9zZXJ2aWNlLWFjY291bnQubmFtZSI6ImFkbWluLXVzZXIiLCJrdWJlcm5ldGVzLmlvL3NlcnZpY2VhY2NvdW50L3NlcnZpY2UtYWNjb3VudC51aWQiOiIzNGUxMjU3MC0wYTE3LTRjYzgtOTcwNS1iMjEwNWI0ZDRjZTUiLCJzdWIiOiJzeXN0ZW06c2VydmljZWFjY291bnQ6a3ViZXJuZXRlcy1kYXNoYm9hcmQ6YWRtaW4tdXNlciJ9.VPKSMCgigajNYlCa-f8XHGo5aYDG7zpU4zCHWOeM5S4iLId37nqQKElLuMjdrKOtdi19xpztU597Y3X7J-CCCLHjZ1bbbDD1BTQONqKQsSzHeTEQXKusmpOZCS9Ot0GAk7n6xk2I0cpOcLMDfYq-UDnKNdAfMGgCf6bHvmRCP6FMhNxNSMMf6vw6vNN3GjafEvPyuVCc7CoyniGDOrNRLGZXoQv8_D5GPM09TruBtVJl9C8f4zSrEGVCfWEuETY6suS5rRksA6vvNrROvbIsWmMbxv1S8xWZkrQkLA61UsS0hDNHHpu0XS0SzWGviYpnLdmdBXOx2YcB4uEz2pJO7A

```

## 高可用架構

![image-20201028140744764](https://d33wubrfki0l68.cloudfront.net/d1411cded83856552f37911eb4522d9887ca4e83/b94b2/images/kubeadm/kubeadm-ha-topology-stacked-etcd.svg)

## CoreDNS

coredns.yaml



## 遠程管理k8s

預設情況下，只可以在master上管理k8s 集群

1. 將管理工具複制到node上

2. 生成管理員證書

   ```
   vim admin-csr.json
   {
   	"CN": "k8s-etcd",
   	"hosts": [],
   	"key": {
   		"algo": "rsa",
   		"size": 2048
   	},
   	"names": [
   		{
   			"C": "TW",
   			"ST": "TW",
   			"L": "Taipei",
   			"O": "system:masters",
   			"OU": "System"
   		}
   	]
   }
   
   cfssl gencert -ca=ca.pem -ca-key=ca-key.pem -config=ca-config.json -profile=peer etcd-peer-csr.json
   
   ```

3. 創建kubeonfig文件

4. 

## K8S使用

創建

```
kubectl run nginx-deploy --image=nginx --port=80 --replicas=1
```

獲取資訊

```
kubectl get deployment
kubectl get pods -o wide
```

刪除

```
kubectl delete pod nginx-7c45b84548-qxqfk
```

#### 透過yaml文件創建

1. 創建文件

   ```
   touch nginx_pod.yml
   ```

2. 編輯yaml文件

   ```
   apiVersion: apps/v1
   kind: Deployment
   metadata:
       name: beego-nginx
       labels:
         app: nginx
   spec:
       replicas: 3
       selector:
          matchLabels:
            app: nginx
       template:
          metadata:
            labels:
              app: nginx
          spec:
            volumes:
            - name: tz-config
              hostPath:
                path: /etc/localtime
            containers:
            - name: nginx
              image: nginx
              command: ["/bin/bash"]
              args: ["-c","hostname > /usr/share/nginx/html/index.html && nginx -g\"daemon off;\""]
              ports:
              - containerPort: 80
              volumeMounts:
              - name: tz-config
                mountPath: /etc/localtime
                readOnly: true
   ```

3. 根據yaml文件創建pod

   ```
   kubectl apply -f nginx_pod.yml
   ```

4. 查看pod運行情況

   ![image-20200325150206474](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200325150206474.png)

5. 在/tmp/test01下創建nginx_srv.yml

6. 編輯

   ```
   kind: Service
   apiVersion: v1
   metadata:
       name: nignx-service
   spec:
       type: NodePort
       selector:
          app: nginx
       ports:
       - protocol: TCP
         port: 80
         targetPort: 80
         nodePort: 30002
   ```

7. 根據yml文件創建服務

   ```
   kubectl apply -f 
   ```

8. 查看

   ```
   kubectl get service
   NAME            TYPE        CLUSTER-IP     EXTERNAL-IP   PORT(S)        AGE
   kubernetes      ClusterIP   10.96.0.1      <none>     443/TCP        10h
   nignx-service   NodePort    10.97.211.56   <none>        80:30002/TCP   73s
   ```

9. 透過工作節點訪問 確認負載均衡的功能

10. ```
    [root@master test01]# curl 10.97.211.56
    beego-nginx-79d8474457-bvj4b
    [root@master test01]# curl 10.97.211.56
    beego-nginx-79d8474457-pkb48
    [root@master test01]# curl 10.97.211.56
    beego-nginx-79d8474457-bvj4b
    [root@master test01]# curl 10.97.211.56
    beego-nginx-79d8474457-pkb48
    [root@master test01]# curl 10.97.211.56
    beego-nginx-79d8474457-bvj4b
    [root@master test01]# curl 10.97.211.56
    beego-nginx-79d8474457-tks2q
    
    ```

#### 應用副本的動態伸縮

1. 創建應用

   ```
   kubectl run myapp --image=nginx --replicas=2 
   
   #查看
   kubectl get deplyment
   ```

2. 增加副本數

   ```
   kubectl scale --replicas=5 deployemnt myapp
   ```

3. 
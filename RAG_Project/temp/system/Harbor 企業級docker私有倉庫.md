# Harbor 企業級docker私有倉庫

## 安裝harbor

* yum install docker-compose -y
* python 2.7
* docker

### 安裝簽發證書服務

創建用到的目錄

```
mkdir /opt/certs -p
mkdir /opt/harbor -p
mkdir /data/harbor/logs -p

cd /opt/certs/
```

下載cfssl的二進制

```
wget https://pkg.cfssl.org/R1.2/cfssl_linux-amd64 -O /usr/bin/cfssl

wget https://pkg.cfssl.org/R1.2/cfssljson_linux-amd64 -O /usr/bin/cfssl-json

wget https://pkg.cfssl.org/R1.2/cfssl-certinfo_linux-amd64 -O /usr/bin/cfssl-certinfo

chmod +x /usr/bin/cfssl*
```

創建ca請求文件

```
cat> /opt/certs/ca-csr.json<<EOF
{
	"CN": "harbor.example.com",
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
			"O": "it",
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
cd /opt/certs/
cfssl gencert -initca /opt/certs/ca-csr.json | cfssl-json -bare ca
```

查看

```
ls 
ca-csr.json  ca-key.pem  ca.csr  ca.pem
cat ca-key.pem
```

### 部署docker私有鏡像倉庫harbor

https://github.com/goharbor/harbor/releases

```
mkdir -p /opt/src && cd /opt/src
wget https://github.com/goharbor/harbor/releases/download/v2.1.0/harbor-offline-installer-v2.1.0.tgz
#解壓harbor包到/opt中
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

### 配置docker daemon.json

```
vi /etc/docker/daemon.json
{
"exec-opts": ["native.cgroupdriver=systemd"],
"log-driver": "json-file",
"log-opts": {
"max-size": "100m"
},
"insecure-registries": ["https://hub.example.com"]
}

```

```
cfssl gencert -ca=../ca.pem -ca-key=../ca-key.pem -config=../ca-config.json -profile=peer harbor-peer-csr.json | cfssl-json -bare harbor-peer 
```



### 安裝nginx

nginx用來做為harbor的反向代理

```
yum install nginx -y
```

vi /etc/nginx/conf.d/harbor.example.com.conf

```
server {
	listen    80;
	server_name hub.atguigu.com;
	client_max_body_size  1000m;
	location / {
		proxy_pass http://192.168.56.11:180; #harbor IP
	}
}
```

新建項目 

* public
* 公開

對倉庫做驗證

```
vi /etc/sysconfig/docker
#在末行增加
OPTIONS='--insecure-registry=192.168.56.16:5000'

systemctl restart docker
```

使用docker登入私有倉庫

```
docker images | grep 1.7.9
nginx                         1.7.9               84581e99d807        5 years ago         91.7MB
 docker tag 84581e99d807 hub.atguigu.com/public/nginx:v1.7.9
 docker login hub.atguigu.com
 docker push hub.atguigu.com/public/nginx:v1.7.9
```


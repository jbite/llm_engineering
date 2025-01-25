# Linux雲計算 Docker

## Docker介紹

docker的底層技術實現架構

2. docker engine
   * docker engine是核心，裡面有後台進程dockerd

## Docker體系結構

鏡像： 包裝好的環境，用來運行在容器內

容器：隔離好的環境，用來運行映像

倉庫

底層技術支持：

* namespace:做隔離、pid、net 可以啟動多個容器
* control groups 做資源限制 比如對內存 對CPU做限制
* union file systems

#### docker化應用體驗

```
docker run --name db --env MYSQL_ROOT_PASSWORD=example -d mariadb
docker run --name MyWordPress --link db:mysql -p 8080:80 -d wordpress
```

## Docker安裝

```
yum install docker -y 
```

```
systemctl enable docker
systemctl start docker
```

## docker-compose 

docker提倡理念是一個容器一個進程，假設一個服務需要由多個進程組成，就需要多個容器組成一個系統，相互分工和配合對外提供完整服務

組件1：mariadb

組件2：wordpress的apache web

在啟動容器是，同一台主機下如果兩個容器之間需要由數據交流，使用--link選項建立的互聯，前提是mariadb已經開啟

容器編排工具，允許用戶在一個模版（YAML）中定義一組相關聯的容器，會根據--link等參數，對啟動的優先級進行排序

### docker-compose 操作

```
-f        指定使用的yaml文件位置
ps        顯示所有容器信息
restart   重新啟動容器
logs      查看日誌信息
config -q 驗證yaml配置文件是否正確
stop      停止容器
start     啟動容器
up -d     啟動容器項目
pause     暫停容器
unpause   恢復暫停
rm        刪除容器
```

### docker-compose操作示例

wordpress.yml

```
version: '2'

services:
  db:
     image: mysql:5.7
     restart: always
     environment:
       MYSQL_ROOT_PASSWORD: somewordpress
       MYSQL_DATABASE: wordpress
       MYSQL_USER: wordpress
       MYSQL_PASSWORD: wordpress
  wordpress:
     depends_on:
       - db
     image: wordpress:latest
     restart: always
     ports:
       - "8000:80"
     environment:
       WORDPRESS_DB_HOST: DB:3306
       WORDPRESS_DB_USER: wordpress
       WORDPRESS_DB_PASSWORD: wordpress
```

啟動容器

```
docker -f docker-compose.ymal -d up 
```

## docker基礎概念

倉庫(repository)

鏡像(image)

容器(container)

### 創建容器

```
docker run --name ConatinerName --link db:mysql -p 8080:80 -d image_name
```

### docker基本命令

```
docker info       守護進程的系統資源設置
docker search     Docker倉庫的查詢
docker pull       Docker倉庫的下載
docker images     Docker鏡像的查詢
docker rmi        Docker鏡像的刪除
docker ps         容器的查詢
docker run        容器的創建啟動
docker start/stop 容器啟動停止
#docker指令可以直接使用
```

構建image，最後的.表示當前目錄 -t 為自己的帳號

```
docker build -t jbite/hello-world .
```

查看Image的分層

```
docker history img_id
```

image運行

```
docker run jbite/hello-world
```

container概念和使用

* container可讀寫，Image只可讀

* 例子，類與對像的關係

* 查詢本地container

  ```
  docker container ls
  docker container ls -a
  ```

* 進行交互運行容器

  ```
  docker run -it centos 
  [root@319f0fb2ce56]#
  ```

* 刪除容器

  ```
  docker container rm container_id
  
  #列出目前所有container ID
  docker container ls -aq
  
  #刪除括弧內產生的內容
  docker rm $(docker container ls -qa)
  
  #刪除已經運行完畢的container
  docker container ls -f "status-exited"
  docker container rm $(docker container ls -f "status-exited")
  ```

#### 自訂義容器名稱

```
docker run -d --name=demo jbite9057/flask-hello

 docker container ls
CONTAINER ID        IMAGE                   COMMAND             CREATED             STATUS                  PORTS               NAMES
2d6f596fb62a        jbite9057/flask-hello   "python app.py"     10 seconds ago      Up 8 seconds        5000/tcp            demo
```

* 容器名稱操作

  ```
  docker stop demo
  docker start demo
  docker inspect demo
  ```

* 查看容器日志

  ```
  #持續查看日志尾部
  docker logs -f demo
  #查看日志尾部
  docker logs --tail demo 
  ```

* 修改名稱

  ```
  docker container rename demo demo2
  ```

#### docker 指令示例

docker images

```
# docker images 
REPOSITORY              TAG                 IMAGE ID            CREATED             SIZE
docker.io/mariadb       latest              4e7e0dfceed8        10 hours ago        406 MB
docker.io/nginx         latest              992e3b7be046        2 days ago          133 MB
docker.io/wordpress     latest              1b83fad37165        6 days ago          546 MB
docker.io/mysql         5.7                 ef08065b0a30        4 weeks ago         448 MB
docker.io/hello-world   latest              bf756fb1ae65        9 months ago        13.3 kB

```

### container ID

每個容器被創建後，都會分配一個container ID作為容器的唯一標示，後續對容器的啟動、停止、修改、刪除等操作，都是通過container ID來完成，偏向於資料庫概念中的主鍵

### 容器管理

```
docker ps --no-tunc
docker stop/start ID
docker inspect ID  # 查看容器的完整資訊
docker logs ID     # 查看容器的日誌
docker stats ID
docker exec ID
	docker exec -it db /bin/bash   #交互式存取
docker run
	--restart=always     容器的自動啟動
	-h x.xx.xx           設置容器主機名
	--dns x.x.x.x        設置容器使用的DNS伺服器
	--dns-search         DNS搜索設置
	--add-host hostname:ip 注入hostname()IP解析
	--rm                 服務停止時自動刪除
```

### docker image概述

* 文件和meta data的集合
* 鏡像名和版本號合成一個標識符
* 鏡像的分層：docker的鏡像通過聯合文件系統（union file system）將各層文件系統疊加在一起
  * bootfs：用於系統引導的文件系統，包括bootloader和kernel，容器啟動完成後會被卸載以節省記憶體資源
  * rootfs：位於bootfs之上，表現為docker容器的根文件系統
    * 傳統模式：系統啟動時，內核掛載rootfs時會首先將其掛載為"只讀"模式，完整性自檢完成後將其掛載為讀寫模式
    * Docker中，rootfs由內核掛載為"只讀"模式，而後通過UFS技術掛載一個"可寫"層
  * 已有的分層只能讀不能修改
  * 上層鏡像優先級大於底層鏡像

## 容器轉換成鏡像

### 使用docker commit製作鏡像

```
docker commit [OPTIONS] CONTAINER [REPOSITORY[:TAG]]   #-t 新鏡像名稱
```

下載一個有tools的鏡像並啟用

```
docker pull hub.c.163.com/public/centos:6.7-tools
docker --name mysql -d hub.c.163.com/public/centos:6.7-tools
```

進入容器並安裝mysql

```
docker -it mysql /bin/bash
yum install mysql-server
service mysqld start
mysqladmin -uroot password 123
mysql
create database shangguigu;
```

製作鏡像

```
#執行鏡像製作
docker commit mysql mysql:5.1
#查看新鏡像
docker images
REPOSITORY     TAG  IMAGE ID      CREATED         SIZE
mysql          5.1  4d5326c2d9c5  6 seconds ago   792 MB

#使用新鏡像運行容器
docker run --name mysql-my -d 4d5326c2d9c5
#docker ps 
```

* 運行image

  ```
  docker run img_id
  ```

* 在容器中安裝lrzsz

  ```
  [root@f836621cc37c /]# yum install lrzsz -y
  ```

* 構建新image

  ```
  #查詢docker暱稱
  docker container ls -a
  CONTAINER ID        IMAGE               COMMAND             CREATED            STATUS                       PORTS               NAMES
  f836621cc37c        470671670cac        "/bin/bash"         2 minutes ago      Exited (0) 7 seconds ago                         silly_cray
  
  #執行創建新鏡像
  docker commit silly_cray jbite/centos-lrzsz
  
  docker image ls
  REPOSITORY          TAG    IMAGE ID     CREATED       SIZE
  jbite/centos-lrzsz  latest c75a0389a366 2 minutes ago 274 MB
  ```

### 使用Dockerfile製作鏡像

dockerfile是一種被docker程序解釋的腳本，dockerfile由一條一條的指令組成，每條指令對應linux下面的一條命令。docker程序將這些dockerfile指令翻譯真正的linux命令。dockerfiler有自己書寫格式和支持的命令，docker程序解決這些命令間的依賴關係，類似於makefile。docker程序將讀取dockerfile，根據指令生成定制的image

1. 創建Dockerfile

   ```
   cat ~/test2/Dockerfile
   FROM centos
   RUN yum -y install lrzsz
   RUN mkdir /data
   RUN touch /data/myfile.txt
   CMD ["tail","-f","/data/myfile.txt"]
   ```

2. 構建

   ```
   cd test2
   docker build -t jbite/centos-lrzsz-dockerfile .
   ```

3. 查看

   ```
   Successfully built e046515b179a
   [root@docker test2]# docker image ls
   REPOSITORY                    TAG    IMAGE ID      CREATED    SIZE
   jbite/centos-lrzsz-dockerfile latest e046515b179a 39 seconds ago  274 MB
   ```

#### Dockerfile詳解



```
1. FROM 指定基礎鏡像
構建指令，必須指定且需要在dockerfile其他指令的前面，後續的指令都依賴於該指令指定的image，FROM指令指定的基礎image可以是官方遠程倉庫中的，也可以位於本地倉庫

2. MAINTAINER（用來指定鏡像創建者信息）
構建指令，用於將image的製作者相關的信息寫入到image中，當我們對該image執行docker inspect命令時，輸出中有相應的字段記錄該信息

3. RUN（安裝軟體用）
構建指令，RUN可以運行任何被基礎image支持的命令。如基礎image選擇了centos，那麼軟體管理部份只能使用centos的包管理命令

4. CMD（設置container啟動時執行的操作）
設置指令，用於container啟動時指定的操作。該操作可以是執行自定義腳本也可以是執行系統命令。該指令只能在文件中存在一次。如果有多個，則只執行最後一條。

5. ENTRYPOINT（設置container啟動時執行的動作）
	ENTRYPOINT ls -l
	CMD指令配合來指定參數，這時CMD指令不是一個完整的可執行命令，僅僅是參數部份；ENTRYPOINT指令只能使用JSON方式指定執行命令，而不能指定參數
	FROM unbuntu
	CMD ["-l]
	ENTRYPOINT ["/usr/bin/ls"]
6. USER (設置container容器的用戶)
設置指令，設置啟動容器的用戶，畎認是root用戶
example：
	USER daemon = ENTRYPOINT ["memcached","-u","daemon"]
7. EXPOSE(指定容器需要映射到宿主機器的端口)
設置指令，該指令會將容器中的端口映射成宿主機器中的某個端口。當你需要訪問容器的時候，可以不是用容器的IP地址而是使用宿主機器的IP地址和映射後的端口。要完成整個操作需要兩個步驟，首先在dockerfile使用EXPOSE設置需要映射的容器端口，が後在運行容器的時候指定-p選項加上EXPOSE設置的端口，這樣EXPOSE設置的端口號會被隨機映射成宿主機器中的一個端口號。也可以指定需要映射到宿主機器的那個端口，這時要確保宿主機器上的端口號沒有被使用。EXPOSE指令可以一次設置多個端口號，相應的運行容器的時候，可以配套的多次使用-p選項。
example：
	映射一個端口
	EXPOSE 22
	相應的運行容器使用的命令
	docker run -p port1 image
	
	映射多個端口
	EXPOSE port1 port2 port3
	相應的運行容器使用的命令
	docker run -p port1 -p port2 -p port3 image
	還可以指定需要映射到宿主機器上的某個端口號
    docker run -p host_port1:port1 -p host_port2:port2 -p host_port3:port3 image
8. ENV(用於設置環境變量)：構建指令，在image中設置一個環境變量
example：
	設置了後，後續的RUN命令都可以使用，container啟動後，可以通過docker inspect查看這個環境變量，也可以通過在docker run --env key=value時設置或修改環境變量。假如你安裝了JAVA程序，需要設置JAVA_HOME，那麼可以在dockerfile中這樣寫：
	ENV JAVA_HOME /path/to /java/direct
	
9. ADD(從src複制文件到container的dest路徑)
example：
	ADD <src> <dest>
	<src> 是相對被構建的源目錄的相對路徑，可以是文件或目錄的路徑，也可以是遠程的文件url
	<dest> 是container中的dest路徑
10. COPY <src> <dest>
	
11. VOLUME(指定掛載點)
設置指令，使容器中的一個目錄具有持久化存儲數據的功能，該目錄可以被容器本身使用，也可以共用給其他容器使用。我們知道容器使用的是AUFS，這種文件系統不能持久化數據，當容器關閉後，所有的更改都會丟失。當容器中的應用有持久化數據的需求時可以在dockerfile中使用應指令
example：
FROM base
VOLUME ["/tmp/data"]

12. WORKDIR(切換目錄)：設置指令，可以多次切換（相當於cd命令），對RUN, CMD, ENTRYPOINT生效
example：
	WORKDIR /p1 WORKDIR p2 RUN vim a.txt
	
13. ONBUILD (在子鏡像中執行)：ONBUILD指定的命令在構建鏡像時並不執行，而是在它的子鏡像中執行。
example：
	ONBUILD ADD . /app/src
	ONBUILD RUN /usr/local/bin/python-build --dir /app/src
```

1. FROM: 引入和開始

   ```
   #從頭baseImage
   FROM scratch
   #使用已有的image
   FROM centos
   #指定使用的版本
   FROM ubuntu:14.04
   ```

2. LABEL: 定義一些說明訊息

   ```
   LABEL maintainer=jbite@qq.com
   LABEL version="1.0"
   LABEL description="xxxx"
   ```

3. RUN: 執行命令，每執行一條RUN，多一個分層，一般用&&合併語句

   ```
   RUN yum -y update && yum install -y lrzsz \ 
   net-tool
   
   RUN apt-get -y update && apt-get install lrzsz -y
   
   RUN /bin/bash -c "source $HOME/.bashrc;echo $HOME"
   ```

4. WORKDIR : 進入或創建目錄 盡量不要使用相對路徑

   ```
   WORKDIR /root
   #如果沒有會自動創建
   WORKDIR /test
   WORKDIR demo
   #輸出: /test/demo
   RUN pwd
   ```

5. ADD和COPY: 將本地的文件，添加到image裡

   ```
   #將hello添加到根目錄下
   ADD hello /
   #將tar包直接解壓到根目錄
   ADD test.tar.gz /
   
   #最終hello應該在/root/test/下
   WORKDIR /root
   COPY hello test/
   ```

6. ENV : 增加Dockerfile的可讀性 

   ```
   #設置環境變量
   ENV MYSQL_MAJOR 5.5
   #使用常量
   $MYSQL_MAJOR
   RUN apt-get -y install mysql-server="${MYSQL_VERSION}"
   ```

##### Dockerfile CMD 和 ENTRYPOINT

```
#shell
RUN apt-get -y install lrzsz
CMD echo "hello docker"
ENTERPOINT echo "hello docker"

#exec格式
RUN ["apt-get","-y","install","lrzsz"]
CMD ["/bin/echo","hello docker"]
ENTERPOINT ["/bin/echo","hello docker"]
```

1. ENTRYPOINT : 設置容器啟動時運作的命令 一定會執行，不會被忽略

   編寫Dockerfile

   ```
   cd test03
   cat ~/test03/Dockerfile
   #shell格式
   FROM centos
   ENV name Docker
   ENTRYPOINT echo "hello $name"
   
   #
   docker build -t test1 .
   
   cat ~/test03/Dockerfile
   #exec格式
   FROM centos
   ENV name Docker
   ENTRYPOINT ["/bin/echo","$name"]
   
   docker build -t test02 .
   ```

   1. 分別運行

​			2. 改成/bin/bash

```
#exec格式
FROM centos
ENV name Docker
ENTRYPOINT ["/bin/bash","-c","echo hello $name"]

docker build -t test4 .
```

2. CMD: 設置容器啟動默認執行的參數和命令，若docker指定了其他命令，CMD可以被忽略，CMD若指定多個，只運行最後一個

   1. 編輯Dockerfile

      ```
      cat Dockerfile
      FROM centos
      ENV name Docker
      CMD echo "hello $name"
      
      docker build -t test5 . 
      ```

   2. CMD和ENTRYPOINT對比，CMD會被覆蓋

      ```
      docker run -it test5 /bin/bash
      [root@docker test03]# docker run -it test5 /bin/bash
      [root@3827b0a9469d /]#
      ```

#### 分享docker image

分享的Image一定要自己docker用戶名開頭，例如zhangsang/centos

```
docker login
username:
password:
Login Succeeded
```

上傳image

```
docker image push jbite9057/centos-lrzsz
```

#### 分享Dockerfile

分享Image不太安全 一般會分享Dockerfile

## 搭建私有registry

在一台centos中運行並安裝docker

Run a local registory:Quick Version

```
docker run -d -p 5000:5000 -v /opt/registry:/var/lib/registry --restart=always --name registy registry:2
```

```
vim /etc/docker/daemon.json
{
# 驗證客戶端IP
	"insecure-registries": ["192.168.56.15:5000"]
}
```

docker客戶端設置

```
vi /etc/sysconfig/docker
#在末行增加
OPTIONS='--insecure-registry=192.168.56.16:5000'

systemctl restart docker
```

更改鏡像tag

```
docker tag centos:tag registryHost/imageName:tag
```

上傳鏡像

```
docker push registryHost/imageName:tag 
```

查看鏡像

```
curl -XGET http://192.168.56.16:5000/v2/_catalog
{"repositories":["centos","mariadb"]}
```



登入本地registry

```
docker login 192.168.56.31:5000
username: d
password:d
#因為沒有帳號密碼 輸入任意就可以登入
```

從私有倉庫拉鏡像

倉庫地址/userName/imageName：tag

```
docker pull 192.168.56.31:5000/centos
```

### Dockerfile 案例

* 編寫一個py web應用app.py

  ```
  from flask import Flask
  app = Flask(__name__)
  @app.route('/')
  def hello():
          return "hello docker"
  if __name__ == '__main__':
          app.run()
  ```

* 安裝wget, pip，並安裝sklearn, flask模組

* 編寫Dockerfile

  ```
  FROM python:2.7
  LABEL maintainer="jbite9057@gmail.com"
  RUN pip install flask
  #將app.py複製到/app/目錄下
  COPY app.py /app/
  #設置工作目錄
  WORKDIR /app
  #暴露端口
  EXPOSE 5000
  CMD ["python","app.py"]
  ```

* 運行

  ```
  docker run -d jbite9057/flask-hello
  #查看
  docker container ls
  ```

* 操作容器

  ```
  #進入容器內的命令行
  docker exec -it dd063b372ccf /bin/bash
  #對應到Dockerfile所設定的workdir
  root@dd063b372ccf:/app#
  
  #使用環境內的python
  docker exec -it dd063b372ccf python
  
  Python 2.7.17 (default, Feb 26 2020, 17:18:08)
  [GCC 8.3.0] on linux2
  Type "help", "copyright", "credits" or "license" for more information.
  >>>
  
  #查看容器內的ip
  docker exec -it dd063b372ccf ip a
  5: eth0@if6: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc noqueue state UP group default
      link/ether 02:42:ac:11:00:02 brd ff:ff:ff:ff:ff:ff link-netnsid 0
      inet 172.17.0.2/16 scope global eth0
  ```

* 停止容器

  ```
  docker stop dd063b372ccf
  ```



### 名稱空間

| namespace | 系統調用參數  | 隔離內容                   | 內核版本 |
| --------- | ------------- | -------------------------- | -------- |
| UTS       | CLONE_NEWUTS  | 主機名和域名               | 2.6.19   |
| IPC       | CLONE_NEWIPC  | 信號量、消息隊列和共享內存 | 2.6.19   |
| PID       | CLONE_NEWPID  | 進程編號                   | 2.6.24   |
| NetWork   | CLONE_NEWNET  | 網路設備、網路線、端口     | 2.6.29   |
| Mount     | CLONE_NEWNS   | 掛載點（文件系統）         | 2.4.19   |
| User      | CLONE_NEWUSER | 用戶和用戶組               | 3.8      |

### docker 網路通訊

容器與容器之間

容器訪問外部網路

```
iptables -t nat -A POSTROUTING -s 172.17.0.0/16 -o docker0 -j MASQUERADE
```

外部網路訪問容器

```
docker run -d -p 80:80 apache

iptables -t nat -A PREROUTING -m addrtype --dst-type LOCAL -j DOCKER
iptables -t nat -A DOCKER ! -i docker0 -p tcp -m tcp --dport 80 -j DNAT --to-destination 172.17.0.2:80
```

進程網路設定

```
--bridge=" " 指
--bip 指定Docker0的IP和掩碼，使用標準的CIDR形式，如10.10.10.10/24
--dns配置容器的DNS，在啟動Docker進程是添加，所有容器全部生效
```

容器網路修改

```
--dns用於指定啟動的容器的DNS
--net用於指定容器的網路通訊方式，有下列四種值
	> bridge：Docker默認方式，網橋模式
	> none 容器沒有網路棧
	> container 使用其他容器的網路棧，Docker容器會加入其他容器的network namespace
	> host 表示容器使用Host的網路，沒有自己獨立的網路棧，容器可以完全謗問host的網路，不安全
```

-p的使用規則

```
-p :<containerPort> 將指定的容器端口映射至主機昕有地址的一個動態端口
-p <hostPort>:<containerPort> 映射至指定的主機端口
-p <IP>::<containerPort> 映射至指定的主機的IP的動態端口
-p <IP>:<hostPort>:<containerPort> 映射至指定的主機IP的主機端口
-P（大寫） 暴露所需要的所有端口
```

查看容器當前的映射關係

```
docker port ContainerName
```

修改/etc/docker/daemon.json

```
{
	"bip": "192.168.1.5/24",
	"fixed-cidr": "10.20.0.0/16",
	"fixed-cidr-v6": "2001:db8::/64",
	"mtu": "1500",
	"default-gateway": "10.20.1.1",
	"default-gateway-v6": "2001:db8:abcd::89",
	"dns": ["10.20.1.2","10.20.1.3"]
}
```

### 常見隔離方式

`docker network ls` ：列出網路方式

docker network create -d 類型 網路空間名稱

```
docker network create -d bridge lamp
docker network create -d bridge lnmp
```

指定網路名稱空間

```
docker run --name tomcat11 --network=lamp -d tomcat:v1.0
```

### 讓容器之間可以訪問

先創建一個新網橋，讓容器通信

cd /etc/sysconfig/network-scripts/ifcfg-br0

```
DEVICE=br0
TYPE=Bridge
ONBOOT=yes
BOOTPROTO=static
IPADDR=192.168.56.15
NETMASK=255.255.255.0
```

cd /etc/sysconfig/network-scripts/ifcfg-enp0s8

```
TYPE=Ethernet
PROXY_METHOD=none
BROWSER_ONLY=no
BOOTPROTO=static
DEFROUTE=yes
IPV4_FAILURE_FATAL=no
IPV6INIT=yes
IPV6_AUTOCONF=yes
IPV6_DEFROUTE=yes
IPV6_FAILURE_FATAL=no
IPV6_ADDR_GEN_MODE=stable-privacy
NAME=enp0s8
DEVICE=enp0s8
ONBOOT=yes
BRIDGE=br0
```

安裝pipwork

```
yum install -y git
# 下載工具
git clone https://github.com/jpetazzo/pipwork
cp pipwork/pipwork /usr/local/bin/
#授權
chmod a+x /usr/local/bin/pipwork

docker run --name tomcat --net=none -d tomcat:v1.0
pipwork br0 tomcat 192.168.56.17/24
```

### docker數據存儲

#### 數據卷特性

docker鏡像由多個只讀層疊加而成，啟動容器時，docker會加載只讀層並在鏡像棧頂部添加一個讀寫層

如果運行中的容器修改了現有的一個已經存在的文件，那麼該文件將從讀寫層下面的只讀層複制到讀寫層，該文件的只讀版本仍然存在，只是已經被讀寫層中該文件的副本所隱藏，此即："寫時複制"機制

* 關閉並重啟容器，其數據不受影響：但刪除docker容器，則其改變將會全部丟失。
* 存在的問題
  * 存在於聯合文件系統中，不易於宿主機訪問
  * 容器間數據共享不便
  * 刪除容器其數據會丟失
* 解決方案：卷
  * 卷是容器上的一個或多個目錄，此類目錄可繞過聯合文件系統，與宿主機上的某個目錄綁定。
* volume可以在運行容器時即完成創建與綁定操作。當然，前提需要有對應的申明
* volume的初衷就是數據持久化

卷分為兩種：

1. 綁定卷：`/my/bind/volume`--> `/some/specified/directory`
2. docker管理卷：`/managed/volume`-->`/var/lib/docker/vfs/dir/some_volume_ID`

### 容器中的數據卷

使用

```
Docker-managed Volume
>> docker run -it --name roc -v MOUNTDIR roc/lamp:v1.0
>> docker insepct -f {{.Mounts}} roc

Bind-mount Volume
>> docker run -it --name roc -v HOSTDIR:VOLUMEDIR roc/lamp:v1.0

Union Volume
>> docker run -it --name roc --volumes-from ContainerName roc/lamp:v1.0
```

#### 容器自管理數據卷持久化實作

```
vi Dockerfile
FROM centos:8
RUN touch /tmp/abc.txt
RUN mkdir /volume123
ENV name Docker
VOLUME ["/volume123"]
CMD tail -f /tmp/abc.txt


docker build -t testv2 . 
docker run --name mytest -d testv2
docker exec -it mytest /bin/bash

#確認
ls /var/lib/docker/volumes
```

如果想要預設刪除數據卷

```
docker rm -f -v container_name
```

#### 容器綁定卷方法

```
# 將容器/data數據卷加載到宿主機的/data
docker run --name test11 -v /data:/data -d test5
#
```

### 容器間數據共享

法一：

```
#container1
docker run --name test1 -d -v /data:/data test5
#container2
docker run --name test2 -d -v /data:/data test5
```

法二：

```
#container1
docker run --name test1 -d -v /data:/data test5
#container2
docker run --name test2 -d --volumes-from test1 test5
```

必須要有持久化的volume

### 存儲驅動

docker存儲驅動（storage driver)是docker的核心組件，它是docker實現分層鏡像的基礎

1. device mapper（DM）：性能和穩定性存在問題，不推薦生產環境使用
2. btrfs：社區實現法btrfs driver，穩定性和性能存在問題
3. overlayfs：內核3.18overlayfs進入主線，性能和穩定性優異，第一選擇

```
mkdir /var/overlay/{work,low,merged,upper} -p
```

掛載目錄

```
mount -t overlay overlay -olowerdir=./low,upperdir=./upper,workdir=./work ./merged
```

## 資源限制

CGroup 是Control Groups的縮寫，是Linux內核提供的一種可以限制、紀錄、隔離進程組所使用的物力資源的機制。2007年進入linux2.6.24內核，CGroups不是全新創造的，它將進程管理從cpuset中剝離出來，作者是google的Paul Menage

預設情況下，如果不對容器做任何限制，容器能夠占用當前系統能給容器提供的所有資源

docker限制可以從memory、CPU、block I/O 三個方面

OOME：out of memory exception

一旦發生OOME，任何進程都有可能被殺死，包括docker daemon在內

為此，docker調整了docker daemon的OOM優先級，以免被內核關閉

在docker啟動參數中，和內存限制有關的包括(參數的值一般是記憶體大小，也就是一個正數，後面跟著記憶體單位b、k、m、g，分別對應bytes、KB、MB、和GB)

 ### 記憶體資源限制

* -m --memory：容器能使用的最大記憶體大小，最小值為4m
* --memory-swap：容器能夠使用的swap大小
* --memory-swappiness：預設情況下，主機可以把容器使用的匿名頁(anonymous page)swap出來，你可以設置一個0-100之間的值，代表允許swap出來的比例
* --memory-reservation：設置一個記憶體使用的soft limit，設置值小於-m設置
* --kernel-memory：容器能夠使用的kernel memory大小，最小值為4m
* --oom-kill-disable：是否運行OOM的時候殺死容器。只有設置了-m，才可以把這個選項設置為false，否則容器會耗盡主機記憶體，而且導致主機應用被殺死

### CPU資源限制

docker提供的CPU資源限制選項可以在多核系統上限制容器能利用哪些vCPU。而對容器最多能使用的CPU時間有兩種限制方式：

* 一是有多個CPU密集型的容器競爭CPU時，設置各個容器能使用的CPU時間相對比例
* 二是以絕對的方式設置容器在每個調度周期內最多能使用的CPU時間

```
--cpuset-cpus="" ：允許使用的CPU集，值可以為0-3,0,1
-c,--cpu-shares=0 ：CPU共享權值（相對權重），預設值1024
--cpuset-mems=""  ：允許在上執行的記憶體節點（MEMs）
--cpu-period=0    ：即可設置每個周期內容器能使用的CPU時間，容器的CPU配額必須不小於1ms～1s，對應的--cpu-period的數值範圍是1000～1000000
--cpu-quota=0     ：設置在每個周期內容器能使用的CPU時間，容器的CPU配額必須不小於1ms，即--cpu-quota的值必須>=1000單位微秒
--cpus 能夠限制容器可以使用的主機CPU個數，並且還可以指定如1.5之類的小數
```

### 限制實驗

```
docker run --name stress -it --rm -m 256m lorel/docker-stress-ng:latest stress -vm 2
docker run --name stress -it --rm --cpus 2 lorel/docker-stress-ng:latest stress --cpu 8
docker run --name stress -it --rm --cpuset-cpus 0 lorel/docker-stress-ng:latest stress --cpu 8
```

### 開啟遠程連接



```
#
```



#### 容器資源限制測試

* 創建容器

  ```
  docker run -it ubuntu
  
  root@4a3a7d9116bd:/#apt-get update && apt-get install -y stress
  
  #使用256MB記憶體 --vm 1
  stress --vm 1 --verbose
  ```

* 測試搭建

  ```
   cat Dockerfile
  FROM ubuntu
  RUN apt-get update && apt-get install -y stress
  ENTRYPOINT ["/usr/bin/stress"]
  #等待輸入參數
  CMD []
  
  docker build -d jbite9057/stress .
  docker run -it jbite9057/stress --vm 1 --verbose
  ```

docker run --memory 200M jbite9057/stress --vm 1 --verbose

### Docker網路

網路分類

* 單機
  * Bridge Network
  * Host Network
  * None Network
* 多機
  * Overlay Network

namespace

運行兩個容器

```
docker run -d --name test1 busybox /bin/sh -c "while true;do sleep 3600;done"
docker run -d --name test2 busybox /bin/sh -c "while true;do sleep 3600;done"

docker exec -it test1 ip a
    inet 172.17.0.3/16 scope global eth0
       valid_lft forever preferred_lft forever
    inet6 fe80::42:acff:fe11:3/64 scope link
       valid_lft forever preferred_lft forever
docker exec -it test2 ip a
25: eth0@if26: <BROADCAST,MULTICAST,UP,LOWER_UP,M-DOWN> mtu 1500 qdisc noqueue
    link/ether 02:42:ac:11:00:04 brd ff:ff:ff:ff:ff:ff
    inet 172.17.0.4/16 scope global eth0
       valid_lft forever preferred_lft forever
    inet6 fe80::42:acff:fe11:4/64 scope link
       valid_lft forever preferred_lft forever

```

新增namespace

```
ip netns add test1
#查看
ip netns

#ip netns exec test1 ip a
1: lo: <LOOPBACK> mtu 65536 qdisc noop state DOWN group default qlen 1000
#ip netns exec test1 ip link dev lo up 
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN group default qlen 1000
```

增加docker虛擬機的ip link對

```
ip link add veth-test1 type veth peer name veth-test2
#查看
ip link
```

新增ip link至test1

```
ip link set veth-test1 netns test1
#查看
ip netns exec test1 ip link
```

新增ip link至test2

```
ip link set veth-test2 netns test2
#查看
ip netns exec test2 ip link
```

為veth分配IP位址

```
ip netns exec test1 ip addr add 192.168.1.1/24 dev veth-test1
ip netns exec test2 ip addr add 192.168.1.2/24 dev veth-test2

#啟動端口
ip netns exec test1 ip link set dev veth-test1 up
ip netns exec test2 ip link set dev veth-test2 up
```

網路測試

```
ip netns exec test2 ping 192.168.1.1
PING 192.168.1.1 (192.168.1.1) 56(84) bytes of data.
64 bytes from 192.168.1.1: icmp_seq=1 ttl=64 time=0.044 ms
64 bytes from 192.168.1.1: icmp_seq=2 ttl=64 time=0.055 ms
```

##### Docker bridge網路

![image-20200322162248487](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200322162248487.png)

下載工具

```
yum install -y bridge-utils
```

查看本機的veth

![image-20200322162504852](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200322162504852.png)

![image-20200322163302716](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200322163302716.png)

#### 容器通信

希望透過hostname來達成容器間的通信

1. 啟動一個容器

2. 使用link來創建一個有link的容器，就可以做到使用container name來通信

   ```
   docker run -d test4 --link test3 busybox /bin/sh -c "while true;do sleep 3600;done"
   ```

3. 測試

   ```
   docker exec test4 ping test3
   PING test3 (172.17.0.3): 56 data bytes
   64 bytes from 172.17.0.3: seq=0 ttl=64 time=0.052 ms
   ```

#### 端口映射

1. 創建一個web主機

   ```
   docker run --name web -d nginx
   ```

2. 查看網路狀況

   ```
   docker network inspect bridge
   ```

   ![image-20200322170136367](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200322170136367.png)

3. 先停用並刪除web

   ```
   docker stop web
   docker rm web
   ```

4. 創建有端口映射的容器設備

   ```
   docker run --name web -d -p 8080:80 nginx
   ```

5. 測試

   ![image-20200322170750443](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200322170750443.png)

#### 網路的none和host

none網路:

外面無法訪問的容器

1. 創建none網路設備

   ```
   docker run -d --name test7 busybox /bin/sh -c "while true;do sleep 3600;done"
   ```

2. 查看none網路

   ```
   docker network inspect none
    "Containers": {
               "dc5df1c0dfb4fb42411e61651ce9bb4651855129a296123404974122a37f8a2a": {
             "Name": "test7",
             "EndpointID": "15bbc62bb5adb80f21383e442c9d0ed080c74ac0c1c98cfdbeb5bacda2ae7813",
   ```

![image-20200322172230580](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200322172230580.png)

host網路:

創建跟設備相通的設備

1. 創建host網路容器設備

   ```
   docker run -d --name test8 --network host busybox /bin/sh -c "while true;do sleep 3600;done"
   ```

2. 查看host設備

   ```
   # docker network inspect host
   "Containers": {
               "fd704242b8f28935066cb4c915e1c97ea76941fdee0887329909e803c9d5c322": {
                   "Name": "test8",
                   "EndpointID": "4574a510a4a314179be807a84c983279cc490d62773cc7048b409bf9a69e18bb",
   ```

3. 設備內

   ```
   3: enp0s8: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast qlen 1000
       link/ether 08:00:27:d5:7f:d1 brd ff:ff:ff:ff:ff:ff
       inet 192.168.56.33/24 brd 192.168.56.255 scope global enp0s8
          valid_lft forever preferred_lft forever
       inet6 fe80::6a45:5917:ba7f:235d/64 scope link
          valid_lft forever preferred_lft forever
   4: docker0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc noqueue
       link/ether 02:42:67:d3:d7:99 brd ff:ff:ff:ff:ff:ff
       inet 172.17.0.1/16 scope global docker0
          valid_lft forever preferred_lft forever
       inet6 fe80::42:67ff:fed3:d799/64 scope link
          valid_lft forever preferred_lft forever
   ```

### 多容器部屬

應用:flask做web應用 redis做自增

1. 停止並刪除之前的容器

2. 運行redis容器，作為redis服務端

   ```
   docker run -d --name redis redis
   ```

3. 作為客戶端使用，編輯app.py

   ```
   from flask import Flask
   from redis impoort Redis
   import os
   import socket
   
   app = Flask(__name__)
   redis = Redis(host=os.environ.get('REDIS_HOST','127.0.0.1'),port=6379)
   
   @app.route('/')
   
   def hello():
           redis.incr('hits')
           return "Hello docker! Count %s hostname is %s.\n" % (redis.get('hits'),socket.gethostname())
   
   if __name__ == '__main__':
           app.run(host="0.0.0.0",port=5000,debug=True)
   ```

4. 編輯Dockerfile

   ```
   FROM python:2.7
   LABEL maintainer="jbite9057@gmail.com"
   COPY . /app
   WORKDIR /app
   RUN pip install flask redis
   EXPOSE 5000
   CMD ["python", "app.py"]
   ```

5. 建立flask容器

   ```
   docker build -t jbite9057/flask-redis .
   ```

6. 運行容器並設置變量

   ```
   docker run -d --link redis --name flask-redis -e REDIS_HOST=redis jbite9057/flask-redis
   ```

7. 至虛擬機內部測試

   ```
   docker exec -it flask-redis /bin/sh 
   
   # curl 127.0.0.1:5000
   Hello docker! Count 1 hostname is 8bb9b5543194.
   # curl 127.0.0.1:5000
   Hello docker! Count 2 hostname is 8bb9b5543194.
   # curl 127.0.0.1:5000
   Hello docker! Count 3 hostname is 8bb9b5543194.
   # curl 127.0.0.1:5000
   Hello docker! Count 4 hostname is 8bb9b5543194.
   ```

#### 多機多容器通信

使用etcd分布式來儲存IP

![image-20200323145044516](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200323145044516.png)

##### overlay網路

> * 多機的版本要一致

1. 安裝etcd

   ```
   yum install -y etcd
   ```

2. 配置node1

   ```
   ETCD_DATA_DIR="/var/lib/etcd/default.etcd"
   ETCD_LISTEN_PEER_URLS="http://192.168.56.33:2380"
   ETCD_LISTEN_CLIENT_URLS="http://192.168.56.33:2379,http://127.0.0.1:2379"
   ETCD_NAME="docker-node1"
   ETCD_INITIAL_ADVERTISE_PEER_URLS="http://192.168.56.33:2380"
   ETCD_ADVERTISE_CLIENT_URLS="http://192.168.56.33:2379"
   ETCD_INITIAL_CLUSTER="docker-node1=http://192.168.56.33:2380,docker-node2=http://192.168.56.31:2380"
   ETCD_INITIAL_CLUSTER_TOKEN="etcd-cluster"
   ETCD_INITIAL_CLUSTER_STATE="new"
   ```

3. 配置node2

   ```
   ETCD_DATA_DIR="/var/lib/etcd/default.etcd"
   ETCD_LISTEN_PEER_URLS="http://192.168.56.31:2380"
   ETCD_LISTEN_CLIENT_URLS="http://192.168.56.31:2379,http://127.0.0.1:2379"
   ETCD_NAME="docker-node2"
   ETCD_INITIAL_ADVERTISE_PEER_URLS="http://192.168.56.31:2380"
   ETCD_ADVERTISE_CLIENT_URLS="http://192.168.56.31:2379"
   ETCD_INITIAL_CLUSTER="docker-node1=http://192.168.56.33:2380,docker-node2=http://192.168.56.31:2380"
   ETCD_INITIAL_CLUSTER_TOKEN="etcd-cluster"
   ETCD_INITIAL_CLUSTER_STATE="new"
   ```

4. 查詢集群健康狀態

   ```
   etcdctl cluster-health
   member 76ad54f64fd8e326 is healthy: got healthy result from http://192.168.56.33:2379
   member 7d306e3d720b6267 is healthy: got healthy result from http://192.168.56.31:2379
   cluster is healthy
   ```

5. 兩台機器都停用docker服務，並使用etcd啟用

   ```
   systemctl stop docker
   #node1
   /usr/bin/dockerd -H tcp://0.0.0.0:2375 -H unix:///var/run/docker.sock --cluster-store=etcd://192.168.56.33:2379 --cluster-advertise=192.168.56.33:2375
   
   #node2
   /usr/bin/dockerd -H tcp://0.0.0.0:2375 -H unix:///var/run/docker.sock --cluster-store=etcd://192.168.56.31:2379 --cluster-advertise=192.168.56.31:2375
   ```

6. 創建overlay網路

   ```
   docker network create -d overlay demo
   
   #查看
   docker network ls
   NETWORK ID          NAME                DRIVER              SCOPE
   2c1a960de20c        bridge              bridge              local
   3e2381ffce81        demo                overlay             global
   26c75c3c6555        host                host                local
   804bed4cbd59        none                null                local
   
   #node2自動新增一個overlay
   docker network ls
   NETWORK ID          NAME                DRIVER              SCOPE
   d10e2bd5edc3        bridge              bridge              local
   3e2381ffce81        demo                overlay             global
   28c928c91745        host                host                local
   cd215033e76d        none                null                local
   
   ```

7. 查看etcd的存儲

   ```
   etcdctl ls /docker/
   ```

8. 查看overlay網路訊息

   ```
   docker network inspect demo
   ```

9. 運行容器

   ```
   /usr/bin/docker-current: Error response from daemon: shim error: docker-runc not installed on system.
   
   /usr/bin/docker-current: Error response from daemon: shim error: docker-runc not installed on system.
   ```

10. 解決執行錯誤

    ```
    cd /usr/libexec/docker/
    cp docker-runc-current /usr/bin/docker-runc
    ```

11. 錯誤解決

    ```
    docker rm test10
    docker run -d --name test10 --net demo busybox /bin/sh -c "while true;do sleep 3600;done"
    2f2188c7e4f6787d23a43b25aaf722308f65742a6a827215217ed6d3180c8326
    ```

12. node2新增一個容器

    ```
    docker run -d --name test10 --net demo busybox /bin/sh -c "while true;do sleep 3600;done"
    /usr/bin/docker-current: Error response from daemon: service endpoint with name test10 already exists.
    
    docker run -d --name test11 --net demo busybox /bin/sh -c "while true;do sleep 3600;done"
    success
    ```

#### docker的數據持久化方案

1. 停止並刪除之前的容器

   ```
   docker stop $(docker container ls -qa)
   docker rm $(docker container ls -qa)
   ```

2. 綁定掛載的volume 真實去存儲數據 可以指定volume位置(數據持久化)

3. Volume:

   ```
   docker run -d --name mysql1 -e MYSQL_ALLOW_EMPTY_PASSWORD=true mysql
   ```

4. 查看volume

   ```
   docker volume ls
   DRIVER              VOLUME NAME
   local               286a3c9f2b044912f7724b3cdc58ac042491f33c57eaab16cfd2ff80bc230856
   local               4f4d77dbed365f1ce0acac26f02d5f0eaa94ce427a2d9a6f3ee01b9cbea82e7e
   ```

   ![image-20200323110921728](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200323110921728.png)

5. 存儲持久化成功

   ![image-20200323111022301](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200323111022301.png)

6. 刪除無使用的volume

   ```
   docker volume prune
   ```

7. 自定義創建volume容器，將容器中/var/lib/mysql下的數據持久化

   ```
   docker run -d -v mysql:/var/lib/mysql --name mysql1 -e MYSQL_ALLOW_EMPTY_PASSWORD=true mysql
   ```

8. 進入容器，新增一個資料庫

   ```
   docker exec -it mysql1 /bin/sh
   
   #進入Mysql 沒有密碼
   mysql -uroot
   create database xdl1801
   mysql> show databases;
   +--------------------+
   | Database           |
   +--------------------+
   | information_schema |
   | mysql              |
   | performance_schema |
   | sys                |
   | xdl1801            |
   +--------------------+
   ```

9. 刪除mysql1

   ```
   docker stop mysql1
   docker rm mysql1
   ```

10. 創建一個新的mysql進行數據回復

    ```
    docker run -d --name mysql2 -v mysql:/var/lib/mysql -e MYSQL_ALLOW_EMPTY_PASSWORD=true mysql
    ```

11. 查看

    ```
    docker run -d --name mysql2 -v mysql:/var/lib/mysql -e MYSQL_ALLOW_EMPTY_PASSWORD=true mysql
    
    mysql -uroot
    show databases;
    +--------------------+
    | Database           |
    +--------------------+
    | information_schema |
    | mysql              |
    | performance_schema |
    | sys                |
    | xdl1801            |
    +--------------------+
    ```

bind mount 實現數據持久化

> 指定一個與容器同步的目錄，容器或者目錄變化，都會隨著變動

1. 創建並進入一個目錄

   ```
   mkdir ~/nginx && cd ~/nginx
   ```

2. 隨意編輯一個文件

   ```
   cat index.html
   <h1>Hello Docker</h1>
   ```

3. 新增Dockerfile

   ```
   FROM nginx:latest
   WORKDIR /usr/share/nginx/html
   COPY index.html index.html
   ```

4. 構建鏡像

   ```
   docker build -t nginx .
   ```

5. 解決無法啟用nginx問題

   ```
   cd /usr/libexec/docker
   cp docker-proxy-current /usr/bin/docker-proxy
   ```

6. 

7. 開啟並實現共享目錄

   ```
   docker run -d -v /root/nginx/:/usr/share/nginx/html --name web -p 8080:80 nginx2
   ```

8. 驗證

   ![image-20200323140446318](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200323140446318.png)

#### docker-compose 部屬wordpress

1. 將dockerHub上的mysql 5.5和wordpress拉下來確認只有mysql5.5版的鏡像

   ```
   docker pull mysql:5.5
   docker pull wordpress
   ```

2. 將5.5版標記為latest

   ```
   docker tag mysql:5.5 mysql:latest
   ```

3. 運行wordpress透過8080端口映射

   ```
   docker run -d -e WORDPRESS_DB_HOST=mysql:3306 --link mysql -p 8080:80 wordpress
   ```

#### 什麼是docker-compose?

多容器下 app會很難部屬和管理，docker-compose是一種命令行工具，用來處理批量工作

docker-compose.yml

重要概念:

* services
  
  * 相當於container
* Networks
  
  * 相當於使用的網路
* Volumes
  
* 數據持久化
  
* 安裝docker-compose

  ```
  cd ~
  
  curl -L "https://github.com/docker/compose/releases/download/1.25.4/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose
  
  chmod +x /usr/local/bin/docker-compose
  
  #驗證安裝 看到版本代表安裝完成
  docker-compose --version
  docker-compose version 1.25.4, build 1110ad01
  ```

* 編輯docker-compose.yml

  ```
  version: '3'
  
  services:
    wordpress:
      image: wordpress
      ports:
        - 8080:80
      environment:
        WORDPRESS_DB_HOST: mysql
        WORDPRESS_DB_PASSWORD: admin
      networks:
        - my-bridge
    mysql:
      image: mysql:5.5
      environment:
        MYSQL_ROOT_PASSWORD: admin
        MYSQL_DATABASE: wordpress
      volumes:
        - mysql-data:/var/lib/mysql
      networks:
        - my-bridge
  
  volumes:
    mysql-data:
  networks:
    my-bridge:
      driver: bridge
  
  ```

* 啟用docker-compose

* ```
  docker-compose up
  #指定配置檔啟動
  docker-compose -f docker-compose.yml up
  
  #加-d變成後台運行
  ```

* 停止並刪除

  ```
  docker-compose down
  ```

* 命令行執行

  ```
  docker-compoae exec mysql bash
  ```

另一種部屬方式

* 編輯docker-compose.yml

  ```
  version: '3'
  
  services:
    redis:
      image: redis
    web:
      build:
        context: .
        dockerfile: Dockerfile
      ports:
        - 8080:5000
      environment:
        REDIS_HOST: redis
  
  ```

* 啟動

  ```
  docker-compose up -d
  ```

### 容器擴展和負載均衡

#### 容器擴展

可以用scale擴展容器

* 啟用之前的容器

  ```
  docker-compose up -d
  ```

* 擴展

  * 停用之前的

  ```
  docker-compose down
  
  ```

  * 修改docker-compose.yml

    ```
    version: '3'
    
    services:
      redis:
        image: redis
      web:
        build:
          context: .
          dockerfile: Dockerfile
        environment:
          REDIS_HOST: redis
    ```

  * 啟用

    ```
    docker-compose up --scale web=3 -d
    ```

  * 擴展成功

    ![image-20200324085235797](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200324085235797.png)

  #### 負載均衡

  * 修改app.py

    ![image-20200324085633270](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200324085633270.png)

  * 修改docker-compose.yml

    ```
    version: '3'
    
    services:
      redis:
        image: redis
      web:
        build:
          context: .
          dockerfile: Dockerfile
        environment:
          REDIS_HOST: redis
      ld:
        image: dockercloud/haproxy
        links:
          - web
        ports:
          - 9999:80
        volumes:
          - /var/run/docker.sock:/var/run/docker.sock
    ```

  * 啟用

    ```
    docker-compose up --scale web=3 -d
    ```

  * 訪問測試，說明haproxy預設使用倫尋算法，主機會變動

    ```
    [root@docker flask-redis]# curl 127.0.0.1:9999
    Hello docker! Count 1 hostname is 9560097dd090.
    [root@docker flask-redis]# curl 127.0.0.1:9999
    Hello docker! Count 2 hostname is 0452e2cc4286.
    [root@docker flask-redis]# curl 127.0.0.1:9999
    Hello docker! Count 3 hostname is 1ea86a063faf.
    [root@docker flask-redis]# curl 127.0.0.1:9999
    Hello docker! Count 4 hostname is 9560097dd090.
    [root@docker flask-redis]# curl 127.0.0.1:9999
    Hello docker! Count 5 hostname is 0452e2cc4286.
    [root@docker flask-redis]# curl 127.0.0.1:9999
    Hello docker! Count 6 hostname is 1ea86a063faf.
    [root@docker flask-redis]# curl 127.0.0.1:9999
    Hello docker! Count 7 hostname is 9560097dd090.
    [root@docker flask-redis]# curl 127.0.0.1:9999
    Hello docker! Count 8 hostname is 0452e2cc4286.
    [root@docker flask-redis]# curl 127.0.0.1:9999
    Hello docker! Count 9 hostname is 1ea86a063faf.
    
    ```

  * 減少容器實例

    ```
    docker-compose up --scale web=1 -d
    ```

### 複雜應用

docker-compose.yml

```
version: '3'

services:
  voting-app:
    build: ./voting-app/.
    volumes:
     - ./voting-app:/app
    ports:
      - "5000:80"
    links:
      - redis
    networks:
      - front-tier
      - back-tier

  result-app:
    build: ./result-app/.
    volumes:
      - ./result-app:/app
    ports:
      - "5001:80"
    links:
      - db
    networks:
      - front-tier
      - back-tier

  worker:
    build: ./worker
    links:
      - db
      - redis
    networks:
      - back-tier

  redis:
    image: redis
    ports: ["6379"]
    networks:
      - back-tier

  db:
    image: postgres:9.4
    volumes:
      - "db-data:/var/lib/postgresql/data"
    networks:
      - back-tier

volumes:
  db-data:

networks:
  front-tier:
  back-tier:
```

![image-20200324094142407](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200324094142407.png)

### docker swarm

#### 介紹

docker生產環境使用

![image-20200324095140328](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200324095140328.png)

至少兩個manager，使用一個分布式存儲Raft來溝通，實現數據同步

manager做決策 擴展的容器部屬到哪個機器

![image-20200324101601700](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200324101601700.png)

#### 搭建swarm集群

用三台機器，用一個新的docker機器做三個節點

* 安裝docker-compose
* 設置/etc/hosts，將三台主機的主機名加入



1. 在manager節點設定初始化

   ```
   docker swarm init --advertise-addr 192.168.56.31
   Swarm initialized: current node (sbp1bki8woskw4uo70bydhbbz) is now a manager.
   
   To add a worker to this swarm, run the following command:
   
   docker swarm join --token SWMTKN-1-5htoi8i9renn0udu9c380q5smdsdmm7d9s8eij7c81caq1vepg-6ncmocijxdl2z0fanjc7o9pm7 192.168.56.31:2377
   
   To add a manager to this swarm, run 'docker swarm join-token manager' and follow the instructions.
   ```

2. 在子節點輸入初始化時產生的token

   ```
   docker swarm join \
       --token SWMTKN-1-5htoi8i9renn0udu9c380q5smdsdmm7d9s8eij7c81caq1vepg-6ncmocijxdl2z0fanjc7o9pm7 \
       192.168.56.31:2377
   ```

3. 操作

   ```
   #查看節點狀態
   docker node ls
   
   #節點脫離集群
   docker swarm leave 
   ```

4. 集群中管理容器

   ```
   docker service
   docker service create --name demo busybox sh -c "while true;don sleep 3600;done"
   
   #集群容器擴展
    docker service scale demo=5
   ```

### 使用dockerstack部屬voting app

docker-compose  單機模式部屬多個服務

docker-swarm是集群中隊單個服務的部屬

docker-stack是集群中對多個服務的部屬

* 集群部屬docker- stack

  ```
  docker stack deploy example --compose-file=docker-compose.yml
  ```

#### docker stack 佈屬可視化應用

```
docker stack deploy -c docker-compose.yml stack-demo
```

```
docker service create \
--name portainer \
--publish 9000:9000 \
--constraint 'node.role == manager' \
--mount type=bind,source=/var/run/docker.sock,target=/var/run/docker.sock \
--mount type=bind,source=/path/on/host/data,target=/data \
portainer/portainer \
-H unix:///var/run/docker.sock
```

### docker secret

* 創建密碼

  ```
  docker secret create my-pw password
  
  echo "admin" | docker secret create my-pw2 -
  ```

* 創建容器，使用自己的密碼

  ```
  docker service create --name db --secret my-pw -e MYSQL_ROOT_PASSWORD_FILE=/run/secrets/my-pw mysql
  ```

* 


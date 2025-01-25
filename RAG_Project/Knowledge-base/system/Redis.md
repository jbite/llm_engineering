## Redis

### 介紹

基於鍵值對的NOSQL資料庫

* 特性 
  * 速度快
  * 使用C語言實現
* 多種數據結構
  * 字符串
  * 哈希
  * 列表
  * 集合
  * 有序集合

### 應用場景

* 鍵過期 緩存 session會話保存 優惠券過期
* 列表 : 排行榜
* 天然計數器 帖子瀏覽數 視頻播放數 評論留言數
* 集合 興趣 廣告投放
* 消息隊列: ELK

### 安裝部屬

* 目錄規劃
  * redis下載目錄
    * /data/soft
  * redis安裝目錄
    * /opt/redis_cluster/redis_(PORT)/{conf, logs, pid}
  * 數據目錄
    * /data/redis_cluster/redis_(PORT)/redis__(PORT).rdb
  * 運維腳本
    * /root/scripts/redis_shell.sh

```
mkdir -p /data/soft
mkdir -p /data/redis_cluster/redis_6379
mkdir -p /opt/redis_cluster/redis_6379/{conf,logs,pid}

cd /data/soft
wget http://download.redis.io/releases/redis-3.2.9.tar.gz
tar zxf redis-3.2.9.tar.gz -C /opt/redis_cluster/

ln -s /opt/redis_cluster/redis-3.2.9/ /opt/redis_cluster/redis

cd /opt/redis_cluster/redis
make && make install
```

準備動作

```
mkdir -p /data/redis_cluster/redis_26379
mkdir -p /opt/redis_cluster/redis_26379/{conf,pid,logs}
```



編輯/opt/redis_cluster/redis_6379/conf/redis_6379.conf

```
daemonize yes

bind 192.168.56.35

port 6379

pidfile /opt/redis_cluster/redis_6379/pid/redis_6379.pid
logfile /opt/redis_cluster/redis_6379/logs/redis_6379.log
## 設置數據庫的數量
databases 16
## 指定本地持久化文件的文件名 預設視dump.rdb
dbfilename redis_6379.rdb
## 本地數據庫的目錄
dir /data/redis_cluster/redis_6379
```

啟用

```
redis-server /opt/redis_cluster/redis_6379/conf/redis_6379.conf

redis-cli -h 192.168.56.31
```

### 全局命令

[redis命令參考](https://redisdoc.com)



#### 數據類型

全局操作

```
#判斷key是否存在
EXISTS key_name

#刪除
DEL key_name

#查看key的類型
type key_name

#超時時間
TTL k2
-1 永不過期
-2 key不存在
正值 存活秒數

#設置超時
EXPIRE key_name seconds

#設置永久
PERSIST key_name

#批量插入多個
MSET k1 v1 k2 v2 k3 v3 k4 v4

#批量獲取
MGET k1 k2 k3 k4
```

字串類型

```
#設定key及值
set key_name vlaue

#獲取Key的值
get key_name

#增加值
INCR key_name
INCRBY key_name value
```

列表類型

```
#從右方插入數據
RPUSH list1 value1
LPUSH list1 1 2 3 4 5 6 7

#從左方插入數據
LPUSH list1 value2

#查看列表長度
LLEN list1

#查看列表元素
LRANGE list1 0 -1

#刪除數據
RPOP list1
LPOP list1
```

哈希類型

```
#插入哈希類型數據
hmset user:1000 username jackyfeng age 27 job it

#查詢數據
hmget user:1000 username age job

#獲取值得所有數據
HGETALL user:1000
```

集合類型

```
#創建集合，集合不允許出現重複元素
SADD set1 1 2 3 4 5

#查看集合元素
SMEMBERS set1

#差集
SDIFF set1 set2

#交集
SINTER set1 set2

#聯集
SUNION set1 set2
```

有序集合

```
zadd zset 10 nimei 9 womei 8 tamei
zrange zset 0 2
```



### 持久化

RDB

```
BGSAVE
```

配置/opt/redis_cluster/redis_6379/conf/redis_6379.conf

```
save 900 1
save 300 100
save 60 10000
```

優點 安全

缺點 佔用空間

加入持久化以後  shutdown redis會自動執行`bgsave`

AOF

命令寫入持久化

```
vi /
#是否打開aof日誌功能
appendonly yes
#是否每一個命令 都立即同步
appendfsync always
#每秒寫一次
appendfsync everysec
#寫入工作交給操作系統 由操作系統判斷緩川區大小 統一寫入到aof
appendsync no
#創建的aof名稱
appendfilename "appendonly.aof"
```

如果AOF及RDB同時存在，redis會優先使用AOF的數據

### 哨兵(Sentinel)

規劃:

角色      IP                         端口

Master 192.168.56.31   6379

sent-01                             26379



哨兵配置:

```
daemonize yes

bind 192.168.56.31

port 26379

pidfile /opt/redis_cluster/redis_6379/pid/redis_26379.pid
logfile /opt/redis_cluster/redis_6379/logs/redis_26379.log

dbfilename redis_26379.rdb

dir /data/redis_cluster/redis_26379

sentinel monitor mymaster 192.168.56.31 6379 2
sentinel down-after-milliseconds mymaster 3000
sentinel parallel-syncs mymaster 1
sentinel failover-timeout mymaster 18000
```

同步配置

```
rsync -avz /opt/* db02:/opt/
rsync -avz /opt/* db03:/opt/
```

修改db02

```
sed -i "s#bind 192.168.56.31#bind 192.168.56.32#g" /opt/redis_cluster/redis_6379/conf/redis_6379.conf

sed -i "s#bind 192.168.56.31#bind 192.168.56.32#g" /opt/redis_cluster/redis_26379/conf/redis_26379.conf
```

修改db03

```
sed -i "s#bind 192.168.56.31#bind 192.168.56.33#g" /opt/redis_cluster/redis_6379/conf/redis_6379.conf

sed -i "s#bind 192.168.56.31#bind 192.168.56.33#g" /opt/redis_cluster/redis_26379/conf/redis_26379.conf
```

vi /opt/redis_cluster/redis_6379/conf/redis_6379.conf

```
slaveof 192.168.56.31 6379
```

先確認所有redis服務都關閉

```
pkill redis
#開啟redis服務db01 db02 db03
redis-server /opt/redis_cluster/redis_6379/conf/redis_6379.conf
#開啟哨兵服務db01 db02 db03
redis-sentinel /opt/redis_cluster/redis_26379/conf/redis_26379.conf
```

sentinel的API

![image-20200326170337069](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200326170337069.png)

透過哨兵得知現在master是誰

```
redis-cli -h db02 -p 26379
192.168.56.32:26379> Sentinel get-master-addr-by-name mymaster
```

手動切換master

```
#改變priority
redis-cli -h db02 -p 6379

#要求重新選舉
redis-cli -h db02 -p 26379
sentinel failover mymaster
```

### Redis授權認證

禁止protected-mode  預設為開啟 禁止非本地訪問

```
protected-mode yes/no
```

設定密碼

```
requirepass password
redis-cli -h
AUTH password
```

### Redis集群

只有V3.0以上才有

![image-20200329120621841](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200329120621841.png)

集群計算hash值來決定要分配資料到哪個位置 

* 透過slot觀念來分配 總共會有16384個槽位 不管有幾台伺服器 
* 透過槽位比例來決定分配比重

* 槽的順序無所謂

#### 運維腳本

```
cat >redis_shell.sh << EOF
#!/bin/bash

USAG(){
    echo "sh $0 {start|stop|restart|login|ps|tail} PORT"
}
if [ "$#" = 1 ]
then
    REDIS_PORT='6379'
elif 
    [ "$#" = 2 -a -z "$(echo "$2"|sed 's#[0-9]##g')" ]
then
    REDIS_PORT="$2"
else
    USAG
    exit 0
fi

REDIS_IP=$(hostname -I|awk '{print $1}')
PATH_DIR=/opt/redis_cluster/redis_${REDIS_PORT}/
PATH_CONF=/opt/redis_cluster/redis_${REDIS_PORT}/conf/redis_${REDIS_PORT}.conf
PATH_LOG=/opt/redis_cluster/redis_${REDIS_PORT}/logs/redis_${REDIS_PORT}.log

CMD_START(){
    redis-server ${PATH_CONF}
}

CMD_SHUTDOWN(){
    redis-cli -c -h ${REDIS_IP} -p ${REDIS_PORT} shutdown
}

CMD_LOGIN(){
    redis-cli -c -h ${REDIS_IP} -p ${REDIS_PORT}
}

CMD_PS(){
    ps -ef|grep redis
}

CMD_TAIL(){
    tail -f ${PATH_LOG}
}

case $1 in
    start)
        CMD_START
        CMD_PS
        ;;
    stop)
        CMD_SHUTDOWN
        CMD_PS
        ;;
    restart)
        CMD_START
        CMD_SHUTDOWN
        CMD_PS
        ;;
    login)
        CMD_LOGIN
        ;;
    ps)
        CMD_PS
        ;;
    tail)
        CMD_TAIL
        ;;
    *)
        USAG
esac
EOF
```



#### 安裝

db01

```
mkdir -p /opt/redis_cluster/redis_{6380,6381}/{conf,logs,pid}
mkdir -p /data/redis_cluster/redis_{6380,6381}

cat > /opt/redis_cluster/redis_6380/conf/redis_6380.conf<< EOF
bind 192.168.56.31
port 6380
daemonize yes
pidfile "/opt/redis_cluster/redis_6380/pid/redis_6380.pid"
logfile "/opt/redis_cluster/redis_6380/logs/redis_6380.log"
dbfilename "redis_6380.rdb"
dir "/data/redis_cluster/redis_6380/"
cluster-enabled yes
cluster-config-file nodes_6380.conf
cluster-node-timeout 15000
EOF


cd /opt/redis_cluster/
cp redis_6380/conf/redis_6380.conf redis_6381/conf/redis_6381.conf
sed -i 's#6380#6381#g' redis_6381/conf/redis_6381.conf
rsync -avz /opt/redis_cluster/redis_638* db02:/opt/redis_cluster/
rsync -avz /opt/redis_cluster/redis_638* db03:/opt/redis_cluster/

redis-server /opt/redis_cluster/redis_6380/conf/redis_6380.conf
redis-server /opt/redis_cluster/redis_6381/conf/redis_6381.conf
```

db02 db03

```
mkdir -p /data/redis_cluster/redis_{6380,6381}
redis-server /opt/redis_cluster/redis_6380/conf/redis_6380.conf
redis-server /opt/redis_cluster/redis_6381/conf/redis_6381.conf
```

命令

```
#查看集群內部節點
CLUSTER NODES
```

#### 使用工具部屬集群

先清空原本集群狀態 並且重新啟動redis服務

```
pkill redis
rm -rf /data/redis_cluster/redis_6380/*
rm -rf /data/redis_cluster/redis_6381/*

redis-server /opt/redis_cluster/redis_6380/conf/redis_6380.conf
redis-server /opt/redis_cluster/redis_6381/conf/redis_6381.conf
```

開始部屬

```
cd /opt/redis_cluster/redis/src

./redis-trib.rb create --replicas 1 192.168.56.31:6380 192.168.56.32:6380 192.168.56.33:6380 192.168.56.31:6381 192.168.56.32:6381 192.168.56.33:6381

Can I set the above configuration? (type 'yes' to accept): yes
```

查看

```
./redis-trib.rb check 192.168.56.31:6380
./redis-trib.rb rebalance
```

#### 集群擴容和收縮

只使用redis-trib.rb工具來操作

Lab: 先建立新的節點

1. 在db01上建立兩個新節點

   ```
   mkdir -p /opt/redis_cluster/redis_{6390,6391}/{conf,logs,pid}
   mkdir -p /data/redis_cluster/redis_{6390,6391}
   
   cd /opt/redis_cluster/
   cp redis_6380/conf/redis_6380.conf redis_6390/conf/redis_6390.conf
   cp redis_6380/conf/redis_6380.conf redis_6391/conf/redis_6391.conf
   
   sed -i 's#6380#6390#g' redis_6390/conf/redis_6390.conf
   sed -i 's#6380#6391#g' redis_6391/conf/redis_6391.conf
   ```

2. 啟動新節點

   ```
   redis-server /opt/redis_cluster/redis_6390/conf/redis_6390.conf
   redis-server /opt/redis_cluster/redis_6391/conf/redis_6391.conf
   ```

3. 添加節點

   ```
   cd /opt/redis_cluster/redis/src
   ./redis-trib.rb add-node 192.168.56.31:6390 192.168.56.31:6380
   ./redis-trib.rb add-node 192.168.56.31:6391 192.168.56.31:6380
   ```

4. 重新分配槽位

   ```
   ./redis-trib.rb reshard 192.168.56.31:6380
   ```

收縮

不能隨便刪 需要身上的槽位為0

1. 將節點的槽位分配出去

   ```
   cd /opt/redis_cluster/redis/src
   ./redis-trib.rb reshard 192.168.56.31:6380
   How many slots do you want to move (from 1 to 16384)?1365
   What is the receiving node ID? 接收方ID
   接收方
   yes
   ```

2. 上一步需要做三次 幾個節點做幾次

3. 刪除節點 需要附上ID當作參數

   ```
   ./redis-trib.rb del-node 192.168.56.31:6390 e2a6a2885afb33a6c42144911ddd3ad3c1c10601
   ```

### 集群故障轉移

##### redis cluster ask 路由

![image-20200329101449652](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200329101449652.png)

結合redis ask插入數據

```
redis-cli -h db01 -p 6380 -c
db01:6380> set oldboy wahaha
-> Redirected to slot [6766] located at 192.168.56.32:6380
OK
192.168.56.32:6380>
```

兩個現象:

會協助在主節點插入數據

並返回操作介面轉移至主節點

讓你知道數據插入在哪台主機

#### 數據導入導出工具

切換到redis集群時 會面臨數據導入的問題 推薦使用redis-migrate-tool來導入數據

安裝:

```
yum install libtool bzip2 -y
cd /opt/redis_cluster/
git clone https://github.com/vipshop/redis-migrate-tool.git
cd redis-migrate-tool/
autoreconf -fvi
./configure
make && make install 
```

創建配置文件

```
cat > redis_6379_to_6380.conf <<EOF
[source]
type: single
servers:
- 192.168.56.31:6379

[target]
type: redis cluster
servers:
- 192.168.56.31:6380

[common]
listen: 0.0.0.0:8888
source_safe: true
EOF
```

生成測試數據

```
for i in $(seq 1 1000)
do
    redis-cli -c -h db01 -p 6379 set jbite_k_${i} jbite_v_${i} && echo "set jbite_k_${i} is ok"
done
```

導入數據

```
redis-migrate-tool -c /opt/redis_cluster/redis-migrate-tool/redis_6379_to_6380.conf
```

需要確認redis集群服務有起來

#### 分析鍵值大小

需求背景
redis的内存使用太大键值太多,不知道哪些键值占用的容量比较大,而且在線分析會影響性能

安裝

```
yum install -y epel-release
yum makecache
yum install python-pip gcc python-devel -y
pip install rdbtools python-lzf
cd /opt/redis_cluster
git clone https://github.com/sripathikrishnan/redis-rdb-tools
cd redis-rdb-tools
python setup.py install
```

分析鍵值大小

```
cd /data/redis_cluster/redis_6380/
rdb -c memory redis_6380.rdb -f redis_6380.rdb.csv

awk -F ',' '{print $4,$2,$3,$1}' redis_6380.rdb.csv |sort  > 6380.txt
```

#### 監控過期鍵

需求背景
因為開發重複提交，導致電商網站優惠卷過期時間失效
問題分析
如果一個鍵已經設置了過期時間，這時候在set這個鍵，過期時間就會取消
解決思路
如何在不影響機器性能的前提下批量獲取需要監控鍵過期時間
1.Keys * 查出來匹配的鍵名。然後循環讀取ttl時間
2.scan * 範圍查詢鍵名。然後循環讀取ttl時間
Keys 重操作，會影響服務器性能，除非是不提供服務的從節點
Scan 負擔小，但是需要去多次才能取完，需要寫腳本
腳本內容

```
cat 01get_key.sh 
#!/bin/bash
key_num=0
> key_name.log
for line in $(cat key_list.txt)
do
    while true
    do
        scan_num=$(redis-cli -h 192.168.47.75 -p 6380 SCAN ${key_num} match ${line}\* count 1000|awk 'NR==1{print $0}')
        key_name=$(redis-cli -h 192.168.47.75 -p 6380 SCAN ${key_num} match ${line}\* count 1000|awk 'NR>1{print $0}')
        echo ${key_name}|xargs -n 1 >> key_name.log
        ((key_num=scan_num))
        if [ ${key_num} == 0 ]
           then
           break
        fi
    done
done
```

### 故障案例

為了防止記憶體超載 需要限制記憶體使用量

* 記憶體超載
  * 惰性刪除
  * 隨即刪除
  * 不做任何事

配置文件新增

```
maxmemory <bytes>
```

```
vm.overcommit_memory = 1
```


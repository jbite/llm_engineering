# Nginx

一個高性能的HTTP和反向代理服務器 也是一個IMAP/POP3/SMTP伺服器

優勢:

* 高併發
* IO多路復用
* epoll
* 異步
* 非阻塞
* IO多路復用

### 部屬

#### 安裝

```
# pcre依賴
yum install -y pcre-devel pcre
# openssl依賴
yum install -y openssl-devel openssl
# gcc依賴
yum install -y gcc-c++

wget http://www.nginx.org
useradd nginx -s /sbin/nologin -M
cd nginx/
./configure --user=nginx --group=nginx --prefix=/application/nginx --with-http_stub_status_module --with-http_ssl_module

./configure \
--prefix=/usr/local/nginx \
--pid-path=/var/run/nginx/nginx.pid \
--lock-path=/var/lock/nginx.lock \
--error-log-path=/var/log/nginx/error.log \
--http-log-path=/var/log/nginx/access.log \
--with-http_gzip_static_module \
--http-client-body-temp-path=/var/temp/nginx/client \
--http-proxy-temp-path=/var/temp/nginx/proxy \
--http-fastcgi-temp-path=/var/temp/nginx/fastcgi \
--http-uwsgi-temp-path=/var/temp/nginx/uwsgi \
--http-scgi-temp-path=/var/temp/nginx/scgi    
make && make install
```
注意：上边将临时文件目录指定为/var/temp/nginx，需要在/var下创建temp及nginx目录



#### 配置文件

```
/etc/logrotate.d/nginx
/etc/nginx
/etc/nginx/conf.d
/etc/nginx/conf.d/default.conf
/etc/nginx/fastcgi_params
/etc/nginx/koi-utf
/etc/nginx/koi-win
/etc/nginx/mime.types
/etc/nginx/modules
/etc/nginx/nginx.conf
/etc/nginx/scgi_params
/etc/nginx/uwsgi_params
/etc/nginx/win-utf
/etc/sysconfig/nginx
/etc/sysconfig/nginx-debug
/usr/lib/systemd/system/nginx-debug.service
/usr/lib/systemd/system/nginx.service
/usr/lib64/nginx
/usr/lib64/nginx/modules
/usr/libexec/initscripts/legacy-actions/nginx
/usr/libexec/initscripts/legacy-actions/nginx/check-reload
/usr/libexec/initscripts/legacy-actions/nginx/upgrade
/usr/sbin/nginx
/usr/sbin/nginx-debug
/usr/share/doc/nginx-1.17.9
/usr/share/doc/nginx-1.17.9/COPYRIGHT
/usr/share/man/man8/nginx.8.gz
/usr/share/nginx
/usr/share/nginx/html
/usr/share/nginx/html/50x.html
/usr/share/nginx/html/index.html
/var/cache/nginx
/var/log/nginx
```

### 編譯參數

nginx -V

```
configure arguments:
--prefix=/etc/nginx
--sbin-path=/usr/sbin/nginx # 命令路徑
--modules-path=/usr/lib64/nginx/modules
--conf-path=/etc/nginx/nginx.conf
--error-log-path=/var/log/nginx/error.log
--http-log-path=/var/log/nginx/access.log
--pid-path=/var/run/nginx.pid
--lock-path=/var/run/nginx.lock
--http-client-body-temp-path=/var/cache/nginx/client_temp
--http-proxy-temp-path=/var/cache/nginx/proxy_temp
--http-fastcgi-temp-path=/var/cache/nginx/fastcgi_temp
--http-uwsgi-temp-path=/var/cache/nginx/uwsgi_temp
--http-scgi-temp-path=/var/cache/nginx/scgi_temp
--user=nginx
--group=nginx
--with-compat
--with-file-aio
--with-threads
--with-http_addition_module
--with-http_auth_request_module
--with-http_dav_module
--with-http_flv_module
--with-http_gunzip_module
--with-http_gzip_static_module
--with-http_mp4_module
--with-http_random_index_module
--with-http_realip_module
--with-http_secure_link_module
--with-http_slice_module
--with-http_ssl_module
--with-http_stub_status_module
--with-http_sub_module
--with-http_v2_module
--with-mail
--with-mail_ssl_module
--with-stream
--with-stream_realip_module
--with-stream_ssl_module
--with-stream_ssl_preread_module
--with-cc-opt='-O2 -g -pipe -Wall -Wp,-D_FORTIFY_SOURCE=2 -fexceptions -fstack-protector-strong --param=ssp-buffer-size=4 -grecord-gcc-switches -m64 -mtune=generic -fPIC'
--with-ld-opt='-Wl,-z,relro -Wl,-z,now -pie'
```

### 配置文件

#### 主配置文件

```
vi /etc/nginx/nginx.conf
user  nginx;
worker_processes  1;
#錯誤日誌
error_log  /var/log/nginx/error.log warn;
pid        /var/run/nginx.pid;

#事件模塊
events {
    worker_connections  1024;
}

#http核心模塊
http {
    include       /etc/nginx/mime.types;
    # 字節流處理類型
    default_type  application/octet-stream;

    log_format  main  '$remote_addr - $remote_user [$time_local] "$request" '
                      '$status $body_bytes_sent "$http_referer" '
                      '"$http_user_agent" "$http_x_forwarded_for"';

    access_log  /var/log/nginx/access.log  main;

    sendfile        on;
    #tcp_nopush     on;

    keepalive_timeout  65;

    #gzip  on;

    include /etc/nginx/conf.d/*.conf;
}

```

#### 虛擬主機配置文件

/etc/nginx/conf.d/default.conf

```
server {
    #監聽端口
    listen       80;
    #伺服器名稱 FQDN
    server_name  localhost;

    #網頁字符類型
    #charset koi8-r;
    #訪問日誌
    #access_log  /var/log/nginx/host.access.log  main;

    #網頁定位
    location / {
        root   /usr/share/nginx/html;
        index  index.html index.htm;
    }

    #error_page  404              /404.html;

    # redirect server error pages to the static page /50x.html
    #
    error_page   500 502 503 504  /50x.html;
    location = /50x.html {
        root   /usr/share/nginx/html;
    }

    # proxy the PHP scripts to Apache listening on 127.0.0.1:80
    #
    #location ~ \.php$ {
    #    proxy_pass   http://127.0.0.1;
    #}

    # pass the PHP scripts to FastCGI server listening on 127.0.0.1:9000
    #
    #location ~ \.php$ {
    #    root           html;
    #    fastcgi_pass   127.0.0.1:9000;
    #    fastcgi_index  index.php;
    #    fastcgi_param  SCRIPT_FILENAME  /scripts$fastcgi_script_name;
    #    include        fastcgi_params;
    #}

    # deny access to .htaccess files, if Apache's document root
    # concurs with nginx's one
    #
    #location ~ /\.ht {
    #    deny  all;
    #}
}
```

##### 多網卡多ip

 ```
 #/etc/nginx/conf.d/ip.conf

 server {
 	listen 10.0.0.1:80;
 	server_name _;

 	location / {
 		root /web/eth0;
 		index index.html index.htm;
 	}
 }

 server {
 	listen 10.0.0.2:80;
 	server_name _;

 	location / {
 		root /web/eth1;
 		index index.html index.htm;
 	}
 }
 ```

新增另一台虛擬主機

/etc/nginx/conf.d/nlab.conf

```
server {
 listen 80;
 server_name nlab.com;

 location / {
   root /web/nlab;
   index index.html index.htm;
 }
}
```

#### 軟重啟

```
nginx -s reload
```

### Nginx 日誌log

#### 日誌配置

* 日誌模塊: ngx_http_log_module

* 相關指令

  * log_format: 定義日志格式語法

  * access_log

  * error_log: 由主配置文件中定義

    ```
    log_format main '$remote_addr - $remote_user [$time_local] "$request" '
    '$status $body_bytes_sent "$http_referer" '
    '"$http_user_agent" "$http_x_forwarded_for"';
    ```

    * $remote_addr : 遠程(客戶)主機的位址
    * $remote_user: 遠程用戶
    * $time_local: 本地時間
    * $request: 可能會有特殊符號 所以外面會增加""
    * $status:  200 404 503 100 301
    * $body_bytes_sent: 傳送字節大小
    * $http_referer: url來源參考
    * $http_user_agent: 瀏覽器相關訊息
    * $http_x_forwarded_for:

  * open_log_file_cache

* 設定不要看得訪問日誌，在主配置文件(/etc/nginx/nginx.conf)中加入

  ```
  location = /favicon.ico {
   log_not_found off;
   access_log off;
  }
  ```

#### 錯誤頁面

```
server{
  error_page 404 /404.html;
  location = /404.html {
     root   /web/nlab;
  }

}
```

#### 錯誤日誌

```
nginx/logs/error.log
```

#### 日誌緩存

大量訪問到來時，對於每一條日誌紀錄，都是先打開文件 再寫入日誌

一般不開 記憶體比較貴 硬碟便宜

open_log_file_cache on;

#### 日誌輪轉/切割

##### 前言

* 預設啟用日誌輪轉
* rpm -ql nginx | grep log

##### /etc/logrotate.conf

```
cat /etc/logrotate.d/nginx
/var/log/nginx/*.log {
        daily
        missingok
        rotate 52
        compress
        delaycompress
        notifempty
        create 640 nginx adm
        sharedscripts
        postrotate
                if [ -f /var/run/nginx.pid ]; then
                        kill -USR1 `cat /var/run/nginx.pid`
                fi
        endscript
}
```



```
cat /etc/logrotate.conf
# see "man logrotate" for details
# rotate log files weekly
weekly

# keep 4 weeks worth of backlogs
rotate 4

# create new (empty) log files after rotating old ones
create

# use date as a suffix of the rotated file
dateext

# uncomment this if you want your log files compressed
#compress

# RPM packages drop log rotation information into this directory
include /etc/logrotate.d

# no packages own wtmp and btmp -- we'll rotate them here
/var/log/wtmp {
    monthly
    create 0664 root utmp
        minsize 1M
    rotate 1
}

/var/log/btmp {
    missingok
    monthly
    create 0600 root utmp
    rotate 1
}

# system-specific logs may be also be configured here.
```



```
/usr/sbin/logrotate -s /var/lib/logrotate/logrotate.status /etc/logrotate.conf
```

#### 日誌分析

* 統計2017年9月5日 PV量(page view 網頁點擊量) UV 24小時內的獨特訪客

  * ````
    grep '05/Sep/2017' cd.modiletrain.org.log | wc -l
    ````

  * 八點到九點

  * ```
    grep '05/Sep/2017:08' cd.modiletrain.org.log | wc -l
    awk '$4>="[05/Sep/2017:08:00:00]"&&$4<="[05/Sep/2017:09:00:00]"'
    ```

* 統計一天內訪問最多的十個IP

  * ```
    grep '05/Sep/2017' file | awk '{ips[$1]++}END{for(i in ips){print i,ips }}' | sort -k2 -rn | head -3
    ```

* 訪問最多的十個頁面

* 統計每個URL訪問內容總大小

* ```
  grep '05/Sep/2017' file | awk '{urls[$7]++;size[$7]+=$10}END{for(i in url){print urls[i],size[i],i}}'
  ```



### Nginx WEB模塊

查詢模塊安裝

```
nginx -V
```

#### 連接狀態

* keepalived 長鏈接狀態

  ```
  server{
    keepalive_timeout 65;
  }
  ```

*

#### 隨機主頁

random_index_module: 將主頁設置成隨機頁面 是一種微調更新機制

```
vi /etc/nginx/conf.d/default.conf
location / {
        root   /usr/share/nginx/html;
        index  index.html index.htm;
        ##新增此行
        random_index on;
    }

```

#### 替換模塊

sub_module

快速更換字串的方式

```
server {
  sub_filter nginx 'Apache';
  #啟用之後 只會替換第一個看到的
  sub_filter_once on;
}
```

#### 文件讀取

ngx_http_core_module

* 語法一
  * Syntax: sendfile on | off;
  * Default: sendile on;
  * Context: http, server, location, if in location
  * 發送文件: 使用此功能加速網路傳輸速度，會跳過用戶緩衝，內河直接轉向socket
* 語法二
  * Syntax: tcp_nopush on | off;
  * Default: tcp_nopush off;
  * Context: http, server, location, location
  * 將封包累積到一定大小才發送
* 語法三
  * Syntax: tcp_nodelay on | off;
  * Default: tcp_nodelay off;
  * Context: http, server, location, location
  * tcp ack不延遲發送 避免重放的請求

#### 文件壓縮

ngx_http_gzip_module

gzip on | off;

gzip_comp_level level;

gzip_comp_level1;

gzip_http_version 1.0 | 1.1;

````
http {
 gzip on;
 gzip_http_version 1.1;
 gzip_comp_level 2;
 gzip_types text/plain application/javascript

}
````

#### 頁面緩存

ngx_hrrp_headers_module

expires 起到控制頁面緩存的作用

cache-control

#### 防盜鏈

ngx_http_referer_module

```
syntax valid_referers none blocked *.a.com;
if ($invalid_referer){
  return 403;
}
```

設置白名單

```
syntax valid_referers none blocked *.a.com server_name 192.168.100.* ~tianyun ~\.google\. ~\.baidu\. b.com;
if ($invalid_referer){
  return 403;
}
```

#### 連接狀態

stub_status_module

### Nginx訪問限制

安裝壓力測試工具

yum install httpd-tool -y

ab -n 100 -c http://mlab.com

```
Server Software:        nginx/1.17.9
Server Hostname:        mlab.com
Server Port:            80

Document Path:          /
Document Length:        71 bytes

Concurrency Level:      100
Time taken for tests:   0.640 seconds
Complete requests:      10000
Failed requests:        0
Write errors:           0
Total transferred:      3020000 bytes
HTML transferred:       710000 bytes
Requests per second:    15633.94 [#/sec] (mean)
Time per request:       6.396 [ms] (mean)
Time per request:       0.064 [ms] (mean, across all concurrent requests)
Transfer rate:          4610.79 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0    1   0.4      1       4
Processing:     2    5   1.8      5      21
Waiting:        0    4   1.8      4      21
Total:          2    6   1.8      6      22

Percentage of the requests served within a certain time (ms)
  50%      6
  66%      6
  75%      6
  80%      6
  90%      7
  95%      9
  98%     14
  99%     17
 100%     22 (longest request)
```

#### ngx_http_limit_req_module

#### ngx_http_limit_conn_module

啟動限制

```
#定義
#限制請求 二進制地址 限制策略的名稱 占用10M空間 允許每秒1次請求
limit_req_zone $binary_remote_addr zone=req_zone:10m rate=1r/s;
#調用
limit_req zone=req_zone;
```

配置

```
vi /etc/nginx/nginx.conf
http {
  limit_req_zone $binary_remote_addr zone=req_zone:10m rate=1r/s;
}
vi /etc/nginx/conf.d/mlab.conf
server {
  limit_req zone=req_zone;
}
```

測試

```
Benchmarking mlab.com (be patient).....done


Server Software:        nginx/1.17.9
Server Hostname:        mlab.com
Server Port:            80

Document Path:          /
Document Length:        71 bytes

Concurrency Level:      10
Time taken for tests:   0.005 seconds
Complete requests:      100
Failed requests:        99
   (Connect: 0, Receive: 0, Length: 99, Exceptions: 0)
Write errors:           0
Non-2xx responses:      99
Total transferred:      36833 bytes
HTML transferred:       19574 bytes
Requests per second:    19069.41 [#/sec] (mean)
Time per request:       0.524 [ms] (mean)
Time per request:       0.052 [ms] (mean, across all concurrent requests)
Transfer rate:          6859.22 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0    0   0.0      0       0
Processing:     0    0   0.0      0       0
Waiting:        0    0   0.0      0       0
Total:          0    0   0.0      0       1

Percentage of the requests served within a certain time (ms)
  50%      0
  66%      1
  75%      1
  80%      1
  90%      1
  95%      1
  98%      1
  99%      1
 100%      1 (longest request)

```

#### ngx_http_access_module

訪問限制

* allow by host
  * syntax: allow address | CIDR | unix: |all;
* deny

IP限制

```
server {
  allow 10.18.45.65;
  allow 10.18.45.11;
  allow all;
}
```

username&password限制

```
syntax: auth_basic_user_file file;
context: http, server, location
```

創建帳號密碼文件:

```
htpasswd -c /etc/nginx/conf.d/passwd user10
```

設定檔加入模組

```
server {
 auth_basic "nginx access test";
 auth_basic_user_file /etc/nginx/conf.d/passwd;
}
```

### http協議詳解

超文傳輸協議

## Nginx常用模塊

* nginx目錄索引：ngx_http_authoindex_module

```
server {
	listen 80;
	server_name game.oldboy.com;
	charset utf-8,gbk;

	location / {
		root /code;

		# index index.html index.htm;
		#開啟模組
		autoindex on;
	}
}
```

​				指定是否在目錄中輸出確切的文件大小，on顯示字節，off顯示大概單位

```
Syntax: autoindex_exact_size on|off;
Default: autoindex_exact_size on;
Context: http, server, location
```

​				指定目錄列表中，輸出的時間是以本地時區還是UTC輸出

```
autoindex_locationtime on;
```

* nginx狀態監控：應使用--wtih-http_stub_status_module配置參數啟用

```
ngx_http_stub_status_module
location = /basie_status {
	stub_status;
}
```

* nginx訪問控制

  基於來源ip做限制

  ```
  ngx_http_access_module
  # 允許配置語法
  syntax: allow address | CIDR | unix: | all;
  Default: -
  Context: http, server, location, limit_except

  #拒絕配置語法
  syntax: deny address | CIDR | unix: | all;
  Default: -
  Context: http, server, location, limit_except

  server {
  	allow 10.0.0.1/32;
  	deny 10.0.0.0/24;
  	allow all;
  }
  ```

* nginx資源限制：允許使用http基本身份驗證，驗證用戶名和密碼來限制對資源的訪問。

  ```
  ngx_http_auth_basic_module
  #使用http基本身份驗證協議啟用用戶名和密碼驗證
  Syntax: auth_basic string| off;
  Default: auth_basic off;
  Context: http, server, location, limit_except
  #指定保存用戶名和密碼的文件
  Syntax: auth_basic_user_file file;
  Default: -
  Context: http, server, location, limit_except
  ```

  - 生成一個密碼文件，密碼文件的格鏑name:password(加密)  (建議使用htpasswd) openssl password

  - `yum install http-tools`

  - `htpasswd -c -b /etc/nginxauth_conf oldboy 123`

  - ```
    location /download{
    	root /module;
    	autoindex on;
    	auth_baic "Please password";
    	auth_basic_user_file /etc/nginxauth_conf;
    }
    ```

* nginx訪問限制

* nginx location

## Nginx WEB架構實戰篇

### 動態網站架構

資源

* index.php開源的php windows/linux + nginx + php + mysql
* index.py開源的php windows/linux + apache + py + mysql
* index.jsp商業java windows/linux + tomcat + JDK + oracle
* index.asp商業c# windows+ iis + asp.net + sql-server/oracle/mongodb

## LNMP 動態網站環境部屬

* Linux

* Nginx: yum install -y nginx

* php-fpm: rpm部屬:

  * ```
    yum install epel-release yum-utils -y
    yum install http://rpms.remirepo.net/enterprise/remi-release-7.rpm -y
    yum-config-manager --enable remi-php73 -y
    yum install -y php-fpm php-mysql php-gd
    yum install php php-common php-opcache php-mcrypt php-cli php-gd php-curl php-mysqlnd -y
    systemctl restart php-fpm
    systemctl enable php-fpm
    ```

  * vim /usr/share/nginx/html/index.php

    * ```

      <?php
      phpinfo();
      ?>

      ```

  * vim /etc/nginx/conf.d/default.conf

    * ```
      location ~ \.php$ {
              root           /usr/local/html;
              fastcgi_pass   127.0.0.1:9000;
              fastcgi_index  index.php;
              fastcgi_param  SCRIPT_FILENAME  $document_root$fastcgi_script_name;
              include        fastcgi_params;
          }
      ```

  * nginx -s reload

  * fpm: fastCGI process manager fast common gateway interface

* mysql

  * ```
    yum install -y mariadb-server mariadb
    systemctl start mariadb
    systemctl enable mariadb
    mysqladmin password '123456'
    ```

  * 創建資料庫

  * ```
    mysql -uroot -p
    123456
    create database discuz;
    grant all on discuz.* to phptest@'192.168.56.%' identified by '123456';
    ```

  * 測試j文件

  * ```
    vim /usr/share/nginx/html/index.php
    <?php
    $link = new mysqli('192.168.30.31','root','123456','discuz');
    if ($link) {
      echo "linked";
    }
    else {
     echo "fail";
    }
    $link -> close();
    ?>
    ```

  *

* 業務上線

### Nginx location

用來定位頁面

有些頁面需要特殊設置

* 語法規則:
  * location [= | ~ | ~* | !~ | !~* |  ^~] { }

* location 優先級:
  * =   ：精準匹配
  * ^~ ：以某個字符串開頭
  * ~   ：區分大小寫的正則匹配  
  * ~* ：不區分大小寫的正則匹配  
  * !~  ：區分大小寫不匹配的正則
  * !~* ：不區分不匹配的正則
  * /     ：通用匹配，任何請求都會匹配到
  * 精確匹配>字符開頭>正規匹配>通配

### Nginx Rewrite

重寫用戶的url

使用場景: 不同公司合併時，可以直接將被合併的域名轉成新域名

使用方式: 當用戶訪問192.168.100.10/abc/a/1.html會重定向至192.168.100.10/ccc/bbb/2.html

語法:

```
location /abc {
	rewrite .* /ccc/bbb/2.html;
}

#網址會也改變 推薦使用 減少伺服器負擔
location /abc {
	rewrite .* /ccc/bbb/2.html permanent;
}
```

表達式問題:

```
location /2016 {
	rewrite /2016/(.*)$ /2017/$1 permanent;
}
```

將訪問quianfeng.com的流量導到jd.com

```
if ($host ~* qianfeng.com) {
	rewrite .* http://jd.com permanent;
}
```

訪問www.qianfeng.com/ccc/bbb/1.html將訪問cloud.com/ccc/bbb/1.html

```
if ($host ~* qianfeng.com) {
	rewrite .* http://cloud.com/$request_uri permanent;
}
```



```
server{
      if( -d $request_filename ){
         rewrite ^(.*)([^/])$ http://$1$2/;
      }
    }

```



```
server{
  listen 80;
  root /web/cloud;
  server_name cloud.com;
  location /qf {
     rewrite ^/qf/([0-9]+)-([0-9]+)-([0-9]+)(.*)$ /qf/$1/$2/$3/$4 permanent;

  }
}
```



```
if ($host ~* (.*)\.cloud\.com) {
    set $user $1;
    rewrite .*  http://cloud.com/$user.html permanent;
  }
```

### CA 及HTTPS

* 私有CA

  * 生成證書及密鑰文件

    * 準備存放證書和密鑰的目錄

      ```
      mkdir -p /etc/nginx/ssl
      ```

    * 使用openssl生成基於rsa數學算法長度為1024的密鑰，文件必須以key為結尾

      ```
      openssl genrsa 1024 > /etc/nginx/ssl/server.key
      ```

    * 使用密鑰文件生成證書-申請書

      ```
      openssl req -new -key /etc/nginx/ssl/server.key > /etc/nginx/ssl/server.csr
      Country Name (2 letter code) [XX]:taiwan
      string is too long, it needs to be less than  2 bytes long
      Country Name (2 letter code) [XX]:TW
      State or Province Name (full name) []:Taiwan
      Locality Name (eg, city) [Default City]:kaohsiung
      Organization Name (eg, company) [Default Company Ltd]:powergate
      Organizational Unit Name (eg, section) []:IT
      Common Name (eg, your name or your server's hostname) []:jacky
      Email Address []:jacky@powergate.ph

      Please enter the following 'extra' attributes
      to be sent with your certificate request
      A challenge password []:
      An optional company name []:
      ```

    * 同意證書申請書

      ```
      openssl req -x509 -days 365 -key /etc/nginx/ssl/server.key -in /etc/nginx/ssl/server.csr > /etc/nginx/ssl/server.crt
      ```

      * -x509 : 證書格式
      * days: 證書有效期
      * key: 指定密鑰文件
      * in: 指定證書申請文件

  * 私有CA的https部屬

    * 創建目錄

    ```
    server {
     server_name www.bj.com;
     root /web/bj;
     listen 443 ssl;
     ssl_certificate /etc/nginx/ssl/server.crt;
     ssl_certificate_key /etc/nginx/ssl/server.key;

     location / {
       index index.html 1.html;
     }
    }
    ```

    *

* 公網CA

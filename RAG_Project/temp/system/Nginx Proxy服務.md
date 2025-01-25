# Nginx Proxy服務

## 代理原理

* 正向代理
  * 內網用戶透過代理伺服器請求上網
* 反向代理
  * 外網用戶訪問內網網站伺服器的代理

## 代理模組

ngx_http_proxy_module

* syntax: proxy_pass URL
* context: location, if in location, limit_expcept

緩衝區

* syntax: proxy_buffer on | off;
* default: proxy_buffer on;

* context: http, server, location

* syntax: proxy_buffer_size



## Lab

兩台伺服器

* nginx1: 實際網站
* nginx2: 反向代理

設定代理:

* ```
   location / {
          proxy_pass http://mlab.com:80;
          proxy_redirect default;
    
          proxy_set_header Host $http_host;
          proxy_set_header X-Real-IP $remote_addr;
          proxy_set_header X-Forwarded-for $proxy_add_x_forwarded_for;
    
          proxy_connect_timeout 30;
          proxy_send_timeout 60;
          proxy_read_timeout 60;
    
          proxy_buffering on;
          proxy_buffer_size 32k;
          proxy_buffers 4 128k;
          proxy_busy_buffers_size 256k;
          proxy_max_temp_file_size 256k;
  }
  ```

### Proxy緩存

* 緩存類型:
  * 網頁緩存: CDN
    * 廠商:
    * 收費標準:
    * 收費方式:
    * 
  * 數據庫緩存: memcache redis
  * 網頁緩存: nginx-proxy
  * 客戶端緩存: 瀏覽器緩存

設置緩存

* ```
  http {
   proxy_cache_path /app/tianyun.me/cache levels=1:2 keys_zone=proxy_mlab:10m
   max_size=10g inactive=60m use_temp_path=off;
  
  location / {
  	proxy_cache proxy_mlab;
  	proxy_cache_valid 200 304 12h;
  	proxy_cache_valid any 10m;
  	proxy_cache_key $host$uri$is_args$args;
  }
  }
  ```

* levels: 定義了緩存的層次結構 推薦使用二層結構

* keys_zone: 設置一個共享記憶體區 該記憶體區用於存儲緩存鍵和元數據 有些類似計時器的用途 可以使nginx在不檢索硬碟的情況下快速決定一個請求是HIT還是MISS 這樣可以提高檢索速度 一個1MB的記憶體空間大約可以存儲大約8000個key

*  max_size: 定義緩存最大佔據的空間 如果到達上限 就會調用cache manager來移除最近最少被使用的文件 以清出可用空間

* inactive: 指定在不被訪問的情況下 能夠被記憶體保存的時間

![image-20200420111625245](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200420111625245.png)

### Nginx 七層負載均衡

```
location / {
   
}
location - \.html${
proxy_pass ...
}
location - \.php${
proxy_passs ...
}
location -\.(jpg|png|css|js)${
proxy_pass ...
}
```

proxy_pass 後面跟著upstream 的群組原則，選擇伺服器做分流

```
upstream html{
  server web1:80;
  server web2:80;
}
upstream php{
  server web3:80;
  server web4:80;
}
server{
location / {
   
}
location - \.html${
proxy_pass http://html;
}
location - \.php${
proxy_passs http://php;
}
location -\.(jpg|png|css|js)${
proxy_pass http://html
}
}
```

IP_hash

```
upstream backend{

IP_hash;

server 192.168.56.37:80

server 192.168.56.38:80

}
```

weight

```
upstream backend{
 server 192.168.56.37 weight=10;
 server 192.168.56.38 weight=5;
}
```

url_hash 對url做hash運算

```

```

fair 根據後臺響應時間來分發請求 響應時間短的 分發的請求多




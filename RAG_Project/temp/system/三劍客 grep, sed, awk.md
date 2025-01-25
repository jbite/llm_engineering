# 三劍客 grep, sed, awk

## 正規表示法

* 語法 使用中括號刮起來

  ```
  [[ ^[0-9]+$ ]]
  ```

* 示例:

  ```
  [root@es-node1 ~]# cat shell31.sh
  #!/bin/bash
  
  read -p "輸入數字才退出:" num
  
  if [[ $num =~ ^[0-9]+$ ]]
  then
      echo "correct!"
    else
      echo "Wrong input"
  
  fi
  ```

* 特殊符號

  * `^` : 匹配行首
  * `$` : 匹配行尾
  * `.` : 匹配任意單個字符 
  * `*` : 匹配0到多個
  * `.*` : 匹配任意n個字符
  * `[]` : 匹配指定範圍內的一個字符
    * `[ - ]` : 連續範圍
    * `[ ^ ]` : 匹配不包含在內的符號
  * `\<` : 詞首定位符，定位一個字的首位符
  * `\>` : 詞尾定位符 
  * `()` : 被包起來的字符串使用\1\2...來表示
  * `x\{m\}` : x出現m次
  * `x\{m,\}` : x出現m次以上
  * `x\{m,n\}` : x出現m次到n次

* 擴展

  * `+` : 1次以上
  * `?` : 匹配0~1個前導字符
  * a|b

* 轉義符: " ", ' ', \

## grep

* -q 安靜模式
* -v 反向查詢
* -R 查詢目錄下所有文件
* -o 只顯示查到的東西
* -B2 顯示前兩行
* -A2顯示後兩行
* -C2 顯示上下兩行
* -l 只要文件名
* -n 帶行號

特殊字符

* `\w` : 所有字符與數字
* `\W` : 所有非字符與數字的符號
* `\b` : 邊界

#### egrep

支援正則

#### fgrep

沒有正則

## sed

流編輯器

格式

```
sed 選項 命令 文件
sed 選項 -f 腳本 文件
```

不管對錯 返回值都是0，除非指令輸入錯誤

#### sed指令

-r : 支持正規表示法

```
sed -r 'command' file
```

##### 刪除 d

```
#刪除第5行
sed -r '5d' passwd
#刪除末行
sed -r '$d' passwd
#刪除有root的行
sed -r '/root/d' passwd
#刪除m到n行
sed -r 'm,nd' passwd
```

##### 替換 s

```
#全局替換root為gggg
sed -r 's/root/gggg/g' passwd
#
sed -r 's#(mail)#E\1#g' passwd
```

##### 讀取 r

```
cat 88.txt
/123/456

#只要有數字的行，即將他讀取88.txt
sed -r '/[0-9]/r 88.txt' passwd 
root:x:0:0:root:/root:/bin/bash
/123/456
bin:x:1:1:bin:/bin:/sbin/nologin
/123/456
daemon:x:2:2:daemon:/sbin:/sbin/nologin
/123/456
adm:x:3:4:adm:/var/adm:/sbin/nologin
/123/456

```

##### 寫入w

```
sed -r 'w'
```

##### 追加a

```
#每行追加
sed -r 'a123' passwd
#單行增加
sed -r '10a123' passwd
#追加多航
sed -r 'a 123\
456\
789' passwd
```

##### 插入i

##### 整行替換 c

```
sed -r '2c aaaaa' passwd

sed -r '2c aaaa\
bbbb\
ccccc'
passwd
```

##### 下一行 n

```
#刪除包含root的下一行 非匹配到的那行
sed -r '/root/{n;d}' passwd
```

##### 取反 !

```
sed -r '2,$!d' passwd
```

##### 多重編輯

```
sed -r -e '2d' -e '3,$d' passwd

sed -r '2d;3,$d' passwd
```

##### hHGgx

* h : 覆蓋暫存空間
* H : 追加暫存空間
* g : 覆蓋行
* G : 追加行
* x : 互換

![image-20200410233240094](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200410233240094.png)

sed -r '1!G;$!h;$!d' 12345.txt
5
4
3
2
1

## awk

> 是一種編程語言 用於在linux/unix下對文本處理
>
> 逐行掃描文件 從第一行到最後一行 尋找特定的模式的行 把匹配的行 處理過後輸出到螢幕

使用

* ```
  awk -F : '{print $1;$3}' /etc/passwd
  awk option 'commands' filename
  ```

* $0 該行

* $1 第一個分割 $2 第二個分割 依此類推

* command : 
  * BEGIN{} : 行處理之前執行
  *  {} : 行處理之中
  *  END{} : 行處理之後執行

#### awk 內部變量

* FS : 定義輸入字段分隔符

  ```
  awk 'BEGIN{FS=":"}{print $1}' /etc/sdpasswd
  #通常使用
  awk -F ":" '{print $0}' /etc/passwd
  ```

* OFS : 定義輸出字段分隔符，輸出時的分隔符號

  ```
  awk 'BEGIN{OFS="()"}{print $1,$2,$3,$4}' /etc/passwd
  ```

* RS : 輸入記錄分隔符 默認換行符 (行)。一個$0就是一個紀錄

  ```
  cat a.txt
  111 222 333 444 555:666:777
  #換行符改為" "
  awk 'BEGIN{RS=" "}{print $0}' a.txt
  111
  222
  333
  444
  555:666:777
  #換行符改為":"
  awk 'BEGIN{RS=":"}{print $0}' a.txt
  111 222 333 444 555
  666
  777
  ```

* ORS : 輸出紀錄符

  ```
  awk 'BEGIN{RS=" "; ORS=":D"}{print $0}' a.txt
  111:D222:D333:D444:D555:666:777
  ```

* FNR : 多文件獨立編號

  ```
  awk '{print FNR,$0}' aaa.txt bbb.txt
  1 11111
  2 22222
  1 33333
  ```

* NR

  ```
  
  ```

* NF : 字段總數

#### 模式(正則表示)和動作

字符串比較

```
awk '$0 ~ /^root/' /etc/passwd
awk -F: '$1 ~ /^root/' /etc/passwd
```

數值比較

```
<
<=
==
!=
>=
>

#找出誰的uid為0
awk -F: '$3==0' /etc/passwd

#進行運算
awk -F: '$3*10>500' /etc/passwd

#判斷 && and, || or
awk -F: '$1 ~/root/ && $3 <= 15' /etc/passwd
awk -F: '$1 ~/root/ || $3 <= 15' /etc/passwd
```

範圍匹配

```
awk -F: '/adm:/,/lpd/' /etc/passwd
adm:x:3:4:adm:/var/adm:/sbin/nologin
lp:x:4:7:lp:/var/spool/lpd:/sbin/nologin
```

#### awk腳本編程

##### 變量

變量調用

* 自定義變量 -v 

  ```
  awk -v uuuser=root -F: '$1 == uuuser' /etc/passwd
  => root:x:0:0:root:/root:/bin/bash
  
  #調用系統環境變數，需要使用引號
  #但也需要注意""的轉義
  var="baaash"
  echo "unix script" | awk "{print \"$var\"}"
  ```

##### 條件判斷

* if : {if(表達式){語句;語句;....}}

  * 需求: 如果$3為0，就說他是管理員

  * ```
    awk -F: '{if($3==0){print $1 " is admin"}}' /etc/passwd
    ```

* else : {if(表達式){語句;語句;....} else{語句;語句;....}}

  * ```
    awk -F: '{if($3==0){print $1}else{print $7}}' /etc/passwd
    root
    /sbin/nologin
    /sbin/nologin
    /sbin/nologin
    /sbin/nologin
    /bin/sync
    ```

  * 統計管理員和系統用戶數量

  * ```
    awk -F: '{if($3==0){count++}else{i++}} END{print "管理員個數: " count;print "系統用戶數: " i}' /etc/passwd
    管理員個數: 1
    系統用戶數: 43
    ```

* 多分支 : {if(){}else if(){}else{}}

  * 需求: 顯示出三種用戶的信息

  * ```
    awk -F: '{if($3==0){print $1 " is admin"}else if($3>999){ print $1,"is user"}else{print $1,"is software user"}}' /etc/passwd
    root is admin
    bin is software user
    daemon is software user
    adm is software user
    lp is software user
    sync is software user
    shutdown is software user
    halt is software user
    mail is software user
    operator is software user
    games is software user
    ftp is software user
    nobody is software user
    systemd-network is software user
    dbus is software user
    polkitd is software user
    tss is software user
    abrt is software user
    sshd is software user
    postfix is software user
    oldboy is user
    elasticsearch is software user
    nginx is software user
    redis is software user
    logstash is software user
    apache is software user
    jacky is user
    dex is user
    bob is user
    jane is user
    jee is user
    ggggg is user
    siji is user
    yuji is user
    daji is user
    zhenji is user
    zhaozilong is user
    sunwulkong is user
    wuzetian is user
    xiaoqiao is user
    xuance is user
    malixian is user
    baillala is user
    ntp is software user
    ```

  * 統計數量:

    ```
    awk -F: '{if($3==0){admin++}else if($3>999){ user++ }else{buildin++}}END{print "admin #:"admin,"buildin #:" buildin,"user #:" user}' /etc/passwd
    
    admin #:1 buildin #:25 user #:18
    ```

##### 循環

* while循環

  * ```
    {while(條件句){語句;語句;}}
    ```

* for

  * ```
    {for(i=0;i<=5;i++){print i}}
    ```

##### 數組

* for(i in var) 中 : i 會是索引值

* ```
  #將帳戶寫入數組並且使用循環遍歷來顯示內容
  awk -F: '{username[++i]=$1;}END{for(i in username){print username[i]}}' /etc/passwd
  root
  bin
  daemon
  adm
  lp
  sync
  shutdown
  halt
  mail
  ```

* 統計shell數

  ```
  awk -F: '{shells[$NF]++} END{for(i in shells){print i,"共有",shells[i]}}' /etc/passwd
  /bin/sync 共有 1
  /bin/bash 共有 19
  /sbin/nologin 共有 22
  /sbin/halt 共有 1
  /sbin/shutdown 共有 1
  ```

* 統計日誌中的訪問前十

  ```
  #透過這個日誌文件來統計訪問來源
  #ls /var/log/httpd/access_log
  cat /var/log/httpd/access_log | awk '{ips[$1]++} END{for(i in ips){print i,ips[i]}}'| sort -k2 -rn | head -3
  ```

* 

##### awk編程案例


### IPSec VPN

解決廣域網存在的各種安全隱患

* 被竊聽(私密性)---加密
* 被竄改(完整性)---驗證
* 被冒充(源認證)---認證
* 不可否認性


#### IPSec框架

* 散列驗證算法:MD5 SHA1
* 加密算法:DES,3DES,AES
* 封裝協議:AH,ESP
* 封裝模式傳輸模式,隧道模式
* 密鑰有效期:60s-86400s

## HASH算法

* 驗證數據完整性(防止數據算改)
* 常用MD5, SHA1

##### 散列函數特點

* hash值長度固定
  * MD5:128bits
  * SHA1:160bits
* 雪崩效應:修改其中任一字符，得到的hash值會全部變化
* 不可逆:無法透過hash值反向得到原始數據
* 唯一性:不存在hash值相同的兩個不同文件
* 只能保證數據沒有被竄改，無法對數據進行認證，不知道用戶是不是合法的

#### HMAC (Keyed-hash Message Authentication Code)

* 密鑰化散列訊息認證碼



* 在訊息中加入密碼校驗，來印證來源合法性

## 加密算法

* 對稱算法
  * 加解密雙方使用相同的密鑰與算法進行加解密
  * 主流
    * DES,3DES,AES,RC4
  * 優點
    * 速度快
    * 安全
    * 緊湊
  * 缺點
    * 明文傳輸共享密鑰，容易出現中途劫持案竊聽的問題
    * 隨著參與者數量的增加，密鑰數量急遽膨脹
    * 因為密鑰數量過多，對密鑰的管理和存儲是一個很大的問題
    * 不支持數字簽名和不可否認性
* 非對稱算法
  * 一個密鑰加密的訊息，必須使用另一個密鑰來解密
  * 公鑰加密斯鑰解密，私鑰加密公鑰解密
  * 對數據進行數位簽章
  * 主流算法
    * RSA,DH,ECC
  * 數位簽章


* 優點
  * 運算數度慢，不可能使用非對稱算法加密實際數據
  * 運算過後的數據會變得很長 用RSA加密1GB的數據加密後可能變成2GB

#### IPSec解決方案

* 通過非對稱加密算法加密對稱加密算法的密鑰
* 然後再用對稱加密算法加密實際鑰傳輸的數據 
* 封裝協議
  * ESP
    * Encapsulation Security Payload
    * IP protocol : 50
    * 提供加密 完整性校驗和源認證三大方面的保護 也能抵禦重放攻擊
    * 只保護IP payload 不對原始IP頭部進行安全防護
  * AH
    * Authentication Header
    * IP protocol : 51
    * 只提供完整性校驗和源認證兩方面的安全服務 也可以抵禦重放攻擊
    * 不支持數據加密

![image-20200222133559487](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200222133559487.png)

### 封裝協議

#### ESP

* 安全參數索引(SPI)
  * 一個32 bit 的字段 用來標示處理數據包的安全關聯 (SA)
  * 序列號(SN) :用來防止重放攻擊
  * 初始化向量(IV) : 使得加密更安全
  * 負載數據
  * 墊片(0-255 bytes):用來補齊不足的數據
  * 墊片長度
  * 下一個頭部
  * 驗證數據
    * ESP會對從ESP頭部到ESP尾部的所有數據進行hash計算 得到的hash值就會被會放到驗證數據部分

![image-20200222134050653](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200222134050653.png)

#### AH

* 對數據驗證的範圍更廣，不僅包含原始數據 還包含了原始IP頭部 AH認證頭部的名稱就由此得名

![image-20200222134716803](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200222134716803.png)

<div style='color:red;text-align:center'>
    致命問題:其對IP頭部進行認證 導致數據包不允許經過NAT
</div>

## 封裝模式

#### 傳輸模式

* 保留原始IP頭部
* 無法直接在互聯網上傳輸
* 效率較高
* GRE + IPSec 才會使用傳輸模式

#### 隧道模式

* 保護原始IP頭部
* 重寫IP，可直接在互聯網上傳輸
* 效率較低

![Encapsulating Security Payload Modes](http://www.ipv6now.com.au/pics/IPsecESPmodes.png)

## 密鑰有效期

* 思科設備每小時就要更新一次密鑰 可以根據實際的情況對密鑰有效期進行調整
* 加密數據越多 有效期就應該越短
* PFS(perfect forward secrecy)
  * 要求每一次密鑰更新 都需要重新產生全新的密鑰 和以前使用的密鑰不存在任何衍生關係
  * 默認不啟用
  * crypto map WORD 10 set pfs groups

### IKE

* 互聯網密鑰交換協議(Internet Key Exchange)

  * 對建立IPSEC的雙方進行認證(需要預先協商認證方式)
  * 通過密鑰交換，產生用於加密和HMAC的隨機密鑰
  * 協商協議參數(加密協議 HASH函數 封裝協議 封裝模式 密鑰有效期)

* 協商完成後的結果就叫做安全關聯(SA)

  * IKE SA
    * 安全防護 協議的細節
  * IPSEC SA
    * 安全防護實際用戶流量的細節

* SKEME(安全密鑰交換機制):主要使用DH來實現密鑰交換

* oakley : 決定了IPSec的框架設計 讓IPSEC能夠支持更多的協議

* ISAKMP : IKE協議的本質協議 決定了IKE協商包的封裝格式 交換過程和模式的切換

  

### 兩個階段 三個模式

* 第一階段協商
  * IKE1
    * 主模式(MM) : 六個包交換
      * 1-2包交換
        * 加密策略
        * HASH函數
        * DH組
        * 認證方式
        * 密鑰有效期
      * ![image-20200222141121212](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200222141121212.png)
      * ![image-20200222141034516](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200222141034516.png)
    * IKE3-4包
      * 密鑰交互
      * 使用DH產生
      * ![image-20200222141605212](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200222141605212.png)
      * IKE5-6包
        * 在安全環境進行認證
        * 第5-6 包開始往後 都使用IKE1-2 包交換所協商的加密與HMAC算法進行保護
      * 認證
        * PSK
        * ![image-20200222141951124](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200222141951124.png)
        * 證書認證
        * RSA加密隨機數認證
  * 主動模式(AM) : 三個包交換

* IKE2第二階段
  * 快速模式 1-3包交換
  * 在安全的環境下 基於感興趣流加密協商處理其IPSEC的策略
  * SPI安全參數索引
    * 用於唯一標示一個IPSEC SA
    * IN方向
    * OUT方向



#### 配置

![image-20200223202150709](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200223202150709.png)

```
R2
!1.
crypto isakmp policy 10
 encr aes
 hash md5
 authentication pre-share
 group 2
 lifetime 3600
crypto isakmp key 6 CCIE6 address 13.1.1.1
!
!
crypto ipsec transform-set CCIE55 esp-aes esp-md5-hmac
!
!
crypto map CCIE55MAP 10 ipsec-isakmp
 set peer 13.1.1.1
 set transform-set CCIE55
 match address 101
!
!
interface Ethernet0/0
 ip address 12.1.1.1 255.255.255.0
 ip nat outside
 crypto map CCIE55MAP
 
access-list 100 deny   ip 192.168.1.0 0.0.0.255 192.168.2.0 0.0.0.255
access-list 100 permit ip any any
access-list 101 permit ip 192.168.1.0 0.0.0.255 192.168.2.0 0.0.0.255
----------------------------------------------------------------------------------------
R4
!
 crypto isakmp policy 10
 encr aes
 hash md5
 authentication pre-share
 group 2
 lifetime 3600
crypto isakmp key 6 CCIE6 address 12.1.1.1
!
!
crypto ipsec transform-set CCIE55 esp-aes esp-md5-hmac
!
!
crypto map CCIE55MAP 10 ipsec-isakmp
 set peer 12.1.1.1
 set transform-set CCIE55
 match address 101
!
!
interface Ethernet0/0
 ip address 192.168.2.254 255.255.255.0
 ip nat inside
 ip virtual-reassembly in
!
interface Ethernet0/1
 ip address 13.1.1.1 255.255.255.0
 ip nat outside
 ip virtual-reassembly in
 crypto map CCIE55MAP
!
access-list 100 deny   ip 192.168.2.0 0.0.0.255 192.168.1.0 0.0.0.255
access-list 100 permit ip any any
access-list 101 permit ip 192.168.2.0 0.0.0.255 192.168.1.0 0.0.0.255
!
```

#### IPsec 問題

* 不支持組播
* 不能建立動態路由協議

#### 解決方案

* GRE Over IPSec

* ![image-20200226093147516](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200226093147516.png)

* ```
  R1
  !
  ip access-list extended vpn
   permit gre host 102.1.1.1 host 203.1.1.3
  !
  access-list 1 permit 10.1.1.0 0.0.0.255
  access-list 100 deny   ip 10.1.1.0 0.0.0.255 2.2.2.0 0.0.0.255
  access-list 100 deny   ip host 1.1.1.1 host 2.2.2.2
  access-list 100 permit ip any any
  !
  !Build GRE tunnel
  interface Tunnel0
   ip address 100.1.1.1 255.255.255.0
   tunnel source 102.1.1.1
   tunnel destination 203.1.1.3
  !
  !stage 1 negotiation
  crypto isakmp policy 10
   encr 3des
   hash sha256
   authentication pre-share
   group 5
   lifetime 3600
  crypto isakmp key 6 cisco address 203.1.1.3
  !
  crypto ipsec transform-set Tran esp-3des esp-sha256-hmac
   mode transport
  !
  crypto map Crymap 10 ipsec-isakmp
   set peer 203.1.1.3
   set transform-set Tran
   match address vpn
  !
  interface Ethernet0/0
   ip address 102.1.1.1 255.255.255.0
   ip nat outside
   crypto map Crymap
  !
  ip nat inside source list 100 interface Ethernet0/0 overload
  !
  ```

* GRE缺點是什麼?

  * 會增加GRE包頭

* SVTI
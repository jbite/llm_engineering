## PIM-SM 雙向樹

Bidirectional PIM and Source Specific Multicast

SM中的高級特性

* 讓s<-->rp之間全部走(*, G) ===> BDP
* 讓S<----->R之間全部形成(S, G) ==> SSM

![image-20200306121146888](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200306121146888.png)

6. 雙向樹用在源及接收者同一側的狀態

R6 RP loopback口也需要啟用ip pim sparse-mode
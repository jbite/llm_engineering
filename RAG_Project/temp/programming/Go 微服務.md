# Go 微服務

一種分布式系統解決方案 推動細粒度服務的使用

可以理解為一種細粒度SOA(service-oriented Architecture)

微服務假夠風格是將單個應用程序作為一組小型服務開發的方法 每個程序都可以在自己的進程中運行 併且與輕量級機制 (通常是HTTP資源API)進行通信 這些服務是圍繞業務功能構建的



特點: 

* 單一職責
* 輕量級的通信
* 隔離性
* 有自己的數據
* 技術多樣性

誕生背景:

* 互聯網行業的快速發展 需求變化快 用戶數量變化快
* 敏捷開發深入人心 用最小的代價 做最快的迭代 頻繁修改測試 上限
* 容器技術的成熟 是微服務的技術基礎

### 互聯網架構演進之路

* 單體
  * 所有功能集中在一個項目中
  * 項目整個打包 可以部屬到伺服器運行
  * 應用與資料庫可以分開部屬 提高性能
    * 優點: 小項目的首選 開發成本低 架構簡單
    * 缺點: 項目複雜之後很難擴展及維護
      * 擴展成本高 有瓶頸
      * 技術棧受限

![image-20200613191514914](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200613191514914.png)

​	

* 垂直架構
  * 

![image-20200613192044038](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200613192044038.png)

* SOA架構
* 微服務架構

![image-20200613192537024](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200613192537024.png)

![image-20200613192831167](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200613192831167.png)

![image-20200613192950525](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200613192950525.png)

* 優勢
  * 獨立性 
  * 使用者容易理解
  * 技術棧靈活
  * 高效團隊
* 不足
  * 額外的工作 服務的拆分
  * 保證數據一致性
  * 增加了溝通的成本

### RPC

Remote Procedure Call 

* 電腦透過通信協議傳輸指令





## GoLang RPC 語法要求

```
- the method's type is exported.
- the method is exported.
- the method has two arguments, both exported (or builtin) types.
- the method's second argument is a pointer.
- the method has return type error.
```



### go RPC 服務端

```go
package main

import (
	"net/http"
	"net/rpc"
)

type Rect struct {
}

type Params struct {
	Width, Height int
}

func (r *Rect) Area(p Params, ret *int) error {
	*ret = p.Width * p.Height
	return nil
}

func (r *Rect) Perimeter(p Params, ret *int) error {
	*ret = (p.Width + p.Height) * 2
	return nil
}

func main() {
	//註冊服務
	rect := new(Rect)
	rpc.Register(rect)
	rpc.HandleHTTP()

	err := http.ListenAndServe(":8080", nil)
	if err != nil {
		return
	}
}

```

### go RPC 客戶端

```go
package main

import (
	"fmt"
	"log"
	"net/rpc"
)

type Params struct {
	Width, Height int
}

//調用服務
func main() {
	//1.連接遠程RPC服務
	conn, err := rpc.DialHTTP("tcp", "127.0.0.1:8080")
	if err != nil {
		return
	}
	//2.調用遠程方法
	p := new(Params)
	p.Width = 10
	p.Height = 5
	//求面積
	ret := 0
	err = conn.Call("Rect.Area", p, &ret)
	if err != nil {
		log.Fatal(err)
	}
	fmt.Println("面積為:", ret)
	err = conn.Call("Rect.Perimeter", p, &ret)
	fmt.Println("周長為:", ret)
}
```

### jsonrpc

可以跨語言調用



server

```go 
type Calc struct {
}
type Params struct {
	Num1, Num2 int
}
type Response struct {
	Mul int
	Quo int
}

func (c *Calc) Add(p Params, ret *int) error {
	*ret = p.Num1 + p.Num2
	return nil
}

func (c *Calc) Sub(p Params, ret *int) error {
	*ret = p.Num1 - p.Num2
	return nil
}

func (c *Calc) CalMul(p Params, r *Response) error {
	r.Mul = p.Num1 * p.Num2
	return nil
}

func (c *Calc) CalQuo(p Params, r *Response) error {
	r.Quo = p.Num1 / p.Num2
	return nil
}
func main() {
	calc := new(Calc)
	rpc.Register(calc)
	listener, err := net.Listen("tcp", "127.0.0.1:8081")
	if err != nil {
		log.Fatal(err)
	}

	for {
		conn, err := listener.Accept()
		if err != nil {
			continue
		}
		go func(conn net.Conn) {
			fmt.Println("new Client")
			jsonrpc.ServeConn(conn)
		}(conn)
	}
}
```

client

```go
package main

import (
	"fmt"
	"net/rpc/jsonrpc"
)

type Params struct {
	Num1, Num2 int
}

type Response struct {
	Mul int
	Quo int
}

func main() {
	p := new(Params)
	p.Num1 = 90
	p.Num2 = 60
	ret := 0
	res := new(Response)
	conn, _ := jsonrpc.Dial("tcp", "127.0.0.1:8081")
	conn.Call("Calc.Add", p, &ret)
	fmt.Println(ret)
	conn.Call("Calc.Sub", p, &ret)
	fmt.Println(ret)
	conn.Call("Calc.CalMul", p, &res)
	fmt.Println(res.Mul)
	conn.Call("Calc.CalQuo", p, &res)
	fmt.Println(res.Quo)
}

```

### RPC調用流程

![image-20200613225516263](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200613225516263.png)

#### 網路傳輸數據格式

header payload

uint32    []byte

### 實現RPC

```go
package server

import (
	"encoding/binary"
	"io"
	"net"
)

//編寫會話中數據讀寫
//會話連接的結構體

type Session struct {
	conn net.Conn
}

func NewSession(conn net.Conn) *Session {
	return &Session{conn: conn}
}

func (s *Session) Write(data []byte) error {
	buf := make([]byte, 4+len(data))

	//寫入頭部數據
	//binary只認固定長度的類型 所以使用uint32 而不是直接寫入
	binary.BigEndian.PutUint32(buf[:4], uint32(len(data)))
	copy(buf[4:], data)
	_, err := s.conn.Write(buf)
	if err != nil {
		return err
	}
	return nil
}

func (s *Session) Read() ([]byte, error) {
	//讀取頭部數據
	header := make([]byte, 4)
	//按頭部讀出的長度開闢一個切片
	_, err := io.ReadFull(s.conn, header)
	if err != nil {
		return nil, err
	}
	dataLen := binary.BigEndian.Uint32(header)
	data := make([]byte, dataLen)
	_, err = io.ReadFull(s.conn, data)
	if err != nil {
		return nil, err
	}
	return data, nil
}
```

### RPC服務端

* 接受的數據包括:

  * 調用的函數名 參數列表
  * 一般會約定函數的第二個返回值是error類型

  * 通過反射實現

* 服務端要解決的問題是什麼

  * client調用時只傳過來函數名 須要維護函數名到函數之間的Map映射

* 服務端的核心功能有哪些
  * 維護函數名到函數反射值的map
  * client端傳函數名 參數列表後 服務端要解析為反射值 調用執行
  * 函數的返回值打包 通過網路返回給客戶端
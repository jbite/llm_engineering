# Go實戰-用戶聊天系統

## 需求分析

1. 用戶註冊
2. 用戶驗證
3. 顯示在線客戶列表
4. 群聊
5. 點對點聊天
6. 離線留言: 需要數據庫

### 項目開發流程

需求分析→ 設計階段→編碼實現→測試階段→實施

# 聊天室系統規劃

## 伺服端

### 客戶登入

1. 接收用戶id pwd
2. 比較
3. 返回結果

## 客戶端

### 客戶登入

1. 接收輸入的id和pwd
2. 發送id和密碼
3. 接收到服務端返回的結果
4. 判斷成功還是失敗 並顯示對應的頁面

**關鍵問題**

怎樣組織發送的數據

1. 設計消息協議

   ```go
   type Message struct{
     Type string
     Data string
   }
   ```

2. 登入消息

   ```go
   type LoginMessage struct{
     UserId int
     UserPwd string
   }
   //序列化後放入Message的Data內容
   ```

3. 消息發送的流程

   1. 先創建一個Message的結構體

   2. 設置消息類型 Mes.Type = 登入消息類型

   3. Mes.Data = Serialized(LoginMes)

   4. Serialized(Mes): 序列化Mes結構體

   5. 在網路傳輸中 防止丟包問題

      (1). 發給伺服器要發送的mes長度有多少(byte)

      (2). 發送消息

4. 消息接收的流程:

   1. 接收客戶端待發送的訊息長度
   2. 根據要接收的長度 來接收消息
   3. 接收並檢查Mes訊息長度是否吻合
   4. 如果消息長度正確，就將Mes反序列化
   5. 取出消息將LoginMes反序列化
   6. 取出loginMes.userId 和LoginMes.userPwd
   7. 比對登入結果
   8. 返回結果Mes

#### 實作步驟

思路:

1. 先確定消息message的結構體
2. 完成客戶端可以發送消息本身 服務器可以正常接收到消息 並根據客戶端發送的消息LgoinMes判斷用戶的合法性 並返回相應的LoginResMes
   1. 讓客戶端發送消息本身
   2. 伺服器端接收到消息，將消息反序列化成對應的消息結構體
   3. 伺服器根據消息收到內容 判斷用戶是否合法 返回LgoingResMes
   4. 客戶端解析返回的LoginResMes顯示對應介面
   5. 這裡我們需要封裝函數

### 實現功能 完成用戶登入

程序結構的改進

畫出程序架構圖

代碼實現程式結構的改進

1. 先改進伺服器 先劃出程序的框架圖 再寫代碼

![image-20200530164919091](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200530164919091.png)

2. 步驟
   * 先把分析出來的文件創建好 放到相應的目錄中(包)
   * 跟據文件完成的任務不同 剝離到對應的文件

![image-20200530191455187](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200530191455187.png)

2. 客戶端的改進
3. ![image-20200530193340899](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200530193340899.png)

### 使用redis儲存用戶

1. 先手動添加用戶 測試登入成功之後 再來實現註冊功能
2. ![image-20200530223054626](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200530223054626.png)

在redis手動添加用戶

```
redis> hset users 100 "{\"userId\":100,\"userPwd\":\"123456\",\"userName\":\"Scott\"}"
```

透過redis判斷用戶驗證訊息:

提示信息:

1. 用戶不存在或是密碼錯誤
2. 邀請用戶註冊



新增model/user.go

```go
package model

// announce an user struct

type User struct {
	//為了序列化及反序列化成功
	//必須保證 用戶訊息的json字符串的key和結構體的自斷對應的tag名字一致
	UserId   int    `json:"userId"`
	UserPwd  string `json:"userPwd"`
	UserName string `json:"userName"`
}

```

新增model/error.go

```go
package model

import "errors"

//根據業務邏輯需要 自定義一些錯誤

var (
	ErrorUserNotExists = errors.New("用戶不存在")
	ErrorUserExists    = errors.New("用戶已存在")
	ErrorUserPwd       = errors.New("密碼不正確")
)

```

新增model/userDao.go

```go
package model

import (
	"encoding/json"
	"fmt"

	"github.com/garyburd/redigo/redis"
)

//After server initial, start a userDau instance
//make it be a global variable, when we need to m
var (
	MyUserDao *UserDao
)

//定義一個UserDao結構體
//完成對User結構體的各種操作
type UserDao struct {
	Pool *redis.Pool
}

//使用工廠模式創建一個UserDao實例
func NewUserDao(pool *redis.Pool) (userdao *UserDao) {
	userdao = &UserDao{
		Pool: pool,
	}
	return
}

//想想應該提供哪些方法
//get an instance from redis by pool,
func (u *UserDao) GetUserById(conn redis.Conn, id int) (user *User, err error) {
	//query user from redis by id
	res, err := redis.String(conn.Do("HGET", "users", id))
	fmt.Println(res)
	if err != nil {
		//error
		if err == redis.ErrNil { //not found users in hash
			err = ErrorUserNotExists
			return
		}
		return
	}
	user = &User{}
	//把res反序列化成User實例
	err = json.Unmarshal([]byte(res), user)
	if err != nil {
		fmt.Println("GetUserById unmarshal err=", err)
		return
	}
	return
}

//完成登入的校驗
//1. Login
//2. if user id and pwd both correct, return an user instance
//3. if user or pwd is wrong, reutrn err Message
func (u *UserDao) Login(userId int, userPwd string) (user *User, err error) {
	//get a connection from pool
	conn := u.Pool.Get()
	defer conn.Close()
	user, err = u.GetUserById(conn, userId)
	if err != nil {
		return
	}

	if userPwd != user.UserPwd {
		err = ErrorUserPwd
		return
	}
	return
}
```

### 透過redis 驗證用戶訊息

新增main/redis.go

```go
package main

import (
	"time"

	"github.com/garyburd/redigo/redis"
)

var pool *redis.Pool

func initPool(addr string, maxIdle int, maxActive int, idleTimeOut time.Duration) {
	pool = &redis.Pool{
		MaxIdle:     maxIdle,
		MaxActive:   maxActive,
		IdleTimeout: idleTimeOut,
		Dial: func() (redis.Conn, error) {
			return redis.Dial("tcp", addr)
		},
	}
}
```

main中新增redis連接池的初始化任務

```go
//a function to init userDao
func initUserDao() {
	model.MyUserDao = model.NewUserDao(pool)
}

func init() {
	//初始化redis pool when server start
	initPool("192.168.56.35:6379", 8, 16, 300*time.Second)
	initUserDao()
}
```

在process/userProcess.go修改到redis做驗證

```go
//1.announce a resMes struct to pack LoginResMes
	var resMes message.Message
	resMes.Type = message.LoginResMesType
	//announce a LoginResMes
	var LoginResMes message.LoginRes
	//use model.MyUserDao to auth from redis
	user, err := model.MyUserDao.Login(loginMes.UserId, loginMes.UserPwd)

	if err != nil {
		if err == model.ErrorUserNotExists {
			LoginResMes.Code = 500
			LoginResMes.Error = err.Error()
		} else if err == model.ErrorUserPwd {
			LoginResMes.Code = 403
			LoginResMes.Error = err.Error()
		} else {
			LoginResMes.Code = 505
			LoginResMes.Error = "服務器內部錯誤"
		}
	} else {
		LoginResMes.Code = 200
		fmt.Println(user, "登入成功了")
	}
```

### 實現用戶註冊

0. 先把user.go放入到commoin/中

1. common/message/message.go中新增消息類型
2. 在客戶端接收用戶的輸入
3. 在client/process/userProcess.go增加一個Register方法 請求完成註冊
4. server/model/userDao.go增加Register方法

### 當前用戶在線列表

![image-20200531130953807](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200531130953807.png)

### 當新用戶上線發送通知

可能方法:

1. 當一個用戶上線 伺服器就把維護的onlineUsers map整體推送

2. 每隔一段時間把維護的onlineUsers map整體推送
3. 用戶A上線過後 伺服器就把A的狀態推送給在線的用戶
   1. 客戶端各自維護一個map 依照伺服器更新的訊息來更新用戶狀態
   2. map[int]User
4. 客戶端和伺服器的通訊通道要依賴serverProcessMes協程



### 群聊功能

完成客戶端可以發送消息的思路

1. 新增一個消息結構體 smsMes
2. 新增一個model CurUser
3. 在smsProcess.go增加相應的方法 SendGroupMes

common/message/message.go

增加SmsMes struct

還需要維護一個跟客戶端的連接 以便跟伺服器溝通

把這個連接透過CurUser這個結構體來維護

之後要使用就從這邊調用

發送消息到其它用戶端(發送者除外)

在伺服器端接收到smsMes類型的消息後

server/process/smsProcess.go中新增群發消息的方法

在客戶端還要處理伺服器轉發的群發消息

### 擴展功能

1. 點對點聊天(私聊)
2. 如果一個登入用戶離線 就把此人從在線列表去除
3. 實線離線留言 :在群聊時，如果某個用戶沒有在線 登入後 可以接受離線的消息
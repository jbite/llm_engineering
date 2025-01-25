# GoLang 操作Redis

## 依賴庫

```
github.com/garyburd/redigo/redis
```

### 連接資料庫

```go
func main1601() {
	conn, e := redis.Dial("tcp", "192.168.56.35:6379")
	fmt.Println("連接成功")
	defer conn.Close()
}
```

### redis操作

```go
func main() {
	conn, e := redis.Dial("tcp", "192.168.56.35:6379")
	fmt.Println("連接成功")
	defer conn.Close()
	reply, e := conn.Do("Get", "name")
}
```

### 輸出結果

```go
func main() {
	conn, e := redis.Dial("tcp", "192.168.56.35:6379")
	HadleError(e, "redis.Dial")
	fmt.Println("連接成功")
	defer conn.Close()
	reply, e := conn.Do("Get", "name")
	HadleError(e, "conn.Do")
    //轉換類型
	ret, _ := redis.Strings(reply, e)
	fmt.Printf("%s", ret)
}
```

### Redis連接池 

避免無限的開闢連接

創建一個池子，讓用戶連接，預留IO讓伺服器不會因此資源耗盡

```go
package main

import (
	"fmt"
	"strconv"
	"time"
	//pool依賴庫
	"github.com/garyburd/redigo/redis"
)
//建立redis連接並且從pool中拿取資源
func getConnFromPoolAndHappy(pool *redis.Pool, i int) {
	conn := pool.Get()
	defer conn.Close()

	reply, err := conn.Do("set", "conn"+strconv.Itoa(i), i)
	s, _ := redis.String(reply, err)
	fmt.Println(s)
	time.Sleep(20 * time.Second)
}

func main() {
    //創建pool指針
  //可以放在init()
	pool := &redis.Pool{
		//最大閒置連接數
		MaxIdle: 2,
		//最大活動連接數, 0=無限
		MaxActive: 8,
		//閒置連接的超時時間
		IdleTimeout: time.Second * 100,
		//定義撥號獲得連接的函數
		Dial: func() (redis.Conn, error) {
			return redis.Dial("tcp", "192.168.56.35:6379")
		}}
	defer pool.Close()
    //併發與redis連接
	for i := 0; i < 100; i++ {
		go getConnFromPoolAndHappy(pool, i)
	}
	time.Sleep(300 * time.Second)
}

```

### 實現二級緩存

程序運行起來之後 提示"請輸入命令: " 輸入getall查詢並顯示所有人員信息

第一次查詢mysql將結果緩存在redis設置60秒的過期時間

以後的每次查詢 如果redis有數據就從redis加載 沒有就重複上一步的操作

```go
package main

import (
	"fmt"
	"os"

	"github.com/garyburd/redigo/redis"
	_ "github.com/go-sql-driver/mysql"
	"github.com/jmoiron/sqlx"
)

type Person struct {
	Name string `db:"name"`
	Age  int    `db:"age"`
}

func main() {
	var cmd string
	for {
		fmt.Printf("請輸入命令:")
		fmt.Scan(&cmd)

		switch cmd {
		case "getall":
			//顯示人員信息
			GetAllPeople()
		case "exit":
			goto GameOver
		default:
			fmt.Println("命令錯誤")
		}
	}
GameOver:
	fmt.Println("Game Over")
	os.Exit(1)
}

func GetAllPeople() {

	//先嘗試拿緩存
	p := GetPeopleFromRedis()
	//緩存如果沒有資料 則從mysql取得數據
	if p == nil || len(p) == 0 {
		people := GetPeopleFromMySQL()
		//緩存查詢結果到redis
		CachePeople2Redis(people)
	} else {
		fmt.Println("從redis取得數據:", p)
	}

}

func GetPeopleFromMySQL() (people []Person) {
	db, _ := sqlx.Connect("mysql", "root:12345678@tcp(192.168.56.35:3306)/mydb")
	defer db.Close()
	err := db.Select(&people, "select name,age from person;")
	HadleError(err, "select name,age from person;")
	fmt.Println("從MySQL取得數據: ", people)
	return
}

//從redis拿信息
func GetPeopleFromRedis() (people []string) {
	conn, err := redis.Dial("tcp", "192.168.56.35:6379")
	defer conn.Close()
	reply, err := conn.Do("lrange", "people", 0, -1)
	HadleError(err, "@lrange people 0 -1")
	people, err = redis.Strings(reply, err)
	// fmt.Println("緩存拿取結果", people, err)
	return
}
//執行緩存數據
func CachePeople2Redis(p []Person) {
	conn, err := redis.Dial("tcp", "192.168.56.35:6379")
	defer conn.Close()
	HadleError(err, "redis.Dial")
	for _, person := range p {
		personStr := fmt.Sprint(person)
		_, err := conn.Do("rpush", "people", personStr)
		HadleError(err, "rpus people"+personStr)
	}
	_, err = conn.Do("expire", "people", 60)
	HadleError(err, "expire people 60")
	fmt.Println("緩存成功")
}
//錯誤處理
func HadleError(err error, when string) {
	if err != nil {
		fmt.Println(when, err)
		os.Exit(1)
	}
}
```




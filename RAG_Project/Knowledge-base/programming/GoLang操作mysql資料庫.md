# GoLang操作mysql資料庫

## 安裝第三方庫

```
#cmd
go get github.com/go-sql-driver/mysql
go get github.com/jmoiron/sqlx
```

## 建表

```sql
create database mydb;

create table person (
id int auto_increment,
name varchar(20) not null,
age int,
rmb float,
gender bool,
primary key (id)
);

create table person (
id int auto_increment,
name varchar(20) not null,
password varchar(20),
email varchar(20),
primary key (id)
);
```

### 連接資料庫

```go
package main

import (
	"fmt"
	//庫引用
	_ "github.com/go-sql-driver/mysql"
	"github.com/jmoiron/sqlx"
)


/* 執行增刪改 */
func main() {
	db, err := sqlx.Open("mysql", "root:12345678@tcp(192.168.56.35:3306)/mydb")
	defer db.Close()
    if err != nil {
		fmt.Println(err)
        return
	}
}
```



### 增刪改

```go
package main

import (
	"fmt"
//庫引用
	_ "github.com/go-sql-driver/mysql"
	"github.com/jmoiron/sqlx"
)

type Person struct {
	//對應Nname表字段
	Name string `db:"name"`
	//對應age表字段
	Age int `db:"Age"`
	//對應rmb表字段
	Money float64 `db:"rmb"`
}

/* 執行增刪改 */
func main() {
	db, err := sqlx.Open("mysql", "root:12345678@tcp(192.168.56.35:3306)/mydb")
	defer db.Close()
	if err != nil {
		fmt.Println(err)
	}
	//增
	// result, err := db.Exec("insert into person (name, age,rmb,gender,birthday) values(?,?,?,?,?);", "張句蛋", 50, 123, false, 19880123)
    //刪
	// result, err := db.Exec("delete from person where name not like ?;", "%但")
    //改
	result, err := db.Exec("update person set name=(?) where id=?", "張一蛋", 5)

	if err != nil {
		fmt.Println("失敗", err)
		return
	} else {
		rowsAffected, _ := result.RowsAffected()
		lastInsertID, _ := result.LastInsertId()

		fmt.Println("受影響的行數=", rowsAffected)
		fmt.Println("最後一行的ID=", lastInsertID)
	}
}

```

#### NamedExec

```go
func insertUserDemo()(err error){
	_, err = db.NamedExec(`INSERT INTO users (name,age) VALUES (:name,:age)`,
		map[string]interface{}{
			"name": "七米",
			"age": 28,
		})
	return
}
```

#### NamedQuery

```go
// 使用map做命名查询
rows, err = db.NamedQuery(`SELECT * FROM users WHERE name=:name`, map[string]interface{}{"name": "Q1mi"})

type User struct {
	Name string `db:"name"`
	Age  int    `db:"age"`
}
u := User{
	Name: "q1mi",
}
// 使用结构体命名查询，根据结构体字段的 db tag进行映射
rows, err = db.NamedQuery(`SELECT * FROM users WHERE name=:name`, u)
```



### 查

單行查詢

```go
type Person struct {
	//對應Nname表字段
	Name string `db:"name"`
	//對應age表字段
	Age int `db:"age"`
	//對應rmb表字段
	Money float64 `db:"rmb"`
}
func main() {
	db, err := sqlx.Open("mysql", "root:12345678@tcp(192.168.56.35:3306)/mydb")
	defer db.Close()
	if err != nil {
		fmt.Println("連接失敗", err)
		return
    
  sqlStr:="select name, age,money from person where id =?"
    var u Person
    err:= db.Get(&u, sqlStr,1) 
	}
  
  
```





```go
type Person struct {
	//對應Nname表字段
	Name string `db:"name"`
	//對應age表字段
	Age int `db:"age"`
	//對應rmb表字段
	Money float64 `db:"rmb"`
}
func main() {
	db, err := sqlx.Open("mysql", "root:12345678@tcp(192.168.56.35:3306)/mydb")
	defer db.Close()
	if err != nil {
		fmt.Println("連接失敗", err)
		return
	}
	//
	var ps []Person
	db.Select(&ps, "select name, age,rmb from person where id=?;", 5)

	fmt.Println("查詢成功", ps)

}
```

### 事務

有原子性 不可分割 一個步驟失敗 一系列的操作都要回滾

就像外交事務中，接待、安防、餐會、記者會全部都不能有失敗，否則就是一場失敗的外交事務。

現實中我們不能復原失敗的事務，資料庫中，可以透過事務來確保操作的完整性

在連接資料庫以後，透過`Begin()`開啟事務，`commit()`確認事務

```go
func main() {
	db, err := sqlx.Open("mysql", "root:12345678@tcp(192.168.56.35:3306)/mydb")
	defer db.Close()
	if err != nil {
		fmt.Println(err)
		return
	}
	//開啟事務
	tx, _ := db.Begin()

	//執行系列增刪改方法
	ret1, e1 := tx.Exec("insert into person(name,age,gender) values(?,?,?);", "鹹鴨蛋", 20, true)
	ret2, e2 := tx.Exec("delete from person where name not like ?;", "%蛋")
	ret3, e3 := tx.Exec("update person set name=? where name=?;", "滷蛋", "雙黃蛋")

	//有任何失敗，事務就回滾
	if e1 != nil || e2 != nil || e3 != nil {
		fmt.Println("事務執行失敗,e1/e2/e3=", e1, e2, e3)
		//事務回滾
		tx.Rollback()
	} else {
		tx.Commit()
		ra1, _ := ret1.RowsAffected()
		ra2, _ := ret2.RowsAffected()
		ra3, _ := ret3.RowsAffected()

		fmt.Println("事務執行成功,受影響的行=", ra1+ra2+ra3)
	}
}

```


# Go Web框架 Gin

### 安裝

```
go get github.com/gin-gonic/gin
```

[依照七米影片所做的筆記](https://www.liwenzhou.com/posts/Go/Gin_framework/)



### 啟動web應用

1. 創建一個main.go

2. 引用下列包

   ```go
   github.com/gin-gonic/gin
   ```

3. 開始寫web應用

   一個最基本的web應用就是這樣的結構

   ```go 
   func main() {
   	r := gin.Default() //返回默認的路由引擎
   	//url路徑 當用戶端連到這個路徑時 返回sayHello函數
   	r.GET("/hello", sayHello)
    
       r.Run()
   }
   //定義sayHello函數要做的事情
   func sayHello(c *gin.Context){
       c.JSON(200,gin.H{
           "message":"Hello",
       })
   }
   ```

   



### Gin 的渲染

使用go內置template庫

#### 模板與渲染

#### GO的模板引擎

`text/template` 和`html/template`

* 模板文件通常訂為.tml和.tpl，必須使用utf8編碼
* 使用{{和}}包住和標示需要傳入的數據
* 傳給模板這樣的數據就可以通過點號(`.`)來訪問，如果數遽是引用類型，可以通過{{.FieldName}}來訪問他的字段
* 除了{{和}}包裹的內容外 其他內容均不做修改原樣輸出

#### 模板引擎的使用

* 定義模板文件
* 解析模板文件
* 模板渲染

### 定義模板文件

通常會將模板文件命名為.tml並放到templates的目錄中

一個模板會長得像這樣，命名為hello.tml

下面渲染範例都會使用這個文件可以先創建並加入以下內容:

```html
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <title>Hello</title>
</head>
<body>
    <p>Hello {{ . }}</p>
</body>
</html>
```

### 解析模板文件

```go
func (t *Template) Parse(src string) (*Template, error)
func ParseFiles(filenames ...string) (*Template, error)
func ParseGlob(pattern string) (*Template, error)
```

### 模板渲染

```go
func (t *Template) Execute(wr io.Writer, data interface{}) error

func (t *Template) ExecuteTemplate(wr io.Writer, name string, data interface{}) error
```

#### 渲染基本變數

```go
t, err := template.ParseFiles("../templates/hello.tml")
user:= "小王子"
t.Execute(w, user)
```

渲染結果

```
Hello, 小王子
```

#### 渲染map

```go
t, err := template.ParseFiles("../templates/hello.tml")
m1 := map[string]interface{}{
		"name":   "小王子",
		"gender": "Male",
		"age":    18,
	}
t.Execute(w, m1)
```

hello.tml則要改成

```html
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <title>Hello</title>
</head>
<body>
    <p>Hello, {{ .name }}</p>
    <p>性別: {{ .gender }}</p>
    <p>年齡: {{ .age }}</p>
</body>
</html>
```

渲染結果

```
Hello 小王子
性別:Male
年齡:18
```

#### 渲染結構體

```go
t, err := template.ParseFiles("../templates/hello.tml")
type User struct {
		Name   string
		Gender string
		Age    int
	}
u1 := User{"Jacky", "Male", 18}
t.Execute(w, u1)
```

hello.tml則要改成

> 這裡要注意，變數名開頭要用大寫。在go的結構體中，首字母大寫，代表public可讀，小寫表示此變數是private

```html
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <title>Hello</title>
</head>
<body>
    <p>Hello, {{ .Name }}</p>
    <p>性別: {{ .Gender }}</p>
    <p>年齡: {{ .Age }}</p>
</body>
</html>
```

渲染結果

```
Hello 小王子
性別:Male
年齡:18
```

#### 同時渲染多個變數

```go
//定義map
m1 := map[string]interface{}{
		"name":   "小王子",
		"gender": "Male",
		"age":    18,
	}
//定義struct
type User struct {
	Name   string
	Gender string
	Age    int
}
u1 := User{"Jacky", "Male", 18}
//使用map將兩個變數傳入，interface{}表示個種類型的變數
err = t.Execute(w, map[string]interface{}{
		"u1":   u1,
		"user": user,
	})
```

#### 模板注釋

```
{{/*   */}}
```

#### 模板變量

```
{{ $v1 := 100 }}
//接收go傳進來的變數
{{ $age := .m1.age }}
```

#### 移除空格

```
{{-  -}}
```

#### 條件判斷

```
{{if $v1 }}
{{ $v1 }}
{{else if}}
{{ $v2 }}
{{else}}
啥都沒有
{{end}}
```

#### range

Go的模板語法中使用`range`關键字進行遍歷，有以下两種寫法，其中`pipeline`的值必须是數组、切片、字典或者通道。

```
{{range pipeline}} T1 {{end}}

{{range pipeline}} T1 {{else}} T0 {{end}}

{{range $idx,$hobby := .hobby}}
<p>{{$idx}}-{{$hobby}}</p>
{{end}}
```

渲染效果

```
0-籃球

1-足球

2-排球
```

渲染結構體切片 先將結構體切片透過json.Marshaly再用json.Ummarshal()轉換成map的切片，即可在html頁面中循環渲染

```go
/
type Article struct {
	Title   string `json:"title"`
	Content string `json:"content"`
}

article1 := &Article{Title: "hello body", Content: "articel content"}
	article2 := &Article{Title: "hello body", Content: "articel content"}
articles := []*Article{article1, article2}
b, _ := json.Marshal(&articles)
//透過切片將結構體封裝進去
var m []map[string]interface{}
json.Unmarshal(b, &m)
	
	fmt.Println("m:", m)
//m: [map[content:articel conten title:hello body] map[content:articel conten title:hello body]]
	c.HTML(http.StatusOK, "index.html", gin.H{
		"user":     "Jacky",
		"articles": m,
	})
```

#### 比較符號

```
eq
lt
gt
ne
le
ge

{{ if lt .m1.ag 18 }}
好好上學
{{ else }}
好好工作
{{ end }}
```

#### with

```
{{with .m1}}
<p>{{.name}}</p>
<p>{{.age}}</p>
```

### 模板嵌套

#### 新增自定義函數

當模板內有非內建的函數時，我們就需要告訴模板引擎，怎樣渲染

```
{{kua .}}
```

main.go

```go
//定義一個函數kua
//要麼只有一個返回值
//要兩個返回值 第二個返回值必須為error
func kua(name string) (string, error) {
	return name + "is handsome guy!", nil
}
func f1(w http.ResponseWriter, r *http.Request) {
	t := template.New("f.tml")
	//告訴模板引擎 我現在多了一個字定義的函數kua
	t.Funcs(template.FuncMap{
		"kua": kua,
	})
	//解析模板
	_, err := t.ParseFiles("../templates/f.tml")
	//渲染模板
	name := "小王子"
	t.Execute(w, name)
}
```

#### 嵌套

文件可以在同一個文件內嵌套使用 也可以在不同文件中使用

t.tml

```html
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <title>tmpl test</title>
</head>
<body>

<h1>測試嵌套template語法</h1>
<hr>
{{/* 嵌套另一個單獨的模板文件*/}}
{{template "ul.tml"}}
<hr>
{{/* 嵌套另一個define定義的模板*/}}
{{template "ol.tmpl"}}
<div>你好, {{.}}</div>
</body>
</html>

{{/*通過define定義一個模板*/}}
{{ define "ol.tmpl"}}
<ol>
    <li>吃飯</li>
    <li>睡覺</li>
    <li>打豆豆</li>
</ol>
{{end}}
```

ul.html

```html
<ul>
    <li>注释</li>
    <li>日志</li>
    <li>测试</li>
</ul>
```

main.go

```go
func f2(w http.ResponseWriter, r *http.Request) {
	pattern := "../templates/l6/*.tml"
	t, err := template.ParseGlob(pattern)
	HandleError(err, "template.ParseGlob")

	name := "小王子"
	t.Execute(w, name)
}
func main(){
  http.HandleFunc("/demo1", f2)
}
```

#### block

類似模板繼承

```
{{block "name" pipline}} T1 {{end}}
```

go

```go
func index(w http.ResponseWriter, r *http.Request) {
	tml := "index.tml"
	t := template.New(tml)
	t, err := template.ParseFiles("./templates/main.tml", "./templates/"+tml)
	HandleError(err, "t.ParseFiles(./templates/l7")

	name := "小王子"
	t.ExecuteTemplate(w, tml, name)

}

func home(w http.ResponseWriter, r *http.Request) {
	tml := "home.tml"
	t := template.New(tml)
	t, err := template.ParseFiles("./templates/main.tml", "./templates/"+tml)
	HandleError(err, "t.ParseFiles(./templates/")
	name := "小王子"
	t.ExecuteTemplate(w, tml, name)
}
```

html

index.tml

```html
{{template "main.tml" .}}
{{define "content" }}
<h1>這是index頁面</h1>
<p>Hello,{{.}}</p>
{{end}}

```

home.tml

```html
{{template "main.tml" .}}
{{define "content" }}
<h1>這是home頁面</h1>
<p>Hello,{{ . }}</p>
{{end}}
```

main.tml

```html
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <title>模板繼承</title>
    <style>
      * { margin :0;}
      .nav {
        height:50px;
        width: 100%;
        top:0;
        position:fixed;
        background-color: burlywood;
      }
      .main{
        margin-top:50px;

      }
      .menu{
        width:20%;
        height: 100%;
        position:fixed;
        left:0;
        background-color: cornflowerblue;
      }
      .content{
        width:80%;
        height:100%;
        right:0;
        position:fixed;
      }
    </style>
</head>
<body>
  <div class="nav">
    <a href="/index">index</a>
    <a href="/home">home</a>
  </div>
  <div class="main">
  <div class="menu"></div>
  <div class="content">
    {{block "content" .}}{{end}}
  </div>
  </div>}}
</body>
</html>
```

### 修改默認的標示符

```go
template.New("test").Delims("{%","%}")
```

#### 取消原始內容顯示

```go
func safe(s string) template.HTML {
	return template.HTML(s)
}

func index(w http.ResponseWriter, r *http.Request) {
	tml := "index.tml"
	t := template.New(tml)
    //將safe函數透過自定義的方式加入
	t.Funcs(template.FuncMap{
		"safe": safe,
	})
	_, err := t.ParseFiles("./templates/" + tml)
	HandleError(err, "t.ParseFiles(./templates/")
	str := "<script>alert(123);</script>"
	t.Execute(w, str)
}
```

html

```html
<body>
{{ . | safe }}
</body>
```

## GIN框架的模板渲染

### 使用LoadHTMLGLobe() 或是LoadHTMLFIles()

```go
func main() {
	r := gin.Default()
	r.LoadHTMLGlob("templates/**/*")
	r.GET("/posts/index", func(c *gin.Context) {
		c.HTML(http.StatusOK, "posts/index.html", gin.H{
			"title": "posts/index",
		})
	})

	r.GET("users/index", func(c *gin.Context) {
		c.HTML(http.StatusOK, "users/index.html", gin.H{
			"title": "users/index",
		})
	})

	r.Run(":8080")
}
```

### 靜態文件處理

```go
func main() {
	r := gin.Default()
    //當遇到xxx開頭的文件 到./statics去搜尋
	r.Static("/xxx", "./statics")
	r.LoadHTMLGlob("templates/**/*")
   // ...
	r.Run(":8080")
}
```

#### 模板繼承

使用這個功能需要另一個庫的支援`github.com/gin-contrib/multitemplate`

示例如下:

先準備home.tml和index.tml來繼承base.tml

```
templates
├── includes
│   ├── home.tmpl
│   └── index.tmpl
├── layouts
│   └── base.tmpl
└── scripts.tmpl
```

然後定義一個`loadTemplates`函數如下:

```go
func loadTemplates(templatesDir string) multitemplate.Renderer {
	r := multitemplate.NewRenderer()
	layouts, err := filepath.Glob(templatesDir + "/layouts/*.tmpl")
	if err != nil {
		panic(err.Error())
	}
	includes, err := filepath.Glob(templatesDir + "/includes/*.tmpl")
	if err != nil {
		panic(err.Error())
	}
	// 为layouts/和includes/目录生成 templates map
	for _, include := range includes {
		layoutCopy := make([]string, len(layouts))
		copy(layoutCopy, layouts)
		files := append(layoutCopy, include)
		r.AddFromFiles(filepath.Base(include), files...)
	}
	return r
}
```

在`main`函數中

```go
func indexFunc(c *gin.Context){
	c.HTML(http.StatusOK, "index.tmpl", nil)
}

func homeFunc(c *gin.Context){
	c.HTML(http.StatusOK, "home.tmpl", nil)
}

func main(){
	r := gin.Default()
	r.HTMLRender = loadTemplates("./templates")
	r.GET("/index", indexFunc)
	r.GET("/home", homeFunc)
	r.Run()
}
```

### GIN返回json

```go
func main() {

	r := gin.Default()
	r.GET("/json", func(c *gin.Context) {
		//方法一:使用map
		// data := map[string]interface{}{
		// 	"name":    "小王子",
		// 	"message": "hello world",
		// 	"age":     18,
		// }
		data := gin.H{
			"name":    "小王子",
			"message": "hello world",
			"age":     18,
		}
		c.JSON(http.StatusOK, data)
	})
	//方法二:結構體 靈活使用tag來對結構體字段做定製化操作
	type Msg struct {
		Name    string `json:"name"`
		Message string
		Age     int
	}
	r.GET("/another_json", func(c *gin.Context) {
		data := Msg{"小王子", "Hello World", 18}
		c.JSON(http.StatusOK, data)
	})
	r.Run()
}
```

### 獲取queryString

```go
func main() {
	r := gin.Default()
	r.GET("/json", func(c *gin.Context) {
		//獲取瀏覽器請求的querystring參數
		name := c.Query("query") //通過Query請求中攜帶的querystring
		age := c.Query("age")
		// name := c.DefaultQuery("query", "somebody")//取不到就用指定的默認值
		// name, ok := c.GetQuery("query")
		// age, ok := c.GetQuery("age")
		// if !ok {
		// 	//取不到
		// 	name = "somebody"
		// 	age = 1
		// } else {
		c.JSON(http.StatusOK, gin.H{
			"name": name,
			"age":  age,
		})
		// }

	})

	r.Run()
}
```

### 獲取form參數

```go
func main() {
	r := gin.Default()
	r.LoadHTMLFiles("./login.html", "./index.html")
	r.GET("/login", func(c *gin.Context) {
		c.HTML(http.StatusOK, "login.html", nil)

	})
	//處理lgoin post的請求
	r.POST("/login", func(c *gin.Context) {
		//獲取form表單提交的數據
		username := c.PostForm("username")
		//取到返回值 取不到就返回空
		password := c.PostForm("password")
		// username := c.DefaultPostForm("username", "erroruser")
		// password := c.DefaultPostForm("password", "123456")
		c.HTML(http.StatusOK, "index.html", gin.H{
			"Name":     username,
			"Password": password,
		})
	})
	r.Run()
}
```

html

```html
<form action="/login" method="post" novalidate autocomplete="off">
      <div>
      <label for="username">Useranme:</label>
      <input type="text" name="username" id="username">
    </div>
    <div>
      <label for="password">Password:</label>
      <input type="password" name="password" id="password">
    </div>
      <input type="submit" value="登錄">
    </form>
```

### 獲取path參數

```go
func main() {
	r := gin.Default()
	r.GET("/user/:name/:age", func(c *gin.Context) {
		//獲取路徑參數
		name := c.Param("name")
		age := c.Param("age")
		c.JSON(http.StatusOK, gin.H{
			"name": name,
			"age":  age,
		})
	})
	r.GET("/blog/:year/:month/", func(c *gin.Context) {
		year := c.Param("year")
		month := c.Param("month")
		c.JSON(http.StatusOK, gin.H{
			"year":  year,
			"month": month,
		})
	})
	r.Run(":8080")
}
```

### GIN參數綁定

使用ShouldBind()可以接收form json get請求 不需要其他額外的工作

```go
//獲取url path
func main() {
	r := gin.Default()
	r.GET("/user", func(c *gin.Context) {
		// username := c.Query("username")
		// password := c.Query("password")
		// u := UserInfo{
		// 	Username: username,
		// 	Password: password,
		// }
		var u UserInfo //聲明一個UserInfo類型的變量u
		err := c.ShouldBind(&u)
		if err != nil {
			c.JSON(http.StatusBadRequest, gin.H{
				"message": "error",
				"error":   err.Error(),
			})
		} else {
			fmt.Printf("%#v\n", u)
			c.JSON(http.StatusOK, gin.H{
				"message": "ok",
			})
		}
	})
	r.POST("/json", func(c *gin.Context) {
		var u UserInfo //聲明一個UserInfo類型的變量u
		err := c.ShouldBind(&u)
		if err != nil {
			c.JSON(http.StatusBadRequest, gin.H{
				"message": "error",
				"error":   err.Error(),
			})
		} else {
			fmt.Printf("%#v\n", u)
			c.JSON(http.StatusOK, gin.H{
				"message": "ok",
			})
		}
	})
	r.Run(":8080")
}
```

### 文件上傳



html

```html
<!DOCTYPE html>
<html lang="en" dir="ltr">
  <head>
    <meta charset="utf-8">
    <title></title>
  </head>
  <body>
    <h1>單文件上傳</h1>
    <form action="/upload" method="post" enctype="multipart/form-data">
      <input type="file" name="f1">
      <input type="submit" value="上傳">
    </form>
    <hr>
    <h1>多文件上傳</h1>
    <form action="/multiupload" method="post" enctype="multipart/form-data">
      <input type="file" name="f1" multiple>
      <input type="submit" value="上傳">
    </form>
  </body>
</html>

```

#### 單文件上傳

main.go

```go
func main() {
	r := gin.Default()
	r.LoadHTMLFiles("./index.html")
	r.GET("/index", func(c *gin.Context) {
		c.HTML(http.StatusBadRequest, "index.html", nil)
	})

	r.POST("/upload", func(c *gin.Context) {
		//從請求中讀取文件

		f, err := c.FormFile("f1")
		if err != nil {
			c.JSON(http.StatusBadRequest, gin.H{
				"Error": err.Error(),
			})
		} else {
			//將讀取到的文件保存在本地(伺服器端本地)
			// filepath:=fmt.Sprintf("./%s",f.Filename)
			filepath := path.Join("./data/", f.Filename)
			c.SaveUploadedFile(f, filepath)
			c.JSON(http.StatusOK, gin.H{
				"message": "ok",
			})
		}
	})
	r.Run(":8080")
}
```

#### 多文件上傳

main.go

```go
func main() {
	r := gin.Default()
	r.LoadHTMLFiles("./index.html")
	r.GET("/index", func(c *gin.Context) {
		c.HTML(http.StatusBadRequest, "index.html", nil)
	})

r.POST("/multiupload", func(c *gin.Context) {
		form, _ := c.MultipartForm()
    //文件name屬性
		files := form.File["f1"]

		for index, file := range files {

			dst := fmt.Sprintf("./data/%s_%d", file.Filename, index)
			log.Println(dst)
			// 上传文件到指定的目录
			c.SaveUploadedFile(file, dst)

		}
		c.JSON(http.StatusOK, gin.H{
			"message": "ok",
		})
	})
```

### 重定向

#### HTTP重定向

```go
r.GET("/soguo"
```



#### 路由重定向

```go
//重定向與請求轉發
func main() {
	r := gin.Default()
	r.LoadHTMLFiles("./index.html")
	r.GET("/sogo", func(c *gin.Context) {
		c.Redirect(http.StatusMovedPermanently, "http://www.sogo.com/")
	})
	r.GET("/a", func(c *gin.Context) {
		//跳轉到/b對應的路由處理函數
		c.Request.URL.Path = "/b" //把請求的url修改
		r.HandleContext(c)        //繼續後續的請求
	})
	r.GET("/b", func(c *gin.Context) {
		c.JSON(http.StatusOK, gin.H{
			"message": "b",
		})
	})
	r.Run(":8080")
}

```

### GIN路由

#### Any

使用Any可以處理所有類型的請求併 另外根據類型來處理

```go
r.Any("/index",func(c *gin.Context){
	switch c.Request.Method{
		case "GET":
		c.JSON(http.StatusOK,gin.H{"message":"GET"})
		case http.MethodPost:
        c.JSON(http.StatusOK,gin.H{"meassage":"POST"})
	
	}
})
```

#### NoRoute

當用戶訪問不存在的路由時

```go
//no route
	r.NoRoute(func(c *gin.Context) {
		c.JSON(http.StatusNotFound, gin.H{
			"message": "404",
		})
	})
```

#### 路由組

用來區分不同的業務線或是不同版本

```go
//路由組
//視頻的首頁和詳情頁
	videoGroup := r.Group("/video")
	{
		videoGroup.GET("/index", func(c *gin.Context) {
			c.JSON(http.StatusOK, gin.H{
				"msg": "/video/index",
			})
		})
		videoGroup.GET("/detail", func(c *gin.Context) {
			c.JSON(http.StatusOK, gin.H{
				"msg": "/video/detail",
			})
		})
	}
//商城的首頁和詳情頁
	shopGroup := r.Group("/shop")
	{
		shopGroup.GET("/index", func(c *gin.Context) {
			c.JSON(http.StatusOK, gin.H{
				"msg": "/shop/index",
			})
		})
		shopGroup.GET("/detail", func(c *gin.Context) {
			c.JSON(http.StatusOK, gin.H{
				"msg": "/shop/detail",
			})
		})
	}
```

路由組也支持嵌套

```go
shopGroup := r.Group("/shop")
	{
		shopGroup.GET("/index", func(c *gin.Context) {
			c.JSON(http.StatusOK, gin.H{
				"msg": "/shop/index",
			})
		})
		shopGroup.GET("/detail", func(c *gin.Context) {
			c.JSON(http.StatusOK, gin.H{
				"msg": "/shop/detail",
			})
		})
        cloth:=shopGroup.Group("/cloth")
        {	//==/shop/cloth/index
            cloth.GET("/index",func(c *gin.Context{...})
        }
	}
```

### GIN中間件

又稱為鉤子函數 框架允許開發者在處理請求的過程中 加入自己的鉤子函數

例如 登錄認證 權限驗證 數據分頁 紀錄日誌 耗時統計

gin的中間件必須是一個`gin.HandlerFunc`

定義中間件

```go
//定義一個中間件m1
func m1(c *gin.Context) {
	fmt.Println("m1 in ....")
	start := time.Now()
	c.Next() //調用後續的處理函數
	// c.Abort()//阻止調用後續的處理函數
	cost := time.Since(start)
	fmt.Printf("cost:%v\n", cost)
}
```

`c.Next()`:

`c.Abort`:



一個函數增加中間件

```go
r.GET("/index", m1, indexHandler)
```

全局調用中間件

```go
func main() {
	r := gin.Default()
	r.Use(m1) //全局註冊中間件函數
	//GET(relativePath string, handlers ...HandlerFunc)
	r.GET("/index", indexHandler)
	r.GET("/shop", func(c *gin.Context) {
		c.JSON(http.StatusOK, gin.H{
            "message":"shop",
        })
	})
	r.Run(":8080")
}
```

為路由組插入中間件

```go
shopGroup := r.Group("/shop")
shopGroup.Use(StatCost())
{
    shopGroup.GET("/index", func(c *gin.Context) {...})
    ...
}
```

## GORM

```go
import (
	"fmt"

	"github.com/jinzhu/gorm"
	_ "github.com/jinzhu/gorm/dialects/mysql"
)

type UserInfo struct {
	ID     uint
	Name   string
	Gender string
	Hobby  string
}

func main() {
	db, err := gorm.Open("mysql", "root:12345678@(192.168.56.35)/mydb?charset=utf8mb4&parseTime=True&loc=Local")
	defer db.Close()

	if err != nil {
		panic("fail to connect database")
	}
	//自動遷移 把結構體和數據表進行對應
	db.AutoMigrate(&UserInfo{})

	// db.Create(&UserInfo{1, "七米", "Male", "籃球"})
	var u UserInfo
	db.First(&u)
	fmt.Printf("u%#v", u)

	//更新
	db.Model(&u).Update("hobby", "足球")
	db.First(&u)
	fmt.Printf("u%#v", u)

	//刪除
	db.Delete(&u)
}
```

#### SingularTable(true)

禁用表名的複數形式

#### 指定field名稱

在定義結構體時，tag加入`gorm:"column:column_name"`

如此一來，欄位名稱就可以自訂為`column_name`

#### 時間戳

* CreateAt
* UpdateAt
* DeleteAt

#### 設定默認值

通過tag創建預設值

```
Name string `gorm:"default:'小王子'"`
```

### GORM查詢語句

```go
// 获取第一个匹配的记录
db.Where("name = ?", "jinzhu").First(&user)
//// SELECT * FROM users WHERE name = 'jinzhu' limit 1;

// 获取所有匹配的记录
db.Where("name = ?", "jinzhu").Find(&users)
//// SELECT * FROM users WHERE name = 'jinzhu';

// <>
db.Where("name <> ?", "jinzhu").Find(&users)
//// SELECT * FROM users WHERE name <> 'jinzhu';

// IN
db.Where("name IN (?)", []string{"jinzhu", "jinzhu 2"}).Find(&users)
//// SELECT * FROM users WHERE name in ('jinzhu','jinzhu 2');

// LIKE
db.Where("name LIKE ?", "%jin%").Find(&users)
//// SELECT * FROM users WHERE name LIKE '%jin%';

// AND
db.Where("name = ? AND age >= ?", "jinzhu", "22").Find(&users)
//// SELECT * FROM users WHERE name = 'jinzhu' AND age >= 22;

// Time
db.Where("updated_at > ?", lastWeek).Find(&users)
//// SELECT * FROM users WHERE updated_at > '2000-01-01 00:00:00';

// BETWEEN
db.Where("created_at BETWEEN ? AND ?", lastWeek, today).Find(&users)
//// SELECT * FROM users WHERE created_at BETWEEN '2000-01-01 00:00:00' AND '2000-01-08 00:00:00';

```

### 多對多關聯

models

建立多對多關聯

```go 
type Book struct {
	gorm.Model
	Title   string
	Price   int16
	Authors []Author `gorm:"many2many:books_authors"`
}

type Author struct {
	gorm.Model
	Name  string
	Books []Book `gorm:"many2many:books_authors"`
}
```

創建資料

```go
b1 := Book{
    Title: "老人與海", 
    Price: 220, 
    Authors: []Author{
        {Name: "海明威"},
        {Name: "小鯨魚"}
    }
}

```

根據外鍵搜尋

```go
b := []Book{}
//將Authors載入加入查找
err := DB.Preload("Authors").Find(&b).Error
```

選擇字段輸出

```go
b := []Book{}
DB.Select("title,price").Find(&b).Error
```


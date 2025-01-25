# Go Web

Hello web

```go
package main

import (
	"fmt"
	"net/http"
)

func main() {

	http.HandleFunc("/", handler)
	http.ListenAndServe(":8080", nil)
}

func handler(w http.ResponseWriter, r *http.Request) {
	fmt.Fprintln(w, "Hello World", r.URL.Path)
}

```

自行創建handler

```go
package main

import (
	"fmt"
	"net/http"
)

type MyHandler struct{}

func (m *MyHandler) ServeHTTP(w http.ResponseWriter, r *http.Request) {
	fmt.Fprintln(w, "通過自己創建的處理器處理請求!")

}

func main() {
	myHandler := MyHandler{}
	http.Handle("/myHandler", &myHandler)
	http.ListenAndServe(":9000", nil)
}

```

### type Server

```go 
type Server struct{
  
}
```

### type Request

```go
type Request struct{

Form

}
```

#### HTTP request header

| Header filed   | Description                                                  |
| -------------- | ------------------------------------------------------------ |
| Accept         | 客戶端可以接受的HTTP回應的部分。例如：text/html 告訴伺服器，客戶端想要的回應體內容類型在HTML中 |
| Accept-Charset | 從伺服器返回必要的字符集                                     |
| Authorization  | 傳送基本驗證訊息到伺服器                                     |
| Cookie         | 客戶端必須回傳響應的伺服器設置的cookie，多個cookie透過;隔開  |
| Content-Length | 請求體的內容長度                                             |
| Content-Type   | The content type of the request body (when there’s a request body). When aPOST or a PUT is sent, the content type is by default x-www-form-urlencoded.But when uploading a file (using the HTML input tag with the type attribute set to file, or otherwise) the content type should be multipart/form-data. |
| Host           | 伺服器的名稱 除了port號                                      |
| Referrer       | 連接至請求的頁面的前一個頁面                                 |
| User-Agent     | 呼叫的客戶端描述                                             |



Request.Form可以進行操作

```go
v := url.Values{}
v.Set("name", "Ava")
v.Add("friend", "Jess")
v.Add("friend", "Sarah")
v.Add("friend", "Zoe")
// v.Encode() == "name=Ava&friend=Jess&friend=Sarah&friend=Zoe"
fmt.Println(v.Get("name"))
fmt.Println(v.Get("friend"))
fmt.Println(v["friend"])

```

獲取請求方法

r.Method

獲取URL

r.URL.Path

r.URL.RawQuery

獲取請求頭

r.Header.Get("Accept-Encoding")

獲取請求體長度

length:=r.ContentLength

body:=make([]byte,length)

r.Body.Read(body)

fmt.Fprintln(w,body)

#### 獲取請求參數

執行r.Form()之前先使用r.ParseForm才會獲得表單內的參數

r.PostForm只接受表單內的屬性

如果表單enctype的屬性值為"multipart/form-data"，需要使用r.MultipartForm字段來獲取 

### FormValue和PostFormValue方法

1. FormValue方法

   會自動調用ParseForm，不需要手動調用

2. 類似FormValue 但只取得表單中的屬性值

### HTTP 響應

回復響應使用w.Write([]byte,"你的請求收到了")

回應Json報文

  w.Header().Set("Content-Type","application/json")

設置重定向

w.Header().Set("Location","https://www.baidu.com")

設置響應頭必須在w.Write([]byte)之前

### 渲染模板

html/template

func (t \*Template ParseFiles(filename))

func (t \*Template)ParseFile(pattern string)(*Template,error)

func (t \*Template)Execute(wr io.Write,data interface{})

func (t \*Template)ExecTemplate(wr io.Write,name string,data interface{})

### 處理靜態文件

1. StripPrefix(prefix string,h Handler)Handler:

StripPrefix會處理請求的URL去調prefix

1. FileServer

```go
http.Handle("/static/", http.StripPrefix("/static/", http.FileServer(http.Dir("./static/"))))
```

/static/會匹配以/static開頭的路徑 當瀏覽器請求index.html葉面中的style.css文件時，static前綴會被替換為http.Dir()中的目錄去尋找css/style.css

### 防止XSS

如果不做任何防護，直接將用戶提交的`<script>alert()</script>`回傳到用戶頁面上，就會讓script在用戶瀏覽器中執行

![image-20200602221849501](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200602221849501.png)

```go
script := template.HTMLEscapeString(r.Form.Get("username"))

fmt.Fprintf(w, script)
```

將用戶傳入的script使用`template.HTMLEscapeString()`處理以後，就可以防止script運行![image-20200602222053459](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200602222053459.png)

如果想要輸出原型

```go
script := r.Form.Get("username")//取得<script>alert("haha")</script>
t, _ := template.New("foo").Parse(`{{define "T"}}Hello,{{.}}{{end}}`)//使用``將字符刮起來
t.ExecuteTemplate(w, "T", template.HTML(script))
```

### 驗證表單的輸入

`r.Form.Get()`取出數據較好，透過`r.Form["username"][0]`可能取出空值，會報錯



```go
if r.Method == "POST" {
		r.ParseForm()

		//處理username
		fmt.Fprintln(w, r.Form.Get("username"))

		//處理年齡 只接收數字
		age, err := strconv.Atoi(r.Form.Get("age"))
		if err != nil {
			w.Write([]byte(err.Error()))
		} else {
			fmt.Fprintln(w, string(r.Form.Get("age")))
			fmt.Println("年齡", age)
		}
		//處理姓名 要是漢字
		if m, _ := regexp.MatchString("^\\p{Han}+$", r.Form.Get("name")); !m {
			fmt.Fprintf(w, "姓名須為全漢字\n")
		}

		//處理email
		if m, _ := regexp.MatchString(`^([\w]+\.{0,1}[\w]+)@(\w{1,})\.([a-z]{2,4})$`, r.Form.Get("email")); !m {
			fmt.Println("email not match")
		} else {
			fmt.Println("email match")
		}

		//處理下拉選單
		slice := []string{"taipei", "taichung", "kaohsiung"}
		v := r.Form.Get("bornfrom")
		fmt.Println(v)
		for _, item := range slice {
			if item == v {
				fmt.Println("出生地", "true")
			}
		}
		//處理單選框跟下拉選單邏輯一樣
		fmt.Println("興趣: ")
		interest := r.Form["interest"]
		for _, v := range interest {
			fmt.Println(string(v))
		}
	}
```

### 防止多次提交

```html
<input type="checkbox" name="interest" value="football">足球
<input type="checkbox" name="interest" value="basketball">篮球
<input type="checkbox" name="interest" value="tennis">网球    
用户名:<input type="text" name="username">
密码:<input type="password" name="password">
<input type="hidden" name="token" value="{{.}}">
<input type="submit" value="登录">
```

透過將一個token存儲到伺服器端的session中 使用md5來設定時間戳

### 文件上傳

1. 在表單form 的屬性中增加

   ```html
   <form enctype="multipart/form-data" action="/upload" method="post">
     
   </form>
   ```

2. 使用`r.ParseMultipartForm`來解析表單，將上傳的文件存在臨時文件中

3. 透過`r.FormFile("uploadfile")`獲取上傳的文件及文件指針

   `r.FormFile`獲取的handler如以下結構

   ```go
   type FileHeader struct {
       Filename string
       Header   textproto.MIMEHeader
       // contains filtered or unexported fields
   }
   ```



```go
if r.Method == "POST" {
		r.ParseMultipartForm(32 << 20)
		file, handler, err := r.FormFile("uploadfile")
		if err != nil {
			fmt.Println(err)
			return
		}
		defer file.Close()
		fmt.Fprintf(w, "%v", handler.Header)
		f, err := os.OpenFile("./test/"+handler.Filename,
			os.O_WRONLY|os.O_CREATE, 0666)
		if err != nil {
			fmt.Println(err)
			return
		}
		defer f.Close()
		io.Copy(f, file)
}
```

### Session的處理

session透過cookie將訪客的資訊記錄起來，以便將來使用。

例如，會將使用者名稱記錄起來，讓使用者不用反覆地進行驗證。



我們使用一個管理器來管理session

```go
type Manager struct {
	cookieName  string     //cookie標籤
	lock        sync.Mutex //鎖 用在修改數據時
	provider    Provider   //數據存儲的結構
	maxLifeTime int64      //最大存活時間
}
```

管理器可以透過這個方法來初始化

```go
func NewManager(provideName, cookieName string, maxLifeTime int64) (*Manager, error) {
	provider, ok := provides[provideName]
	if !ok {
		return nil, fmt.Errorf("session: unknown provide %q(forgotten import?)", provideName)
	}
	return &Manager{provider: provider,
		cookieName: cookieName, maxLifeTime: maxLifeTime}, nil
}
```


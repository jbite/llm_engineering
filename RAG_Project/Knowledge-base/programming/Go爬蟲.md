# Go爬蟲

## 解決編碼問題

* 安裝兩個包

```
go get golang.org/x/text
go get golang.org/x/net/html
```

程式碼

```go 
package main

//安裝golang.org/x/text
//安裝golang.org/x/net/html

import (
	"bufio"
	"fmt"
	"io/ioutil"
	"log"
	"net/http"

	"golang.org/x/net/html/charset"
	"golang.org/x/text/encoding"
	"golang.org/x/text/transform"
)

func main() {
  //爬取指定網站內容
	resp, err := http.Get("http://www.chinanews.com/")
	if err != nil {
		panic(err)
	}
//在完成爬蟲之後將讀取內容關閉
	defer resp.Body.Close()

	if resp.StatusCode != http.StatusOK {
		fmt.Println("Error status code: %v", resp.StatusCode)
	}
//創建一個讀取器來讀取網頁內容
	bodyReader := bufio.NewReader(resp.Body)
 //使用determineEncoding來判斷編碼器
	e := determineEncoding(bodyReader)
 //將內容使用新的編碼器重新編碼
	utf8Reader := transform.NewReader(bodyReader, e.NewDecoder())
//將編碼完成的內容輸出
	result, err := ioutil.ReadAll(utf8Reader)
	if err != nil {
		panic(err)
	}
	fmt.Printf("%s", result)
}
//用來處理各種編碼的函數，回傳編碼類型
func determineEncoding(r *bufio.Reader) encoding.Encoding {
	bytes, err := r.Peek(1024)
	if err != nil {
		log.Printf("fetch error:%v\n", err)
	}
	e, _, _ := charset.DetermineEncoding(bytes, "")
	return e
}
```

### 正規表示法獲取內容


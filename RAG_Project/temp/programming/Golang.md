# Golang

google的一種系統編程語言

具有內置的垃圾收集機制

支持高併發

代碼可以編譯成一個可執行二進制文件 不需要添加庫或運行時環境即可在服務器上執行

## GoLang執行流程分析

```
.go文件--go build--->可執行文件--運行-->結果

.go文件--go run----->結果
```

1）如果我們先編譯生成了可執行文件，那我們可以將該可執行文件拷貝到沒有go開發環境的機器上，仍然可以運行。

2）如果我們直接go run go 源代碼，那麼如果要在另一個機器上這麼運行，也需要go開發環境，否則無法執行。

3）編譯器會將程式運行依賴的庫文件包含在可執行文件中，所以，可執行文件變大了很多。

### 字串處理

字串一旦賦值，就不能修改

自訂分隔符

```go
strings.Split("a,b,c",",")
[a b c]
```

使用空白及換行當作分隔符

```go
strings.Fields("a \t b \t c \n")
```

#### 字符串比較

go 1.3以前

 相等返回0

不等返回-1 +1

首先比較第一個字母 左邊小於右邊-1 左邊大於右邊+1

a<b<c

```go
func main() {
    fmt.Println(strings.Compare("a", "b"))
    fmt.Println(strings.Compare("b", "a"))
    fmt.Println(strings.Compare("a2", "b1"))
    fmt.Println(strings.Compare("c", "b"))
    fmt.Println(strings.Compare("a", "a"))
}

-1
1
-1
1
0
```

go 1.10以後

```go
func main() {
    fmt.Println("a" > "b")
    fmt.Println("c" > "b")
    fmt.Println("d" > "c")

}

func main1() {
    fmt.Println(strings.Compare("a", "b"))
    fmt.Println(strings.Compare("b", "a"))

}
```

#### 字串查找

```
b := strings.Contains("seafood", "foo")
fmt.Println(b)
```

#### 字符串轉換

1. ```
    str = fmt.Sprintf("%d", num1)
    fmt.Printf("str type %T, str=%q\n", str, str)
   ```
2. #### FormatInt進位治轉換
   
   ```
   str := "123"
   a, _ := strconv.Atoi(str)
   fmt.Printf("字串的%s的二進制為%v", str, strconv.FormatInt(int64(a), 2))
   ```

#### 字符串的拼接

當一個拼接字符串很長時，可以寫成一串，但需要把+號留在前面一行

```
var str = "hello " + "world"
str += "hahaha!"

var str4 = "hello" +
"world" + "hello" +
"hello"
```

#### 字符串轉基本數據類型

```
    var str string = "true"
    var b bool
    b, _ = strconv.ParseBool(str)
    fmt.Printf("b type %T, b = %v\n", b, b)

    var str2 string = "1234560"
    var i int64

    i, _ = strconv.ParseInt(str2, 10, 64)
    fmt.Printf("i type %T, i=%v\n", i, i)

    var str3 string = "123.456"
    var f1 float64

    f1, _ = strconv.ParseFloat(str3, 64)
    fmt.Printf("f1 type is %T, F1 is %v", f1, f1)
```

如果將錯誤的字符串值傳到ParseInt中，則會將0傳回。

## Go程式結構組成結構

package

//導入模組

import . "fmt"

//定義常量

const PI = 3.14

//一般類型聲明

type newType int

//結構的聲明

type gopher struct()

//接口的聲明

type golang interface()

//由main函數作為程序入口點啟動

func main() {

   Println("Hello world")

}

## 變量與常量

### 變量聲明

* 未初始化的標準格式
  
  ```
  var vname
  ```

* 未初始化的批量格式
  
  ```
  var (
    a int
    b string
    c []float32
    d func() bool
    e struct {
       x int
       y string
    }
  )
  ```

* int 及float32類型初始值為0，string初始值為空

* 也可以在賦值的時候動態決定類型
  
  ```
  var f = 100
  ```

* 變量簡短聲明格式
  
  ```
  * 變量名 := 表達示
  * := 可以高效的創建一個新的變量 稱之為初始化聲明
  * 聲明語句省略了var關鍵字
  * 變量類型將由編譯器自動推斷
  * 只能用在函數體內，不能用在全局變量的聲明與賦值
  ```

* "_"匿名變量: 某些變量不占用命名空間 不會分配內存

* 多重賦值功能
  
  ```
  x := 10
  y := 20
  z := 30
  
  x, y, z = y, z, x
  20, 30, 10
  ```

### 數據類型

基本數據類型:

整形: 

* int8 : -128~128
* int16: -32768~32767
* int32: -2147483648~2147483647
* int64
* uint8 0-255 2^8
* uint16 0-65535 2^16
* uint32 0-2^32
* uint64
* rune: 類似int32 用來處理字符串
* byte: 長度等於uint8 也是用來處理字符串
* uintptr: 存放一個指針

複合數據類型: 指針 數組 切片 映射 函數 結構體 通道

字符串單行使用""包起來

如果要跨行，必須使用``反引號刮起來

#### 打印格式化

%v: 值的默認格式表示 打印值

%+v: 列印時 會添加字段名

%#v 值的Go語法表示

%T 值的類型Go語法表示

布爾值

%t 單詞true或false

整數

%b 二進制

%c unicode的碼值

%d表示為十進制

%8d表示整形長度是8 不足8位 則補上空格 如果超過8位 以實際為準

%08d 數字長度8 不足8位的前方補上0

%o 表示八進制

%q 該值對應的單引號刮起來的go語法字符字面值 

%x 十六進制

%X 十六進制使用A-F

%U 表示為unicode格式

浮點數

%b 無小數部分 二進制指數的科學計數法

%e (=%.6e) 有6位小數部分的科學計數法1234.456e+78

%E

%f (=%.6f)有6位小數部分 如123.456123

%F 等價於%F

字符串和[]byte

%s 直接輸出

%c 打印字符

%% 打印%這個符號

使用`Sprintf`可以產生返回的字符串

#### 基本數據類型轉換字串

* 方法一：
  
  ```
    var num1 int = 99
    var num2 float64 = 23.456
  
    var b bool = true
    var str string
    str = fmt.Sprintf("%d", num1)
    fmt.Printf("str type %T, str=%q\n", str, str)
  
    str = fmt.Sprintf("%f", num2)
    fmt.Printf("str type %4T, str=%q\n", str, str)
  
    str = fmt.Sprintf("%t", b)
    fmt.Printf("str type %T, str=%q\n", str, str)
  ```

* 方法二：
  
  ```
    var num3 int = 99
    var num4 float64 = 23.456
    var b2 bool = true
  
    str = strconv.FormatInt(int64(num3), 10)
    fmt.Printf("str type %T str = %q\n", str, str)
  
    str = strconv.FormatFloat(num4, 'f', 10, 64)
    fmt.Printf("str type %T str = %q\n", str, str)
  
    str = strconv.FormatBool(b2)
    fmt.Printf("str type %T str = %q\n", str, str)
  ```

```
* 方法三
```

Strvonc.Itoa(int)

```
### iota建立常數組

＊ 常數變量的作用域 定義在函數內，僅當前函數可見。

定義在函數外，首字母小寫(私有成員)包內所有源文件可見

​                           首字母大寫(公有成員)所有包可見

iota 表示0
```

/* 自動沿用 排頭兵的表達式逐一遞增*/
const (
    USA = (iota + 1) * 1000
    China
    Russia
    British
    France
)

func main() {
    fmt.Println(USA, China, Russia, British, France)

}

```
## go指針
1. 基本數據類型，變量存的就是值，也叫值類型
2. 獲取變量的地址，用&，比如：var num int, 獲取num的地址： &num
3. 指針類型，變量類型存的是一個地址，這個地址指向的空間存的才是值 比如var ptr *int = &num
4. 獲取指針類型所指向的值，使用： *，比如：var *ptr int, 使用*ptr獲取p指向的值

## 標識符概念
## 流程控制

### 順序結構

### 選擇結構

```go
switch weekday {
    case Monday:
        fmt.Printf("Monday\n")
    case Tuesday:
         fmt.Println("Tuesday")
    case Wednesday:
         fmt.Println("Wednesday")
    case Thursday, Friday:
         fmt.Println("Thursday")
    case Saturday:
         fmt.Println("Saturday")
    case Sunday:
         fmt.Println("Sunday")
    default:
         fmt.Println("Wrong input")
     }
```

```
var age = 20
    switch {
    case age < 18:
        fmt.Println("你是一個少年")
    case age >= 18 && age < 36:
        fmt.Println("你是一個青年")
    default:
        fmt.Println("你是一個傳奇")
    }
```

#### switch及fallthrough

```go
switch{
case score>90:
    fmt.Println("得到妹子")
    fallthrough
case score>80:
    fmt.Println("得到書包")
    fallthrough
case score>70:
    fmt.Println("得到書籤")

}
```

### 循環結構

有限循環

```go
for i:= 0 i< 10;i++{
    //循環內容...
}
```

```go
for i < 20{
    //循環內容
}
```

無限循環

```go
for{
//循環內容
}
```

###g

### 命令行參數

#### 參數輸入方法

##### 方法一: os.Args

```go
import "os"
//.\demo01_命令行參數輸入.exe -name=jacky -age=44 -rmb 1.2345 -alive=false
//將參數依照切片類型輸入
func main(){
        for i, v := range os.Args {
        fmt.Println(i, v)
    }
}
```

##### 方法二: os.flags

```go
import "os"
//.\demo01_命令行參數輸入.exe -name=jacky -age=44 -rmb 1.2345 -alive=false
func main(){
    namePtr := flag.String("name", "無名氏", "姓什名誰")
    agePtr := flag.Int("age", -1, "閣下的年齡")
    rmbPtr := flag.Float64("rmb", 1.0, "你的資產")
    alivePtr := flag.Bool("alive", true, "是否建在")
    //參數取得
    flag.Parse()
    fmt.Println(*namePtr, *agePtr, *rmbPtr, *alivePtr)
}
```

```go
func main(){
  var name string
  var age int
  var rmb float64
  var alive bool
//參數名 參數預設值 提示
  flag.StringVar(&name,"name","無名氏","性什名誰")
  flag.IntVar(&age,"age", -1, "閣下的年齡")
    flag.Float64Var(&rmb,"rmb", 1.0, "你的資產")
    flag.BoolVar(&alive,"alive", true, "是否建在")

  flag.Parse()
  fmt.Println(name,age,rmb,alive)
}
```

### 面向對像

#### 封裝

將業務相近的變量、函數封裝為一個結構體(類)

* 變量(variable) -> 屬性(field)
* 函數(function)->方法(method)

化繁為簡，減少直接管理的成員數，便於做大規模開發

##### 封裝一個結構體(類)

```go
type Person struct{
    //封裝結構體的屬性
    name string
    age int
    sex bool
    hobby []string
}
/* 封裝結構體的方法
- 無論方法得主與定義為值類型還是指針類型 對像值和對像指針都能夠正常訪問
- 通常會將主語定義為指針類型 畢竟西門赫的副本吃了飯 肉不會漲到西門赫本人身上去
*/
func (p *Person)Eat() {
    fmt.Printf("%s愛營養\n", p.name)
}
func (p *Person)Drink(){
    fmt.Printf("%s愛喝酒\n",p.name)
}

func (p *Person)Love(){
    fmt.Printf("%s是有感情的\n",p.name)
}

MakehimLove(p Person){
    p.Love()
}
MakehimPtrLove(p *Person){
    p.Love()
}
```

##### 創建對像實例

```go
func main() {
    Jacky := Person{}
    Jacky.name = "Jacky"
    Jacky.hobby = append(Jacky.hobby, "Linux", "Python", "Golang")
    Jacky.Eat()
    Jacky.Drink()
    Jacky.Love()
}
```

```go
func main() {
    Jacky := &Person{name:"Jacky",age:20, sex: true,hobby:[]string{"Linux","GoLang"}}
    Jacky.Eat()
    Jacky.Drink()
    Jacky.Love()
}
```

```go
func main(){
    //要求傳遞值就必須傳遞值
    MakeHimLove(Jacky)
    //要求傳遞指針就必須傳遞指針
    MakeHimPtrLove(&Jacky)
}
//值傳遞的是副本，引用傳遞才是真身
```

#### 繼承

將公共的部分提取到父類，減少重複代碼

繼承的目的是為了發展 

* 增加新的屬性和方法 
* 修改父類屬性和方法

##### 子類的封裝

```go
package main

import "fmt"
//Person類的封裝 Person類是父類
type Person struct {
    name  string
    age   int
    sex   bool
    hobby []string
}
//父類方法
func (p *Person) Eat() {
    fmt.Printf("%s愛營養\n", p.name)
}
func (p *Person) Drink() {
    fmt.Printf("%s愛喝酒\n", p.name)
}

func (p *Person) Love() {
    fmt.Printf("%s是有感情的\n", p.name)
}

/* 封裝子類，堆代碼的 */
type coder struct {
    //持有一個父類聲明 繼承了person
    Person
    //會的語言
    langs []string
}
//定義子類方法 調用父類屬性及子類屬性
func (c *coder) Code() {
    fmt.Printf("%s會%v,他正在堆代碼", c.name, c.langs)
}
```

##### 創建子類實例

```go
func main() {
    //新增一個物件實例 返回的是pointer
    Jacky := new(coder)
    Jacky.name = "Jacky"
    Jacky.langs = []string{"GO", "Python", "閩南話"}
    //調用父類方法
    Jacky.Drink()
    //調用子類方法
    Jacky.Code()
}
```

##### 子類修改父類方法

```go
type driver struct {
    Person

    licenseID string
    isDriving bool
}

func (d *driver) Drink() {
    if !d.isDriving {
        fmt.Printf("%s愛喝酒", d.name)
    } else {
        fmt.Printf("fuckoff,司機一滴酒 親人兩行淚")
    }
}
func main811() {
    Jacky := new(coder)
    Jacky.name = "Jacky"
    Jacky.langs = []string{"GO", "Python", "閩南話"}
    Jacky.Drink()
    Jacky.Code()
}

func main() {
    Jacky := new(driver)
    Jacky.name = "Jacky"
    fmt.Printf("%s在開車嗎?true/false", Jacky.name)
    fmt.Scan(&Jacky.isDriving)
    Jacky.Drink()
}
```

##### 接口的繼承

```go
type Animal interface {
    //新陳代謝
    Eate(food string) (shit string)
    Die()
}

type Fighter interface {
    Attack() (bloodLoss int)
    Defend()
}

//顯式繼承
//野獸接口 擁有動物的 一切特徵
//野獸接口 擁有戰士的一切特徵
type Beast interface {
    //野獸接口繼承動物接口
    Animal
    //野獸接口繼承鬥士接口
    Fighter
    Run()
}

//隱式繼承
type Beast interface {
    Animal
    //隱式繼承鬥士接口，沒有明確的說繼承鬥士，但事實上定義了其全部抽像方法
    Attack(bloodLoss int)
    Defend()

    Run()
}
```

#### 多態

* 一個父類有多種不同的具體子類型態
* 共性: 通過父類方法去調度子類實例
* 各性: 不同子類對父類方法的具體實現各不相同 

```go
package main
/* 
接口: 只有方法的定義，沒有實現
實現接口: 結構體實現接口的全部抽象方法，就稱為結構體實現了接口
多態: 一個父類/接口有不同的子類實現，本例中【勞動者】接口的具體實現有coder PM  boss
共性: 【程序員】【PM】【老闆】都會勞動和休息
個性: 【程序員】【PM】【老闆】勞動和休息方式各不相同
*/
import (
    "fmt"
    "math/rand"
    "time"
)

/*勞動者父類接口 內含抽像方法: 工作 休息*/
type Worker interface {
    Work(hour int) (product string)

    Rest()
}
type Coder struct {
    skill string
}

func (c *Coder) Work(hour int) (product string) {
    fmt.Printf("碼農一天公作%d小時\n", hour)
    fmt.Printf("碼農正在%s\n", c.skill)
    return "BUGS"
}

func (c *Coder) Rest() {
    fmt.Println("休息是什麼?")
}

func (c *Coder) WorkHome() {
    fmt.Printf("程序員在家工作")
}

type ProductManager struct {
    skill string
}

func (pm *ProductManager) Work(hour int) (product string) {
    fmt.Printf("PM一天公作%d小時\n", hour)
    fmt.Printf("PM正在%s\n", pm.skill)
    return "Product"
}
func (pm *ProductManager) Rest() {
    fmt.Println("PM不會累")
}

type Boss struct {
    skill string
}

func (b *Boss) Work(hour int) (product string) {
    fmt.Printf("Boss一天公作%d小時\n", hour)
    fmt.Printf("Boss正在%s\n", b.skill)
    return "Income"
}

func (b *Boss) Rest() {
    fmt.Println("Boss不會累")
}

func main() {
    workers := make([]Worker, 0)

    workers = append(workers, &Coder{"嚕代碼"})
    workers = append(workers, &ProductManager{"拍腦門"})
    workers = append(workers, &Boss{"吹牛逼"})

    r := rand.New(rand.NewSource(time.Now().UnixNano()))
    weekday := r.Intn(7)

    fmt.Printf("今天是星期%d\n", weekday)

    if weekday > 0 && weekday < 6 {
        //全體工作
        for _, worker := range workers {
            worker.Work(8)
        }
    } else {
        for _, worker := range workers {
            worker.Rest()
        }
    }
}
```

#### 類型斷言(類型判斷/檢測)

方式一:判斷接口時例的具體類型
switch xxx.( type) {
case *Coder:
//...
case *Boss:
//...
}

```go
func main() {
    workers := make([]Worker, 0)
    //添加不同類型的勞動者
    workers = append(workers, &Coder{"嚕代碼"})
    workers = append(workers, &ProductManager{"拍腦門"})
    workers = append(workers, &Boss{"吹牛逼"})

    for _, worker := range workers {
        switch worker.(type) {
        case *Coder:
            fmt.Println("倫加是個嚕代碼的")
        case *ProductManager:
            fmt.Println("輪家是搞創意的，不要跟我聊邏輯")
        case *Boss:
            fmt.Println("我是你老闆")
        default:
            fmt.Println("不知道什麼鳥")
        }
    }
}
```

方式二:判斷接口實例是不是程序員
if coder,ok := xxx. (*Coder);ok{
//確實是程序員
//此時的coder是程序員指針類型
}else{
  //xxx壓根不是程序員
}

```go
func main() {
    workers := make([]Worker, 0)
    //添加不同類型的勞動者
    workers = append(workers, &Boss{"吹牛逼"})
    workers = append(workers, &ProductManager{"拍腦門"})
    workers = append(workers, &Coder{"嚕代碼"})

    for _, worker := range workers {
        if coder, ok := worker.(*Coder); ok {
            fmt.Println("發現一隻程序猿在", coder.skill)
            coder.WorkHome()
        } else {
            fmt.Println(worker, "不是程序猿")
        }
    }
}
```

#### 抽象

父類接口對只定義方法不做具體實現

戰士接口:

* 抽像方法1: 進攻
* 抽象方法2: 防守

顯卡接口:

* 顯示圖形

* 

#### 物件導向練習

需求: 定義動物接口: 出生 死亡 活著

定義動物實現類: 鳥 魚 野獸(跑 捕食)

繼承野獸: 實現老虎 實現人

業務場景 工作日所有動物都活著 周末人出來補食 野獸逃跑 其他動物死光光

練習需求二:

/*
接口: 影片 方法: 製作 上映
接口: 黃暴產品 方法: 刺激你的神經
封裝: 封裝電影類 屬性: 名字 公司 主演們 實現影視作品接口
繼承: 愛情片 科幻片繼承於電影 各自覆寫製作方法
接口繼承: 東方藝術 繼承影視作品和黃暴產品
多態: 影視作品細分 電視劇 網路劇
類型斷言: 零點以前看隨機電影 零點以後看東方藝術
*/

## 鍵盤輸入

* fmt.Scanln

```go
  var name string
  var age byte
  var sal float32
  var isPass bool
  fmt.Println("請輸入姓名：")
  fmt.Scanln(&name)

  fmt.Println("請輸入年齡：")
  fmt.Scanln(&age)

  fmt.Println("請輸入薪水：")
  fmt.Scanln(&sal)

  fmt.Println("請輸入是否通過考試：")
  fmt.Scanln(&isPass)

  fmt.Printf("名字是 %v \n 年齡是 %v \n 薪水是 %v \n 是否通過考試 %v \n", name, age, sal, isPass)
```

* fmt.Scanf

```go
var name string
var age byte
var sal float32
var isPass bool
fmt.Println("請輸入你的姓名，年齡，薪水，是否通過考試(使用空格隔開)")
fmt.Scanf("%s %d %f %t", &name, &age, &sal, &isPass)
fmt.Printf("名字是 %v \n 年齡是 %v \n 薪水是 %v \n 是否通過考試 %v \n", name, age, sal, isPass)
```



## 文件讀寫操作

### 打開文件

以只讀方式打開

```go
func main() {
    file, err := os.Open("file路徑")
    if err == nil {
        fmt.Println("文件打開成功")
    } else {
        fmt.Println("文件打開失敗,err=", err)
        return
    }
    defer func() {
        file.Close()
        fmt.Println("文件已關閉")
    }()

    fmt.Println("拿著文件一頓騷操作")
    time.Sleep(1 * time.Second)

}
```

```go
/* 以只讀方式打開一個文件 創建帶緩衝的讀取器 逐行讀取到末尾*/
func main() {
    //以只讀的方式打開文件 並且賦予0666的權限
    file, err := os.OpenFile("C:/Users/bited/Desktop/0330會議", os.O_RDONLY, 0666)
    if err == nil {
        fmt.Println("文件打開成功")
    } else {
        fmt.Println(err)
    }
    defer func() {
        file.Close()
        fmt.Println("文件已關閉")
    }()

    //建立文件緩衝讀取器
    reader := bufio.NewReader(file)
    //循環讀取文件行
    for true {
        //依行讀取文件
        str, err := reader.ReadString('\n')
        if err == nil {
            fmt.Println(str)
        //判斷文件是否到了末尾
        } else if err == io.EOF {
            fmt.Println("已經到文件末尾")
            break
        } else {
            fmt.Println(err)
        }
    }

}
```

### 使用ioutil讀入寫出

```go
/*use ioutil read a file*/
func main() {
    file, err := ioutil.ReadFile(file_path)
    if err == nil {
        fmt.Println(string(file))
    } else {
        fmt.Println("讀取失敗", err)
    }

}
```

```go
func main() {
    data := `
  兩隻老虎
  兩隻老虎
  跑得快
  `
    perm := 0666
    ioutil.WriteFile("D:/四些逼嗑.txt", []byte(data), os.FileMode(perm))
    // ioutil.WriteFile(filename, data, perm)
}
```

#### 創寫追加或創寫覆蓋

以創寫追加或創寫覆蓋方式打開一個文件 緩衝式寫出幾行數據 倒乾緩衝區後退出

```go
func main() {
    file, err := os.OpenFile("D:/一些逼嗑.txt", os.O_CREATE|os.O_WRONLY|os.O_APPEND, 0666)
    if err != nil {
        fmt.Println(err)
        return
    }
    defer func() {
        file.Close()
        fmt.Println("文件已關閉")
    }()
//創建文件寫出器
    writer := bufio.NewWriter(file)
      //分批次的寫出數據
    writer.WriteString("女人四大願望\n")
    writer.WriteString("男人腦袋都壞掉\n")
    writer.WriteString("天天給錢要我花\n")
    writer.WriteString("還要排隊任我挑\n")
    writer.WriteString("青春年輕不變老\n")
    //將緩衝區的內容寫入文件
    writer.Flush()
}
```

文件開啟的模式

```go
O_RDONLY // open the file read-only.
O_WRONLY // open the file write-only.
O_RDWR   // open the file read-write.
// The remaining values may be or'ed in to control behavior.
O_APPEND // append data to the file when writing.
O_CREATE // create a new file if none exists.
O_EXCL   // used with O_CREATE, file must not exist.
O_SYNC   // open for synchronous I/O.
O_TRUNC  // truncate regular writable file when opened.
```

#### 判斷文件是否存在

```go
func main() {
    fileInfo, err := os.Stat("D:/四些逼嗑.txt")
    if err != nil {
        if os.IsNotExist(err) {
            fmt.Println("文件不存在!")
        }
    } else {
        fmt.Println(fileInfo)
    }

}
```

#### 檔案複製

傻瓜式複製檔案

```go
func main() {
  bytes, _ := ioutil.ReadFile("D:/fuckoff.jpg")
  err := ioutil.WriteFile("D:/fuckoff22222.jpg", bytes, 0666)
  if err == nil{
    fmt.Println("複製檔案成功")
  }else{
      fmt.Println("複製失敗")
  }
    fmt.Println("執行完畢")
}
```

io.Copy

```go
func main() {
  srcFile, err := os.OpenFile(filepath, os.O_RDONLY, 0666)
  if err == nil {
    dstFile, _ := os.OpenFile(filepath, os.O_WRONLY|os.O_CREATE, 0666)
    written, err := io.Copy(dstFile, srcFile)
    if err == nil {
      fmt.Println("複製成功", written)
    } else {
        fmt.Println("複製失敗, ", err)
    }
  } else {
     fmt.Println(err)
  }
}
```

使用緩衝1K的緩衝區配合緩衝讀寫器進行圖片複製

```go
func main() {
    srcFilename := "D:/All-38-HanyiSentyFonts.zip"
    dstFilename := srcFilename + ".copy"
    //打開原文件
    srcFile, err := os.OpenFile(srcFilename, os.O_RDONLY, 0666)
    if err == nil {
        //打開目的文件
        dstFile, _ := os.OpenFile(dstFilename, os.O_WRONLY|os.O_CREATE, 0666)
        //延時關閉文件
        defer func() {
            srcFile.Close()
            dstFile.Close()
            fmt.Println("文件全部關閉")
        }()
        //創建源文件緩衝讀取器
        reader := bufio.NewReader(srcFile)
        //創建目標文件的寫出器
        writer := bufio.NewWriter(dstFile)
        //創建小水桶 也就是緩衝區
        buffer := make([]byte, 1024)
        //一桶一桶的讀入數據到緩衝區(水桶) 直到io.EOF
        for {
            _, err := reader.Read(buffer)
            // fmt.Printf("%v", err)
            if err != nil {
                if err == io.EOF {
                    break
                }
            } else {
                writer.Write(buffer)
            }
        }
        writer.Flush()
    }
}
```

#### 字符統計

## 程式流程控制

1. 順序控制

2. 分支控制

3. 循環控制

### switch分支

基於不同條件執行不同動作，每一個case分支都是唯一的，從上到下逐一測試，直到匹配為止。匹配項後面不需要再加break。

```go
switch 表達式 {
    case 表達式1, 表達式2, ...:
        語句塊1
    case 表達式3, 表達式4, ...:
        語句塊2
    default:
        語句塊
}
```

### 循環分支

* for循環控制

```go
for i := 1; i < 10; i++{
    fmt.Println("Hello world")
  }
```

* 字符串遍歷

```go

```



## JSON

JavaScript Object Notation

* 什麼是JSON

使用JSON描述一下謙哥 盡量詳細地描述其特徵 包含其妻兒們的信息

```json
{
  "name": "于謙",
  "age": 50,
  "rmb": 12345.67,
  "sex": true,
  "hobby": ["抽菸","喝酒","燙頭"],
  "wife": {"name":"王剛但","sex":false},
  "aunts":[
  {"name":"王鐵蛋","sex":false},
  {"name":"王同蛋","sex":false},
  {"name":"王銀蛋","sex":false},
  ]
}
```

### JSON Marshal

將謙哥老婆的JSON信息轉換為MAP
將謙哥老婆的JSON轉換為結構體
將謙哥小姨子們的JSON信息轉換為MAP切片
將謙哥小姨子們的JSON信息轉換為M結構體切片

interface{}代表任意數據類型

```go
//定義結構體 需要外部能訪問
type Person struct {
    Name  string
    Age   int
    Rmb   float64
    Sex   bool
    Hobby []string
}

func main() {
    person := Person{"于謙", 50, 12345.67, true, []string{"抽菸", "喝酒", "燙頭"}}
    bytes, err := json.Marshal(person)
    if err != nil {
        fmt.Println("序列化失敗", err)
        return
    }
    fmt.Println(string(bytes))
}
```

* map類型轉化json

```go
func main(){
  dataMap := make(map[string]interface{})

  dataMap["name"] = "于謙"
  dataMap["age"] = 50
  dataMap["rmb"] = 123456.78
  dataMap["sex"] = true
  dataMap["hobby"] = []string{"抽菸","喝酒","燙頭"}

  bytes,err := json.Marshal(dataMap)
  if err != nil{
    fmt.Println("序列化失敗")
    return
  }
  fmt.Println(string(bytes))
}
```

### JSON Unmarshal

* 將謙哥的JSON信息轉換為結構體
  
  ```go
  // {"Name":"于謙","Age":50,"Rmb":12345.67,"Sex":true,"Hobby":["抽菸","喝酒","燙頭"]}
  func main() {
      jsonStr := `
      {
      "Name":"于謙",
      "Age":50,
      "Rmb":12345.67,
      "Sex":true,
      "Hobby":["抽菸","喝酒","燙頭"]
      }`
      type Person struct {
          Name  string
          Age   int
          Rmb   float64
          Sex   bool
          Hobby []string
      }
      person := Person{}
      json.Unmarshal([]byte(jsonStr), &person)
      fmt.Printf("%v", person)
  }
  ```

* 將謙哥的JSON信息轉換為map
  
  ```go
  func main() {
      jsonStr := `
    {
      "Name":"于謙",
      "Age":50,
      "Rmb":12345.67,
      "Sex":true,
      "Hobby":["抽菸","喝酒","燙頭"]
    }`
      person := make(map[string]interface{})
      json.Unmarshal([]byte(jsonStr), &person)
      fmt.Printf("%v", person)
  }
  ```

* 將謙哥小姨子的JSON信息轉換為map切片

```go
func main() {
    jsonStr := `
  [
    {"hobby":["抽中華","喝牛欄山","燙花捲頭"],"name":"王剛蛋"},
    {"hobby":["抽玉溪","喝五糧液","燙莎瑪特"],"name":"王鐵蛋"},
    {"hobby":["抽玉溪","喝五糧液","燙莎瑪特"],"name":"王銅蛋"}
  ]
  `
    person := make([]map[string]interface{}, 0)
    json.Unmarshal([]byte(jsonStr), &person)
    fmt.Printf("%v", person)
}
```

* 將謙哥小姨子的JSON信息轉換為結構體切片

* ```go
  func main() {
      jsonStr := `
    [
      {"hobby":["抽中華","喝牛欄山","燙花捲頭"],"name":"王剛蛋"},
      {"hobby":["抽玉溪","喝五糧液","燙莎瑪特"],"name":"王鐵蛋"},
      {"hobby":["抽玉溪","喝五糧液","燙莎瑪特"],"name":"王銅蛋"}
    ]
    `
      type Person struct {
          Name  string
          Hobby []string
      }
      persons := make([]Person, 0)
      json.Unmarshal([]byte(jsonStr), &persons)
      fmt.Println(persons)
  
  }
  ```

### 編碼及解碼JSON文件

#### 編碼數據到JSON文件

```go
func main() {
//于謙的go語言數據
    dataMap := make(map[string]interface{}, 0)
    dataMap["name"] = "于謙"
    dataMap["age"] = 50
    dataMap["rmb"] = 123456.78
    dataMap["sex"] = true
    dataMap["hobby"] = []string{"抽菸", "喝酒", "燙頭"}
//創建或打開欲寫出的JSON文件
    dstFile, _ := os.OpenFile("file/于謙.txt", os.O_WRONLY|os.O_CREATE, 0666)
    defer dstFile.Close()
//創建目標文件的編碼器
    encoder := json.NewEncoder(dstFile)
//將go語言數據編碼到json文件
    e := encoder.Encode(dataMap)
    if e != nil {
        fmt.Println("編碼到JSON文件失敗")
        return
    }
    fmt.Println("編碼成功")
}
```

#### 編碼謙哥八大姨結構體切片為JSON文件

```go
type Person struct {
    Name  string
    Age   int
    Rmb   float64
    Sex   bool
    Hobby []string
}

func main() {
    p1 := Person{"王鋼蛋", 30, 123.45, false, []string{"抽中華", "喝農夫山泉", "燙芋頭"}}
    p2 := Person{"王金蛋", 30, 123.45, false, []string{"抽玉溪", "喝樂百氏", "燙手"}}
    p3 := Person{"王銀蛋", 30, 123.45, false, []string{"抽黃金葉", "喝三路牛奶", "燙光頭"}}
    people := make([]Person, 0)
    people = append(people, p1, p2, p3)

    dstFile, err := os.OpenFile("file/八大姨.json", os.O_CREATE|os.O_WRONLY, 0666)
    defer dstFile.Close()
    if err != nil {
      fmt.Println("文件開啟失敗\n")
      return
    }
    encoder := json.NewEncoder(dstFile)
    e := encoder.Encode(people)
    if e != nil {
        fmt.Println("編碼失敗\n")
        return
    }
    fmt.Println("編碼成功\n")

}
```

#### 解碼<<謙嫂.json>>為map

```go
func main() {
    dataMap := make(map[string]interface{}, 0)
    srcFile, _ := os.OpenFile("file/謙嫂.json", os.O_RDONLY, 0666)
    defer srcFile.Close()
    decoder := json.NewDecoder(srcFile)
    //解碼源文件 丟入dataMap
    e := decoder.Decode(&dataMap)
    //判斷解碼成功或失敗
    if e != nil {
        fmt.Println("解碼失敗\n")
        return
    }
    fmt.Printf("%v", dataMap)
}
```

#### 解碼<<八大姨.json>>為結構體切片

```go
func main() {
    type Person struct {
        Name  string
        Age   int
        Rmb   float64
        Sex   bool
        Hobby []string
    }
    dataSlice := make([]Person, 0)
    srcFile, _ := os.OpenFile("file/八大姨.json", os.O_RDONLY, 0666)
    defer srcFile.Close()
    decoder := json.NewDecoder(srcFile)
    //解碼源文件 丟入dataSlice
    e := decoder.Decode(&dataSlice)
    //判斷解碼成功或失敗
    if e != nil {
        fmt.Println("解碼失敗\n")
        return
    }
    fmt.Printf("%v", dataSlice)
}
```

#### 使用simplejson

```go
import "github.com/bitly/go-simplejson"

func main() {
    js, _ := simplejson.NewJson([]byte(`{
    "test": {
        "array": [1, "2", 3],
        "int": 10,
        "float": 5.150,
        "bignum": 9223372036854775807,
        "string": "simplejson",
        "bool": true
    }
}`))

    arr, _ := js.Get("test").Get("array").Array()
    i, _ := js.Get("test").Get("int").Int()
    ms := js.Get("test").Get("string").MustString()
    fmt.Println("arr is ", arr)
    fmt.Println("int is ", i)
    fmt.Println("ms is ", ms)
}
```

## panic 錯誤處理

### 幾個恐慌示例

* 指針值為空時，對其取值
  
  ```go
  func main(){
    var intPtr *int //<nil>
    fmt.Printf("%v", *intPtr)
  }
  //panic: runtime error: invalid memory address or nil pointer dereference
  ```

* 下標越界
  
  ```go
  func main() {
      // var intPtr *int //<nil>
      // fmt.Printf("%v", *intPtr)
      mySlice := make([]int, 0)
      mySlice = append(mySlice, 1, 2, 3, 4, 5)
      fmt.Println(mySlice[10])
  }
  //panic: runtime error: index out of range [10] with length 5
  ```

* 對空map賦值
  
  ```go
  func main() {
      var myMap map[string]interface{}
      myMap["name"] = "于謙"
  }
  ```

### 恐慌處理

#### defer處理方式

```go
func main() {
    //在函數結束之前 處理恐慌
    defer func() {
        //從恐慌中復活 找到導致恐慌的原因
        if err := recover(); err != nil {
            fmt.Println("致死的兇手是", err)
            fmt.Println("送錢送房送美女2")
        }
    }()
    //龐涓因為傳遞了負數半徑而死於此樹下
    v := GetBallVolumn(-1)
    fmt.Println("送錢送房送美女")
    fmt.Printf("球體積為%f\n", v)
}
//定義求體積函數
func GetBallVolumn(radius float64) float64 {
    if radius <= 0 {
        panic("半徑必須大於0!")
    }
    return (4 / 3.0) * math.Pi * math.Pow(radius, 3)
}
```

#### 回覆結果-錯誤對

```go
func main() {
    v, err := GetBallVolumn(2)
    if err == nil {
        fmt.Printf("球體積為%f\n", v)
    } else {
        fmt.Println("獲取體積失敗, err=", err)
        return
    }

}

func GetBallVolumn(radius float64) (volumn float64, err error) {
    if radius < 0 {
        panic("半徑不能指定為負數!")
    }
    //半徑如果不在取值費為[5,50]內 溫和地返回錯誤
    if radius < 5 || radius > 50 {
        err := errors.New("合法的半徑為[5,50]")
        return -1, err
    }
    return (4 / 3.0) * math.Pi * math.Pow(radius, 3), nil
}
```

### 自定義錯誤

```go
func main() {
    v, err := GetBallVolumn(4)
    if err == nil {
        fmt.Printf("球體積為%f\n", v)
    } else {
        fmt.Println("獲取體積失敗, err=", err)
        return
    }

}

//自定義異常InvalidRadiusError結構體
type InvalidRadiusError struct {
    Radius    float64
    MinRadius float64
    MaxRadius float64
}

/* 創建工廠方法 直接返回*InvalidRadiusError */
func NewInvalidRadiusError(radius float64) *InvalidRadiusError {
    ire := new(InvalidRadiusError)
    ire.Radius = radius
    ire.MinRadius = 5
    ire.MaxRadius = 50
    return ire
}
func (e *InvalidRadiusError) Error() string {
    info := fmt.Sprintf("%.2f是非法半徑，合法半徑範圍是[%.2f,%.2f]\n", e.Radius, e.MinRadius, e.MaxRadius)
    return info
}
func GetBallVolumn(radius float64) (volumn float64, err error) {
    if radius < 0 {
        panic(NewInvalidRadiusError(radius))
    }
    //半徑如果不在取值範圍[5,50]內 溫和地返回錯誤
    if radius < 5 || radius > 50 {
        err = NewInvalidRadiusError(radius)
        return -1, err
    }
    return (4 / 3.0) * math.Pi * math.Pow(radius, 3), nil
}
```

## 一個枚舉的例子

```go
type Month int

const (
    January Month = 1 + iota
    February
    March
    April
    May
    June
    July
    August
    September
    October
    November
    December
)

var months = [...]string{
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December",
}

// String returns the English name of the month ("January", "February", ...).
func (m Month) String() string {
    if January <= m && m <= December {
        return months[m-1]
    }
    buf := make([]byte, 20)
    n := fmtInt(buf, uint64(m))
    return "%!Month(" + string(buf[n:]) + ")"
}
```

## 網路編程

OSI 開放系統互聯(open system interconnect)

### 常用API

#### TCP

* 服務端
  
  * ```go
    listener_socket,err := net.Listen("tcp","127.0.0.1:8898")
    ```
  
  * ```go
    conn,err:= listener_socket.Accept()
    conn.Close()
    remoteAddr := conn.RemoteAddr()
    numOfByte,err := conn.Read(buf)
    conn.Write([]byte("hello numei"))
    ```
  
  * ```go
    listener_socket.Close()
    ```

* 代碼示例

* ```go
  package main
  
  import (
      "fmt"
      "net"
      "os"
  )
  //錯誤訊息處理
  func ServerHandlerError(err error, when string) {
      if err != nil {
          fmt.Println(err, when)
          os.Exit(1)
      }
  }
  //連接消息處理
  func ChatWith(conn net.Conn) {
  //創建緩接收客戶端消息的緩衝(水桶)
      buffer := make([]byte, 1024)
  //循環接收客戶端的訊息到緩衝
      for {
          //讀出緩衝內的數據 數據byte長度為n
          n, err := conn.Read(buffer)
          ServerHandlerError(err, "conn.Read()")
          //轉換緩衝內的[]byte為string類型
          clientMsg := string(buffer[:n])
          fmt.Printf("收到%s的消息:%s\n", conn.RemoteAddr(), clientMsg)
  
          if clientMsg == "im off" {
              conn.Write([]byte("bye"))
              break
          }else{
              //伺服器寫入一段訊息給客戶端
          conn.Write([]byte("server recieved:" + clientMsg))
          ServerHandlerError(err, "conn.Write()")
          }
      }
      fmt.Printf("客戶端%s已經斷開", conn.RemoteAddr())
      //斷開客戶端連接
      conn.Close()
  }
  
  func main() {
      ip := "127.0.0.1"
      port := "8888"
    listener, e := net.Listen("tcp", net.JoinHostPort(ip,port))
      ServerHandlerError(e, "net.Listen()")
      fmt.Println("Listening on " + ip + ":" + port + "....")
  
      for {
          conn, e := listener.Accept()
          ServerHandlerError(e, "listener.Accept()")
   //開闢一條獨立協程與該客戶端聊天
          go ChatWith(conn)
  
      }
  }
  ```

* 客戶端
  
  * ```go
    conn,err:=net.Dial("tcp","127.0.0.1:8898")
    conn.Close()
    conn.Write([]byte("hello nimei"))
    numOfBytes,err:=conn.Read(buf)
    ```
  
  * ```go
    reader := bufio.NewReader(os.Stdin)
    dat,isPrefix,err:=reader.ReadLine()
    ```

* 程式碼範例

* ```go
  package main
  
  import (
      "bufio"
      "fmt"
      "net"
      "os"
  )
  
  func ClientHandlerError(err error, when string) {
  
  }
  
  func main() {
      //透過tcp方式撥入伺服器
      conn, e := net.Dial("tcp", "127.0.0.1:8888")
      ClientHandlerError(e, "net.Dial")
      //準備標準輸入讀取器
      reader := bufio.NewReader(os.Stdin)
      //創建訊息緩衝區
      buffer := make([]byte, 1024)
  //開始不斷地發送消息
      for {
          //從命令行輸入一行消息
          lineBytes, _, _ := reader.ReadLine()
          //將消息寫給伺服器
          conn.Write(lineBytes)
          //讀取伺服器回應的訊息
          n, _ := conn.Read(buffer)
          //[]byte轉成字串
          serverMsg := string(buffer[:n])
          //判斷是否是退出訊息 如果伺服器傳來Bye
          if serverMsg == ("Bye") {
              break
          }else{
              fmt.Println("server:", serverMsg)
          }
      }
      fmt.Println("game over!")
  }
  ```

#### UDP

* 服務端
  
  * ```go
    udp_addr,err:=net.ResolveUDPAddr("udp",":8848")
    conn,err:=net.ListenerUDP("udp" udp_Addr)
    conn.Close()
    n,raddr,err:=conn.ReadFromUDP(buf[0:])
    fmt.Println("消息是",string(buf[0:n]))
    
    _,err=conn.WriteToUDP([]byte("hao nimei"),raddr)
    ```

* 代碼範例

* ```go
  package main
  
  import (
      "fmt"
      "net"
      "os"
  )
  
  func HandleError(err error, when string) {
      if err != nil {
          fmt.Println(err, "when", when)
          os.Exit(1)
      }
  }
  func main() {
      localIP := "127.0.0.1"
      localPort := "8848"
      udpAddr, err := net.ResolveUDPAddr("udp", localIP+":"+localPort)
      HandleError(err, "net.ResolveUDPAddr")
      udpConn, err := net.ListenUDP("udp", udpAddr)
      HandleError(err, "net.ListenUDP")
      fmt.Println(localIP + ":" + localPort + " 開始接收udp數據")
      buffer := make([]byte, 1024)
      for {
          n, raddr, err := udpConn.ReadFromUDP(buffer[0:])
          HandleError(err, "updConn.ReadFromUDP")
          clientMsg := string(buffer[:n])
          fmt.Printf("收到來自%s的消息%s\n", raddr, clientMsg)
          if clientMsg != "im off" {
              b := []byte("已閱" + clientMsg)
              udpConn.WriteToUDP(b, raddr)
          } else {
              b := []byte("bye")
              udpConn.WriteToUDP(b, raddr)
          }
      }
  
  }
  ```

* 客戶端
  
  * ```go
    conn,err:=net.Dial("tcp","127.0.0.1:8898")
    conn.Close()
    conn.Write([]byte("hello nimei"))
    numOfBytes,err:=conn.Read(buf)
    ```

* 代碼範例

* ```go
  package main
  
  import (
      "bufio"
      "fmt"
      "net"
      "os"
      "time"
  )
  
  func HandleClientError(err error, when string) {
      if err != nil {
          fmt.Println(err, when)
          os.Exit(1)
      }
  }
  func main() {
      serverIP := "127.0.0.1"
      serverPort := "8848"
      conn, err := net.Dial("udp", serverIP+":"+serverPort)
      HandleClientError(err, "net.Dial")
      buffer := make([]byte, 1024)
      reader := bufio.NewReader(os.Stdin)
  
      for {
          lineByte, _, _ := reader.ReadLine()
          conn.Write([]byte(lineByte))
          n, _ := conn.Read(buffer)
          serverMsg := string(buffer[:n])
          if serverMsg != "bye" {
              fmt.Println("伺服器:" + serverMsg)
          } else {
              fmt.Println("嗚呼 狡兔死 走狗烹 飛鳥盡 良弓藏 吾去也")
              time.Sleep(1 * time.Second)
              break
          }
      }
  
  }
  ```

* 

#### HTTP

* 服務端
  
  * ```
    http.HandlerFunc("/hellp",func(writer,request){w.Write([]byte("hello"))})
    http.ListenAndServe("127.0.0.1:8080",nil)
    ```

* 客戶端
  
  * GET
  
  * ```go
    resp,err:=http.Get(url)
    resp.Body.Close()
    byres,_:=ioutil.ReadAll(resp.Body)
    ```
  
  * POST
  
  * ```go
    resp.err:=http.Post(url,"application/x-www-form-urlencoded",strings.NewReader("id=nimei&age=30"))
    ```
  
  * ```go
    package main
    
    import (
        "fmt"
        "io/ioutil"
        "net/http"
        "os"
        "strings"
    )
    
    func CHandleError(err error, when string) {
        if err != nil {
            fmt.Println(err, when)
            os.Exit(1)
        }
    }
    
    func main() {
        var url string
        var contentType string
        // var searching string
        url = "https://httpbin.org/post?name1=AAA&boss=Max"
        contentType = "application/x-www-form-urlencoded"
        resp, err := http.Post(url, contentType, strings.NewReader("name=Jacky&teacher=John"))
        defer resp.Body.Close()
        CHandleError(err, "http.Post")
        bytes, err := ioutil.ReadAll(resp.Body)
        CHandleError(err, "ioutil.ReadAll")
        fmt.Println(string(bytes))
    }
    ```

## 單元測試

程式員自行做單元測試 可以自行做測試

### TDD  測試驅動開發

test: 前綴為test

benchmark

example

現在我們想要測試split這個函數:

```go
package split

import "strings"

// Split  separate string by sep
// a:b:c : --> [a b c]
func Split(s string, sep string) []string {

    index := strings.Index(s, sep)
    var str []string
    for index > 0 {
        str = append(str, s[:index])
        s = s[index+1:]
        index = strings.Index(s, sep)
    }
    str = append(str, s)
    return str
}
```

split_test.go

```go
package split

import (
    "reflect"
    "testing"
)

func TestSplit(t *testing.T) {
    t.Log("測試一")
    got := Split("a:b:c", ":")
    want := []string{"a", "b", "c"}
    if ok := reflect.DeepEqual(got, want); !ok {
        t.Fatalf("期望得到:%v,實際得到%v\n", want, got)
    }
}

func TestNoneSplit(t *testing.T) {
    got := Split("a:b:c", "*")
    want := []string{"a:b:c"}
    if ok := reflect.DeepEqual(got, want); !ok {
        t.Fatalf("期望得到:%v,實際得到%v\n", want, got)
    }
}
```

cmd

```go
go test -v
```



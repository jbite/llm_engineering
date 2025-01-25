# GoLang 協程併發

協程: co-routine 使用諧音 goroutine。在go中又稱go程

```go
/*同步阻塞 無併發
CountNumber2被CountNumber2阻塞

*/
func main1701() {
	CountNumber1()
	CountNumber2()
	fmt.Println("main over!")
}

func main1702() {
	//將CountNumber1丟到子協程中執行
	//開闢獨立的協程運行
	go CountNumber1()
	CountNumber2()
	fmt.Println("main over!")
}
func CountNumber1() {
	for i := 1; i <= 10; i++ {
		fmt.Println(i)
		time.Sleep(200 * time.Millisecond)
	}
}
func CountNumber2() {
	for i := 90; i <= 100; i++ {
		fmt.Println(i)
		time.Sleep(200 * time.Millisecond)
	}
}
```

主程序一中止  協程也會立即中止

```go
func 
//創建goroutine
func main(){
    //開闢一條協程 與主協程併發地執行newTask()
    go newTask()
    //主協程賴著不死 主協程如果死了 子協程也得陪葬
    for{
        fmt.Println("this is a main goroutine")
        time.Sleep(time.Second)
    }
}
```

### 出讓協程資源

通過runtime.Gosched()出讓協程資源，讓其他協程先執行

```go
func Hello(no int) {
	//協程內的代碼執行在子協程
	//協程內有一定的初始化時間
	for j := 11; j <= 20; j++ {
		if no == 5 {
			//出讓當前協程的優先級
			runtime.Gosched()
		}
		fmt.Printf("協程%d:%d", no, j)
	}
	fmt.Printf("\n")
}

func main() {
	//開闢十條協程
	for i := 1; i <= 9; i++ {
		go Hello(i)
	}
	time.Sleep(time.Second * 3)
}
```

### 查看可用內核數

runtime.NumCPU()

### 設置最大可用核心數

將可用CPU邏輯核心數 並返回之前的設置

GOMAXPROCS(1)

### 協程自殺

runtime.Goexit()

```go
func main() {
	go func() {
		for i := 1; i <= 10; i++ {
			if i == 5 {
				fmt.Println("goroutine:我要退出了")
				//當前協程退出
				runtime.Goexit()
			}
			fmt.Println("goroutine", i)
			time.Sleep(time.Millisecond * 500)
		}
	}()

	for i := 0; i < 10; i++ {
		fmt.Println("main", i)
		time.Sleep(time.Millisecond * 500)
	}
	fmt.Println("Main over!")
}
```

主協程如果提前透過runtime.Goexit()退出了，會讓子協程自由跑完

```go
func main() {
	go func() {
		for i := 1; i <= 10; i++ {

			fmt.Println("goroutine", i)
			time.Sleep(time.Millisecond * 500)
		}
	}()
	for i := 0; i < 10; i++ {
		if i == 5 {
			fmt.Println("main:我要s了，還子協程自由")
			//當前協程退出
			runtime.Goexit()
		}
		fmt.Println("main", i)
		time.Sleep(time.Millisecond * 500)
	}
	fmt.Println("Main over!")
}
```

## 併發技術: 管道通信

channel提供了一種通信機制 通過他 goroutine可以向另一個goroutine發送消息

channel本身還關聯一個類型 也就是channel可以發送數據的類型 例如int類型消息的channel寫作chan int 

channel創建使用內置的make函數創建  下面聲明了一個chan int類型的channel

```
ch := make(chan int)
```

channel 和map類似 make創建了一個底層數據結構的引用 當賦值或參數傳遞時 只是拷貝了一個channel引用 指向相同的channel對像。和其他引用類型一樣 channel的空值為nil 使用==可以對類型相同的channel進行比較 只有指向相同對像或同為nil時，才返回true

#### channel的讀寫操作

```go
ch := make(chan int)
//write to channel
ch <- 123
//read from channel
x := <- ch
fmt.Println(x)
//讀取失敗 因為channel緩存為0
```

```go
ch := make(chan int,10)
//write to channel
ch <- 123
//read from channel
x := <- ch
fmt.Println(x)
//讀取成功
```

主協程寫入管道 子協程讀出

```go
func main() {
	var myChan chan int
	fmt.Println(myChan)

	myChan = make(chan int, 1)

	go func() {
		for i := 0; i < 5; i++ {
      //已經讀空 沒有人寫 就讀出阻塞
			x := <-myChan
			fmt.Println("讀出", x)
		}
	}()
	for i := 0; i < 5; i++ {
		myChan <- i
		fmt.Println("寫入: ", i)
		time.Sleep(time.Second * 1)
	}

}
```

### 關閉管道

管道關閉以後不可再寫入 但可以讀出

```go
func main() {
	bools := make(chan int, 3)
	go func() {
		//等候管道關閉
		time.Sleep(time.Second * 3)
		//從關閉的管道讀出數據
		x := <-bools
		fmt.Println(x)
		x = <-bools
		fmt.Println(x)
		//管道空了以後 讀出零值
		x = <-bools
		fmt.Println(x)

		x, ok := <-bools
        if ok{
            fmt.Println("讀出管道讀出的值",x)
        }else{
            fmt.Println("讀出零值",x)
        }	
	}()

	bools <- 123
	bools <- 456
	close(bools)
	//panic: send on closed channel
	//關閉的管道無法再寫入數據
	// bools <- true

	for {
		time.Sleep(time.Second * 1)
	}
}

```

關閉一個空管道(沒有初始化的管道)會panic

```go
func main(){
    var cc = make(chan int)
    close(cc)
}

//panic
```

關閉一個關閉的管道會panic

```
cc <- 123
closse(cc)
close(cc)
```

管道的遍歷

```go
func main1772() {
	bools := make(chan int, 3)
	go func() {
		//等候管道關閉
		time.Sleep(time.Second * 3)
		//從關閉的管道讀出數據
		x := <-bools
		fmt.Println(x)
		x = <-bools
		fmt.Println(x)
		//管道空了以後 讀出零值
		x = <-bools
		fmt.Println(x)

		x, ok := <-bools
		fmt.Println(x, ok)
	}()

	bools <- 123
	bools <- 456
	close(bools)
	//panic: send on closed channel
	//關閉的管道無法再寫入數據
	// bools <- true

	for {
		time.Sleep(time.Second * 1)
	}
}
func main() {

	myChan := make(chan int, 5)
	go func() {
		for x := range myChan {
			fmt.Println(x)
		}
	}()
	myChan <- 123
	myChan <- 124
	myChan <- 125
	myChan <- 126

	time.Sleep(3 * time.Second)

}
```

### 管道的緩存能力

```go
len(myChan)
cap(myChan)
```

已滿的管道無法再進行寫入

已讀空的管道無法再讀出



### 管道的調度能力

```go
//三個協程分別數數
// 要求主協程分別在所有協程工作結束時剛好結束

func Count(grName string, n int, chanQuit chan string) {
	for i := 0; i < n; i++ {
		fmt.Println(grName, i)
		time.Sleep(time.Millisecond * 200)
	}
	fmt.Println(grName, "工作完畢")

	//向通知任務完畢通知管道中寫入數據
	chanQuit <- grName + "mission completed!"
}

func main() {
	//創建一個長度3的任務完畢通知管道
	chanMsg := make(chan string, 3)
	go Count("son", 10, chanMsg)
	go Count("daughter", 70, chanMsg)
	Count("main", 5, chanMsg)
	//當管道還沒讀完 主協程不會結束
	for i := 0; i < 3; i++ {
		<-chanMsg
	}
	//阻塞等待從完畢通知管道讀出所有完畢消息
	fmt.Println("over")
}
```

### 生產者消費者模型

生產者每天生產一件產品

生產者每生產完一件 消費者就消費一件

消費十輪後 主協程退出



```go
type Product struct{
 Name string   
}

func main(){
    //創建【商店管道】【記數管道】
    shopChan := make(chan Product,5)
    countChan := make(chan int,5)
    
    go Produce()
    //創建多個消費者
    for j := 1; j < 10; j++ {
		go Consumer(j, productChan, countChan)
	}
    
    //主協程阻塞從【記數管道】讀出10個數據就結束
    for i:=0;i<10;i++{
    	<-countChan
    }
}
//消費者阻塞等待從【商店管道】
//shopChan chan <-Product只寫管道
func Produce(shopChan chan <-Product){
    for{
        //生產產品
		p := Product{"產品" + strconv.Itoa(time.Now().Second())}
        //生產以後放商店
        shopChan <- p
        fmt.Println("生產出一個產品了")
        //休息
        time.Sleep(500 * time.Millisecond)
    }
}

//shopChan <- chan Product 此處的shopChan是只讀管道
func Consume(shopChan <- chan Product,countChan chan<- int){
    for{
        p := <- shopChan
    	fmt.Printf("消費者%d買了一個%s\n", i, p.Name)
        countChan <- 1
    }
    
}
```

### 併發理論

#### 共享內存

多個併發線程通過共享內存的方式交互數據

AB間共享的數據地址可能被C併發修改

#### 同步鎖/資源鎖

共享的內存地只在特定時間內被鎖定

加鎖期間 期他線程無法訪問 帶來低效率問題

#### 死鎖

A鎖住B的資源

B鎖住A的資源

AB同時阻塞

#### 線程池

線程的開銷大:保存上下文數據 需要CPU調度線程

為了避免無度創建線程(導致OutOfMemory):

* 在池中創建一堆線程
* 循環利用這些線程
* 用完以後丟回池中

利: 避免無度創建線程 降低了OOM的風險

弊: 需要先佔用memory

線程併發弊端:

* 開線程佔memory

* 線程切換佔CPU
* 共享內存不安全
* 回調地獄導致開發難度高

#### 協程併發

##### CSP模型

communicatingSequentialProcess

可通信的序列化進程

併發的進程間通過管道進行通信

##### 管道通信

* CSP模型提出

* 以點對點管道代替內存共享實現併發進程間的數據交互

* 相比內存共享數據交互的相率要高很多 所有的併發協程都可以讀寫管道 
* 通過緩存限制讀寫堵塞實現協程間的調度和切換 是一種邏輯態的調度

##### 協程

微線程

通過管道實現同步

阻塞寫入協程 管道緩存已滿無法寫入

阻塞讀出協程 管道緩存已空無法讀出

管道的緩存能力 

* 無緩存的管道 讀寫必須同時 否則就被阻塞

關閉管道後依然可以讀取但不能寫入，關閉管道時，會向讀取協程發出通知

* 一但收到管道關閉通知 就會結束阻塞讀取
* 沒有收到管道關閉通知 就會持續堵塞

##### 單向管道

### 併發控制

@通過管道控制併發數{

* 100條協程併發求1-10000平方根
* 最大併發數控制在5
* 管道實現

}

```go
func main() {
	//信號量 控制併發數的管道
	//凡要併發執行的協程必須先將協程名字註冊道該管道
	semaphore := make(chan string, 5)
	//丟入需要計算任務到【任務管道】
	missionChan := make(chan int, 5)
	//產生100條協程處理工作
	for l := 0; l < 100; l++ {
		go computeSqr("協程"+strconv.Itoa(l), numChan, semaphore)
	}
    //負責將任務放到【任務管道】的協程
	go putNumToChan(1000000, numChan)

	for {
		time.Sleep(time.Second * 1)
	}
}
//從信號管道註冊名字並且開始工作，如果名字註冊不了就阻塞
//註冊成功後，從【任務管道】取出任務
//將運算結果輸出
//從信號管道中，騰出一個空間
func computeSqr(name string, missionChan <-chan int, semaphore chan string) {
	for {
		semaphore <- name
		n := <-missionChan
		f := math.Sqrt(float64(n))
		time.Sleep(500 * time.Millisecond)
		fmt.Printf("%s:%d的平方根是%.2f\n", name, n, f)
		<-semaphore
	}

}
//將要運算的工作放到【任務管道】missionChan
func putNumToChan(i int, missionChan chan int) {
	for l := 0; l < i; l++ {
		missionChan <- l
	}
}
```

#### select 多路復用

隨機選擇一條可以走通的路

循環從一寫兩讀三條管道中隨機選擇一條能走的路

等所有路都走不通了 就退出循環

> 隨機選擇一條能走通的case
>
> 所有case都走不通時 走default
>
> 可以通過break跳出select break XXX 跳出指定標籤

```go
func main() {
	chA := make(chan int, 5)

	chB := make(chan int, 4)
	chB <- 123
	chB <- 123
	chB <- 123
	chB <- 123
	chC := make(chan int, 3)
	chC <- 123
	chC <- 123
	chC <- 123

    OUTER:
	for {
		select {
		case chA <- 123:
			fmt.Println("執行任務A")
		case x := <-chB:
			fmt.Println("執行任務B", x)
		case <-chC:
			fmt.Println("執行任務C")
		default:
			fmt.Println("全部任務已結束")
			//跳出select
			break OUTER
		}
	}

}
```

#### 定時器的中止與重置

3秒鐘之後宣布一項重大決定

使用兩種方式來實現

爆炸前終止定時器

爆炸前對定時器進行重置

爆炸後 對定時器進行重置

啟動定時器

```go
func main() {
	timer := time.NewTimer(5 * time.Second)
	fmt.Println("計時開始", time.Now())

	//阻塞5秒
	endTime := <-timer.C
	fmt.Println("炸彈引爆於", endTime)
}
```

停止定時器

```go
func main() {
	timer := time.NewTimer(5 * time.Second)
	fmt.Println("計時開始", time.Now())
	go func() {
		time.Sleep(time.Second * 3)
		//停止計時器 永遠不會向timer.C寫入數據
		ok := timer.Stop()
		if ok {
			fmt.Println("炸彈已拆除")
			os.Exit(1)
		}
	}()

	//阻塞5秒
	endTime := <-timer.C
	fmt.Println("炸彈引爆於", endTime)
}
```

重置定時器

```go
func main() {
	timer := time.NewTimer(5 * time.Second)
	fmt.Println("計時開始", time.Now())
    //重置定時器
	timer.Reset(time.Second * 2)
	endTime := <-timer.C
	fmt.Println("炸彈引爆於", endTime)

	time.Sleep(time.Millisecond * 500)
	ok := timer.Reset(1 * time.Second)
	if !ok {
		fmt.Println("reset false")
	}
}
```

#### 周期性定時器

每秒喊一次我要去浪 九次後退出

```go
func main() {
	var tickerStopped = false
	ticker := time.NewTicker(1 * time.Second)
	go func() {
		time.Sleep(time.Second * 9)
		ticker.Stop()
		tickerStopped = true
	}()
	for {
		if tickerStopped {
			os.Exit(1)
		} else {
			fmt.Println("我要去浪", <-ticker.C)
		}
	}
}
```

#### 等待組

每增加一個子協程 就向等待組中+1 每結束一個協程，就從等待組中-1 主協程會阻塞等待直到組中的協程數等於0為止

這種方式就比使用`time.Sleep()`好控制併發結束了

* 依賴包: `sync`

```go
func main() {
    //聲明一個等待組
	var wg sync.WaitGroup
    //每開闢一個協程 就從等待組中加一
	wg.Add(1)
	go func() {
		fmt.Println("協程A開始工作")
		time.Sleep(time.Second * 3)
		fmt.Println("協程A over")
        //每完成一個工作 就從等待組中註銷
		wg.Done()
	}()
	//每開闢一個協程 就從等待組中加一
	wg.Add(1)
	go func() {
		fmt.Println("協程B開始工作")
		<-time.After(5 * time.Second)
		fmt.Println("協程B over")
        //每完成一個工作 就從等待組中註銷
		wg.Done()
	}()
	//每開闢一個協程 就從等待組中加一
	wg.Add(1)
	go func() {
		fmt.Println("協程C開始工作")
		ticker := time.NewTicker(time.Second * 1)
		for i := 0; i < 4; i++ {
			<-ticker.C
		}
		ticker.Stop()
		fmt.Println("協程C over")
        //每完成一個工作 就從等待組中註銷
		wg.Done()
	}()
	//阻塞等待wg中的協程數歸零
	wg.Wait()
	fmt.Println("main over")
}

```

#### 讀寫鎖(readWriteMutex)

```go
/*
讀寫鎖
多路只讀
一路只寫
讀寫互斥
*/
func main() {
	var rwm sync.RWMutex
	//鎖定為寫模式  一路只寫
	rwm.Lock()
	//解鎖寫模式
	rwm.Unlock()
	//鎖定為讀模式 多路只讀
	rwm.RLock()
	//解鎖只寫模式
	rwm.RUnlock()
}
```

資料庫的一讀多寫

```go
/*
ReadDB方法設為多路只讀
WriteDB方法設定為單路只寫
創建5讀5寫10條協程 觀察讀寫鎖效果
*/

func main() {
	var wg sync.WaitGroup
	var rwm sync.RWMutex
	for i := 0; i < 5; i++ {
		wg.Add(1)
		go func() {
			//鎖定為只讀模式 允許多個協程同時搶到多個讀鎖
			rwm.RLock()
			fmt.Println("讀取數據庫")
			<-time.After(3 * time.Second)
			//解開讀鎖
			rwm.RUnlock()
			wg.Done()
		}()
		wg.Add(1)
		go func() {
			//設為寫模式 只允許一條協程取得鎖
			rwm.Lock()
			fmt.Println("寫入數據庫")
			<-time.After(2 * time.Second)
			//解開寫鎖
			rwm.Unlock()
			wg.Done()
		}()
	}

	wg.Wait()
	fmt.Println("main over")
}
```

#### 死鎖案例

@死鎖案例

AB互相要求對方先發紅包再發

```go
package main

import "sync"

func main17161() {
	chA := make(chan int)
	chB := make(chan int)

	//a
	go func() {
		<-chA
		chB <- 123
	}()

	//b
	<-chB
	chA <- 123
}
func main() {
	var wg sync.WaitGroup
	chA := make(chan int)
	chB := make(chan int)

	//a
	wg.Add(1)
	go func() {

		<-chA
		chB <- 123
		wg.Done()
	}()

	//b
	wg.Add(1)
	go func() {
		<-chB
		chA <- 123
		wg.Done()
	}()
	wg.Wait()
}

```

讀寫對方要求對方先執行後再執行

```go
func main() {
	var rwm sync.RWMutex
	ch := make(chan int)
	go func() {
		rwm.RLock()
		//沒人寫就讀不出來
		x := <-ch
		fmt.Println("讀到", x)
		rwm.RUnlock()
	}()

	go func() {
		rwm.Lock()
		//沒人讀就寫不進去
		ch <- 123
		fmt.Println("寫入123")
		rwm.Unlock()
	}()
	time.Sleep(1 * time.Second)
	fmt.Println("main over")
}
```

#### 只做一次

`once`

```go
type Person struct {
	Name  string
	Alive bool
}

func Kill(p *Person) {
	fmt.Println(p.Name + "死了")
	p.Alive = false
}
func main() {
	var once sync.Once
	var wg sync.WaitGroup

	bill := &Person{"Bill", true}
	//開闢十條協程追殺比爾
	for i := 0; i < 10; i++ {
		wg.Add(1)
		go func() {
            //保證比爾只被殺死一次
			once.Do(func() {
				Kill(bill)
			})
			wg.Done()
		}()
	}
	wg.Wait()
}
```

#### 條件變量

監控比特幣的漲跌決定買進或不買進

```go
package main

import (
	"fmt"
	"sync"
	"time"
)

func main() {
	bitcoinRising := false
	cond := sync.NewCond(&sync.Mutex{})

	go func() {
		for {
			time.Sleep(time.Second * 1)
			cond.L.Lock()
			bitcoinRising = !bitcoinRising
			cond.Broadcast()
			cond.L.Unlock()
		}
	}()

	for {
		cond.L.Lock()
		for !bitcoinRising {
			fmt.Println("停止投資比特幣")
			//彼特幣沒有漲價 
            //內部會釋放鎖 等待比特幣漲價的消息     
			cond.Wait()
		}
		fmt.Println("開始投資比特幣")
		cond.L.Unlock()
	}
}
```

##### 伺服器負載控制

監聽最大客戶端連接數

服務端協程: 只要伺服器客戶端連接 通知控制協程就進入阻塞等待

控制協程 收到服務端預警 削減客戶端數量後通知服務端 再次接入

```go
package main

import (
	"fmt"
	"math/rand"
	"sync"
	"time"
)

type Server struct {
	maxConnections int
	connections    int
	chAlarm        chan bool
	cond           *sync.Cond
}

func NewServer(maxConnections int) *Server {
	server := new(Server)
	server.maxConnections = maxConnections
	server.chAlarm = make(chan bool)
	server.cond = sync.NewCond(&sync.Mutex{})
	return server
}
func (s *Server) StartOverLoadHandler() {
	for {
		//阻塞監聽是否有預警
		<-s.chAlarm
		//加鎖 削減客戶端數量 發送預警解除
		s.cond.L.Lock()
		time.After(3 * time.Second)
		s.connections -= rand.Intn(s.maxConnections)
		fmt.Println("s.connections", s.connections)
		s.cond.Broadcast()
		fmt.Println("過載預警已解除")
		s.cond.L.Unlock()
	}
}
func (s *Server) Run() {
	go s.StartOverLoadHandler()
	for {
		s.cond.L.Lock()
		//監聽是否過載
		for s.connections == s.maxConnections {
			//發送預警
			s.chAlarm <- true
			fmt.Println("過載預警已發送")
			//等待預警解除
			s.cond.Wait()
		}
		//接入客戶端
		time.Sleep(1 * time.Second)
		s.connections++
		fmt.Println("已接入客戶端:s.connections=", s.connections)
		s.cond.L.Unlock()
	}
}

func main() {
	server := NewServer(5)
	server.Run()
}
```

城管預警

監聽城管大隊

燒烤攤集群: 監聽城管大隊 只要出洞就進入阻塞等待至被喚醒 否則就提供露天燒烤

公關專員: 擺平城管大隊 並廣播通知所有燒烤攤

```go
package main

import (
	"fmt"
	"sync"
	"time"
)

func main() {
	var safe = true
	cond := sync.NewCond(&sync.Mutex{})
	chAlarm := make(chan bool, 1)
	for i := 0; i < 3; i++ {
		go func(index int) {
			for {
				//危險出現 就等待起來
				cond.L.Lock()
				for !safe {
					//發送預警
					select {
					case chAlarm <- true:
					default:
						//已經有人發過了
					}
					fmt.Println("燒烤攤", index, ":先避風頭吧")
					cond.Wait()
				}
				cond.L.Unlock()
				fmt.Println(index, ":提供露天燒烤")
				time.Sleep(time.Second * 3)
				safe = !safe
				fmt.Println("燒烤攤", index, ":城管出來啦")
			}

		}(i)
	}
	go func() {
		for {
			select {
			//幫大家平事
			case <-chAlarm:
				cond.L.Lock()
				time.Sleep(time.Second * 3)
				fmt.Println("公會主席出來賣面子")
				safe = true
				cond.Broadcast()
				cond.L.Unlock()
			default:
				//日常生活
				fmt.Println("公會主席的日常幸福生活")
				time.Sleep(time.Second * 3)

			}
		}
	}()
	time.Sleep(time.Second * 300)
}

```

#### 原子操作

鎖的一種替代方案

原子操作由底層應件支持 而鎖則由操作系統提供的API實現

若實現相同的功能 原子操作通常更有效率

只能對基本類型加鎖



### 併發實現文件讀寫 並透過原子操作保證數據安全

```go
package main

import (
	"errors"
	"fmt"
	"io"
	"os"
	"sync"
	"sync/atomic"
)

type DataFile interface {
	/*
	  可以讀
	  rsn read-serial-number 當前讀取到的塊的序列號
	  d 原始字節數據
	  err 錯誤
	*/
	Read() (rsn int64, d Data, err error)
	/*
	  寫入一個數據塊
	  d Data 要寫入的數據
	  wsn int64 當前寫入的塊序列號
	*/
	Write(d Data) (wsn int64, err error)
	//獲取下一次讀取的數據塊的序列號
	Rsn() int64
	//獲取下一次寫入的數據塊的序列號
	Wsn() int64
	//獲取數據塊的長度
	DataLen() uint32
}

//給原始字節切片起別名
type Data []byte

/*
DataFile接口的實現類
*/
type myDataFile struct {
	//文件
	f *os.File

	datalen uint32
	//用於文件的讀寫鎖
	fmutex sync.RWMutex
	//寫操作需要用到的字節偏移量
	woffset int64
	//讀操作需要用到的字節偏移量
	roffset int64

	rcond *sync.Cond //條件變量
	//數據塊長度
}

/*
工廠方法
參數:
path: string
datalen uint32 指定數據塊大小
----------
返回值
DataFile 返回文件對像

*/
func NewDataFile(path string, datalen uint32) (DataFile, error) {
	f, err := os.Create(path)
	if err != nil {
		return nil, err
	}
	if datalen == 0 {
		return nil, errors.New("Invalid data length!")
	}
	//創建指定的數據文件對像 並指定IO文件 和指定數據塊大小
	df := &myDataFile{f: f, datalen: datalen}
	//初始化對象的條件變量
	df.rcond = sync.NewCond(df.fmutex.RLocker())
	return df, nil
}

func (df *myDataFile) Rsn() int64 {
	//同步加載下一次讀取的序列號
	offset := atomic.LoadInt64(&df.roffset)

	return offset / int64(df.datalen)
}

func (df *myDataFile) Wsn() int64 {
	//同步獲取當前df的寫入字節偏移量
	offset := atomic.LoadInt64(&df.woffset)
	//下一個數據塊的序列號
	return offset / int64(df.datalen)
}

/*返回塊文件的大小*/
func (df *myDataFile) DataLen() uint32 {
	//同步獲取當前df的數據塊大小 並返回
	return atomic.LoadUint32(&df.datalen)
}

/*
  可以讀
  rsn read-serial-number 當前讀取到的塊的序列號
  d 原始字節數據
  err 錯誤
*/
func (df *myDataFile) Read() (rsn int64, d Data, err error) {
	var offset int64
	//使用原子操作確保offset被正確賦值
	offset = atomic.LoadInt64(&df.roffset)

	//計算本次讀取的數據塊的序列號
	rsn = offset / int64(df.datalen)
	//創建一個數據塊大小的緩衝區
	buffer := make([]byte, df.datalen)
	//加讀鎖
	df.fmutex.RLock()
	fmt.Println("Read get lock")
	//延遲釋放讀鎖
	defer func() {
		df.fmutex.RUnlock()
		fmt.Println("Read release lock")
	}()
	for {
		//從指定的字節偏移量處進行讀取
		_, err := df.f.ReadAt(buffer, offset)

		if err != nil {

			//如果已經讀到文件末尾
			if err == io.EOF {
				fmt.Println("eof:Read release lock")
				//堵塞等待有新的內容寫入
				df.rcond.Wait()
				fmt.Println("eof:Read get lock")
				continue
			}

		}
		d = buffer
		//通過原子操作，讓最後一次讀取的字節偏移量+=3
		atomic.AddInt64(&df.roffset, int64(df.datalen))
		return rsn, d, nil
	}
}

func (df *myDataFile) Write(d Data) (wsn int64, err error) {
	//獲取並更新偏移量
	var offset int64
	offset = atomic.LoadInt64(&df.woffset)

	//如果要寫入的數據超過一個數據塊的長度 就進行截取操作 否則直接使用
	var buffer []byte
	if len(d) > int(df.datalen) {
		buffer = d[0:df.datalen]
	} else {
		buffer = d
	}

	//加寫鎖 開始進行寫入
	df.fmutex.Lock()
	fmt.Println("Write get lock")
	//操作完成 釋放寫鎖
	defer func() {
		df.fmutex.Unlock()
		fmt.Println("Write release lock")
	}()

	//寫入數據並像讀取協程發送信號
	_, err = df.f.WriteAt(buffer, offset)
	if err != nil {
		fmt.Println("寫入錯誤")
		return
	}
	//本次寫入導致最後一次寫入的字節偏移量增加
	atomic.AddInt64(&df.woffset, int64(df.datalen))

	//計算本次寫入的塊序列號
	wsn = offset / int64(df.datalen)
	fmt.Println("write signal")
	//向讀取協程發送更新完成
	df.rcond.Signal()

	return
}

func main() {
	var wg sync.WaitGroup
	df, e := NewDataFile("example.txt", 15)
	fmt.Println(df, e)

	//寫入3個數據塊
	wg.Add(2)
	go func() {
		sr, err := df.Write(Data("明月幾時有"))
		fmt.Println("sr,err=", sr, err)

		sr, err = df.Write(Data("把酒問青天"))
		fmt.Println("sr,err=", sr, err)

		sr, err = df.Write(Data("不知天上宮闕"))
		fmt.Println("sr,err=", sr, err)
		wg.Done()
	}()
	//讀出3個數據塊
	go func() {
		rsn, data, err := df.Read()
		fmt.Println("rsn,data,err=", rsn, string(data), err)

		rsn, data, err = df.Read()
		fmt.Println("rsn,data,err=", rsn, string(data), err)

		rsn, data, err = df.Read()
		fmt.Println("rsn,data,err=", rsn, string(data), err)
		wg.Done()
	}()

	wg.Wait()
}

```




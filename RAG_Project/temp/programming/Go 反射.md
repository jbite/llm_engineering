# Go 反射

運行時通過反射來分析一個結構體 取得結構體本身的信息

檢查其類型和變量 (類型和取值)和方法

動態地獲取變量的各種訊息例如type kind

修改變量和調用方法

對沒有源代碼的包特別有用

使用反射 需要引用reflect包

### 反射應用場景

struct 寫入tags

函數的適配器

![image-20200528141810504](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200528141810504.png)

![image-20200528143714084](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200528143714084.png)

### API 

oType := reflect.TypeOf(obj)

* t.Name() : 結構體的名稱
* Kind(): 返回類型
  * oValue := reflect.ValueOf(obj).Kind()
* t.NumField()
* t.NumMethod()

* structField := oType.Field(i)
  * structField.Name
  * structField.Type
* method := oType.Method(i)
  * method.Name
  * methodType :=method.Type
    * argNum:=method.Type.NumIn() :參數個數
    * artType:=method.Type.In(0):第一個參數類型
* t.FieldByIndex([]int{0,1})

oValue := reflect.ValueOf(obj)

* fieldValue := oValue.Field(i)
  * fieldValue := value.Field(i).Interface() : 獲得第i個屬性值的正射形式
* fieldValue := value.FieldByIndex([]int{0,0}).Interface(): 找出第0個父結構體中的第0個屬性的值
* oPtrValue.Elem(): 獲取地址value中的值value
* oPtrValue.CanSet(): 檢查當前地址value內的值是否可以改變 可改變條件 可循址 且不來自非導出字段
* oPtrValue.Elem().SetInt(999): 設置值
* value.SetString("JACK")
* nameValue := value.FieldByName("Name")
* isValid := value.IsValid(): nil(0值)非法
* kind:= value.Kind()
* methodValue :=oValue.Method(i)
  * methodValue.Call([]reflect.Value{val1,val2}): 調用一個方法，此處主語是一個方法型的values

* 獲取

#### 反射注意事項

* reflect.Value.Kind 可以取得對應的kind常量
* type適類型 kind是類別 type和kind可能是相同的 也可能是不同的
* 使用反射獲取值 類型需要匹配
* 通過反射來修改變量 注意當使用SeXxx方法來設置

#### 通過反射修改變數值

```go
func reflect01(b interface{}){
  rVal := reflect.ValueOf(b)
  //Elem()可改變指針所指向的值
  rVal.Elem().SetInt(20)
}

func main(){
  var num int = 10
  reflect01(&num)
  fmt.Println("num=",num)
}
```

#### 反射實踐

遍歷結構體的字段 調用結構體的方法 獲取結構體標籤的值

Method()

Call(in []Value)

#### 反射練習

```go
func reflectTest2(b interface{}) {
	rVal := reflect.ValueOf(b)

	iV := rVal.Interface()

	// fmt.Printf("iv=%v iv type=%T\n", iV, iV.(Student).Name)
	switch iV.(type) {
	case Student:
		stu, _ := iV.(Student)
		fmt.Printf("學生類型 iv=%v iv type=%T\n", stu, stu.Name)

	case Monster:
		mon, _ := iV.(Monster)
		fmt.Printf("怪獸類型 iv=%v iv type=%T\n", mon, mon.Name)
	}
}
```



需求:

/*
所有商品有一些共性: 品茗 價格
個性無千無萬 自行封裝出三種商品 模擬30萬種商品
隨意給出一個商品的切片
將每間商品的所有屬性信鄩輸出到JSON文件(品名.json)
*/



```go
package main 

import (
	"encoding/json"
	"fmt"
	"os"
	"reflect"
)
//定義商品結構體
type Computer struct {
	Name   string
	Price  float64
	Color  string
	Cpu    string
	Memory int
	Disk   int
}
type TSHirt struct {
	Name  string
	Price float64
	Color string
	Size  int
	Sex   bool
}

type Car struct {
	Name  string
	Price float64
	Color string
	Brand string
	Power int
}

func main() {
	products := make([]interface{}, 0)
//將商品加入切片
	products = append(products, Computer{"外星人", 85000, "冰河藍", "AMD R7", 16, 512})
	products = append(products, TSHirt{"小熊維尼的蜂蜜", 250, "白", 45, true})
	products = append(products, Car{"Saab 9-5 Areo Turbo", 2000000, "黑", "SAAB", 310})
    
	var file_name string

	for i := 0; i < len(products); i++ {
		file_name = GetFieldValue(products[i])

		EncodeObj2JsonFile(products[i], file_name+".json")
	}
}
//將數據編碼為json寫出文件
func EncodeObj2JsonFile(objs interface{}, filename string) bool {
	dstFile, err := os.OpenFile(filename, os.O_WRONLY|os.O_CREATE, 0666)
	defer func() {
		dstFile.Close()
		fmt.Println("檔案關閉")
	}()
	encoder := json.NewEncoder(dstFile)
	err = encoder.Encode(objs)
	if err != nil {
		fmt.Println("編碼失敗,err=", err)
		return false
	} else {
		fmt.Println("編碼成功")
		return true
	}
}
//透過反射將field值讀出來
func GetFieldValue(obj interface{}) (fieldvalue string) {
	oValue := reflect.ValueOf(obj)
    //將讀出的reflect.Value類型，透過assertion轉成string類型
	fieldvalue = oValue.FieldByName("Name").Interface().(string)
	fmt.Println(fieldvalue)
	return
}
```

## INI讀取

config.ini

```ini
[mysql]
address=10.20.30.40
port=3306
username=root
password=rootroot

[redis]
host=127.0.0.1
port=6379
passwor=root
database=0
```

main.go

```go
type MysqlConfig struct{
  Address string `ini:"address"`
  Port int `ini:"port"`
  Username `ini:"username"`
  Password `ini:"password"`
}

type RedsiConfig struct{
  Host string `ini:"host"`
  Port int `ini:port`
  Password string `ini:password`
  Database int `ini:database`
}
type Config struct {
	MysqlConfig `ini:"mysql"`
	RedisConfig `ini:"redis"`
}

func loadIni(b []byte,data interface{}){
  
  if !isDataOK(data) {
		return
	}
	file, err := ioutil.ReadFile(filename)
	if err != nil {
		fmt.Println("檔案讀取問題", err)
		return
	}
	// fmt.Println(string(file))
	//切割文件
	strs := cutIni(file)
	var section string
	var configField string
	var key string
	var val string
	for _, str := range strs {
		//跳過注釋
		if IsAnnotate(str) {
			continue
		}
		//如果是[mysql]或[redis]這樣的字段 就當作節
		if IsSection(str) {
			section = string(Section(str)[1])
			//找出configField
			configField = SetConfigField(data, section)
			fmt.Println("找到configField:", configField)
		} else {
			key, val = KeyAndValue(str)
			if key != "" {
				// fmt.Println(key, val)
//取得configField中的結構體值
structVal := reflect.ValueOf(data).Elem().FieldByName(configField)
//將結構體值轉換成reflect.StructType

sType := structVal.Type()
for i := 0; i < sType.NumField(); i++ {
  //轉成type之後才能取得Tag內的訊息
	field := sType.Field(i).Tag.Get("ini")
  //有Tag訊息之後就能開始讀取要修改的key及value
	if field == key {
		switch structVal.Field(i).Type().Kind() {
			case reflect.String:
				structVal.Field(i).SetString(val)
			case reflect.Int, reflect.Int8, reflect.Int16, reflect.Int32, reflect.Int64:
				valint, _ := strconv.ParseInt(val, 10, 64)
				structVal.Field(i).SetInt(valint)
						}
					}
				}
			}
		}
	}
  //1.讀文件得到字節類型數據
  //2.一行一行讀文件
  //2.1如果是注釋 就跳過
  //2.2如果是方括號[]開頭 就表示是節
  //2.3如果是文字開頭就是分割的鍵值對
}
func KeyAndValue(str string) (key, val string) {
	temp := strings.Split(str, "=")
	if len(temp) != 2 {
		fmt.Println(str, "格式錯誤")
		return
	}
	if len(temp[0]) == 0 || len(temp[1]) == 0 {
		fmt.Println(str, "有值為空")
		return
	}
	key = strings.TrimSpace(temp[0])
	val = strings.TrimSpace(temp[1])
	return
}

//SetConfigField 判斷configField是什麼
func SetConfigField(data interface{}, section string) string {
	confObj := reflect.TypeOf(data).Elem()
	for i := 0; i < confObj.NumField(); i++ {
		tag := confObj.Field(i).Tag.Get("ini")
		if tag == section {
			//取得Config struct內含字段的名稱
			return confObj.Field(i).Name
		}
	}
	return ""
}

//判斷data為struct且為指針
func isDataOK(data interface{}) bool {
  //0.參數的校驗
  //0.1傳進來的參數必須是指針(因為需要在函數中對其賦值)
  //0.2傳進來的data參數必須是結構體類型指針(因為配置文件中各種鍵值對需要付值給結構體的字段)
	obj := reflect.TypeOf(data)
	if obj.Kind() == reflect.Ptr && obj.Elem().Kind() == reflect.Struct {
		return true
	}
	if obj.Kind() != reflect.Ptr {
		fmt.Println("data is not pointer")
	}

	if obj.Elem().Kind() != reflect.Struct {
		fmt.Println("data is not struct")
	}
	return false
}

func cutIni(file []byte) []string {
	strs := strings.Split(string(file), "\r\n")

	for i, str := range strs {
		strs[i] = strings.TrimSpace(str)
	}
	return strs
}

func IsAnnotate(str string) bool {
	return strings.HasPrefix(str, "#") || strings.HasPrefix(str, ";")
}

func IsSection(str string) bool {
	if m, _ := regexp.MatchString(`\[\s*(\S+)\s*\]`, str); m {
		return true
	}
	return false
}
func Section(str string) (s [][]byte) {
	b := []byte(str)
	// fmt.Println(str)
	re := regexp.MustCompile(`\[\s*(\S+)\s*\]`)
	s = re.FindSubmatch(b)
	return s
}
func main(){
 	var mc MysqlConfig 
  err:=loadIni("./conf.ini",&mc)
  if err !=nil{
    fmt.Printf("load ini failed,err %v\n",err)
  	return
  }
  fmt.Println(mc.Address,mc.Port,mc.Username,mc.Password)
}
```




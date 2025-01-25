# GoLang GUI 圖形用戶介面

## 依賴庫

walk



```
go get github.com/lxn/walk
```

## 運行範例

1. 創建一個test 目錄

2. 在test新增test.go

   ```go
   package main
   
   import (
   	"github.com/lxn/walk"
   	. "github.com/lxn/walk/declarative"
   	"strings"
   )
   
   func main() {
   	var inTE, outTE *walk.TextEdit
   
   	MainWindow{
   		Title:   "SCREAMO",
   		MinSize: Size{600, 400},
   		Layout:  VBox{},
   		Children: []Widget{
   			HSplitter{
   				Children: []Widget{
   					TextEdit{AssignTo: &inTE},
   					TextEdit{AssignTo: &outTE, ReadOnly: true},
   				},
   			},
   			PushButton{
   				Text: "SCREAM",
   				OnClicked: func() {
   					outTE.SetText(strings.ToUpper(inTE.Text()))
   				},
   			},
   		},
   	}.Run()
   }
   ```

3. 在test目錄下新增``test.manifest``

   ```xml
   <?xml version="1.0" encoding="UTF-8" standalone="yes"?>
   <assembly xmlns="urn:schemas-microsoft-com:asm.v1" manifestVersion="1.0">
       <assemblyIdentity version="1.0.0.0" processorArchitecture="*" name="SomeFunkyNameHere" type="win32"/>
       <dependency>
           <dependentAssembly>
               <assemblyIdentity type="win32" name="Microsoft.Windows.Common-Controls" version="6.0.0.0" processorArchitecture="*" publicKeyToken="6595b64144ccf1df" language="*"/>
           </dependentAssembly>
       </dependency>
       <application xmlns="urn:schemas-microsoft-com:asm.v3">
           <windowsSettings>
               <dpiAwareness xmlns="http://schemas.microsoft.com/SMI/2016/WindowsSettings">PerMonitorV2, PerMonitor</dpiAwareness>
               <dpiAware xmlns="http://schemas.microsoft.com/SMI/2005/WindowsSettings">True</dpiAware>
           </windowsSettings>
       </application>
   </assembly>
   ```

4. 在test中透過cmd中執行已下命令

   ```
   go get github.com/akavel/rsrc
   rsrc -manifest test.manifest -o rsrc.syso
   ```

5. 創建執行檔

   ```
   go build -ldflags="-H windowsgui"
   ```

![alt tag](https://camo.githubusercontent.com/ac1992d5d6bdaa0091b757fdaed23094c9c64b8f/687474703a2f2f692e696d6775722e636f6d2f6c5572674532512e706e67)
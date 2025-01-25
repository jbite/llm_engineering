# GoLang 多併發 下載網頁圖片

```go
package main

import (
	"fmt"
	"io/ioutil"
	"net/http"
	"os"
	"regexp"
	"strconv"
	"strings"
	"sync"
)

var (
	chSem = make(chan int, 5)
)
var (
	rePhone = `1[3658][457][28]{8}`
	reImg   = `<img([\s\S\]+)?src="(http:[\s\S]+?.jpeg)"`
)

func main() {
	var wg sync.WaitGroup
	urls := spiderImg("https://www.lasvegasch.com/482.html", reImg)
	var index int
	wg.Add(len(urls))
	for _, url := range urls {
		filename := strconv.Itoa(index) + ".jpeg"
		DownloadImgAsync(url, filename, &wg)
		index++
	}
	wg.Wait()
}

func GetHtml(url string) string {
	resp, err := http.Get(url)
	HadleError(err, "GetHtml http.Get"+url)
	str, _ := ioutil.ReadAll(resp.Body)
	return string(str)
}

func GetPageImgUrls(url string, regstr string) {
	html := GetHtml(url)
	fmt.Println(regstr)
	re := regexp.MustCompile(regstr)
	rets := re.FindAllString(html, -1)
	fmt.Println("捕獲圖片張數", len(rets))

	for _, ret := range rets {
		var s []string
		s = strings.Split(ret, "\"")
		fmt.Println(s[1])
	}
}
func spiderImg(url string, regstr string) (imgUrls []string) {
	imgUrls = make([]string, 0)
	html := GetHtml(url)
	re := regexp.MustCompile(regstr)
	rets := re.FindAllString(html, -1)
	fmt.Println("捕獲圖片張數", len(rets))

	for _, ret := range rets {
		var s []string
		s = strings.Split(ret, "\"")
		imgUrls = append(imgUrls, s[1])
	}
	return
}

func DownloadImg(url string, filename string) {
	fmt.Println(filename, "開始下載")
	resp, _ := http.Get(url)
	defer resp.Body.Close()
	imgBytes, _ := ioutil.ReadAll(resp.Body)
	e := ioutil.WriteFile(filename, imgBytes, 0644)
	if e != nil {
		HadleError(e, "ioutil.WriteFile")
		fmt.Println("下載失敗")
	} else {
		fmt.Println("下載成功")
	}
}

func DownloadImgAsync(url string, filename string, wg *sync.WaitGroup) {

	go func() {
		chSem <- 1
		DownloadImg(url, filename)
		<-chSem
		wg.Done()
	}()

}
func HadleError(err error, when string) {
	if err != nil {
		fmt.Println(when, err)
		os.Exit(1)
	}
}

```


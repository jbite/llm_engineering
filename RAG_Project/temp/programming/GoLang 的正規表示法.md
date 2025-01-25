# GoLang 的正規表示法

#### 常用舉例

* 手機
* 郵箱
* 超鏈接
* 身分證號
* 圖片鏈接

### 常用規則

`\d`: 數字

`\D`:非數字

`\w`: 單詞字符 大小寫字母 +數字+ 下劃線

`\W`: 非單詞字符

`\s`: 空白字符 \t \n \r \f

`\S`: 非空白字符 

`.`: 換行符以外的任意字符

`re+`: re表示的片段出現1到多次

`re*`: re表示的片段出現0到多次

`re?`: re表示的片段出現0到1次

`re{n}`: re表示的片段出現n次

`re{m,n}`: re表示的片段出現m到n次

`re{m,}`: re表示的片段出現m到無限次

`[abc]`: a b c中間的一個字符

`[^abc]`: 除了a b c中間的一個字符

`[\s\S]`:所有字符

`[a-z]`: a-z的任一字符

`re1|re2`:re1或re2所表示的片段

`^re$`: 剛好匹配re的字符串





### API

```go
re := regexp.MustCompile(reStr)
ret := re.FindAllStringSubmatch(srcStr,-1)
```


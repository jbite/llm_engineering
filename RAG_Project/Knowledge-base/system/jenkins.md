# jenkins

CI/CD持續集成及自動化發布

開發(git) -->git主庫--> (git+jdk+tomcat+maven打包+測試)-->發佈到tomcat伺服器



是一款開源的 提供友好操作介面的持續集成工具。起源於hudson，主要用於持續自動的構建測試軟件項目 監控一些定時執行的任務。jenkins用java語言編寫 

* jenkins目標
  * 監控軟體開發流程
  * 快速顯示問題
  * 提高開發效率
  * 過程控制

* jenkins特性
  * 易於配置
  * 文件識別
  * 分布式構建
  * 插件支持
  * 任務
  * 工作流程



### Lab準備

* jenkins主機

  * ```
    yum install -y curl-devel expat-devel gettext-devel openssl-devel zlib-devel gcc perl-EctUtils-MakeMaker 
    ```

  * 

* tomcat主機
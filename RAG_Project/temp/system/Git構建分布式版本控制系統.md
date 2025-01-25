# Git構建分布式版本控制系統

版本控制: 紀錄開發時間的概念

### 版本控制分類

* 本地版本控制
* 集中化的版本控制系統: VCS
* 分布式版本控制

## 私有版本控制Gitlab

* 安裝環境及需求

  ```
  yum install curl policycoreutils openssh-server openssh-clients -y
  systemctl enable sshd
  systemctl start sshd
  firewall-cmd --permanent --add-service=http
  firewall-cmd --permanent --add-service=https
  systemctl reload firewalld
  yum install postfix -y
  systemctl enable postfix
  systemctl start postfix
  
  ```

* 建立gitlab的yum源

  ```
  curl https://packages.gitlab.com/install/repositories/gitlab/gitlab-ee/script.rpm.sh | sudo bash
  
  # 安裝並啟動gitlab
  EXTERNAL_URL="https://gitlab.example.com" yum install -y gitlab-ee
  
  ```

* 登入gitlab

  ![image-20200426113604096](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200426113604096.png)

* 創建項目
  * ![image-20200426114317593](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200426114317593.png)

* 命令行使用

  ```
  git config --global user.name "git"
  git config --global user.email "git@gitlab.example.com"
  git clone git@github.com:jbite/jumpserver.git
  ```

  

### Git global setup

```
git config --global user.name "Administrator"
git config --global user.email "admin@example.com"
```

### Create a new repository

```
git clone git@gitlab.example.com:root/jumpserver.git
cd jumpserver
touch README.md
git add README.md
git commit -m "add README"
git push -u origin master
```

### Push an existing folder

```
cd existing_folder
git init
git remote add origin git@github.com:jbite/jumpserver.git
git add .
git commit -m "Initial commit"
git push -u origin master
```

### Push an existing Git repository

```
cd existing_repo
git remote rename origin old-origin
git remote add origin git@github.com:jbite/jumpserver.git
git push -u origin --all
git push -u origin --tags
```


OPENSTACK 全新架構實戰

雲計算服務類型

* IAAS  基礎設施即服務
* PAAS 平台即服務
* SAAS  軟體即服務



![image-20201019173212883](https://docs.openstack.org/install-guide/_images/openstack_kilo_conceptual_arch.png)

OpenStack 屬於IAAS。KVM虛擬化的管理平台

虛擬機管理平台 每台虛擬機的管理 都用數據庫來統計

openstack 是開源的雲計算平台 apaxhe 2.0開源協議 阿里雲 青雲

PAAS docker

6. openstack (SOA架構)

* keystone認證服務*
* glance鏡像服務*
* nova計算服務*
* neutron網路服務*
* cinder存儲服務*
* horizon web 介面*
* switft 對象存儲
* heat 編排
* ceilmeter監控
* trove資料庫服務*
* sahara數據處理



每個服務: 數據庫 消息對列 memcache緩存 時間同步



nginx + php _ mysql

SOA 拆業務 把每一個功能都拆成一個獨立的web服務  每一個獨立的web服務 都擁有至少一個集群。千萬用戶同時訪問

首頁 www.jd.com/index.html

秒殺 miaosha.jd.com

優惠券a.jd.com

會員 

登陸





微服務架構: 億級用戶

開源的微服務框架

阿里 dubbo

spring boot



自動化代碼上線 Jenkins + gitlab ci

自動化代碼質量檢查 sonarqube





#### 虛擬機規劃

安裝步驟:

> controller 192.168.56.11 memory 1.5G 2C
>
> computer1 192.168.56.31 memory 4M 4C
>
> block      192.168.56.41 2C 2M 
>
> 開啟虛擬化

修改主機名

```
hostnamectl set-hostname controller
```

 ip地址網關 主機名解析 



NTP

```
vi /etc/chrony.conf
# Allow NTP client access from local network.
systemctl restart chronyd
```

對時間同步要求很高



設定base源

```
curl -o /etc/yum.repos.d/CentOS-Base.repo http://mirrors.aliyun.com/repo/Centos-7.repo

yum makecache
yum list | grep openstack
yum install centos-release-openstack-stein -y
```

在所有節點執行:

```
yum install python2-openstackclient.noarch openstack-selinux -y
```

### 僅在控制節點安裝:

#### 安裝資料庫

```
yum install mariadb mariadb-server python2-PyMySQL -y
```

* 設定檔/etc/my.cnf.d/openstack.cnf

* ```
  vi /etc/my.cnf.d/openstack.cnf
  [mysqld]
  bind-address = 10.0.2.11
  
  default-storage-engine = innodb
  innodb_file_per_table = on
  max_connections = 4096
  collation-server = utf8_general_ci
  character-set-server = utf8
  ```

* ```
  systemctl enable mariadb.service
  systemctl start mariadb.service
  ```

* 設定安全值

* ```
  mysql_secure_installation
  ```



#### 安裝RabbitMQ

```
yum install rabbitmq-server -y
systemctl enable rabbitmq-server.service
systemctl start rabbitmq-server.service
```

創建用戶

```
rabbitmqctl add_user openstack RABBIT_PASS
```

設定用戶權限

```
rabbitmqctl set_permissions openstack ".*" ".*" ".*"
```

> rabbitMQ監控兩個端口tcp 5672 tcp25672

啟用監控頁面

```
rabbitmq-plugins enable rabbitmq_management
```

> 會多一個監控端口 15672, 透過瀏覽器存取192.168.56.11:15672, guest/guest



#### 安裝memcache

```
yum install memcached python-memcached -y
```

編輯設定

```
sed -i 's#OPTIONS="-l 127.0.0.1,::1"#OPTIONS="-l 192.168.56.11,::1,controller"#g' /etc/sysconfig/memcached
```

啟用及執行

```
systemctl enable memcached.service
systemctl start memcached.service
```

memcache建立11211端口



#### 安裝ETCD

```
yum install etcd -y
```

修改**/etc/etcd/etcd.conf**

```
#[Member]
ETCD_DATA_DIR="/var/lib/etcd/default.etcd"
ETCD_LISTEN_PEER_URLS="http://10.0.0.11:2380"
ETCD_LISTEN_CLIENT_URLS="http://10.0.0.11:2379"
ETCD_NAME="controller"
#[Clustering]
ETCD_INITIAL_ADVERTISE_PEER_URLS="http://10.0.0.11:2380"
ETCD_ADVERTISE_CLIENT_URLS="http://10.0.0.11:2379"
ETCD_INITIAL_CLUSTER="controller=http://10.0.0.11:2380"
ETCD_INITIAL_CLUSTER_TOKEN="etcd-cluster-01"
ETCD_INITIAL_CLUSTER_STATE="new"
```

啟用並執行

```
systemctl enable etcd
systemctl start etcd
```

Nova 提供虛擬機環境

Glance 提供鏡像管理

Cinder 提供VM卷服務

Neutron 提供VM網路服務

Cellometer 監控計費

Keystone 驗證服務

Switft 鏡像存儲服務

Horizon 提供各功能的UI服務

Heat 編排服務 透過預先編排 批量處理虛擬機建置



#### openstack 服務安裝的通用步驟

* 創建資料庫並授權設置密碼
* 在keystone上創建用戶，關聯角色
* 在keystone上創建服務 註冊API(catalog功能)
* 安裝服務相關的軟體包
* 修改配置文件 
  * 資料庫的連接資訊
  * keystone的認證授權資訊 
  * rabbitMQ的連接資訊 
  * 其他配置
* 同步資料庫 創建表
* 啟動服務



#### Keystone

功能:

* 認證管理
  * 帳號密碼
  * 密鑰登入
* 授權管理
  * 管理權限
* 服務目錄
  * 電話簿
  * 協助管理者紀錄安裝在openstack中的服務的url，讓管理者快速進入該服務的頁面

#### 安裝keystone

##### 建立並授權資料庫

```
mysql -u root -p
```

創建keystone資料庫並授權

```
CREATE DATABASE keystone;
GRANT ALL PRIVILEGES ON keystone.* TO 'keystone'@'localhost' \
IDENTIFIED BY 'KEYSTONE_DBPASS';

GRANT ALL PRIVILEGES ON keystone.* TO 'keystone'@'%' \
IDENTIFIED BY 'KEYSTONE_DBPASS';
```

##### 安裝並配置元件

```
yum install openstack-keystone httpd mod_wsgi -y
```

> web服務與程式碼溝通:
>
> nginx<---fastcgi---> php
>
> nginx <---uwsgi---> python
>
> httpd <---wsgi----->pyhton

安裝openstack配置工具

```
yum install openstack-utils -y
```

配置/etc/keystone/keystone.conf

```
cp /etc/keystone/keystone.conf{,.bak}
grep -Ev "^$|#" /etc/keystone/keystone.conf.bak >  /etc/keystone/keystone.conf

openstack-config --set /etc/keystone/keystone.conf database connection mysql+pymysql://keystone:KEYSTONE_DBPASS@controller/keystone

openstack-config --set /etc/keystone/keystone.conf token provider fernet
```

keystone認證方式UUID PKI Fernet 都是生成隨機字符串的方法

##### 填充驗證服務資料庫

```
su -s /bin/sh -c "keystone-manage db_sync" keystone
```

查詢資料庫同步內容

```
mysql -u root -ppassword keystone -e 'show tables;'
+------------------------------------+
| Tables_in_keystone                 |
+------------------------------------+
| access_rule                        |
| access_token                       |
| application_credential             |
| application_credential_access_rule |
| application_credential_role        |
| assignment                         |
| config_register                    |
| consumer                           |
| credential                         |
| endpoint                           |
| endpoint_group                     |
| federated_user                     |
| federation_protocol                |
| group                              |
| id_mapping                         |
| identity_provider                  |
| idp_remote_ids                     |
| implied_role                       |
| limit                              |
| local_user                         |
| mapping                            |
| migrate_version                    |
| nonlocal_user                      |
| password                           |
| policy                             |
| policy_association                 |
| project                            |
| project_endpoint                   |
| project_endpoint_group             |
| project_option                     |
| project_tag                        |
| region                             |
| registered_limit                   |
| request_token                      |
| revocation_event                   |
| role                               |
| role_option                        |
| sensitive_config                   |
| service                            |
| service_provider                   |
| system_assignment                  |
| token                              |
| trust                              |
| trust_role                         |
| user                               |
| user_group_membership              |
| user_option                        |
| whitelisted_config                 |
+------------------------------------+
```

##### 初始化Fernet密鑰倉庫

```
keystone-manage fernet_setup --keystone-user keystone --keystone-group keystone
keystone-manage credential_setup --keystone-user keystone --keystone-group keystone
```

##### 引導身分服務

```
keystone-manage bootstrap --bootstrap-password ADMIN_PASS \
  --bootstrap-admin-url http://controller:5000/v3/ \
  --bootstrap-internal-url http://controller:5000/v3/ \
  --bootstrap-public-url http://controller:5000/v3/ \
  --bootstrap-region-id RegionOne
```

##### 配置httpd

```
echo "ServerName controller" >> /etc/httpd/conf/httpd.conf
```

```
ln -s /usr/share/keystone/wsgi-keystone.conf /etc/httpd/conf.d/
```

```
systemctl enable httpd.service
systemctl start httpd.service
```

##### 配置管理帳戶

先輸入環境變數

```
export OS_USERNAME=admin
export OS_PASSWORD=ADMIN_PASS
export OS_PROJECT_NAME=admin
export OS_USER_DOMAIN_NAME=Default
export OS_PROJECT_DOMAIN_NAME=Default
export OS_AUTH_URL=http://controller:5000/v3
export OS_IDENTITY_API_VERSION=3
```

##### 創建域項目用戶及角色

```
openstack domain create --description "An Example Domain" example

openstack project create --domain default   --description "Service Project" service

openstack project create --domain default   --description "Demo Project" myproject

openstack user create --domain default   --password MYUSER_PASS myuser

openstack role create myrole

openstack role add --project myproject --user myuser myrole
```

#### 創建openstack 用戶環境腳本

建立admin腳本

```
echo 'export OS_PROJECT_DOMAIN_NAME=Default
export OS_USER_DOMAIN_NAME=Default
export OS_PROJECT_NAME=admin
export OS_USERNAME=admin
export OS_PASSWORD=ADMIN_PASS
export OS_AUTH_URL=http://controller:5000/v3
export OS_IDENTITY_API_VERSION=3
export OS_IMAGE_API_VERSION=2' > ~/admin-openrc
```

建立demo腳本

```
echo 'export OS_PROJECT_DOMAIN_NAME=Default
export OS_USER_DOMAIN_NAME=Default
export OS_PROJECT_NAME=myproject
export OS_USERNAME=myuser
export OS_PASSWORD=MYUSER_PASS
export OS_AUTH_URL=http://controller:5000/v3
export OS_IDENTITY_API_VERSION=3
export OS_IMAGE_API_VERSION=2' > ~/demo-openrc
```

開機自啟動

```
echo '. admin-openrc' >> ~/.bashrc
```

keystone完成安裝



#### Glance鏡像服務安裝

> 允許用戶發現 註冊和獲取虛擬機鏡像

1. 創建資料庫

```
mysql -u root -p
```

```
CREATE DATABASE glance;
GRANT ALL PRIVILEGES ON glance.* TO 'glance'@'localhost' \
  IDENTIFIED BY 'GLANCE_DBPASS';
GRANT ALL PRIVILEGES ON glance.* TO 'glance'@'%' \
  IDENTIFIED BY 'GLANCE_DBPASS';
```

2. keystone中創建用戶

```
openstack user create --domain default --password GLANCE_PASS glance
```

3. Keystone中授權角色

```
openstack role add --project service --user glance admin
```

4. 在keystone創建image服務

```
openstack service create --name glance --description "OpenStack Image" image
+-------------+----------------------------------+
| Field       | Value                            |
+-------------+----------------------------------+
| description | OpenStack Image                  |
| enabled     | True                             |
| id          | 1b7d57459e314fe0909027d907d84c20 |
| name        | glance                           |
| type        | image                            |
+-------------+----------------------------------+ 
```

5. 在keystone創建映像服務API端點

```
openstack endpoint create --region RegionOne image public http://controller:9292

openstack endpoint create --region RegionOne image internal http://controller:9292
  
openstack endpoint create --region RegionOne image admin http://controller:9292

```

6. 安裝glance服務軟體包

```
yum install openstack-glance -y
```

7. 編輯**/etc/glance/glance-api.conf**

```
cp /etc/glance/glance-api.conf{,.bak}
grep -Ev "^$|#" /etc/glance/glance-api.conf.bak > /etc/glance/glance-api.conf

openstack-config --set /etc/glance/glance-api.conf database connection mysql+pymysql://glance:GLANCE_DBPASS@controller/glance

openstack-config --set /etc/glance/glance-api.conf keystone_authtoken auth_uri http://controller:5000
openstack-config --set /etc/glance/glance-api.conf keystone_authtoken auth_url http://controller:5000
openstack-config --set /etc/glance/glance-api.conf keystone_authtoken memcached_servers controller:11211
openstack-config --set /etc/glance/glance-api.conf keystone_authtoken auth_type password
openstack-config --set /etc/glance/glance-api.conf keystone_authtoken project_domain_name Default
openstack-config --set /etc/glance/glance-api.conf keystone_authtoken user_domain_name Default
openstack-config --set /etc/glance/glance-api.conf keystone_authtoken project_name service
openstack-config --set /etc/glance/glance-api.conf keystone_authtoken username glance
openstack-config --set /etc/glance/glance-api.conf keystone_authtoken password GLANCE_PASS

openstack-config --set /etc/glance/glance-api.conf paste_deploy flavor keystone

openstack-config --set /etc/glance/glance-api.conf glance_store stores file,http
openstack-config --set /etc/glance/glance-api.conf glance_store default_store file
openstack-config --set /etc/glance/glance-api.conf glance_store filesystem_store_datadir /var/lib/glance/images/
```

8. 編輯**/etc/glance/glance-registry.conf**

```
cp /etc/glance/glance-registry.conf{,.bak}
grep -Ev "^$|#" /etc/glance/glance-registry.conf.bak > /etc/glance/glance-registry.conf

openstack-config --set /etc/glance/glance-registry.conf database connection mysql+pymysql://glance:GLANCE_DBPASS@controller/glance

openstack-config --set /etc/glance/glance-registry.conf keystone_authtoken auth_uri http://controller:5000
openstack-config --set /etc/glance/glance-registry.conf keystone_authtoken auth_url http://controller:5000
openstack-config --set /etc/glance/glance-registry.conf keystone_authtoken memcached_servers controller:11211
openstack-config --set /etc/glance/glance-registry.conf keystone_authtoken auth_type password
openstack-config --set /etc/glance/glance-registry.conf keystone_authtoken project_domain_name Default
openstack-config --set /etc/glance/glance-registry.conf keystone_authtoken user_domain_name Default
openstack-config --set /etc/glance/glance-registry.conf keystone_authtoken project_name service
openstack-config --set /etc/glance/glance-registry.conf keystone_authtoken username glance
openstack-config --set /etc/glance/glance-registry.conf keystone_authtoken password GLANCE_PASS

openstack-config --set /etc/glance/glance-registry.conf paste_deploy flavor keystone
```

9. 更新資料庫

```
su -s /bin/sh -c "glance-manage db_sync" glance
```

10. 啟用

```
systemctl enable openstack-glance-api.service openstack-glance-registry.service
systemctl start openstack-glance-api.service openstack-glance-registry.service
```

監聽9191及9292端口

#### 測試鏡像

```
yum install wget -y
```

```
wget http://download.cirros-cloud.net/0.4.0/cirros-0.4.0-x86_64-disk.img
```

測試上傳鏡像

```
openstack image create "cirros" \
  --file cirros-0.4.0-x86_64-disk.img \
  --disk-format qcow2 --container-format bare \
  --public
```

#### 安裝placement服務

1. 創建placement資料庫並授權

   * ```
     mysql -u root -p
     CREATE DATABASE placement;
     GRANT ALL PRIVILEGES ON placement.* TO 'placement'@'localhost' \
       IDENTIFIED BY 'PLACEMENT_DBPASS';
       
     GRANT ALL PRIVILEGES ON placement.* TO 'placement'@'%' \
       IDENTIFIED BY 'PLACEMENT_DBPASS';
     ```

2. 在keystone註冊placement服務

   * ```
     openstack user create --domain default --password PLACEMENT_PASS placement
     
     openstack role add --project service --user placement admin
     
     openstack service create --name placement \
       --description "Placement API" placement
       
     openstack endpoint create --region RegionOne \
       placement public http://controller:8778
     
     openstack endpoint create --region RegionOne \
       placement internal http://controller:8778
     
     openstack endpoint create --region RegionOne \
       placement admin http://controller:8778
     ```

3. 安裝placement軟體包

   * ```
     yum install openstack-placement-api -y
     ```

4. 配置/etc/placement/placement.conf

   * ```
     cp /etc/placement/placement.conf{,.bak}
     grep -Ev "^$|#" /etc/placement/placement.conf.bak > /etc/placement/placement.conf
     
     
     
     openstack-config --set /etc/placement/placement.conf placement_database connection mysql+pymysql://placement:PLACEMENT_DBPASS@controller/placement
     
     openstack-config --set /etc/placement/placement.conf api auth_strategy keystone
     
     openstack-config --set /etc/placement/placement.conf keystone_authtoken auth_url http://controller:5000/v3
     openstack-config --set /etc/placement/placement.conf keystone_authtoken memcached_servers controller:11211
     openstack-config --set /etc/placement/placement.conf keystone_authtoken auth_type password
     openstack-config --set /etc/placement/placement.conf keystone_authtoken project_domain_name Default
     openstack-config --set /etc/placement/placement.conf keystone_authtoken user_domain_name Default
     openstack-config --set /etc/placement/placement.conf keystone_authtoken project_name service
     openstack-config --set /etc/placement/placement.conf keystone_authtoken username placement
     openstack-config --set /etc/placement/placement.conf keystone_authtoken password PLACEMENT_PASS
     ```

5. 同步資料庫

   * ```
     su -s /bin/sh -c "placement-manage db sync" placement
     ```

6. 修改Placement的apache配置文件

   ```
   #启用placement API访问
   [root@controller ~]# vim /etc/httpd/conf.d/00-placement-api.conf
    ...
   15   #SSLCertificateKeyFile
     #SSLCertificateKeyFile ...
   <Directory /usr/bin>
      <IfVersion >= 2.4>
         Require all granted
      </IfVersion>
      <IfVersion < 2.4>
         Order allow,deny
         Allow from all
      </IfVersion>
   </Directory>
   ...
   
   #重启apache服务
   systemctl restart httpd.service
   netstat -lntup|grep 8778
   lsof -i:8778
   
   #curl地址看是否能返回json
   curl http://controller:8778
   ```
   
7. 

8. 完成安裝

   * ```
     systemctl restart httpd
     ```

### NOVA計算服務安裝

nova-api 

接受並響應來自最終用戶的計算API請求 此服務支持openstack計算服務API Amazon EC2 API以及特殊的管理API用於賦予用戶做一些管理的操作 他會強制實施一些規則發起多數的

nova-compute

​	真正管理虛擬機

nova-scheduler

​	nova調度器(挑選出最適合的nova-compute來創建虛擬機)

nova-conductor

​	幫助nova-compute代理修改資料庫中虛擬機的狀態

nova-network

​	早期openstack版本管理虛擬機的網路(改用neutron)

nova-consoleauth和nova-novncproxy

​	web版的vnc來直接操作雲主機

novncoroxy

​	web版vnc客戶端

nova-api-metadata

​	接受來自虛擬機發送的源數據請求(配合neutron-metadata-agent來虛擬機定製化)

##### controller安裝步驟

1. 創庫授權

```
mysql -u root -p
CREATE DATABASE nova_api;
CREATE DATABASE nova;
CREATE DATABASE nova_cell0;

GRANT ALL PRIVILEGES ON nova_api.* TO 'nova'@'localhost' \
  IDENTIFIED BY 'NOVA_DBPASS';
GRANT ALL PRIVILEGES ON nova_api.* TO 'nova'@'%' \
  IDENTIFIED BY 'NOVA_DBPASS';

GRANT ALL PRIVILEGES ON nova.* TO 'nova'@'localhost' \
  IDENTIFIED BY 'NOVA_DBPASS';
GRANT ALL PRIVILEGES ON nova.* TO 'nova'@'%' \
  IDENTIFIED BY 'NOVA_DBPASS';

GRANT ALL PRIVILEGES ON nova_cell0.* TO 'nova'@'localhost' \
  IDENTIFIED BY 'NOVA_DBPASS';
GRANT ALL PRIVILEGES ON nova_cell0.* TO 'nova'@'%' \
  IDENTIFIED BY 'NOVA_DBPASS';
```

2. 在keystone創建系統用戶並註冊角色到用戶上

```
openstack user create --domain default --password NOVA_PASS nova

openstack role add --project service --user nova admin
#在keystone註冊NOVA服務
openstack service create --name nova --description "OpenStack Compute" compute
```

3. 創建計算API服務節點

```
openstack endpoint create --region RegionOne compute public http://controller:8774/v2.1

openstack endpoint create --region RegionOne compute internal http://controller:8774/v2.1

openstack endpoint create --region RegionOne compute admin http://controller:8774/v2.1
```

4. 安裝並配置軟體包

```
yum -y install openstack-nova-api openstack-nova-conductor openstack-nova-console openstack-nova-novncproxy  openstack-nova-scheduler -y
```

編輯/etc/nova/nova.conf

```
cp /etc/nova/nova.conf{,.bak}
grep -Ev "^$|#" /etc/nova/nova.conf.bak > /etc/nova/nova.conf

openstack-config --set /etc/nova/nova.conf DEFAULT enabled_apis osapi_compute,metadata
openstack-config --set /etc/nova/nova.conf DEFAULT transport_url rabbit://openstack:RABBIT_PASS@controller
openstack-config --set /etc/nova/nova.conf DEFAULT my_ip 10.0.2.11
openstack-config --set /etc/nova/nova.conf DEFAULT use_neutron true
openstack-config --set /etc/nova/nova.conf DEFAULT firewall_driver nova.virt.firewall.NoopFirewallDriver

openstack-config --set /etc/nova/nova.conf api_database connection mysql+pymysql://nova:NOVA_DBPASS@controller/nova_api

openstack-config --set /etc/nova/nova.conf database connection mysql+pymysql://nova:NOVA_DBPASS@controller/nova

openstack-config --set /etc/nova/nova.conf api auth_strategy keystone

openstack-config --set /etc/nova/nova.conf keystone_authtoken auth_url http://controller:5000/v3
openstack-config --set /etc/nova/nova.conf keystone_authtoken memcached_servers controller:11211
openstack-config --set /etc/nova/nova.conf keystone_authtoken auth_type password
openstack-config --set /etc/nova/nova.conf keystone_authtoken project_domain_name Default
openstack-config --set /etc/nova/nova.conf keystone_authtoken user_domain_name Default
openstack-config --set /etc/nova/nova.conf keystone_authtoken project_name service
openstack-config --set /etc/nova/nova.conf keystone_authtoken username nova
openstack-config --set /etc/nova/nova.conf keystone_authtoken password NOVA_PASS


openstack-config --set /etc/nova/nova.conf vnc enabled true
openstack-config --set /etc/nova/nova.conf vnc server_listen '$my_ip'
openstack-config --set /etc/nova/nova.conf vnc server_proxyclient_address '$my_ip'

openstack-config --set /etc/nova/nova.conf glance api_servers http://controller:9292

openstack-config --set /etc/nova/nova.conf oslo_concurrency lock_path /var/lib/nova/tmp

openstack-config --set /etc/nova/nova.conf placement region_name RegionOne
openstack-config --set /etc/nova/nova.conf placement project_domain_name Default
openstack-config --set /etc/nova/nova.conf placement project_name service
openstack-config --set /etc/nova/nova.conf placement auth_type password
openstack-config --set /etc/nova/nova.conf placement user_domain_name Default
openstack-config --set /etc/nova/nova.conf placement auth_url http://controller:5000/v3
openstack-config --set /etc/nova/nova.conf placement username placement
openstack-config --set /etc/nova/nova.conf placement password PLACEMENT_PASS
```

※補充: python library路徑 /lib/python2.7/site-packeages/nova/virt/

5. 配置apache允許訪問placement API

   * ```
     echo '<Directory /usr/bin>
        <IfVersion >= 2.4>
           Require all granted
        </IfVersion>
        <IfVersion < 2.4>
           Order allow,deny
           Allow from all
        </IfVersion>
     </Directory>' >> /etc/httpd/conf.d/00-placement-api.conf
     ```

6. 更新nova-api資料庫

```
su -s /bin/sh -c "nova-manage api_db sync" nova
```

7. 註冊cell0資料庫

```
su -s /bin/sh -c "nova-manage cell_v2 map_cell0" nova
#創建cell1
su -s /bin/sh -c "nova-manage cell_v2 create_cell --name=cell1 --verbose" nova
```

8. 更新nova資料庫

```
su -s /bin/sh -c "nova-manage db sync" nova
```

9. 確認nova cell0及cell1正確註冊

```
su -s /bin/sh -c "nova-manage cell_v2 list_cells" nova
```

10. 完成安裝

```
systemctl enable openstack-nova-api.service openstack-nova-scheduler.service openstack-nova-conductor.service openstack-nova-novncproxy.service

ssystemctl start openstack-nova-api.service openstack-nova-scheduler.service openstack-nova-conductor.service openstack-nova-novncproxy.service
```

```
echo '#!/bin/bash
systemctl restart openstack-nova-api.service   openstack-nova-consoleauth.service openstack-nova-scheduler.service   openstack-nova-conductor.service openstack-nova-novncproxy.service
' >> ~/nova-restart.sh
```



11. 檢查

```
nova service-list
+--------------------------------------+----------------+------------+----------+---------+-------+----------------------------+-----------------+-------------+
| Id                                   | Binary         | Host       | Zone     | Status  | State | Updated_at                 | Disabled Reason | Forced down |
+----------------------+----------------+------------+----------+---------+-------+----------------------------+-----------------+-------------+
| c7ffffde-7c06-4b11-8 | nova-scheduler | controller | internal | enabled | up    | 2020-03-07T14:49:59.000000 | -               | False       |
| 1b436a5f-27a4-455f-a | nova-conductor | controller | internal | enabled | up    | 2020-03-07T14:50:03.000000 | -               | False       |
+----------------------+----------------+------------+----------+---------+-------+----------------------------+-----------------+-------------+

```

檢查novnvproxy服務

```
netstat -lntup | grep 6080
tcp        0      0 0.0.0.0:6080       0.0.0.0:*      LISTEN      28299/python2

ps -ef | grep 28299
 nova     28299     1  0 22:31 ?        00:00:03 /usr/bin/python2 /usr/bin/nova-novncproxy --web /usr/share/novnc/
```

```
# nova-status upgrade check
+--------------------------------------------------------------------+
| Upgrade Check Results                                              |
+--------------------------------------------------------------------+
| Check: Cells v2                                                    |
| Result: Success                                                    |
| Details: None                                                      |
+--------------------------------------------------------------------+
| Check: Placement API                                               |
| Result: Success                                                    |
| Details: None                                                      |
+--------------------------------------------------------------------+
| Check: Ironic Flavor Migration                                     |
| Result: Success                                                    |
| Details: None                                                      |
+--------------------------------------------------------------------+
| Check: Request Spec Migration                                      |
| Result: Success                                                    |
| Details: None                                                      |
+--------------------------------------------------------------------+
| Check: Console Auths                                               |
| Result: Success                                                    |
| Details: None                                                      |
+--------------------------------------------------------------------+
```

##### nova 計算節點

nova-compute調用libvirtd來創建虛擬機

1. 安裝

```
yum install openstack-nova-compute -y
yum install openstack-utils -y
```

2. 配置/etc/nova/nova.conf

```
cp /etc/nova/nova.conf{,.bak}
grep -Ev "^$|#" /etc/nova/nova.conf.bak > /etc/nova/nova.conf

openstack-config --set /etc/nova/nova.conf DEFAULT enabled_apis osapi_compute,metadata
openstack-config --set /etc/nova/nova.conf DEFAULT my_ip 192.168.56.31
openstack-config --set /etc/nova/nova.conf DEFAULT use_neutron true
openstack-config --set /etc/nova/nova.conf DEFAULT firewall_driver nova.virt.firewall.NoopFirewallDriver
openstack-config --set /etc/nova/nova.conf DEFAULT transport_url rabbit://openstack:RABBIT_PASS@controller
openstack-config --set /etc/nova/nova.conf api auth_strategy keystone

openstack-config --set /etc/nova/nova.conf keystone_authtoken auth_url http://controller:5000/v3
openstack-config --set /etc/nova/nova.conf keystone_authtoken memcached_servers controller:11211
openstack-config --set /etc/nova/nova.conf keystone_authtoken auth_type password
openstack-config --set /etc/nova/nova.conf keystone_authtoken project_domain_name Default
openstack-config --set /etc/nova/nova.conf keystone_authtokenuser_domain_name Default
openstack-config --set /etc/nova/nova.conf keystone_authtoken project_name service
openstack-config --set /etc/nova/nova.conf keystone_authtoken username nova
openstack-config --set /etc/nova/nova.conf keystone_authtoken password NOVA_PASS

openstack-config --set /etc/nova/nova.conf vnc enabled true
openstack-config --set /etc/nova/nova.conf vnc server_listen 0.0.0.0
openstack-config --set /etc/nova/nova.conf vnc server_proxyclient_address '$my_ip'
openstack-config --set /etc/nova/nova.conf vnc novncproxy_base_url http://controller:6080/vnc_auto.html

openstack-config --set /etc/nova/nova.conf glance api_servers http://controller:9292

openstack-config --set /etc/nova/nova.conf oslo_concurrency lock_path /var/lib/nova/tmp

openstack-config --set /etc/nova/nova.conf placement region_name RegionOne
openstack-config --set /etc/nova/nova.conf placement project_domain_name Default
openstack-config --set /etc/nova/nova.conf placement project_name service
openstack-config --set /etc/nova/nova.conf placement auth_type password
openstack-config --set /etc/nova/nova.conf placement user_domain_name Default
openstack-config --set /etc/nova/nova.conf placement auth_url http://controller:5000/v3
openstack-config --set /etc/nova/nova.conf placement username placement
openstack-config --set /etc/nova/nova.conf placement password PLACEMENT_PASS

openstack-config --set /etc/nova/nova.conf neutron url http://controller:9696
openstack-config --set /etc/nova/nova.conf neutron auth_url http://controller:5000
openstack-config --set /etc/nova/nova.conf neutron auth_type password
openstack-config --set /etc/nova/nova.conf neutron project_domain_name default
openstack-config --set /etc/nova/nova.conf neutron user_domain_name default
openstack-config --set /etc/nova/nova.conf neutron region_name RegionOne
openstack-config --set /etc/nova/nova.conf neutron project_name service
openstack-config --set /etc/nova/nova.conf neutron username neutron
openstack-config --set /etc/nova/nova.conf neutron password NEUTRON_PASS
```

3. 啟動

```
systemctl enable libvirtd.service openstack-nova-compute.service
systemctl start libvirtd.service openstack-nova-compute.service
```

#### 控制端配置主動發現

添加計算節點到cell資料庫

```
openstack compute service list --service nova-compute
```

主動發現計算節點

```
su -s /bin/sh -c "nova-manage cell_v2 discover_hosts --verbose" nova

Found 2 cell mappings.
Skipping cell0 since it does not contain hosts.
Getting computes from cell 'cell1': 34f5afab-3171-49a9-ac03-90f4b3d7dd16
Checking host mapping for compute host 'compute1': edc32fe1-4cc1-4ab3-93a9-99eef9161073
Creating host mapping for compute host 'compute1': edc32fe1-4cc1-4ab3-93a9-99eef9161073
Found 1 unmapped computes in cell: 34f5afab-3171-49a9-ac03-90f4b3d7dd16
```

設定定期發現:

```
vi /etc/nova/nova.conf

[scheduler]
discover_hosts_in_cells_interval=300
```

重啟nova服務

```
bash nova-restart.sh
```



#### netron網路服務安裝

> ##### neutron-server 
>
> 端口9696 接受響應外部的網路管理請求
>
> ##### neutron-linuxbridge-agent
>
> 負責創建橋接網卡
>
> ##### neutron-dhcp-agent
>
> 負責分配IP
>
> ##### neutron-matadata-agent
>
> 配合nova-metadata-api實現虛擬機的定製化操作
>
> ##### L3-agent
>
> 實現三層網路vxlan(網路層) 例如LBaaS load balance 即服務 



#### 安裝步驟(controller)

1. 創建資料庫並授權

   * ```
     mysql -u root -p
     CREATE DATABASE neutron;
     
     GRANT ALL PRIVILEGES ON neutron.* TO 'neutron'@'%' IDENTIFIED BY 'NEUTRON_DBPASS';
     ```

2. 在keystone中註冊服務驗證

   * ```
     openstack user create --domain default --password NEUTRON_PASS neutron
     
     openstack role add --project service --user neutron admin
     
     openstack service create --name neutron --description "OpenStack Networking" network
     ```

   * 建立網路服務API

   * ```
     openstack endpoint create --region RegionOne network public http://controller:9696
       
     openstack endpoint create --region RegionOne network internal http://controller:9696
       
     openstack endpoint create --region RegionOne network admin http://controller:9696
     ```

3. 安裝組件

   * ```
     yum install openstack-neutron openstack-neutron-ml2 openstack-neutron-linuxbridge ebtables -y
     ```

   * :star: ebtable 類似iptable

4. 設定公用網路

   * 編輯/etc/neutron/neutron.conf

   * ```
     cp /etc/neutron/neutron.conf{,.bak}
     grep -Ev "^$|#" /etc/neutron/neutron.conf.bak > /etc/neutron/neutron.conf
     
     openstack-config --set /etc/neutron/neutron.conf database connection mysql+pymysql://neutron:NEUTRON_DBPASS@controller/neutron
     
     openstack-config --set /etc/neutron/neutron.conf DEFAULT core_plugin ml2
     openstack-config --set /etc/neutron/neutron.conf DEFAULT service_plugins  
     openstack-config --set /etc/neutron/neutron.conf DEFAULT transport_url rabbit://openstack:RABBIT_PASS@controller
     openstack-config --set /etc/neutron/neutron.conf DEFAULT auth_strategy keystone
     openstack-config --set /etc/neutron/neutron.conf DEFAULT notify_nova_on_port_status_changes true
     openstack-config --set /etc/neutron/neutron.conf DEFAULT notify_nova_on_port_data_changes true
     
     openstack-config --set /etc/neutron/neutron.conf keystone_authtoken www_authenticate_uri http://controller:5000
     openstack-config --set /etc/neutron/neutron.conf keystone_authtoken auth_url http://controller:5000
     openstack-config --set /etc/neutron/neutron.conf keystone_authtoken memcached_servers controller:11211
     openstack-config --set /etc/neutron/neutron.conf keystone_authtoken auth_type password
     openstack-config --set /etc/neutron/neutron.conf keystone_authtoken project_domain_name default
     openstack-config --set /etc/neutron/neutron.conf keystone_authtoken user_domain_name default
     openstack-config --set /etc/neutron/neutron.conf keystone_authtoken project_name service
     openstack-config --set /etc/neutron/neutron.conf keystone_authtoken username neutron
     openstack-config --set /etc/neutron/neutron.conf keystone_authtoken password NEUTRON_PASS
     
     
     openstack-config --set /etc/neutron/neutron.conf nova auth_url http://controller:5000
     openstack-config --set /etc/neutron/neutron.conf nova auth_type password
     openstack-config --set /etc/neutron/neutron.conf nova project_domain_name default
     openstack-config --set /etc/neutron/neutron.conf nova user_domain_name default
     openstack-config --set /etc/neutron/neutron.conf nova region_name RegionOne
     openstack-config --set /etc/neutron/neutron.conf nova project_name service
     openstack-config --set /etc/neutron/neutron.conf nova username nova
     openstack-config --set /etc/neutron/neutron.conf nova password NOVA_PASS
     
     openstack-config --set /etc/neutron/neutron.conf oslo_concurrency lock_path /var/lib/neutron/tmp
     ```

   * 編輯/etc/neutron/plugins/ml2/ml2_conf.ini

   * ```
     cp /etc/neutron/plugins/ml2/ml2_conf.ini{,.bak}
     grep -Ev "^$|#" /etc/neutron/plugins/ml2/ml2_conf.ini.bak > /etc/neutron/plugins/ml2/ml2_conf.ini
     
     openstack-config --set /etc/neutron/plugins/ml2/ml2_conf.ini ml2 type_drivers flat,vlan
     openstack-config --set /etc/neutron/plugins/ml2/ml2_conf.ini ml2 tenant_network_types  
     openstack-config --set /etc/neutron/plugins/ml2/ml2_conf.ini ml2 mechanism_drivers linuxbridge
     openstack-config --set /etc/neutron/plugins/ml2/ml2_conf.ini ml2 extension_drivers port_security
     
     
     openstack-config --set /etc/neutron/plugins/ml2/ml2_conf.ini ml2_type_flat flat_networks provider
     
     openstack-config --set /etc/neutron/plugins/ml2/ml2_conf.ini securitygroup enable_ipset true
     ```

   * 配置linuxbridge代理 /etc/neutron/plugins/ml2/linuxbridge_agent.ini

   * ```
     cp /etc/neutron/plugins/ml2/linuxbridge_agent.ini{,.bak}
     grep -Ev "^$|#" /etc/neutron/plugins/ml2/linuxbridge_agent.ini.bak > /etc/neutron/plugins/ml2/linuxbridge_agent.ini
     
     openstack-config --set /etc/neutron/plugins/ml2/linuxbridge_agent.ini linux_bridge physical_interface_mappings provider:enp0s3
     
     openstack-config --set /etc/neutron/plugins/ml2/linuxbridge_agent.ini vxlan enable_vxlan false
     
     openstack-config --set /etc/neutron/plugins/ml2/linuxbridge_agent.ini securitygroup enable_security_group true
     openstack-config --set /etc/neutron/plugins/ml2/linuxbridge_agent.ini securitygroup firewall_driver neutron.agent.linux.iptables_firewall.IptablesFirewallDriver
     ```

   * 新增/etc/sysctl

   * ```
     echo "net.bridge.bridge-nf-call-iptables = 1" >> /etc/sysctl.conf
     echo "net.bridge.bridge-nf-call-ip6tables = 1" >> /etc/sysctl.conf
     
     sysctl -p
     ```

   * 配置DHCP代理/etc/neutron/dhcp_agent.ini

   * ```
     cp /etc/neutron/dhcp_agent.ini{,.bak}
     grep -Ev "^$|#" /etc/neutron/dhcp_agent.ini.bak > /etc/neutron/dhcp_agent.ini
     
     
     openstack-config --set /etc/neutron/dhcp_agent.ini DEFAULT interface_driver linuxbridge
     
     openstack-config --set /etc/neutron/dhcp_agent.ini DEFAULT dhcp_driver neutron.agent.linux.dhcp.Dnsmasq
     
     openstack-config --set /etc/neutron/dhcp_agent.ini DEFAULT enable_isolated_metadata true
     ```

   * 配置metadata代理

   * ```
     cp /etc/neutron/metadata_agent.ini{,.bak}
     grep -Ev "^$|#" /etc/neutron/metadata_agent.ini > /etc/neutron/metadata_agent.ini
     
     
     openstack-config --set /etc/neutron/metadata_agent.ini DEFAULT nova_metadata_host controller
     openstack-config --set /etc/neutron/metadata_agent.ini DEFAULT metadata_proxy_shared_secret METADATA_SECRET
     ```

   * 配置計算服務中的網路服務

   * ```
     openstack-config --set /etc/nova/nova.conf neutron url http://controller:9696
     openstack-config --set /etc/nova/nova.conf neutron auth_url http://controller:5000
     openstack-config --set /etc/nova/nova.conf neutron auth_type password
     openstack-config --set /etc/nova/nova.conf neutron project_domain_name default
     openstack-config --set /etc/nova/nova.conf neutron user_domain_name default
     openstack-config --set /etc/nova/nova.conf neutron region_name RegionOne
     openstack-config --set /etc/nova/nova.conf neutron project_name service
     openstack-config --set /etc/nova/nova.conf neutron username neutron
     openstack-config --set /etc/nova/nova.conf neutron password NEUTRON_PASS
     openstack-config --set /etc/nova/nova.conf neutron service_metadata_proxy true
     openstack-config --set /etc/nova/nova.conf neutron metadata_proxy_shared_secret METADATA_SECRET
     ```

5. 完成安裝

   * ```
     ln -s /etc/neutron/plugins/ml2/ml2_conf.ini /etc/neutron/plugin.ini
     
     su -s /bin/sh -c "neutron-db-manage --config-file /etc/neutron/neutron.conf \
       --config-file /etc/neutron/plugins/ml2/ml2_conf.ini upgrade head" neutron
       
     systemctl restart openstack-nova-api.service
     
     systemctl enable neutron-server.service \
       neutron-linuxbridge-agent.service neutron-dhcp-agent.service \
       neutron-metadata-agent.service
     systemctl start neutron-server.service \
       neutron-linuxbridge-agent.service neutron-dhcp-agent.service \
       neutron-metadata-agent.service
     systemctl restart neutron-server.service \
       neutron-linuxbridge-agent.service neutron-dhcp-agent.service \
       neutron-metadata-agent.service
       
     systemctl enable neutron-l3-agent.service
     systemctl start neutron-l3-agent.service
     systemctl restart neutron-l3-agent.service
     ```

#### 配置neutron計算節點

安裝元件

```
yum install openstack-neutron-linuxbridge ebtables ipset -y
```

配置/etc/neutron/neutron.conf

```
cp /etc/neutron/neutron.conf{,.bak}
grep -Ev "^$|#" /etc/neutron/neutron.conf.bak > /etc/neutron/neutron.conf


openstack-config --set /etc/neutron/neutron.conf DEFAULT transport_url rabbit://openstack:RABBIT_PASS@controller
openstack-config --set /etc/neutron/neutron.conf DEFAULT auth_strategy keystone

openstack-config --set /etc/neutron/neutron.conf keystone_authtoken www_authenticate_uri http://controller:5000
openstack-config --set /etc/neutron/neutron.conf keystone_authtoken auth_url http://controller:5000
openstack-config --set /etc/neutron/neutron.conf keystone_authtoken memcached_servers controller:11211
openstack-config --set /etc/neutron/neutron.conf keystone_authtoken auth_type password
openstack-config --set /etc/neutron/neutron.conf keystone_authtoken project_domain_name default
openstack-config --set /etc/neutron/neutron.conf keystone_authtoken user_domain_name default
openstack-config --set /etc/neutron/neutron.conf keystone_authtoken project_name service
openstack-config --set /etc/neutron/neutron.conf keystone_authtoken username neutron
openstack-config --set /etc/neutron/neutron.conf keystone_authtoken password NEUTRON_PASS

openstack-config --set /etc/neutron/neutron.conf oslo_concurrency lock_path /var/lib/neutron/tmp
```

配置公用網路

```
cp /etc/neutron/plugins/ml2/linuxbridge_agent.ini{,.bak}
grep -Ev "^$|#" /etc/neutron/plugins/ml2/linuxbridge_agent.ini.bak > /etc/neutron/plugins/ml2/linuxbridge_agent.ini

openstack-config --set /etc/neutron/plugins/ml2/linuxbridge_agent.ini linux_bridge physical_interface_mappings provider:enp0s3

openstack-config --set /etc/neutron/plugins/ml2/linuxbridge_agent.ini vxlan enable_vxlan false

openstack-config --set /etc/neutron/plugins/ml2/linuxbridge_agent.ini securitygroup enable_security_group true

openstack-config --set /etc/neutron/plugins/ml2/linuxbridge_agent.ini securitygroup firewall_driver neutron.agent.linux.iptables_firewall.IptablesFirewallDriver
###

```

* 設置`/etc/sysctl.conf`文件

```bash
echo "net.bridge.bridge-nf-call-iptables = 1" >> /etc/sysctl.conf
echo "net.bridge.bridge-nf-call-ip6tables = 1" >> /etc/sysctl.conf

sysctl -p
```



完成安裝

```
systemctl enable openstack-nova-compute.service
systemctl start openstack-nova-compute.service
systemctl restart openstack-nova-compute.service

systemctl enable neutron-linuxbridge-agent.service
systemctl start neutron-linuxbridge-agent.service
systemctl restart neutron-linuxbridge-agent.service
```

> linuxbridge 出現時間早 特別成熟 功能較少 穩定, 配置簡單
>
> openvswitch 出現時間晚 功能比較多 穩定性不如linuxbridge, 配置複雜

#### 控制節點驗證

```
openstack extension list --network
```

```
openstack network agent list
```

![image-20200310174341414](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200310174341414.png)

### Horizon Dashboard安裝配置

1. 安裝軟體包

   * ```
     yum install openstack-dashboard -y
     ```

2. 配置/etc/openstack-dashboard/local_settings

```
OPENSTACK_HOST = "controller"
ALLOWED_HOSTS = ['*']
SESSION_ENGINE = 'django.contrib.sessions.backends.cache'

CACHES = {
    'default': {
         'BACKEND': 'django.core.cache.backends.memcached.MemcachedCache',
         'LOCATION': 'controller:11211',
    }
}

OPENSTACK_KEYSTONE_URL = "http://%s:5000/v3" % OPENSTACK_HOST

OPENSTACK_KEYSTONE_MULTIDOMAIN_SUPPORT = True

OPENSTACK_API_VERSIONS = {
    "identity": 3,
    "image": 2,
    "volume": 2,
}

OPENSTACK_KEYSTONE_DEFAULT_DOMAIN = "Default"

OPENSTACK_KEYSTONE_DEFAULT_ROLE = "user"

OPENSTACK_NEUTRON_NETWORK = {
    ...
    'enable_router': False,
    'enable_quotas': False,
    'enable_distributed_router': False,
    'enable_ha_router': False,
    'enable_lb': False,
    'enable_firewall': False,
    'enable_vpn': False,
    'enable_fip_topology_check': False,
}

TIME_ZONE = "Asia_Taipei"
```

編輯/etc/httpd/conf.d/openstack-dashboard.conf 新增以下內容

```
WSGIApplicationGroup %{GLOBAL}
```



### 新增一個實例

1. 創建虛擬網路(公用網路)

   * 建立網路

   * ```
     openstack network create  --share --external --provider-physical-network provider --provider-network-type flat provider
     ```

   * 建立子網路

   * ```
     openstack subnet create --network provider --allocation-pool start=10.0.2.131,end=10.0.2.150 --dns-nameserver 10.0.2.1 --gateway 10.0.2.1 --subnet-range 10.0.2.0/24 provider-sub
     ```

2. 創建m1.nano模組

   * ```
     openstack flavor create --id 0 --vcpus 1 --ram 64 --disk 1 m1.nano
     openstack flavor create --id 1 --vcpus 1 --ram 512 --disk 1 m1.tiny
     openstack flavor create --id 2 --vcpus 1 --ram 2048 --disk 1 m1.small
     ```

3. 創建密鑰對

   * 生成密鑰對

   * ```
     ssh-keygen -q -N "" -f ~/.ssh/id_rsa
     ```

   * 上傳密鑰對

   * ```
     openstack keypair create --public-key ~/.ssh/id_rsa.pub mykey
     ```

   * 確認

   * ```
     openstack keypair list
     ```

4. 新增安全組規則

   * ```
     openstack security group rule create --proto icmp default
     openstack security group rule create --proto tcp --dst-port 22 default
     ```

5. 啟動實例

   * 先查出network-id

   * ```
     openstack network list
     +--------------------------------------+----------+--------------------------------------+
     | ID                                   | Name     | Subnets                              |
     +--------------------------------------+----------+--------------------------------------+
     | 34d13eb9-371c-4d00-8f60-b81342738437 | provider | e3b68511-51af-4359-9215-c3448cffe71a |
     +--------------------------------------+----------+--------------------------------------+
     ```

   * 創建主機

   * ```
     openstack server create --flavor m1.nano --image cirros --nic net-id=5512efdf-df5a-41d0-8ed3-48cf320d2d7d --security-group default --key-name mykey newinstance
     ```

### 新增計算節點

```
yum install centos-release-openstack-stein -y
yum install python2-openstackclient.noarch openstack-selinux -y
yum install openstack-utils.noarch -y
yum install openstack-nova-compute -y
```



```
cp /etc/nova/nova.conf{,.bak}
grep -Ev "^$|#" /etc/nova/nova.conf.bak > /etc/nova/nova.conf

openstack-config --set /etc/nova/nova.conf DEFAULT enabled_apis osapi_compute,metadata
openstack-config --set /etc/nova/nova.conf DEFAULT my_ip 10.0.2.32
openstack-config --set /etc/nova/nova.conf DEFAULT use_neutron true
openstack-config --set /etc/nova/nova.conf DEFAULT firewall_driver nova.virt.firewall.NoopFirewallDriver
openstack-config --set /etc/nova/nova.conf DEFAULT transport_url rabbit://openstack:RABBIT_PASS@controller
openstack-config --set /etc/nova/nova.conf api auth_strategy keystone

openstack-config --set /etc/nova/nova.conf keystone_authtoken auth_url http://controller:5000/v3
openstack-config --set /etc/nova/nova.conf keystone_authtoken memcached_servers controller:11211
openstack-config --set /etc/nova/nova.conf keystone_authtoken auth_type password
openstack-config --set /etc/nova/nova.conf keystone_authtoken project_domain_name Default
openstack-config --set /etc/nova/nova.conf keystone_authtokenuser_domain_name Default
openstack-config --set /etc/nova/nova.conf keystone_authtoken project_name service
openstack-config --set /etc/nova/nova.conf keystone_authtoken username nova
openstack-config --set /etc/nova/nova.conf keystone_authtoken password NOVA_PASS

openstack-config --set /etc/nova/nova.conf vnc enabled true
openstack-config --set /etc/nova/nova.conf vnc server_listen 0.0.0.0
openstack-config --set /etc/nova/nova.conf vnc server_proxyclient_address '$my_ip'
openstack-config --set /etc/nova/nova.conf vnc novncproxy_base_url http://controller:6080/vnc_auto.html

openstack-config --set /etc/nova/nova.conf glance api_servers http://controller:9292

openstack-config --set /etc/nova/nova.conf oslo_concurrency lock_path /var/lib/nova/tmp

openstack-config --set /etc/nova/nova.conf placement region_name RegionOne
openstack-config --set /etc/nova/nova.conf placement project_domain_name Default
openstack-config --set /etc/nova/nova.conf placement project_name service
openstack-config --set /etc/nova/nova.conf placement auth_type password
openstack-config --set /etc/nova/nova.conf placement user_domain_name Default
openstack-config --set /etc/nova/nova.conf placement auth_url http://controller:5000/v3
openstack-config --set /etc/nova/nova.conf placement username placement
openstack-config --set /etc/nova/nova.conf placement password PLACEMENT_PASS

openstack-config --set /etc/nova/nova.conf neutron url http://controller:9696
openstack-config --set /etc/nova/nova.conf neutron auth_url http://controller:5000
openstack-config --set /etc/nova/nova.conf neutron auth_type password
openstack-config --set /etc/nova/nova.conf neutron project_domain_name default
openstack-config --set /etc/nova/nova.conf neutron user_domain_name default
openstack-config --set /etc/nova/nova.conf neutron region_name RegionOne
openstack-config --set /etc/nova/nova.conf neutron project_name service
openstack-config --set /etc/nova/nova.conf neutron username neutron
openstack-config --set /etc/nova/nova.conf neutron password NEUTRON_PASS
```

設定neutron

```
yum install openstack-neutron-linuxbridge ebtables ipset -y

cp /etc/neutron/neutron.conf{,.bak}
grep -Ev "^$|#" /etc/neutron/neutron.conf.bak > /etc/neutron/neutron.conf


openstack-config --set /etc/neutron/neutron.conf DEFAULT transport_url rabbit://openstack:RABBIT_PASS@controller
openstack-config --set /etc/neutron/neutron.conf DEFAULT auth_strategy keystone

openstack-config --set /etc/neutron/neutron.conf keystone_authtoken www_authenticate_uri http://controller:5000
openstack-config --set /etc/neutron/neutron.conf keystone_authtoken auth_url http://controller:5000
openstack-config --set /etc/neutron/neutron.conf keystone_authtoken memcached_servers controller:11211
openstack-config --set /etc/neutron/neutron.conf keystone_authtoken auth_type password
openstack-config --set /etc/neutron/neutron.conf keystone_authtoken project_domain_name default
openstack-config --set /etc/neutron/neutron.conf keystone_authtoken user_domain_name default
openstack-config --set /etc/neutron/neutron.conf keystone_authtoken project_name service
openstack-config --set /etc/neutron/neutron.conf keystone_authtoken username neutron
openstack-config --set /etc/neutron/neutron.conf keystone_authtoken password NEUTRON_PASS

openstack-config --set /etc/neutron/neutron.conf oslo_concurrency lock_path /var/lib/neutron/tmp

###
cp /etc/neutron/plugins/ml2/linuxbridge_agent.ini{,.bak}
grep -Ev "^$|#" /etc/neutron/plugins/ml2/linuxbridge_agent.ini.bak > /etc/neutron/plugins/ml2/linuxbridge_agent.ini

openstack-config --set /etc/neutron/plugins/ml2/linuxbridge_agent.ini linux_bridge physical_interface_mappings provider:enp0s3

openstack-config --set /etc/neutron/plugins/ml2/linuxbridge_agent.ini vxlan enable_vxlan false

openstack-config --set /etc/neutron/plugins/ml2/linuxbridge_agent.ini securitygroup enable_security_group true

openstack-config --set /etc/neutron/plugins/ml2/linuxbridge_agent.ini securitygroup firewall_driver neutron.agent.linux.iptables_firewall.IptablesFirewallDriver
###
```



```
echo "net.bridge.bridge-nf-call-iptables = 1" >> /etc/sysctl.conf
echo "net.bridge.bridge-nf-call-ip6tables = 1" >> /etc/sysctl.conf

sysctl -p
```



```
systemctl enable openstack-nova-compute.service neutron-linuxbridge-agent.service libvirtd.service openstack-nova-compute.service

systemctl start openstack-nova-compute.service neutron-linuxbridge-agent.service libvirtd.service openstack-nova-compute.service

systemctl restart openstack-nova-compute.service neutron-linuxbridge-agent.service libvirtd.service openstack-nova-compute.service
```



#### 驗證 

控制節點上

```
nova service-list

neutron agent-list
```



### 驗證是否可用

切換不同計算節點驗證節點可用

### openstack 項目 用戶 角色的關係

```
openstack domain create --description "An Example Domain" example

openstack project create --domain default --description "Service Project" service

openstack project create --domain default --description "Demo Project" myproject

openstack user create --domain default --password MYUSER_PASS myuser

```



一個域中可以包含多個項目、用戶及角色

admin項目

service項目



admin用戶

oldboy用戶



admin角色

user角色



項目及用戶透過角色來授權，一個項目可以透過角色來賦予不同用戶權限

![image-20200310214113179](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200310214113179.png)

創用戶方法

1. cli
2. GUI



openstack權限設計不合理 需要進行更多二次開發

### glance鏡像服務遷移

##### 在控制節點上

1. 停止glance服務

```
systemctl stop openstack-glance-api.service openstack-glance-registry.service
systemctl disable openstack-glance-api.service openstack-glance-registry.service
```

2. 在另一個節點上安裝mariadb(lab用compute2)

   * ```
     yum install mariadb mariadb-server python2-PyMySQL -y
     
     systemctl start mariadb
     systemctl enable mariadb
     
     mysql_secure_installation
     ```

3. 恢復glance資料庫的數據

   * ```
     mysqldump -B glance > glance.sql
     ```

   * 推至compute2

   * ```
     scp glance.sql 10.0.2.32:/root
     ```

   * 

   * ```
     mysql < glance.sql
     
     mysql glance -e 'show tables;'
     
     #授權
     mysql
     CREATE DATABASE glance;
     GRANT ALL PRIVILEGES ON glance.* TO 'glance'@'localhost' IDENTIFIED BY 'GLANCE_DBPASS';
     GRANT ALL PRIVILEGES ON glance.* TO 'glance'@'%' IDENTIFIED BY 'GLANCE_DBPASS';
     ```

##### 在計算節點上

1. 還原資料庫(在compute2節點)

   * ```
     mysql < glance.sql
     
     mysql glance -e 'show tables;'
     
     #授權
     mysql
     CREATE DATABASE glance;
     GRANT ALL PRIVILEGES ON glance.* TO 'glance'@'localhost' IDENTIFIED BY 'GLANCE_DBPASS';
     GRANT ALL PRIVILEGES ON glance.* TO 'glance'@'%' IDENTIFIED BY 'GLANCE_DBPASS';
     ```

2. 安裝glance服務

* ```
  yum install openstack-glance -y
  ```

* 拉設定檔

* ```
  scp -rp 10.0.2.11:/etc/glance/glance-api.conf /etc/glance/glance-api.conf
   
  scp -rp 10.0.2.11:/etc/glance/glance-registry.conf /etc/glance/glance-registry.conf
  
  yum install openstack-utils -y
  
  openstack-config --set /etc/glance/glance-api.conf database connection mysql+pymysql://glance:GLANCE_DBPASS@10.0.2.32/glance
  
  openstack-config --set /etc/glance/glance-registry.conf database connection mysql+pymysql://glance:GLANCE_DBPASS@10.0.2.32/glance
  ```

3. 啟用服務

* ```
  systemctl start openstack-glance-api openstack-glance-registry
  systemctl enable openstack-glance-api openstack-glance-registry
  ```

4. 確認端口啟用

   * ```
     netstat -lntup | grep 9292
     ```

   * 

5. 遷移鏡像

   * ```
     scp -rp 10.0.2.11:/var/lib/glance/images/* /var/lib/glance/images/
     
     chown -R glance:glance /var/lib/glance/images
     ```

6. 控制節點修改連接資訊

   * ```
     mysqldump keystone endpoint -uroot -p> endpoint.sql
     #備份
     cp endpoint.sql /opt/
     
     vim endpoint.sql
     :%s#https://controller:9292#http://10.0.2.32:9292#gc
     
     openstacke image list
     ```

7. 修改所有節點的nova配置文件

   * ```
     sed -i 's#http://controller:9292#http://10.0.2.32:9292#g' /etc/nova/nova.conf
     ```

   * 重啟服務

   * 控制節點

     ```
     systemctl restart openstack-nova-api
     ```

   * 計算節點

     ```
     systemctl restart openstack-nova-compute
     ```

8. 驗證以上所有操作的方法

   * 上傳一個新鏡像並且啟動新實例

### cinder塊存儲服務

kvm 熱添加硬碟

cinder-api:            接收外部的API請求

cinder-volume:    提供存儲空間

cinder-schedule: 調度器 決定將要分配的空間由哪一個cinder-volume提供

cinder-backup:     備份存儲

#### cinder計算節點安裝步驟

1. 創庫授權

   * ```
     mysql -u root -p
     CREATE DATABASE cinder;
     GRANT ALL PRIVILEGES ON cinder.* TO 'cinder'@'localhost' IDENTIFIED BY 'CINDER_DBPASS';
     GRANT ALL PRIVILEGES ON cinder.* TO 'cinder'@'%' IDENTIFIED BY 'CINDER_DBPASS';
     ```

2. 在keystone創建cinder用戶 註冊cinder服務

   * ```
     openstack user create --domain default --password CINDER_PASS cinder
     
     openstack role add --project service --user cinder admin
     
     openstack service create --name cinderv2 --description "OpenStack Block Storage" volumev2
     
     openstack service create --name cinderv3 --description "OpenStack Block Storage" volumev3
     
     openstack endpoint create --region RegionOne \
       volumev2 public http://controller:8776/v2/%\(project_id\)s
       
     openstack endpoint create --region RegionOne \
       volumev2 internal http://controller:8776/v2/%\(project_id\)s
       
     openstack endpoint create --region RegionOne \
       volumev2 admin http://controller:8776/v2/%\(project_id\)s
       
     openstack endpoint create --region RegionOne \
       volumev3 public http://controller:8776/v3/%\(project_id\)s
       
     openstack endpoint create --region RegionOne \
       volumev3 internal http://controller:8776/v3/%\(project_id\)s
       
     openstack endpoint create --region RegionOne \
       volumev3 admin http://controller:8776/v3/%\(project_id\)s
     ```

3. 安裝cinder軟體包

   * ```
     yum install openstack-cinder -y
     ```

4. 設定配置檔/etc/cinder/cinder.conf

   * ```
     cp /etc/cinder/cinder.conf{,.bak}
     grep -Ev "^$|#" /etc/cinder/cinder.conf.bak > /etc/cinder/cinder.conf
     
     openstack-config --set /etc/cinder/cinder.conf database connection mysql+pymysql://cinder:CINDER_DBPASS@controller/cinder
     
     openstack-config --set /etc/cinder/cinder.conf DEFAULT auth_strategy keystone
     openstack-config --set /etc/cinder/cinder.conf DEFAULT transport_url rabbit://openstack:RABBIT_PASS@controller
     openstack-config --set /etc/cinder/cinder.conf DEFAULT my_ip 10.0.2.11
     
     openstack-config --set /etc/cinder/cinder.conf keystone_authtoken www_authenticate_uri http://controller:5000
     openstack-config --set /etc/cinder/cinder.conf keystone_authtoken auth_url http://controller:5000
     openstack-config --set /etc/cinder/cinder.conf keystone_authtoken memcached_servers controller:11211
     openstack-config --set /etc/cinder/cinder.conf keystone_authtoken auth_type password
     openstack-config --set /etc/cinder/cinder.conf keystone_authtoken project_domain_name default
     openstack-config --set /etc/cinder/cinder.conf keystone_authtoken user_domain_name default
     openstack-config --set /etc/cinder/cinder.conf keystone_authtoken project_name service
     openstack-config --set /etc/cinder/cinder.conf keystone_authtoken username cinder
     openstack-config --set /etc/cinder/cinder.conf keystone_authtoken password CINDER_PASS
     
     openstack-config --set /etc/cinder/cinder.conf oslo_concurrency lock_path /var/lib/cinder/tmp
     
     openstack-config --set /etc/nova/nova.conf cinder os_region_name RegionOne
     ```

   * 同步資料庫

     ```
     su -s /bin/sh -c "cinder-manage db sync" cinder
     ```

5. 完成安裝

   ```
   systemctl restart openstack-nova-api.service
   systemctl enable openstack-cinder-api.service openstack-cinder-scheduler.service
   systemctl start openstack-cinder-api.service openstack-cinder-scheduler.service
   
   systemctl restart openstack-cinder-api.service openstack-cinder-scheduler.service
   ```

#### cinder觀念說明

![image-20200311115041124](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200311115041124.png)

![image-20200311115453129](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200311115453129.png)

#### 存儲節點安裝

1. 安裝軟體包

   ```
   yum install lvm2 device-mapper-persistent-data -y
   ```

2. 啟用服務

   ```
   systemctl enable lvm2-lvmetad.service
   systemctl start lvm2-lvmetad.service
   ```

3. 添加虛擬硬碟30G及10G

4. 熱添加時` echo '- - -' > /sys/class/scsi_host/host0/scan` 使用指令掃描，`fdisk -l`檢查

5. 加入虛擬硬碟

6. ```
   pvcreate /dev/sdb
   pvcreate /dev/sdc
   vgcreate cinder-ssd /dev/sdb
   vgcreate cinder-sata /dev/sdc
   ```

7. 修改過濾器/etc/lvm/lvm.cond 130行

   ```
   filter = [ "a/sdb/","a/sdc/","r/.*/" ]
   ```

8. 安裝cinder軟體包

   ```
   yum install openstack-cinder targetcli python-keystone -y
   ```

targetcli為isci服務用

![image-20200311135059589](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200311135059589.png)

9. 配置文件/etc/cinder/cinder.conf

10. ```
    openstack-config --set /etc/cinder/cinder.conf connection mysql+pymysql://cinder:CINDER_DBPASS@controller/cinder
    
    openstack-config --set /etc/cinder/cinder.conf DEFAULT transport_url rabbit://openstack:RABBIT_PASS@controller
    openstack-config --set /etc/cinder/cinder.conf DEFAULT auth_strategy keystone
    openstack-config --set /etc/cinder/cinder.conf DEFAULT my_ip 10.0.2.31
    openstack-config --set /etc/cinder/cinder.conf DEFAULT enabled_backends ssd,sata
    openstack-config --set /etc/cinder/cinder.conf DEFAULT glance_api_servers http://10.0.2.32:9292
    
    openstack-config --set /etc/cinder/cinder.conf keystone_authtoken www_authenticate_uri http://controller:5000
    openstack-config --set /etc/cinder/cinder.conf keystone_authtoken auth_url http://controller:5000
    openstack-config --set /etc/cinder/cinder.conf keystone_authtoken memcached_servers controller:11211
    openstack-config --set /etc/cinder/cinder.conf keystone_authtoken auth_type password
    openstack-config --set /etc/cinder/cinder.conf keystone_authtoken project_domain_name default
    openstack-config --set /etc/cinder/cinder.conf keystone_authtoken user_domain_name default
    openstack-config --set /etc/cinder/cinder.conf keystone_authtoken project_name service
    openstack-config --set /etc/cinder/cinder.conf keystone_authtoken username cinder
    openstack-config --set /etc/cinder/cinder.conf keystone_authtoken password CINDER_PASS
    
    openstack-config --set /etc/cinder/cinder.conf ssd volume_driver cinder.volume.drivers.lvm.LVMVolumeDriver
    openstack-config --set /etc/cinder/cinder.conf ssd volume_group cinder-ssd
    openstack-config --set /etc/cinder/cinder.conf ssd target_protocol iscsi
    openstack-config --set /etc/cinder/cinder.conf ssd target_helper lioadm
    openstack-config --set /etc/cinder/cinder.conf ssd volume_backend_name ssd
    
    openstack-config --set /etc/cinder/cinder.conf sata volume_driver cinder.volume.drivers.lvm.LVMVolumeDriver
    openstack-config --set /etc/cinder/cinder.conf sata volume_group cinder-sata
    openstack-config --set /etc/cinder/cinder.conf sata target_protocol iscsi
    openstack-config --set /etc/cinder/cinder.conf sata target_helper lioadm
    openstack-config --set /etc/cinder/cinder.conf sata volume_backend_name sata
    
    openstack-config --set /etc/cinder/cinder.conf oslo_concurrency lock_path /var/lib/cinder/tmp
    ```

11. 啟用服務

    ```
    systemctl enable openstack-cinder-volume.service target.service
    systemctl start openstack-cinder-volume.service target.service
    systemctl restart openstack-cinder-volume.service target.service
    ```

12.  驗證

13. ```
    cinder service-list
    
     cinder-volume | compute1@sata | nova | enabled | up
    | cinder-volume| compute1@ssd  | nova | enabled | up   
    ```

14. 至GUI創建卷

#### cinder的安全及操控

1. 創建卷類型
   * 管理員 > 卷 > 卷類型 > 創建卷類型
   * 擴展規格 > 創建 > 動作
     * 鍵 : volume_backend_name
     * 值 : ssd

![image-20200311161728860](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200311161728860.png)

![image-20200311161938715](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200311161938715.png)

驗證資料安全性

```
dd if=/dev/mapper/cinder--ssd-volume--ec325f59--5778--4ac9--8f44--17c31b2d1849 of=/opt/jiage.raw
 
qemu-img info /opt/jiage.raw
mount -o loop /opt/jiage.raw /srv

df -Th
ll /srv
```



磁盤文件

/var/lib/nova/instances/

#### 排錯:

清空現在log

```
find /var/log/ -type f | awk '{print ">"$0}'|bash
```

重新觸發錯誤

搜尋有關error的錯誤

```
find /var/log/ -type f | xargs grep -i error
```

### 增加一個flat網段

如果將網路服務重啟 會出現實體網卡也有IP的狀況 會導致服務中的網路無法正常使用

在伺服器中，將IP刪除及可以解決

```
ip address del 10.0.2.31/24 dev enp0s3
```

#### 步驟

##### 控制節點步驟

1. 編輯控制節點/etc/neutron/plugins/ml2

```
vim /etc/neutron/plugins/ml2/ml2_conf.ini
[ml2_type_flat]
flat_networks = provider,net10_0_3
```

2. 編輯/etc/neutron/plugins/ml2/linuxbridge

```
[linux_bridge]
physical_interface_mappings = provider:enp0s3,net10_0_3:enp0s9
```

3. 重啟服務

```
systemctl restart neutron-server.service neutron-linuxbridge-agent.service
```

##### 計算節點步驟

1. 編輯/etc/neutron/plugins/ml2/linuxbridge_agent.ini

```
vim /etc/neutron/plugins/ml2/linuxbridge_agent.ini
[linux_bridge]
physical_interface_mappings = provider:enp0s3,net10_0_3:enp0s9
```

2. 重啟服務

```
systemctl restart neutron-linuxbridge-agent.service
```



##### 建立一個flat網路

控制節點

```
openstack network create  --share --external --provider-physical-network net10_0_3 --provider-network-type flat jiage

openstack subnet create --network jiage --allocation-pool start=10.0.3.101,end=10.0.3.200 --dns-nameserver 10.0.3.1 --gateway 10.0.3.1 --subnet-range 10.0.3.0/24 jiage-sub
```



### cinder對接NFS

安裝nfs

```
yum install nfs-utils -y
```

編輯/etc/exports

```
/data 10.0.2.0/24(rw,async,no_all_squash,no_root_squash) 10.0.3.0/24(ro) 192.168.56.0(rw,async,no_all_squash,no_all_squash)
```

啟用服務

```
systemctl enable nfs rpcbind
systemctl start nfs rpcbind
```



存儲節點上設定

修改/etc/cinder/cinder.conf

```
[DEFAULT]
enabled_backends = sata,ssd,nfs
[nfs]
volume_driver = cinder.volume.drivers.nfs.NfsDriver
nfs_shares_config = /etc/cinder/nfs_shares
volume_backend_name = nfs
```

編輯nfs配置

```
vi /etc/cinder/nfs_shares
10.0.2.32:/data
```

重啟服務

```
systemctl restart openstack-cinder-volume
```

創建卷類型

>  管理員>卷>卷類型>創建卷類型

![image-20200312142711328](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200312142711328.png)

#### 把控制節點兼職計算節點

1. 安裝軟體包

   ```
   yum install openstack-nova-compute -y
   ```

2. 編輯vi /etc/nova/nova.conf

   ```
   [vnc]
   enabled = true
   server_listen = 0.0.0.0
   server_proxyclient_address = $my_ip
   novncproxy_base_url = http://controller:6080/vnc_auto.html
   ```

3. 啟用服務

   ```
   systemctl enable libvirtd.service openstack-nova-compute.service
   systemctl start libvirtd.service openstack-nova-compute.service
   
   systemctl restart neutron-linuxbridge-agent neutron-dhcp-agent neutron-metadata-agent neutron-server
   ```



#### 雲主機冷遷移

增加主機記憶體

```
usermod -s /bin/bash nova
su - nova
cp /etc/skel/.bash* .
```

生成密鑰對

```
ssh-keygen -t rsa -q -N ''
```

複製公鑰

cd .ssh

cp -fa id_rsa.pub authorized_keys

推送密鑰至各計算節點

scp -rp .ssh root@10.0.2.32:\`pwd\`



設定讓各節點不用密碼即可使用nova用戶登入其他計算節點

修改控制節點/etc/nova/nova.conf

```
[DEFAULT]

scheduler_default_filters=RetryFilter,AvailabilityZoneFilter,RamFilter,DiskFilter,ComputeFilter,ComputeCapabilitiesFilter,ImagePropertiesFilter,ServerGroupAntiAffinityFilter,ServerGroupAffinityFilter
```

```
systemctl restart openstack-nova-scheduler.service
```

修改計算節點/etc/nova/nova.conf

```
[DEFAULT]
allow_resize_to_same_host = true
```

### openstack創建虛擬機

#### 流程圖

![image-20200313135135222](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200313135135222.png)

1. horizon/cmd透過keystone驗證身分，透過就會取得一個token
2. 拿著token去找nova-api將各種屬性寫入nova db
3. 將任務丟給nova-scheduler，並開始分析現有的資源剩餘情況，如果資源足夠就會找出最適合的compute節點來創建虛擬機
4. nova透過glance尋找虛擬機的鏡像，並將鏡像下載下來
5. 透過neutron-server尋找網路資源，並驗證權限

### 定製化腳本

```
#!/bin/bash
yum install mariadb-server -y

system start mariadb
```

容器機

ip netns 

ip netns exec qdhcp- /bin/bash

ip addr

169.254.169.254

啟動一個80服務 /bin/neutron-ns-metadata-proxy

控制節點nova的配置

[neutron]
url = http://controller:9696
auth_url = http://controller:5000
auth_type = password
project_domain_name = default
user_domain_name = default
region_name = RegionOne
project_name = service
username = neutron
password = NEUTRON_PASS
service_metadata_proxy = true
metadata_proxy_shared_secret = METADATA_SECRET

[root@controller ~]# cat /etc/neutron/metadata_agent.ini
[DEFAULT]
nova_metadata_host = controller
metadata_proxy_shared_secret = METADATA_SECRET

[root@controller ~]# cat /etc/neutron/dhcp_agent.ini
[DEFAULT]
interface_driver = linuxbridge
dhcp_driver = neutron.agent.linux.dhcp.Dnsmasq
enable_isolated_metadata = true

### VxLAN

控制節點

1. 先把flat網路的設備刪除

2. 編輯/etc/neutron/neutron.conf

   ```
   [DEFAULT]
   core_plugin = ml2
   service_plugins = router
   allow_overlapping_ips = True
   ```

3. 編輯/etc/neutron/plugins/ml2/ml2_conf.ini

   ```
   [ml2]
   type_drivers = flat,vlan,vxlan
   tenant_network_types = vxlan
   mechanism_drivers - linuxbridge,L2population
   
   [ml2_type_vxlan]
   vni_ranges = 1:10000
   ```

   ※vlan 1-4094 vxlan 4096*4096 - 2 = 1678萬個網段

4. 配置/etc/neutron/plugins/ml2/linuxbridge_agent.ini

   ```
   [vxlan]
   enable_vxlan = True
   Local_ip = OVERLAP_INTERFACE_IP_ADDRESS
   L2_population = True
   ```

5. 各節點增加一塊網卡，IP位址與OVERLAP_INTERFACE_IP_ADDRESS相同

6. 配置L3 agent /etc/neutron/l3_agent.ini

   ```
   [DEFAULT]
   interface_driver = neutron.agent.linux.interface.BridgeInterfaceDriver
   external_network_bridge = 
   ```

7. 重啟所有服務

   ```
   systemctl restart neutron-server.service neutron-linuxbridge-agnet.service neutron-dhcp-agent.service neutron-metadata-agent.service
   ```

8. 啟用l3服務

   ```
   systemctl enable neutron-l3-agnet.service
   systemctl start neutron-l3-agnet.service
   ```

計算節點

1. 配置/etc/neutron/plugins/ml2/linuxbridge_agent.ini

   ```
   [vxlan]
   enable_vxlan = True
   Local_ip = OVERLAP_INTERFACE_IP_ADDRESS
   L2_population = True
   ```

2. ```
   systemctl restart neutron-linuxbridge-agnet.service
   ```

3. 

編輯

vim /etc/openstack-dashboard/local_settings 

OPENSTACK_NEUTRON_NETWORK = {

 'enable_router' : True,

'enable_quotas' : False,

'enable_ipv6' : False,

'enable_distributed_router' : False,

'enable_ha_router' : False,

'enable_lb' : False,

'enable_firewall' : False

}



```
systemctl restart httpd
```


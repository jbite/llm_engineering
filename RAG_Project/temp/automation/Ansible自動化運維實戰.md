# Ansible自動化運維實戰

#### 搭建環境

* ```
  主機部屬
  ansible-master 192.168.56.31
  ansible-node1 192.168.56.32
  ansible-node2 192.168.56.33
  ansible-node3 192.168.56.34
  ```

* 主機名稱解析

  ```
  cat >> /etc/hosts<<EOF
  192.168.56.31 ansible-master
  192.168.56.32 ansible-node1
  192.168.56.33 ansible-node2
  192.168.56.34 ansible-node3
  EOF
  ```

* 安裝epel源

  ```
  yum install -y epel-release
  ```

* 安裝

  ```
  yum install -y ansible
  ```

* ansible幫助

  * rpm -qc ansible
  * ansible --help
  * ansible-doc -l

* ssh-key(可選)

### ansible基礎

#### ansible主機加入

在/etc/ansible/hosts加入受控的主機

```
cat /etc/ansible/hosts
ansible-node1
ansible-node2
```

#### Inventory file 設定

```
ansible localhost -m ping
localhost | SUCCESS => {
    "changed": false,
    "ping": "pong"
}

ansible localhost -m ping -o 
localhost | SUCCESS => {"changed": false, "ping": "pong"}

#輸入密碼
ansible ansible-node2 -m ping -u root -k -o
SSH password:
ansible-node2 | SUCCESS => {"ansible_facts": {"discovered_interpreter_python": "/usr/bin/python"}, "changed": false, "ping": "pong"}

#incentory file
#1.首先在/etc/ansible/hosts中修改
[webserver]
ansible-node1
ansible-node2

##2. 使用群組名稱來進行操作
ansible webserver -m ping -o
ansible-node3 | SUCCESS => {"ansible_facts": {"discovered_interpreter_python": "/usr/bin/python"}, "changed": false, "ping": "pong"}
ansible-node2 | SUCCESS => {"ansible_facts": {"discovered_interpreter_python": "/usr/bin/python"}, "changed": false, "ping": "pong"}
ansible-node1 | SUCCESS => {"ansible_facts": {"discovered_interpreter_python": "/usr/bin/python"}, "changed": false, "ping": "pong"}
```

inventory 子群組

將database及dotcms加入ubuntu群組

```
[database]
db01
db02

[dotcms]
dotcms01
dotcms02
dotcms03

[ubuntu:children]
database
dotcms
```



#### inventory常用變數

- ansible_connection

  Connection type to the host. This can be the name of any of ansible’s connection plugins. SSH protocol types are `smart`, `ssh` or `paramiko`. The default is smart. Non-SSH based types are described in the next section.

General for all connections:

- ansible_host

  The name of the host to connect to, if different from the alias you wish to give to it.

- ansible_port

  The connection port number, if not the default (22 for ssh)

- ansible_user

  The user name to use when connecting to the host

- ansible_password

  The password to use to authenticate to the host (never store this variable in plain text; always use a vault. See [Variables and Vaults](https://docs.ansible.com/ansible/latest/user_guide/playbooks_best_practices.html#best-practices-for-variables-and-vaults))

Specific to the SSH connection:

- ansible_ssh_private_key_file

  Private key file used by ssh. Useful if using multiple keys and you don’t want to use SSH agent.

- ansible_ssh_common_args

  This setting is always appended to the default command line for **sftp**, **scp**, and **ssh**. Useful to configure a `ProxyCommand` for a certain host (or group).

- ansible_sftp_extra_args

  This setting is always appended to the default **sftp** command line.

- ansible_scp_extra_args

  This setting is always appended to the default **scp** command line.

- ansible_ssh_extra_args

  This setting is always appended to the default **ssh** command line.

- ansible_ssh_pipelining

  Determines whether or not to use SSH pipelining. This can override the `pipelining` setting in `ansible.cfg`.

- ansible_ssh_executable (added in version 2.2)

  This setting overrides the default behavior to use the system **ssh**. This can override the `ssh_executable` setting in `ansible.cfg`.

Privilege escalation (see [Ansible Privilege Escalation](https://docs.ansible.com/ansible/latest/user_guide/become.html#become) for further details):

- ansible_become

  Equivalent to `ansible_sudo` or `ansible_su`, allows to force privilege escalation

- ansible_become_method

  Allows to set privilege escalation method

- ansible_become_user

  Equivalent to `ansible_sudo_user` or `ansible_su_user`, allows to set the user you become through privilege escalation

- ansible_become_password

  Equivalent to `ansible_sudo_password` or `ansible_su_password`, allows you to set the privilege escalation password (never store this variable in plain text; always use a vault. See [Variables and Vaults](https://docs.ansible.com/ansible/latest/user_guide/playbooks_best_practices.html#best-practices-for-variables-and-vaults))

- ansible_become_exe

  Equivalent to `ansible_sudo_exe` or `ansible_su_exe`, allows you to set the executable for the escalation method selected

- ansible_become_flags

  Equivalent to `ansible_sudo_flags` or `ansible_su_flags`, allows you to set the flags passed to the selected escalation method. This can be also set globally in `ansible.cfg` in the `sudo_flags` option

Remote host environment parameters:

- ansible_shell_type

  The shell type of the target system. You should not use this setting unless you have set the [ansible_shell_executable](https://docs.ansible.com/ansible/latest/user_guide/intro_inventory.html#ansible-shell-executable) to a non-Bourne (sh) compatible shell. By default commands are formatted using `sh`-style syntax. Setting this to `csh` or `fish` will cause commands executed on target systems to follow those shell’s syntax instead.

- ansible_python_interpreter

  The target host python path. This is useful for systems with more than one Python or not located at **/usr/bin/python** such as *BSD, or where **/usr/bin/python** is not a 2.X series Python. We do not use the **/usr/bin/env** mechanism as that requires the remote user’s path to be set right and also assumes the **python** executable is named python, where the executable might be named something like **python2.6**.

- ansible_*_interpreter

  Works for anything such as ruby or perl and works just like [ansible_python_interpreter](https://docs.ansible.com/ansible/latest/user_guide/intro_inventory.html#ansible-python-interpreter). This replaces shebang of modules which will run on that host.

*New in version 2.1.*

- ansible_shell_executable

  This sets the shell the ansible controller will use on the target machine, overrides `executable` in `ansible.cfg` which defaults to **/bin/sh**. You should really only change it if is not possible to use **/bin/sh** (i.e. **/bin/sh** is not installed on the target machine or cannot be run from sudo.).

#### 當每台主機密碼不同時

```
#在/etc/ansible/hosts中加入個別主機帳號密碼
[webserver]
ansible-node[1:3] ansible_ssh_user='root' ansibel_ssh_pass='password'
```

#### 增加端口

```
#修改/etc/ansible/hosts主機紀錄的port變數
ansible-node2 ansible_ssh_port=2222
```

#### 對主機組定義變數

```
[webserver]
ansible-node[1:3]
[webserver:vars]
ansible_ssh_user='root'
ansible_ssh_pass='password'
ansible_ssh_port=22
```

#### 子分組

```
[apache]
host[1:2]
[nginx]
host[3:4]
[webserver:children]
apache
nginx
[webserver:vars]
ansible_ssh_user='root'
ansible_ssh_pass='password'
```

####自定義主機列表

```
#指定外部主機列表文件
ansible -i /root/hostlist

cat /root/hostlist
[docker]
host1
host2
```

### ad-hoc點對點模式

#### 複製模塊

* 幫助: ansible-doc copy

* ```
  copy:
    src: /srv/myfiles/foo.conf #ansible
    dest: /etc/foo.conf        #host
    owner: foo
    group: foo
    mode: '0644'
  ```

* 案例:

* ```
  #複製文件到host中
  ansible webserver -m copy -a 'src=/etc/hosts dest=/etc/hosts owner=root group=root mode=700'
  
  #備份文件 如果文件不同 會多備份一份
  ansible webserver -m copy -a 'src=/etc/hosts dest=/etc/hosts owner=root group=root mode=700 backup=yes'
  ```

#### 用戶模塊

* ansible-doc user

* 創建用戶

  ```
  ansible ansible-node1 -m user -a 'name=qianfeng state=present'
  ```

* 刪除用戶

  ```
  ansible ansible-node1 -m user -a 'name=qianfeng state=absent'
  ```

* 修改密碼

  ```
  #生成加密密碼
  echo 777777 | openssl passwd -1 -stdin
  $1$m1ytoh9i$RDi4rwjZtGONfvHsYfRso1
  #修改密碼
   ansible ansible-node1 -m user -a 'name=qianfeng password="$1$m1ytoh9i$RDi4rwjZtGONfvHsYfRso1"'
  ```

  $1$kDE7bUHa$p3j9k.1x967YnytbxhTzk/

* 修改bash

  ```
   ansible ansible-node1 -m user -a 'name=qianfeng shell=/sbin/nologin append=yes'
  ```

#### 軟件包模塊

* ansible-doc 

* 安裝軟體

  ```
  ansible ansible-node1 -m yum -a 'name=httpd state=latest'
  ```

* 升級所有包

  ```
  ansible ansible-node1 -m yum -a 'name="*" state=latest'
  ```

#### 服務模塊

* ansible ansible-node1 -m service -a 'name=httpd state=started'
* ansible ansible-node1 -m service -a 'name=httpd state=started enabled=yes'

* ansible ansible-node1 -m service -a 'name=http state=stopped'
* ansible ansible-node1 -m service -a 'name=http state=restarted'

#### 文件管理模塊

* 創建文件

* ```
  ansible ansible-node1 -m file -a 'path=/tmp/88.txt mode=777 state=touch'
  ```

* 創建目錄

* ```
  ansible ansible-node1 -m file -a 'path=/tmp/99 mode=777 state=directory'
  ```

#### 收集模塊

* 主要用來查看主機訊息 例如 硬碟 記憶體 使用者登入狀態

* 查看IP地址

* ```
  ansible ansible-node1 -m setup -a 'filter=ansible_all_ipv4_addresses'
  ```

#### shell模塊

* 可以做到以上所有模塊的功能

* ```
  ansible ansible-node1 -m shell -a 'hostname' -o
  ```

  

* ```
  ansible ansible-node1 -m shell -a 'yum -y install httpd' -o
  ```

### YAML-YAML Ain't Markup Language 非標記語言

語法:

* 列表

  * ```
    fruits:
    	- Apple
    	- Orange
    	- Strawberry
    	- Mango
    ```

* 字典

  * ```
    martin:
    	name: Martin D'vloper
    	job: Developer	
    ```

示例: 

```
#清理目標主機
ansible ansible-node1 -m yum -a 'name=httpd state=removed'
#在ansible安裝httpd
yum install -y httpd
mkdir /apache
cd /apache
cp -rf /etc/httpd/conf/httpd.conf .
grep '^Listen' httpd.conf
```

編寫劇本

```
cat apache.yaml
---
- hosts: ansible-node1
  tasks:
  - name: install apache packages
    yum: name=httpd state=present
  - name: copy apache conf file
    copy: src=./httpd.conf dest=/etc/httpd/conf/httpd.conf
  - name: ensure apache is running
    service: name=httpd state=started enabled=yes
```

執行劇本

```
ansible-playbook apache.yaml
```

語法測試

```
ansible-playbook apache.yaml --list-hosts
playbook: apache.yaml

  play #1 (ansible-node1): ansible-node1        TAGS: []
    pattern: [u'ansible-node1']
    hosts (1):
      ansible-node1
```

#### playbook handler

* 劇本發生變化: Listen 9000，呼叫handler來重啟httpd服務

* ```
  cat apache.yaml
  ---
  - hosts: ansible-node1
    tasks:
    - name: install apache packages
      yum: name=httpd state=present
    - name: copy apache conf file
      copy: src=./httpd.conf dest=/etc/httpd/conf/httpd.conf
      notify: restart apache service
    - name: ensure apache is running
      service: name=httpd state=started enabled=yes
    handlers:
    - name: restart apache service
      service: name=httpd state=restarted
  ```



#### role-角色扮演

劇本的目錄結構將代碼或文件進行模塊化 成為roles的文件目錄組織結構

##### 目標

##### 目錄結構

```
tree
.
├── apache.yaml
├── httpd.conf
└── roles           # actions for automatic install and config
    ├── nginx
    │   ├── defaults
    │   │   └── main.yaml
    │   ├── files
    │   │   └── index.html
    │   ├── handlers
    │   │   └── main.yaml
    │   ├── meta
    │   │
    │   ├── tasks
    │   │   └── main.yaml
    │   ├── templates
    │   └── vars
    │       └── main.yaml
    └── site.yaml

```

defaults/main.yaml

```
---
dotcms_path: /opt/dotcms
dotcms_user: dotcms
dotcms_group: dotcms
dotcms_db_name: dotcms
dotcms_db_user: dotcmsuser

```

編輯tasks/main.yaml

```
---
- name: install epel-release package
  yum: name=epel-release state=latest
- name: install nginx package
  yum: name=nginx state=latest
- name: copy index.html
  copy: src=index.html dest=/usr/share/nginx/nginx.conf
- name: copy nginx.conf template
  template: src=nginx.conf.j2 dest=/etc/nginx/nginx.conf
  notify: restart nginx
- name: make sure nginx service running
  service: name=nginx state=started enabled=yes

```

更改模板文件templates/nginx.conf.j2其中的變量

```
roles/nginx/templates/nginx.conf.j2

worker_processes {{ ansible_proccessor_cores }};

events {
    worker_connections {{ worker_connections }};
}
```

ansible_processor_cores會調用設備變數來複製到受控主機的配置文件中

```
ansible ansible-node1 -m setup -a 'filter=ansible_processor_cores'
ansible-node1 | SUCCESS => {
    "ansible_facts": {
        "ansible_processor_cores": 1,
        "discovered_interpreter_python": "/usr/bin/python"
    },
    "changed": false
}
```

worker_connections則需要在vars/main.yaml自定義變數

```
worker_connections:10240
```

編寫handlers

```
# cat roles/nginx/handlers/main.yaml
---
- name: restart nginx
  service: name=nginx state=restarted
```

handler dotcms範例

```
---
- name: dotcms reload systemd
  command: systemctl daemon-reload
```

編寫roles/site.yaml

```
- hosts: ansible-node1
  roles:
  - nginx
```

dotcms範例

```
---
- hosts: database
  become: true
  roles:
    postgresql
    
- hosts: dotcms
  become: true
  roles:
    - dotcms
    - java
```



檢查語法

```
ansible-playbook site.yaml --syntax-check
```

執行劇本

```
 ansible-playbook roles/site.yaml
```

### ansible with cisco ios

顯示bgp訊息

```
---
- name: Show BGP Neighbors
  hosts: routers
  gather_facts: false
  
  
  tasks:
    - name: BGP Neighbors
      raw: "show ip bgp neighbors"
      register: output
    - name: Print Output
      debug: var=output.stdout_lines
```

inventory

```
[NWSW]
sw18f1 ansible_host="10.255.0.18"
[NWSW:vars]
ansible_user='admin'
ansible_ssh_pass='mypassword'
ansible_network_os='ios'ZZ
```



ios_command module

執行結果後輸出畫面

vim ios_cmd.yaml

```
---
- name: Running show commands on Cisco IOS
  hosts: sw18f1
  gather_facts: false
  connection: network_cli
  
  tasks:
    - name: Run multiple commands on Cisco IOS nodes
      ios_command:
        commands:
          - show version
          - show interfaces
          
      register: print_output
      
    - debug: var=print_output.stdout_lines
    
...
```

#### saving output to a file

```
---
- name: Running show commands on Cisco IOS
  hosts: sw18f1
  gather_facts: false
  connection: network_cli
  
  tasks:
    - name: Run multiple commands on Cisco IOS nodes
      ios_command:
        commands:
          - show version
          
          
      register: my_config
      
    - name: Save output to  o file on disk
      copy:
        content: "{{my_config.stdout[0]}"
        dest: "/root/lab/ansible/{{inventory_hostname}}.txt"
...
```

#### how to use behavior parameter in playbook

execute by authentication

```
---
- name: Running show commands on Cisco IOS
  hosts: sw18f1
  gather_facts: false
  connection: network_cli
  #become: yes
  #become_method: enable

  vars:
    ansible_user: username
    ansible_ssh_pass: pass
    ansible_become: yes
    ansible_become_method: enable
    ansible_become_pass: enable_pass
    ansible_network_os: ios

  tasks:
    - name: Run privileges command
      ios_command:
        commands:
          - show run
      register: output

    - name: Print output
      debug: var=output.stdout_lines
...   
```

#### config cisco device by ios_config module

```
---
- name: Configuring Cisco IOS Devices
  hosts: sw18f1
  gather_facts: no
  connection: network_cli
 
 
  tasks:
    - name: Basic config
      ios_config:
        save_when: modified
        lines:
          - hostname "{{inventory_hostname}}"
          - ip name-server 8.8.8.8
          - no ip http server
          - ip http secure-server
      register: output

    - name: Print output 
      debug: var=output.stdout_lines
...   
```

#### ios_config module parent argument

```
---
- name: Configuring Cisco IOS Devices
  hosts: sw18f1
  gather_facts: no
  connection: network_cli
 
 
  tasks:
    - name: Basic config
      ios_config:
        save_when: modified
        lines:
          - network 0.0.0.0 0.0.0.0 area 0
          - distance 50
          - default-information originate
        parents: router ospf 1
      register: output

    - name: Print output 
      debug: var=output.stdout_lines
...   
```

#### backup config

```
---
- name: Configuring Cisco IOS Devices
  hosts: sw18f1
  gather_facts: no
  connection: network_cli
 
 
  tasks:
    - name: Basic config
      ios_config:
        backup: yes
...   
```

#### use loop with list and dict

[參考](https://docs.ansible.com/ansible/latest/user_guide/playbooks_loops.html#iterating-over-a-dictionary)

列表循環

```
---
- name: Run a Loop, create Multiple Users
  hosts: host
  gather_facts: no 
  become: yes
  become_user: root
  connection: ssh
  vars:
    new_users:
      - u1
      - u2
      - u3
  tasks:
    - name: Add user using a loop
      user:
        name: "{{item}}"
        state: present
        groups: sudo
      loop:
        "{{new_users}}"
...
```

列表循環

```
---
- name: Run a Loop, create Multiple Users
  hosts: host
  gather_facts: no 
  become: yes
  become_user: root
  connection: ssh
  
  tasks:
    - name: Add user using a loop
      user:
        name: "{{item.name}}"
        state: present
        groups: "{{item.groups}}"
      with_items:
        - {name: "test1", groups: "sudo"}
        - {name: "test2", groups: "wheel"}
...
```

字典循環  需要使用`dict2items` (dict filter)

```
 tasks:
    - name: Add user using a loop
      user:
        name: "{{item.name}}"
        state: present
        groups: "{{item.groups}}"
      loop: "{{ tags|dict2items }}"
       vars:
        configured_tools:
          java:
            envar: javaenv
            dir: javapath
          maven:
            envar: mavenenv
            dir: mavenpath
          candle:
            envar: candleenv
            dir: candlepath
```

![image-20200501194721632](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200501194721632.png)



### Ansible Vault

創建密碼文件

```
ansible-vault create vault.yaml

router1_become_password:
router2_become_password
```

更換密碼

```
ansible rekey vault.yaml
```

編輯vault文件

```
ansible edit vault.yaml
```

在hosts文件中更改

```
[routers]
router1 ansible_host=192.168.100.1 ansible_become_password="{{router1_become_password}}"
router2 ansible_host=192.168.100.2 ansible_become_password="{{router2_become_password}}"
```

```
ansible ipcdetector -m ping --ask-vault -e@./vault.yaml
```



#### play with limit

只執行-l 中的主機組或主機

```
ansible-playbook playbook.yaml -l balance
```

執行tags標記的劇本

```
---
- hosts: database
  become: true
  tasks:
    - name: install
      package: name=postgresql state=installed
    - name: service
      service: name=postgresql state=started enabled=yes
      tags: service
      
- hosts: dotcms
  become: true
  tasks:
    - name: install jre
      package: name=openjdk-8-jre state=installed

ansible-playbook playbook.yaml --tags service
```

### useful module

#### packaging module

```
- name: install jre
  package: name=openjdk-8-jre state=installed
```

```
- name: nodesource repository
  yum_repository:
    name: nodesource
    description: Nodesource Yum Repo
    baseurl: https://rpm.nodesouce.com
```

python

```
- name: tensorflow
  pip:
    name: tensorflow
    virtualenv: /opt/tensorflow
```



#### file module

#### system module



## 一些使用案例

### 安裝nginx

install_nginx.yaml

```
---
- hosts: web01
  become: true

  tasks:
    - name: copy yum.repo to remote server
      copy:
        src: "/root/ansible_lab/repo/nginx.repo"
        dest: "/etc/yum.repos.d/nginx.repo"
        owner: root
        group: root
        mode: 0644

    - name: install nginx
      yum:
        name: nginx
        state: latest

    - name: running nginx services
      service:
        name: nginx
        enabled: yes
        state: started

...
```

系統模組

![image-20200430101456927](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200430101456927.png)

## Ansible Galaxy

下載roles組件

```
ansible-galaxy collection roles_name
```

使用，創建一個playbook

```
---
- hosts: web01
  roles:
    - roles_name.role
```

### 角色依賴

在安裝tomcat之前，需要先安裝java。

在tomcat的roles加入一個meta/main.yaml

```
---
dependencies:
  - java
```

如此，在tomcat安裝之前，會先安裝java

如果，需要使用不同版本的安裝包

```
---
dependencies:
  - { role: java, java_package: openjdk-9-jre }
```

### Use template Library

write a file with template

template module

```
- name: configure
  template:
  src: haproxy.cfg
  dest: /etc/haproxy/haproxy.cfg
  owner: root
  group: root
  mode: 0644
```

使用範例

```
---
- name: install
  package: name=haproxy state=installed
  
- name: configure
  template:
    src: haproxy.cfg
    dest: /etc/haproxy/haproxy.cfg
    owner: root
    group: root
    mode: 0644
  notify: reload haproxy
  
- name: config rsyslog
  template:
    src: rsyslog.haproxy.conf
    dest: /etc/rsyslog.d/haproxy.conf
    owner: root
    group: root
    mode: 0644
  notify: restart rsyslog
  
- name: service
  service: name=haproxy state=started enabled=yes
```

template配置檔中要更改的部分使用雙大括號包起來

```
bind {{ haproxy_listen_address }}:{{ haproxy_listen_port }}
```

這個變量會放在vars/main.yaml中，也可以放在defaults/main.yaml

```
vim defaults/main.yaml
haproxy_listen_address=0.0.0.0
haproxy_listen_port=8080
```

template module changes

* hash of content( after variables applied)
* owner
* group
* mode

#### template flow control

* conditional sections in templates 使用jinjia2模板語言

* ```
  {% if haproxy_stats %}
   stats enable
   stats uri /haproxy?stats
   stats realm HAProxy Statistics
   stats auth admin:admin
  {% endif %}
  ```

* tests and compound expressions

#### repeated configuration content

use loop to control

```
{% for backend in haproxy_backends %}
   {{ backend }}
{% endif %}
```

vars/main.yaml

```
haproxy_backends:
  dotcms01:
    ip: 172.31.0.21
    port: 8080
  dotcms02:
    ip: 172.31.0.22
    port: 8080    
```

使用key及value

```
{% for key, value in haproxy_backends.iteritems() %}
   server {{ key }}{{value.ip }}:{{ port }} check
{% endif %}
```

使用macro來定義要重複的語句

backend.j2

```
{% macro backend(name, ip, port=8080) -%}
    server {{ name }} {{ ip }}:{{ port }} check
{%- endmacro %}
```

```
{% import 'backend.j2' as backend %}
{% for key, value in haproxy_backends.iteritems() %}
   backend(key, value.ip, value.port)
{% endif %}
```

#### Using Defaults and filters

* default values

  * 當變量在defaults中，就會使用filter給的值來賦值
  * {{ haproxy_listen_port|default('80') }}

* join

  * ```
    servers:
      - server01
      - server02
      - server03
      
    {{ servers|join(',')}}
    
    => server01,server02,server03
    ```

  * ![image-20200430223432996](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200430223432996.png)

* Map
  
  * ![image-20200430223654732](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200430223654732.png)

#### defined reusable block and inheritance

* create base template

  * 定義模板

    ```
    templates/nginx.base.j2
    user  nginx;
    worker_processes  1;
    
    {% block path_error_log %}
    {% endblock %}
    pid        /var/run/nginx.pid;
    
    
    events {
        worker_connections  10240;
    }
    
    
    http {
        include       /etc/nginx/mime.types;
        default_type  application/octet-stream;
    
        log_format  main  '$remote_addr - $remote_user [$time_local] "$request" '
                          '$status $body_bytes_sent "$http_referer" '
                          '"$http_user_agent" "$http_x_forwarded_for"';
    
        access_log  /var/log/nginx/access.log  main;
    
        sendfile        on;
        #tcp_nopush     on;
    
        keepalive_timeout  65;
    
        #gzip  on;
    
        include /etc/nginx/conf.d/*.conf;
    }
    ```

  * 模板寫入

    ```
    templates/nginx.conf.j2
    {% extends 'nginx.base.j2' %}
    {% block path_error_log %}
    error_log /var/log/nginx/error_log
    {% endblock %}
    ```

* extending a template

#### Maintaining roles and templates with variables

* using variables for all roles

  * role defaults:

    * dynamic values for a field, it is useful to create reusable role

  * the all group

    ```
    roles/group_vars/all.yaml
    ```

  * role variables in the playbook

* exploring ansible fact: facts 是anisble自動收集的系統資訊變數

  ```
  ansible all -m setup
  ```

  * setup module
  * example facts

* applying variable

  * vars
  * hosts vars: 在inventory裡面定義變量
  * group vars: 在inventory裡面定義
  * groups["group_name"] 代表inventory中定義的group裡的主機列表(list)

#### use variable to control tasks

```
--- 
- name: install jre (Debian)
  package: name=openjdk-8-jre state=installed
  when: ansible_os_family == 'Debian'
  
- name: install jre (RHEL)
  package: name=java-1.8.0-openjdk state=installed
  when: ansible_os_family == 'RedHat'
```

![image-20200501154750799](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200501154750799.png)

#### registering results in variable

* registing results into variables

  ```
  ---
  - name: check volume_path
    stat: 
      path: "{{ volume_path }}"
    register: vp
  - debug: 
      msg="{{ vp }}"
  
  - name: partition
    parted:
      device: "{{ volume_path }}"
      number: 1
      state: present
    when: vp.stat.isblk is defined and vp.stat.isblk
    
  - name: filesystem
    filesystem:
      fstype: xfs
      dev: "{{ volume_path }}1"
    when: vp.stat.isblk is defined and vp.stat.isblk
    
  - name: mount directory
    file:
      name: "{{ volume_mount_path }}"
      state: directory
      owner: "{{ volume_mount_user }}"
      group: "{{ volume_mount_group }}"
  ```

  

* checking results in conditional tasks

### store project in the vault

* ansible vault purpose
* way to manage secrets
* ansible vault

Encrypt file and decrypt file

* encrypting files with ansible vault

  ```
  ansible-vault encrypt group-var/all.yaml
  ```

* Using encrypted files

  ```
  ansible-playbook playbook.yaml --ask-vault-pass
  ```

* editing

  ```
  ansible-vault edit group_vars/all.yaml
  ```

#### create vault password file

使用密碼檔案加密文件

```
pwgen 16 1 > vault-password

ansible-vault encrypt vars/main.yaml --vault-password-file vault-password

ansible-playbook roles/site.yaml --vault-password-file vault-password
```

替加密文件加上vault-id方便辨識

```
ansible-vault create --vault-id pass1@source_file foo.yaml

ansible-vault create --vault-id pass1@prompt foo.yaml
```

### build module 

* adding a custom module to a role

  * 創建一個library目錄

  * 放入一個python文件

  * ```
    mkdir roles/library
    touch roles/library/dotcms_news.py
    ```

  * 在tasks中使用該module

  * ```
    roles/tasks/news.yaml
    ---
    - name: dependencies
      pip: name=requests state=present
      
    - name: Report sky status
      dotcms_news:
        server_url: "http://{{ ansible_default_ipv4.address }}:8080"
        username: "admin@dotcms.com"
        password: "admin"
        host_folder: "demo.dotcms.com"
        news_url: "sky-is-falling"
        publish_date: "01-01-2018"
        byline: "chicken little"
        title: "The Sky is Falling"
        story: "A piece of it hit me on the <b>head!</b>"
      tags:
        - news
    ```

  * main.yaml中導入該tasks

  * ```
    import_tasks: news.yaml
    ```

* running a custom module

* module basic structure

  ![image-20200502115015795](C:\Users\bited\AppData\Roaming\Typora\typora-user-images\image-20200502115015795.png)

### Module Arguments and Results

dotcms_news.py

```python
from ansible.module_utils.basic import AnsibleModule
def run_module():
  # Define the argument specification
  module_args = dict(       username=dict(type='str',required=True),
passworddict(type='str',required=True, no_log=True),
server_urldict(type='str',required=True),
host_folder=dict(type='str',required=True),
news_url=dict(type='str',required=True),
publish_date=dict(type='str',required=True),
byline=dict(type='str',required=True),
title=dict(type='str',required=True),
story=dict(type='str',required=True),
  )
#Declare the module results  
result = dict(
  changed=False,
  details='',
  inode='',
  identifier=''
)

module = AnsibleModule(
  argument_spec=module_args,
  supports_check_mode=True
)
```

#### Module Idempotence

* Doing work in a module
* checking existing state

## Using Ansible with other tools

### Ansible Docker connector

running a docker container

incentory file

```
postgresql ansible_connection=docker
```

### Ansible with Vagrant

## Ansible for DevOp 


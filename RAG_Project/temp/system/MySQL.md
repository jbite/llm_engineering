# MySQL

#### 關係型資料庫

用表儲存數據

表與表之間存在關聯關係

提供強大的查詢能力

通用查詢語言SQL

structural Query Language

#### 非關係型資料庫

redis

MySQL基本命令

```sql
---
show databases;
---
use db_name;
---
show tables;
```

賦予權限

```sql
grant all on *.* to root@'%' identified by '123456' with grant option;

grant all on *.* to admin@'192.168.56.37' identified by '12345678' with grant option;
---刷新權限
flush privileges;
```

修改密碼

```sql
update user set plugin='mysql_native_password' ,authentication_string=password("12345678") where User="root";
```

### 增刪改查

```sql
#增

insert into table_name values(
    name type,
...
);

insert into table_name values(v1,v2,
    v3 
);
#刪

#改


#查
```

### 查看表結構

```sql
desc table_name;

show create_table 表名;
```

### 數據類型

| 類型       | 大小     | 範圍有號 | 範圍無號 | 用途  |
| -------- | ------ | ---- | ---- | --- |
| TINYINT  | 1byte  |      |      |     |
| SMALLINT | 2 BYTE |      |      |     |
| MEDIUINT | 3BYTE  |      |      |     |
| INT      | 4BYTE  |      |      |     |

* 
- BIGINT 8byte



### 資料庫操作

```sql
---刪除庫
drop database db_name;

---新增庫
create databases db_name charset=utf8;
```

### 表操作

```sql
---新增表
create table students(
stu_id INT NOT NULL  AUTO_INCREMENT ,
name CHAR(32) not null,
age INT not null,
register_date date not null,
PRIMARY KEY (stu_id)
);

create table person (
 id int Not Null AUTO_INCREMENT, 
 name char(20) not null, 
 age int, 
 rmb float, 
 gender bool, 
 birthday date,
 primary key (id) 
);
---刪除表
drop table student;

---顯示表結構
desc table_name;
/*
+---------------+----------+------+-----+---------+----------------+
| Field         | Type     | Null | Key | Default | Extra          |
+---------------+----------+------+-----+---------+----------------+
| stu_id        | int(11)  | NO   | PRI | NULL    | auto_increment |
| name          | char(32) | NO   |     | NULL    |                |
| age           | int(11)  | NO   |     | NULL    |                |
| register_date | date     | NO   |     | NULL    |                |
+---------------+----------+------+-----+---------+----------------+

*/

---插入數據
insert into student (name, age,register_date) values('Jacky Feng',18,'2020-1-23');

---查詢數據
select * from person;
select stu_id,name,age from person;

---刪除數據
delete from person where name="馮哲琦"

---修改表數據
update student set name="馮哲旗" where name="Jacky Feng";

-- 修改表欄位屬性
alter table student drop column_name;
alter table student add column_name attr;
alter table student modify a attr;
alter table student change a b attr;
```

### 創建中國數據庫

```sql
create database china charset=utf8;

CREATE TABLE T_Province(
  ProID INT AUTO_INCREMENT,
  ProName VARCHAR(50) NOT NULL,
  ProSort INT,
  ProRemark VARCHAR(50),
  PRIMARY KEY (ProID)
);

CREATE TABLE T_City
(
    CityID INT  AUTO_INCREMENT,
    CityName VARCHAR(50)  NOT NULL,
    ProID INT,
    CitySort INT,
    PRIMARY KEY (CityID)
);

CREATE TABLE  T_District
(
    Id INT AUTO_INCREMENT,
    DisName VARCHAR(30) NOT NULL,
    CityID INT NOT NULL,
    DisSort INT,
    PRIMARY KEY (Id)
);
```

插入省分

```sql
insert T_Province(ProName,ProSort,ProRemark) Values('北京市','1','直轄市');
insert T_Province(ProName,ProSort,ProRemark) Values('天津市','2','直轄市');
insert T_Province(ProName,ProSort,ProRemark) Values('河北省','5','省份');
insert T_Province(ProName,ProSort,ProRemark) Values('山西省','6','省份');
insert T_Province(ProName,ProSort,ProRemark) Values('內蒙古自治區','32','自治區');
insert T_Province(ProName,ProSort,ProRemark) Values('遼寧省','8','省份');
insert T_Province(ProName,ProSort,ProRemark) Values('吉林省','9','省份');
insert T_Province(ProName,ProSort,ProRemark) Values('黑龍江省','10','省份');
insert T_Province(ProName,ProSort,ProRemark) Values('上海市','3','直轄市');
insert T_Province(ProName,ProSort,ProRemark) Values('江蘇省','11','省份');
insert T_Province(ProName,ProSort,ProRemark) Values('浙江省','12','省份');
insert T_Province(ProName,ProSort,ProRemark) Values('安徽省','13','省份');
insert T_Province(ProName,ProSort,ProRemark) Values('福建省','14','省份');
insert T_Province(ProName,ProSort,ProRemark) Values('江西省','15','省份');
insert T_Province(ProName,ProSort,ProRemark) Values('山東省','16','省份');
insert T_Province(ProName,ProSort,ProRemark) Values('河南省','17','省份');
insert T_Province(ProName,ProSort,ProRemark) Values('湖北省','18','省份');
insert T_Province(ProName,ProSort,ProRemark) Values('湖南省','19','省份');
insert T_Province(ProName,ProSort,ProRemark) Values('廣東省','20','省份');
insert T_Province(ProName,ProSort,ProRemark) Values('海南省','24','省份');
insert T_Province(ProName,ProSort,ProRemark) Values('廣西壮族自治區','28','自治區');
insert T_Province(ProName,ProSort,ProRemark) Values('甘肅省','21','省份');
insert T_Province(ProName,ProSort,ProRemark) Values('陕西省','27','省份');
insert T_Province(ProName,ProSort,ProRemark) Values('新疆维吾爾自治區','31','自治區');
insert T_Province(ProName,ProSort,ProRemark) Values('青海省','26','省份');
insert T_Province(ProName,ProSort,ProRemark) Values('寧夏回族自治區','30','自治區');
insert T_Province(ProName,ProSort,ProRemark) Values('重慶市','4','直轄市');
insert T_Province(ProName,ProSort,ProRemark) Values('四川省','22','省份');
insert T_Province(ProName,ProSort,ProRemark) Values('貴州省','23','省份');
insert T_Province(ProName,ProSort,ProRemark) Values('雲南省','25','省份');
insert T_Province(ProName,ProSort,ProRemark) Values('西藏自治區','29','自治區');
insert T_Province(ProName,ProSort,ProRemark) Values('澳門特别行政區','33','特别行政區');
insert T_Province(ProName,ProSort,ProRemark) Values('香港特别行政區','34','特别行政區');
```

#### 約束

PRIMARY : 主鍵約束

NOT NULL

DEFAULT 默認值約束

UNIQUE 唯一約束

### 單表復雜查詢

#### and, or,like, not like

找出不是區縣的區縣

```sql
SELECT * FROM T_District WHERE (DisName NOT LIKE '%區') AND (DisName NOT LIKE '%縣');
```

t查詢內蒙和新疆的省分ID

```sql
SELECT * FROM T_Province WHERE ProName LIKE '內蒙%' OR ProName LIKE '新疆%';
```

找出內蒙和新疆的所有地級市

```sql
SELECT * FROM T_City WHERE ProID=12 or ProID=44 ;
```

#### order by

#### limit

#### in

#### between

#### group by

分組是為了統計

查詢中國各省分有多少地級市

```sql
SELECT * FROM T_City;
---統計count(*)
select ProID, count(*) from T_City group by ProID;

---查詢中國各省分有多少地級市，降序取前五名
SELECT ProID,COUNT(CityID) AS cities FROM T_City GROUP BY ProID
ORDER BY cities DESC limit 5;

---查詢中國各省分有多少地級市，取超過10個的省分
SELECT ProID,COUNT(CityID) AS cities FROM T_City GROUP BY ProID HAVING cities > 10 ORDER BY ProID DESC;

---統計每個城市中有幾個區縣
SELECT CityID, COUNT(ID) AS Disnumber  FROM T_District GROUP BY CityID ORDER BY Disnumber DESC LIMIT 10;

---統計最多區縣的前十個城市
SELECT CityID, COUNT(ID) AS Disnumber  FROM T_District GROUP BY CityID ORDER BY Disnumber DESC LIMIT 10;
---列出最多區縣的前十個城市名
SELECT * FROM T_City
WHERE CityID in (
    SELECT temp.CityID from (
        SELECT CityID, COUNT(ID) AS Disnumber  
        FROM T_District 
        GROUP BY CityID 
        ORDER BY Disnumber DESC LIMIT 10
    )temp
);
```

#### 統計函數

* max() 
* min() 
* avg() 
* sum()
* count()

#### having

配合group by使用

#### DISTINCT

使重複field只出現一次

```sql
SELECT DISTINCT ProID FROM T_City; 
```

### 分組聚合

GROUP BY: 根據組可以求出聚合，但不能找出對應該值的其他字段，只有分組字段有效

MAX, MIN, SUM, AVG, COUNT



#### HAVING

```sql
用來過濾聚
```

### ORDER BY

排序

### LIMIT

限制用

```sql
select * from table group by pos limit 3;
```





### 校園資料庫

#### 一對一關係

根據班級找班主任

```sql
select * from teacher where id=(select id from clazz where name="小刀会");
```

根據班主任找班級

```sql
select * from clazz where masterid=(select id from teacher where name="steve");
```

#### 一對多查詢

為班級添加學生

```sql
update student set classid=(select id from clazz where id=1) where student.id=1;
update student set classid=(select id from clazz where id=2) where student.id=2;
update student set classid=(select id from clazz where id=3) where student.id=3;
update student set classid=(select id from clazz where id=4) where student.id=4;
update student set classid=(select id from clazz where id=1) where student.id=5;
update student set classid=(select id from clazz where id=2) where student.id=6;
update student set classid=(select id from clazz where id=3) where student.id=7;
update student set classid=(select id from clazz where id=4) where student.id=8;
update student set classid=(select id from clazz where id=1) where student.id=9;
update student set classid=(select id from clazz where id=2) where student.id=10;
update student set classid=(select id from clazz where id=3) where student.id=11;
update student set classid=(select id from clazz where id=4) where student.id=12;
```

根據班級查學生

```sql
select * from student where classid=(select id from clazz where name='小刀会');
```

根據學生查班級

```sql
select * from clazz where id=(select id from student where name='二郎神');
```

#### 多對多關係 使用一個第三方表紀錄兩張表之間的關係

* 如果A表中的一條紀錄對應B表中的多條紀錄 B表中的一條紀錄也對應A表中的多條紀錄 ，就稱A表和B表多對多的關係
* 例如：一門課程可以有多個學生，一個學生也可以選擇多門課程

```sql
--創建【學生_課程】中間表
CREATE TABLE student_course(
    sid INT not null,
    cid INT not null,
    primary key (sid,cid)
);
-- 如果沒有設置主鍵，可以通過修改表自斷的方式來添加【聯合主鍵】
-- alter table student_course add constraint s_c primary key (sid,cid);
```

插入學生-課程對應資料

```sql
-- 十三姨選修了Python
INSERT INTO student_course(sid,cid) values ((SELECT id AS sid FROM student WHERE name='十三姨')
,(SELECT id AS cid FROM course WHERE name='Python'));

-- 十三姨選修了Java
INSERT INTO student_course(sid,cid) values ((SELECT id AS sid FROM student WHERE name='十三姨')
,(SELECT id AS cid FROM course WHERE name='Java'));

-- 香香八婆選修了Java
INSERT INTO student_course(sid,cid) values ((SELECT id AS sid FROM student WHERE name='香香八婆')
,(SELECT id AS cid FROM course WHERE name='Java'));

-- 山本五十六選修了Java
INSERT INTO student_course(sid,cid) values ((SELECT id AS sid FROM student WHERE name='山本五十六')
,(SELECT id AS cid FROM course WHERE name='Java'));

-- 山本五十六選修了HTML
INSERT INTO student_course(sid,cid) values ((SELECT id AS sid FROM student WHERE name='山本五十六')
,(SELECT id AS cid FROM course WHERE name LIKE 'HTML%'))
```

透過學生名查所有選修課程

```sql
select * from course where id in (
    select cid from student_course where sid=(
        select id from student where name='十三姨'
    )
);
```

查詢選修Python的所有學生

```sql
select * from student where id in (select sid from student_course where cid=(select id from course where name='Python'));
```

## 聯合查詢

### UNION

```sql
--union
-- 中國各省的ID 省分名字
select ProID, ProName from T_Province
union
-- 河北所有地級市ID 名字
select CityID, CityName from T_City
where ProID=(select ProID from T_Province where ProName='河北省');

-- 查詢河北省有多少地級市
select ProID, count(CityID) AS Cities from T_City
where ProID=(select ProID from T_Province where ProName='河北省');

-- 統計各省地級市的數量 輸出省名 地級市數量
select T_Province.ProID, T_Province.ProName, count(CityID) AS Cities from (
    T_Province join T_City
    on T_City.ProID = T_Province.ProID
)
group by ProID
order by Cities desc;
-- 增加別名
select tp.ProID, tp.ProName, count(CityID) AS Cities from (
    T_Province tp join T_City tc
    on tc.ProID = tp.ProID
)
group by ProID
order by Cities desc;
```

#### JOIN

```sql
-- 求每個省分中最大的城市ID
select tp.ProID,tp.ProName, max(tc.CityID) as MaxCityID from (
    T_Province tp join T_City tc
    on tc.ProID = tp.ProID
)
group by tp.ProID;

-- 地級市最多的省分取前十名

-- 查詢擁有最多區縣的城市的前十名

-- 查詢擁有二十個以上區縣的城市，輸出城市名，區縣數量

select tc.CityName, count(td.ID) as "District Count" from (
    T_City tc join T_District td
    on
    tc.CityID = td.CityID
)
group by tc.CityID
having count(td.ID) > 20
limit 10;

-- 區縣最多的城市是哪個省的什麼城市 查詢結果包含省名 市名 區縣數量
select tp.ProName, tc.CityName, count(td.id),tp.ProRemark from(
    T_Province tp join T_City tc
    on tp.ProID = tc.ProID 
    join T_District td on
    tc.CityID = td.CityID
) 
where tp.ProRemark='省份'
group by  tc.CityID
order by count(td.id) desc
limit 10;
```

#### 內連接-基於左右兩表公用部分

左連接(left join)基於左右兩表公共的部分+左邊特有的部分

右連接(right join)基於左右兩表公共的部分+右邊特有的部分

```sql
-- 插入省市表獨有部分
insert into T_Province(ProName) values("中原省");
insert into T_City(CityName) values("洛杉磯市");
```

### 練習

```sql
-- 現有宜居城市排行如下:("寧波市","銀川市","宜春市","宜昌市","咸陽市","蕪湖市","泰州市","秦皇島市","南通市","南京市","昆明市")
-- 求各省分分別擁有多少宜居城市 降序排列 輸出省名 宜居城市數量
select tp.ProName, count(goodcity.CityID) from (
    (select * from T_City where CityName in ("寧波市","銀川市","宜春市","宜昌市","咸陽市","蕪湖市","泰州市","秦皇島市","南通市","南京市","昆明市")) goodcity join T_Province tp
on goodcity.ProID = tp.ProID)
group by tp.ProID
order by count(goodcity.CityID) desc;
```

#### SQL 事務

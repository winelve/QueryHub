# SQL 语句使用文档

根据提供的代码，以下是支持的 SQL 语句分类及其使用方法：

## 数据库操作

### 创建数据库

```sql
CREATE DATABASE database_name;
```

### 使用数据库

```sql
USE database_name;
```

### 删除数据库

```sql
DROP DATABASE database_name;
```

### 重命名数据库

```sql
ALTER DATABASE old_name RENAME TO new_name;
```

### 显示所有数据库

```sql
SHOW DATABASES;
```

## 表操作

### 支持的type 和 Constraint

```

type支持3种:
int float string


constraint的形式.
1.只能使用一下的5种:

primary_key
foregin_key(ref_tb,ref_column)
default(value)
not_null
unique


说明: [constraint] 就是表示  constraint1 constraint2 constraint3 ...    并且[]中的内容是可选的.
```

### 创建表

```sql
CREATE TABLE table_name (
    column1 datatype constraint1 constraint2 constraint3,
    column2 datatype constraint1,
    column3 datatype ,
    ...
);
```

例如：

```sql
CREATE TABLE users (
    id INT PRIMARY_KEY AUTO_INCREMENT,
    name VARCHAR NOT_NULL DEFAULT(unknown),
    email CHAR
);
```

### 删除表

```sql
DROP TABLE table_name;
```

### 显示所有表

```sql
SHOW TABLES;
```

### 查看表结构

```sql
DESC table_name;
```

## 表的修改操作

### 添加列

```sql
ALTER TABLE table_name ADD column_name datatype [constraints];
```

### 修改列的类型

```sql
ALTER TABLE table_name MODIFY column_name datatype;
```

### 删除列

```sql
ALTER TABLE table_name DROP column_name;
```

或带数据类型(没用)：

```sql
ALTER TABLE table_name DROP column_name datatype;
```

### 重命名列

```sql
ALTER TABLE table_name RENAME COLUMN old_name TO new_name;
```

### 重命名表

```sql
ALTER TABLE old_name RENAME TO new_name;
```

## 约束操作

### 添加约束

```sql
ALTER TABLE table_name ADD CONSTRAINT column_name [constraints];
```

例如：

```sql
ALTER TABLE users ADD CONSTRAINT order_id PRIMARY_KEY NOT_NULL;
```

### 删除约束

```sql
ALTER TABLE table_name DELETE CONSTRAINT column_name cosntraint;
```

例如：

```sql
ALTER TABLE users DELETE CONSTRAINT order_id PRIMARY_KEY;
```

## 数据操作语言 (DML)

### 插入数据

```sql
INSERT INTO table_name (column1, column2, ...) VALUES 
    (value1, value2, ...), 
    (value1, value2, ...);
```

### 查询数据

```sql
SELECT (c1,c2,c3,c4) FROM tb1 WHERE (c1>10 AND c2<100) JOIN (tb2,tb3) ORDERED BY (c1,c2...);
SELECT (*) FROM tb1 WHERE (c1>10 AND c2<100) JOIN (tb2,tb3) ORDER BY (c1,c2...);

//注意: 空格是必须的, 括号是必须的. WHERE, JOIN 和 ORDER BY 都是可选的.

```

WHERE 条件可以使用比较运算符（`>`, `<`, `=`, `!=`, `>=`, `<=`）和逻辑运算符（AND, OR）构建复杂查询。

### 更新数据

```sql
UPDATE tb_name SET (c1=v1,c2=v2,c3=v3) WHERE (c1>10 AND c2<100);
```

### 删除数据

```sql
DELETE tb_name WHERE (c1<10 AND c2>100);
```

## 注意事项

1. 所有 SQL 语句必须以分号(`;`)结尾
2. 关键字不区分大小写（例如：`CREATE` 和 `create` 被视为相同）
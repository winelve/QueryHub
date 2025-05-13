# QueryHub
Sophomore practical training course: DBMS

---

### 开发环境

- g++13.0.1
  - c++ 17(最低版本要求)
- cmake 3.10 (最低版本要求)
- 编译套件: Desktop Qt 6.8.1 MinGW 64-bit



### 关于:如何扩展sql的语法解析的文档

1. 首先先确定好,查询语句的格式和将要捕获的参数.

2. 然后在`AST.md`文档中确定好,解析出AST树的格式.

   请确保包含三个字段: cmd,op,object

3. 然后去sqlcmd对应的.h和.cpp中定义对应的解析器, 用于将 数据解析成 ast 树的形式.

4. 然后再parser中注册对应的解析器.

5. 然后去`params.json`文件添加,对应的参数和函数.

6. 接着去`FuncMap.h`文件,添加对应的匿名函数.

7. 最后在executor中去调用这个函数即可.



### Server发送的数据格式

```json
{
    "code": "status_code",
    "func": "ShowDatabases",
    "data": ["db1","db2"]
}


{
    "code": "status_code",
    "func": "ShowTables",
    "data": ["tb1","tb2"]
}


{
    "code": "status_code",
    "func": "ShowDatabases",
    "data": ["db1","db2"]
}


{
    "code": "status_code",
    "func": "DescribeTable",
    "data": [
        {
            "cname":"",
            "type":"",
            "cs_name":""
        }
    ]
}


{
    "code": "status_code",
    "func": "Select",
    "data": [
        ["c1","c2","c3","c4"],
        ["v11","v12","v13","v14"],
        ["v21","v22","v23","v24"],
    ]
}


// others

{
    "code" : "status_code",
    "func" : "func_name",
    "data": 
}


```




#ifndef DATAPROCESSOR_H
#define DATAPROCESSOR_H

/**
 * 单例模式的数据处理接口类,与解析器交互
 */

#include <vector>
#include <tuple>
#include "database.h"
#include "constants.h"
#include "user.h"

#include "filemanager.h"

class DataProcessor {
private:
    std::vector<Database> databases;
    std::vector<User> users;

    //当前数据库
    Database* currentDatabase;
    std::string currentDatabaseName;
    //当前用户
    User* currentUser;
    std::string currentUserName;

    //约束名->数据库名，表名
    std::unordered_map<std::string, std::pair<std::string, std::string>> constraintMap;

    //将字符串转为对应的权限数
    static const std::unordered_map<std::string, authorityNum> strToAuthorityNum;
private:

    DataProcessor();
public:

    static DataProcessor& GetInstance();

    //更新指针
    int UpdatePointer();
//用户操作
    int CreateUser(const std::string& userName,const std::string& userPassword); //创建用户(注册)
    int Login(const std::string& userName,const std::string& userPassword); //登入
//GET
    int GetCurrentDatabase(std::string& databaseName);//给参数赋值
    int GetCurrentUser(std::string& userName);//给参数赋值
//数据库操作
    int CreateDatabase(std::string databaseName);                              //创建数据库
    int DeleteDatabase(std::string databaseName);                               //删除数据库
    int AlterDatabaseName(std::string databaseName,std::string newDatabaseName); //修改数据库名
    int UseDatabase(std::string databaseName = "");                                 //使用数据库
    int ShowDatabases(std::vector<std::string>& allDatabases);      //显示所有数据库

//表DDL操作
    int ShowTables(std::vector<std::string>& returnTables);                                     //显示所有表
    int CreateTable(std::string tableName, std::vector<std::pair<std::string, std::string>> fields, std::vector<Constraint *> constraints);
    int DropTable(std::string tableName);                                      //删除表
    int AlterTableName(const std::string& databaseName,const std::string& tableName, const std::string& newTableName); //修改表名
    int DescribeTable(std::string tableName,std::vector<std::pair<std::string, std::string>>& fields,std::vector<Constraint*>&    constraints);//查看表结构
    int UpdateConstraintMap();
    int AlterTableAdd(std::string tableName, std::pair<std::string, std::string> field);//修改表结构,增加字段
    int AlterTableDrop(std::string tableName, std::string fieldName);          //删除字段
    int AlterTableModify(std::string tableName, std::pair<std::string, std::string> field); //修改字段内容(类型修改)
    int AlterTableColumnName(std::string tableName, std::string columnName, std::string newColumnName);//修改列名
    int AlterTableConstraint(std::string tableName, Constraint* constraint);    //添加约束条件
    int AlterTableDeleteConstraint(std::string tableName, std::string constraintName); //删除约束
    int ShowConstraints(std::vector<std::vector<std::any>>& retRecords);
//表DML操作
    int Select(std::string tableName,
               std::vector<std::string> fieldName,
               std::vector<std::tuple<std::string, std::string, int>> conditions,
               std::vector<std::vector<std::any>> &returnRecords,
               const std::vector<std::string>& orderbyKey = std::vector<std::string>());   //单表查询
    int Select(std::vector<std::string> tablesName,
               std::vector<std::string> fieldName,
               std::vector<std::tuple<std::string, std::string, int>> conditions,
               std::vector<std::vector<std::any>> &returnRecords,
               const std::vector<std::string>& orderbyKey = std::vector<std::string>());   //多表查询
    int Insert(const std::string& tableName,const std::vector<std::pair<std::string,std::string>>& records);   //插入记录 records中 pair<field, values>

    int Update(const std::string& tableName,const std::vector<std::pair<std::string, std::string>>& values, const std::vector<std::tuple<std::string, std::string, int>>& conditions);   //更新(修改)记录

    int Delete(const std::string& tableName,const std::vector<std::tuple<std::string, std::string, int>>& conditions);   //删除记录
//Index
    int BuildIndex(const std::string& tableName,const std::string& fieldName);
    int DestroyIndex(const std::string& tableName,const std::string& fieldName);

//文件操作
    int Write();//写入
    int Read(bool isPrint);//读取
//权限操作
    bool isRoot(); //判断当前用户是否为root用户(root拥有全部权限)

    bool isDatabaseOwner(const std::string& databaseName);//判断当前用户是否为对应数据库拥有者(是否有管理该数据库权限)

    int GrantDatabaseOwner(const std::string& databaseName,const std::string& userName);//(只有root能进行)授予用户数据库所有权

    int GrantAuthority(const std::string& userName, const std::string& databaseName,const std::string& tableName,const std::string& authority) ;//授予权限

    int RevokeDatabaseOwner(const std::string& databaseName, const std::string& userName = "");//(只有root能进行)收回用户数据库所有权

    int RevokeAuthority(const std::string& userName, const std::string& databaseName,const std::string& tableName,const std::string& authority) ;//收回权限

};

#endif // DATAPROCESSOR_H

#ifndef DATAPROCESSOR_H
#define DATAPROCESSOR_H

/**
 * 单例模式的数据处理类,与解析器交互
 */

#include <vector>
#include "database.h"
#include "constants.h"


#include "filemanager.h"

class DataProcessor {
private:
    std::vector<Database> databases;
    // std::vector<User> users;

    Database* currentDatabase;
    std::string currentDatabaseName;
    std::string currentUserName;

    //约束名->数据库名，表名
    std::unordered_map<std::string, std::pair<std::string, std::string>> constraintMap;
private:

    DataProcessor();
public:

    static DataProcessor& GetInstance();

    //更新指针
    int UpdatePointer();

    int GetCurrentDatabase(std::string& databaseName);
//数据库操作
    int CreateDatabase(std::string databaseName);                              //创建数据库
    int DeleteDatabase(std::string databaseName);
    int UseDatabase(std::string databaseName = "");                                 //使用数据库
    int ShowDatabases(std::vector<std::string>& allDatabases);      //显示所有数据库

//表DDL操作
    int ShowTables(std::vector<std::string>& return_tables);                                     //显示所有表
    int CreateTable(std::string table_name, std::vector<std::pair<std::string, std::string>> fields, std::vector<Constraint *> constraints);
    int DropTable(std::string table_name);                                      //删除表
    int DescribeTable(std::string table_name,std::vector<std::pair<std::string, std::string>>& fields,std::vector<Constraint*>&    constraints);//查看表结构
    int UpdateConstraintMap();
    int AlterTableAdd(std::string table_name, std::pair<std::string, std::string> field);//修改表结构,增加字段
    int AlterTableDrop(std::string table_name, std::string field_name);          //删除字段
    int AlterTableModify(std::string table_name, std::pair<std::string, std::string> field); //修改字段
    int AlterTableConstraint(std::string table_name, Constraint* constraint);    //添加约束条件
    int AlterTableDeleteConstraint(std::string table_name, std::string constraint_name); ///删除约束
    int ShowConstraints(std::vector<std::vector<std::any>>& ret_records);
//表DML操作

//文件操作
    int Write();
    int Read(bool isPrint);

};

#endif // DATAPROCESSOR_H



#ifndef TABLE_H
#define TABLE_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <any>
#include <map>
#include "all_constraints.h"
#include "constants.h"

#include "sqltool.h"

class Database;

class Table
{
private:

    std::string tableName;//表名
    std::vector<std::pair<std::string,std::string>> fieldList;//字段列表 <字段名,类型>
    std::unordered_map<std::string,std::string> fieldMap;//字段哈希表
    std::vector<Constraint*> constraints;//约束
    std::vector<std::unordered_map<std::string, std::any>>  records;//记录


    int index;//索引
public:


    Table(const std::string &tableName,
          const std::vector<std::pair<std::string,std::string>> & fieldList = {},
          const std::vector<Constraint*>& constraints = {},
          const std::vector<std::unordered_map<std::string, std::any>>&  records = {}
          );

    ~Table();//析构函数

    Table(const Table& t);//拷贝构造函数

    Table& operator = (Table other);//拷贝赋值函数

    void swap(Table& s1, Table& s2);
    //GET
    const std::string& GetTableName() const;

    const std::vector<std::pair<std::string,std::string>>& GetFieldList() const;

    const std::vector<std::unordered_map<std::string, std::any>>&  GetRecords() const;

    const std::vector<Constraint*>& GetConstraints() const;

    //SET
    void SetTableName(const std::string& tableName);

    int FindField(std::string fieldName, std::any value) const;//查询字段

    //描述表（字段信息、约束等）
    int DescribeTable(std::vector<std::pair<std::string, std::string>>& fields,
                      std::vector<Constraint*>& constraints);


    //检查唯一
    int CheckUnique(std::string fieldName) const;
    //检查数据类型
    int CheckDataType(std::string type, std::string value);

    //增加字段
    int AlterTableAdd(std::pair<std::string, std::string> new_field);
    int AlterTableDrop(std::string field_name, Database* db);
    int AlterTableModify(std::pair<std::string, std::string> field);
    int AlterTableConstraint(Constraint* constraint);

    //根据表名来删除外键约束。
    int DropForeignReferedConstraint(std::string table_name);
    //根据表名和字段名来删除外键约束。
    int DropForeignReferedConstraint(std::string table_name, std::string field_name);
    //删除约束
    int DeleteConstraint(std::string constraint_name, Database* db);
};

#endif // TABLE_H



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


    // int index;//索引
    // Index* indexPtr; //索引
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

    const std::unordered_map<std::string, std::string>& GetFieldMap() const;
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
//DDL
    //增加字段
    int AlterTableAdd(std::pair<std::string, std::string> new_field);
    //删除字段
    int AlterTableDrop(std::string field_name, Database* db);
    //修改字段
    int AlterTableModify(std::pair<std::string, std::string> field);
    //修改列名
    int AlterTableColumnName(std::string columnName, std::string newColumnName);
    //修改约束
    int AlterTableConstraint(Constraint* constraint);

    //根据表名来删除外键约束。
    int DropForeignReferedConstraint(std::string table_name);
    //根据表名和字段名来删除外键约束。
    int DropForeignReferedConstraint(std::string table_name, std::string field_name);
    //删除约束
    int DeleteConstraint(std::string constraint_name, Database* db);
//DML
    //判断记录是否符合条件
    int CheckCondition(const std::unordered_map<std::string, std::any>& record, const std::vector<std::tuple<std::string, std::string, int>>& conditions);

    //查询
    int Select(std::vector<std::string> fieldName,
               std::vector<std::tuple<std::string, std::string, int>> conditions,
               std::vector<std::vector<std::any>> &returnRecords , const std::vector<std::string>& orderbyKey = std::vector<std::string>());

    //判断记录是否被引用
    int CheckBeingRefered(const std::unordered_map<std::string, std::any>& record, Database* db, std::string fieldName = "");
    //判断约束
    int CheckConstraint(std::unordered_map<std::string, std::any>& record, Database* db);
    //判断约束
    int CheckConstraint(std::unordered_map<std::string, std::any>& record, Database* db, std::vector<std::unordered_map<std::string, std::any> > records, int currentRecordIndex);
    //插入记录
    int Insert(std::vector<std::pair<std::string,std::string>> recordsOfIn, Database * db);
    //直接进行记录更新
    int ApplyFieldUpdate(std::vector<std::unordered_map<std::string, std::any>>& records, const std::vector<std::pair<std::string, std::string>>& values,const std::vector<std::tuple<std::string, std::string, int>>& conditions, Database *db, int checkConstraintsOrNot = 0);
    //更新(修改)记录
    int Update(const std::vector<std::pair<std::string, std::string>>& values,const std::vector<std::tuple<std::string, std::string, int>>& conditions, Database* db);
    //删除记录
    int Delete(const std::vector<std::tuple<std::string, std::string, int>>& conditions, Database* db);
};

#endif // TABLE_H

#ifndef DATABASE_H
#define DATABASE_H

#include "table.h"
#include <iostream>
#include <vector>
#include<algorithm>
#include "all_constraints.h"


class Database {
private:
    std::string ownerUser;//数据库拥有者可授予或收回权限(但无法与root)
    std::string databaseName;
    std::vector<Table> tables;

private:

    Database();

public:

    Database(std::string databaseName, std::string ownerUser);
//GET
    const std::string& GetOwnerUser() const;
    const std::string& GetDatabaseName() const;
    const std::vector<Table>& GetTables() const;
//SET
    void SetTables(const std::vector<Table>& tables);
    void SetDatabaseName(const std::string databaseName);
    void SetTableName(const std::string& tableName, const std::string& newTableName);
    void SetOwnerUser(const std::string& ownerName);
//FIND
    int FindField(std::string tableName, std::string fieldName, std::any = nullptr);
    int FindTable(std::string tableName);
    int FindTable(std::string tableName, Table& returnTable);
    Table& FindTableReference(std::string tableName);
//DDL
    int DescribeTable(std::string tableName,std::vector<std::pair<std::string, std::string>>& fields,std::vector<Constraint*>& constraints);
    int ShowTables(std::vector<std::string>& returnTables);
    int CreateTable(std::string tableName, std::vector<std::pair<std::string, std::string>> fieldList, std::vector<Constraint *> constraints);
    int DropTable(std::string tableName);

    int AlterTableAdd(std::string tableName, std::pair<std::string, std::string> field);
    int AlterTableDrop(std::string tableName, std::string fieldName);
    int AlterTableModify(std::string tableName, std::pair<std::string, std::string> field);
    int AlterTableColumnName(std::string tableName, std::string columnName, std::string newColumnName);
    int AlterTableConstraint(std::string tableName, Constraint* constraint);
    int AlterTableDeleteConstraint(std::string tableName, std::string constraint_name);

    int CheckUnique(std::string tableName, std::string fieldName);
//DML
    int CheckCondition(const std::unordered_map<std::string, std::any>& record,
                                 const std::vector<std::tuple<std::string, std::string, int>>& conditions, std::unordered_map<std::string, std::string> fieldMap);
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

    //插入记录
    int Insert(const std::string& tableName,const std::vector<std::pair<std::string,std::string>>& records);
    //更新(修改)记录
    int Update(const std::string& tableName,const std::vector<std::pair<std::string, std::string>>& values, const std::vector<std::tuple<std::string, std::string, int>>& conditions);
    int Delete(const std::string& tableName,const std::vector<std::tuple<std::string, std::string, int>>& conditions);   //删除记录
//Index
    int BuildIndex(const std::string& tableName,const std::string& fieldName);
    int DestroyIndex(const std::string& tableName,const std::string& fieldName);
};

#endif // DATABASE_H

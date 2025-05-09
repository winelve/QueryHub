#ifndef DATABASE_H
#define DATABASE_H

#include "table.h"
#include <iostream>
#include <vector>
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
    Table& FindTableReference(std::string table_name);
//DDL
    int DescribeTable(std::string table_name,std::vector<std::pair<std::string, std::string>>& fields,std::vector<Constraint*>& constraints);
    int ShowTables(std::vector<std::string>& returnTables);
    int CreateTable(std::string tableName, std::vector<std::pair<std::string, std::string>> fieldList, std::vector<Constraint *> constraints);
    int DropTable(std::string tableName);

    int AlterTableAdd(std::string table_name, std::pair<std::string, std::string> field);
    int AlterTableDrop(std::string table_name, std::string field_name);
    int AlterTableModify(std::string table_name, std::pair<std::string, std::string> field);
    int AlterTableConstraint(std::string table_name, Constraint* constraint);
    int AlterTableDeleteConstraint(std::string table_name, std::string constraint_name);

    int CheckUnique(std::string table_name, std::string fieldName);

};

#endif // DATABASE_H

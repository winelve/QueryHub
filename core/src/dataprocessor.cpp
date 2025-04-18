#include "dataprocessor.h"

DataProcessor::DataProcessor() {
    currentDatabase = nullptr;

    //to do
    currentUserName = "root";

    //初始读入数据
    // Read(1);
}

DataProcessor& DataProcessor::GetInstance() {
    static DataProcessor Instance;
    return Instance;
}


int DataProcessor::GetCurrentDatabase(std::string& databaseName) {
    if(currentDatabase == nullptr) return sDatabaseNotUse;
    databaseName = currentDatabase->GetDatabaseName();
    return sSuccess;
}

int DataProcessor::UpdatePointer() {

    for(auto& database : databases) {
        if(database.GetDatabaseName() == currentDatabaseName) {
            currentDatabase = &database;
            return sSuccess;
        }
    }

    std::cout<<"ERROR"<<std::endl;
    return sCurrentDatabaseError;
}

//--------DB---------

int DataProcessor::CreateDatabase(std::string databaseName){
    if(currentUserName.empty()) {
        return sUserNotLogin;
    }

    //查重
    for (auto& database : databases) {
        if (database.GetDatabaseName() == databaseName) {
            return sDatabaseExisted;
        }
    }

    //to do
    databases.push_back(Database(databaseName, currentUserName));

    return sSuccess;
}

int DataProcessor::DeleteDatabase(std::string databaseName) {
    if(currentUserName.empty()) {
        return sUserNotLogin;
    }

    for(int i = 0; i < databases.size(); ++i) {
        auto& database = databases[i];
        if(database.GetDatabaseName() == databaseName) {
            if(currentDatabase != nullptr) {
                if(currentDatabaseName == databaseName) {
                    currentDatabase = nullptr;
                    currentDatabaseName = "";
                }
            }
            currentDatabase = nullptr;
            databases.erase(databases.begin() + i);
            for (auto& database : databases) {
                if (database.GetDatabaseName() == currentDatabaseName) {
                    //若删的不是当前数据库则再还原反之currentDatabase = nullptr则表当前未选择数据库
                    currentDatabase = &database;
                    break;
                }
            }
            return sSuccess;
        }
    }

    return sDatabaseNotFound;
}

int DataProcessor::UseDatabase(std::string databaseName) {
    if(currentUserName.empty()) {
        return sUserNotLogin;
    }

    if (databaseName == "") {
        currentDatabase = nullptr;
        currentDatabaseName = "";
        return sSuccess;
    }

    for (auto& database : databases) {
        if (database.GetDatabaseName() == databaseName) {

                currentDatabase = &database;
                currentDatabaseName = databaseName;
                return sSuccess;
        }
    }
    return sDatabaseNotFound;

}

int DataProcessor::ShowDatabases(std::vector<std::string>& allDatabases) {
    if(currentUserName.empty()) {
        return sUserNotLogin;
    }

    allDatabases.clear();
    for (const auto& database : databases) {
        std::string currName = database.GetDatabaseName();
        allDatabases.push_back(currName);
        // std::cout << currName << std::endl;
    }

    return sSuccess;
}


//-------DDL----
int DataProcessor::ShowTables(std::vector<std::string>& return_tables) {
    if(currentUserName.empty()) {
        return sUserNotLogin;
    }
    if (currentDatabase == nullptr) {
        return sDatabaseNotUse;
    }

    return_tables.clear();
    int ret = currentDatabase->ShowTables(return_tables);
    // if(ret == sSuccess) {
        // for(auto it = return_tables.begin(); it != return_tables.end();) {
        //     it++;
        // }
    // }
    return ret;
}

int DataProcessor::CreateTable(std::string tableName, std::vector<std::pair<std::string, std::string>> fieldList, std::vector<Constraint *> constraints) {
    if(currentUserName.empty()) {
        return sUserNotLogin;
    }
    if (currentDatabase == nullptr) {
        return sDatabaseNotUse;
    }
    for(const auto& constraint :constraints) {
        if(constraintMap.count(constraint->GetConstraintName())) {
            return sConstraintNameExisted;
        }
    }
    int ret = currentDatabase->CreateTable(tableName, fieldList, constraints);

    if(ret == sSuccess) {
        UpdatePointer();
        UpdateConstraintMap();
        UpdatePointer();
    }
    return ret;
}

int DataProcessor::DropTable(std::string tableName) {
    if(currentUserName.empty()) {
        return sUserNotLogin;
    }
    if (currentDatabase == nullptr) {
        return sDatabaseNotUse;
    }
    int w = currentDatabase->DropTable(tableName);
    if(w == sSuccess) {
        UpdatePointer();
    }
    return w;
}

//查看表结构
int DataProcessor::DescribeTable(std::string tableName,std::vector<std::pair<std::string, std::string>>& fieldList,std::vector<Constraint*>&    constraints) {
    if(currentUserName.empty()) {
        return sUserNotLogin;
    }
    if (currentDatabase == nullptr) {
        return sDatabaseNotUse;
    }
    fieldList.clear();
    constraints.clear();
    int ret = currentDatabase->DescribeTable(tableName, fieldList, constraints);
    return ret;
}


int DataProcessor::UpdateConstraintMap() {
    constraintMap.clear();
    for(const auto& database : databases) {
        const auto & tables = database.GetTables();
        for(const auto& table :tables) {
            const auto& constraints = table.GetConstraints();
            for(auto constraint : constraints) {
                constraintMap[constraint->GetConstraintName()] = {database.GetDatabaseName(), table.GetTableName()};
            }
        }
    }
    return sSuccess;
}
int DataProcessor::AlterTableAdd(std::string tableName, std::pair<std::string, std::string> field) {
    if(currentUserName.empty()) {
        return sUserNotLogin;
    }
    if (currentDatabase == nullptr) {
        return sDatabaseNotUse;
    }

    if(currentDatabase->FindTable(tableName) != sSuccess) {
        return sTableNotFound;
    }

    int ret = currentDatabase->AlterTableAdd(tableName, field);
    UpdatePointer();
    return ret;
}
int DataProcessor::AlterTableDrop(std::string tableName, std::string fieldName) {
    if(currentUserName.empty()) {
        return sUserNotLogin;
    }
    if (currentDatabase == nullptr) {
        return sDatabaseNotUse;
    }
    if(currentDatabase->FindTable(tableName) != sSuccess) {
        return sTableNotFound;
    }
    int ret = currentDatabase->AlterTableDrop(tableName, fieldName);
    UpdatePointer();
    return ret;
}

int DataProcessor::AlterTableModify(std::string tableName, std::pair<std::string, std::string> field){
    // todo
    if(currentUserName.empty()) {
        return sUserNotLogin;
    }
    if (currentDatabase == nullptr) {
        return sDatabaseNotUse;
    }
    if(currentDatabase->FindTable(tableName) != sSuccess) {
        return sTableNotFound;
    }
    int ret = currentDatabase->AlterTableModify(tableName, field);
    UpdatePointer();
    return ret;
}
int DataProcessor::AlterTableConstraint(std::string tableName, Constraint* constraint) {
    if(currentUserName.empty()) {
        return sUserNotLogin;
    }
    if (currentDatabase == nullptr) {
        return sDatabaseNotUse;
    }
    if(currentDatabase->FindTable(tableName) != sSuccess) {
        return sTableNotFound;
    }

    //若已存在返回sConstraintNameExisted
    if(constraintMap.count(constraint->GetConstraintName())) return sConstraintNameExisted;
    //反之对该表进行加约束操作
    int ret = currentDatabase->AlterTableConstraint(tableName, constraint);
    UpdateConstraintMap();
    return ret;
}

int DataProcessor::AlterTableDeleteConstraint(std::string tableName, std::string constraintName){
    if(currentUserName.empty()) {
        return sUserNotLogin;
    }
    if (currentDatabase == nullptr) {
        return sDatabaseNotUse;
    }
    if(currentDatabase->FindTable(tableName) != sSuccess) {
        return sTableNotFound;
    }

    if(!constraintMap.count(constraintName)) return sConstraintNotFound;
    auto x = constraintMap[constraintName];
    std::string database_name = x.first;
    std::string table_name_2 = x.second;
    for(auto& database : databases) {
        if(database.GetDatabaseName() == database_name) {
            // std::cerr << database_name << " ========== " << tableName << ", " << table_name_2 << std::endl;
            int ret = database.AlterTableDeleteConstraint(tableName, constraintName);
            if(ret != sSuccess) return ret;
            break;
        }
    }
    UpdateConstraintMap();
    return sSuccess;
}
int DataProcessor::ShowConstraints(std::vector<std::vector<std::any>>& retRecords) {
    if(currentUserName.empty()) {
        return sUserNotLogin;
    }
    UpdateConstraintMap();
    std::vector<std::any> inner_record;
    //格式如下
    inner_record.push_back(std::any(std::string("Name")));
    inner_record.push_back(std::any(std::string("Type")));
    inner_record.push_back(std::any(std::string("Database")));
    inner_record.push_back(std::any(std::string("Table")));
    inner_record.push_back(std::any(std::string("Field")));

    retRecords.push_back(inner_record);
    for(const auto& database : databases) {

        const auto & tables = database.GetTables();
        for(const auto& table :tables) {
            const auto& constraints = table.GetConstraints();
            for(const auto& constraint : constraints) {
                //输出对应格式
                inner_record.clear();
                if(dynamic_cast<const ForeignReferedConstraint *>(constraint) != nullptr) {
                    //被外键依赖不算
                    continue;
                }
                //约束名
                inner_record.push_back(std::any(constraint->GetConstraintName()));
                //约束类型
                if(dynamic_cast<const ForeignKeyConstraint *>(constraint) != nullptr) {
                    inner_record.push_back(std::any(std::string("FK")));
                }
                else if(dynamic_cast<const PrimaryKeyConstraint *>(constraint) != nullptr) {
                    inner_record.push_back(std::any(std::string("PK")));
                }
                else if(dynamic_cast<const UniqueConstraint *>(constraint) != nullptr) {
                    inner_record.push_back(std::any(std::string("UNI")));
                }
                else if(dynamic_cast<const NotNullConstraint *>(constraint) != nullptr) {
                    inner_record.push_back(std::any(std::string("NN")));
                }
                else if(dynamic_cast<const DefaultConstraint *>(constraint) != nullptr) {
                    inner_record.push_back(std::any(std::string("DF")));
                }
                //数据库名
                inner_record.push_back(std::any(database.GetDatabaseName()));
                //表名
                inner_record.push_back(std::any(table.GetTableName()));
                //字段名
                inner_record.push_back(std::any(constraint->GetFieldName()));
                retRecords.push_back(inner_record);
            }
        }
    }
    return sSuccess;
}


//---------File-------
int DataProcessor::Write() {

    //清除之前的记录
    FileManager::GetInstance().ClearData();

    FileManager::GetInstance().WriteDatabasesFile(databases);

    // FileManager::GetInstance().WriteUsersFile();

    for(const auto &database : databases) {

        FileManager::GetInstance().WriteTablesFile(database.GetDatabaseName(),
                                                   database.GetTables());
    }

    return sSuccess;
}


int DataProcessor::Read(bool isPrint) {
    // FileManager::GetInstance().ReadUsersFile(users);
    FileManager::GetInstance().ReadDatabasesFile(databases);

    if(isPrint) {
        std::cout << databases.size() << std::endl;
    }

    for (auto& database : databases) {
        std::vector<Table> tables;
        FileManager::GetInstance().ReadTablesFile(database.GetDatabaseName(),
                                                  tables);
        database.SetTables(tables);

        if (isPrint) {
            std::cout << " - " << tables.size() << std::endl;
            for (const auto& table : tables) {
                std::cout << " - - " << table.GetTableName() << std::endl;

                std::cout << " - - - ";
                for (const auto& field : table.GetFieldList()) {
                    std::cout << "(" << field.first << ", " << field.second << ") ";
                }
                std::cout << std::endl;

                std::cout << " - - - ";
                for (const auto& record : table.GetRecords()) {
                    std::cout << "[";
                    for (const auto& [name, value] : record) {
                        std::cout << name << ": " << sqlTool::AnyToString(value) << " ";
                    }
                    std::cout << "]    ";
                }
                std::cout << std::endl;

                std::cout << std::endl;
            }
        }
    }

    return 0;
}

#include "dataprocessor.h"


//将字符串转为对应的权限数
const std::unordered_map<std::string,authorityNum> DataProcessor::strToAuthorityNum = {
    //表级
    {"select", SELECT},
    {"insert", INSERT},
    {"update", UPDATE},
    {"delete", DELETE},
    {"alter", ALTER},
    {"index", INDEX},
    //数据库级
    {"create", CREATE},
    {"drop", DROP}

};



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

//-----------用户操作------------

//创建用户(注册)
int DataProcessor::CreateUser(const std::string& userName,const std::string& userPassword) {
    //检查是否名字重复
    for(const auto &user : users) {
        if(user.getUserName() == userName) {
            return sUserNameExisted;
        }
    }

    users.push_back(User(userName, userPassword));

    if(currentUser != nullptr) {
        //更新当前User
        for(auto &user : users) {
            if(userName == currentUserName) {
                currentUser = &user;
                return sSuccess;
            }
        }

        return sCurrentUserError;
    }

    return sSuccess;
}
//登入
int DataProcessor::Login(const std::string& userName,const std::string& userPassword) {
    for(auto&user: users) {
        if(user.getUserName() == userName) {
            if(user.getUserPassword() == userPassword) {
                currentUserName = userName;
                currentUser = &user;
                return sSuccess;
            } else {
                return sUserPasswordError;
            }
        }

    }

    return sUserNameNotFound;

}


//-----GET--------
int DataProcessor::GetCurrentUser(std::string& userName) {
    if(currentUser == nullptr) return sUserNotLogin;
    userName = currentUser->getUserName();
    return sSuccess;
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

    //判断权限(只有root能够)
    if(!isRoot()) return sInsufficientAuthority;

    //to do
    databases.push_back(Database(databaseName, currentUserName));

    return sSuccess;
}

int DataProcessor::DeleteDatabase(std::string databaseName) {
    if(currentUserName.empty()) {
        return sUserNotLogin;
    }
    //判断当前用户类型

    for(int i = 0; i < databases.size(); ++i) {
        auto& database = databases[i];
        if(database.GetDatabaseName() == databaseName) {

            //判断权限(只有root能够)
            if(!isRoot()) return sInsufficientAuthority;


            if(currentDatabase != nullptr) {
                if(currentDatabaseName == databaseName) {
                    currentDatabase = nullptr;
                    currentDatabaseName = "";
                }
            }

            currentDatabase = nullptr;

            //删除users表对应该数据库的信息
            for(auto& user: users) {
                user.RevokeAllDatabaseAndTableAuthorities(databaseName);
            }


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

//修改数据库名
int DataProcessor::AlterDatabaseName(std::string databaseName,std::string newDatabaseName) {
    if(currentUserName.empty()) {
        return sUserNotLogin;
    }


    //查重(若存在新数据库名的数据库则失败)
    for (auto& database : databases) {
        if (database.GetDatabaseName() == newDatabaseName) {
            return sDatabaseExisted;
        }
    }

    //找到预修改的数据库
    for (auto& database : databases) {
        if (database.GetDatabaseName() == databaseName) {


            //判断权限(只有root或数据库拥有者能够)
            if(!isRoot() && !isDatabaseOwner(databaseName)) return sInsufficientAuthority;


            //开始修改

            //先更改所有User的权限信息
            for(auto& user: users) {
                user.ModifyDatabaseName(databaseName, newDatabaseName);
            }


            //若当前使用的数据库为此修改的数据库则需对应修改
            if(currentDatabaseName == databaseName) {
                currentDatabaseName = newDatabaseName;
            }

            //再更改当前
            database.SetDatabaseName(newDatabaseName);

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
int DataProcessor::ShowTables(std::vector<std::string>& returnTables) {
    if(currentUserName.empty()) {
        return sUserNotLogin;
    }
    if (currentDatabase == nullptr) {
        return sDatabaseNotUse;
    }

    returnTables.clear();
    int ret = currentDatabase->ShowTables(returnTables);
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

    //检查权限
    if(currentUser->CheckAuthority(currentDatabaseName, authorityNum::CREATE) != sSuccess) return sInsufficientAuthority;

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

    //检查权限
    if(currentUser->CheckAuthority(currentDatabaseName, authorityNum::DROP) != sSuccess) return sInsufficientAuthority;

    int w = currentDatabase->DropTable(tableName);
    if(w == sSuccess) {
        UpdatePointer();
    }
    return w;
}

//修改表名
int DataProcessor::AlterTableName(const std::string& databaseName,const std::string& tableName, const std::string& newTableName) {
    if(currentUserName.empty()) {
        return sUserNotLogin;
    }



    for(auto &database:databases) {
        if(database.GetDatabaseName() == databaseName) {

            //查重
            for(const auto& table: database.GetTables()) {
                if(table.GetTableName() == newTableName) {
                    return sTableNameExisted;
                }
            }

            //是否存在
            for(auto& table: database.GetTables()) {
                if(table.GetTableName() == tableName) {

                    //检查权限

                    //检查权限(必须得同时拥有CREATE和DROP权限才能改表名)
                    if(currentUser->CheckAuthority(currentDatabaseName, authorityNum::CREATE) != sSuccess) return sInsufficientAuthority;
                    if(currentUser->CheckAuthority(currentDatabaseName, authorityNum::DROP) != sSuccess) return sInsufficientAuthority;

                    //修改user表
                    for(auto& user: users) {
                        user.ModifyTableName(databaseName, tableName, newTableName);
                    }

                    //修改表名
                    database.SetTableName(tableName, newTableName);
                    return sSuccess;
                }


            }


            return sTableNotFound;

        }
    }

    return sDatabaseNotFound;
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

    //检查权限
    if(currentUser->CheckAuthority(currentDatabaseName, tableName, authorityNum::ALTER) != sSuccess) return sInsufficientAuthority;

    //进行Add操作
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

    //检查权限
    if(currentUser->CheckAuthority(currentDatabaseName, tableName, authorityNum::ALTER) != sSuccess) return sInsufficientAuthority;

    //进行表字段的Drop操作
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

    //检查权限
    if(currentUser->CheckAuthority(currentDatabaseName, tableName, authorityNum::ALTER) != sSuccess) return sInsufficientAuthority;

    //进行
    int ret = currentDatabase->AlterTableModify(tableName, field);
    UpdatePointer();
    return ret;
}

//修改列名
int DataProcessor::AlterTableColumnName(std::string tableName, std::string columnName, std::string newColumnName) {
    if(currentUserName.empty()) {
        return sUserNotLogin;
    }
    if (currentDatabase == nullptr) {
        return sDatabaseNotUse;
    }
    if(currentDatabase->FindTable(tableName) != sSuccess) {
        return sTableNotFound;
    }

    //检查权限
    if(currentUser->CheckAuthority(currentDatabaseName, tableName, authorityNum::ALTER) != sSuccess) return sInsufficientAuthority;

    int ret = currentDatabase->AlterTableColumnName(tableName, columnName, newColumnName);

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

    //检查权限
    if(currentUser->CheckAuthority(currentDatabaseName, tableName, authorityNum::ALTER) != sSuccess) return sInsufficientAuthority;

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

    //检查权限
    if(currentUser->CheckAuthority(currentDatabaseName, tableName, authorityNum::ALTER) != sSuccess) return sInsufficientAuthority;

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
//------------DML----------


//单表查询
int DataProcessor::Select(std::string tableName,
           std::vector<std::string> fieldName,
           std::vector<std::tuple<std::string, std::string, int>> conditions,
           std::vector<std::vector<std::any>> &returnRecords,
                          const std::vector<std::string>& orderbyKey) {

    returnRecords.clear();
    if (currentUser == nullptr) {
        return sUserNotLogin;
    }
    if (currentDatabase == nullptr) {
        return sDatabaseNotUse;
    }
    if(currentDatabase->FindTable(tableName) != sSuccess) {
        return sTableNotFound;
    }
    if(currentUser->CheckAuthority(currentDatabaseName,tableName,authorityNum::SELECT) != sSuccess) return sInsufficientAuthority;
    int ret = currentDatabase->Select(tableName, fieldName, conditions,
                                       returnRecords, orderbyKey);
    UpdatePointer();
    return ret;

}
//多表查询
int DataProcessor::Select(std::vector<std::string> tablesName,
           std::vector<std::string> fieldName,
           std::vector<std::tuple<std::string, std::string, int>> conditions,
           std::vector<std::vector<std::any>> &returnRecords,
        const std::vector<std::string>& orderbyKey) {


    returnRecords.clear();
    if (currentUser == nullptr) {
        return sUserNotLogin;
    }
    if (currentDatabase == nullptr) {
        return sDatabaseNotUse;
    }
    for(const auto &tableName : tablesName) {
        if(currentDatabase->FindTable(tableName) != sSuccess) {
            return sTableNotFound;
        }
    }

    for(const auto &tableName : tablesName) {
        if(currentUser->CheckAuthority(currentDatabaseName,tableName,authorityNum::SELECT) != sSuccess) return sInsufficientAuthority;
    }

    int ret = currentDatabase->Select(tablesName, fieldName, conditions,
                                      returnRecords, orderbyKey);
    UpdatePointer();
    return ret;
}


//插入记录 records中 pair<field, values>
int DataProcessor::Insert(const std::string& tableName,const std::vector<std::pair<std::string,std::string>>& records) {
    if(currentUserName.empty()) {
        return sUserNotLogin;
    }
    if (currentDatabase == nullptr) {
        return sDatabaseNotUse;
    }

    if(currentDatabase->FindTable(tableName) != sSuccess) {
        return sTableNotFound;
    }


    //检查权限
    if(currentUser->CheckAuthority(currentDatabaseName, tableName, authorityNum::INSERT) != sSuccess) return sInsufficientAuthority;

    int ret = currentDatabase->Insert(tableName, records);
    UpdatePointer();

    return ret;
}

//更新(修改)记录
int DataProcessor::Update(const std::string& tableName,const std::vector<std::pair<std::string, std::string>>& values, const std::vector<std::tuple<std::string, std::string, int>>& conditions) {
    if(currentUserName.empty()) {
        return sUserNotLogin;
    }
    if (currentDatabase == nullptr) {
        return sDatabaseNotUse;
    }

    if(currentDatabase->FindTable(tableName) != sSuccess) {
        return sTableNotFound;
    }

    //检查权限
    if(currentUser->CheckAuthority(currentDatabaseName, tableName, authorityNum::UPDATE) != sSuccess) return sInsufficientAuthority;

    //进行删除
    int ret = currentDatabase->Update(tableName, values,conditions);

    //过程中传了DB故需更新
    UpdatePointer();

    return ret;
}


//删除记录
int DataProcessor::Delete(const std::string& tableName,const std::vector<std::tuple<std::string, std::string, int>>& conditions) {
    if(currentUserName.empty()) {
        return sUserNotLogin;
    }
    if (currentDatabase == nullptr) {
        return sDatabaseNotUse;
    }

    if(currentDatabase->FindTable(tableName) != sSuccess) {
        return sTableNotFound;
    }

    //检查权限
    if(currentUser->CheckAuthority(currentDatabaseName, tableName, authorityNum::DELETE) != sSuccess) return sInsufficientAuthority;

    //进行删除
    int ret = currentDatabase->Delete(tableName, conditions);

    //过程中传了DB故需更新
    UpdatePointer();

    return ret;
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

    FileManager::GetInstance().WriteUsersFile(users);

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

        //恐有问题
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
                    // std::cout << record.size() << std::endl;
                    for (const auto& [name, value] : record) {
                        std::cout << name << ": " << sqlTool::AnyToString(value) << " ";
                    }
                    std::cout << "]    ";
                }
                std::cout << std::endl;

            }
        }
    }

    FileManager::GetInstance().ReadUsersFile(users);

    if(isPrint) {
        std::cout << users.size() << std::endl;
    }

    return 0;
}

//-----------权限操作--------

//判断当前用户是否为root用户(是否拥有所有权限)
bool DataProcessor::isRoot() {
    if(currentUser->getUserName() == "root") {
        return true;
    }

    return false;
}

//判断当前用户是否为数据库的拥有者
bool DataProcessor::isDatabaseOwner(const std::string& databaseName) {
    for(const auto&database :databases) {
        if(database.GetDatabaseName() == databaseName) {
            if(database.GetOwnerUser() == currentUserName) {
                return true;
            }
            return false;
        }
    }

    return false;
}

//(只有root能进行)授予用户数据库所有权(不清除之前数据库拥有者的使用权)
int DataProcessor::GrantDatabaseOwner(const std::string& databaseName,const std::string& userName) {
    if(!isRoot()) {
        return sInsufficientAuthority;
    }

    for(auto& database : databases) {
        if(database.GetDatabaseName() == databaseName) {
            database.SetOwnerUser(userName);
            break;
        }
    }

    GrantAuthority(userName, databaseName, "*", "all");

    return sSuccess;
}

//授予权限
int DataProcessor::GrantAuthority(const std::string& userName,const std::string& databaseName,const std::string& tableName,const std::string& authority) {
    //若参数有为空
    if(userName.empty() || databaseName.empty() || tableName.empty() || authority.empty()) return sValueEmpty;

    //判断用户
    if(currentUserName.empty()) return sUserNotLogin;

    //判断是否为root
    bool isRootOrNot = isRoot();

    //找到授权的用户
    for(int i = 0; i < users.size(); i++) {
        if(users[i].getUserName() == userName) {
            //若数据库参数为*则当且仅当*.*语句允许
            if(databaseName == "*") {

                if(tableName != "*") return sValueIllegal;

                //*.*时需要root身份才能进行
                if(!isRootOrNot) return sInsufficientAuthority;

                //递归
                for(const auto& database: databases) {
                    GrantAuthority(userName, database.GetDatabaseName(), "*", authority);
                }


                return sSuccess;
            }

            //若为单数据库语句

            //判断权限(需要root或该数据库的拥有者)
            if(!isRootOrNot && !isDatabaseOwner(databaseName)) return sInsufficientAuthority;

            //寻找当前数据库
            for(auto &database : databases) {
                if(database.GetDatabaseName() == databaseName) {

                    if(authority == "all") {
                        //若为全部权限

                        //数据库级权限
                        for(int j = authorityNum::TABLE_COUNT + 1; j < authorityNum::DATABASE_COUNT; j++) {
                            users[i].GrantAuthority(databaseName, static_cast<authorityNum>(j));
                        }

                        //表级权限

                        for(const auto& table: database.GetTables()){
                            if(tableName == "*") {
                                //若为全表(*)
                                for(int j = 0; j < authorityNum::TABLE_COUNT; j++) {
                                    users[i].GrantAuthority(databaseName, table.GetTableName(),static_cast<authorityNum>(j));
                                }
                            } else {
                                //若为单表
                                if(tableName == table.GetTableName()) {
                                    for(int j = 0; j < authorityNum::TABLE_COUNT; j++) {
                                        users[i].GrantAuthority(databaseName, table.GetTableName(),static_cast<authorityNum>(j));
                                    }
                                    return sSuccess;
                                }
                            }
                        }

                        return sSuccess;


                    } else {
                        //单权限

                        //寻找该权限
                        auto it = strToAuthorityNum.find(authority);
                        //若找到
                        if (it != strToAuthorityNum.end()) {
                            const authorityNum& currAuthority = it->second;

                            if(currAuthority  >  authorityNum::TABLE_COUNT) {
                                //若为数据库权限则直接加
                                users[i].GrantAuthority(databaseName, currAuthority);

                                return sSuccess;
                            } else {
                                int isSuccessOrNot = 0;
                                //反之为表级权限
                                for(const auto& table: database.GetTables()) {
                                    if(tableName == "*") {
                                        isSuccessOrNot = 1;
                                        users[i].GrantAuthority(databaseName, table.GetTableName(), currAuthority);
                                    } else {
                                        if(tableName == table.GetTableName()) {
                                            users[i].GrantAuthority(databaseName, table.GetTableName(), currAuthority);
                                            isSuccessOrNot = 1;
                                            break;
                                        }



                                    }

                                }

                                //若未找到对应表
                                if(!isSuccessOrNot)  return sTableNotFound;


                                return sSuccess;
                            }



                        }


                        return sAuthorityNotFound;
                    }

                    return sSuccess;

                }
            }

            return sDatabaseNotFound;

        }
    }

    return sUserNameNotFound;
}

//(只有root能进行)收回用户数据库所有权(不清除之前数据库拥有者的使用权)
int DataProcessor::RevokeDatabaseOwner(const std::string& databaseName) {
    if(!isRoot()) {
        return sInsufficientAuthority;
    }

    for(auto& database : databases) {
        if(database.GetDatabaseName() == databaseName) {
            database.SetOwnerUser("");
        }
    }

    return sSuccess;

}


//收回权限
int DataProcessor::RevokeAuthority(const std::string& userName,const std::string& databaseName,const std::string& tableName,const std::string& authority) {
    //若参数有为空
    if(userName.empty() || databaseName.empty() || tableName.empty() || authority.empty()) return sValueEmpty;

    //判断用户
    if(currentUserName.empty()) return sUserNotLogin;

    //判断是否为root
    bool isRootOrNot = isRoot();

    //找到收回权限的用户
    for(int i = 0; i < users.size(); i++) {
        if(users[i].getUserName() == userName) {
            //若数据库参数为*则当且仅当*.*语句允许
            if(databaseName == "*") {

                if(tableName != "*") return sValueIllegal;

                //*.*时需要root身份才能进行
                if(!isRootOrNot) return sInsufficientAuthority;

                //递归
                for(const auto& database: databases) {
                    RevokeAuthority(userName, database.GetDatabaseName(), "*", authority);
                }


                return sSuccess;
            }

            //若为单数据库语句

            //判断权限(需要root或该数据库的拥有者)
            if(!isRootOrNot && !isDatabaseOwner(databaseName)) return sInsufficientAuthority;

            //寻找当前数据库
            for(auto &database : databases) {
                if(database.GetDatabaseName() == databaseName) {

                    if(authority == "all") {
                        //若为全部权限

                        //数据库级权限
                        for(int j = authorityNum::TABLE_COUNT + 1; j < authorityNum::DATABASE_COUNT; j++) {
                            users[i].RevokeAuthority(databaseName, static_cast<authorityNum>(j));
                        }

                        //表级权限

                        for(const auto& table: database.GetTables()){
                            if(tableName == "*") {
                                //若为全表(*)
                                for(int j = 0; j < authorityNum::TABLE_COUNT; j++) {
                                    users[i].RevokeAuthority(databaseName, table.GetTableName(),static_cast<authorityNum>(j));
                                }
                            } else {
                                //若为单表
                                if(tableName == table.GetTableName()) {
                                    for(int j = 0; j < authorityNum::TABLE_COUNT; j++) {
                                        users[i].RevokeAuthority(databaseName, table.GetTableName(),static_cast<authorityNum>(j));
                                    }
                                    return sSuccess;
                                }
                            }
                        }

                        return sSuccess;


                    } else {
                        //单权限

                        //寻找该权限
                        auto it = strToAuthorityNum.find(authority);
                        //若找到
                        if (it != strToAuthorityNum.end()) {
                            const authorityNum& currAuthority = it->second;

                            if(currAuthority  >  authorityNum::TABLE_COUNT) {
                                //若为数据库权限则直接加
                                users[i].RevokeAuthority(databaseName, currAuthority);

                                return sSuccess;
                            } else {
                                int isSuccessOrNot = 0;
                                //反之为表级权限
                                for(const auto& table: database.GetTables()) {
                                    if(tableName == "*") {
                                        isSuccessOrNot = 1;
                                        users[i].RevokeAuthority(databaseName, table.GetTableName(), currAuthority);
                                    } else {
                                        if(tableName == table.GetTableName()) {
                                            users[i].RevokeAuthority(databaseName, table.GetTableName(), currAuthority);
                                            isSuccessOrNot = 1;
                                            break;
                                        }



                                    }

                                }

                                //若未找到对应表
                                if(!isSuccessOrNot)  return sTableNotFound;


                                return sSuccess;
                            }



                        }


                        return sAuthorityNotFound;
                    }

                    return sSuccess;

                }
            }

            return sDatabaseNotFound;

        }
    }

    return sUserNameNotFound;
}


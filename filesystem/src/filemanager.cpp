#include "filemanager.h"

//window

#include <assert.h>
#include <filesystem>
#include <fstream>

//创建新目录(true表成功)
bool Mkdir(const std::string& path) {
    std::error_code ec;
    return std::filesystem::create_directory(path, ec);
}

//递归移除整个目录(true表成功)
bool Rmdir(const std::string& path) {
    std::error_code ec;
    return std::filesystem::remove_all(path, ec) > 0;
}


FileManager::FileManager() {

}


//获取单例
FileManager& FileManager::GetInstance() {
    static FileManager Instance;
    return Instance;
}

//删除所有数据(供调试使用)
int FileManager::ClearData() {
    if(Rmdir("./data")) {
        return sFileClearFailed;
    }
    return sSuccess;
}

//Write

int FileManager::WriteDatabasesFile(const std::vector<Database>& databases) {
    Mkdir("./data");

    //采取截断式(若已存在则将其清空再写入)
    std::ofstream out("./data/databases.txt",std::ofstream::out | std::ofstream::trunc );


    if(out.is_open()) {
        //输出数据库列表

        /** databases.txt
         * 格式(例子):
         * 2
         * queryhub root
         * man xt
         */

        out << databases.size() << std::endl;
        for(const auto & db : databases) {
            out << db.GetDatabaseName() << " " << db.GetOwnerUser() <<  std::endl;
        }
    }

    out.close();

    return sSuccess;
}

int FileManager::WriteTablesFile(const std::string& databaseName,const std::vector<Table>& tables) {

    std::string path = "./data/" + databaseName + "/";

    Mkdir(path.c_str());


    //tables.txt(表列表)

    /** tables.txt
     * 格式(例子):
     * 2
     * student
     * teacher
     */

    std::ofstream out(path + "tables.txt",std::ofstream::out | std::ofstream::trunc );

    if (!out.is_open())
        assert(false);

    out << tables.size() << std::endl;
    for (const auto& table : tables)
    {
        out << table.GetTableName() << std::endl;
    }
    out.close();

    //遍历所有表
    for(auto & table : tables) {

        //tableName.txt(字段表)

        /** tables.txt
         * 格式(例子):
         * 2
         * sName VARCHAR(2)
         * Age INT
         */

        out.open(path + table.GetTableName() + ".txt",std::ofstream::out | std::ofstream::trunc );

        if (!out.is_open())
            assert(false);

        const std::vector<std::pair<std::string,std::string>>& fieldList = table.GetFieldList();

        out << fieldList.size() << std::endl;
        for(const auto & field : fieldList) {
            out << field.first << " " << field.second << std::endl;
        }

        out.close();
        //tableNameRecords.txt(记录表)

        /** tables.txt
         * 格式(例子):
         * 2
         * Sname xt Age 19
         * Sname cat Age 1
         */
        out.open(path + table.GetTableName() + "Records.txt",std::ofstream::out | std::ofstream::trunc );
        if (!out.is_open())
            assert(false);

        const std::vector<std::unordered_map<std::string, std::any>>& records = table.GetRecords();
        out << fieldList.size() << std::endl;
        for(const auto & record : records) {
            out << record.size() << std::endl;

            for(const auto & [name, value] : record) {
                out << name << " " << sqlTool::AnyToString(value) << " ";
            }

            out << std::endl;
        }

        out.close();

        // tableNameConstraints.txt

        /** tableNameConstraints.txt
         *  2
         *  110 hhh d 111 ddd 12 13
         */

        out.open(path + table.GetTableName() + "Constraints.txt",
                 std::ofstream::out | std::ofstream::trunc);
        if (!out.is_open())
            assert(false);

        out << table.GetConstraints().size() << std::endl;
        out << sqlTool::ConstraintsToString(table.GetConstraints())
            << std::endl;
        out.close();

    }





    return sSuccess;
}

int FileManager::WriteUsersFile(){
    return sSuccess;
}

//Read

int FileManager::ReadDatabasesFile(std::vector<Database>& databases){

    std::ifstream in("./data/databases.txt", std::ifstream::in);

    if(!in.is_open())
        assert(false);

    //清空数据库
    int n;
    in >> n;
    for(int i = 1; i <= n; i++) {

        std::string databaseName;
        std::string ownerUser;
        std::cin >> databaseName >> ownerUser;
        databases.push_back(Database(databaseName, ownerUser));
    }

    in.close();

    return sSuccess;
}

int FileManager::ReadTablesFile(const std::string& databaseName,
                                std::vector<Table>& tables) {

    std::string path = "./data/" + databaseName + "/";

    tables.clear();

    std::vector<std::string> tableNames;
    int n, m;

    // tables.txt
    std::ifstream in(path + "tables.txt", std::ifstream::in);


    if (!in.is_open())
        return -1;

    in >> n;

    for (int i = 1; i <= n; i++)
    {
        std::string tableName;
        in >> tableName;
        tableNames.push_back(tableName);
    }


    in.close();

    //遍历所有表
    for (const auto& tableName : tableNames)
    {
        std::vector<std::pair<std::string, std::string>> fieldList;
        std::unordered_map<std::string, std::string> fieldMap;
        std::vector<std::unordered_map<std::string, std::any>> records;
        std::vector<Constraint*> constraints;
        // std::vector .... indexes;

        // tableName.txt
        in.open(path + tableName + ".txt", std::ifstream::in);
        if (!in.is_open())
            return -1;

        in >> n;
        //字段
        for (int i = 1; i <= n; i++)
        {
            std::string name;
            std::string type;
            in >> name >> type;
            fieldList.push_back({name, type});
            fieldMap[name] = type;
        }

        in.close();

        // tableNameRecords.txt
        in.open(path + tableName + "Records.txt", std::ifstream::in);

        if (!in.is_open())
            return -1;

        in >> n;
        //记录
        for (int i = 1; i <= n; i++)
        {
            std::unordered_map<std::string, std::any> record;
            std::string name;
            std::string value;
            in >> m;
            for (int j = 1; j <= m; j++)
            {
                in >> name >> value;
                record[name] =
                    sqlTool::TypeAndValueToAny(fieldMap[name], value);
            }
            records.push_back(record);
        }

        in.close();


        // tableRecords.txt
        in.open(path + tableName + "Records.txt", std::ifstream::in);
        if (!in.is_open())
            return -1;

        in >> n;
        for (int i = 1; i <= n; i++)
        {
            std::unordered_map<std::string, std::any> record;
            std::string name;
            std::string value;
            in >> m;
            for (int j = 1; j <= m; j++)
            {
                in >> name >> value;
                record[name] =
                    sqlTool::TypeAndValueToAny(fieldMap[name], value);
            }
            records.push_back(record);
        }
        in.close();

        // tableNameConstraints.txt
        in.open(path + tableName + "Constraints.txt", std::ifstream::in);
        if (!in.is_open())
            return -1;

        in >> n;
        for (int i = 1; i <= n; i++)
        {
            int type;
            std::string fieldName, constraintName;
            in >> type >> constraintName >> fieldName;
            //根据type(consts类里的约束Int值来进行转换)
            if (type == sPrimaryKey)
            {
                constraints.push_back(
                    new PrimaryKeyConstraint(fieldName, constraintName));
            }
            else if (type == sForeignKey)
            {
                std::string s1, s2;
                in >> s1 >> s2;
                constraints.push_back(new ForeignKeyConstraint(
                    fieldName, constraintName, s1, s2));
            }
            else if (type == sForeignRefered)
            {
                std::string s1, s2;
                in >> s1 >> s2;
                constraints.push_back(new ForeignReferedConstraint(
                    fieldName, constraintName, s1, s2));
            }
            else if (type == sUnique)
            {
                constraints.push_back(
                    new UniqueConstraint(fieldName, constraintName));
            }
            else if (type == sNotNull)
            {
                constraints.push_back(
                    new NotNullConstraint(fieldName, constraintName));
            }
            else if (type == sDefault)
            {
                std::string value;
                in >> value;
                constraints.push_back(
                    new DefaultConstraint(fieldName, constraintName,
                                          sqlTool::TypeAndValueToAny(
                                              fieldMap.at(fieldName), value)));
            }
            else
            {
                std::cerr << "Unknown Constraint Type" << std::endl;
            }
        }
        in.close();

    }

    return sSuccess;
}

int FileManager::ReadUsersFile(){
    return sSuccess;
}

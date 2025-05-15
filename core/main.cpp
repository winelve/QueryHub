//core核心模块的main函数


#include "dataprocessor.h"
#include <windows.h>
#include <QDebug>

void readDataFromFile() {
    DataProcessor::GetInstance().Read(1);
    DataProcessor::GetInstance().Login("root", "123456");
    // 切换数据库
    DataProcessor::GetInstance().UseDatabase("school");
    // 创建表 student，字段为 name(string), age(int)，加一个唯一约束
    std::vector<std::pair<std::string, std::string>> fields = {
        {"name", "string"},
    };

    std::vector<Constraint*> constraints;
    constraints.push_back(new UniqueConstraint("name", "unique_name"));
    DataProcessor::GetInstance().CreateTable("hhh", fields, constraints);
    // 修改字段类型
    // DataProcessor::GetInstance().AlterTableAdd("xt", {"name", "string"});
    DataProcessor::GetInstance().AlterTableAdd("hhh", {"age", "int"});
    DataProcessor::GetInstance().AlterTableAdd("hhh", {"gender", "string"});
    DataProcessor::GetInstance().AlterTableDrop("hhh", "age");
    DataProcessor::GetInstance().AlterTableModify("hhh", {"gender", "int"});


    //ddd
    // DataProcessor::GetInstance().Write();
    // DataProcessor::GetInstance().Read(1);
    // 显示所有数据库
    // std::vector<std::string> allDatabases;
    // DataProcessor::GetInstance().ShowDatabases(allDatabases);
    // std::cout << "\nALLDB:\n";
    // for (const auto& db : allDatabases) {
    //     std::cout << "  " << db << std::endl;
    // }
    // allDatabases.clear();

}

void writeDataToFile() {
    DataProcessor::GetInstance().Read(1);
    DataProcessor::GetInstance().Login("root", "123456");
    //----------DDL----------
    // 创建数据库
    DataProcessor::GetInstance().CreateDatabase("school");
    DataProcessor::GetInstance().CreateDatabase("xt");
    // 切换数据库
    DataProcessor::GetInstance().UseDatabase("school");

    std::string currUsingDB;
    DataProcessor::GetInstance().GetCurrentDatabase(currUsingDB);
    std::cout << "\ncurrUsingDB:\n " << currUsingDB << std::endl;
    // 显示所有数据库
    std::vector<std::string> allDatabases;
    DataProcessor::GetInstance().ShowDatabases(allDatabases);
    std::cout << "\nALLDB:\n";
    for (const auto& db : allDatabases) {
        std::cout << "  " << db << std::endl;
    }
    allDatabases.clear();

    // === 表操作 ===

    // 查看 student 表结构
    std::vector<std::pair<std::string, std::string>> descFields;
    std::vector<Constraint*> descConstraints;
    // 查看约束信息
    std::vector<std::vector<std::any>> ret_constraints;


    // 创建表 student，字段为 name(string), age(int)，加一个唯一约束
    std::vector<std::pair<std::string, std::string>> fields = {
        {"name", "string"},
        {"age", "int"}
    };

    std::vector<Constraint*> constraints;
    constraints.push_back(new UniqueConstraint("name", "unique_name"));
    DataProcessor::GetInstance().CreateTable("student", fields, constraints);

    //查看结果
    // DataProcessor::GetInstance().Write();

    //print

    //表字段
    DataProcessor::GetInstance().DescribeTable("student", descFields, descConstraints);
    std::cout << "\nstudent TAble structure:\n";
    for (const auto& f : descFields) {
        std::cout << "  " << f.first << " : " << f.second << std::endl;
    }
    descFields.clear();

    //约束
    DataProcessor::GetInstance().ShowConstraints(ret_constraints);
    std::cout << "\n NUM OF CURR CONSTRAINT: " << (ret_constraints.size()-1) << std::endl;
    for(const auto & c : ret_constraints) {
        for(int i = 0; i < 5; i++) {
            std::cout << sqlTool::AnyToString(c[i]) << " ";
        }
        std::cout << std::endl;

    }
    ret_constraints.clear();


    // 展示所有表
    std::vector<std::string> tables;
    DataProcessor::GetInstance().ShowTables(tables);
    std::cout << " curr Table:" << std::endl;
    for (const std::string& table : tables) {
        std::cout << "  " << table << std::endl;
    }



    // 修改结构：添加一个字段
    DataProcessor::GetInstance().AlterTableAdd("student", {"zvc", "string"});
    DataProcessor::GetInstance().AlterTableAdd("student", {"gender", "string"});

    //print
    DataProcessor::GetInstance().DescribeTable("student", descFields, descConstraints);
    std::cout << "student TAble structure:" << std::endl;
    for (const auto& f : descFields) {
        std::cout << "  " << f.first << " : " << f.second << std::endl;
    }
    descFields.clear();

    // 删除字段
    DataProcessor::GetInstance().AlterTableDrop("student", "age");

    DataProcessor::GetInstance().DescribeTable("student", descFields, descConstraints);
    std::cout << "student TAble structure:" << std::endl;
    for (const auto& f : descFields) {
        std::cout << "  " << f.first << " : " << f.second << std::endl;
    }
    descFields.clear();

    // 修改字段类型
    DataProcessor::GetInstance().AlterTableModify("student", {"gender", "int"});  // 将 gender 改为 int 类型

    DataProcessor::GetInstance().DescribeTable("student", descFields, descConstraints);
    std::cout << "student TAble structure:" << std::endl;
    for (const auto& f : descFields) {
        std::cout << "  " << f.first << " : " << f.second << std::endl;
    }
    descFields.clear();

    // 添加约束
    Constraint* notnull = new NotNullConstraint("gender", "notnull_gender");
    DataProcessor::GetInstance().AlterTableConstraint("student", notnull);

    // 删除约束
    DataProcessor::GetInstance().AlterTableDeleteConstraint("student", "unique_name");

    // 查看约束信息
    DataProcessor::GetInstance().ShowConstraints(ret_constraints);
    std::cout << " NUM OF CURR CONSTRAINT: " << (ret_constraints.size()-1) << std::endl;
    for(const auto & c : ret_constraints) {
        for(int i = 0; i < 5; i++) {
            std::cout << sqlTool::AnyToString(c[i]) << " ";
        }
        std::cout << std::endl;

    }


    DataProcessor::GetInstance().ShowTables(tables);
    std::cout << "curr Table:" << std::endl;
    for (const auto& table : tables) {
        std::cout << "  " << table << std::endl;
    }
    tables.clear();

    DataProcessor::GetInstance().DescribeTable("student", descFields, descConstraints);
    std::cout << "student TAble structure:\n";
    for (const auto& f : descFields) {
        std::cout << "  " << f.first << " : " << f.second << std::endl;
    }
    descFields.clear();

    DataProcessor::GetInstance().Write();

    // 删除表
    DataProcessor::GetInstance().DropTable("student");




    DataProcessor::GetInstance().ShowTables(tables);
    std::cout << "curr TableSIZE :  " << tables.size() << std::endl;

    // 删除数据库
    // DataProcessor::GetInstance().DeleteDatabase("school");



    DataProcessor::GetInstance().ShowDatabases(allDatabases);
    std::cout << "NumOfDB: " << allDatabases.size() << std::endl;
    std::cout << "ALLDB:" << std::endl;
    for (const auto& db : allDatabases) {
        std::cout << "  " << db << std::endl;
    }

    allDatabases.clear();

    // DataProcessor::GetInstance().CreateDatabase("xt");
    // DataProcessor::GetInstance().CreateDatabase("cat");
    // std::vector<std::string> returnDB;
    // DataProcessor::GetInstance().ShowDatabases(returnDB);
    // DataProcessor::GetInstance().DeleteDatabase("xt");
    // DataProcessor::GetInstance().ShowDatabases(returnDB);
    // filemanager.WriteDatabasesFile(db);
}

void testUserAuthorityPart() {
    DataProcessor::GetInstance().Read(1);

    DataProcessor::GetInstance().CreateUser("xt", "23456");
    std::cout <<  " 1" << std::endl;

    DataProcessor::GetInstance().CreateUser("root", "123456");
    std::cout <<  " 2" << std::endl;

    DataProcessor::GetInstance().CreateUser("cat", "55555");

    std::cout <<  " 3" << std::endl;
    // DataProcessor::GetInstance().Login("root", "123456");
    std::cout <<  " 4" << std::endl;
    DataProcessor::GetInstance().GrantDatabaseOwner("school","xt");
    std::cout <<  " 5" << std::endl;
    DataProcessor::GetInstance().GrantAuthority("cat", "school", "*", "all");
    std::cout <<  " 6" << std::endl;
    DataProcessor::GetInstance().RevokeAuthority("cat", "school", "*", "select");
    std::cout <<  " 7" << std::endl;
    DataProcessor::GetInstance().RevokeAuthority("cat", "school", "*", "create");
    std::cout <<  " 8" << std::endl;
    DataProcessor::GetInstance().RevokeAuthority("cat", "school", "*", "index");
    std::cout <<  " 9" << std::endl;

    DataProcessor::GetInstance().GrantAuthority("cat", "school", "hhh", "select");
    std::cout <<  " 10" << std::endl;
    DataProcessor::GetInstance().RevokeAuthority("cat", "school", "hhh", "drop");
    std::cout <<  " 11" << std::endl;
    DataProcessor::GetInstance().GrantAuthority("cat", "school", "hhh", "create");
    std::cout <<  " 12" << std::endl;
    DataProcessor::GetInstance().AlterTableName("school","student", "teacher");
    std::cout <<  " 13" << std::endl;
    DataProcessor::GetInstance().UseDatabase("school");
    DataProcessor::GetInstance().AlterTableColumnName("hhh","name", "number");
    DataProcessor::GetInstance().Write();
    std::cout <<  " 14" << std::endl;
}

void testDML() {
    DataProcessor::GetInstance().Read(0);
    DataProcessor::GetInstance().Login("root", "123456");
    DataProcessor::GetInstance().UseDatabase("school");

    // DataProcessor::GetInstance().AlterTableColumnName("teacher","zvc", "id");

    //创建班级表
    std::vector<std::pair<std::string, std::string>> fieldsOfClass = {
        {"ClassName", "string"},
        {"ClassNo", "string"}
                                                               };
    std::vector<Constraint*> constraintsOfClass;
    constraintsOfClass.push_back(new PrimaryKeyConstraint("ClassNo","ClassNoprimary_name"));
    DataProcessor::GetInstance().CreateTable("Class", fieldsOfClass, constraintsOfClass);

    //创建学生表
    std::vector<std::pair<std::string, std::string>> fieldsOfStudent = {
        {"Sno", "string"},
        {"Sname", "string"},
        {"ClassNo", "string"}
    };
    std::vector<Constraint*> constraintsOfStudent;
    constraintsOfStudent.push_back(new PrimaryKeyConstraint("Sno", "Snoprimary_name"));
    // constraintsOfStudent.push_back();
    int ret2 = DataProcessor::GetInstance().CreateTable("student", fieldsOfStudent, constraintsOfStudent);
    printf("Create_2:%d\n", ret2);
    //加上外键
    printf("Foreign_2:%d\n", DataProcessor::GetInstance().AlterTableConstraint("student", new ForeignKeyConstraint("ClassNo", "foreign_name", "Class","ClassNo")));

    //INSERT

    std::vector<std::pair<std::string, std::string>> recordsOfClass;

    recordsOfClass.push_back(std::pair<std::string,std::string>("ClassNo", "RJ2303"));

    recordsOfClass.push_back(std::pair<std::string,std::string>("ClassName", "软件2303"));

    printf("Insert_5:%d\n", DataProcessor::GetInstance().Insert("Class", recordsOfClass));

    std::vector<std::pair<std::string, std::string>> records;
    // records.push_back(std::pair<std::string,std::string>("name", "fgc"));
    records.push_back(std::pair<std::string,std::string>("Sname", "xt"));
    // records.push_back(std::pair<std::string,std::string>("id", "1009"));
    // records.push_back(std::pair<std::string,std::string>("gender", "0"));

    printf("Insert_1:%d\n", DataProcessor::GetInstance().Insert("student", records));

    records.push_back(std::pair<std::string,std::string>("Sno", "23301082"));

    // printf("Insert_2:%d\n", DataProcessor::GetInstance().Insert("student", records));

    records.push_back(std::pair<std::string,std::string>("ClassNo", "RJ2303"));

    printf("Insert_3:%d\n", DataProcessor::GetInstance().Insert("student", records));
    // std::vector<std::pair<std::string, std::string>> records;
    // records.push_back(std::pair<std::string,std::string>("name", "fgc"));
    // records.push_back(std::pair<std::string,std::string>("name", "ALG"));
    // records.push_back(std::pair<std::string,std::string>("id", "1009"));
    // records.push_back(std::pair<std::string,std::string>("gender", "0"));



    // records.push_back(std::pair<std::string,std::string>("name", "dddd"));
    // records.push_back(std::pair<std::string,std::string>("id", "1005"));
    // records.push_back(std::pair<std::string,std::string>("gender", "0"));

    // DataProcessor::GetInstance().Insert("teacher", records);

    //UPDATE


    std::vector<std::pair<std::string, std::string>> values;
    // records.push_back(std::pair<std::string,std::string>("name", "wqy"));
    // records.push_back(std::pair<std::string,std::string>("id", "1001"));
    values.push_back(std::pair<std::string,std::string>("Sname", "wyq"));
    // values.push_back(std::pair<std::string,std::string>("Sname", "WYQ"));

    std::vector<std::tuple<std::string, std::string, int>> conditions1;
    conditions1.push_back(std::tuple<std::string, std::string, int>("Sno", "23301076", sEqualCondition));

    printf("UPDATE_1: %d\n", DataProcessor::GetInstance().Update("student", values, conditions1));
    // printf("Read_2: %d\n",DataProcessor::GetInstance().Read(0));

    // printf("Read_2: %d\n",DataProcessor::GetInstance().Read(0));
    // printf("Read_2: %d\n",DataProcessor::GetInstance().Read(0));

    //DELETE
    std::vector<std::tuple<std::string, std::string, int>> conditions;
    conditions.push_back(std::tuple<std::string, std::string, int>("Sno", "23301082", sEqualCondition));

    printf("Delete_1:%d\n", DataProcessor::GetInstance().Delete("student", conditions));
    // printf("Delete_2:%d\n", DataProcessor::GetInstance().Delete("Class", conditions));

    // int ret = DataProcessor::GetInstance().Delete("teacher",conditions);
    // printf("%d\n", ret);


    //SELECT


    // std::vector<std::tuple<std::string, std::string, int>> conditions;
    // conditions.push_back(std::tuple<std::string, std::string, int>("gender", "0", sEqualCondition));
    std::vector<std::vector<std::any>> returnRecords;
    printf("SELECT_1: %d\n",DataProcessor::GetInstance().Select(std::vector<std::string>{"student","Class"}, {"*"}, conditions, returnRecords, {"Sno"}));
    // printf("SELECT_1: %d\n",DataProcessor::GetInstance().Select("student", {"*"}, conditions, returnRecords, {"Sno"}));
    for(const auto& row: returnRecords) {
        for(const auto& record: row) {
            std::cout << sqlTool::AnyToString(record) << " ";
            // printf("%s ", (sqlTool::AnyToString(record)).c_str());
        }
        printf("\n");
    }

    std::any a = std::string("你好，世界");
    std::cout << sqlTool::AnyToString(a) << std::endl;


    printf("Index_1:%d\n",DataProcessor::GetInstance().BuildIndex("student", "Sno"));

    printf("Index_2:%d\n",DataProcessor::GetInstance().BuildIndex("student", "Sname"));

    // printf("Index_3:%d\n",DataProcessor::GetInstance().BuildIndex("student", "ClassNo"));

    printf("Index_1:%d\n",DataProcessor::GetInstance().DestroyIndex("student", "Sname"));

    printf("Index_4:%d\n",DataProcessor::GetInstance().DestroyIndex("student", "ClassNo"));

    printf("Write_1:%d\n",DataProcessor::GetInstance().Write());


}

// void testIndex() {

// }

int main() {
    //设置编码以得以输出中文
    SetConsoleOutputCP(CP_UTF8); // 需引用#include <windows.h>
    //测试数据

    // writeDataToFile();
    // readDataFromFile();

    // testUserAuthorityPart();
    testDML();

//---------B+树--------------
    // bPlusTree bPlusTree;

    // bPlusTree.insert({42, "apple", 3.14}, {"Red", 10, 0.99});
    // bPlusTree.insert({13, "banana", 2.71}, {"Yellow", 5, 0.79});
    // bPlusTree.insert({67, "cherry", 1.41}, {"Red", 20, 2.99});
    // bPlusTree.insert({23, "date", 0.58}, {"Brown", 12, 1.49});
    // bPlusTree.insert({89, "fig", 6.28}, {"Purple", 7, 3.19});
    // bPlusTree.insert({5, "grape", 4.20}, {"Purple", 30, 2.59});
    // bPlusTree.insert({55, "kiwi", 7.77}, {"Green", 15, 1.89});
    // bPlusTree.insert({76, "lemon", 8.88}, {"Yellow", 25, 0.99});
    // bPlusTree.insert({30, "mango", 9.99}, {"Orange", 8, 2.49});
    // bPlusTree.insert({14, "nectarine", 5.55}, {"Orange", 18, 1.69});
    // bPlusTree.insert({48, "orange", 3.33}, {"Orange", 22, 1.29});
    // bPlusTree.insert({90, "papaya", 2.22}, {"Orange", 11, 1.99});
    // bPlusTree.insert({2, "peach", 1.11}, {"Pink", 14, 2.09});
    // bPlusTree.insert({19, "pear", 0.99}, {"Green", 16, 1.59});
    // bPlusTree.insert({33, "pineapple", 6.66}, {"Brown", 6, 3.49});
    // bPlusTree.insert({71, "plum", 7.89}, {"Purple", 9, 2.19});
    // bPlusTree.insert({62, "raspberry", 4.56}, {"Red", 13, 2.79});
    // bPlusTree.insert({8, "strawberry", 8.23}, {"Red", 24, 2.99});
    // bPlusTree.insert({99, "tangerine", 9.01}, {"Orange", 10, 1.09});
    // bPlusTree.insert({50, "watermelon", 10.10}, {"Green", 17, 4.99});


    // std::vector<std::any> result1 = bPlusTree.search({42, "apple", 3.14});   // 存在
    // std::vector<std::any> result2 = bPlusTree.search({13, "banana", 2.71});  // 存在
    // std::vector<std::any> result3 = bPlusTree.search({76, "lemon", 8.88});   // 存在
    // std::vector<std::any> result4 = bPlusTree.search({99, "tangerine", 9.01}); // 存在
    // std::vector<std::any> result5 = bPlusTree.search({100, "unknown", 1.23});  // 不存在
    // std::vector<std::any> result6 = bPlusTree.search({1, "not_in_tree", 5.67}); // 不存在
    // std::vector<std::any> result7 = bPlusTree.search({8, "strawberry", 8.23}); // 存在
    // std::vector<std::any> result8 = bPlusTree.search({23, "date", 0.58});  // 存在
    // std::vector<std::any> result9 = bPlusTree.search({71, "plum", 7.89});  // 存在
    // std::vector<std::any> result10 = bPlusTree.search({50, "watermelon", 10.10});  // 存在

    // bPlusTree.printBPlusTree();
}


#include "database.h"
#include "constants.h"
#include <cassert>

Database::Database() {

}


Database::Database(std::string databaseName, std::string ownerUser) {
    this->databaseName = databaseName;
    this->ownerUser = ownerUser;
}

const std::string& Database::GetOwnerUser() const {
    return ownerUser;
}

const std::string& Database::GetDatabaseName() const {
    return databaseName;
}

const std::vector<Table>& Database::GetTables() const {
    return tables;
}

void Database::SetTables(const std::vector<Table>& tables) {
    this->tables = tables;
}

void Database::SetDatabaseName(const std::string databaseName) {
    this->databaseName = databaseName;
}

void Database::SetTableName(const std::string& tableName, const std::string& newTableName) {
    for(auto &table: tables) {
        if(table.GetTableName() == tableName) {
            table.SetTableName(newTableName);
            return;
        }
    }
}

void Database::SetOwnerUser(const std::string& ownerName) {
    this->ownerUser = ownerName;
}

int Database:: FindTable(std::string tableName) {
    for(const auto& table : tables) {
        if(table.GetTableName() == tableName) return sSuccess;
    }
    return sTableNotFound;
}

int Database::FindTable(std::string tableName, Table& return_table) {
    for(const auto& table : tables) {
        if(table.GetTableName() == tableName) {
            return_table = table;
            return sSuccess;
        }
    }
    return sFieldNotFound;
}


int Database::FindField(std::string tableName, std::string fieldName, std::any value){
    if(FindTable(tableName) != sSuccess) return sFieldNotFound;
    for(const auto& table: tables) {
        if(table.GetTableName() == tableName) {
            return table.FindField(fieldName, value);
        }
    }
    return sFieldNotFound;
}

//依表名找到依赖的表
Table& Database::FindTableReference(std::string tableName) {
    for(auto& table : tables) {
        if(table.GetTableName() == tableName) return table;
    }
    assert(1);
    //impossible situation
    return tables[0];
}

int Database::DescribeTable(std::string tableName,std::vector<std::pair<std::string, std::string>>& fieldList,std::vector<Constraint*>& constraints) {
    for(auto& table: tables) {
        if(table.GetTableName() == tableName) {
            return table.DescribeTable(fieldList, constraints);

        }
    }
    return sTableNotFound;
}

int Database::ShowTables(std::vector<std::string>& returnTables) {
    // std::cout << "TABLES_SIZE:" << tables.size() << std::endl;
    for(const auto& table:tables) {
        returnTables.push_back(table.GetTableName());
    }
    return sSuccess;
}

int Database::CreateTable(std::string tableName, std::vector<std::pair<std::string, std::string>> fieldList, std::vector<Constraint *> constraints) {
    for(const auto& table : tables) {
        if(table.GetTableName() == tableName) {
            return sTableNameExisted;
        }
    }

    Table table = Table(tableName, fieldList, constraints);


    //tables.push_back(table);
    int sum_pri = 0;
    //遍历约束表
    for(auto constraint : constraints) {
        int ret = 0;//FindField(table_name, constraint->GetFieldName());
        int flag = 0;
        //找到字段
        for(const auto& x:fieldList) {
            std::string field_name = x.first;
            if(field_name == constraint->GetFieldName()) {
                flag = 1;
                break;
            }
        }
        if(flag == 0) return sFieldNotFound;
        if(dynamic_cast<const PrimaryKeyConstraint *>(constraint) != nullptr) {
            //std::cout<<"sumpri = "<<sum_pri<<std::endl;
            if(sum_pri > 0)  return sPrimaryKeyExcessive;

            sum_pri++;
        }

        if(ret != sSuccess) return ret;

        if(dynamic_cast<const ForeignKeyConstraint *>(constraint) != nullptr) {

            int ret = CheckUnique(dynamic_cast<const ForeignKeyConstraint *>(constraint)->GetReferenceTableName(), dynamic_cast<const ForeignKeyConstraint *>(constraint)->GetReferenceFieldName());
            if(ret!=sSuccess) return ret;


            ForeignReferedConstraint* refered_constraint = new ForeignReferedConstraint(dynamic_cast<const ForeignKeyConstraint *>(constraint)->GetReferenceFieldName(), constraint->GetConstraintName() + "refered", tableName,constraint->GetFieldName());
            ret = FindTable(dynamic_cast<const ForeignKeyConstraint *>(constraint)->GetReferenceTableName());
            if(ret!=sSuccess) return ret;
            ret = FindField(dynamic_cast<const ForeignKeyConstraint *>(constraint)->GetReferenceTableName(), dynamic_cast<const ForeignKeyConstraint *>(constraint)->GetReferenceFieldName());
            if(ret!=sSuccess) return ret;
        }
    }

    //检查外键约束
    for(const auto& constraint : constraints) {
        if(dynamic_cast<const ForeignKeyConstraint *>(constraint) == nullptr) continue;

        //std::cout<<"AlterTableConstraint checked in database"<<std::endl;

        //std::cout<<"AlterTableConstraint checked foreignkey and adding ForeignReferedConstraint"<<std::endl;
        ForeignReferedConstraint* refered_constraint = new ForeignReferedConstraint(dynamic_cast<const ForeignKeyConstraint *>(constraint)->GetReferenceFieldName(), constraint->GetConstraintName() + "refered", tableName,constraint->GetFieldName());
        int ret = AlterTableConstraint(dynamic_cast<const ForeignKeyConstraint *>(constraint)->GetReferenceTableName(),refered_constraint);
        if(ret!=sSuccess) return ret;

    }
    tables.push_back(table);
    return sSuccess;
}


int Database::DropTable(std::string tableName) {
    for(int i = 0; i < tables.size(); ++i) {
        if(tables[i].GetTableName() == tableName) {

            //check constraint
            auto constraints = tables[i].GetConstraints();
            for(const auto& constraint : constraints) {
                if(dynamic_cast<const ForeignReferedConstraint *>(constraint) != nullptr){return sBeingRefered;}
            }
            //std::cout<<"check point 1"<<std::endl;
            //删除所有被此表参考的约束
            for(const auto& constraint : constraints) {
                if(dynamic_cast<const ForeignKeyConstraint *>(constraint) != nullptr){
                    std::string reference_table_name = dynamic_cast<const ForeignKeyConstraint *>(constraint)->GetReferenceTableName();
                    for(auto& table:tables) {
                        if(table.GetTableName() == reference_table_name) {
                            table.DropForeignReferedConstraint(tableName);
                        }
                    }
                }
            }
            tables.erase(tables.begin() + i);
            return sSuccess;
        }
    }
    return sTableNotFound;
}

int Database::AlterTableAdd(std::string tableName, std::pair<std::string, std::string> field) {
    for(auto& table:tables) {
        if(table.GetTableName() == tableName) {
            return table.AlterTableAdd(field);
        }
    }
    return sTableNotFound;
}

int Database::AlterTableDrop(std::string tableName, std::string fieldName) {
    for(auto& table:tables) {
        if(table.GetTableName() == tableName) {
            int ret= table.AlterTableDrop(fieldName, this);

            return ret;
        }
    }
    return sTableNotFound;
}

int Database::AlterTableModify(std::string tableName, std::pair<std::string, std::string> field) {
    for(auto& table :tables) {
        if(table.GetTableName() == tableName) {
            int ret = table.AlterTableModify(field);
            //std::cout<<"ret is "<<ret<<std::endl;
            return ret;
        }
    }
    return sTableNotFound;
}

int Database::AlterTableColumnName(std::string tableName, std::string columnName, std::string newColumnName) {
    for(auto & table : tables) {
        if(table.GetTableName() == tableName) {
            int ret = table.AlterTableColumnName(columnName, newColumnName);
            //若ret返回sSuccess则修改成功,还需修改对应其他与改表字段有关联的表的约束
            // if(ret == sSuccess) {
            //     for(auto &constraint : constraints) {

            //     }
            // }

            return ret;
        }
    }

    return sTableNotFound;
}

int Database::AlterTableConstraint(std::string tableName, Constraint* constraint) {
    for(auto& table:tables) {
        if(table.GetTableName() == tableName) {

            const auto& constraints = table.GetConstraints();
            const auto& fields = table.GetFieldList();
            int flag = 0;
            for(const auto& field: fields) {
                if(field.first == constraint->GetFieldName()) {
                    flag = 1;
                    break;
                }
            }
            if(flag == 0) return sFieldNotFound;
            int sumpri = 0;

            //只能有一个主键
            if(dynamic_cast<const PrimaryKeyConstraint *>(constraint) != nullptr) {
                for(const auto& other_constraint : constraints) {
                    if(dynamic_cast<const PrimaryKeyConstraint *>(other_constraint) != nullptr) {
                        sumpri++;
                        break;
                    }
                }
                if(sumpri != 0) return sPrimaryKeyExcessive;
            }


            if(dynamic_cast<const ForeignKeyConstraint *>(constraint) != nullptr) {
                //std::cout<<"AlterTableConstraint checked foreignkey and adding ForeignReferedConstraint"<<std::endl;
                const auto& records = table.GetRecords();
                for(const auto& record : records) {
                    if(!record.count(constraint->GetFieldName())) continue;
                    int ret = FindField(dynamic_cast<const ForeignKeyConstraint *>(constraint)->GetReferenceTableName(), dynamic_cast<const ForeignKeyConstraint *>(constraint)->GetReferenceFieldName(), record.at(constraint->GetFieldName()));
                    if(ret != sSuccess) return ret;
                }
                int ret = CheckUnique(dynamic_cast<const ForeignKeyConstraint *>(constraint)->GetReferenceTableName(), dynamic_cast<const ForeignKeyConstraint *>(constraint)->GetReferenceFieldName());
                if(ret!=sSuccess) return ret;
                ForeignReferedConstraint* refered_constraint = new ForeignReferedConstraint(dynamic_cast<const ForeignKeyConstraint *>(constraint)->GetReferenceFieldName(), constraint->GetConstraintName() + "refered", tableName,constraint->GetFieldName());
                ret = AlterTableConstraint(dynamic_cast<const ForeignKeyConstraint *>(constraint)->GetReferenceTableName(),refered_constraint);
                if(ret!=sSuccess) return ret;
            }

            int ret = table.AlterTableConstraint(constraint);


            return ret;
        }
    }

    return sTableNotFound;
}

int Database::AlterTableDeleteConstraint(std::string tableName, std::string constraintName) {
    for(auto& table : tables) {
        if(table.GetTableName() == tableName) {
            return table.DeleteConstraint(constraintName, this);
        }
    }
    return sTableNotFound;
}


int Database::CheckUnique(std::string tableName, std::string fieldName) {
    for(const auto& table: tables) {
        if(table.GetTableName() == tableName) {
            return table.CheckUnique(fieldName);
        }
    }
    return sTableNotFound;
}

//判断记录是否满足条件(与table的该函数一致)
int Database::CheckCondition(const std::unordered_map<std::string, std::any>& record,
                             const std::vector<std::tuple<std::string, std::string, int>>& conditions, std::unordered_map<std::string, std::string> fieldMap) {

    for(const auto& condition : conditions) {
        std::string fieldName = std::get<0>(condition);
        int expectedResult = std::get<2>(condition);
        std::any value = sqlTool::TypeAndValueToAny(fieldMap[fieldName], std::get<1>(condition));

        if(!record.count(fieldName)) {
            //若都为NULL
            if(value.type() == typeid(sqlTool::sqlNull)) {
                continue;
            }
            //record为NULL,value不为NULL
            return sConditionsNotSatisfied;
        }
        //若record为NULL, value不为NULL
        if(value.type() == typeid(sqlTool::sqlNull)) {
            return sConditionsNotSatisfied;
        }
        //比较
        int compareResult = sqlTool::CompareAny(record.at(fieldName), value);

        //根据结果与预期结果对比判断
        if(compareResult == sEqual) {
            if (expectedResult != sEqualCondition && expectedResult != sLessEqualCondition && expectedResult != sLargerEqualCondition)
                return sConditionsNotSatisfied;
        }
        if(compareResult == sLarger) {
            if(expectedResult != sLargerCondition && expectedResult != sLargerEqualCondition && expectedResult != sNotEqualCondition) {
                return sConditionsNotSatisfied;
            }
        }
        if(compareResult == sLess) {
            if(expectedResult != sLessCondition && expectedResult != sLessEqualCondition && expectedResult != sNotEqualCondition) {
                return sConditionsNotSatisfied;
            }
        }
    }
    return sSuccess;
}
//单表查询
int Database::Select(std::string tableName,
           std::vector<std::string> fieldName,
           std::vector<std::tuple<std::string, std::string, int>> conditions,
           std::vector<std::vector<std::any>> &returnRecords,
           const std::vector<std::string>& orderbyKey) {
    for(auto &table : tables) {
        if(table.GetTableName() == tableName) {
            int ret = table.Select(fieldName, conditions, returnRecords, orderbyKey);
            return ret;
        }
    }

    return sTableNotFound;
}
//多表查询
int Database::Select(std::vector<std::string> tablesName,
           std::vector<std::string> fieldsName,
           std::vector<std::tuple<std::string, std::string, int>> conditions,
           std::vector<std::vector<std::any>> &returnRecords,
           const std::vector<std::string>& orderbyKey) {

    //参数
    std::unordered_map<std::string, std::string> fieldMap;
    std::vector<std::vector<std::vector<std::any>>> tdRecords;//存储所有表的记录
    std::vector<std::tuple<std::string, std::string, int>> emptyCondition;
    std::vector<std::string> allFieldNames;
    //往其中加*然后再无条件Select获得所有字段和记录
    allFieldNames.push_back("*");

    //获取全部表的记录
    for(const auto& tableName: tablesName) {
        for(auto & table: tables) {
            if(table.GetTableName() == tableName) {
                returnRecords.clear();
                //获取记录
                table.Select(allFieldNames, emptyCondition, returnRecords);

                //获取Map
                std::unordered_map<std::string, std::string> tableFieldMap = table.GetFieldMap();
                //插入字段Map
                for(const auto& x : tableFieldMap) {
                    if(fieldMap.count(x.first) && fieldMap.at(x.first) != x.second) {
                        //若有通字段但类型不匹配的则表无法做自然连接?
                        return sDataTypeWrong;
                    }
                    fieldMap[x.first] = x.second;
                }
                //插入记录
                tdRecords.push_back(returnRecords);
            }
        }
    }
    int sz = tdRecords.size();

    returnRecords.clear();

    allFieldNames.clear();
    //筛选做自然连接
    //returnRecords第一行全是字段名
    std::vector<std::any> tmp;
    if(fieldsName[0] == "*") {
        //若为*全部
        fieldsName.clear();


        //先获取第一行字段名
        std::map<std::string, int> mp;


        for(const auto& innerRecords: tdRecords) {
            for(const auto& name : innerRecords[0]) {
                //获取其中的非重复字段插入tmp中(即自然连接)
                if(!mp.count(sqlTool::AnyToString(name)))tmp.push_back(name);
                mp[sqlTool::AnyToString(name)] = 1;
            }
        }


        //填充自然连接字段作为输出字段
        for(const auto& x : tmp) {
            fieldsName.push_back(sqlTool::AnyToString(x));
        }
    }
    else {
        //先获取第一行字段名
        std::map<std::string, int> mp;
        for(const auto& inner_records: tdRecords) {
            for(const auto& name : inner_records[0]) {
                mp[sqlTool::AnyToString(name)] = 1;
            }
        }

        //构造输出字段
        for(const auto& name: fieldsName) {
            if(!mp.count(name)) return sFieldNotFound;
            tmp.push_back(std::any(name));
        }
    }

    //tmp作为自然连接字段 作为返回记录的第一行
    returnRecords.push_back(tmp);

    //获取返回的记录
    std::vector<std::unordered_map<std::string, std::any>> records;

    //lamaba表达式获取返回记录
    const auto& getReturnRecords = [&]() {
        //dfs(遍历所有self做递归) 对选择的表进行笛卡尔积
        const auto& dfs = [&](auto&& self, int now, std::unordered_map<std::string, std::any> record) {

            if(now == sz) {
                //遍历一遍所有笛卡尔积后取得的表的记录

                //若满足条件则添加
                if(CheckCondition(record, conditions, fieldMap) == sSuccess) {
                    records.push_back(record);
                }
                return;
            }

            const auto& innerRecords = tdRecords[now];

            std::unordered_map<std::string, std::any> newRecord;
            for(int i = 1; i < innerRecords.size(); ++i) {
                const auto& innerRecord = innerRecords[i];
                newRecord = record;
                bool flag = 1;
                //遍历每个字段
                for(int j = 0; j < innerRecord.size(); ++j){
                    //若当前newRecord中有该字段且对应Value与innerRecord不相等则跳过
                    if(newRecord.count(sqlTool::AnyToString(innerRecords[0][j])) && sqlTool::CompareAny(innerRecord[j], newRecord.at(sqlTool::AnyToString(innerRecords[0][j])))!= sEqual){
                        flag = 0;
                        break;
                    }

                    //添加该字段(相等时无碍)
                    newRecord[sqlTool::AnyToString(innerRecords[0][j])] = innerRecord[j];
                }
                if(flag == 0) continue;

                //递归
                self(self, now + 1, newRecord);
            }
        };

        //进行Dfs
        std::unordered_map<std::string, std::any> initialRecord;
        dfs(dfs, 0, initialRecord);
    };

    getReturnRecords();

    //对返回记录根据key进行排序

    if(orderbyKey.size() > 0) {

        std::sort(records.begin(), records.end(), [&](std::unordered_map<std::string, std::any> x, std::unordered_map<std::string, std::any> y) {
            for(const auto& key: orderbyKey) {

                int ret = sqlTool::CompareAny(x.at(key), y.at(key));
                if(ret < 0) return true;
                else if(ret > 0) return false;
            }
            return false;
        });
    }


    for(const auto& record : records) {
        std::vector<std::any> returnRecord;
        for(const auto& fieldName: fieldsName) {
            //std::cout<<"check dfs: "<<field_name<<" ";
            if(!record.count(fieldName)) returnRecord.push_back(std::any(sqlTool::sqlNull()));
            else returnRecord.push_back(record.at(fieldName));
        }
        returnRecords.push_back(returnRecord);
    }

    return sSuccess;

    // return sTableNotFound;
}

//插入记录
int Database::Insert(const std::string& tableName,const std::vector<std::pair<std::string,std::string>>& records) {
    for(auto &table : tables) {
        if(table.GetTableName() == tableName) {
            int ret = table.Insert(records, this);
            return ret;
        }
    }

    return sTableNotFound;
}
//更新(修改)记录
int Database::Update(const std::string& tableName,const std::vector<std::pair<std::string, std::string>>& values, const std::vector<std::tuple<std::string, std::string, int>>& conditions) {
    for(auto &table : tables) {
        if(table.GetTableName() == tableName) {
            int ret = table.Update(values,conditions, this);
            return ret;
        }
    }

    return sTableNotFound;
}

int Database::Delete(const std::string& tableName,const std::vector<std::tuple<std::string, std::string, int>>& conditions) {
    for(auto &table : tables) {
        if(table.GetTableName() == tableName) {
            int ret = table.Delete(conditions, this);
            return ret;
        }

    }

    return sTableNotFound;
}


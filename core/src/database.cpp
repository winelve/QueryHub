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

int Database::FindTable(std::string tableName) {
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
Table& Database::FindTableReference(std::string table_name) {
    for(auto& table : tables) {
        if(table.GetTableName() == table_name) return table;
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

int Database::AlterTableAdd(std::string table_name, std::pair<std::string, std::string> field) {
    for(auto& table:tables) {
        if(table.GetTableName() == table_name) {
            return table.AlterTableAdd(field);
        }
    }
    return sTableNotFound;
}

int Database::AlterTableDrop(std::string table_name, std::string field_name) {
    for(auto& table:tables) {
        if(table.GetTableName() == table_name) {
            int ret= table.AlterTableDrop(field_name, this);

            return ret;
        }
    }
    return sTableNotFound;
}
int Database::AlterTableModify(std::string table_name, std::pair<std::string, std::string> field) {
    for(auto& table :tables) {
        if(table.GetTableName() == table_name) {
            int ret = table.AlterTableModify(field);
            //std::cout<<"ret is "<<ret<<std::endl;
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
            return table.AlterTableConstraint(constraint);
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
    return sFieldNotFound;
}


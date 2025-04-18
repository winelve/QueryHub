#include "database.h"
#include "table.h"

// ----基本函数----
Table::Table(const std::string &tableName,
      const std::vector<std::pair<std::string,std::string>> & fieldList,
             const std::vector<Constraint*>& constraints,
             const std::vector<std::unordered_map<std::string, std::any>>&  records) {

    this->tableName = tableName;
    this->fieldList = fieldList;
    this->records = records;
    this->constraints = constraints;
    // std::cout << "CONSTRAINTS: " << constraints.size() << std::endl;
    // std::cout << "THIS_CONSTRAINTS: " << (this->constraints).size() << std::endl;
    //建立哈希表
    for(auto& field: fieldList) {
        fieldMap[field.first] = field.second;
    }

}

//析构函数
Table::~Table() {

}

//拷贝构造函数
Table::Table(const Table& t) : tableName(t.tableName), fieldList(t.fieldList), constraints(t.constraints), records(t.records){
    for(auto& field: fieldList) {
        fieldMap[field.first] = field.second;
    }
}

//拷贝赋值函数
Table& Table::operator = (Table other) {
    swap(*this, other);
    return *this;
}

void Table::swap(Table& s1, Table& s2) {
    using std::swap;
    swap(s1.tableName, s2.tableName);
    swap(s1.fieldList, s2.fieldList);
    swap(s1.fieldMap, s2.fieldMap);
    swap(s1.constraints, s2.constraints);
    swap(s1.records, s2.records);
}

// ----Get函数----
const std::string& Table::GetTableName() const {
    return tableName;
}

const std::vector<std::pair<std::string,std::string>>& Table::GetFieldList() const {
    return fieldList;
}


const std::vector<std::unordered_map<std::string, std::any>>&  Table::GetRecords() const {
    return records;
}

const std::vector<Constraint*>& Table::GetConstraints() const {
    return constraints;
}



int Table::FindField(std::string fieldName, std::any value) const {
    if(!fieldMap.count(fieldName)) return sFieldNotFound;
    for(const auto& record : records) {
        if(!record.count(fieldName)) continue;
        if(sqlTool::CompareAny(record.at(fieldName), value) == sEqual) return sSuccess;
    }
    return sFieldValueNotFound;
}

//描述表（字段信息、约束等）
int Table::DescribeTable(std::vector<std::pair<std::string, std::string>>& fieldList,
                  std::vector<Constraint*>& constraints) {
    fieldList = this->fieldList;
    constraints = this->constraints;
    return sSuccess;
}


//检查唯一
int Table::CheckUnique(std::string fieldName) const {
    if(!fieldMap.count(fieldName)) return sFieldNotFound;
    std::map<std::string, int> mp;
    for(const auto& record : records) {
        if(!record.count(fieldName)) continue;
        if(mp.count(sqlTool::AnyToString(record.at(fieldName)))) return sUniqueRepeated;
        mp[sqlTool::AnyToString(record.at(fieldName))] = 1;
    }
    return sSuccess;
}

//检查数据类型
int Table::CheckDataType(std::string type, std::string value) {
    if(type == "int") {
        for(auto x : value) {
            if(x > '9' || x < '0') return sDataTypeWrong;
        }
    }
    else if(type == "float") {
        int sum_dot = 0;
        for (auto x : value) {
            if(x == '.') sum_dot++;
            if ((x > '9' || x < '0') && sum_dot>=2)
                return sDataTypeWrong;
        }
    }
    return sSuccess;
}

//增加字段
int Table::AlterTableAdd(std::pair<std::string, std::string> newField) {
    if(fieldMap.count(newField.first)) return sFieldExisted;
    fieldList.push_back(newField);
    fieldMap[newField.first] = newField.second;
    return sSuccess;
}

//删除字段
int Table::AlterTableDrop(std::string fieldName, Database* db) {
    if(!fieldMap.count(fieldName)) return sFieldNotFound;


    //若被其他表参考则不能
    for(const auto& constraint : constraints) {
        if(dynamic_cast<const ForeignReferedConstraint *>(constraint) != nullptr && dynamic_cast<const ForeignReferedConstraint *>(constraint)->GetFieldName()==fieldName)
            return sBeingRefered;
    }

    for(auto it = constraints.begin(); it != constraints.end();) {
        if((*it)->GetFieldName() != fieldName) {it++;continue;}
        if(dynamic_cast<const ForeignKeyConstraint *>(*it) != nullptr) {
        //若删除的是外键约束
            //delete foreignreferedconstraint
            std::string reference_table_name = dynamic_cast<const ForeignKeyConstraint *>(*it)->GetReferenceTableName();
            //找到参照的表删除其被依赖的这个记录
            db->FindTableReference(reference_table_name).DropForeignReferedConstraint(tableName, fieldName);
        }
        delete (*it);
        constraints.erase(it);

    }


    //删除记录中的相应字段
    for(auto& record : records) {
        for(auto it = record.begin(); it != record.end(); ++it) {
            if(it->first == fieldName) {
                record.erase(it);
                break;
            }
        }
    }
    //删除字段
    for(auto it = fieldMap.begin(); it != fieldMap.end(); ++it) {
        if(it->first == fieldName) {
            fieldMap.erase(it);
            break;
        }
    }

    for(int i = 0; i < fieldList.size(); ++i) {
        if(fieldList[i].first == fieldName) {
            fieldList.erase(fieldList.begin() + i);
            break;
        }
    }
    return sSuccess;
}
int Table::AlterTableModify(std::pair<std::string, std::string> field) {
    //若找不到该字段
    if(!fieldMap.count(field.first)) {
        return sFieldNotFound;
    }

    //若已存在则直接返回
    if(fieldMap[field.first] == field.second) return sSuccess;

    //遍历约束检查该字段是否可MODIFY
    for(const auto& constraint : constraints) {
        if(dynamic_cast<const ForeignKeyConstraint *>(constraint) != nullptr && dynamic_cast<const ForeignKeyConstraint *>(constraint)->GetFieldName()==field.first)
            return sConstraintForeignKeyConflict;
        if(dynamic_cast<const ForeignReferedConstraint *>(constraint) != nullptr && dynamic_cast<const ForeignReferedConstraint *>(constraint)->GetFieldName()==field.first)
            return sBeingRefered;
        if(dynamic_cast<const DefaultConstraint *>(constraint) != nullptr&& dynamic_cast<const DefaultConstraint *>(constraint)->GetFieldName()==field.first) return
                sConstraintDefaultConflict;
    }

    //遍历记录检查对应该字段的值在改后是否符合type
    for(auto& record: records) {
        if(!record.count(field.first)) {
            continue;
        }
        int ret = CheckDataType(field.second, sqlTool::AnyToString(record[field.first]));
        //std::cout<<"ret : "<<ret<<std::endl;
        if(ret != sSuccess) return ret;
    }
    //成功修改后进行相应的更改
    fieldMap[field.first] = field.second;


    for(auto& old_field : fieldList) {
        if(old_field.first == field.first) {
            old_field = field;
        }
    }

    for(auto& record: records) {
        if(!record.count(field.first)) {
            continue;
        }
        std::string tmp = sqlTool::AnyToString(record[field.first]);
        record[field.first] = sqlTool::TypeAndValueToAny(field.second, tmp);
    }
    return sSuccess;
}
int Table::AlterTableConstraint(Constraint* constraint) {

    //只能有一个外键
    if(dynamic_cast<const PrimaryKeyConstraint *>(constraint) != nullptr) {
        std::string field_name = constraint->GetFieldName();
        std::map<std::string, int> mp;
        for(const auto& record : records) {
            if(!record.count(field_name)) return sPrimaryKey;
            if(mp.count(sqlTool::AnyToString(record.at(field_name)))) return sPrimaryKeyRepeated;
            mp[sqlTool::AnyToString(record.at(field_name))] = 1;
        }

    }
    else if(dynamic_cast<const NotNullConstraint *>(constraint) != nullptr) {
        //如果有空的但设置了非空则拒绝修改
        std::string field_name = constraint->GetFieldName();
        for(const auto& record : records) {
            if(!record.count(field_name)) return sNotNullEmpty;
        }
    }
    else if(dynamic_cast<const UniqueConstraint *>(constraint) != nullptr) {
        //如果设置唯一但有值并非唯一则拒绝修改
        std::string field_name = constraint->GetFieldName();
        std::map<std::string, int> mp;
        for(const auto& record : records) {
            if(!record.count(field_name)) continue;
            if(mp.count(sqlTool::AnyToString(record.at(field_name)))) return sUniqueRepeated;
            mp[sqlTool::AnyToString(record.at(field_name))] = 1;
        }
    }

    constraints.push_back(constraint);
    return sSuccess;
}


//根据表名来删除外键约束。
int Table::DropForeignReferedConstraint(std::string table_name) {
    for(auto it = constraints.begin(); it != constraints.end();) {
        if(dynamic_cast<const ForeignReferedConstraint *>(*it) != nullptr) {
            if(dynamic_cast<const ForeignReferedConstraint *>(*it)->GetReferenceTableName() == table_name) {
                delete *it;
                constraints.erase(it);
            }
            else it++;
        }
        else it++;
    }
    return sSuccess;
}
//根据表名和字段名来删除外键约束。
int Table::DropForeignReferedConstraint(std::string tableName, std::string fieldName) {
    for(auto it = constraints.begin(); it != constraints.end();) {
        if(dynamic_cast<const ForeignReferedConstraint *>(*it) == nullptr) {
            if(dynamic_cast<const ForeignReferedConstraint *>(*it)->GetReferenceTableName() == tableName && dynamic_cast<const ForeignReferedConstraint *>(*it)->GetReferenceFieldName() == fieldName) {
                delete *it;
                constraints.erase(it);
            }
            else it++;
        }
        else it++;
    }
    return sSuccess;
}

//删除约束
int Table::DeleteConstraint(std::string constraintName, Database* db) {
    for(auto it = constraints.begin(); it != constraints.end();) {
        if((*it)->GetConstraintName() == constraintName) {
            //std::cout<<"constraint_name is "<<constraint_name;
            if(dynamic_cast<const ForeignKeyConstraint *>(*it) != nullptr) {
                //std::cout<<"deleting refered "<<dynamic_cast<const ForeignKeyConstraint *>(*it)->GetReferenceTableName()<<std::endl;
                db->AlterTableDeleteConstraint(dynamic_cast<const ForeignKeyConstraint *>(*it)->GetReferenceTableName(), constraintName + "refered");
            }
            constraints.erase(it);
            return sSuccess;
        }
        else it++;
    }
    return sConstraintNotFound;
}


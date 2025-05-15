#include "database.h"
#include <cassert>
#include "table.h"

// ----基本函数----
Table::Table(const std::string &tableName,
      const std::vector<std::pair<std::string,std::string>> & fieldList,
             const std::vector<Constraint*>& constraints,
             const std::vector<std::unordered_map<std::string, std::any>>&  records
             ,const std::vector<std::string>& indexKey) {

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

    availableRecordsNum = records.size();//初始化为record大小
    //建立记录有效表
    for(int i = 0; i < records.size(); i++) {
        ValidIndexOfRecords.push_back(1);
        // printf("ValidIndexOfRecords: %d\n", ValidIndexOfRecords.size());
    }

    for(const auto&key : indexKey) {
        BuildIndex(key);
    }
}

//析构函数
Table::~Table() {

}

//拷贝构造函数
Table::Table(const Table& t) : tableName(t.tableName), fieldList(t.fieldList), constraints(t.constraints), records(t.records), ValidIndexOfRecords(t.ValidIndexOfRecords), indexes(t.indexes), availableRecordsNum(t.availableRecordsNum){
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
    swap(s1.ValidIndexOfRecords, s2.ValidIndexOfRecords);
    swap(s1.indexes, s2.indexes);
    swap(s1.availableRecordsNum, s2.availableRecordsNum);
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

const std::vector<int>& Table::GetValidIndexOfRecords() const {
    return ValidIndexOfRecords;
}

int Table::GetIndex(std::vector<std::string>& compareKey) const {
    for(const auto &index: indexes) {
        compareKey.push_back(index.first);
    }

    return sSuccess;
}

int Table::GetRecordsSize() const {
    return records.size();
}

int Table::GetAvailableRecordsNum() const {
    return availableRecordsNum;
}
const std::vector<Constraint*>& Table::GetConstraints() const {
    return constraints;
}

const std::unordered_map<std::string, std::string>& Table::GetFieldMap() const {
    return fieldMap;
}

//SET
void Table::SetTableName(const std::string& tableName) {
    this->tableName = tableName;
}

int Table::FindField(std::string fieldName, std::any value) const {
    if(!fieldMap.count(fieldName)) return sFieldNotFound;

    //VALID写法
    for(int i = 0; i < records.size(); i++) {
        if(ValidIndexOfRecords[i] == 0 ) continue;

        if(!records[i].count(fieldName)) continue;

        if(sqlTool::CompareAny(records[i].at(fieldName), value) == sEqual) return sSuccess;

    }

    // for(const auto& record : records) {
    //     if(!record.count(fieldName)) continue;
    //     if(sqlTool::CompareAny(record.at(fieldName), value) == sEqual) return sSuccess;
    // }
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

    //VALID写法
    for(int i = 0; i < records.size(); i++) {
        if(ValidIndexOfRecords[i] == 0 ) continue;

        if(!records[i].count(fieldName)) continue;

        if(mp.count(sqlTool::AnyToString(records[i].at(fieldName)))) return sUniqueRepeated;
        mp[sqlTool::AnyToString(records[i].at(fieldName))] = 1;

    }

    // for(const auto& record : records) {
    //     if(!record.count(fieldName)) continue;
    //     if(mp.count(sqlTool::AnyToString(record.at(fieldName)))) return sUniqueRepeated;
    //     mp[sqlTool::AnyToString(record.at(fieldName))] = 1;
    // }
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


    //删除记录中的相应字段(不实际删而是修改Valid)
    for(int i = 0; i < records.size(); i++) {

        if(ValidIndexOfRecords[i] == 0) continue;

        std::unordered_map<std::string, std::any> newRecord = records[i];
        for(auto it = newRecord.begin(); it != newRecord.end(); ++it) {
            if(it->first == fieldName) {
                ValidIndexOfRecords[i] = 0;
                newRecord.erase(it);
                // record.erase(it);
                break;
            }
        }

        if(ValidIndexOfRecords[i] == 0) {
            //若有修改则将修改后的record加入records中
            records.push_back(newRecord);
            ValidIndexOfRecords.push_back(1);
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

//字段类型修改
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
    //(Valid写法)
    for(int i = 0; i < records.size(); i++) {

        if(ValidIndexOfRecords[i] == 0) continue;

        if(!records[i].count(field.first)) {
            continue;
        }
        int ret = CheckDataType(field.second, sqlTool::AnyToString(records[i][field.first]));
        //std::cout<<"ret : "<<ret<<std::endl;
        if(ret != sSuccess) return ret;

    }

    // for(auto& record: records) {
    //     if(!record.count(field.first)) {
    //         continue;
    //     }
    //     int ret = CheckDataType(field.second, sqlTool::AnyToString(record[field.first]));
    //     //std::cout<<"ret : "<<ret<<std::endl;
    //     if(ret != sSuccess) return ret;
    // }

    //成功修改后进行相应的更改
    fieldMap[field.first] = field.second;


    for(auto& oldField : fieldList) {
        if(oldField.first == field.first) {
            oldField = field;
        }
    }


    //valid写法修改记录
    for(int i = 0; i < records.size(); i++) {

        if(ValidIndexOfRecords[i] == 0) continue;

        if(!records[i].count(field.first))  continue;

        ValidIndexOfRecords[i] = 0;

        std::unordered_map<std::string, std::any> newRecord = records[i];


        std::string tmp = sqlTool::AnyToString(newRecord[field.first]);
        newRecord[field.first] = sqlTool::TypeAndValueToAny(field.second, tmp);

        //若有修改则将修改后的record加入records中
        records.push_back(newRecord);
        ValidIndexOfRecords.push_back(1);
    }


    // for(auto& record: records) {
    //     if(!record.count(field.first)) {
    //         continue;
    //     }
    //     std::string tmp = sqlTool::AnyToString(record[field.first]);
    //     record[field.first] = sqlTool::TypeAndValueToAny(field.second, tmp);
    // }
    return sSuccess;
}

//修改列名
int Table::AlterTableColumnName(std::string columnName, std::string newColumnName) {
    //若找不到该列
    if(!fieldMap.count(columnName)) {
        return sFieldNotFound;
    }

    //若已存在该修改后的新列名则直接返回
    if(fieldMap.count(newColumnName)) {
        return sFieldExisted;
    }

    //遍历约束检查该字段是否可重命名(字段需不与其他表关联)
    for(const auto& constraint : constraints) {
        //不可为外键或被其他表做外键依赖
        if(dynamic_cast<const ForeignKeyConstraint *>(constraint) != nullptr && dynamic_cast<const ForeignKeyConstraint *>(constraint)->GetFieldName()==columnName)
            return sConstraintForeignKeyConflict;
        if(dynamic_cast<const ForeignReferedConstraint *>(constraint) != nullptr && dynamic_cast<const ForeignReferedConstraint *>(constraint)->GetFieldName()==columnName)
            return sBeingRefered;
    }

    //更新表内容

    //valid写法修改记录
    for(int i = 0; i < records.size(); i++) {

        if(ValidIndexOfRecords[i] == 0) continue;


        std::unordered_map<std::string, std::any> newRecord = records[i];

        auto it = newRecord.find(columnName);

        if(it != newRecord.end()) {
        //若原纪录要改则
            ValidIndexOfRecords[i] = 0;
            std::any value = it->second;
            newRecord.erase(it);
            newRecord[newColumnName] = value;
            //若有修改则将修改后的record加入records中
            records.push_back(newRecord);
            ValidIndexOfRecords.push_back(1);
        }


    }

    //修改records
    // for(auto &record : records) {
    //     auto it = record.find(columnName);
    //     if(it != record.end()) {
    //         std::any value = it->second;
    //         record.erase(it);
    //         record[newColumnName] = value;
    //     }
    // }

    //修改fieldMap
    auto it = fieldMap.find(columnName);
    if(it != fieldMap.end()) {
        std::string type = it->second;
        fieldMap.erase(it);
        fieldMap[newColumnName] = type;
    }

    //修改fieldList
    for(auto & field : fieldList) {
        if(field.first == columnName) {
            field.first = newColumnName;
            break;
        }
    }

    //修改Constraints
    for(auto& constraint : constraints) {
        //不可为外键或被其他表做外键依赖
        if(constraint && constraint->GetFieldName() == columnName) constraint->SetFieldName(newColumnName);
    }

    return sSuccess;
}
int Table::AlterTableConstraint(Constraint* constraint) {

    //只能有一个外键
    if(dynamic_cast<const PrimaryKeyConstraint *>(constraint) != nullptr) {
        std::string field_name = constraint->GetFieldName();
        std::map<std::string, int> mp;


        //valid写法
        for(int i = 0; i < records.size(); i++) {

            if(ValidIndexOfRecords[i] == 0) continue;

            if(!records[i].count(field_name)) return sPrimaryKey;
            if(mp.count(sqlTool::AnyToString(records[i].at(field_name)))) return sPrimaryKeyRepeated;
            mp[sqlTool::AnyToString(records[i].at(field_name))] = 1;

        }

        // for(const auto& record : records) {
        //     if(!record.count(field_name)) return sPrimaryKey;
        //     if(mp.count(sqlTool::AnyToString(record.at(field_name)))) return sPrimaryKeyRepeated;
        //     mp[sqlTool::AnyToString(record.at(field_name))] = 1;
        // }

    }
    else if(dynamic_cast<const NotNullConstraint *>(constraint) != nullptr) {
        //如果有空的但设置了非空则拒绝修改
        std::string field_name = constraint->GetFieldName();
        //VALID写法
        for(int i = 0; i < records.size(); i++) {

            if(ValidIndexOfRecords[i] == 0) continue;

            if(!records[i].count(field_name)) return sNotNullEmpty;
        }
        // for(const auto& record : records) {
        //     if(!record.count(field_name)) return sNotNullEmpty;
        // }
    }
    else if(dynamic_cast<const UniqueConstraint *>(constraint) != nullptr) {
        //如果设置唯一但有值并非唯一则拒绝修改
        std::string field_name = constraint->GetFieldName();
        std::map<std::string, int> mp;

        for(int i = 0; i < records.size(); i++) {

            if(ValidIndexOfRecords[i] == 0) continue;

            if(!records[i].count(field_name)) continue;
            if(mp.count(sqlTool::AnyToString(records[i].at(field_name)))) return sUniqueRepeated;
            mp[sqlTool::AnyToString(records[i].at(field_name))] = 1;

        }

        // for(const auto& record : records) {
        //     if(!record.count(field_name)) continue;
        //     if(mp.count(sqlTool::AnyToString(record.at(field_name)))) return sUniqueRepeated;
        //     mp[sqlTool::AnyToString(record.at(field_name))] = 1;
        // }
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
//判断记录是否符合条件
int Table::CheckCondition(const std::unordered_map<std::string, std::any>& record, const std::vector<std::tuple<std::string, std::string, int>>& conditions) {
    for(const auto& condition : conditions) {
        std::string fieldName = std::get<0>(condition);
        int expectedResult = std::get<2>(condition);

        std::any value = sqlTool::TypeAndValueToAny(fieldMap[fieldName], std::get<1>(condition));


        if(!record.count(fieldName)) {
            //若当前记录无该字段的值即为NULL时

            if(value.type() == typeid(sqlTool::sqlNull)) {
                //若当前value也为NULL则跳过此判断
                continue;
            }
            //反之不为空则不满足
            return sConditionsNotSatisfied;
        }

        if(value.type() == typeid(sqlTool::sqlNull)) {
            //若record不为NULL但当前value为NULL则不满足
            return sConditionsNotSatisfied;
        }

        int compareResult = sqlTool::CompareAny(record.at(fieldName), value);

        if(compareResult == sEqual) {
            //=,<=,>=满足
            if (expectedResult != sEqualCondition && expectedResult != sLessEqualCondition && expectedResult != sLargerEqualCondition)
                return sConditionsNotSatisfied;
        }
        if(compareResult == sLarger) {
            //>, >=, !=满足
            if(expectedResult != sLargerCondition && expectedResult != sLargerEqualCondition && expectedResult != sNotEqualCondition) {
                return sConditionsNotSatisfied;
            }
        }
        if(compareResult == sLess) {
            //<, <=, !=满足
            if(expectedResult != sLessCondition && expectedResult != sLessEqualCondition && expectedResult != sNotEqualCondition) {
                return sConditionsNotSatisfied;
            }
        }
    }

    return sSuccess;
}

//查询
int Table::Select(std::vector<std::string> fieldName,
           std::vector<std::tuple<std::string, std::string, int>> conditions,
           std::vector<std::vector<std::any>> &returnRecords , const std::vector<std::string>& orderbyKey) {
    if(fieldName[0] == "*") {
        //若为*全部则清除并全部替换成该表字段
        fieldName.clear();
        for(const auto& field : fieldList) {
            fieldName.push_back(field.first);
        }
    }

    for(const auto& name: fieldName) {
        //若字段Map中缺少则返回错误
        if(!fieldMap.count(name)) {
            return sFieldNotFound;
        }
    }

    //returnRecords第一行全是字段名
    std::vector<std::any> tmp;
    for(const auto& name: fieldName) {
        tmp.push_back(std::any(name));
    }
    returnRecords.push_back(tmp);
    //ok


    bool haveGetAnswer = false;


    std::vector<int> selectedIndex;

    // 可以由索引得到选中记录
    if(!haveGetAnswer && conditions.size() > 0) {
        //找到
        for(const auto&condition : conditions) {
            std::string fieldName = std::get<0>(condition);
            std::string key = std::get<1>(condition);
            int compareResult = std::get<2>(condition);

            // printf("FieldName: %s", fieldName.c_str());
            if(indexes.count(fieldName)) {
                selectedIndex = indexes[fieldName]->search({sqlTool::AnyToString(key)},compareResult, conditions, this);
                // printf("useIndex: %d", selectedIndex.size());
                haveGetAnswer = true;

                break;
            }
        }


    }

    // 默认使用暴力方法求选中记录
    if(!haveGetAnswer) {
        for(int i = 0; i < records.size(); i++) {
            if(ValidIndexOfRecords[i] == 0) continue; //检测记录有效性
            // 检查条件
            if(CheckCondition(records[i], conditions) != sSuccess) continue;
            // 添加一条记录
            selectedIndex.push_back(i);
        }
    }

    // 根据某个KEY对记录进行排序
    if(orderbyKey.size() > 0) {
        std::sort(selectedIndex.begin(), selectedIndex.end(), [&](int x, int y) {
            for(const auto& key: orderbyKey) {
                int ret = sqlTool::CompareAny(records[x].at(key), records[y].at(key));
                if(ret < 0) return true;
                else if(ret > 0) return false;
            }
            return false;
        });
    }

    // ===== 将记录索引转换成表格 =====

    // 这个lambda表达式用于往returnRecords中加入一条记录
    auto addRecord = [&](const auto& record) {
        std::vector<std::any> retRecord;
        for(const auto& name: fieldName) {
            if(!record.count(name)) {
                retRecord.push_back(std::any(sqlTool::sqlNull()));
            } else {
                retRecord.push_back(record.at(name));
            }
        }
        returnRecords.push_back(retRecord);
    };

    //遍历排序索引, 往里添加对应索引的记录
    for(const auto& idx: selectedIndex) {
        assert(idx < records.size());
        addRecord(records[idx]);
    }

    return sSuccess;
}
//判断记录是否被引用
int Table::CheckBeingRefered(const std::unordered_map<std::string, std::any>& record, Database* db, std::string fieldName) {

    for(const auto& constraint : constraints) {

        if(dynamic_cast< ForeignReferedConstraint * >(constraint) != nullptr) {
            //若为被依赖约束

            //若该记录无该字段则跳过
            if(!record.count(constraint->GetFieldName())) continue;

            //若是查该记录的某个field是否
            if(fieldName != "" && constraint->GetFieldName() != fieldName) continue;

            std::string fieldName = constraint->GetFieldName();
            std::any value = record.at(fieldName);
            std::string referenceTableName = dynamic_cast< ForeignReferedConstraint * >(constraint)->GetReferenceTableName();
            std::string referenceFieldName = dynamic_cast< ForeignReferedConstraint * >(constraint)->GetReferenceFieldName();

            //获取依赖的表
            std::string temp;

            Table referenceTable = Table(temp);

            int ret = db->FindTable(referenceTableName, referenceTable);

            if(ret != sSuccess)  return sErrorInProcess;

            //VALID写法
            //找到依赖表中与该记录对应字段的value相等的即返回被依赖
            std::vector<std::unordered_map<std::string, std::any>> referenceTableRecords = referenceTable.GetRecords();
            std::vector<int> referenceTableValidIndexOfRecords = referenceTable.GetValidIndexOfRecords();

            for(int i = 0; i < referenceTableRecords.size(); i++) {
                if(referenceTableValidIndexOfRecords[i] == 0) continue;

                if(!referenceTableRecords[i].count(referenceFieldName)) continue;

                if(sqlTool::CompareAny(value, referenceTableRecords[i].at(referenceFieldName)) == sEqual) return sBeingRefered;
            }

            // for(const auto& record : referenceTable.GetRecords()) {


            //     if(!record.count(refereneceFieldName)) continue;

            //     if(sqlTool::CompareAny(value, record.at(refereneceFieldName)) == sEqual) return sBeingRefered;
            // }
        }
    }

    //没有被依赖
    return sSuccess;
}
//判断约束
int Table::CheckConstraint(std::unordered_map<std::string, std::any>& record, Database* db) {
    //此时记录还未插入故无需跳过自身


    for(auto constraint:constraints) {

        if (dynamic_cast<const DefaultConstraint *>(constraint) != nullptr){//默认
            if (!record.count(constraint->GetFieldName())) {
                //若无值则用默认值覆盖
                record[constraint->GetFieldName()] = dynamic_cast<const DefaultConstraint *>(constraint)->GetValue();
            }
        }

        if(dynamic_cast<const NotNullConstraint *>(constraint) != nullptr) {//非空
            //若为空则返回错误
            if(!record.count(constraint->GetFieldName())) {
                return sConstraintNotNullConflict;
            }
        }

        if (dynamic_cast<const UniqueConstraint *>(constraint) != nullptr){//唯一
            std::string fieldName = constraint->GetFieldName();
            if(record.count(fieldName)) {
                //遍历记录看是否有重复,若重复则不满足唯一约束返回错误
                for(int i = 0; i < records.size(); ++i){

                    if(ValidIndexOfRecords[i] == 0) continue;

                    auto& otherRecord = records[i];
                    if (otherRecord.count(fieldName)){
                        if (sqlTool::CompareAny(record[fieldName], otherRecord[fieldName]) == sEqual){
                            return sConstraintUniqueConflict;
                        }
                    }
                }
            }
        }

        if (dynamic_cast<const PrimaryKeyConstraint *>(constraint) != nullptr) { //主键

            //主键非空
            if(!record.count(constraint->GetFieldName())) {
                return sConstraintPrimaryKeyConflict;
            }

            std::string fieldName = constraint->GetFieldName();

            //主键唯一
            for(int i = 0; i < records.size(); ++i){

                if(ValidIndexOfRecords[i] == 0) continue;

                auto& otherRecord = records[i];

                if (otherRecord.count(fieldName)){
                    if (sqlTool::CompareAny (record[fieldName], otherRecord[fieldName]) == sEqual){
                        //std::cout<<"failed record[field_name]"<< ColasqlTool::AnyToString(record[field_name])<<std::endl;
                        return sConstraintPrimaryKeyConflict;
                    }
                }
            }
        }

        if (dynamic_cast<const ForeignKeyConstraint *>(constraint) != nullptr) { //检查外键约束
            if(!record.count(constraint->GetFieldName())) {
                continue;
            }

            std::string fieldName = constraint->GetFieldName();
            std::any currentValue = record[fieldName];
            std::string referenceTableName = dynamic_cast<const ForeignKeyConstraint *>(constraint)->GetReferenceTableName();
            std::string referenceFieldName = dynamic_cast<const ForeignKeyConstraint *>(constraint)->GetReferenceFieldName();
            std::string tmp;

            //找到依赖的表
            Table referenceTable = Table(tmp);
            int ret = db->FindTable(referenceTableName, referenceTable);

            if(ret != sSuccess) return sErrorInProcess;

            int success = 0;
            //VALID写法

            std::vector<std::unordered_map<std::string, std::any>> referenceTableRecords = referenceTable.GetRecords();
            std::vector<int> referenceTableValidIndexOfRecords = referenceTable.GetValidIndexOfRecords();

            for(int i = 0; i < referenceTableRecords.size(); i++) {
                if(referenceTableValidIndexOfRecords[i] == 0) continue;

                if(!referenceTableRecords[i].count(referenceFieldName)) continue;

                //只有找到从依赖表中找到对应依赖的值才算符合约束
                if(sqlTool::CompareAny(currentValue,referenceTableRecords[i].at(referenceFieldName)) == sEqual) {
                    success = 1;
                    break;
                }
            }



            // for(const auto& record : referenceTable.GetRecords()) {
            //     if(!record.count(referenceFieldName)) continue;

            //     //只有找到从依赖表中找到对应依赖的值才算符合约束
            //     if(sqlTool::CompareAny(currentValue,record.at(referenceFieldName)) == sEqual) {
            //         success = 1;
            //         break;
            //     }
            // }

            if(success == 0)return sConstraintForeignKeyConflict;
        }


        if (dynamic_cast<const ForeignReferedConstraint *>(constraint) != nullptr) { //检查反外键

            if(!record.count(constraint->GetFieldName())) {
                continue;
            }

            //反外键需唯一
            for(int i = 0; i < records.size(); ++i){

                if(ValidIndexOfRecords[i] == 0 ) continue;

                auto& otherRecord = records[i];
                if(otherRecord.count(constraint->GetFieldName()) && sqlTool::CompareAny(otherRecord.at(constraint->GetFieldName()), record.at(constraint->GetFieldName())) == sEqual) {
                    return sBeingRefered;
                }
            }
        }
    }
    return sSuccess;
}

//检查记录是否符合约束
int Table::CheckConstraint(std::unordered_map<std::string, std::any>& record, Database* db, std::vector<std::unordered_map<std::string, std::any> > records,std::vector<int> ValidIndexOfRecords, int currentRecordIndex) {

    for(auto constraint:constraints) {

        if (dynamic_cast<const DefaultConstraint *>(constraint) != nullptr){//默认
            if (!record.count(constraint->GetFieldName())) {
                //若无值则用默认值覆盖
                record[constraint->GetFieldName()] = dynamic_cast<const DefaultConstraint *>(constraint)->GetValue();
            }
        }

        if(dynamic_cast<const NotNullConstraint *>(constraint) != nullptr) {//非空
            //若为空则返回错误
            if(!record.count(constraint->GetFieldName())) {
                return sConstraintNotNullConflict;
            }
        }

        if (dynamic_cast<const UniqueConstraint *>(constraint) != nullptr){//唯一
            std::string fieldName = constraint->GetFieldName();
            if(record.count(fieldName)) {
                //遍历记录看是否有重复,若重复则不满足唯一约束返回错误
                for(int i = 0; i < records.size(); ++i){

                    if(ValidIndexOfRecords[i] == 0) continue;//检查valid

                    if(i == currentRecordIndex) continue;

                    auto& otherRecord = records[i];
                    if (otherRecord.count(fieldName)){
                        if (sqlTool::CompareAny(record[fieldName], otherRecord[fieldName]) == sEqual){
                            return sConstraintUniqueConflict;
                        }
                    }
                }
            }
        }

        if (dynamic_cast<const PrimaryKeyConstraint *>(constraint) != nullptr) { //主键

            //主键非空
            if(!record.count(constraint->GetFieldName())) {
                return sConstraintPrimaryKeyConflict;
            }

            std::string fieldName = constraint->GetFieldName();

            // printf("ValidSize: %d\n", ValidIndexOfRecords.size());
            // printf("wdf: %d\n", ValidIndexOfRecords[0]);
            //主键唯一
            for(int i = 0; i < records.size(); ++i){

                if(ValidIndexOfRecords[i] == 0) continue;//检查valid

                if(i == currentRecordIndex) continue;
                auto& otherRecord = records[i];

                if (otherRecord.count(fieldName)){
                    if (sqlTool::CompareAny (record[fieldName], otherRecord[fieldName]) == sEqual){
                        //std::cout<<"failed record[field_name]"<< ColasqlTool::AnyToString(record[field_name])<<std::endl;
                        return sConstraintPrimaryKeyConflict;
                    }
                }
            }
        }

        if (dynamic_cast<const ForeignKeyConstraint *>(constraint) != nullptr) { //检查外键约束
            if(!record.count(constraint->GetFieldName())) {
                continue;
            }

            std::string fieldName = constraint->GetFieldName();
            std::any currentValue = record[fieldName];
            std::string referenceTableName = dynamic_cast<const ForeignKeyConstraint *>(constraint)->GetReferenceTableName();
            std::string referenceFieldName = dynamic_cast<const ForeignKeyConstraint *>(constraint)->GetReferenceFieldName();
            std::string tmp;

            //找到依赖的表
            Table referenceTable = Table(tmp);
            int ret = db->FindTable(referenceTableName, referenceTable);

            if(ret != sSuccess) return sErrorInProcess;
            int success = 0;

            //VALID写法
            std::vector<std::unordered_map<std::string, std::any>> referenceTableRecords = referenceTable.GetRecords();
            std::vector<int> referenceTableValidIndexOfRecords = referenceTable.GetValidIndexOfRecords();

            for(int i = 0; i < referenceTableRecords.size(); i++) {
                if(referenceTableValidIndexOfRecords[i] == 0) continue;

                if(!referenceTableRecords[i].count(referenceFieldName)) continue;

                //只有找到从依赖表中找到对应依赖的值才算符合约束
                if(sqlTool::CompareAny(currentValue,referenceTableRecords[i].at(referenceFieldName)) == sEqual) {
                    success = 1;
                    break;
                }
            }

            // for(const auto& record : referenceTable.GetRecords()) {
            //     if(!record.count(referenceFieldName)) continue;

            //     //只有找到从依赖表中找到对应依赖的值才算符合约束
            //     if(sqlTool::CompareAny(currentValue,record.at(referenceFieldName)) == sEqual) {
            //         success = 1;
            //         break;
            //     }
            // }
            if(success == 0)return sConstraintForeignKeyConflict;
        }


        if (dynamic_cast<const ForeignReferedConstraint *>(constraint) != nullptr) { //检查反外键

            if(!record.count(constraint->GetFieldName())) {
                continue;
            }

            //反外键需唯一
            for(int i = 0; i < records.size(); ++i){

                if(ValidIndexOfRecords[i] == 0) continue;

                if(i == currentRecordIndex) continue;

                auto& otherRecord = records[i];
                if(otherRecord.count(constraint->GetFieldName()) && sqlTool::CompareAny(otherRecord.at(constraint->GetFieldName()), record.at(constraint->GetFieldName())) == sEqual) {
                    return sBeingRefered;
                }
            }
        }
    }
    return sSuccess;
}

//插入记录
int Table::Insert(std::vector<std::pair<std::string,std::string>> recordsOfIn, Database * db) {
    std::unordered_map<std::string, std::any> record;
    if(recordsOfIn[0].first == "*") {
        //若为*全部
        if(fieldList.size() != recordsOfIn.size()) {
            return sSizeNotProper;
        }
        //加上对应字段
        for(int i = 0; i < fieldList.size(); ++i) {
            recordsOfIn[i].first = fieldList[i].first;
        }
    }
    for(const auto& field : recordsOfIn) {
        if(!fieldMap.count(field.first)) {
            return sFieldNotFound;
        }
        if(fieldMap[field.first] == "int") {
            for(auto x : field.second) {
                if(x > '9' || x < '0') return sDataTypeWrong;
            }
            record[field.first] = std::any(std::stoi(field.second));
        }
        else if(fieldMap[field.first] == "float") {
            int sum_dot = 0;
            for (auto x : field.second) {
                if(x == '.') sum_dot++;
                if ((x > '9' || x < '0') && sum_dot>=2)
                    return sDataTypeWrong;
            }
            record[field.first] = std::any(std::stof(field.second));
        }
        else if (fieldMap[field.first] == "string") {
            record[field.first] = std::any(field.second);
        }
    }
    int ret = CheckConstraint(record, db);
    if(ret != sSuccess) return ret;

    records.push_back(record);
    ValidIndexOfRecords.push_back(1);//增加valid
    availableRecordsNum++;
    //更新索引
    for(auto & [fieldName, bPlusTree] : indexes) {
        bPlusTree->insert({record[fieldName]}, std::pair<int, std::unordered_map<std::string, std::any>>(records.size()-1, record));
        bPlusTree->isValidOfRecord[records.size()-1] = 1;
    }

    return sSuccess;
}
int Table::ApplyFieldUpdate(std::vector<std::unordered_map<std::string, std::any>>& records,std::vector<int>& ValidIndexOfRecords, const std::vector<std::pair<std::string, std::string>>& values,const std::vector<std::tuple<std::string, std::string, int>>& conditions, Database *db, int checkConstraintsOrNot) {

    //lambada函数
    auto modifyRecord = [&](int i, const std::vector<std::pair<std::string, std::string>>& values, bool checkConstraintsOrNot) -> int {
        auto newRecord = records[i];

        for(const auto& field : values) {
            if(CheckBeingRefered(newRecord, db, field.first) == sBeingRefered)
                return sBeingRefered;

            if(field.second == "") {
                if(!newRecord.count(field.first)) continue;
                newRecord.erase(field.first);
            }
            else if(fieldMap[field.first] == "int") {
                for(auto x : field.second) {
                    if(x < '0' || x > '9') return sDataTypeWrong;
                }
                newRecord[field.first] = std::any(std::stoi(field.second));
            }
            else if(fieldMap[field.first] == "float") {
                int sum_dot = 0;
                for (auto x : field.second) {
                    if(x == '.') sum_dot++;
                    else if(x < '0' || x > '9') return sDataTypeWrong;
                    if(sum_dot > 1) return sDataTypeWrong;
                }
                newRecord[field.first] = std::any(std::stof(field.second));
            }
            else if(fieldMap[field.first] == "string") {
                newRecord[field.first] = std::any(field.second);
            }
        }

        ValidIndexOfRecords[i] = 0;
        records.push_back(newRecord);
        ValidIndexOfRecords.push_back(1);

        if(!checkConstraintsOrNot) {
            for(auto& [fieldName, bPlusTree] : indexes) {
                bPlusTree->isValidOfRecord[i] = 0;
                bPlusTree->insert({newRecord[fieldName]}, std::pair<int, std::unordered_map<std::string, std::any>>(records.size()-1, newRecord));
                bPlusTree->isValidOfRecord[records.size()-1] = 1;
            }
        }

        if(checkConstraintsOrNot) {
            int ret = CheckConstraint(newRecord, db, records, ValidIndexOfRecords, records.size()-1);
            if(ret != sSuccess) return ret;
        }

        return sSuccess;
    };

    int haveGetAnswer = 0;
    //若可用索引
    if(!haveGetAnswer && conditions.size() > 0) {
        //找到
        std::vector<int> selectedIndex;
        for(const auto&condition : conditions) {
            std::string fieldName = std::get<0>(condition);
            std::string key = std::get<1>(condition);
            int compareResult = std::get<2>(condition);

            // printf("FieldName: %s\n", fieldName.c_str());
            if(indexes.count(fieldName)) {
                selectedIndex = indexes[fieldName]->search({sqlTool::AnyToString(key)},compareResult, conditions, this);
                // printf("useIndexUPDATE: %d\n", selectedIndex.size());
                haveGetAnswer = true;

                break;
            }
        }

        if(haveGetAnswer) {
            for(auto i : selectedIndex) {
                // printf("%d\n", i);
                int ret = modifyRecord(i, values, checkConstraintsOrNot);
                if(ret != sSuccess) return ret;
            }

        }
    }

    //暴力法
    if(!haveGetAnswer) {
        //记录索引用来快速排除本身
        int size = records.size();
        for(int j = 0; j < size; j++) {
            // printf("%d\n", j);
            if(ValidIndexOfRecords[j] == 0) {
                continue;
            }

            // printf("WC\n");
            if(CheckCondition(records[j],conditions) != sSuccess) {
                // printf("WDF\n");
                continue;
            }


            int ret = modifyRecord(j, values, checkConstraintsOrNot);
            if(ret != sSuccess) return ret;
        }
    }


    return sSuccess;
}
//更新(修改)记录
int Table::Update(const std::vector<std::pair<std::string, std::string>>& values,const std::vector<std::tuple<std::string, std::string, int>>& conditions, Database* db) {
    for(const auto & condition : conditions) {
        //判断条件对应字段是否都有
        if(!fieldMap.count(std::get<0>(condition))) {
            return sFieldNotFound;
        }
    }


    for(const auto& changeField : values) {
        //判断字段是否都有
        if(!fieldMap.count(changeField.first)) {
            return sFieldNotFound;
        }
    }

    for(const auto& value : values) {
        //判断修改后的value类型是否符合
        int ret = CheckDataType(fieldMap[value.first], value.second);
        if(ret != sSuccess) return ret;
    }

    //先测试一次来判断是否能进行该操作后再正式操作数据
    auto testRecords = records;
    auto testValidIndex = ValidIndexOfRecords;
    //记录索引用来快速排除本身
    int ret = ApplyFieldUpdate(testRecords,testValidIndex,values,conditions, db, 1);

    //只有测试成功才能对正式数据操作
    if(ret != sSuccess) return ret;

    //对真实值进行修改
    ApplyFieldUpdate(records, ValidIndexOfRecords ,values, conditions, db, 0);


    return sSuccess;
}

//删除记录
int Table::Delete(const std::vector<std::tuple<std::string, std::string, int>>& conditions, Database* db) {
    for(const auto & condition : conditions) {
        if(!fieldMap.count(std::get<0>(condition))) {
            return sFieldNotFound;
        }
    }

    int haveGetAnswer = 0;

    //索引法
    if(!haveGetAnswer && conditions.size() > 0) {
        std::vector<int> selectedIndex;
        for(const auto&condition : conditions) {
            std::string fieldName = std::get<0>(condition);
            std::string key = std::get<1>(condition);
            int compareResult = std::get<2>(condition);

            // printf("FieldName: %s", fieldName.c_str());
            if(indexes.count(fieldName)) {
                selectedIndex = indexes[fieldName]->search({sqlTool::AnyToString(key)},compareResult, conditions, this);
                // printf("useIndexDelete: %d", selectedIndex.size());
                haveGetAnswer = true;

                break;
            }
        }


        if(haveGetAnswer) {
            for(auto i : selectedIndex) {
                std::unordered_map<std::string, std::any>& record = records[i];

                //若满足条件且无被依赖则可删
                if(CheckCondition(record, conditions) == sSuccess) {
                    if(CheckBeingRefered(record,db) == sBeingRefered) return sBeingRefered;
                    // indexOfDeletings.push_back(i);
                    ValidIndexOfRecords[i] = 0;
                    availableRecordsNum--;
                    //更新索引
                    for(auto & [fieldName, bPlusTree] : indexes) {
                        bPlusTree->isValidOfRecord[i] = 0;
                    }
                }
            }
        }
    }

    //暴力法
    if(!haveGetAnswer) {
        //遍历记录(VALID修改替代删除)
        for(int i = 0; i < records.size(); ++i) {
            if(ValidIndexOfRecords[i] == 0) continue;
            std::unordered_map<std::string, std::any>& record = records[i];

            //若满足条件且无被依赖则可删
            if(CheckCondition(record, conditions) == sSuccess) {
                if(CheckBeingRefered(record,db) == sBeingRefered) return sBeingRefered;
                // indexOfDeletings.push_back(i);
                ValidIndexOfRecords[i] = 0;
                availableRecordsNum--;
                //更新索引
                for(auto & [fieldName, bPlusTree] : indexes) {
                    bPlusTree->isValidOfRecord[i] = 0;
                }
            }
        }
    }

    //反转(使Index从大到小从而保证删除时能正确erase)
    // reverse(indexOfDeletings.begin(), indexOfDeletings.end());
    // for(const auto& x : indexOfDeletings) {
    //     records.erase(records.begin() + x);
    // }
    return sSuccess;
}
//---------Index------

int Table::BuildIndex(const std::string& fieldName) {
    if(!fieldMap.count(fieldName)) return sFieldNotFound;

    // printf("is :%d", indexes.count(fieldName));
    // printf("size : %d\n", indexes.size());
    if(indexes.count(fieldName)) return sIndexExisted;

    auto bpt = std::make_shared<bPlusTree>();

    for(int i = 0; i < records.size(); i++) {
        if(ValidIndexOfRecords[i] == 0) continue;

        bpt->insert({records[i].at(fieldName)},std::pair<int, std::unordered_map<std::string, std::any>>(i, records[i]));
        bpt->isValidOfRecord[i] = 1;
    }

    indexes[fieldName] = bpt;

    // indexes[fieldName]->printBPlusTree();
    return sSuccess;
}

int Table::DestroyIndex(const std::string& fieldName) {
    if(!fieldMap.count(fieldName)) return sFieldNotFound;

    if(!indexes.count(fieldName)) return sIndexNotFound;

    auto it = indexes.find(fieldName);

    indexes.erase(it);

    return sSuccess;
}


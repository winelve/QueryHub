#ifndef __FOREIGN_KEY_CONSTRAINT_H__
#define __FOREIGN_KEY_CONSTRAINT_H__

#include <string>
#include <any>

#include "constraint.h"

// 外键约束: 字段名,约束名,依赖表名, 依赖字段名
class ForeignKeyConstraint: public Constraint {

public:
    ForeignKeyConstraint(const std::string& fieldName,
                         const std::string& constraintName, 
                         const std::string& referenceTableName,
                         const std::string& referenceFieldName);

    std::string GetReferenceTableName() const;

    void SetReferenceTableName(const std::string& referenceTableName);
    
    std::string GetReferenceFieldName() const;

    void SetReferenceFieldName(const std::string& referenceFieldName);

private:
    std::string _referenceTableName;
    std::string _referenceFieldName;

};

#endif // __FOREIGN_KEY_CONSTRAINT_H__

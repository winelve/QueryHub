#ifndef __FOREIGN_REFERED_CONSTRAINT_H__
#define __FOREIGN_REFERED_CONSTRAINT_H__

#include <string>
#include <any>

#include "constraint.h"

// 被外键引用约束：被引用的表名,对应的字段名,依赖表名, 依赖字段名
class ForeignReferedConstraint: public Constraint {

public:
    ForeignReferedConstraint(const std::string& fieldName,
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

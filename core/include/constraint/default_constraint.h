#ifndef __DEFAULT_CONSTRAINT_H__
#define __DEFAULT_CONSTRAINT_H__

#include <string>
#include <any>

#include "constraint.h"

// 默认: 默认值
class DefaultConstraint: public Constraint {

public:
    DefaultConstraint(const std::string& fieldName, const std::string& constraintName, std::any value);

    std::any GetValue() const;

    void SetValue(std::any value);

private:
    std::any _value;

};

#endif // __DEFAULT_CONSTRAINT_H__

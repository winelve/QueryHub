#ifndef SQLTOOL_H
#define SQLTOOL_H
/**
 * 工具类： 提供一些通用的处理
 */

#include <iostream>
#include <string>
#include <any>
#include <vector>
#include <all_constraints.h>
#include "constants.h"
class sqlTool {
public:

    //any转为string
    static std::string AnyToString(const std::any&);

    //type+value转any
    static std::any TypeAndValueToAny(const std::string& type, const std::string& value);

    //vector<constraint>转string
    static std::string ConstraintsToString(const std::vector<Constraint*>& constraints);

    //比较any
    static int CompareAny(const std::any& any1, const std::any& any2);

    //用于异常时返回易于debug
    class sqlException {

    };
    //用于表数据库中的NULL
    class sqlNull{

    };
};

#endif // SQLTOOL_H

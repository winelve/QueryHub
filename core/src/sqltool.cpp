#include "sqltool.h"


//any转string
std::string sqlTool::AnyToString(const std::any& a) {
    if (a.type() == typeid(int)) return std::to_string(std::any_cast<int>(a));
    if (a.type() == typeid(double)) return std::to_string(std::any_cast<double>(a));
    if(a.type() == typeid(float)) return std::to_string(std::any_cast<float>(a));
    if (a.type() == typeid(std::string)) return std::any_cast<std::string>(a);
    if (a.type() == typeid(const char*)) return std::string(std::any_cast<const char*>(a));
    if (a.type() == typeid(sqlNull)) return "NULL";


    return "UNKNOWNTYPE";
}


//type+value转any
std::any sqlTool::TypeAndValueToAny(const std::string& type, const std::string& value) {
    std::any res;

    if(value == "") return sqlNull();
    else if(type == "int") res = std::stoi(value);
    else if(type == "float") res = std::stof(value);
    else if(type == "double") res = std::stod(value);
    else if(type == "string") res = value;
    else res = sqlException();

    return res;
}

int sqlTool::CompareAny(const std::any& any1, const std::any& any2) {
    if(any1.type() != any2.type()) {
        return sNotSameType;
    }

    //比int
    if (any1.type() == typeid(int)) {
        int val1 = std::any_cast<int>(any1);
        int val2 = std::any_cast<int>(any2);
        if(val1 == val2) return sEqual;
        if(val1 < val2) return sLess;
        if(val1 > val2) return sLarger;
    }
    //比float
    if (any1.type() == typeid(float)) {
        float val1 = std::any_cast<float>(any1);
        float val2 = std::any_cast<float>(any2);
        if(std::fabs(val1 - val2) <= 1e-6) return sEqual;
        if(val1 < val2) return sLess;
        if(val1 > val2) return sLarger;
    }

    //比string
    if (any1.type() == typeid(std::string)) {

        std::string val1 = std::any_cast<std::string>(any1);
        std::string val2 = std::any_cast<std::string>(any2);
        if (val1 == val2) return sEqual;
        if(val1 < val2) return sLess;
        if(val1 > val2) return sLarger;
    }
    return sEqual;
}



// vector<constraint>转字符串（用于文件输出）
std::string sqlTool::ConstraintsToString(const std::vector<Constraint*>& constraints) {
    std::string str = "";

    bool DEBUG = false;

    str += std::to_string(constraints.size()) + "\n";
    for(auto &it: constraints) {
        if(DEBUG) std::cout << "===1.1" << std::endl;

        if(dynamic_cast<const PrimaryKeyConstraint*>(it) != nullptr) {
            PrimaryKeyConstraint* p = dynamic_cast<PrimaryKeyConstraint*>(it);

            if(DEBUG) std::cout  << "===1.1.1" << std::endl;

            str += std::to_string(sPrimaryKey) + " "
                   + p->GetConstraintName() + " "
                   + p->GetFieldName() + "\n";
        } else if(dynamic_cast<ForeignKeyConstraint*>(it) != nullptr) {
            ForeignKeyConstraint* p = dynamic_cast<ForeignKeyConstraint*>(it);

            if(DEBUG) std::cout  << "===1.1.2" << std::endl;

            str += std::to_string(sForeignKey) + " "
                   + p->GetConstraintName() + " "
                   + p->GetFieldName() + " "
                   + p->GetReferenceTableName() + " "
                   + p->GetReferenceFieldName() + "\n";

        } else if(dynamic_cast<ForeignReferedConstraint*>(it) != nullptr) {
            ForeignReferedConstraint* p = dynamic_cast<ForeignReferedConstraint*>(it);


            if(DEBUG) std::cout  << "===1.1.3" << std::endl;

            str += std::to_string(sForeignRefered) + " "
                   + p->GetConstraintName() + " "
                   + p->GetFieldName() + " "
                   + p->GetReferenceTableName() + " "
                   + p->GetReferenceFieldName() + "\n";
        } else if(dynamic_cast<NotNullConstraint*>(it) != nullptr) {
            NotNullConstraint* p = dynamic_cast<NotNullConstraint*>(it);

            if(DEBUG) std::cout  << "===1.1.4" << std::endl;

            str += std::to_string(sNotNull) + " "
                   + p->GetConstraintName() + " "
                   + p->GetFieldName() + "\n";

        } else if(dynamic_cast<UniqueConstraint*>(it) != nullptr) {
            UniqueConstraint* p = dynamic_cast<UniqueConstraint*>(it);

            if(DEBUG) std::cout  << "===1.1.5" << std::endl;

            str += std::to_string(sUnique) + " "
                   + p->GetConstraintName() + " "
                   + p->GetFieldName() + "\n";

        } else if(dynamic_cast<DefaultConstraint*>(it) != nullptr) {
            DefaultConstraint* p = dynamic_cast<DefaultConstraint*>(it);

            if(DEBUG) std::cout  << "===1.1.6" << std::endl;

            str += std::to_string(sDefault) + " "
                   + p->GetConstraintName() + " "
                   + p->GetFieldName() + " ";
            if(p->GetValue().type() == typeid(int))
                str += std::to_string(std::any_cast<int>(p->GetValue())) + "\n";
            else if(p->GetValue().type() == typeid(float))
                str += std::to_string(std::any_cast<float>(p->GetValue())) + "\n";
            else if(p->GetValue().type() == typeid(std::string))
                str += std::any_cast<std::string>(p->GetValue()) + "\n";
            else
                str += "0\n";

        } else {
            // error
            std::cout << "Unknown Constraint Type" << std::endl;
        }
    }
    return str;
}

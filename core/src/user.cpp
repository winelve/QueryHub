#include "user.h"


User::User(const std::string& userName,const std::string& userPassword,const std::vector<Privilege>& authorities) {
    this->userName = userName;
    this->userPassword = userPassword;
    this->authorities = authorities;
}

User::User(const std::string& userName,const std::string& userPassword) {
    this->userName = userName;
    this->userPassword = userPassword;
}


/***登入相关***/

//登入验证
int User::Identify(std::string password) {
    if(password == userPassword) {
        return sSuccess;
    }

    return sIdentifiedFail;
}


/***权限信息修改***/

int User::ModifyDatabaseName(const std::string& databaseName,const std::string& newDatabaseName) {
    for(auto& authority: authorities) {
        if(authority.databaseName == databaseName) {
            authority.databaseName = newDatabaseName;
        }
    }

    return sSuccess;
}

int User::ModifyTableName(const std::string& databaseName,const std::string& tableName,const std::string& newTableName) {
    for(auto& authority: authorities) {
        if(authority.databaseName == databaseName && authority.tableName == tableName) {
            authority.tableName = newTableName;
        }
    }

    return sSuccess;
}


/***权限查询***/

//查询数据库级特定权限
int User::CheckAuthority(const std::string& databaseName,const authorityNum& number) const {
    if(userName == "root") {
        return sSuccess;
    }

    for(const auto & authority: authorities) {
        if(authority.databaseName == databaseName && authority.number == number) {
            return sSuccess;
        }
    }

    return sInsufficientAuthority;
}
//查询表级特定权限
int User::CheckAuthority(const std::string& databaseName,const std::string& tableName,const authorityNum& number) const {
    if(userName == "root") {
        return sSuccess;
    }

    for(const auto & authority: authorities) {
        if(authority.databaseName == databaseName && authority.tableName == tableName && authority.number == number) {
            return sSuccess;
        }
    }

    return sInsufficientAuthority;
}
//查询用户是否有该数据库的权限
int User::CheckDatabaseInAuthorities(const std::string& databaseName) const {
    if(userName == "root") {
        return sSuccess;
    }

    for(const auto & authority: authorities) {
        if(authority.databaseName == databaseName) {
            return sSuccess;
        }
    }

    return sInsufficientAuthority;
}
//查询用户是否有该表的权限
int User::CheckTableInAuthorities(const std::string& databaseName,const std::string& tableName) const {
    if(userName == "root") {
        return sSuccess;
    }

    for(const auto & authority: authorities) {
        if(authority.databaseName == databaseName && authority.tableName == tableName) {
            return sSuccess;
        }
    }

    return sInsufficientAuthority;
}

/***权限赋予***/


//授予数据库级对应权限
int User::GrantAuthority(const std::string& databaseName,const authorityNum& number) {
    if(CheckAuthority(databaseName, number) == sSuccess) return sAuthorityExisted;

    Privilege authority;
    authority.databaseName = databaseName;
    authority.number = number;
    authorities.push_back(authority);
    return sSuccess;
}
//授予表级对应权限
int User::GrantAuthority(const std::string& databaseName,const std::string& tableName,const authorityNum& number) {
    if(CheckAuthority(databaseName, tableName,number) == sSuccess) return sAuthorityExisted;

    Privilege authority;
    authority.databaseName = databaseName;
    authority.tableName = tableName;
    authority.number = number;
    authorities.push_back(authority);
    return sSuccess;
}
//授予表级的全部权限
int User::GrantAllTableAuthorities(const std::string& databaseName,const std::string& tableName) {
    for(int i = 0; i < TABLE_COUNT; i++) {
        authorityNum curr = static_cast<authorityNum>(i);
        if(CheckAuthority(databaseName, tableName, curr) != sSuccess) {
            GrantAuthority(databaseName, tableName, curr);
        }
    }

    return sSuccess;
}
//授予数据库级的全部权限
int User::GrantAllDatabaseAuthorities(const std::string& databaseName) {
    for(int i = TABLE_COUNT + 1; i < DATABASE_COUNT; i++) {
        authorityNum curr = static_cast<authorityNum>(i);
        if(CheckAuthority(databaseName, curr) != sSuccess) {
            GrantAuthority(databaseName, curr);
        }
    }

    return sSuccess;
}

/***权限消去***/

//收回数据库级对应权限
int User::RevokeAuthority(const std::string& databaseName,const authorityNum& number) {
    if(CheckAuthority(databaseName, number) != sSuccess) return sAuthorityNotFound;
    for(int i = 0; i < authorities.size(); ++i) {
        const auto& authority = authorities[i];
        if(authority.databaseName == databaseName && authority.tableName == "" && authority.number == number) {
            authorities.erase(authorities.begin() + i);
            return sSuccess;
        }
    }
    return sSuccess;
}
//收回表级对应权限
int User::RevokeAuthority(const std::string& databaseName,const std::string& tableName,const authorityNum& number) {
    if(CheckAuthority(databaseName, tableName, number) != sSuccess) return sAuthorityNotFound;
    for(int i = 0; i < authorities.size(); ++i) {
        const auto& authority = authorities[i];
        if(authority.databaseName == databaseName && authority.tableName == tableName && authority.number == number) {
            authorities.erase(authorities.begin() + i);
            return sSuccess;
        }
    }
    return sSuccess;
}
//收回表级全部权限
int User::RevokeAllTableAuthorities(const std::string& databaseName,const std::string& tableName) {
    for(auto it = authorities.begin(); it != authorities.end();) {
        if(it->databaseName == databaseName && it->tableName == tableName) {
            authorities.erase(it);
        }
        else it++;
    }
    return sSuccess;
}
//收回数据库级全部权限
int User::RevokeAllDatabaseAuthorities(const std::string& databaseName) {
    for(auto it = authorities.begin(); it != authorities.end();) {
        if(it->databaseName == databaseName && it->tableName == "") {
            authorities.erase(it);
        }
        else it++;
    }
    return sSuccess;
}

//收回对该数据库的所有权限
int User::RevokeAllDatabaseAndTableAuthorities(const std::string& databaseName) {
    for(auto it = authorities.begin(); it != authorities.end();) {
        if(it->databaseName == databaseName) {
            authorities.erase(it);
        }
        else it++;
    }
    return sSuccess;
}



//GET
std::string User::getUserName() const{
    return userName;
}
std::string User::getUserPassword() const{
    return userPassword;
}
std::vector<Privilege> User::getAuthorities() const {
    return authorities;
}

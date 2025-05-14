#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include <constants.h>

//全部权限总览(对应数字从0开始)
enum authorityNum {
    //表级权限
    SELECT = 0,//0
    INSERT,//1
    UPDATE,
    DELETE,
    ALTER,
    INDEX,

    TABLE_COUNT,//表级权限标识符
    //数据库级权限
    CREATE,
    DROP,

    DATABASE_COUNT,//数据库级权限标识符
};

//用户对应权限
struct Privilege {

    std::string databaseName;//数据库级
    std::string tableName;//表级
    authorityNum number;//对应权限数
};


class User {




private:
    std::vector<Privilege> authorities;
    std::string userName;
    std::string userPassword;
public:
    User(const std::string& userName,const std::string& userPassword,const std::vector<Privilege>& authorities);
    User(const std::string& userName,const std::string& userPassword);

    /***登入相关***/

    int Identify(std::string password);//登入验证


    /***权限信息修改***/

    int ModifyDatabaseName(const std::string& databaseName,const std::string& newDatabaseName);

    int ModifyTableName(const std::string& databaseName,const std::string& tableName,const std::string& newTableName);
    /***权限查询***/

    //查询数据库级特定权限
    int CheckAuthority(const std::string& databaseName,const authorityNum& number) const;
    //查询表级特定权限
    int CheckAuthority(const std::string& databaseName,const std::string& tableName,const authorityNum& number) const;
    //查询用户是否有该数据库的权限
    int CheckDatabaseInAuthorities(const std::string& databaseName) const;
    //查询用户是否有该表的权限
    int CheckTableInAuthorities(const std::string& databaseName,const std::string& tableName) const;

    /***权限授予***/


    //授予数据库级对应权限
    int GrantAuthority(const std::string& databaseName,const authorityNum& number) ;
    //授予表级对应权限
    int GrantAuthority(const std::string& databaseName,const std::string& tableName,const authorityNum& number) ;
    //授予表级的全部权限
    int GrantAllTableAuthorities(const std::string& databaseName,const std::string& tableName);
    //授予数据库级的全部权限
    int GrantAllDatabaseAuthorities(const std::string& databaseName);

    /***权限收回***/

    //收回数据库级对应权限
    int RevokeAuthority(const std::string& databaseName,const authorityNum& number) ;
    //收回表级对应权限
    int RevokeAuthority(const std::string& databaseName,const std::string& tableName,const authorityNum& number) ;
    //收回表级全部权限
    int RevokeAllTableAuthorities(const std::string& databaseName,const std::string& tableName);
    //收回数据库级全部权限
    int RevokeAllDatabaseAuthorities(const std::string& databaseName);
    //收回对该数据库的所有权限
    int RevokeAllDatabaseAndTableAuthorities(const std::string& databaseName);

    //GET
    std::string getUserName() const;
    std::string getUserPassword() const;
    std::vector<Privilege> getAuthorities() const;

};




#endif //USER_H

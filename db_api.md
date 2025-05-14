## 数据库操作

- [x] CreateDatabase(std::string databaseName): 创建数据库
- [x] DeleteDatabase(std::string databaseName): 删除数据库
- [x] AlterDatabaseName(std::string databaseName, std::string newDatabaseName): 修改数据库名
- [x] UseDatabase(std::string databaseName = ""): 使用数据库
- [x] ShowDatabases(std::vector<<std::string>>& allDatabases): 显示所有数据库

## 表DDL操作

- [x] ShowTables(std::vector<std::string>& returnTables): 显示所有表
- [x] CreateTable(std::string tableName, std::vector<std::pair<std::string, std::string>> fields, std::vector<Constraint *> constraints): 创建表
- [x] DropTable(std::string tableName): 删除表
- [x] AlterTableName(const std::string& databaseName, const std::string& tableName, const std::string& newTableName): 修改表名
- [x] AlterTableAdd(std::string tableName, std::pair<std::string, std::string> field): 修改表结构，增加字段
- [x] AlterTableDrop(std::string tableName, std::string fieldName): 删除字段
- [x] AlterTableModify(std::string tableName, std::pair<std::string, std::string> field): 修改字段
- [x] int AlterTableColumnName(std::string tableName, std::string columnName, std::string newColumnName);//修改列名
- [x] AlterTableConstraint(std::string tableName, Constraint* constraint): 添加约束条件
- [x] AlterTableDeleteConstraint(std::string tableName, std::string constraintName): 删除约束
- [x] ShowConstraints(std::vector<std::vector<std::any>>& retRecords): 显示约束
- [x] DescribeTable(std::string tableName,std::vector<std::pair<std::string, std::string>>& fields,std::vector<Constraint*>&    constraints);//查看表结构

## 表DML操作

- [x]  int Select(std::string tableName,
                 std::vector<std::string> fieldName,
                 std::vector<std::tuple<std::string, std::string, int>> conditions,
                 std::vector<std::vector<std::any>> &returnRecords,
                 const std::vector<std::string>& orderbyKey = std::vector<std::string>());   //单表查询
      int Select(std::vector<std::string> tablesName,
                 std::vector<std::string> fieldName,
                 std::vector<std::tuple<std::string, std::string, int>> conditions,
                 std::vector<std::vector<std::any>> &returnRecords,
                 const std::vector<std::string>& orderbyKey = std::vector<std::string>());   //多表查询
- [x] Insert(const std::string& tableName, const std::vector<std::pair<std::string, std::string>>& records): 插入记录
- [x] Update(const std::string& tableName, const std::vector<std::pair<std::string, std::string>>& values, const std::vector<std::tuple<std::string, std::string, int>>& conditions): 更新(修改)记录
- [x] Delete(const std::string& tableName, const std::vector<std::tuple<std::string, std::string, int>>& conditions): 删除记录
- [ ] Index(): 索引

## 文件操作
- [x] Write(): 写入
- [x] Read(bool isPrint): 读取

## 权限操作
- [ ] isRoot(): 判断当前用户是否为root用户(root拥有全部权限)
- [ ] isDatabaseOwner(const std::string& databaseName): 判断当前用户是否为对应数据库拥有者(是否有管理该数据库权限)
- [ ] GrantDatabaseOwner(const std::string& databaseName, const std::string& userName): (只有root能进行)授予用户数据库所有权
- [ ] GrantAuthority(const std::string& userName, const std::string& databaseName, const std::string& tableName, const std::string& authority): 授予权限
- [ ] RevokeDatabaseOwner(const std::string& databaseName): (只有root能进行)收回用户数据库所有权
- [ ] RevokeAuthority(const std::string& userName, const std::string& databaseName, const std::string& tableName, const std::string& authority): 收回权限
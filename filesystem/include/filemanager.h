#ifndef FILEMANAGER_H
#define FILEMANAGER_H
/**
 * 单例模式FileManager
 */

#include <string.h>
#include "database.h"
#include "table.h"
#include "constants.h"
#include "sqltool.h"

class FileManager {
private:
    FileManager();
public:
    static FileManager& GetInstance();

    int ClearData();
    //Write

    int WriteDatabasesFile(const std::vector<Database>& databases);

    int WriteTablesFile(const std::string& databaseName,const std::vector<Table>& tables);

    int WriteUsersFile();

    //Read

    int ReadDatabasesFile(std::vector<Database>& databases);

    int ReadTablesFile(const std::string& databaseName,
                       std::vector<Table>& tables);

    int ReadUsersFile();

};

#endif // FILEMANAGER_H

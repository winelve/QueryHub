#ifndef CONSTANTS_H
#define CONSTANTS_H

/**
 *  常量类: 存储所有常数(函数返回状态或结果)
 */


//MAIN
const int sSuccess = 0;
const int sDatabaseNotUse = 1;
const int sDatabaseNotFound = 2;
const int sCurrentDatabaseError = 3;
const int sDatabaseExisted = 4;
const int sUserNotLogin = 10;

//CONST
const int sEqual =  30;
const int sLess =  31;
const int sLarger =  32;
const int sNotSameType = 33;
const int sDataTypeWrong = 34;
//TABLE
const int sTableNameExisted = 50;
const int sFieldNotFound = 51;
const int sPrimaryKeyExcessive = 52;
const int sFieldValueNotFound = 53;
const int sTableNotFound = 54;
const int sUniqueRepeated = 55;
const int sFieldExisted = 56;
//FILE
const int sFileClearFailed = 160;

//Constraints
const int sPrimaryKey = 111;
const int sForeignKey = 112;
const int sForeignRefered = 113;
const int sUnique = 114;
const int sNotNull = 115;
const int sDefault = 116;
const int sBeingRefered = 117;
const int sConstraintForeignKeyConflict = 118;
const int sConstraintDefaultConflict = 119;
const int sPrimaryKeyRepeated = 120;
const int sNotNullEmpty = 121;
const int sConstraintNameExisted = 122;
const int sConstraintNotFound = 123;

#endif // CONSTANTS_H

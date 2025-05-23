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
const int sCurrentUserError = 5;
const int sValueEmpty = 6;
const int sValueIllegal = 7;
const int sErrorInProcess = 8;
const int sSizeNotProper = 9;

//USER
const int sUserNotLogin = 10;
const int sIdentifiedFail =  11;
const int sInsufficientAuthority = 12;
const int sAuthorityExisted = 13;
const int sAuthorityNotFound = 14;
const int sUserNameExisted = 15;
const int sUserNameNotFound = 16;
const int sUserPasswordError = 17;
const int sUserNotCurrDBOwner = 18;
//CONST
const int sEqual =  0;
const int sLess =  -1;
const int sLarger =  1;
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


//CONDITION
const int sConditionsNotSatisfied = 57;
const int sLessEqualCondition = 58;// <=
const int sLargerEqualCondition = 59;// >=
const int sEqualCondition = 60;// =
const int sLessCondition = 61;// <
const int sLargerCondition = 62;// >
const int sNotEqualCondition = 63;// !=
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
const int sConstraintUniqueConflict = 124;
const int sConstraintNotNullConflict = 125;
const int sConstraintPrimaryKeyConflict = 126;

//INDEX
const int sIndexExisted = 145;
const int sIndexNotFound = 146;

#endif // CONSTANTS_H

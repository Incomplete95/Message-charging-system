#ifndef DBOP_H
#define DBOP_H

#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <odbcss.h>

#define DISTINCT 1

//暂时支持的数据类型
#define money double
#define bigint __int64  //output"%lld"
#define nchar char
#define varchar char
#define date char //array length=11
/*
失败返回0，成功返回1
对于字符串数据需额外加''
使用方法：创建对象DBOP var=DBOP();
          var.open(DSN,UID,AuthStr);
		  var.***();调用功能语句，其中var.getsql(sql),可直接用完整的sql语句执行
		  如果上一步使用var.select()或var.getsql(sql)中sql是select，则
		  循环调用var.getvalues(**)功能，根据需要，输入参数，当前最多七个
*/
class DBOP{
private:
	SQLHENV henv;
    SQLHDBC hdbc;
    SQLHSTMT hstmt;
	RETCODE retcode;
	UCHAR sql[300];
	bool flags;
	int gettype(int var);
	int gettype(char* var);
	int gettype(bigint var);
	int gettype(money var);
public:
	DBOP();
	//DSN ODBC数据源名，UID用户名，AuthStr验证码
	int open(char* DSN,char* UID,char* AuthStr);
	//table表名，distinct=DISTINCT：结果无重复，col获取的列，cond=NULL：无条件设置，order=NULL 不排序
	int select(char* table,int distinct,char* col,char* cond,char* order=NULL);
	int insert(char* table,char* col,char* value);
	int insert(char* table,char* value);
	int update(char* table,char* set,char* cond);
	//if cond=NULL,delete all
	int _delete(char* table,char* cond=NULL);
	int getsql(char* sql);
	//T不支持指针变量，不支持数组，支持字符串，返回0代表错误或数据已全部获取
	template <class T1>
	int getvalues(T1& value1);
	template <class T1,class T2>
	int getvalues(T1& value1,T2& value2);
	template <class T1,class T2,class T3>
	int getvalues(T1& value1,T2& value2,T3& value3);
	template <class T1,class T2,class T3,class T4>
	int getvalues(T1& value1,T2& value2,T3& value3,T4& value4);
	template <class T1,class T2,class T3,class T4,class T5>
	int getvalues(T1& value1,T2& value2,T3& value3,T4& value4,T5& value5);
	template <class T1,class T2,class T3,class T4,class T5,class T6>
	int getvalues(T1& value1,T2& value2,T3& value3,T4& value4,T5& value5,T6& value6);
	template <class T1,class T2,class T3,class T4,class T5,class T6,class T7>
	int getvalues(T1& value1,T2& value2,T3& value3,T4& value4,T5& value5,T6& value6,T7& value7);
	int close();
};
template <class T1>
int DBOP::getvalues(T1& value1){
	int len;
	if(flags){
		len=gettype(value1);
		SQLBindCol(hstmt, 1, len, &value1, sizeof(value1), 0);
		flags=false;
	}
	retcode=SQLFetch(hstmt); 
	if(retcode!=SQL_SUCCESS&&retcode!=SQL_SUCCESS_WITH_INFO){
		return 0;
	}
	return 1;
}
template <class T1,class T2>
int DBOP::getvalues(T1& value1,T2& value2){
	int len;
	if(flags){
		len=gettype(value1);
		SQLBindCol(hstmt, 1, len, &value1, sizeof(value1), 0);
		len=gettype(value2);
		SQLBindCol(hstmt, 2, len, &value2, sizeof(value2), 0);
		flags=false;
	}
	retcode=SQLFetch(hstmt); 
	if(retcode!=SQL_SUCCESS&&retcode!=SQL_SUCCESS_WITH_INFO){
		return 0;
	}
	return 1;
}
template <class T1,class T2,class T3>
int DBOP::getvalues(T1& value1,T2& value2,T3& value3){
	int len;
	if(flags){
		len=gettype(value1);
		SQLBindCol(hstmt, 1, len, &value1, sizeof(value1), 0);
		len=gettype(value2);
		SQLBindCol(hstmt, 2, len, &value2, sizeof(value2), 0);
		len=gettype(value3);
		SQLBindCol(hstmt, 3, len, &value3, sizeof(value3), 0);
		flags=false;
	}
	retcode=SQLFetch(hstmt); 
	if(retcode!=SQL_SUCCESS&&retcode!=SQL_SUCCESS_WITH_INFO){
		return 0;
	}
	return 1;
}
template <class T1,class T2,class T3,class T4>
int DBOP::getvalues(T1& value1,T2& value2,T3& value3,T4& value4){
	int len;
	if(flags){
		len=gettype(value1);
		SQLBindCol(hstmt, 1, len, &value1, sizeof(value1), 0);
		len=gettype(value2);
		SQLBindCol(hstmt, 2, len, &value2, sizeof(value2), 0);
		len=gettype(value3);
		SQLBindCol(hstmt, 3, len, &value3, sizeof(value3), 0);
		len=gettype(value4);
		SQLBindCol(hstmt, 4, len, &value4, sizeof(value4), 0);
		flags=false;
	}
	retcode=SQLFetch(hstmt); 
	if(retcode!=SQL_SUCCESS&&retcode!=SQL_SUCCESS_WITH_INFO){
		return 0;
	}
	return 1;
}
template <class T1,class T2,class T3,class T4,class T5>
int DBOP::getvalues(T1& value1,T2& value2,T3& value3,T4& value4,T5& value5){
	int len;
	if(flags){
		len=gettype(value1);
		SQLBindCol(hstmt, 1, len, &value1, sizeof(value1), 0);
		len=gettype(value2);
		SQLBindCol(hstmt, 2, len, &value2, sizeof(value2), 0);
		len=gettype(value3);
		SQLBindCol(hstmt, 3, len, &value3, sizeof(value3), 0);
		len=gettype(value4);
		SQLBindCol(hstmt, 4, len, &value4, sizeof(value4), 0);
		len=gettype(value5);
		SQLBindCol(hstmt, 5, len, &value5, sizeof(value5), 0);
		flags=false;
	}
	retcode=SQLFetch(hstmt); 
	if(retcode!=SQL_SUCCESS&&retcode!=SQL_SUCCESS_WITH_INFO){
		return 0;
	}
	return 1;
}
template <class T1,class T2,class T3,class T4,class T5,class T6>
int DBOP::getvalues(T1& value1,T2& value2,T3& value3,T4& value4,T5& value5,T6& value6){
	int len;
	if(flags){
		len=gettype(value1);
		SQLBindCol(hstmt, 1, len, &value1, sizeof(value1), 0);
		len=gettype(value2);
		SQLBindCol(hstmt, 2, len, &value2, sizeof(value2), 0);
		len=gettype(value3);
		SQLBindCol(hstmt, 3, len, &value3, sizeof(value3), 0);
		len=gettype(value4);
		SQLBindCol(hstmt, 4, len, &value4, sizeof(value4), 0);
		len=gettype(value5);
		SQLBindCol(hstmt, 5, len, &value5, sizeof(value5), 0);
		len=gettype(value6);
		SQLBindCol(hstmt, 6, len, &value6, sizeof(value6), 0);
		flags=false;
	}
	retcode=SQLFetch(hstmt); 
	if(retcode!=SQL_SUCCESS&&retcode!=SQL_SUCCESS_WITH_INFO){
		return 0;
	}
	return 1;
}
template <class T1,class T2,class T3,class T4,class T5,class T6,class T7>
int DBOP::getvalues(T1& value1,T2& value2,T3& value3,T4& value4,T5& value5,T6& value6,T7& value7){
	int len;
	if(flags){
		len=gettype(value1);
		SQLBindCol(hstmt, 1, len, &value1, sizeof(value1), 0);
		len=gettype(value2);
		SQLBindCol(hstmt, 2, len, &value2, sizeof(value2), 0);
		len=gettype(value3);
		SQLBindCol(hstmt, 3, len, &value3, sizeof(value3), 0);
		len=gettype(value4);
		SQLBindCol(hstmt, 4, len, &value4, sizeof(value4), 0);
		len=gettype(value5);
		SQLBindCol(hstmt, 5, len, &value5, sizeof(value5), 0);
		len=gettype(value6);
		SQLBindCol(hstmt, 6, len, &value6, sizeof(value6), 0);
		len=gettype(value7);
		SQLBindCol(hstmt, 7, len, &value7, sizeof(value7), 0);
		flags=false;
	}
	retcode=SQLFetch(hstmt); 
	if(retcode!=SQL_SUCCESS&&retcode!=SQL_SUCCESS_WITH_INFO){
		return 0;
	}
	return 1;
}
#endif // EXPRESSIV_CONTROL_H
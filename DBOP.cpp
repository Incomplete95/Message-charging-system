#include "DBOP.h"
#include <string>
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
//#include <stdafx>
#include "windows.h"
//#include <winnls>
using namespace std;
DBOP::DBOP(){
	SQLHENV henv = SQL_NULL_HENV;
    SQLHDBC hdbc = SQL_NULL_HDBC;
    SQLHSTMT hstmt = SQL_NULL_HSTMT;
	flags=false;
}
int DBOP::open(char* DSN,char* UID,char* AuthStr){
	retcode = SQLAllocHandle (SQL_HANDLE_ENV, NULL, &henv);
	retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION,(SQLPOINTER)SQL_OV_ODBC3,SQL_IS_INTEGER);
	retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
	retcode = SQLConnect(hdbc, (UCHAR*)DSN, strlen((const char*)DSN),(UCHAR*)UID, strlen((const char*)UID),(UCHAR*)AuthStr, strlen((const char*)AuthStr));
	if ( (retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO) ) {
		SQLDisconnect(hdbc);
        SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
        SQLFreeHandle(SQL_HANDLE_ENV, henv);
		return 0;
	}
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	if((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		return 0;
	}
	return 1;
}
//distinct=DISTINCT：结果无重复，cond=NULL：无条件设置，order=NULL 不排序
int DBOP::select(char* table,int distinct,char* col,char* cond,char* order){
    SQLCloseCursor (hstmt);
	strcpy((char *)sql,"select ");
	if(distinct==1){
		strcat((char *)sql,"distinct ");
	}
	strcat((char *)sql,col);
	strcat((char *)sql," from ");
	strcat((char *)sql,table);
	if(cond!=NULL){
		strcat((char *)sql," where ");
		strcat((char *)sql,cond);
	}
	if(order!=NULL){
		strcat((char *)sql," order by ");
		strcat((char *)sql,order);
	}
    retcode=SQLExecDirect (hstmt,sql,strlen((const char*)sql));
	if((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		return 0;
	}
	flags=true;
	return 1;
}

int DBOP::insert(char* table,char* value){
	SQLCloseCursor (hstmt);
	strcpy((char *)sql,"insert into ");
	strcat((char *)sql,table);
	strcat((char *)sql," values(");
	strcat((char *)sql,value);
	strcat((char *)sql,")");
    retcode=SQLExecDirect (hstmt,sql,strlen((const char*)sql));
	if((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		return 0;
	}
	return 1;
}
int DBOP::insert(char* table,char* col,char* value){
	SQLCloseCursor (hstmt);
	strcpy((char *)sql,"insert into ");
	strcat((char *)sql,table);
	strcat((char *)sql," (");
	strcat((char *)sql,col);
	strcat((char *)sql,")");
	strcat((char *)sql," values(");
	strcat((char *)sql,"value");
	strcat((char *)sql,")");
    retcode=SQLExecDirect (hstmt,sql,strlen((const char*)sql));
	if((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		return 0;
	}
	return 1;
}
int DBOP::update(char* table,char* set,char* cond){
	SQLCloseCursor (hstmt);
	strcpy((char *)sql,"update ");
	strcat((char *)sql,table);
	strcat((char *)sql," set ");
	strcat((char *)sql,set);
	strcat((char *)sql," where ");
	strcat((char *)sql,cond);
	retcode=SQLExecDirect (hstmt,sql,strlen((const char*)sql));
	if((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		return 0;
	}
	return 1;
}

//if cond=NULL,delete all
int DBOP::_delete(char* table,char* cond){
	SQLCloseCursor (hstmt);
	strcpy((char *)sql,"delete from ");
    strcat((char *)sql,table);
	if(cond!=NULL){
		strcat((char *)sql," where ");
		strcat((char *)sql,cond);
	}
	retcode=SQLExecDirect (hstmt,sql,strlen((const char*)sql));
	if((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		return 0;
	}
	return 1;
}
int DBOP::close(){
	SQLCloseCursor (hstmt);
	SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
	SQLDisconnect(hdbc);
    SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
    SQLFreeHandle(SQL_HANDLE_ENV, henv);
	return 1;
}
int DBOP::getsql(char* sql){
	SQLCloseCursor (hstmt);
	retcode=SQLExecDirect (hstmt,(UCHAR*)sql,SQL_NTS);
	if((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)){
		return 0;
	}
	if(sql[0]=='s'||sql[0]=='S'){ 
		flags=true;
	}
	return 1;
}
int DBOP::gettype(int var){
	return 4;
}
int DBOP::gettype(char* var){
	return 1;
}
int DBOP::gettype(bigint var){
	//or return -25;
	return SQL_C_SBIGINT;
}
int DBOP::gettype(money var){
	return 8;
}

/*--------------------------------------------------------------*/
// UCS2编码
// 输入: pSrc - 源字符串指针
//       nSrcLength - 源字符串长度
// 输出: pDst - 目标编码串指针
// 返回: 目标编码串长度
/*----------------------------------------------------------------*/
int gsmEncodeUcs2(char* pSrc, unsigned char* pDst, int nSrcLength)
{
	int nDstLength;        // UNICODE宽字符数目
	wchar_t wchar[128];  // UNICODE串缓冲区
						 // 字符串-->UNICODE串
	nDstLength = MultiByteToWideChar(CP_ACP, 0, pSrc, nSrcLength, wchar, 128);
	// 高低字节对调，输出
	for (int i = 0; i<nDstLength; i++)
	{
		*pDst++ = wchar[i] >> 8;         // 先输出高位字节
		*pDst++ = wchar[i] & 0xff;       // 后输出低位字节
	}
	// 返回目标编码串长度
	return nDstLength * 2;
}


/*----------------------------------------------------*/
// UCS2解码
// 输入: pSrc - 源编码串指针
//       nSrcLength -  源编码串长度
// 输出: pDst -  目标字符串指针
// 返回: 目标字符串长度
/*-------------------------------------------------------*/
int gsmDecodeUcs2(unsigned char* pSrc, char* pDst, int nSrcLength)
{
	int nDstLength;        // UNICODE宽字符数目
	WCHAR wchar[128];  // UNICODE串缓冲区
					   // 高低字节对调，拼成UNICODE
	for (int i = 0; i<nSrcLength / 2; i++)
	{
		wchar[i] = *pSrc++ << 8;    // 先高位字节
		wchar[i] |= *pSrc++;        // 后低位字节
	}
	// UNICODE串-->字符串
	nDstLength = WideCharToMultiByte(CP_ACP, 0, wchar, nSrcLength / 2, pDst, 140, NULL, NULL);
	// 输出字符串加个结束符
	pDst[nDstLength] = '\0';
	// 返回目标字符串长度
	return nDstLength;
}

/*------------------------------------------------------*/
//7-bit编码
//输入：pSrc - 源字符串指针
//		nSrcLength - 源字符串长度
//输出：pDst - 目标编码串指针
//返回：目标编码串长度
/*------------------------------------------------------*/
int   gsmEncode7bit(char*   pSrc, unsigned   char*   pDst, int   nSrcLength)
{
	int   nSrc; //   源字符串的计数值
	int   nDst; //   目标编码串的计数值
	int   nChar; //   当前正在处理的组内字符字节的序号，范围是0-7
	unsigned   char   nLeft; //   上一字节残余的数据
							 //   计数值初始化
	nSrc = 0;
	nDst = 0;
	//   将源串每8个字节分为一组，压缩成7个字节
	//   循环该处理过程，直至源串被处理完
	//   如果分组不到8字节，也能正确处理
	while (nSrc <= nSrcLength)					//！原程序漏掉了等号，导致编码会漏掉最后一个字符！
	{
		//   取源字符串的计数值的最低3位
		nChar = nSrc & 7;
		//   处理源串的每个字节
		if (nChar == 0)
		{
			//   组内第一个字节，只是保存起来，待处理下一个字节时使用
			nLeft = *pSrc;
		}
		else
		{
			//   组内其它字节，将其右边部分与残余数据相加，得到一个目标编码字节
			*pDst = (*pSrc << (8 - nChar)) | nLeft;
			//   将该字节剩下的左边部分，作为残余数据保存起来
			nLeft = *pSrc >> nChar;
			//   修改目标串的指针和计数值
			pDst++;
			nDst++;
		}
		//   修改源串的指针和计数值
		pSrc++;
		nSrc++;
	}
	//   返回目标串长度
	return   nDst;
}

/*------------------------------------------------------*/
//7-bit解码
//输入：pSrc - 源编码串指针
//		nSrcLength - 源编码串长度
//输出：pDst - 目标字符串指针
//返回：目标字符串长度
/*------------------------------------------------------*/
int   gsmDecode7bit(unsigned   char*   pSrc, char*   pDst, int   nSrcLength)
{
	int   nSrc; //   源字符串的计数值
	int   nDst; //   目标解码串的计数值
	int   nByte; //   当前正在处理的组内字节的序号，范围是0-6
	unsigned   char   nLeft; //   上一字节残余的数据
							 //   计数值初始化
	nSrc = 0;
	nDst = 0;
	//   组内字节序号和残余数据初始化
	nByte = 0;
	nLeft = 0;
	//   将源数据每7个字节分为一组，解压缩成8个字节
	//   循环该处理过程，直至源数据被处理完
	//   如果分组不到7字节，也能正确处理
	while (nSrc<nSrcLength)
	{
		//   将源字节右边部分与残余数据相加，去掉最高位，得到一个目标解码字节
		*pDst = ((*pSrc << nByte) | nLeft) & 0x7f;
		//   将该字节剩下的左边部分，作为残余数据保存起来
		nLeft = *pSrc >> (7 - nByte);
		//   修改目标串的指针和计数值
		pDst++;
		nDst++;
		//   修改字节计数值
		nByte++;
		//   到了一组的最后一个字节
		if (nByte == 7)
		{
			//   额外得到一个目标解码字节
			*pDst = nLeft;
			//   修改目标串的指针和计数值
			pDst++;
			nDst++;
			//   组内字节序号和残余数据初始化
			nByte = 0;
			nLeft = 0;
		}
		//   修改源串的指针和计数值
		pSrc++;
		nSrc++;
	}
	//   输出字符串加个结束符
	pDst[nDst] = '\0';
	//   返回目标串长度
	return   nDst;
}

void main() {
	int ctn = 1;
	//数据库相关变量
	DBOP var = DBOP();
	char* InsrtValue;
	char* UpdtSet;
	char* table;
	int distinct;
	char* col;
	char* cond;
	char* order;
	char Num[15];
	char Name[10];
	char discount[2];
	double remains;
	char* MsgNum;
	char* PhoneNum;
	int* MsgType;
	int* MsgLen;
	char* DSN = "test";
	char* UID = "SQL_CPP";
	char* AuthStr = "940504";
	char Remains[10];
	char SinQuo[2] = "'";
	int isConnect;
	int isSelect;
	int isUpdate;
	int isGetValue;
	//短信PDU编解码相关变量
	char ChnSrcStr[70];
	char UCSDecodeDest[140] = { 0 };
	unsigned char UCSEncodeDest[70] = { 0 };
	char EngSrcStr[140];
	unsigned char sBitEncodeDest[140] = { 0 };
	int SrcLen;
	int DestLen;
	int lang = 0;
	char LocNum[20];
	char DestNum[20];
	int LenCnt;
	char UCSEncodeMethd[3] = "08";
	char sBitEncodeMethd[3] = "00";

	char SrvSMSCLen[3]; //PDU[0]-[1]:SMSC地址信息的长度
	char SrvLocNum[20];		 //PDU[4]-[(SMSCLen-1)*2+3]:SMSC地址:LocNum
	char SrvDestNumCnt[3]; //PDU[(SMSCLen-1)*2+8]-[(SMSCLen-1)*2+9]:目标地址数字个数 
	char SrvDestNum[20];   //PDU[(SMSCLen-1)*2+12]-[(SMSCLen-1)*2+12+(DestNumCnt%2)*1+DestNumCnt-1]:目标地址(TP-DA):DestNum 
	//char SrvProtlMark[3]; //协议标识(TP-PID) 是普通GSM类型，点到点方式
	char SrvEncodeMethd[3]; //PDU[(SMSCLen-1)*2+(DestNumCnt%2)*1+DestNumCnt+2]-[(SMSCLen-1)*2+(DestNumCnt%2)*1+DestNumCnt+3]:
							//用户信息编码方式(TP-DCS):00——7-bit;08——UCS-2
	char SrvTextLen[3]; //PDU[(SMSCLen-1)*2+(DestNumCnt%2)*1+DestNumCnt+4]-[(SMSCLen-1)*2+(DestNumCnt%2)*1+DestNumCnt+5]:
						//用户信息长度

	/*---------------------以下是PDU变量---------------------------*/
	char PDU[140] = {};		//PDU总字符串
	char SMSCLen[3]; //1.SMSC地址信息的长度
	char PlusSign[3] = "91"; //2.SMSC地址格式(TON/NPI) 用国际格式号码(在前面加‘+’)
	//3.SMSC地址:LocNum
	char OthrArg[5] = "1100"; // 4.基本参数(TP-MTI/VFP) 发送+消息基准值(TP-MR) 0
	char DestNumCnt[3]; //5.目标地址数字个数 
	//6.PlusSign:目标地址格式(TON/NPI) 用国际格式号码(在前面加‘+’)
	//7.目标地址(TP-DA):DestNum 
	char ProtlMark[3] = "00"; //8.协议标识(TP-PID) 是普通GSM类型，点到点方式
	char EncodeMethd[3]; //9.用户信息编码方式(TP-DCS):00——7-bit;08——UCS-2
	//char TimeStmp[16]; //10.时间戳(TP-SCTS):YYMMDDHHMMSSZZ  ====================>此处应换成有效期而非时间戳
	char TextLen[3]; //11.用户信息长度
	//12.用户信息
	/*-------------------------------------------------------------*/

	/*北斗报文相关变量*/
	char header[5] = "send";
	char sendAddr[20] = { 0 };
	char recvAddr[20] = { 0 };
	char checksum[4] = "sum";
	/*-------------------------连接数据库-----------------------------------------*/
	do {
		isConnect = var.open(DSN, UID, AuthStr);
		cout << "连接数据库结果:" << ((isConnect == 1) ? "成功" : "失败：数据库未开启或参数错误") << endl;
		if (isConnect == 0) {
			cout << "请重新输入数据库名：" << endl;
			cin >> DSN;
			cout << "请重新输入用户名：" << endl;
			cin >> UID;
			cout << "请重新输入密码：" << endl;
			cin >> AuthStr;
		}
	} while (isConnect == 0);

	/*-----------------以下开始短信PDU编解码-----------------------*/
	do {
		cout << "请输入本机电话号码:" << endl;
		cin >> LocNum;
		strcpy(sendAddr, LocNum);
		//sendAddr[sizeof(LocNum)] = '\0';
		LenCnt = (strlen(LocNum) + 1) / 2 + 1;
		_itoa(LenCnt, SMSCLen, 16);
		if (LenCnt > 0 && LenCnt < 16) {
			SMSCLen[1] = SMSCLen[0];
			SMSCLen[0] = '0';
			SMSCLen[2] = '\0';
		}
		cout << "请输入目的电话号码:" << endl;
		cin >> DestNum;
		strcpy(recvAddr, DestNum);
		//recvAddr[sizeof(DestNum)] = '\0';
		LenCnt = strlen(DestNum);
		_itoa(LenCnt, DestNumCnt, 16);
		if (LenCnt > 0 && LenCnt < 16) {
			DestNumCnt[1] = DestNumCnt[0];
			DestNumCnt[0] = '0';
			DestNumCnt[2] = '\0';
		}
		cout << "中文或英文？" << endl;
		cin >> lang;
		//获取当前日期时间和时区
		//time_t Time;
		//time(&Time);
		//strftime(TimeStmp, 16, "%y%m%d%H%M%S%Z", localtime(&Time));
		//printf("%s\n",TimeStmp);
		//cout << Time << endl;
		//system("time");

		//中文编码
		if (lang) {
			strcpy(EncodeMethd, UCSEncodeMethd);
			cout << "请输入短信:" << endl;
			cin >> ChnSrcStr;
			cout << "短信信息:" << endl;
			printf("本机号码:%s\n", LocNum);
			printf("对方号码:%s\n", DestNum);
			printf("短信内容:%s\n", ChnSrcStr);

			if (strlen(LocNum) % 2 == 1)
				strcat(LocNum, "F");
			if (strlen(DestNum) % 2 == 1)
				strcat(DestNum, "F");
			for (int i = 0; i < (strlen(LocNum) / 2); i++) {
				char swap;
				swap = LocNum[i * 2];
				LocNum[i * 2] = LocNum[i * 2 + 1];
				LocNum[i * 2 + 1] = swap;
			}
			for (int i = 0; i < (strlen(DestNum) / 2); i++) {
				char swap;
				swap = DestNum[i * 2];
				DestNum[i * 2] = DestNum[i * 2 + 1];
				DestNum[i * 2 + 1] = swap;
			}
			//strcat(LocNum, DestNum);
			//strcat(LocNum, ChnSrcStr);
			SrcLen = strlen(ChnSrcStr);
			DestLen = gsmEncodeUcs2(ChnSrcStr, UCSEncodeDest, SrcLen);
			_itoa(DestLen, TextLen, 16);
			if (LenCnt > 0 && LenCnt < 16) {
				TextLen[1] = TextLen[0];
				TextLen[0] = '0';
				TextLen[2] = '\0';
			}
			unsigned char* UCSEncodeTemp = UCSEncodeDest;
			printf("UCS-2编码结果:\n");
			for (int i = 0; i < DestLen; i++) {
				if (*UCSEncodeTemp == '\0') {
					printf("0X00 \n");
				}
				else {
					printf("0X%x \n", *UCSEncodeTemp);
				}
				UCSEncodeTemp++;
			}
			//Encapsulation to PDU code
			strcat(PDU, SMSCLen);
			strcat(PDU, PlusSign);
			strcat(PDU, LocNum);
			strcat(PDU, OthrArg);
			strcat(PDU, DestNumCnt);
			strcat(PDU, PlusSign);
			strcat(PDU, DestNum);
			strcat(PDU, ProtlMark);
			strcat(PDU, EncodeMethd);
			strcat(PDU, TextLen);
			printf("PDU:%s\n", PDU);

			printf("北斗报文为：\n");
			printf("%s", header);
			printf("%s", sendAddr);
			printf("%s", recvAddr);
			printf("%s", PDU);
			printf("%s\n", checksum);

			//从PDU中解码
			strncpy(SrvSMSCLen, PDU, 2);
			SrvSMSCLen[2] = '\0';
			printf("SrvSMSCLen = %s\n", SrvSMSCLen);
			int IntSMSCLen = atoi(SrvSMSCLen);
			strncpy(SrvLocNum, PDU + 4, (IntSMSCLen - 1) * 2);
			SrvLocNum[(IntSMSCLen - 1) * 2] = '\0';
			printf("SrvLocNum = %s\n", SrvLocNum);
			strncpy(SrvDestNumCnt, PDU + (IntSMSCLen - 1) * 2 + 8, 2);
			SrvDestNumCnt[2] = '\0';
			printf("SrvDestNumCnt = %s\n", SrvDestNumCnt);
			int IntDestNumCnt = 0;
			sscanf(SrvDestNumCnt, "%x", &IntDestNumCnt);
			int tmp = (atoi(SrvDestNumCnt) % 2) * 1;
			strncpy(SrvDestNum, PDU + (IntSMSCLen - 1) * 2 + 12, (IntDestNumCnt % 2) * 1 + IntDestNumCnt);
			SrvDestNum[(IntDestNumCnt % 2) * 1 + IntDestNumCnt] = '\0';
			printf("SrvDestNum = %s\n", SrvDestNum);
			strncpy(SrvEncodeMethd, PDU + (IntSMSCLen - 1) * 2 + (IntDestNumCnt % 2) * 1 + IntDestNumCnt + 14, 2);
			SrvEncodeMethd[2] = '\0';
			printf("SrvEncodeMethd = %s\n", SrvEncodeMethd);
			strncpy(SrvTextLen, PDU + (IntSMSCLen - 1) * 2 + (IntDestNumCnt % 2) * 1 + IntDestNumCnt + 16, 2);
			SrvTextLen[2] = '\0';
			printf("SrvTextLen = %s\n", SrvTextLen);
			//cout << endl;
			for (int i = 0; i < IntSMSCLen - 1; i++) {
				char swap;
				swap = SrvLocNum[i * 2];
				SrvLocNum[i * 2] = SrvLocNum[i * 2 + 1];
				SrvLocNum[i * 2 + 1] = swap;
			}
			if (SrvLocNum[(IntSMSCLen - 1) * 2 - 1] == 'F')
				SrvLocNum[(IntSMSCLen - 1) * 2 - 1] = '\0';
			for (int i = 0; i < ((IntDestNumCnt % 2) * 1 + IntDestNumCnt) / 2; i++) {
				char swap;
				swap = SrvDestNum[i * 2];
				SrvDestNum[i * 2] = SrvDestNum[i * 2 + 1];
				SrvDestNum[i * 2 + 1] = swap;
			}
			if (SrvDestNum[((IntDestNumCnt % 2) * 1 + IntDestNumCnt) - 1] == 'F')
				SrvDestNum[((IntDestNumCnt % 2) * 1 + IntDestNumCnt) - 1] = '\0';

			//for (int i = 0;i < DestLen;i++) {
			//	cout << DestStr[i] << " ";
			//} 
			SrcLen = gsmDecodeUcs2(UCSEncodeDest, UCSDecodeDest, DestLen);
			//char* UCSDecodeTemp = UCSDecodeDest;
			printf("UCS-2解码结果:\n");
			//for (int i = 0; i < SrcLen; i++) {
			//	printf("%c \n", *UCSDecodeTemp);
			//	UCSDecodeTemp++;
			printf("%s\n", UCSDecodeDest);
		}

		//英文编码
		else {
			strcpy(EncodeMethd, sBitEncodeMethd);
			cout << "请输入短信(空格用/号代替):" << endl; //针对cin把空格作为终止符之一的解决措施
			cin >> EngSrcStr;
			for (int i = 0; EngSrcStr[i] != '\0'; i++) {
				if (EngSrcStr[i] == '/') {
					EngSrcStr[i] = ' ';
				}
			}
			cout << "短信信息:" << endl;
			printf("本机号码:%s\n", LocNum);
			printf("对方号码:%s\n", DestNum);
			printf("短信内容:%s\n", EngSrcStr);

			if (strlen(LocNum) % 2 == 1)
				strcat(LocNum, "F");
			if (strlen(DestNum) % 2 == 1)
				strcat(DestNum, "F");
			for (int i = 0; i < (strlen(LocNum) / 2); i++) {
				char swap;
				swap = LocNum[i * 2];
				LocNum[i * 2] = LocNum[i * 2 + 1];
				LocNum[i * 2 + 1] = swap;
			}
			for (int i = 0; i < (strlen(DestNum) / 2); i++) {
				char swap;
				swap = DestNum[i * 2];
				DestNum[i * 2] = DestNum[i * 2 + 1];
				DestNum[i * 2 + 1] = swap;
			}
			//strcat(LocNum, DestNum);
			//strcat(LocNum, EngSrcStr);
			SrcLen = strlen(EngSrcStr);
			DestLen = gsmEncode7bit(EngSrcStr, sBitEncodeDest, SrcLen);
			_itoa(DestLen, TextLen, 16);
			if (LenCnt > 0 && LenCnt < 16) {
				TextLen[1] = TextLen[0];
				TextLen[0] = '0';
				TextLen[2] = '\0';
			}
			unsigned char* sBitEncodeTemp = sBitEncodeDest;
			printf("7-bit编码结果:\n");
			for (int i = 0; i < DestLen; i++) {
				printf("0X%x \n", *sBitEncodeTemp);
				sBitEncodeTemp++;
			}
			//Encapsulation to PDU code
			strcat(PDU, SMSCLen);
			strcat(PDU, PlusSign);
			strcat(PDU, LocNum);
			strcat(PDU, OthrArg);
			strcat(PDU, DestNumCnt);
			strcat(PDU, PlusSign);
			strcat(PDU, DestNum);
			strcat(PDU, ProtlMark);
			strcat(PDU, EncodeMethd);
			strcat(PDU, TextLen);
			printf("PDU:%s\n", PDU);

			printf("北斗报文为：\n");
			printf("%s", header);
			printf("%s", sendAddr);
			printf("%s", recvAddr);
			printf("%s", PDU);
			printf("%s\n", checksum);

			//从PDU中解码
			strncpy(SrvSMSCLen, PDU, 2);
			SrvSMSCLen[2] = '\0';
			printf("SrvSMSCLen = %s\n", SrvSMSCLen);
			int IntSMSCLen = atoi(SrvSMSCLen);
			strncpy(SrvLocNum, PDU + 4, (IntSMSCLen - 1) * 2);
			SrvLocNum[(IntSMSCLen - 1) * 2] = '\0';
			printf("SrvLocNum = %s\n", SrvLocNum);
			strncpy(SrvDestNumCnt, PDU + (IntSMSCLen - 1) * 2 + 8, 2);
			SrvDestNumCnt[2] = '\0';
			printf("SrvDestNumCnt = %s\n", SrvDestNumCnt);
			int IntDestNumCnt = 0;
			sscanf(SrvDestNumCnt, "%x", &IntDestNumCnt);
			int tmp = (atoi(SrvDestNumCnt) % 2) * 1;
			strncpy(SrvDestNum, PDU + (IntSMSCLen - 1) * 2 + 12, (IntDestNumCnt % 2) * 1 + IntDestNumCnt);
			SrvDestNum[(IntDestNumCnt % 2) * 1 + IntDestNumCnt] = '\0';
			printf("SrvDestNum = %s\n", SrvDestNum);
			strncpy(SrvEncodeMethd, PDU + (IntSMSCLen - 1) * 2 + (IntDestNumCnt % 2) * 1 + IntDestNumCnt + 14, 2);
			SrvEncodeMethd[2] = '\0';
			printf("SrvEncodeMethd = %s\n", SrvEncodeMethd);
			strncpy(SrvTextLen, PDU + (IntSMSCLen - 1) * 2 + (IntDestNumCnt % 2) * 1 + IntDestNumCnt + 16, 2);
			SrvTextLen[2] = '\0';
			printf("SrvTextLen = %s\n", SrvTextLen);
			//cout << endl;
			for (int i = 0; i < IntSMSCLen - 1; i++) {
				char swap;
				swap = SrvLocNum[i * 2];
				SrvLocNum[i * 2] = SrvLocNum[i * 2 + 1];
				SrvLocNum[i * 2 + 1] = swap;
			}
			if (SrvLocNum[(IntSMSCLen - 1) * 2 - 1] == 'F')
				SrvLocNum[(IntSMSCLen - 1) * 2 - 1] = '\0';
			for (int i = 0; i < ((IntDestNumCnt % 2) * 1 + IntDestNumCnt) / 2; i++) {
				char swap;
				swap = SrvDestNum[i * 2];
				SrvDestNum[i * 2] = SrvDestNum[i * 2 + 1];
				SrvDestNum[i * 2 + 1] = swap;
			}
			if (SrvDestNum[((IntDestNumCnt % 2) * 1 + IntDestNumCnt) - 1] == 'F')
				SrvDestNum[((IntDestNumCnt % 2) * 1 + IntDestNumCnt) - 1] = '\0';

			char sBitDecodeDest[140] = { 0 };
			SrcLen = gsmDecode7bit(sBitEncodeDest, sBitDecodeDest, DestLen);
			printf("7-bit解码结果:\n");
			printf("%s\n", sBitDecodeDest);
		}
		
		/*-------------------------以上为短信PDU编解码全过程--------------------------*/
		/*---------------------------以下开始数据库部分操作---------------------------*/
		
		/*-------------------------数据库查询-----------------------------------------*/
		do {
			table = "fee";
			distinct = NULL;
			col = "*";
			char NumHead[25] = "Num = '";
			char NameHead[25] = "Name = '";
			char remainsHead[25] = "remains = '";
			strcat(NumHead, SrvLocNum);
			strcat(NumHead, SinQuo);
			cond = NumHead;
			order = NULL;
			isSelect = var.select(table, distinct, col, cond, order);
			isGetValue = var.getvalues(Num, Name, discount, remains);
			Num[strlen(SrvLocNum)] = '\0';
			int cmpflg = 1;
			for (int i = 0; i < strlen(SrvLocNum); i++) {
				if (SrvLocNum[i] != Num[i]) {
					cmpflg = 0;
					break;
				}
			}
			cout << "查询数据库结果:" << ((cmpflg == 1) ? "成功" : "失败：数据库中不存在此本机号码") << endl;
			if (cmpflg) {
				//flg = var.getvalues(remains);
				cout << "复制查询结果:" << ((isGetValue == 1) ? "成功" : "失败") << endl;
				cout << "电话号码" << "        " << "姓名" << "     " << "套餐" << "  " << "话费余额" << endl;
				cout << Num << "     " << Name << "  " << discount << "    " << remains << endl;
				//cout << "余额:" << remains << endl;
				if (remains <= 0) {
					cout << "余额不足！" << endl;
				}
				else {
					remains = remains - 0.1;
					sprintf(Remains, "%f", remains);
					strcat(remainsHead, Remains);
					strcat(remainsHead, SinQuo);
					UpdtSet = remainsHead;
					isUpdate = var.update(table, UpdtSet, cond);
					cout << "更新数据库结果:" << ((isUpdate == 1) ? "成功" : "失败") << endl;
					isSelect = var.select(table, distinct, col, cond, order);
					cout << "查询数据库结果:" << ((isSelect == 1) ? "成功" : "失败") << endl;
					isGetValue = var.getvalues(Num, Name, discount, remains);
					cout << "复制查询结果:" << ((isGetValue == 1) ? "成功" : "失败") << endl;
					cout << "电话号码" << "        " << "姓名" << "     " << "套餐" << "  " << "话费余额" << endl;
					cout << Num << "  " << Name << "  " << discount << "    " << remains << endl;
				}
			}
			else {
				cout << "请重新输入相关信息" << endl;
				break;
			}

			//cout << Num << "  " << Name << "  " << discount << "    " << remains << endl;
			/*-------------------------数据库插入-----------------------------------------*/
			//InsrtValue = "'18692807366','Admin','2',1000";
			//flg = var.insert(table, InsrtValue);
			//cout << "插入数据库结果:" << ((flg == 1) ? "成功" : "失败") << endl;
			/*col = "*";
			cond = "Num = '18692807366'";
			flg = var.select(table, distinct, col, cond, order);
			cout << "查询数据库结果:" << ((flg == 1) ? "成功" : "失败") << endl;
			flg = var.getvalues(Num, Name, discount, remains);
			cout << "复制查询结果:" << ((flg == 1) ? "成功" : "失败") << endl;
			cout << "电话号码" << "        " << "姓名" << "     " << "套餐" << "  " << "话费余额" << endl;
			cout << Num << "  " << Name << "  " << discount << "    " << remains << endl;
			UpdtSet = "Name = 'Harold'";
			cond = "Num = '15580373586'";
			flg = var.update(table, UpdtSet, cond);
			cout << "更新数据库结果:" << ((flg == 1) ? "成功" : "失败") << endl;*/
			//UpdtSet = "Name = 'admin'";
			//cond = "Num = '18692807366'";
			//flg = var.update(table, UpdtSet, cond);
			//cout << "更新数据库结果:" << (( flg==1 )? "成功":"失败") << endl;
			//cond = "Num = '15580373586'";
			/*flg = var.select(table, distinct, col, cond, order);
			cout << "查询数据库结果:" << ((flg == 1) ? "成功" : "失败") << endl;
			flg = var.getvalues(Num, Name, discount, remains);
			cout << "复制查询结果:" << ((flg == 1) ? "成功" : "失败") << endl;
			cout << "电话号码" << "        " << "姓名" << "     " << "套餐" << "  " << "话费余额" << endl;
			cout << Num << "  " << Name << "  " << discount << "    " << remains << endl;
			//system("pause");*/
		} while (isSelect == 0);
		PDU[0] = '\0';
		cout << "是否有其他短信(是：1/否：0)？" << endl;
		cin >> ctn;
	} while (ctn == 1);
	//system("pause");
}
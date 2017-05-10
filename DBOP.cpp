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
//distinct=DISTINCT��������ظ���cond=NULL�����������ã�order=NULL ������
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
// UCS2����
// ����: pSrc - Դ�ַ���ָ��
//       nSrcLength - Դ�ַ�������
// ���: pDst - Ŀ����봮ָ��
// ����: Ŀ����봮����
/*----------------------------------------------------------------*/
int gsmEncodeUcs2(char* pSrc, unsigned char* pDst, int nSrcLength)
{
	int nDstLength;        // UNICODE���ַ���Ŀ
	wchar_t wchar[128];  // UNICODE��������
						 // �ַ���-->UNICODE��
	nDstLength = MultiByteToWideChar(CP_ACP, 0, pSrc, nSrcLength, wchar, 128);
	// �ߵ��ֽڶԵ������
	for (int i = 0; i<nDstLength; i++)
	{
		*pDst++ = wchar[i] >> 8;         // �������λ�ֽ�
		*pDst++ = wchar[i] & 0xff;       // �������λ�ֽ�
	}
	// ����Ŀ����봮����
	return nDstLength * 2;
}


/*----------------------------------------------------*/
// UCS2����
// ����: pSrc - Դ���봮ָ��
//       nSrcLength -  Դ���봮����
// ���: pDst -  Ŀ���ַ���ָ��
// ����: Ŀ���ַ�������
/*-------------------------------------------------------*/
int gsmDecodeUcs2(unsigned char* pSrc, char* pDst, int nSrcLength)
{
	int nDstLength;        // UNICODE���ַ���Ŀ
	WCHAR wchar[128];  // UNICODE��������
					   // �ߵ��ֽڶԵ���ƴ��UNICODE
	for (int i = 0; i<nSrcLength / 2; i++)
	{
		wchar[i] = *pSrc++ << 8;    // �ȸ�λ�ֽ�
		wchar[i] |= *pSrc++;        // ���λ�ֽ�
	}
	// UNICODE��-->�ַ���
	nDstLength = WideCharToMultiByte(CP_ACP, 0, wchar, nSrcLength / 2, pDst, 140, NULL, NULL);
	// ����ַ����Ӹ�������
	pDst[nDstLength] = '\0';
	// ����Ŀ���ַ�������
	return nDstLength;
}

/*------------------------------------------------------*/
//7-bit����
//���룺pSrc - Դ�ַ���ָ��
//		nSrcLength - Դ�ַ�������
//�����pDst - Ŀ����봮ָ��
//���أ�Ŀ����봮����
/*------------------------------------------------------*/
int   gsmEncode7bit(char*   pSrc, unsigned   char*   pDst, int   nSrcLength)
{
	int   nSrc; //   Դ�ַ����ļ���ֵ
	int   nDst; //   Ŀ����봮�ļ���ֵ
	int   nChar; //   ��ǰ���ڴ���������ַ��ֽڵ���ţ���Χ��0-7
	unsigned   char   nLeft; //   ��һ�ֽڲ��������
							 //   ����ֵ��ʼ��
	nSrc = 0;
	nDst = 0;
	//   ��Դ��ÿ8���ֽڷ�Ϊһ�飬ѹ����7���ֽ�
	//   ѭ���ô�����̣�ֱ��Դ����������
	//   ������鲻��8�ֽڣ�Ҳ����ȷ����
	while (nSrc <= nSrcLength)					//��ԭ����©���˵Ⱥţ����±����©�����һ���ַ���
	{
		//   ȡԴ�ַ����ļ���ֵ�����3λ
		nChar = nSrc & 7;
		//   ����Դ����ÿ���ֽ�
		if (nChar == 0)
		{
			//   ���ڵ�һ���ֽڣ�ֻ�Ǳ�����������������һ���ֽ�ʱʹ��
			nLeft = *pSrc;
		}
		else
		{
			//   ���������ֽڣ������ұ߲��������������ӣ��õ�һ��Ŀ������ֽ�
			*pDst = (*pSrc << (8 - nChar)) | nLeft;
			//   �����ֽ�ʣ�µ���߲��֣���Ϊ�������ݱ�������
			nLeft = *pSrc >> nChar;
			//   �޸�Ŀ�괮��ָ��ͼ���ֵ
			pDst++;
			nDst++;
		}
		//   �޸�Դ����ָ��ͼ���ֵ
		pSrc++;
		nSrc++;
	}
	//   ����Ŀ�괮����
	return   nDst;
}

/*------------------------------------------------------*/
//7-bit����
//���룺pSrc - Դ���봮ָ��
//		nSrcLength - Դ���봮����
//�����pDst - Ŀ���ַ���ָ��
//���أ�Ŀ���ַ�������
/*------------------------------------------------------*/
int   gsmDecode7bit(unsigned   char*   pSrc, char*   pDst, int   nSrcLength)
{
	int   nSrc; //   Դ�ַ����ļ���ֵ
	int   nDst; //   Ŀ����봮�ļ���ֵ
	int   nByte; //   ��ǰ���ڴ���������ֽڵ���ţ���Χ��0-6
	unsigned   char   nLeft; //   ��һ�ֽڲ��������
							 //   ����ֵ��ʼ��
	nSrc = 0;
	nDst = 0;
	//   �����ֽ���źͲ������ݳ�ʼ��
	nByte = 0;
	nLeft = 0;
	//   ��Դ����ÿ7���ֽڷ�Ϊһ�飬��ѹ����8���ֽ�
	//   ѭ���ô�����̣�ֱ��Դ���ݱ�������
	//   ������鲻��7�ֽڣ�Ҳ����ȷ����
	while (nSrc<nSrcLength)
	{
		//   ��Դ�ֽ��ұ߲��������������ӣ�ȥ�����λ���õ�һ��Ŀ������ֽ�
		*pDst = ((*pSrc << nByte) | nLeft) & 0x7f;
		//   �����ֽ�ʣ�µ���߲��֣���Ϊ�������ݱ�������
		nLeft = *pSrc >> (7 - nByte);
		//   �޸�Ŀ�괮��ָ��ͼ���ֵ
		pDst++;
		nDst++;
		//   �޸��ֽڼ���ֵ
		nByte++;
		//   ����һ������һ���ֽ�
		if (nByte == 7)
		{
			//   ����õ�һ��Ŀ������ֽ�
			*pDst = nLeft;
			//   �޸�Ŀ�괮��ָ��ͼ���ֵ
			pDst++;
			nDst++;
			//   �����ֽ���źͲ������ݳ�ʼ��
			nByte = 0;
			nLeft = 0;
		}
		//   �޸�Դ����ָ��ͼ���ֵ
		pSrc++;
		nSrc++;
	}
	//   ����ַ����Ӹ�������
	pDst[nDst] = '\0';
	//   ����Ŀ�괮����
	return   nDst;
}

void main() {
	int ctn = 1;
	//���ݿ���ر���
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
	//����PDU�������ر���
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

	char SrvSMSCLen[3]; //PDU[0]-[1]:SMSC��ַ��Ϣ�ĳ���
	char SrvLocNum[20];		 //PDU[4]-[(SMSCLen-1)*2+3]:SMSC��ַ:LocNum
	char SrvDestNumCnt[3]; //PDU[(SMSCLen-1)*2+8]-[(SMSCLen-1)*2+9]:Ŀ���ַ���ָ��� 
	char SrvDestNum[20];   //PDU[(SMSCLen-1)*2+12]-[(SMSCLen-1)*2+12+(DestNumCnt%2)*1+DestNumCnt-1]:Ŀ���ַ(TP-DA):DestNum 
	//char SrvProtlMark[3]; //Э���ʶ(TP-PID) ����ͨGSM���ͣ��㵽�㷽ʽ
	char SrvEncodeMethd[3]; //PDU[(SMSCLen-1)*2+(DestNumCnt%2)*1+DestNumCnt+2]-[(SMSCLen-1)*2+(DestNumCnt%2)*1+DestNumCnt+3]:
							//�û���Ϣ���뷽ʽ(TP-DCS):00����7-bit;08����UCS-2
	char SrvTextLen[3]; //PDU[(SMSCLen-1)*2+(DestNumCnt%2)*1+DestNumCnt+4]-[(SMSCLen-1)*2+(DestNumCnt%2)*1+DestNumCnt+5]:
						//�û���Ϣ����

	/*---------------------������PDU����---------------------------*/
	char PDU[140] = {};		//PDU���ַ���
	char SMSCLen[3]; //1.SMSC��ַ��Ϣ�ĳ���
	char PlusSign[3] = "91"; //2.SMSC��ַ��ʽ(TON/NPI) �ù��ʸ�ʽ����(��ǰ��ӡ�+��)
	//3.SMSC��ַ:LocNum
	char OthrArg[5] = "1100"; // 4.��������(TP-MTI/VFP) ����+��Ϣ��׼ֵ(TP-MR) 0
	char DestNumCnt[3]; //5.Ŀ���ַ���ָ��� 
	//6.PlusSign:Ŀ���ַ��ʽ(TON/NPI) �ù��ʸ�ʽ����(��ǰ��ӡ�+��)
	//7.Ŀ���ַ(TP-DA):DestNum 
	char ProtlMark[3] = "00"; //8.Э���ʶ(TP-PID) ����ͨGSM���ͣ��㵽�㷽ʽ
	char EncodeMethd[3]; //9.�û���Ϣ���뷽ʽ(TP-DCS):00����7-bit;08����UCS-2
	//char TimeStmp[16]; //10.ʱ���(TP-SCTS):YYMMDDHHMMSSZZ  ====================>�˴�Ӧ������Ч�ڶ���ʱ���
	char TextLen[3]; //11.�û���Ϣ����
	//12.�û���Ϣ
	/*-------------------------------------------------------------*/

	/*����������ر���*/
	char header[5] = "send";
	char sendAddr[20] = { 0 };
	char recvAddr[20] = { 0 };
	char checksum[4] = "sum";
	/*-------------------------�������ݿ�-----------------------------------------*/
	do {
		isConnect = var.open(DSN, UID, AuthStr);
		cout << "�������ݿ���:" << ((isConnect == 1) ? "�ɹ�" : "ʧ�ܣ����ݿ�δ�������������") << endl;
		if (isConnect == 0) {
			cout << "�������������ݿ�����" << endl;
			cin >> DSN;
			cout << "�����������û�����" << endl;
			cin >> UID;
			cout << "�������������룺" << endl;
			cin >> AuthStr;
		}
	} while (isConnect == 0);

	/*-----------------���¿�ʼ����PDU�����-----------------------*/
	do {
		cout << "�����뱾���绰����:" << endl;
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
		cout << "������Ŀ�ĵ绰����:" << endl;
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
		cout << "���Ļ�Ӣ�ģ�" << endl;
		cin >> lang;
		//��ȡ��ǰ����ʱ���ʱ��
		//time_t Time;
		//time(&Time);
		//strftime(TimeStmp, 16, "%y%m%d%H%M%S%Z", localtime(&Time));
		//printf("%s\n",TimeStmp);
		//cout << Time << endl;
		//system("time");

		//���ı���
		if (lang) {
			strcpy(EncodeMethd, UCSEncodeMethd);
			cout << "���������:" << endl;
			cin >> ChnSrcStr;
			cout << "������Ϣ:" << endl;
			printf("��������:%s\n", LocNum);
			printf("�Է�����:%s\n", DestNum);
			printf("��������:%s\n", ChnSrcStr);

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
			printf("UCS-2������:\n");
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

			printf("��������Ϊ��\n");
			printf("%s", header);
			printf("%s", sendAddr);
			printf("%s", recvAddr);
			printf("%s", PDU);
			printf("%s\n", checksum);

			//��PDU�н���
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
			printf("UCS-2������:\n");
			//for (int i = 0; i < SrcLen; i++) {
			//	printf("%c \n", *UCSDecodeTemp);
			//	UCSDecodeTemp++;
			printf("%s\n", UCSDecodeDest);
		}

		//Ӣ�ı���
		else {
			strcpy(EncodeMethd, sBitEncodeMethd);
			cout << "���������(�ո���/�Ŵ���):" << endl; //���cin�ѿո���Ϊ��ֹ��֮һ�Ľ����ʩ
			cin >> EngSrcStr;
			for (int i = 0; EngSrcStr[i] != '\0'; i++) {
				if (EngSrcStr[i] == '/') {
					EngSrcStr[i] = ' ';
				}
			}
			cout << "������Ϣ:" << endl;
			printf("��������:%s\n", LocNum);
			printf("�Է�����:%s\n", DestNum);
			printf("��������:%s\n", EngSrcStr);

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
			printf("7-bit������:\n");
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

			printf("��������Ϊ��\n");
			printf("%s", header);
			printf("%s", sendAddr);
			printf("%s", recvAddr);
			printf("%s", PDU);
			printf("%s\n", checksum);

			//��PDU�н���
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
			printf("7-bit������:\n");
			printf("%s\n", sBitDecodeDest);
		}
		
		/*-------------------------����Ϊ����PDU�����ȫ����--------------------------*/
		/*---------------------------���¿�ʼ���ݿⲿ�ֲ���---------------------------*/
		
		/*-------------------------���ݿ��ѯ-----------------------------------------*/
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
			cout << "��ѯ���ݿ���:" << ((cmpflg == 1) ? "�ɹ�" : "ʧ�ܣ����ݿ��в����ڴ˱�������") << endl;
			if (cmpflg) {
				//flg = var.getvalues(remains);
				cout << "���Ʋ�ѯ���:" << ((isGetValue == 1) ? "�ɹ�" : "ʧ��") << endl;
				cout << "�绰����" << "        " << "����" << "     " << "�ײ�" << "  " << "�������" << endl;
				cout << Num << "     " << Name << "  " << discount << "    " << remains << endl;
				//cout << "���:" << remains << endl;
				if (remains <= 0) {
					cout << "���㣡" << endl;
				}
				else {
					remains = remains - 0.1;
					sprintf(Remains, "%f", remains);
					strcat(remainsHead, Remains);
					strcat(remainsHead, SinQuo);
					UpdtSet = remainsHead;
					isUpdate = var.update(table, UpdtSet, cond);
					cout << "�������ݿ���:" << ((isUpdate == 1) ? "�ɹ�" : "ʧ��") << endl;
					isSelect = var.select(table, distinct, col, cond, order);
					cout << "��ѯ���ݿ���:" << ((isSelect == 1) ? "�ɹ�" : "ʧ��") << endl;
					isGetValue = var.getvalues(Num, Name, discount, remains);
					cout << "���Ʋ�ѯ���:" << ((isGetValue == 1) ? "�ɹ�" : "ʧ��") << endl;
					cout << "�绰����" << "        " << "����" << "     " << "�ײ�" << "  " << "�������" << endl;
					cout << Num << "  " << Name << "  " << discount << "    " << remains << endl;
				}
			}
			else {
				cout << "���������������Ϣ" << endl;
				break;
			}

			//cout << Num << "  " << Name << "  " << discount << "    " << remains << endl;
			/*-------------------------���ݿ����-----------------------------------------*/
			//InsrtValue = "'18692807366','Admin','2',1000";
			//flg = var.insert(table, InsrtValue);
			//cout << "�������ݿ���:" << ((flg == 1) ? "�ɹ�" : "ʧ��") << endl;
			/*col = "*";
			cond = "Num = '18692807366'";
			flg = var.select(table, distinct, col, cond, order);
			cout << "��ѯ���ݿ���:" << ((flg == 1) ? "�ɹ�" : "ʧ��") << endl;
			flg = var.getvalues(Num, Name, discount, remains);
			cout << "���Ʋ�ѯ���:" << ((flg == 1) ? "�ɹ�" : "ʧ��") << endl;
			cout << "�绰����" << "        " << "����" << "     " << "�ײ�" << "  " << "�������" << endl;
			cout << Num << "  " << Name << "  " << discount << "    " << remains << endl;
			UpdtSet = "Name = 'Harold'";
			cond = "Num = '15580373586'";
			flg = var.update(table, UpdtSet, cond);
			cout << "�������ݿ���:" << ((flg == 1) ? "�ɹ�" : "ʧ��") << endl;*/
			//UpdtSet = "Name = 'admin'";
			//cond = "Num = '18692807366'";
			//flg = var.update(table, UpdtSet, cond);
			//cout << "�������ݿ���:" << (( flg==1 )? "�ɹ�":"ʧ��") << endl;
			//cond = "Num = '15580373586'";
			/*flg = var.select(table, distinct, col, cond, order);
			cout << "��ѯ���ݿ���:" << ((flg == 1) ? "�ɹ�" : "ʧ��") << endl;
			flg = var.getvalues(Num, Name, discount, remains);
			cout << "���Ʋ�ѯ���:" << ((flg == 1) ? "�ɹ�" : "ʧ��") << endl;
			cout << "�绰����" << "        " << "����" << "     " << "�ײ�" << "  " << "�������" << endl;
			cout << Num << "  " << Name << "  " << discount << "    " << remains << endl;
			//system("pause");*/
		} while (isSelect == 0);
		PDU[0] = '\0';
		cout << "�Ƿ�����������(�ǣ�1/��0)��" << endl;
		cin >> ctn;
	} while (ctn == 1);
	//system("pause");
}
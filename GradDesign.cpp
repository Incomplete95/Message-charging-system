#include <string>
#include <iostream>
//#include <stdafx>
#include "windows.h"
//#include <winnls>
using namespace std;



/*--------------------------------------------------------------*/
// UCS2����
// ����: pSrc - Դ�ַ���ָ��
//       nSrcLength - Դ�ַ�������
// ���: pDst - Ŀ����봮ָ��
// ����: Ŀ����봮����
/*----------------------------------------------------------------*/
int gsmEncodeUcs2(const char* pSrc, unsigned char* pDst, int nSrcLength)
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
int gsmDecodeUcs2(const unsigned char* pSrc, char* pDst, int nSrcLength)
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
	nDstLength = WideCharToMultiByte(CP_ACP, 0, wchar, nSrcLength / 2, pDst, 160, NULL, NULL);
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
int   gsmEncode7bit(const   char*   pSrc, unsigned   char*   pDst, int   nSrcLength)
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
	while (nSrc   <   nSrcLength)
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
int   gsmDecode7bit(const   unsigned   char*   pSrc, char*   pDst, int   nSrcLength)
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
	//*pDst = "\0";
	//   ����Ŀ�괮����
	return   nDst;
}

int main()
{

}

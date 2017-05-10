#include <string>
#include <iostream>
//#include <stdafx>
#include "windows.h"
//#include <winnls>
using namespace std;



/*--------------------------------------------------------------*/
// UCS2编码
// 输入: pSrc - 源字符串指针
//       nSrcLength - 源字符串长度
// 输出: pDst - 目标编码串指针
// 返回: 目标编码串长度
/*----------------------------------------------------------------*/
int gsmEncodeUcs2(const char* pSrc, unsigned char* pDst, int nSrcLength)
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
int gsmDecodeUcs2(const unsigned char* pSrc, char* pDst, int nSrcLength)
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
	nDstLength = WideCharToMultiByte(CP_ACP, 0, wchar, nSrcLength / 2, pDst, 160, NULL, NULL);
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
int   gsmEncode7bit(const   char*   pSrc, unsigned   char*   pDst, int   nSrcLength)
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
	while (nSrc   <   nSrcLength)
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
int   gsmDecode7bit(const   unsigned   char*   pSrc, char*   pDst, int   nSrcLength)
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
	//*pDst = "\0";
	//   返回目标串长度
	return   nDst;
}

int main()
{

}

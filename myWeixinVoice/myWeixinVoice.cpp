// myWeixinVoice.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>
#include <tchar.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <Windows.h>

#include <atlconv.h>
#include "../include/qisr.h"

#include "../include/cencode.h"
#include "../include/cdecode.h"

#ifdef _WIN64
#pragma comment(lib,"../lib/msc_x64.lib")//x64
#else
#pragma comment(lib,"../lib/msc.lib")//x86
#endif


/* arbitrary buffer size */
 #define SIZE 200
 char* encode(const char* input)
 {
 	/* set up a destination buffer large enough to hold the encoded data */
 	char* output = (char*)malloc(SIZE);
 	/* keep track of our encoded position */
 	char* c = output;
 	/* store the number of bytes encoded by a single call */
 	int cnt = 0;
 	/* we need an encoder state */
 	base64_encodestate s;
 	
 	/*---------- START ENCODING ----------*/
 	/* initialise the encoder state */
 	base64_init_encodestate(&s);
 	/* gather data from the input and send it to the output */
 	cnt = base64_encode_block(input, strlen(input), c, &s);
 	c += cnt;
 	/* since we have encoded the entire input string, we know that 
 	   there is no more input data; finalise the encoding */
 	cnt = base64_encode_blockend(c, &s);
 	c += cnt;
 	/*---------- STOP ENCODING  ----------*/
 	
 	/* we want to print the encoded data, so null-terminate it: */
 	*c = 0;
 	
 	return output;
 }

 char* decode(const char* input)
 {
 	/* set up a destination buffer large enough to hold the encoded data */
 	char* output = (char*)malloc(SIZE);
 	/* keep track of our decoded position */
 	char* c = output;
 	/* store the number of bytes decoded by a single call */
 	int cnt = 0;
 	/* we need a decoder state */
 	base64_decodestate s;
 	
 	/*---------- START DECODING ----------*/
 	/* initialise the decoder state */
 	base64_init_decodestate(&s);
 	/* decode the input data */
 	cnt = base64_decode_block(input, strlen(input), c, &s);
 	c += cnt;
 	/* note: there is no base64_decode_blockend! */
 	/*---------- STOP DECODING  ----------*/
 	
 	/* we want to print the decoded data, so null-terminate it: */
 	*c = 0;
 	
 	return output;
 }



const char* get_grammar( const char* filename );//获取语法
void release_grammar(const char** grammar);//释放语法占用的空间
int run_asr(const char* grammar , const char* asrfile);//测试识别效果

BOOL UrlEncode(const char* szSrc, char* pBuf, int cbBufLen, BOOL bUpperCase);
BOOL UrlDecode(const char* szSrc, char* pBuf, int cbBufLen);

char* ConvertGb2312ToUTF8(const char *pcGb2312)       
{       
	int nUnicodeLen = MultiByteToWideChar(CP_ACP, 0, pcGb2312, -1, NULL, 0);

	wchar_t * pcUnicode = new wchar_t[nUnicodeLen+1]; 
	memset(pcUnicode, 0, nUnicodeLen * 2 + 2); 

	MultiByteToWideChar(CP_ACP, 0, pcGb2312, -1, pcUnicode, nUnicodeLen);

	//unicode to utf8

	int nUtf8Len = WideCharToMultiByte(CP_UTF8, 0, pcUnicode, -1, NULL, 0, NULL, NULL);

	char *pcUtf8=new char[nUtf8Len + 1]; 
	memset(pcUtf8, 0, nUtf8Len + 1);   

	WideCharToMultiByte(CP_UTF8, 0, pcUnicode, -1, pcUtf8, nUtf8Len, NULL, NULL);

	///delete[] pcUtf8; 
	delete[] pcUnicode; 
	return pcUtf8;
}

const int BUFFER_NUM = 4096;
const int MAX_KEYWORD_LEN = 4096;

int _tmain(int argc, _TCHAR* argv[])
{
	//2个参数的时候就是 不需要 base64 加密，
	//3个参数的时候就是 base64 加密过的传参,而且在base64加密前，这个字符串要转换为GBK2312
	if (argc == 2 || argc == 3){}
	else
		return -1;

	const char* grammar = NULL;
	const char* asrfile;// = get_audio_file();
	const char* outstr;
	BSTR unicodestr;

	USES_CONVERSION;
	if (argc == 2)
	{
		asrfile = T2A(argv[1]);
		//printf("2:%s\n", asrfile);
	}
	else if (argc == 3)
	{
		outstr = W2A(argv[1]);
		asrfile = decode(outstr);
		//printf("3:%s\n", asrfile);
	}
	
	int ret = MSP_SUCCESS;
	//appid 请勿随意改动
	ret = QISRInit("appid=");
	if(ret != MSP_SUCCESS)
	{
	//	printf("QISRInit with errorCode: %d \n", ret);
		return -1;
	}
//	printf("QISRInit over\n");

	char szAppPath[MAX_PATH]; 
	GetModuleFileNameA(NULL, szAppPath, MAX_PATH);
	(strrchr(szAppPath, '\\'))[1] = 0;

	sprintf(szAppPath, "%sgm_continuous_mytv.abnf",szAppPath);

	grammar = get_grammar(szAppPath);// gm_continuous_digit
	if(ret != MSP_SUCCESS)
	{
	//	printf("getExID with errorCode: %d \n", ret);
		return -1;
	}

	ret = run_asr(grammar , asrfile);

	release_grammar(&grammar);

	QISRFini();
	//char key = _getch();
	return 111;
}

const char* get_grammar( const char* filename )
{
	int ret = MSP_SUCCESS;
	int file_len = 0;
	char* grammar = NULL;
	FILE *fp=NULL; 
	fp=fopen(filename,"rb");
	if (NULL == fp)
	{
	//	printf("get_grammar| open file \"%s\" failed.\n",filename ? filename : "");
		return NULL;
	}
	fseek(fp,0L,SEEK_END);
	file_len=ftell(fp);
	fseek(fp,0L,SEEK_SET);

	grammar = (char*)malloc(file_len+1); //从文件中读取语法，注意文本编码为GB2312
	fread((void*)grammar,1,file_len,fp);
	grammar[file_len]='\0'; //字符串收尾
	fclose(fp);
	return grammar;
}

void release_grammar(const char** grammar)
{
	if (*grammar)
	{
		free((void*)*grammar);
		*grammar = NULL;
	}	
}

int run_asr(const char* grammar , const char* infile)
{
	int ret = MSP_SUCCESS;
	int i = 0;
	FILE* fp = NULL;
	char buff[BUFFER_NUM];
	unsigned int len;
	int status = MSP_AUDIO_SAMPLE_CONTINUE, ep_status = -1, rec_status = -1, rslt_status = -1;

	const char* param = "rst=plain,sub=asr,ssm=1,aue=speex,auf=audio/L16;rate=16000,grt=abnf";//注意sub=asr,grt=abnf
	const char* sess_id = QISRSessionBegin(grammar, param, &ret);//将语法传入QISRSessionBegin
	if ( MSP_SUCCESS != ret )
	{
		printf("QISRSessionBegin err %d\n", ret);	
		return ret;
	}

	//printf("%s\n",asrfile);
	fp = fopen( infile , "rb");//我们提供了几个音频文件，测试时根据需要在这里更换
	if ( NULL == fp )
	{
		printf("failed to open file,please check the file.\n");
		QISRSessionEnd(sess_id, "normal");
		return -1;
	}

//	printf("writing audio...\n");
	while ( !feof(fp) )
	{
		len = (unsigned int)fread(buff, 1, BUFFER_NUM, fp);
		feof(fp) ? status = MSP_AUDIO_SAMPLE_LAST : status = MSP_AUDIO_SAMPLE_CONTINUE;//最后一块音频要使用last
		ret = QISRAudioWrite(sess_id, buff, len, status, &ep_status, &rec_status);
		if ( ret != MSP_SUCCESS )
		{
			printf("QISRAudioWrite err %d\n", ret);
			break;
		}

		if ( rec_status == MSP_REC_STATUS_SUCCESS )
		{
			const char* result = QISRGetResult(sess_id, &rslt_status, 0, &ret);
			if (ret != MSP_SUCCESS )
			{
				printf("error code: %d\n", ret);
				break;
			}
//  			else if( rslt_status == MSP_REC_STATUS_NO_MATCH )
//  				printf("get result nomatch\n");
//   			else
//   			{
//     			if ( result != NULL )
//     				printf("get result[%d/%d]:len:%d\n %s\n", ret, rslt_status,strlen(result), result);
//   			}
		}
	//	printf(".");
		Sleep(200);//因为是模拟录音，为了避免数据发送太快造成缓冲区溢出，所以这里暂停200ms，如果是实时录音，不必暂停
	}
//	printf("\n");

	if (ret == MSP_SUCCESS)
	{	
	//	printf("get reuslt\n");
		char asr_result[1024] = "";
		char out_result[1024] = "";
		unsigned int pos_of_result = 0;
		int loop_count = 0;
		do 
		{
			const char* result = QISRGetResult(sess_id, &rslt_status, 0, &ret);
			if ( ret != 0 )
			{
				printf("QISRGetResult err %d\n", ret);
				break;
			}

			if( rslt_status == MSP_REC_STATUS_NO_MATCH )
			{
				printf("get result nomatch\n");
			}
			else if ( result != NULL )
			{
			//	printf("[%d]:get result[%d/%d]: %s\n", (loop_count), ret, rslt_status, result);
				strcpy(asr_result+pos_of_result,result);
			//	strcpy(out_result, result);
				pos_of_result += (unsigned int)strlen(result);
			}
			else
			{
			//	printf("[%d]:get result[%d/%d]\n",(loop_count), ret, rslt_status);
			}
			Sleep(500);
		} while (rslt_status != MSP_REC_STATUS_COMPLETE && loop_count++ < 30);
		if (strcmp(asr_result,"")==0)
		{
			printf("no result\n");
		}
		else
		{
			//不进行加密，传输可能会中断，也就传递失败。
			UrlEncode(asr_result, out_result, sizeof(out_result), FALSE);
			printf("%s\n", out_result);
		}

	}
	else
		printf("ret != MSP_SUCCESS\n");

	QISRSessionEnd(sess_id, NULL);
	//printf("QISRSessionEnd.\n");
	fclose(fp); 

	return 0;
}



//百分号编码
////http://zh.wikipedia.org/zh-cn/%E7%99%BE%E5%88%86%E5%8F%B7%E7%BC%96%E7%A0%81
BOOL UrlEncode(const char* szSrc, char* pBuf, int cbBufLen, BOOL bUpperCase)
{
	if(szSrc == NULL || pBuf == NULL || cbBufLen <= 0)
		return FALSE;

	size_t len_ascii = strlen(szSrc);
	if(len_ascii == 0)
	{
		pBuf[0] = 0;
		return TRUE;
	}

	//先转换到UTF-8
	char baseChar = bUpperCase ? 'A' : 'a';
	int cchWideChar = MultiByteToWideChar(CP_ACP, 0, szSrc, len_ascii, NULL, 0);
	LPWSTR pUnicode = (LPWSTR)malloc((cchWideChar + 1) * sizeof(WCHAR));
	if(pUnicode == NULL)
		return FALSE;
	MultiByteToWideChar(CP_ACP, 0, szSrc, len_ascii, pUnicode, cchWideChar + 1);

	int cbUTF8 = WideCharToMultiByte(CP_UTF8, 0, pUnicode, cchWideChar, NULL, 0, NULL, NULL);
	LPSTR pUTF8 = (LPSTR)malloc((cbUTF8 + 1) * sizeof(CHAR));
	if(pUTF8 == NULL)
	{
		free(pUnicode);
		return FALSE;
	}
	WideCharToMultiByte(CP_UTF8, 0, pUnicode, cchWideChar, pUTF8, cbUTF8 + 1, NULL, NULL);
	pUTF8[cbUTF8] = '\0';

	unsigned char c;
	int cbDest = 0; //累加
	unsigned char *pSrc = (unsigned char*)pUTF8;
	unsigned char *pDest = (unsigned char*)pBuf;
	while(*pSrc && cbDest < cbBufLen - 1)
	{
		c = *pSrc;
		if(isalpha(c) || isdigit(c) || c == '-' || c == '.' || c == '~')
		{
			*pDest = c;
			++pDest;
			++cbDest;
		}
		else if(c == ' ')
		{
			*pDest = '+';
			++pDest;
			++cbDest;
		}
		else
		{
			//检查缓冲区大小是否够用？
			if(cbDest + 3 > cbBufLen - 1)
				break;
			pDest[0] = '%';
			pDest[1] = (c >= 0xA0) ? ((c >> 4) - 10 + baseChar) : ((c >> 4) + '0');
			pDest[2] = ((c & 0xF) >= 0xA)? ((c & 0xF) - 10 + baseChar) : ((c & 0xF) + '0');
			pDest += 3;
			cbDest += 3;
		}
		++pSrc;
	}
	//null-terminator
	*pDest = '\0';
	free(pUnicode);
	free(pUTF8);
	return TRUE;
}
//解码后是utf-8编码
BOOL UrlDecode(const char* szSrc, char* pBuf, int cbBufLen)
{
	if(szSrc == NULL || pBuf == NULL || cbBufLen <= 0)
		return FALSE;

	size_t len_ascii = strlen(szSrc);
	if(len_ascii == 0)
	{
		pBuf[0] = 0;
		return TRUE;
	}

	char *pUTF8 = (char*)malloc(len_ascii + 1);
	if(pUTF8 == NULL)
		return FALSE;

	int cbDest = 0; //累加
	unsigned char *pSrc = (unsigned char*)szSrc;
	unsigned char *pDest = (unsigned char*)pUTF8;
	while(*pSrc)
	{
		if(*pSrc == '%')
		{
			*pDest = 0;
			//高位
			if(pSrc[1] >= 'A' && pSrc[1] <= 'F')
				*pDest += (pSrc[1] - 'A' + 10) * 0x10;
			else if(pSrc[1] >= 'a' && pSrc[1] <= 'f')
				*pDest += (pSrc[1] - 'a' + 10) * 0x10;
			else
				*pDest += (pSrc[1] - '0') * 0x10;

			//低位
			if(pSrc[2] >= 'A' && pSrc[2] <= 'F')
				*pDest += (pSrc[2] - 'A' + 10);
			else if(pSrc[2] >= 'a' && pSrc[2] <= 'f')
				*pDest += (pSrc[2] - 'a' + 10);
			else
				*pDest += (pSrc[2] - '0');

			pSrc += 3;
		}
		else if(*pSrc == '+')
		{
			*pDest = ' ';
			++pSrc;
		}
		else
		{
			*pDest = *pSrc;
			++pSrc;
		}
		++pDest;
		++cbDest;
	}
	//null-terminator
	*pDest = '\0';
	++cbDest;

	int cchWideChar = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)pUTF8, cbDest, NULL, 0);
	LPWSTR pUnicode = (LPWSTR)malloc(cchWideChar * sizeof(WCHAR));
	if(pUnicode == NULL)
	{
		free(pUTF8);
		return FALSE;
	}
	MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)pUTF8, cbDest, pUnicode, cchWideChar);
	WideCharToMultiByte(CP_ACP, 0, pUnicode, cchWideChar, pBuf, cbBufLen, NULL, NULL);
	free(pUTF8);
	free(pUnicode);
	return TRUE;
}
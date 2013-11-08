#include "stdafx.h"

/**
* (C) Copyright 2009, asiainfo
* @version 	v1.0
* @author 	chenli
* @brief    base64����ͽ���
*
* history:
* <p>chenli 2009-02-17 1.0 build this moudle</p>
*/

#ifndef ___BASE64_H___
#define ___BASE64_H___


#include <string>

using namespace std;

class CBase64
{
public:
	CBase64();
	~CBase64();

	/*********************************************************
	* ����˵�������������ݽ���base64����
	* ����˵����[in]pIn		��Ҫ���б��������
				[in]uInLen  ����������ֽ���
				[out]strOut ����Ľ���base64����֮����ַ���
	* ����ֵ  ��true����ɹ�,falseʧ��
	* ��  ��  ��ChenLi
	* ��дʱ�䣺2009-02-17
	**********************************************************/
	bool static Encode(const unsigned char *pIn, unsigned long uInLen, string& strOut);

	/*********************************************************
	* ����˵�������������ݽ���base64����
	* ����˵����[in]pIn			��Ҫ���б��������
				[in]uInLen		����������ֽ���
				[out]pOut		����Ľ���base64����֮����ַ���
				[out]uOutLen	����Ľ���base64����֮����ַ�������
	* ����ֵ  ��true����ɹ�,falseʧ��
	* ��  ��  ��ChenLi
	* ��дʱ�䣺2009-02-17
	**********************************************************/
	bool static Encode(const unsigned char *pIn, unsigned long uInLen, unsigned char *pOut, unsigned long *uOutLen);
	
	/*********************************************************
	* ����˵�������������ݽ���base64����
	* ����˵����[in]strIn		��Ҫ���н��������
				[out]pOut		�������֮��Ľ�������
				[out]uOutLen	����Ľ���֮����ֽ�������
	* ����ֵ  ��true����ɹ�,falseʧ��
	* ��  ��  ��ChenLi
	* ��дʱ�䣺2009-02-17
	**********************************************************/
	bool static Decode(const string& strIn, unsigned char *pOut, unsigned long *uOutLen) ;

	/*********************************************************
	* ����˵�������������ݽ���base64����
	* ����˵����[in]strIn		��Ҫ���н��������
				[out]pOut		�������֮��Ľ�������
				[out]uOutLen	����Ľ���֮����ֽ�������
	* ����ֵ  ��true����ɹ�,falseʧ��
	* ��  ��  ��ChenLi
	* ��дʱ�䣺2009-02-17
	**********************************************************/
	bool static Decode(const unsigned char *pIn, unsigned long uInLen, unsigned char *pOut, unsigned long *uOutLen) ;
};

#endif // ___BASE64_H___

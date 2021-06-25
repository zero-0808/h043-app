#ifndef    _AES_H_
#define    _AES_H_
#include  "stdlib.h"
#include  "string.h"



/*******************************************************************************
* Function     : Aes_IV_key128bit_Encrypt
* Description  : AES����  16�ֽ�һ�����ݿ�
* Input Para   : 
-->IV_IN_OUT:��������  �������
-->State_IN_OUT:��������  �������
-->key128bit:��Կ  128bit  16�ֽ�
* Output Para  :
-->IV_IN_OUT:��������  �������
-->State_IN_OUT:��������  �������
* Return Value : NULL
*******************************************************************************/
void Aes_key128bit_Encrypt(unsigned char *State_IN_OUT, unsigned char *key128bit);


/*******************************************************************************
* Function     : Aes_IV_key128bit_Decode
* Description  : AES����  16�ֽ�һ�����ݿ�
* Input Para   : 
-->IV_IN_OUT:��������  ԭ�������
-->State_IN_OUT:��������  �������
-->key128bit:��Կ  128bit  16�ֽ�
* Output Para  :
-->IV_IN_OUT:��������  ԭ�������
-->State_IN_OUT:��������  �������
* Return Value : NULL
*******************************************************************************/
void Aes_key128bit_Decrypt( unsigned char *State_IN_OUT, unsigned char *key128bit);

/*******************************************************************************
* Function     : Aes_IV_key192bit_Encrypt
* Description  : AES����  16�ֽ�һ�����ݿ�
* Input Para   : 
-->IV_IN_OUT:��������  �������
-->State_IN_OUT:��������  �������
-->key128bit:��Կ  192bit  24�ֽ�
* Output Para  :
-->IV_IN_OUT:��������  �������
-->State_IN_OUT:��������  �������
* Return Value : NULL
*******************************************************************************/
void Aes_key192bit_Encrypt(unsigned char *State_IN_OUT, unsigned char *key192bit);

/*******************************************************************************
* Function     : Aes_IV_key192bit_Decode
* Description  : AES����  16�ֽ�һ�����ݿ�
* Input Para   : 
-->IV_IN_OUT:��������  ԭ�������
-->State_IN_OUT:��������  �������
-->key128bit:��Կ  192bit  24�ֽ�
* Output Para  :
-->IV_IN_OUT:��������  ԭ�������
-->State_IN_OUT:��������  �������
* Return Value : NULL
*******************************************************************************/
void Aes_key192bit_Decrypt(unsigned char *State_IN_OUT, unsigned char *key192bit);


/*******************************************************************************
* Function     : Aes_IV_key192bit_Encrypt
* Description  : AES����  16�ֽ�һ�����ݿ�
* Input Para   : 
-->IV_IN_OUT:��������  �������
-->State_IN_OUT:��������  �������
-->key128bit:��Կ  256bit  32�ֽ�
* Output Para  :
-->IV_IN_OUT:��������  �������
-->State_IN_OUT:��������  �������
* Return Value : NULL
*******************************************************************************/
void Aes_key256bit_Encrypt( unsigned char *State_IN_OUT, unsigned char *key256bit);


/*******************************************************************************
* Function     : Aes_IV_key192bit_Decode
* Description  : AES����  16�ֽ�һ�����ݿ�
* Input Para   : 
-->IV_IN_OUT:��������  ԭ�������
-->State_IN_OUT:��������  �������
-->key128bit:��Կ  256bit  32�ֽ�
* Output Para  :
-->IV_IN_OUT:��������  ԭ�������
-->State_IN_OUT:��������  �������
* Return Value : NULL
*******************************************************************************/
void Aes_key256bit_Decrypt(unsigned char *State_IN_OUT, unsigned char *key256bit);



#endif




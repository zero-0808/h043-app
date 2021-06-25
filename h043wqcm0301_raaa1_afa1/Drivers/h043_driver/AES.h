#ifndef    _AES_H_
#define    _AES_H_
#include  "stdlib.h"
#include  "string.h"



/*******************************************************************************
* Function     : Aes_IV_key128bit_Encrypt
* Description  : AES加密  16字节一个数据块
* Input Para   : 
-->IV_IN_OUT:向量输入  密文输出
-->State_IN_OUT:明文输入  密文输出
-->key128bit:秘钥  128bit  16字节
* Output Para  :
-->IV_IN_OUT:向量输入  密文输出
-->State_IN_OUT:明文输入  密文输出
* Return Value : NULL
*******************************************************************************/
void Aes_key128bit_Encrypt(unsigned char *State_IN_OUT, unsigned char *key128bit);


/*******************************************************************************
* Function     : Aes_IV_key128bit_Decode
* Description  : AES解密  16字节一个数据块
* Input Para   : 
-->IV_IN_OUT:向量输入  原密文输出
-->State_IN_OUT:密文输入  明文输出
-->key128bit:秘钥  128bit  16字节
* Output Para  :
-->IV_IN_OUT:向量输入  原密文输出
-->State_IN_OUT:密文输入  明文输出
* Return Value : NULL
*******************************************************************************/
void Aes_key128bit_Decrypt( unsigned char *State_IN_OUT, unsigned char *key128bit);

/*******************************************************************************
* Function     : Aes_IV_key192bit_Encrypt
* Description  : AES加密  16字节一个数据块
* Input Para   : 
-->IV_IN_OUT:向量输入  密文输出
-->State_IN_OUT:明文输入  密文输出
-->key128bit:秘钥  192bit  24字节
* Output Para  :
-->IV_IN_OUT:向量输入  密文输出
-->State_IN_OUT:明文输入  密文输出
* Return Value : NULL
*******************************************************************************/
void Aes_key192bit_Encrypt(unsigned char *State_IN_OUT, unsigned char *key192bit);

/*******************************************************************************
* Function     : Aes_IV_key192bit_Decode
* Description  : AES解密  16字节一个数据块
* Input Para   : 
-->IV_IN_OUT:向量输入  原密文输出
-->State_IN_OUT:密文输入  明文输出
-->key128bit:秘钥  192bit  24字节
* Output Para  :
-->IV_IN_OUT:向量输入  原密文输出
-->State_IN_OUT:密文输入  明文输出
* Return Value : NULL
*******************************************************************************/
void Aes_key192bit_Decrypt(unsigned char *State_IN_OUT, unsigned char *key192bit);


/*******************************************************************************
* Function     : Aes_IV_key192bit_Encrypt
* Description  : AES加密  16字节一个数据块
* Input Para   : 
-->IV_IN_OUT:向量输入  密文输出
-->State_IN_OUT:明文输入  密文输出
-->key128bit:秘钥  256bit  32字节
* Output Para  :
-->IV_IN_OUT:向量输入  密文输出
-->State_IN_OUT:明文输入  密文输出
* Return Value : NULL
*******************************************************************************/
void Aes_key256bit_Encrypt( unsigned char *State_IN_OUT, unsigned char *key256bit);


/*******************************************************************************
* Function     : Aes_IV_key192bit_Decode
* Description  : AES解密  16字节一个数据块
* Input Para   : 
-->IV_IN_OUT:向量输入  原密文输出
-->State_IN_OUT:密文输入  明文输出
-->key128bit:秘钥  256bit  32字节
* Output Para  :
-->IV_IN_OUT:向量输入  原密文输出
-->State_IN_OUT:密文输入  明文输出
* Return Value : NULL
*******************************************************************************/
void Aes_key256bit_Decrypt(unsigned char *State_IN_OUT, unsigned char *key256bit);



#endif




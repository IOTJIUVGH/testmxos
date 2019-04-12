/**
******************************************************************************
* @file    rsa_test.c 
* @author  Jenny Liu
* @version V1.0.0
* @date    13-April-2016
* @brief   RSA Test Demo  based on MiCO !
******************************************************************************
*
*  The MIT License
*  Copyright (c) 2016 MXCHIP Inc.
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy 
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights 
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is furnished
*  to do so, subject to the following conditions:
*
*  The above copyright notice and this permission notice shall be included in
*  all copies or substantial portions of the Software.
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
*  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR 
*  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
******************************************************************************
*
*  Functional Discription of rsa_test.c
*  
*  Demo Function:
*  To encrypt and decrypt for string of input string through RSA algorithm.
*   
*  Encrypt input string and store into output string.
*  Decrypt output string and store into plain string.
*  Compare plain text with input string after decryption.
*  Print the comparision results through serial port log.
*
*  If the result is correct, display message such as "RSA Test passed!".
*  If the result is not correct,stop the process and display the error message 
*  such as "RSA Test Failed! The Wrong Number is 1!" 
*
*  Note:
*  Users can change the string valude of input to validate RSA algorithm function.

*********************************************************************************************/

#include "mico.h" 

#define FOURK_BUF 4096

#define rsa_test_log(format, ...)  custom_log("Security", format, ##__VA_ARGS__)

/*contain both private and public keys*/
const unsigned char client_key_der_1024[] =
{
	0x30, 0x82, 0x02, 0x5C, 0x02, 0x01, 0x00, 0x02, 0x81, 0x81, 
	0x00, 0xBC, 0x73, 0x0E, 0xA8, 0x49, 0xF3, 0x74, 0xA2, 0xA9, 
	0xEF, 0x18, 0xA5, 0xDA, 0x55, 0x99, 0x21, 0xF9, 0xC8, 0xEC, 
	0xB3, 0x6D, 0x48, 0xE5, 0x35, 0x35, 0x75, 0x77, 0x37, 0xEC, 
	0xD1, 0x61, 0x90, 0x5F, 0x3E, 0xD9, 0xE4, 0xD5, 0xDF, 0x94, 
	0xCA, 0xC1, 0xA9, 0xD7, 0x19, 0xDA, 0x86, 0xC9, 0xE8, 0x4D, 
	0xC4, 0x61, 0x36, 0x82, 0xFE, 0xAB, 0xAD, 0x7E, 0x77, 0x25, 
	0xBB, 0x8D, 0x11, 0xA5, 0xBC, 0x62, 0x3A, 0xA8, 0x38, 0xCC, 
	0x39, 0xA2, 0x04, 0x66, 0xB4, 0xF7, 0xF7, 0xF3, 0xAA, 0xDA, 
	0x4D, 0x02, 0x0E, 0xBB, 0x5E, 0x8D, 0x69, 0x48, 0xDC, 0x77, 
	0xC9, 0x28, 0x0E, 0x22, 0xE9, 0x6B, 0xA4, 0x26, 0xBA, 0x4C, 
	0xE8, 0xC1, 0xFD, 0x4A, 0x6F, 0x2B, 0x1F, 0xEF, 0x8A, 0xAE, 
	0xF6, 0x90, 0x62, 0xE5, 0x64, 0x1E, 0xEB, 0x2B, 0x3C, 0x67, 
	0xC8, 0xDC, 0x27, 0x00, 0xF6, 0x91, 0x68, 0x65, 0xA9, 0x02, 
	0x03, 0x01, 0x00, 0x01, 0x02, 0x81, 0x80, 0x13, 0x97, 0xEA, 
	0xE8, 0x38, 0x78, 0x25, 0xA2, 0x5C, 0x04, 0xCE, 0x0D, 0x40, 
	0x7C, 0x31, 0xE5, 0xC4, 0x70, 0xCD, 0x9B, 0x82, 0x3B, 0x58, 
	0x09, 0x86, 0x3B, 0x66, 0x5F, 0xDC, 0x31, 0x90, 0xF1, 0x4F, 
	0xD5, 0xDB, 0x15, 0xDD, 0xDE, 0xD7, 0x3B, 0x95, 0x93, 0x31, 
	0x18, 0x31, 0x0E, 0x5E, 0xA3, 0xD6, 0xA2, 0x1A, 0x71, 0x6E, 
	0x81, 0x48, 0x1C, 0x4B, 0xCF, 0xDB, 0x8E, 0x7A, 0x86, 0x61, 
	0x32, 0xDC, 0xFB, 0x55, 0xC1, 0x16, 0x6D, 0x27, 0x92, 0x24, 
	0x45, 0x8B, 0xF1, 0xB8, 0x48, 0xB1, 0x4B, 0x1D, 0xAC, 0xDE, 
	0xDA, 0xDD, 0x8E, 0x2F, 0xC2, 0x91, 0xFB, 0xA5, 0xA9, 0x6E, 
	0xF8, 0x3A, 0x6A, 0xF1, 0xFD, 0x50, 0x18, 0xEF, 0x9F, 0xE7, 
	0xC3, 0xCA, 0x78, 0xEA, 0x56, 0xD3, 0xD3, 0x72, 0x5B, 0x96, 
	0xDD, 0x4E, 0x06, 0x4E, 0x3A, 0xC3, 0xD9, 0xBE, 0x72, 0xB6, 
	0x65, 0x07, 0x07, 0x4C, 0x01, 0x02, 0x41, 0x00, 0xFA, 0x47, 
	0xD4, 0x7A, 0x7C, 0x92, 0x3C, 0x55, 0xEF, 0x81, 0xF0, 0x41, 
	0x30, 0x2D, 0xA3, 0xCF, 0x8F, 0x1C, 0xE6, 0x87, 0x27, 0x05, 
	0x70, 0x0D, 0xDF, 0x98, 0x35, 0xD6, 0xF1, 0x8B, 0x38, 0x2F, 
	0x24, 0xB5, 0xD0, 0x84, 0xB6, 0x79, 0x4F, 0x71, 0x29, 0x94, 
	0x5A, 0xF0, 0x64, 0x6A, 0xAC, 0xE7, 0x72, 0xC6, 0xED, 0x4D, 
	0x59, 0x98, 0x3E, 0x67, 0x3A, 0xF3, 0x74, 0x2C, 0xF9, 0x61, 
	0x17, 0x69, 0x02, 0x41, 0x00, 0xC0, 0xC1, 0x82, 0x0D, 0x0C, 
	0xEB, 0xC6, 0x2F, 0xDC, 0x92, 0xF9, 0x9D, 0x82, 0x1A, 0x31, 
	0xE9, 0xE9, 0xF7, 0x4B, 0xF2, 0x82, 0x87, 0x1C, 0xEE, 0x16, 
	0x6A, 0xD1, 0x1D, 0x18, 0x82, 0x70, 0xF3, 0xC0, 0xB6, 0x2F, 
	0xF6, 0xF3, 0xF7, 0x1D, 0xF1, 0x86, 0x23, 0xC8, 0x4E, 0xEB, 
	0x8F, 0x56, 0x8E, 0x8F, 0xF5, 0xBF, 0xF1, 0xF7, 0x2B, 0xB5, 
	0xCC, 0x3D, 0xC6, 0x57, 0x39, 0x0C, 0x1B, 0x54, 0x41, 0x02, 
	0x41, 0x00, 0x9D, 0x7E, 0x05, 0xDE, 0xED, 0xF4, 0xB7, 0xB2, 
	0xFB, 0xFC, 0x30, 0x4B, 0x55, 0x1D, 0xE3, 0x2F, 0x01, 0x47, 
	0x96, 0x69, 0x05, 0xCD, 0x0E, 0x2E, 0x2C, 0xBD, 0x83, 0x63, 
	0xB6, 0xAB, 0x7C, 0xB7, 0x6D, 0xCA, 0x5B, 0x64, 0xA7, 0xCE, 
	0xBE, 0x86, 0xDF, 0x3B, 0x53, 0xDE, 0x61, 0xD2, 0x1E, 0xEB, 
	0xA5, 0xF6, 0x37, 0xED, 0xAC, 0xAB, 0x78, 0xD9, 0x4C, 0xE7, 
	0x55, 0xFB, 0xD7, 0x11, 0x99, 0xC1, 0x02, 0x40, 0x18, 0x98, 
	0x18, 0x29, 0xE6, 0x1E, 0x27, 0x39, 0x70, 0x21, 0x68, 0xAC, 
	0x0A, 0x2F, 0xA1, 0x72, 0xC1, 0x21, 0x86, 0x95, 0x38, 0xC6, 
	0x58, 0x90, 0xA0, 0x57, 0x9C, 0xBA, 0xE3, 0xA7, 0xB1, 0x15, 
	0xC8, 0xDE, 0xF6, 0x1B, 0xC2, 0x61, 0x23, 0x76, 0xEF, 0xB0, 
	0x9D, 0x1C, 0x44, 0xBE, 0x13, 0x43, 0x39, 0x67, 0x17, 0xC8, 
	0x9D, 0xCA, 0xFB, 0xF5, 0x45, 0x64, 0x8B, 0x38, 0x82, 0x2C, 
	0xF2, 0x81, 0x02, 0x40, 0x39, 0x89, 0xE5, 0x9C, 0x19, 0x55, 
	0x30, 0xBA, 0xB7, 0x48, 0x8C, 0x48, 0x14, 0x0E, 0xF4, 0x9F, 
	0x7E, 0x77, 0x97, 0x43, 0xE1, 0xB4, 0x19, 0x35, 0x31, 0x23, 
	0x75, 0x9C, 0x3B, 0x44, 0xAD, 0x69, 0x12, 0x56, 0xEE, 0x00, 
	0x61, 0x64, 0x16, 0x66, 0xD3, 0x7C, 0x74, 0x2B, 0x15, 0xB4, 
	0xA2, 0xFE, 0xBF, 0x08, 0x6B, 0x1A, 0x5D, 0x3F, 0x90, 0x12, 
	0xB1, 0x05, 0x86, 0x31, 0x29, 0xDB, 0xD9, 0xE2
};

const int sizeof_client_key_der_1024 = sizeof(client_key_der_1024) ;

int  rsa_test(void);

int main(void)
{
    int ret = 0;
 
    rsa_test_log( "RSA Test Start\r\n" );
    
    if ( (ret = rsa_test()) != 0)        
        rsa_test_log("RSA Test Failed!  The Wrong Number is %d",ret);
    
    else
        rsa_test_log("RSA Test Passed!");
    return 0;

}

int rsa_test(void)
{
    byte*   tmp;
    size_t bytes;
    RsaKey key;
    CyaSSL_RNG rng;
    word32 idx = 0;
    int    ret;
    byte   input[] = "Everyone gets Friday off.";
    word32 inLen = (word32)strlen((char*)input);
    byte   output[256];
    byte   plain[256];

    tmp = (byte*)malloc(FOURK_BUF);
    if (tmp == NULL)
      return -40;
    
    memcpy(tmp, client_key_der_1024, sizeof_client_key_der_1024);
    bytes = sizeof_client_key_der_1024;
    
    wc_InitRsaKey(&key, 0);
    ret = wc_RsaPrivateKeyDecode(tmp, &idx, &key, (word32)bytes);
    if (ret != 0) return -41;
    
    ret = wc_InitRng(&rng);
    if (ret != 0) return -42;

    ret = wc_RsaPublicEncrypt(input, inLen, output, sizeof(output), &key, &rng);
    if (ret < 0) return -43;
    
    ret = wc_RsaPrivateDecrypt(output, ret, plain, sizeof(plain), &key);
    if (ret < 0) return -44;

    if (memcmp(plain, input, inLen)) return -45;

    ret = wc_RsaSSL_Sign(input, inLen, output, sizeof(output), &key, &rng);
    if (ret < 0) return -46;

    memset(plain, 0, sizeof(plain));
    ret = wc_RsaSSL_Verify(output, ret, plain, sizeof(plain), &key);
    if (ret < 0) return -47;

    if (memcmp(plain, input, ret)) return -48;
    
    wc_FreeRsaKey(&key);
    
    free(tmp);

    return 0;
}


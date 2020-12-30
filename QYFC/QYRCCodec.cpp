#include "QYRccodec.h"
#include <ctype.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

QYRCCodec::QYRCCodec()
{
}

QYRCCodec::~QYRCCodec()
{
}


int QYRCCodec::Str2HexBytes(char *str, int len, unsigned char hexBytes[], int *size)
{
	int array_len = 0;
	int isOdd = 0;
	if (len % 2 != 0) {
		isOdd = 1;
	}
	//    printf("isOdd: %d\n", isOdd);
	for (int i = 0; i < len; array_len++) {
		unsigned char b1, b2;
		if (isOdd && i == 0) {

			b1 = 0;
			b2 = toupper(str[i]);
			b2 = b2 <= '9' ? b2 - '0' : b2 - 'a' + 10;
			i += 1;
		}
		else {
			b1 = toupper(str[i]);
			b1 = b1 <= '9' ? b1 - '0' : b1 - 'A' + 10;
			b2 = toupper(str[i + 1]);
			b2 = b2 <= '9' ? b2 - '0' : b2 - 'A' + 10;
			i += 2;
		}

		hexBytes[array_len] = b1 * 16 + b2;

	}
	*size = (len + 1) / 2;

	return 0;
}

/*初始化函数*/
void QYRCCodec::RC4SetKey(unsigned char *rc_key, unsigned char *key, int keylen)
{
	int i, j;
	j = 0;
	unsigned char tmp;

	for (i = 0; i < 256; i++) {
		rc_key[i] = i;
	}

	for (i = 0; i < 256; i++) {
		j = (j + rc_key[i] + key[i % keylen]) % 256;

		tmp = rc_key[i];
		rc_key[i] = rc_key[j];
		rc_key[j] = tmp;
	}
}

/*加解密*/
void QYRCCodec::RC4Transform(unsigned char *rc_key, unsigned char *input, int len)
{
	int i, j, k;
	char tmp;
	unsigned char subkey;
	i = 0;
	j = 0;

	for (k = 0; k < len; k++) {
		i = (i + 1) % 256;
		j = (j + rc_key[i]) % 256;

		tmp = rc_key[i];
		rc_key[i] = rc_key[j];
		rc_key[j] = tmp;

		subkey = rc_key[(rc_key[i] + rc_key[j]) % 256];
		input[k] ^= subkey;
	}
}


int QYRCCodec::RC4Codec(char *data, char *out_data)
{
	int len = strlen(data);
	unsigned char hexBytes[16] = {};
	int size = 0;
	//printf("%s\n", data);
	Str2HexBytes(data, len, hexBytes, &size);

	unsigned char rc_encrypt_key[256];
//	unsigned char rc_decrypt_key[256];

	unsigned char rc_user_key[16] = { 0x17, 0x14, 0x11, 0x15, 0x12, 0x13, 0x7A, 0x1F, 0x95, 0x54, 0xC4, 0x05, 0xA8, 0x26, 0x8B, 0xE9 };

	RC4SetKey(rc_encrypt_key, rc_user_key, 16);
	RC4Transform(rc_encrypt_key, hexBytes, size);

	for (int j = 0; j < 8; j++)
	{
		sprintf_s(&out_data[j * 2], 2,"%02X", hexBytes[j]);
	}

	return 0;
}



/************************************************************/
//rc6codec

unsigned int QYRCCodec::MoveLeft(unsigned int x, unsigned int y)
{
	unsigned int i, temp;
	if (y / 32 != 0)
		y = y % 32;
	for (i = 0; i < y; i++)
	{
		temp = x & 0x80000000;
		x = x << 1;
		x = x + ((temp >> 31) & 0x00000001);
	}
	return x;
}

unsigned int QYRCCodec::MoveRight(unsigned int x, unsigned int y)
{
	unsigned int i, temp;
	if (y / 32 != 0)
		y = y % 32;
	for (i = 0; i < y; i++)
	{
		temp = x & 0x00000001;
		x = x >> 1;
		x = (x & 0x7fffffff) + (temp << 31);
	}
	return x;
}

unsigned int b = 16;
unsigned int c = 4;

void QYRCCodec::KeyExtEnd(unsigned int *s, unsigned int *l)
{
	unsigned int p = 0xb7e15163;
	unsigned int q = 0x9e3779b9;

	unsigned int i, j, k, a, b;
	s[0] = p;
	for (i = 1; i <= 43; i++)
		s[i] = s[i - 1] + q;
	a = 0;
	b = 0;
	i = 0;
	j = 0;

	for (k = 1; k <= 132; k++)
	{
		s[i] = s[i] + a + b;
		s[i] = MoveLeft(s[i], 3);
		a = s[i];
		l[j] = l[j] + a + b;
		l[j] = MoveLeft(l[j], (a + b));
		b = l[j];
		i = (i + 1) % 44;
		j = (j + 1) % 4;
	}
}

void QYRCCodec::Encryp(unsigned int *A, unsigned int *B, unsigned int *C, unsigned int *D, unsigned int *S)
{
	unsigned int r = 20;
	unsigned int w = 32;

	unsigned int i, j, t, u, temp;
	*B = *B + S[0];
	*D = *D + S[1];
	j = (unsigned int)(log((double)w) / log((double)2));
	for (i = 1; i <= r; i++)
	{
		t = MoveLeft(((*B)*(2 * (*B) + 1)), j);
		u = MoveLeft(((*D)*(2 * (*D) + 1)), j);
		temp = MoveLeft(((*A) ^ t), u);
		*A = temp + S[2 * i];
		temp = MoveLeft(((*C) ^ u), t);
		*C = temp + S[2 * i + 1];
		temp = *A;
		*A = *B;
		*B = *C;
		*C = *D;
		*D = temp;
	}
	*A = *A + S[2 * r + 2];
	*C = *C + S[2 * r + 3];
}

void QYRCCodec::Decryp(unsigned int *A, unsigned int *B, unsigned int *C, unsigned int *D, unsigned int *S)
{
	unsigned int r = 20;
	unsigned int w = 32;

	unsigned int i, j, u, t, temp;
	j = (unsigned int)(log((double)w) / log((double)2));
	*C = *C - S[2 * r + 3];
	*A = *A - S[2 * r + 2];
	for (i = r; i >= 1; i--)
	{
		temp = *D;
		*D = *C;
		*C = *B;
		*B = *A;
		*A = temp;
		u = MoveLeft(((*D)*(2 * (*D) + 1)), j);
		t = MoveLeft(((*B)*(2 * (*B) + 1)), j);
		temp = MoveRight(((*C) - S[2 * i + 1]), t);
		*C = temp^u;
		temp = MoveRight(((*A) - S[2 * i]), u);
		*A = temp^t;
	}
	*D = *D - S[1];
	*B = *B - S[0];
}

int QYRCCodec::GetRC6Key(char *src, char *key)
{
	unsigned int A, B, C, D, S[2 * 20 + 4], L[4] = { 0 };

	unsigned int i, j, temp;
//	char g;

	j = 0;
	for (i = 0; i < 16; i++)
	{
		temp = (unsigned int)key[i];
		if (i % 4 == 3)
			temp = temp << 24;
		else if (i % 4 == 2)
			temp = temp << 16;
		else if (i % 4 == 1)
			temp = temp << 8;
		L[j] = L[j] + temp;
		if (i % 4 == 3)
			j++;
	}

	/*明文处理成十六进制*/
	A = ((unsigned int)src[0] << 24) + ((unsigned int)src[1] << 16) + ((unsigned int)src[2] << 8) + (unsigned int)src[3];
	B = ((unsigned int)src[4] << 24) + ((unsigned int)src[5] << 16) + ((unsigned int)src[6] << 8) + (unsigned int)src[7];
	C = ((unsigned int)src[8] << 24) + ((unsigned int)src[9] << 16) + ((unsigned int)src[10] << 8) + (unsigned int)src[11];
	D = ((unsigned int)src[12] << 24) + ((unsigned int)src[13] << 16) + ((unsigned int)src[14] << 8) + (unsigned int)src[15];

	KeyExtEnd(S, L);

	Encryp(&A, &B, &C, &D, S);

	key[0] = ((A & 0xff000000) >> 24) & 0xff;
	key[1] = ((A & 0xff0000) >> 16) & 0xff;
	key[2] = ((A & 0xff00) >> 8) & 0xff;
	key[3] = A & 0xff;
	key[4] = ((B & 0xff000000) >> 24) & 0xff;
	key[5] = ((B & 0xff0000) >> 16) & 0xff;
	key[6] = ((B & 0xff00) >> 8) & 0xff;
	key[7] = B & 0xff;
	key[8] = ((C & 0xff000000) >> 24) & 0xff;
	key[9] = ((C & 0xff0000) >> 16) & 0xff;
	key[10] = ((C & 0xff00) >> 8) & 0xff;
	key[11] = C & 0xff;
	key[12] = ((D & 0xff000000) >> 24) & 0xff;
	key[13] = ((D & 0xff0000) >> 16) & 0xff;
	key[14] = ((D & 0xff00) >> 8) & 0xff;
	key[15] = D & 0xff;

	return 0;
}

int QYRCCodec::RC6Encode( char *plaintxt, char *enctxt)
{
	unsigned char rc_user_key[16] = { 0x14, 0x14, 0x11, 0x13, 0x12, 0x13, 0x7B, 0x1F, 0x45, 0x54, 0xC4, 0x05, 0xA5, 0x26, 0x8A, 0x11 };
	unsigned int S[2 * 20 + 4] = { 0 }, L[4] = { 0 };

	unsigned int i, j, temp,A,B,C,D;
//	char g;

	j = 0;
	for (i = 0; i < 16; i++)
	{
		temp = (unsigned int)rc_user_key[i];
		if (i % 4 == 3)
			temp = temp << 24;
		else if (i % 4 == 2)
			temp = temp << 16;
		else if (i % 4 == 1)
			temp = temp << 8;
		L[j] = L[j] + temp;
		if (i % 4 == 3)
			j++;
	}
	/*明文处理成十六进制*/
	A = ((unsigned int)plaintxt[0] << 24) + ((unsigned int)plaintxt[1] << 16) + ((unsigned int)plaintxt[2] << 8) + (unsigned int)plaintxt[3];
	B = ((unsigned int)plaintxt[4] << 24) + ((unsigned int)plaintxt[5] << 16) + ((unsigned int)plaintxt[6] << 8) + (unsigned int)plaintxt[7];
	C = ((unsigned int)plaintxt[8] << 24) + ((unsigned int)plaintxt[9] << 16) + ((unsigned int)plaintxt[10] << 8) + (unsigned int)plaintxt[11];
	D = ((unsigned int)plaintxt[12] << 24) + ((unsigned int)plaintxt[13] << 16) + ((unsigned int)plaintxt[14] << 8) + (unsigned int)plaintxt[15];

	KeyExtEnd(S, L);

	Encryp(&A, &B, &C, &D, S);

	return 0;
}

int QYRCCodec::RC6Decode(char *enctxt, char *ciphertxt)
{
	unsigned char rc_user_key[16] = { 0x14, 0x14, 0x11, 0x13, 0x12, 0x13, 0x7B, 0x1F, 0x45, 0x54, 0xC4, 0x05, 0xA5, 0x26, 0x8A, 0x11 };
	unsigned int S[2 * 20 + 4] = { 0 }, L[4] = { 0 };

	unsigned int i, j, temp,A,B,C,D;
//	char g;

	unsigned int *nEncTxt  = (unsigned int*)enctxt;
	A = nEncTxt[0];
	B = nEncTxt[1];
	C = nEncTxt[2];
	D = nEncTxt[3];


	j = 0;
	for (i = 0; i < 16; i++)
	{
		temp = (unsigned int)rc_user_key[i];
		if (i % 4 == 3)
			temp = temp << 24;
		else if (i % 4 == 2)
			temp = temp << 16;
		else if (i % 4 == 1)
			temp = temp << 8;
		L[j] = L[j] + temp;
		if (i % 4 == 3)
			j++;
	}

	KeyExtEnd(S, L);

	Decryp(&A, &B, &C, &D, S);
	/*十六进制还原成明文*/
	ciphertxt[0] = ((A & 0xff000000) >> 24) & 0xff;
	ciphertxt[1] = ((A & 0xff0000) >> 16) & 0xff;
	ciphertxt[2] = ((A & 0xff00) >> 8) & 0xff;
	ciphertxt[3] = A & 0xff;
	ciphertxt[4] = ((B & 0xff000000) >> 24) & 0xff;
	ciphertxt[5] = ((B & 0xff0000) >> 16) & 0xff;
	ciphertxt[6] = ((B & 0xff00) >> 8) & 0xff;
	ciphertxt[7] = B & 0xff;
	ciphertxt[8] = ((C & 0xff000000) >> 24) & 0xff;
	ciphertxt[9] = ((C & 0xff0000) >> 16) & 0xff;
	ciphertxt[10] = ((C & 0xff00) >> 8) & 0xff;
	ciphertxt[11] = C & 0xff;
	ciphertxt[12] = ((D & 0xff000000) >> 24) & 0xff;
	ciphertxt[13] = ((D & 0xff0000) >> 16) & 0xff;
	ciphertxt[14] = ((D & 0xff00) >> 8) & 0xff;
	ciphertxt[15] = D & 0xff;

	return 0;
}

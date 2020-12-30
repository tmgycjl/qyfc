#ifndef  _RCCODEC_H_
#define  _RCCODEC_H_


#include "qydefine.h"

class QYUI_EXPORT_CLASS QYRCCodec
{
public:
	QYRCCodec();
	~QYRCCodec();
	int RC4Codec(char *data, char *out_data);
	int RC6Decode(char *enctxt, char *ciphertxt);
	int RC6Encode(char *plaintxt, char *enctxt);
private:
	int Str2HexBytes(char *str, int len, unsigned char hexBytes[], int *size);
	void RC4SetKey(unsigned char *rc_key, unsigned char *key, int keylen);
	void RC4Transform(unsigned char *rc_key, unsigned char *input, int len);
	int GetRC6Key(char *src, char *key);
	void Decryp(unsigned int *A, unsigned int *B, unsigned int *C, unsigned int *D, unsigned int *S);
	void Encryp(unsigned int *A, unsigned int *B, unsigned int *C, unsigned int *D, unsigned int *S);
	void KeyExtEnd(unsigned int *s, unsigned int *l);
	unsigned int MoveRight(unsigned int x, unsigned int y);
	unsigned int MoveLeft(unsigned int x, unsigned int y);
};




#endif
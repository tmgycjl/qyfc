
#pragma  once


#define IS_VALID(p)		(NULL != p)

#define SAFE_DELETE(p)		if(p != NULL){  delete p;	    p = NULL; }

#define SAFE_DELETE_ARRAY(p)		if(p != NULL){  delete[]p;	    p = NULL; }

#define SAFE_RELEASE(p)		if(p != NULL){	p->Release();   p = NULL; }

#define SAFE_CLOSE(p)		if(p != NULL){  ::CloseHandle(p); p = NULL; }

#define SAFE_CLOSE_FILE(f)	if ( f != NULL ){fclose(f);f=NULL;}

#define SAFE_DELETE_PTR(ptr)		if((ptr) != NULL){  delete[] (ptr);	    (ptr) = NULL; }

#define SAFE_CLOSESOCKET(s) if(s != INVALID_SOCKET){closesocket(s);s = INVALID_SOCKET;}

#define USE_VIRTUAL_MEMORY      0

#define MAX(a,b) (a) > (b)?(a):(b)

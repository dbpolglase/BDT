#ifndef __TPC_H
#define __TPC_H
#ifdef __cplusplus
extern "C" {
#endif
#include <limits.h>

/*----------------------------------------------------------------------
 * tipos de lenguaje C
 *--------------------------------------------------------------------*/
typedef enum {	TPC_CHAR   = 'c', TPC_UNS_CHAR  = 'C',
		TPC_SHORT  = 's', TPC_UNS_SHORT = 'S',
		TPC_INT    = 'i', TPC_UNS_INT	= 'I',
		TPC_LONG   = 'l', TPC_UNS_LONG  = 'L',
		TPC_FLOAT  = 'f', TPC_DOUBLE = 'd', 
                TPC_STRING = 't', TPC_REGISTRO = 'R'
		} TipoTPC;

#define TPC_MIN_CHAR		SCHAR_MIN
#define TPC_MAX_CHAR		SCHAR_MAX
#define TPC_MAX_UNS_CHAR	UCHAR_MAX
#define TPC_MIN_SHORT		SHRT_MIN
#define TPC_MAX_SHORT		SHRT_MAX
#define TPC_MAX_UNS_SHORT	USHRT_MAX
#define TPC_MIN_INT		INT_MIN
#define TPC_MAX_INT		INT_MAX
#define TPC_MAX_UNS_INT		UINT_MAX
#define TPC_MIN_LONG		LONG_MIN
#define TPC_MAX_LONG		LONG_MAX
#define TPC_MAX_UNS_LONG	ULONG_MAX

/* Ver si en 64b o 32b. */
#if ULONG_MAX==0xFFFFFFFFFFFFFFFFUL 
#define IS_64 1
#else
#define IS_64 0
#endif

#if IS_64
#define TPC_TAM_COD_CHAR	1
#define TPC_TAM_COD_UNS_CHAR	1
#define TPC_TAM_COD_SHORT	2
#define TPC_TAM_COD_UNS_SHORT	2
#define TPC_TAM_COD_INT		4
#define TPC_TAM_COD_UNS_INT	4
#define TPC_TAM_COD_LONG	8
#define TPC_TAM_COD_UNS_LONG	8
#define TPC_TAM_COD_FLOAT	4
#define TPC_TAM_COD_DOUBLE	8
#else
#define TPC_TAM_COD_CHAR	1
#define TPC_TAM_COD_UNS_CHAR	1
#define TPC_TAM_COD_SHORT	2
#define TPC_TAM_COD_UNS_SHORT	2
#define TPC_TAM_COD_INT		4
#define TPC_TAM_COD_UNS_INT	4
#define TPC_TAM_COD_LONG	4
#define TPC_TAM_COD_UNS_LONG	4
#define TPC_TAM_COD_FLOAT	4
#define TPC_TAM_COD_DOUBLE	8
#endif

void TPC_HazPortable(int portable);
char *TPC_Nombre(TipoTPC tipo);
char *TPC_FuncCodif(TipoTPC tipo);
char *TPC_FuncDecod(TipoTPC tipo);
int   TPC_TamanoCodif(TipoTPC tipo, int dimens);

int TPC_CodChar(char *pCaracter, int dimens, char *pBuff);
int TPC_DecChar(char *pCaracter, int dimens, char *pBuff);
int TPC_CodUnsChar(unsigned char *pByte, int dimens, char *pBuff);
int TPC_DecUnsChar(unsigned char *pByte, int dimens, char *pBuff);

int TPC_CodString(char *string, char *pBuff);
int TPC_DecString(char *string, char *pBuff);
int TPC_LonString(char *pBuff);

int TPC_CodShort(short *pNumShort, int dimens, char *pBuff);
int TPC_DecShort(short *pNumShort, int dimens, char *pBuff);
int TPC_CodUnsShort(unsigned short *pNumUnsShort, int dimens, char *pBuff);
int TPC_DecUnsShort(unsigned short *pNumUnsShort, int dimens, char *pBuff);

int TPC_CodInt(int *pNumInt, int dimens, char *pBuff);
int TPC_DecInt(int *pNumInt, int dimens, char *pBuff);
int TPC_CodUnsInt(unsigned int *pNumUns, int dimens, char *pBuff);
int TPC_DecUnsInt(unsigned int *pNumUns, int dimens, char *pBuff);

int TPC_CodLong(long *pNumLong, int dimens, char *pBuff);
int TPC_DecLong(long *pNumLong, int dimens, char *pBuff);
int TPC_CodUnsLong(unsigned long *pNumUnsLong, int dimens, char *pBuff);
int TPC_DecUnsLong(unsigned long *pNumUnsLong, int dimens, char *pBuff);

int TPC_CodFloat(float *pNumFloat, int dimens, char *pBuff);
int TPC_DecFloat(float *pNumFloat, int dimens, char *pBuff);

int TPC_CodDouble(double *pNumDouble, int dimens, char *pBuff);
int TPC_DecDouble(double *pNumDouble, int dimens, char *pBuff);

#ifdef __cplusplus
}
#endif
#endif /* __TPC_H */

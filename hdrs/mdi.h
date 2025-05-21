#ifndef __MDI_H
#define __MDI_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MDI_String(str)		   MDI__String(str, __FILE__, __LINE__)
#define MDI_EncString(ant, str)	   MDI__EncString(ant, str, __FILE__, __LINE__)
#define MDI_Buffer(lBuf)	   MDI__Buffer(lBuf, __FILE__, __LINE__)
#define MDI_ElemArray(arr, nEle, lEle)		\
			MDI__ElemArray(arr, nEle, lEle, __FILE__, __LINE__)
#define MDI_FicheroTexto(fich, pMem)		\
			MDI__Fichero(fich, pMem, NULL, __FILE__, __LINE__)
#define MDI_FicheroDatos(fich, pMem, pLon)	\
			MDI__Fichero(fich, pMem, pLon, __FILE__, __LINE__)

char *MDI__String(char *string, char *fich, int nLin);
char *MDI__EncString(char *anterior, char *string, char *fich, int nLin);
void *MDI__Buffer(int lBuffer, char *fich, int nLin);
void  MDI__ElemArray(void **pArray, int *numElem, int lonElem,
			char *fich, int nLin);
void MDI__Fichero(char *fichDat, char **pMemoria,
		  int *pLongitud, char *fich, int nLin);

#ifdef __cplusplus
}
#endif
#endif /* __MDI_H */

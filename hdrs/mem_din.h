
#ifndef __MDI_H
#define __MDI_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MALLOC(tam, func, para)		_MALLOC(tam, func, para, \
							__FILE__, __LINE__)
#define REALLOC(punt, tam, func, para)	_REALLOC(punt, tam, func, para, \
							__FILE__, __LINE__)

#define ELEM_ARRAY(arr, nEle, lEle, fun, para)	_ELEM_ARRAY(arr, nEle, lEle, \
						  fun, para, __FILE__, __LINE__)

void *_MALLOC(int tamano, char *nomFunc, char *txtPara, char *fich, int nLin);
void *_REALLOC(char *punt, int tamano, char *nomFunc,
		char *txtPara, char *fich, int nLin);
void _ELEM_ARRAY(void **pArray, int *pNumElem, int lonElem, char *nomFunc,
			char *txtPara, char *fich, int nLin);

#ifdef __cplusplus
}
#endif
#endif /* __MDI_H */

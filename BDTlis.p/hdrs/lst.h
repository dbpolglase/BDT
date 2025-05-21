#ifndef __LST_H
#define __LST_H
#ifdef __cplusplus
extern "C" {
#endif

#define FMT_REGISTRO	'R'
#define FMT_LINEA	'L'

void LST_ListarUnaTabla(char *pBase, char *nomTabla,
			char formato, int strComoBuf);
void LST_ListarTablas(char *pBase, char formato, int strComoBuf);

#ifdef __cplusplus
}
#endif
#endif /* __LST_H */

#ifndef __PPR_H
#define __PPR_H
#ifdef __cplusplus
extern "C" {
#endif

void  PPR_Directorio(char *path);
int   PPR_Definicion(char *txtPatr, char *txtSust);
char *PPR_PreprocFichero(char *fichOri);

#ifdef __cplusplus
}
#endif
#endif /* __PPR_H */

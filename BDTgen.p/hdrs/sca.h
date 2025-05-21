#ifndef __SCA_H
#define __SCA_H

#ifdef __cplusplus
extern "C" {
#endif

void SCA_Inicializar(char *fichero);
int  yylex(void);
void SCA_Finalizar(void);

#ifdef __cplusplus
}
#endif

#endif /* __SCA_H */

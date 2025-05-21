/*========= INI PRIVADO ==========*/

#ifndef __ERH_H
#define __ERH_H
#ifdef __cplusplus
extern "C" {
#endif

#include <err.h>

void ERH_Sintaxis(int numLinea, char *formatoErr, ...);
void ERH_NoFatal(char *formatoErr, ...);
void ERH_Fatal(char *formatoErr, ...);
void ERH_Interno(char *formatoErr, ...);
void ERH_SistOper(char *funcion, char *formatoErr, ...);
int  ERH_Errores(void);

#ifdef __cplusplus
}
#endif
#endif /* __ERH_H */

/*========= FIN PRIVADO ==========*/


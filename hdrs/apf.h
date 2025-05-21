/*========= INI PRIVADO ==========*/

#ifndef __APF_H
#define __APF_H
#ifdef __cplusplus
extern "C" {
#endif

void APF_DefArgumento(char letra, char *nombre, char *descrip, char *nomVal);
void APF_DefOpcion(char letra, char *nombre, char *descripcion);
void APF_SinDefiniciones();
void APF_SinVisualizar();
void APF_DefFichero(int numero, char *extens, char *descripcion);
void APF_DefDescripcion(char *texto);
char * APF_DirName(char * fplantilla);

void APF_Uso(char *formatoErr, ...);
void APF_Analizar(int argc, char *argv[]);

char  *APF_Argumento(char letraArg);
int    APF_Opcion(char letraOpc);
char  *APF_Fichero();
char **APF_LstFich();

#ifdef __cplusplus
}
#endif
/*========= FIN PRIVADO ==========*/

#endif /* __APF_H */

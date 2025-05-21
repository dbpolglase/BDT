#ifndef __FIM_H
#define __FIM_H
#ifdef __cplusplus
extern "C" {
#endif

#define ELEM_OCUPADO	'O'
#define ELEM_LIBRE	'L'
#define EXTENS_IMAGEN	"ibd"

void  FIM_GenerarBase();
char *FIM_LeerBase(char *fichero);
char *FIM_ObtenerBase();
void  FIM_EscribirBase(char *fichero);

#ifdef __cplusplus
}
#endif
#endif /* __FIM_H */

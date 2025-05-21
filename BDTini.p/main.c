#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <apf.h>
#include <err.h>
#include <fim.h>
#include <sca.h>

extern int yyparse();

/*==================================================================== *
 *			PROGRAMA PRINCIPAL
 *====================================================================*/
int main(int argc, char *argv[])
{

	char *fichEnt;
	char *fichSal;

/*--------------------------------------------------------------------
 | Configuracion del interfaz de argumentos de la herramienta.
 +--------------------------------------------------------------------*/
APF_SinVisualizar();
APF_DefFichero(1, EXTENS_IMAGEN, "imagen file BDT");
APF_DefDescripcion("Load records in BDT BBDD ");
APF_DefDescripcion("\tMake the operations (INSERT/DELETE/MODIFY) of the ");
APF_DefDescripcion("\"record \" in the record file over the BDT BBDD ");
APF_DefArgumento('f', "file", "file with the records to load ",
		 "name of the file");
APF_DefArgumento('s', "output", " new image file BDT (if not present overwrite)",
		 "name of the new BDT image ");

/*--------------------------------------------------------------------
 | Analisis de los argumentos de llamada
 +--------------------------------------------------------------------*/
APF_Analizar(argc, argv);

/*--------------------------------------------------------------------
 |
 +--------------------------------------------------------------------*/
if (!APF_Opcion('f'))
    APF_Uso("falta fichero de registros a tratar (-f)");

/*--------------------------------------------------------------------
 | El fichero de registros a tratar es obligatorio
 +--------------------------------------------------------------------*/
fichEnt = APF_Fichero();
if (APF_Opcion('s'))
    fichSal = APF_Argumento('s');
else
    fichSal = fichEnt;

/*--------------------------------------------------------------------
 | Se realiza el tratamiento de la base segun el fichero de registros.
 +--------------------------------------------------------------------*/
FIM_LeerBase(fichEnt);
SCA_Inicializar(APF_Argumento('f'));
yyparse();
SCA_Finalizar();

/*--------------------------------------------------------------------
 | Si hay errores no se salva. En caso contrario si se salva.
 +--------------------------------------------------------------------*/
if (ERR_HayErroresTerminal())
    exit(-1);
FIM_EscribirBase(fichSal);

return (0);
}

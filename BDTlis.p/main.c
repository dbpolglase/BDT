#include <stdio.h>
#include <stdlib.h>
#include <tpc.h>
#include <apf.h>
#include <fim.h>
#include <lst.h>
#include <dat.h>

/* OJO: provisional */int yylineno = 0;

/*==================================================================== *
 *			PROGRAMA PRINCIPAL
 *====================================================================*/
int main(int argc, char *argv[])
{
	char *pBase;
    	char  formato;
	int   strComoBuf;

/*--------------------------------------------------------------------
 | Se define el interfaz de la herramienta BDTlis.
 +--------------------------------------------------------------------*/
APF_SinDefiniciones();
APF_SinVisualizar();
APF_DefFichero(1, EXTENS_IMAGEN, " image file  BDT");
APF_DefDescripcion("List the BDT BBDD records");
APF_DefArgumento('T', "tabla",
		 "name  of the table to list (default all)",
		 "table name ");
APF_DefOpcion('d', "description", " show DDBB format description ");
APF_DefOpcion('r', "register",
	      "list in register format (default line format )");
APF_DefOpcion('b', "buffer", "list strings in buffer format ");

/*--------------------------------------------------------------------
 | Se analizan los argumentos.
 +--------------------------------------------------------------------*/
APF_Analizar(argc, argv);

/*--------------------------------------------------------------------
 | Si existe la opcion "-d" se visualizan las definiciones de la base.
 +--------------------------------------------------------------------*/
pBase = FIM_LeerBase(APF_Fichero());
if (APF_Opcion('d')) {
    printf("===============================================================\n");
    DAT_VisDefiniciones();
    printf("===============================================================\n");
    }

/*--------------------------------------------------------------------
 | Se pone el formato segun si existe la opcion "-r" o no.
 +--------------------------------------------------------------------*/
if (APF_Opcion('r'))
    formato = FMT_REGISTRO;
else
    formato = FMT_LINEA;

/*--------------------------------------------------------------------
 | Se mira si existe la opcion de listar strings con formato de buffer.
 +--------------------------------------------------------------------*/
if (APF_Opcion('b'))
    strComoBuf = 1;
else
    strComoBuf = 0;

/*--------------------------------------------------------------------
 | Se lista una tabla o todas segun la opcion "-T"
 +--------------------------------------------------------------------*/
if (APF_Opcion('T'))
    LST_ListarUnaTabla(pBase, APF_Argumento('T'), formato, strComoBuf);
else
    LST_ListarTablas(pBase, formato, strComoBuf);

exit (0);
}

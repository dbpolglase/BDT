#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <tpc.h>
#include <apf.h>
#include <err.h>
#include <dat.h>
#include <fim.h>
#include <sca.h>
#include <lib.h>

#define EXTENS_BDT	"bdt"

extern int yyparse();

int main(int argc, char *argv[])
{
	int opcVis;

APF_DefFichero(1, EXTENS_BDT, "Data Base ");
APF_DefDescripcion("Compile data base definition");
APF_DefDescripcion(" \n\t and create the library to manager the data base ");
APF_Analizar(argc, argv);
if (ERR_HayErroresTerminal())
    exit(-1);

SCA_Inicializar(APF_Fichero());
yyparse();
SCA_Finalizar();
if (ERR_HayErroresTerminal())
    exit(-1);

opcVis = APF_Opcion('v');

if (opcVis) {
    DAT_VisDefiniciones();
    printf("\nMaking image file ......... ");
    fflush(stdout);
    }
FIM_GenerarBase();
if (opcVis) {
    printf("FINISHING\nMaking src ................. ");
    fflush(stdout);
    }
LIB_GenerarFuente();
if (opcVis) {
    printf("FINISHING\nMaking  header ................. ");
    fflush(stdout);
    }
LIB_GenerarHeader();
if (opcVis) {
    printf("FINISH\n");
    fflush(stdout);
    }

return (0);
}

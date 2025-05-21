#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <err.h>
#include <mdi.h>
#include <tpc.h>
#include <fim.h>
#include <dat.h>
#include <lst.h>

/*====================================================================
 *		DECLARACION DE FUNCIONES PRIVADAS
 *====================================================================*/
static void ListarTabla(char *, RefTABLA, char, int);
static void ListarCampo(RefCAMPO, char *, char, int, int);
static void EscrString(char *);
static void EscrBuffer(char *, int);
static void EscrByte(unsigned char);
static void EscrCaracter(char, int);

/*====================== PROCEDIMIENTO PUBLICO ======================
 * LST_ListarUnaTabla
 *	Lista el contenido de los elementos ocupados de una tabla
 *
 *    Parametros:
 *	pBase: puntero a la memoria de la base.
 *	nomTabla: nombre de la tabla a listar.
 *	formato: FMT_REGISTRO / FMT_LINEA
 *	strComoBuf: flag para listar los strings con formato de buffer.
 *====================================================================*/
void LST_ListarUnaTabla(char *pBase, char *nomTabla,
			char formato, int strComoBuf)
{
	RefTABLA refTabla;

/*--------------------------------------------------------------------
 | Se recorren las tablas y si la encuentra listan los elementos, si
 | no reporta un error al usuario.
 +--------------------------------------------------------------------*/
refTabla = DAT_RecTablas();
while(refTabla) {
    if (strcasecmp(nomTabla, DAT_NombreTabla(EN_MAYUSC, refTabla)) == 0) {
	ListarTabla(pBase, refTabla, formato, strComoBuf);
	return;
	}
    refTabla = DAT_SigTabla(refTabla);
    }
ERR_PorTerminal("table \"%s\" don't exist in the base", nomTabla);
}

/*====================== PROCEDIMIENTO PUBLICO ======================
 * LST_ListarTablas
 *	Lista el contenido de los elementos ocupados de todas las
 *	tablas de la base.
 *
 *    Parametros:
 *	pBase: puntero a la memoria de la base.
 *	formato: FMT_REGISTRO / FMT_LINEA
 *	strComoBuf: flag para listar los strings con formato de buffer.
 *====================================================================*/
void LST_ListarTablas(char *pBase, char formato, int strComoBuf)
{
	RefTABLA refTabla;

/*--------------------------------------------------------------------
 | Se recorren las tablas y se listan los elementos.
 +--------------------------------------------------------------------*/
refTabla = DAT_RecTablas();
while(refTabla) {
    ListarTabla(pBase, refTabla, formato, strComoBuf);
    refTabla = DAT_SigTabla(refTabla);
    }
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * ListarTabla
 *	Lista los elementos de una determinada base.
 *
 *    Parametros:
 *	pBase: puntero a la memoria de la base.
 *	refTabla: referencia a la base a listar.
 *	formato: FMT_REGISTRO / FMT_LINEA
 *	strComoBuf: flag para listar los strings con formato de buffer.
 *====================================================================*/
static void ListarTabla(char *pBase, RefTABLA refTabla,
			char formato, int strComoBuf)
{
	int   ocupados;
	char *pElemento;
	int   bytesElem;
	int   hayElems;
	RefCAMPO refCampo;
	int   primerCampo;

/*--------------------------------------------------------------------
 | Se escribe el nombre de la tabla.
 +--------------------------------------------------------------------*/
printf("-------------------------------------------------------------------\n");
printf("Table %s\n", DAT_NombreTabla(EN_MAYUSC, refTabla));
printf("-------------------------------------------------------------------\n");

/*--------------------------------------------------------------------
 | Se inicializan las variables de recorrido de la tabla.
 +--------------------------------------------------------------------*/
ocupados  = 0;
pElemento = pBase + DAT_PosicElementos(refTabla);
bytesElem = DAT_BytesElemento(refTabla);
hayElems  = DAT_DimMaxTabla(refTabla);

/*--------------------------------------------------------------------
 | Se recorre la tabla y para cada elemento ocupado, se escriben los
 | campos segun sea el formato REGISTRO o LINEA.
 +--------------------------------------------------------------------*/
while(hayElems) {
    if (*pElemento == ELEM_OCUPADO) {
	if (formato == FMT_LINEA)
	    printf("  (");
	refCampo    = DAT_RecCampos(refTabla);
	primerCampo = 1;
	while(refCampo) {
	    ListarCampo(refCampo, pElemento, formato, strComoBuf, primerCampo);
	    refCampo	= DAT_SigCampo(refCampo);
	    primerCampo = 0;
	    }
	if (formato == FMT_LINEA)
	    printf(")");
	printf("\n");
	ocupados++;
	}
    pElemento += bytesElem;
    hayElems--;
    }

/*--------------------------------------------------------------------
 | Se scribe el numero de elementos ocupados de la base.
 +--------------------------------------------------------------------*/
printf("%d records\n\n", ocupados);
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * ListarCampo
 *	Lista el valor de un campo de una tabla segun formato.
 *
 *    Parametros:
 *	refCampo:  referencia al campo a escribir.
 *	pElemento: puntero al elemento al que pertenece el campo.
 *	formato: FMT_REGISTRO / FMT_LINEA
 *	strComoBuf: flag para listar los strings con formato de buffer.
 *	primero: indicador de primer valor.
 *====================================================================*/
static void ListarCampo(RefCAMPO refCampo, char *pElemento,
			char formato, int strComoBuf, int primero)
{
	char *pCampo;
	int   dimens;
	int   i;

/*--------------------------------------------------------------------
 | Se inicializa el puntero al campo y la dimension de este.
 +--------------------------------------------------------------------*/
pCampo = pElemento + DAT_PosicCampo(refCampo);
dimens = DAT_DimensCampo(refCampo);

/*--------------------------------------------------------------------
 | - Si es formato REGISTRO, se escribe el nombre del campo (con '=')
 | - Si es formato LINEA y no es el primer valor se escribe la ',' de
 |   separacion de valores.
 +--------------------------------------------------------------------*/
if (formato == FMT_REGISTRO)
    printf("\t%s = ", DAT_NombreCampo(EN_MAYUSC, refCampo));
if ((formato == FMT_LINEA) && !primero)
    printf(", ");

/*--------------------------------------------------------------------
 | Se decodifica el valor desde el campo codificado y se escribe este,
 | segun el tipo del campo (y dimension, para char y unsigned char).
 +--------------------------------------------------------------------*/
switch(DAT_TipoCampo(refCampo)) {
    case TPC_CHAR: {
		char  caracter;
		char *pBuffer;

	if (dimens == 1) {
	    TPC_DecChar(&caracter, 1, pCampo);
	    EscrCaracter(caracter, 1);
	    }
	else {
	    pBuffer = MDI_Buffer(dimens);
	    TPC_DecChar(pBuffer, dimens, pCampo);
	    if (strComoBuf)
		EscrBuffer((char *)pBuffer, dimens);
	    else
		EscrString(pBuffer);
	    free(pBuffer);
	    }
	break;
	}

    case TPC_UNS_CHAR: {
		unsigned char  byte;
		unsigned char *pBuffer;

	if (dimens == 1) {
	    TPC_DecUnsChar(&byte, 1, pCampo);
	    EscrByte(byte);
	    }
	else {
	    pBuffer = MDI_Buffer(dimens);
	    TPC_DecUnsChar(pBuffer, dimens, pCampo);
	    EscrBuffer((char *)pBuffer, dimens);
	    free(pBuffer);
	    }
	break;
	}

    case TPC_SHORT: {
		short int numero;
        if (dimens == 1)
        {
	  TPC_DecShort(&numero, 1, pCampo);
	  printf("%d", numero);
        }
        else
        {
	  printf("{");
          for (i=0;i<dimens;i++)
          {
	   TPC_DecShort(&numero, 1, pCampo);
           if (i!=dimens-1)
	    printf("%d,", numero);
           else
	    printf("%d", numero);
           pCampo += TPC_TAM_COD_SHORT;
          }
	  printf("}");
        }
	break;
	}

    case TPC_UNS_SHORT: {
	unsigned short int numero;
        if (dimens == 1)
        {
	  TPC_DecUnsShort(&numero, dimens, pCampo);
	  printf("%u", numero);
        }
        else
        {
	  printf("{");
          for (i=0;i<dimens;i++)
          {
	   TPC_DecUnsShort(&numero, 1, pCampo);
           if (i!=dimens-1)
	    printf("%u,", numero);
           else
	    printf("%u", numero);
           pCampo += TPC_TAM_COD_UNS_SHORT;
          }
	  printf("}");
        }
	break;
	}

    case TPC_INT: {
	int numero;
        if (dimens == 1)
        {
	  TPC_DecInt(&numero, 1, pCampo);
  	  printf("%d", numero);
	}
        else
        {
	  printf("{");
          for (i=0;i<dimens;i++)
          {
	   TPC_DecInt(&numero, 1, pCampo);
           if (i!=dimens-1)
	    printf("%d,", numero);
           else
	    printf("%d", numero);
           pCampo += TPC_TAM_COD_INT;
          }
	  printf("}");
        }
	break;
	}

    case TPC_UNS_INT: {
	unsigned int numero;

        if (dimens == 1)
        {
	  TPC_DecUnsInt(&numero, 1, pCampo);
	  printf("%u", numero);
        }
        else
        {
	  printf("{");
          for (i=0;i<dimens;i++)
          {
	   TPC_DecUnsInt(&numero, 1, pCampo);
           if (i!=dimens-1)
	    printf("%u,", numero);
           else
	    printf("%u", numero);
           pCampo += TPC_TAM_COD_UNS_INT;
          }
	  printf("}");
        }
	break;
	}

    case TPC_LONG: {
	long numero;
        if (dimens == 1)
        {
	  TPC_DecLong(&numero, 1, pCampo);
	  printf("%ld", numero);
        }
        else
        {
	  printf("{");
          for (i=0;i<dimens;i++)
          {
	   TPC_DecLong(&numero, 1, pCampo);
           if (i!=dimens-1)
	    printf("%ld,", numero);
           else
	    printf("%ld", numero);
           pCampo += TPC_TAM_COD_LONG;
          }
	  printf("}");
        }
	break;
	}

    case TPC_UNS_LONG: {
	unsigned long numero;
        if (dimens == 1)
        {
  	  TPC_DecUnsLong(&numero, 1, pCampo);
	  printf("%lu", numero);
        }
        else
        {
	  printf("{");
          for (i=0;i<dimens;i++)
          {
	   TPC_DecUnsLong(&numero, 1, pCampo);
           if (i!=dimens-1)
	    printf("%lu,", numero);
           else
	    printf("%lu", numero);
           pCampo += TPC_TAM_COD_UNS_LONG;
          }
	  printf("}");
        }
	break;
	}

    case TPC_FLOAT: {
	float numero;
        if (dimens == 1)
        {
	  TPC_DecFloat(&numero, 1, pCampo);
  	  printf("%f", numero);
        }
        else
        {
	  printf("{");
          for (i=0;i<dimens;i++)
          {
	   TPC_DecFloat(&numero, 1, pCampo);
           if (i!=dimens-1)
	    printf("%f,", numero);
           else
	    printf("%f", numero);
           pCampo += TPC_TAM_COD_FLOAT;
          }
	  printf("}");
        }
	break;
	}


    case TPC_DOUBLE: {
	double numero;
        if (dimens==1)
        {
	  TPC_DecDouble(&numero, 1, pCampo);
	  printf("%lf", numero);
        }
        else
        {
	  printf("{");
          for (i=0;i<dimens;i++)
          {
	   TPC_DecDouble(&numero, 1, pCampo);
           if (i!=dimens-1)
	    printf("%lf,", numero);
           else
	    printf("%lf", numero);
           pCampo += TPC_TAM_COD_DOUBLE;
          }
	  printf("}");
        }
	break;
      default:
	break;
	}
    }
/*--------------------------------------------------------------------
 | En el formato REGISTRO se escribe un valor en cada linea.
 +--------------------------------------------------------------------*/
if (formato == FMT_REGISTRO)
    printf("\n");
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * EscrString
 *	Escribe un string considerando los posibles caracteres no
 *	visibles.
 *
 *    Parametros:
 *	pString: string a escribir.
 *====================================================================*/
static void EscrString(char *pString)
{
/*--------------------------------------------------------------------
 | Se escribe el contenido del buffer entre "".
 +--------------------------------------------------------------------*/
printf("\"");
while (*pString) {
    EscrCaracter(*pString, 0);
    pString++;
    }
printf("\"");
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * EscrBuffer
 *	Escribe un buffer de bytes (char/unsigned char) como caracteres.
 *
 *    Parametros:
 *	pBuffer: 
 *	dimens: 
 *====================================================================*/
static void EscrBuffer(char *pBuffer, int dimens)
{
/*--------------------------------------------------------------------
 | Se escribe el contenido del buffer entre "".
 +--------------------------------------------------------------------*/
printf("\"");
while (dimens) {
    EscrCaracter(*pBuffer, 0);
    dimens--;
    pBuffer++;
    }
printf("\"");
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * EscrByte
 *	Escribe el contenido de un byte en hexadecimal.
 *
 *    Parametros:
 *	byte: 
 *====================================================================*/
static void EscrByte(unsigned char byte)
{
/*	int digito;*/

/*--------------------------------------------------------------------
 | Primero se escribe el digito superior y despues las unidades.
 +--------------------------------------------------------------------*/
/*
digito = byte / 16;
printf("0x%c", (digito > 9) ? (digito-10) + 'A' : digito + '0');
digito = byte % 16;
printf("%c", (digito > 9) ? (digito-10) + 'A' : digito + '0');
*/
printf("%u", (unsigned int) byte);
}

/*====================== PROCEDIMIENTO PRIVADO ======================
 * EscrCaracter
 *	
 *
 *    Parametros:
 *	caracter: 
 *	conComillas: 
 *====================================================================*/
static void EscrCaracter(char caracter, int conComillas)
{
/*--------------------------------------------------------------------
 | Si es con comillas, se pone una al inicio y otra al final.
 +--------------------------------------------------------------------*/
if (conComillas)
    printf("'");

/*--------------------------------------------------------------------
 | Si el caracter es visible, se escribe tal cual, si no, en octal.
 +--------------------------------------------------------------------*/
switch(caracter) {
    case '\a':
        printf("\\a");
        break;
    case '\f':
        printf("\\f");
        break;
    case '\n':
        printf("\\n");
        break;
    case '\r':
        printf("\\r");
        break;
    case '\t':
        printf("\\t");
        break;
    case '\b':
        printf("\\b");
        break;
    case '\v':
        printf("\\v");
        break;
    default:
	if (isprint(caracter))
	    printf("%c", caracter);
	else
	    printf("\\%.3o", caracter & 0x00FF);
     }

if (conComillas)
    printf("'");
}

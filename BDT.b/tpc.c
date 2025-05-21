#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <arpa/inet.h>
#include <tpc.h>

/*--------------------------------------------------------------------
 | DIM: macro de dimension de un array
 +--------------------------------------------------------------------*/
#define DIM(A)   (sizeof(A)/sizeof(A[0]))
 
/*--------------------------------------------------------------------
 | struct ATR_TIPO:
 |	tipo: codigo del tipo.
 |	nombre: nombre del tipo en ASCII.
 |	bytesCod: bytes que ocupan su codificacion
 |	funcCod: nombre de la funcion de codificacion del tipo.
 |	funcDec: nombre de la funcion de decodificacion del tipo.
 +--------------------------------------------------------------------*/
typedef struct {
        TipoTPC  tipo;
        char *nombre;
        int   bytesCod;
	char *funcCod;
	char *funcDec;
        } ATR_TIPO;
 
/*====================================================================
 *		DECLARACION DE FUNCIONES PRIVADAS
 *====================================================================*/
static int TPC_Portable = 0; /* no portable.*/
static int BuscaTipo(TipoTPC tipo);
static int is_little_endian();
static unsigned long fromto_big_endian(unsigned long value);

/*--------------------------------------------------------------------
 | lstTipos: lista de tipos: tipo, nombre y tamano.
 +--------------------------------------------------------------------*/
ATR_TIPO lstTipos[] = {
        {TPC_CHAR,	"char",		      TPC_TAM_COD_CHAR,
			"TPC_CodChar",	      "TPC_DecChar"},
        {TPC_UNS_CHAR,	"unsigned char",      TPC_TAM_COD_UNS_CHAR,
			"TPC_CodUnsChar",     "TPC_DecUnsChar"},

        {TPC_SHORT,	"short int",	      TPC_TAM_COD_SHORT,
			"TPC_CodShort",	      "TPC_DecShort"},
        {TPC_UNS_SHORT,	"unsigned short int", TPC_TAM_COD_UNS_SHORT,
			"TPC_CodUnsShort",    "TPC_DecUnsShort"},

        {TPC_INT,	"int",		      TPC_TAM_COD_INT,
			"TPC_CodInt",	      "TPC_DecInt"},
        {TPC_UNS_INT,	"unsigned int",	      TPC_TAM_COD_UNS_INT,
			"TPC_CodUnsInt",      "TPC_DecUnsInt"},

        {TPC_LONG,      "long",		      TPC_TAM_COD_LONG,
			"TPC_CodLong",	      "TPC_DecLong"},
        {TPC_UNS_LONG,	"unsigned long",      TPC_TAM_COD_UNS_LONG,
			"TPC_CodUnsLong",     "TPC_DecUnsLong"},

        {TPC_FLOAT,	"float",	      TPC_TAM_COD_FLOAT,
			"TPC_CodFloat",      "TPC_DecFloat"},

        {TPC_DOUBLE,	"double",	      TPC_TAM_COD_DOUBLE,
			"TPC_CodDouble",      "TPC_DecDouble"},

        {TPC_STRING,	"char *",	      TPC_TAM_COD_CHAR,
			"TPC_CodString",      "TPC_DecString"},

        {TPC_REGISTRO,	"Registro",	      0,	"", ""
        }};

/*========================= FUNCION PUBLICA =========================
 * TPC_HazPortable
 *	Hacer portable la BBDD entre arquitecturas o lo que es lo mismo
 *     permitir little-big endian. 
 *      Si se hace portable todo se codifica en big endian.
 *
 *    Parametros:
 *	hacerPortable
 *    Devuelve:
 *	void
 *====================================================================*/
void TPC_HazPortable(int hazPortable)
{
   TPC_Portable = hazPortable; /* no portable.*/
}
 
/*========================= FUNCION PUBLICA =========================
 * TPC_Nombre
 *	Calcula el nombre ASCII de un tipo determinado.
 *
 *    Parametros:
 *	tipo: el codigo del tipo.
 *    Devuelve:
 *	el nombre: si existe el tipo.
 *	"?**tipo**?": si no existe el tipo.
 *====================================================================*/
char *TPC_Nombre(TipoTPC tipo)
{
        int i;
 
/*--------------------------------------------------------------------
 | Busca el tipo en la lista y si no existe devuelve "?**tipo**?"
 +--------------------------------------------------------------------*/
i = BuscaTipo(tipo);
if (i == -1)
    return "?**tipo**?";
 
/*--------------------------------------------------------------------
 | Si existe, devuelve el nombre del tipo.
 +--------------------------------------------------------------------*/
return lstTipos[i].nombre;
}

/*========================= FUNCION PUBLICA =========================
 * TPC_FuncCodif
 *	Calcula el nombre de la funcion de codificacion de un tipo
 *	determinado.
 *
 *    Parametros:
 *	tipo: el codigo del tipo.
 *    Devuelve:
 *	el nombre: si existe el tipo.
 *	"?**tipo**?": si no existe el tipo.
 *====================================================================*/
char *TPC_FuncCodif(TipoTPC tipo)
{
        int i;
 
/*--------------------------------------------------------------------
 | Busca el tipo en la lista y si no existe devuelve "?**tipo**?"
 +--------------------------------------------------------------------*/
i = BuscaTipo(tipo);
if (i == -1)
    return "?**tipo**?";
 
/*--------------------------------------------------------------------
 | Si existe, devuelve el nombre de la funcion de codificacion.
 +--------------------------------------------------------------------*/
return lstTipos[i].funcCod;
}

/*========================= FUNCION PUBLICA =========================
 * TPC_FuncDecod
 *	Calcula el nombre de la funcion de decodificacion de un tipo
 *	determinado.
 *
 *    Parametros:
 *	tipo: el codigo del tipo.
 *    Devuelve:
 *	el nombre: si existe el tipo.
 *	"?**tipo**?": si no existe el tipo.
 *====================================================================*/
char *TPC_FuncDecod(TipoTPC tipo)
{
        int i;
 
/*--------------------------------------------------------------------
 | Busca el tipo en la lista y si no existe devuelve "?**tipo**?"
 +--------------------------------------------------------------------*/
i = BuscaTipo(tipo);
if (i == -1)
    return "?**tipo**?";
 
/*--------------------------------------------------------------------
 | Si existe, devuelve el nombre de la funcion de decodificacion.
 +--------------------------------------------------------------------*/
return lstTipos[i].funcDec;
}

/*========================= FUNCION PUBLICA =========================
 * TPC_TamanoCodif
 *	Calcula el tamano en bytes del buffer necesario para la
 *	codificacion de un tipo determinado.
 *
 *    Parametros:
 *	tipo: codigo del tipo.
 *	dimension: dimension (1: dato simple / n: array).
 *    Devuelve:
 *	tamano codificado: si el tipo de dato es correcto.
 *	-1: si el tipo de dato es erroneo.
 *====================================================================*/
int TPC_TamanoCodif(TipoTPC tipo, int dimension)
{
        int i;
 
/*--------------------------------------------------------------------
 | Busca el tipo en la lista y si no existe devuelve -1
 +--------------------------------------------------------------------*/
i = BuscaTipo(tipo);
if (i == -1)
    return -1;
 
/*--------------------------------------------------------------------
 | Si existe, devuelve el tamano unitario por la dimension
 +--------------------------------------------------------------------*/
return (lstTipos[i].bytesCod * dimension);
}

/*========================= FUNCION PUBLICA =========================
 * TPC_CodChar
 *	Codificacion de datos tipo "char".
 *
 *    Parametros:
 *	pCaracter: punt. al caracter o caracteres a codificar.
 *	dimension: dimension del dato a codificar.
 *	pBuff: buffer para la codificacion.
 *    Devuelve:
 *	la longitud correspondiente a la codificacion.
 *====================================================================*/
int TPC_CodChar(char *pCaracter, int dimension, char *pBuff)
{
if (dimension != 1) {
    /*--------------------------------------------------------------------
     | Se copian el/los caracter/es en el buffer como un string truncando
     | el string si la longitud es mayor que la dimension (considerando el
     | NUL de fin de string).
     | se devuelve la dimension por el tamano del un "char" codificado.
     +--------------------------------------------------------------------*/
    strncpy(pBuff, pCaracter, dimension);
    pBuff[dimension - 1] = '\0';
    return (dimension * TPC_TAM_COD_CHAR);
    }
*pBuff = *pCaracter;
return TPC_TAM_COD_CHAR;
}

/*========================= FUNCION PUBLICA =========================
 * TPC_DecChar
 *	Decodificacion de datos tipo "char".
 *
 *    Parametros:
 *	pCaracter: punt. al caracter o caracteres a decodificar.
 *	dimension: dimension del dato a decodificar.
 *	pBuff: buffer de donde decodificar.
 *    Devuelve:
 *	la longitud correspondiente a la codificacion.
 *====================================================================*/
int TPC_DecChar(char *pCaracter, int dimension, char *pBuff)
{
if (dimension != 1) {
    /*--------------------------------------------------------------------
     | Se copian el/los caracter/es del buffer como un string y se
     | devuelve la dimension por el tamano del un "char" codificado.
     +--------------------------------------------------------------------*/
    strcpy(pCaracter, pBuff);
    return (dimension * TPC_TAM_COD_CHAR);
    }
*pCaracter = *pBuff;
return TPC_TAM_COD_CHAR;
}

/*========================= FUNCION PUBLICA =========================
 * TPC_CodUnsChar
 *	Codificacion de datos tipo "unsigned char"
 *
 *    Parametros:
 *	pByte: punt. al byte o bytes a codificar.
 *	dimension: dimension del dato a codificar.
 *	pBuff: buffer para la codificacion.
 *    Devuelve:
 *	la longitud correspondiente a la codificacion.
 *====================================================================*/
int TPC_CodUnsChar(unsigned char *pByte, int dimension, char *pBuff)
{
/*--------------------------------------------------------------------
 | Se copian el/los byte/s en el buffer y se devuelve la dimension
 | por el tamano del un "byte" codificado.
 +--------------------------------------------------------------------*/
memcpy(pBuff, pByte, dimension);
return (dimension * TPC_TAM_COD_UNS_CHAR);
}

/*========================= FUNCION PUBLICA =========================
 * TPC_DecUnsChar
 *	Decodificacion de datos tipo "unsigned char".
 *
 *    Parametros:
 *	pByte: punt. al byte o bytes a decodificar.
 *	dimension: dimension del dato a decodificar.
 *	pBuff: buffer de donde decodificar.
 *    Devuelve:
 *	la longitud correspondiente a la codificacion.
 *====================================================================*/
int TPC_DecUnsChar(unsigned char *pByte, int dimension, char *pBuff)
{
/*--------------------------------------------------------------------
 | Se copian el/los byte/s del buffer y se devuelve la dimension
 | por el tamano del un "byte" codificado.
 +--------------------------------------------------------------------*/
memcpy(pByte, pBuff, dimension);
return (dimension * TPC_TAM_COD_UNS_CHAR);
}


/*========================= FUNCION PUBLICA =========================
 * TPC_CodString
 *	Codificacion de datos tipo "string".
 *
 *    Parametros:
 *	string: punt. al string a codificar.
 *	pBuff: buffer para la codificacion.
 *    Devuelve:
 *	la longitud correspondiente a la codificacion.
 *====================================================================*/
int TPC_CodString(char *string, char *pBuff)
{
/*--------------------------------------------------------------------
 | Se codifica copiando el string y se devuelve su longitud mas el NUL
 +--------------------------------------------------------------------*/
strcpy(pBuff, string);
return (strlen(string) + 1);
}

/*========================= FUNCION PUBLICA =========================
 * TPC_DecString
 *	Decodificacion de datos tipo "string".
 *
 *    Parametros:
 *	string: punt. al string a decodificar.
 *	pBuff: buffer de donde decodificar.
 *    Devuelve:
 *	la longitud correspondiente a la codificacion.
 *====================================================================*/
int TPC_DecString(char *string, char *pBuff)
{
/*--------------------------------------------------------------------
 | Se decodifica copiando el string desde el buffer y se devuelve su
 | longitud mas el NUL.
 +--------------------------------------------------------------------*/
strcpy(string, pBuff);
return (strlen(string) + 1);
}

/*========================= FUNCION PUBLICA =========================
 * TPC_LonString
 *	Calcula la longitud de un dato tipo "string" que esta codificado.
 *
 *    Parametros:
 *	pBuff: buffer donde esta el string codificado.
 *    Devuelve:
 *	la longitud del string (mas el NULL).
 *====================================================================*/
int TPC_LonString(char *pBuff)
{
return (strlen(pBuff) + 1);
}


/*========================= FUNCION PUBLICA =========================
 * TPC_CodShort
 *	Codificacion de datos tipo "short int"
 *
 *    Parametros:
 *	pNumShort: punt. al/los numero/s "short int" a codificar.
 *	dimension: dimension del dato a codificar.
 *	pBuff: buffer para la codificacion.
 *    Devuelve:
 *	la longitud correspondiente a la codificacion.
 *====================================================================*/
int TPC_CodShort(short int *pNumShort, int dimension, char *pBuff)
{
	int numUns;
/*--------------------------------------------------------------------
 | Si la dimension es mayor que 1 se codifica el array de numeros.
 | El numero se pasa al rango de unsigned para no tener problemas con
 | los numeros negativos.
 | Se devuelve el tamano de un "short int" codificado por la dimension.
 +--------------------------------------------------------------------*/
if (dimension != 1) {
	int i;
    for (i = 0; i < dimension; i++) {
	numUns = *pNumShort + 32768;
	pNumShort++;
	*pBuff++ = (char)(numUns >> 8);
	*pBuff++ = (char) numUns;
	}
    return (dimension * TPC_TAM_COD_SHORT);
    }

/*--------------------------------------------------------------------
 | Si la dimension es 1, se codifica por el mismo metodo que antes.
 | Se devuelve el tamano de un "short int" codificado.
 +--------------------------------------------------------------------*/
numUns = *pNumShort + 32768;
*pBuff++ = (char)(numUns >> 8);
*pBuff   = (char) numUns;
return TPC_TAM_COD_SHORT;
}

/*========================= FUNCION PUBLICA =========================
 * TPC_DecShort
 *	Decodificacion de datos tipo "short int"
 *
 *    Parametros:
 *	pNumShort: punt. al/los numero/s "short int" a decodificar.
 *	dimension: dimension del dato a decodificar.
 *	pBuff: buffer de donde decodificar.
 *    Devuelve:
 *	la longitud correspondiente a la codificacion.
 *====================================================================*/
int TPC_DecShort(short int *pNumShort, int dimension, char *pBuff)
{
/*--------------------------------------------------------------------
 | Si la dimension es mayor que 1 se decodifica el array de numeros.
 | Los numero se pasan de nuevo del rango unsignes al de enteros.
 | Se devuelve el tamano de un "short int" codificado por la dimension.
 +--------------------------------------------------------------------*/
if (dimension != 1) {
	int i;
    for (i = 0; i < dimension; i++) {
	*pNumShort  = (int) ((*pBuff++ <<  8) & 0x0000FF00);
	*pNumShort |= (int)  (*pBuff++	& 0x000000FF);
	*pNumShort++ -= 32768;
	}
    return (dimension * TPC_TAM_COD_SHORT);
    }

/*--------------------------------------------------------------------
 | Si la dimension es 1, se decodifica por el mismo metodo que antes.
 | Se devuelve el tamano de un "short int" codificado.
 +--------------------------------------------------------------------*/
*pNumShort  = (int) ((*pBuff++ <<  8) & 0x0000FF00);
*pNumShort |= (int)  (*pBuff  	& 0x000000FF);
*pNumShort -= 32768;
return TPC_TAM_COD_SHORT;
}

/*========================= FUNCION PUBLICA =========================
 * TPC_CodUnsShort
 *	Codificacion de datos tipo "unsigned short int"
 *
 *    Parametros:
 *	pNumUnsShort: punt. al/los numero/s "unsigned short int"
 *		      a codificar.
 *	dimension: dimension del dato a codificar.
 *	pBuff: buffer para la codificacion.
 *    Devuelve:
 *	la longitud correspondiente a la codificacion.
 *====================================================================*/
int TPC_CodUnsShort(unsigned short int *pNumUnsShort,
		    int dimension, char *pBuff)
{
/*--------------------------------------------------------------------
 | Si la dimension es mayor que 1 se codifica el array de numeros.
 | Se devuelve el tamano de un "unsigned short int" codificado por
 | la dimension.
 +--------------------------------------------------------------------*/
if (dimension != 1) {
	int i;
    for (i = 0; i < dimension; i++) {
	*pBuff++ = (char)(*pNumUnsShort >> 8);
	*pBuff++ = (char) *pNumUnsShort++;
	}
    return (dimension * TPC_TAM_COD_UNS_SHORT);
    }

/*--------------------------------------------------------------------
 | Si la dimension es 1, se codifica por el mismo metodo que antes.
 | Se devuelve el tamano de un "unsigned short int" codificado.
 +--------------------------------------------------------------------*/
*pBuff++ = (char)(*pNumUnsShort >> 8);
*pBuff   = (char) *pNumUnsShort;
return TPC_TAM_COD_UNS_SHORT;
}

/*========================= FUNCION PUBLICA =========================
 * TPC_DecUnsShort
 *	Decodificacion de datos tipo "unsigned short int".
 *
 *    Parametros:
 *	pNumUnsShort: punt. al/los numero/s "unsigned short int"
 *		      a decodificar.
 *	dimension: dimension del dato a decodificar.
 *	pBuff: buffer de donde decodificar.
 *    Devuelve:
 *	la longitud correspondiente a la codificacion.
 *====================================================================*/
int TPC_DecUnsShort(unsigned short int *pNumUnsShort,
		    int dimension, char *pBuff)
{
/*--------------------------------------------------------------------
 | Si la dimension es mayor que 1 se decodifica el array de numeros.
 | Se devuelve el tamano de un "unsigned short int" codificado por
 | la dimension.
 +--------------------------------------------------------------------*/
if (dimension != 1) {
	int i;
    for (i = 0; i < dimension; i++) {
	*pNumUnsShort    = (int) ((*pBuff++ << 8) & 0x0000FF00);
	*pNumUnsShort++ |= (int)  (*pBuff++	  & 0x000000FF);
	}
    return (dimension * TPC_TAM_COD_UNS_SHORT);
    }

/*--------------------------------------------------------------------
 | Si la dimension es 1, se decodifica por el mismo metodo que antes.
 | Se devuelve el tamano de un "unsigned short int" codificado.
 +--------------------------------------------------------------------*/
*pNumUnsShort  = (int) ((*pBuff++ << 8) & 0x0000FF00);
*pNumUnsShort |= (int)  (*pBuff  	& 0x000000FF);
return TPC_TAM_COD_UNS_SHORT;
}


/*========================= FUNCION PUBLICA =========================
 * TPC_CodInt
 *	Codificacion de datos tipo "int"
 *
 *    Parametros:
 *	pNumInt: punt. al/los numero/s "int" a codificar.
 *	dimension: dimension del dato a codificar.
 *	pBuff: buffer para la codificacion.
 *    Devuelve:
 *	la longitud correspondiente a la codificacion.
 *====================================================================*/
int TPC_CodInt(int *pNumInt, int dimension, char *pBuff)
{
  union {
    int d;
    uint8_t bytes[TPC_TAM_COD_INT];
  }pAux;

  int i,j;

  for (j = 0; j < dimension; j++,pNumInt++) 
  {
    pAux.d = *pNumInt;
    if (is_little_endian() && TPC_Portable)
    {
     for (i=0;i< TPC_TAM_COD_INT;i++,pBuff++)
     {
       *pBuff = pAux.bytes[TPC_TAM_COD_INT - 1 -i];
     }
    }
    else
    {
     memcpy(pBuff, &pAux.bytes, TPC_TAM_COD_INT);
     pBuff+=TPC_TAM_COD_INT;
    }
  }
 return (dimension * TPC_TAM_COD_INT);
}

/*========================= FUNCION PUBLICA =========================
 * TPC_DecInt
 *	Decodificacion de datos tipo "int"
 *
 *    Parametros:
 *	pNumInt: punt. al/los numero/s "int" a decodificar.
 *	dimension: dimension del dato a codificar.
 *	pBuff: buffer de donde decodificar.
 *    Devuelve:
 *	la longitud correspondiente a la codificacion.
 *====================================================================*/
int TPC_DecInt(int *pNumInt, int dimension, char *pBuff)
{
  union {
    int d;
    uint8_t bytes[TPC_TAM_COD_INT];
  }pAux;

  int i,j,k;
  
  for (k=0;k< dimension;k++,pNumInt++)
  {  
    if (is_little_endian() && TPC_Portable)
    {
     for (i=0,j=TPC_TAM_COD_INT-1;i<TPC_TAM_COD_INT ;i++,j--)
     {
       pAux.bytes[i] = *(pBuff + j);
     }
    }
    else
    {
     memcpy(&pAux.bytes, pBuff, TPC_TAM_COD_INT);
    }
    *pNumInt = pAux.d;
    pBuff +=TPC_TAM_COD_INT;
  }
  pNumInt --;
 return (dimension * TPC_TAM_COD_INT);
}

/*========================= FUNCION PUBLICA =========================
 * TPC_CodUnsInt
 *	Codificacion de datos tipo "unsigned int".
 *
 *    Parametros:
 *	pNumUns: punt. al/los numero/s "unsigned int" a codificar.
 *	dimension: dimension del dato a codificar.
 *	pBuff: buffer para codificacion.
 *    Devuelve:
 *	la longitud correspondiente a la codificacion.
 *====================================================================*/
int TPC_CodUnsInt(unsigned int *pNumUns, int dimension, char *pBuff)
{
/*--------------------------------------------------------------------
 | Se codifican igual que los datos tipo "int".
 +--------------------------------------------------------------------*/
return TPC_CodInt((int *) pNumUns, dimension, pBuff);
}

/*========================= FUNCION PUBLICA =========================
 * TPC_DecUnsInt
 *	Decodificacion de datos tipo "unsigned int".
 *
 *    Parametros:
 *	pNumUns: punt. al/los numero/s "unsigned int" a decodificar.
 *	dimension: dimension del dato a decodificar.
 *	pBuff: buffer de donde decodificar.
 *    Devuelve:
 *	la longitud correspondiente a la codificacion.
 *====================================================================*/
int TPC_DecUnsInt(unsigned int *pNumUns, int dimension, char *pBuff)
{
/*--------------------------------------------------------------------
 | Se decodifican igual que los datos tipo "int".
 +--------------------------------------------------------------------*/
return TPC_DecInt((int *) pNumUns, dimension, pBuff);
}


/*========================= FUNCION PUBLICA =========================
 * TPC_CodLong
 *	Codificacion de datos tipo "long".
 *
 *    Parametros:
 *	pNumLong: punt. al/los numero/s "long" a codificar.
 *	dimension: dimension del dato a codificar.
 *	pBuff: buffer para codificacion.
 *    Devuelve:
 *	la longitud correspondiente a la codificacion.
 *====================================================================*/
int TPC_CodLong(long *pNumLong, int dimension, char *pBuff)
{
  long pAux;
  int i;
   pAux = *pNumLong;
   for (i=0;i<dimension;i++,pNumLong++)
   {
    if (is_little_endian() && TPC_Portable)
     pAux = fromto_big_endian(*pNumLong);

    memcpy(pBuff, &pAux, TPC_TAM_COD_LONG);
    pBuff += TPC_TAM_COD_LONG;
  }
 return (dimension * TPC_TAM_COD_LONG);
}

/*========================= FUNCION PUBLICA =========================
 * TPC_DecLong
 *	Decodificacion de datos tipo "long".
 *
 *    Parametros:
 *	pNumLong: punt. al/los numero/s "unsigned int" a decodificar.
 *	dimension: dimension del dato a decodificar.
 *	pBuff: buffer de donde decodificar.
 *    Devuelve:
 *	la longitud correspondiente a la codificacion.
 *====================================================================*/
int TPC_DecLong(long *pNumLong, int dimension, char *pBuff)
{
/*--------------------------------------------------------------------
 | Se decodifican igual que los datos tipo "int".
 +--------------------------------------------------------------------*/
  long pAux;
  int i;

    for (i=0;i< dimension;i++,pNumLong++)
    {
      memcpy(&pAux, pBuff, TPC_TAM_COD_LONG);
      if (is_little_endian() && TPC_Portable)
       *pNumLong= fromto_big_endian(pAux);
      else
       *pNumLong = pAux;
      pBuff +=TPC_TAM_COD_LONG;
    }
    
  return (dimension * TPC_TAM_COD_LONG);
}

/*========================= FUNCION PUBLICA =========================
 * TPC_CodUnsLong
 *	Codificacion de datos tipo "unsigned long".
 *
 *    Parametros:
 *	pNumUnsLong: punt. al/los numero/s "unsigned long" a codificar.
 *	dimension: dimension del dato a codificar.
 *	pBuff: buffer para codificacion.
 *    Devuelve:
 *	la longitud correspondiente a la codificacion.
 *====================================================================*/
int TPC_CodUnsLong(unsigned long *pNumUnsLong, int dimension, char *pBuff)
{
/*--------------------------------------------------------------------
 | Se codifican igual que los datos tipo "int".
 +--------------------------------------------------------------------*/
  unsigned long pAux;
  int i;

  for (i=0;i<dimension;i++,pNumUnsLong++)
  {
   pAux = *pNumUnsLong;
   if (is_little_endian() && TPC_Portable)
     pAux = fromto_big_endian(*pNumUnsLong);

   memcpy(pBuff, &pAux, TPC_TAM_COD_UNS_LONG);
   pBuff += TPC_TAM_COD_UNS_LONG; 
  }
 return (dimension * TPC_TAM_COD_UNS_LONG);
}

/*========================= FUNCION PUBLICA =========================
 * TPC_DecUnsLong
 *	Decodificacion de datos tipo "unsigned long".
 *
 *    Parametros:
 *	pNumUnsLong: punt. al/los numero/s "unsigned long" a decodificar.
 *	dimension: dimension del dato a decodificar.
 *	pBuff: buffer de donde decodificar.
 *    Devuelve:
 *	la longitud correspondiente a la codificacion.
 *====================================================================*/
int TPC_DecUnsLong(unsigned long *pNumUnsLong, int dimension, char *pBuff)
{
/*--------------------------------------------------------------------
 | Se decodifican igual que los datos tipo "int".
 +--------------------------------------------------------------------*/
  unsigned long pAux;
  int i;

   for (i=0;i<dimension;i++,pNumUnsLong++)
   {
    memcpy(&pAux, pBuff, TPC_TAM_COD_UNS_LONG);
    if (is_little_endian() && TPC_Portable)
     *pNumUnsLong = fromto_big_endian(pAux);
    else
     *pNumUnsLong = pAux;

    pBuff += TPC_TAM_COD_UNS_LONG;
   } 
  return (dimension * TPC_TAM_COD_UNS_LONG);
}


/*========================= FUNCION PUBLICA =========================
 * TPC_CodDouble
 *	Codificacion de datos tipo "double".
 *
 *    Parametros:
 *	pNumDouble: punt. al/los numero/s "double" a codificar.
 *	dimension: dimension del dato a codificar.
 *	pBuff: buffer para codificacion.
 *    Devuelve:
 *	la longitud correspondiente a la codificacion.
 *====================================================================*/
int TPC_CodDouble(double *pNumDouble, int dimension, char *pBuff)
{
  union {
    double d;
    uint8_t bytes[TPC_TAM_COD_DOUBLE];
  }pAux;

  int i,j;

   for (j=0;j<dimension;j++,pNumDouble++)
   {
    pAux.d = *pNumDouble;
    if (is_little_endian() && TPC_Portable)
    {
     for (i=0;i< TPC_TAM_COD_DOUBLE;i++,pBuff++)
     {
       *pBuff = pAux.bytes[TPC_TAM_COD_DOUBLE - 1 -i];
     }
    }
    else
    {
     memcpy(pBuff, &pAux.bytes, TPC_TAM_COD_DOUBLE);
     pBuff += TPC_TAM_COD_DOUBLE;
    }
  }
 return (dimension * TPC_TAM_COD_DOUBLE);
}


/*========================= FUNCION PUBLICA =========================
 * TPC_DecDouble
 *	Decodificacion de datos tipo "double".
 *
 *    Parametros:
 *	pNumDouble: punt. al/los numero/s "double" a decodificar.
 *	dimension: dimension del dato a decodificar.
 *	pBuff: buffer de donde decodificar.
 *    Devuelve:
 *	la longitud correspondiente a la codificacion.
 *====================================================================*/
int TPC_DecDouble(double *pNumDouble, int dimension, char *pBuff)
{
  union {
    double d;
    uint8_t bytes[TPC_TAM_COD_DOUBLE];
  }pAux;

  int i,j,k;

   for (k=0;k<dimension;k++,pNumDouble++)
   {
    if (is_little_endian() && TPC_Portable)
    {
     for (i=0,j=TPC_TAM_COD_DOUBLE-1;i<TPC_TAM_COD_DOUBLE ;i++,j--)
     {
       pAux.bytes[i] = *(pBuff + j);
     }
    }
    else
    {
     memcpy(&pAux.bytes, pBuff, TPC_TAM_COD_DOUBLE);
    }
    pBuff += TPC_TAM_COD_DOUBLE;
    *pNumDouble = pAux.d;
   }

 return (dimension * TPC_TAM_COD_DOUBLE);
}

/*========================= FUNCION PRIVADA =========================
 * BuscaTipo
 *	Dado el codigo de un tipo, averigua el indice en "lstTipos"
 *
 *    Parametros:
 *	tipo: codigo del tipo.
 *    Devuelve:
 *	el indice: si existe el codigo del tipo.
 *	-1: si no existe.
 *====================================================================*/
static int BuscaTipo(TipoTPC tipo)
{
        int i;
 
/*--------------------------------------------------------------------
 | Se recorre la lista y si lo encuentra devuelve el indice.
 +--------------------------------------------------------------------*/
for (i = 0; i < DIM(lstTipos); i++)
    if (lstTipos[i].tipo == tipo)
        return i;
 
/*--------------------------------------------------------------------
 | Si no lo encuentra devuelve -1.
 +--------------------------------------------------------------------*/
return -1;
}

/*========================= FUNCION PUBLICA =========================
 * TPC_CodFloat
 *	Codificacion de datos tipo "float"
 *
 *    Parametros:
 *	pNumFloat: punt. al/los numero/s "float" a codificar.
 *	dimension: dimension del dato a codificar.
 *	pBuff: buffer para la codificacion.
 *    Devuelve:
 *	la longitud correspondiente a la codificacion.
 *====================================================================*/
int TPC_CodFloat(float *pNumFloat, int dimension, char *pBuff)
{
  union {
    float d;
    uint8_t bytes[TPC_TAM_COD_FLOAT];
  }pAux;

  int i;

    pAux.d = *pNumFloat;
    if (is_little_endian() && TPC_Portable)
    {
     for (i=0;i< TPC_TAM_COD_FLOAT;i++,pBuff++)
     {
       *pBuff = pAux.bytes[TPC_TAM_COD_FLOAT - 1 -i];
     }
    }
    else
    {
     memcpy(pBuff, &pAux.bytes, TPC_TAM_COD_FLOAT);
    }
 return TPC_TAM_COD_FLOAT;
}

/*========================= FUNCION PUBLICA =========================
 * TPC_DecFloat
 *	Decodificacion de datos tipo "float"
 *
 *    Parametros:
 *	pNumInt: punt. al/los numero/s "float" a decodificar.
 *	dimension: dimension del dato a codificar.
 *	pBuff: buffer de donde decodificar.
 *    Devuelve:
 *	la longitud correspondiente a la codificacion.
 *====================================================================*/
int TPC_DecFloat(float *pNumFloat, int dimension, char *pBuff)
{
  union {
    float d;
    uint8_t bytes[TPC_TAM_COD_FLOAT];
  }pAux;

  int i,j;
    if (is_little_endian() && TPC_Portable)
    {
     for (i=0,j=TPC_TAM_COD_FLOAT-1;i<TPC_TAM_COD_FLOAT ;i++,j--)
     {
       pAux.bytes[i] = *(pBuff + j);
     }
    }
    else
    {
     memcpy(&pAux.bytes, pBuff, TPC_TAM_COD_FLOAT);
    }
    *pNumFloat = pAux.d;

 return TPC_TAM_COD_FLOAT;
}


/*========================= FUNCION PRIVADA =========================
 * is_little_endian
 *     Indica si estamos en una maquina little o big endian	
 *
 *    Parametros:
 *    Devuelve:
 *	1 : es little endian
 *	0 :  no es little endian (SUN/HP..)
 *====================================================================*/
static int is_little_endian()
{
   uint16_t num = 0x0001;
   return *(uint8_t*)&num == 0x01;
}

/*========================= FUNCION PRIVADA =========================
 * to_big_endian
 *     Convertir de big endian a little endian
 *
 *    Parametros:
 *      numero: numero a convertir
 *    Devuelve:
 *	numero en big endian.
 *====================================================================*/
static unsigned long fromto_big_endian(unsigned long value)
{
  size_t i;
  unsigned long result=0;
  size_t size = sizeof(value);
   for (i=0;i < size; i++)
   {
     result |= ((value >> (8*i)) & 0xFF) << (8 * (size - 1- i));
   }
   return result;
}


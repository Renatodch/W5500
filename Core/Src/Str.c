/* Includes ------------------------------------------------------------------*/

#include "main.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/	
/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * Remplaza un string por otra.
 * @param  dest 		Cadena donde se almacenara el resultado.
 * @param  src  		Cadena fuente.
 * @param  delim    Cadena a buscar para reemplazar.
 * @param  val  		Cadena que reemplaza.
 */
void String_Remplace_Line(char *src, char *line)     /*ab\r\nab\r\nab\r\nab\r\n*/
{
	char *token;
	char result[1024];

	if( String_Is_Empty(src) )
		return;
	
	strcpy(result, "");
	
   token = strtok(src, "\r\n");
   
   while( token != NULL ) 
   {   
			if(strstr(token, line))
				strcat(result, line);
			else
				strcat(result, token);
			
      token = strtok(NULL, src);
   }
	 
	 strcpy(src, result);
}


/**
 * Obtiene la subcadena que encuentra entre el parametro 'start' y el 'end'
 * y lo retorna en el parametro 'val'. Si no encuentra nada, el parametro 'val'
 * retorna vacio (empty).
 * @param  dat    Cadena sobre la que buscar la subcadena.
 * @param  start  Cadena de inicio a buscar.
 * @param  end    Cadena final a buscar.
 * @param  val  	Cadena encontrada.
 */
void String_Between(char *in, char *start, char *end, char *val) 
{
			char *searchStart;
			char *searchEnd;
			
			strcpy(val, "");
			
			if( String_Is_Empty(in) )
				return;

			if( strcmp( start, "" ) == 0)
				searchStart = in;
			else
				searchStart = strstr(in, start);
			
			if(searchStart == NULL)
				return;
			
			searchStart = searchStart + strlen( start );
			
			
			if( strcmp(end, "") == 0)
				searchEnd = searchStart + strlen(searchStart);
			else
				searchEnd = strstr(searchStart, end);
			
			if(searchEnd == NULL)
				return;	
			
			strlcpy(val, searchStart, (searchEnd-searchStart)+1);	
}

/**
 * Remplaza un string por otra.
 * @param  dest 		Cadena donde se almacenara el resultado.
 * @param  src  		Cadena fuente.
 * @param  delim    Cadena a buscar para reemplazar.
 * @param  val  		Cadena que reemplaza.
 */
void String_Remplace(char *dest, char *src, char *delim, char *remplaze)
{
	char *token;

	if( String_Is_Empty(src) )
		return;
	
   token = strtok(src, delim);
	
	 strcpy(dest, "");
   
   while( token != NULL ) 
   {   
			strcat(dest, token);
		  strcat(dest, remplaze);
      token = strtok(NULL, delim);
   }
}

void String_Remplace2(char *dest, char *src, char *delim, char *remplaze)
{
	char *token;
//	char *t;
	strcpy(dest, "");
	if( String_Is_Empty(src) )
		return;
	
   token = strstr(src, delim);
	
	 if(token != NULL)
	 {
//			if((t=strstr(token,"="))!=NULL)
//			{
//				
//			}
	 }	
	 else return;
	 
//	 strcpy(dest, "");
//   
//   while( token != NULL ) 
//   {   
//			strcat(dest, token);
//		  strcat(dest, remplaze);
//      token = strtok(NULL, delim);
//   }
}

/**
 * Retira los espacios de una cadena y el resultado le devuelve en la misma cadena.
 * @param  data  		Cadena fuente.
 */
void String_Trim_Total(char *in)
{
	int s, d, len;
	
	if( String_Is_Empty(in) )
		return;
	
	d=0;
	
	len = strlen(in);
	
	for(s=0; s<len ;s++)
	{
		if(in[s] == 0x20)
			continue;
		
		in[d] = in[s];
		d++;
	}
	
	in[d] = 0;
}

/**
 * Verifica si la cadena tiene datos.
 * @param  data  		Cadena a testar.
 * @param  delim    Cadena a buscar como delimitador.
 * @return char			1 si hay datos o 0 si la cadena es vacia (empty)
 */
char String_Has(char *data)
{
		if ( strcmp(data, STR_EMPTY) == 0 ) 
			return 0;
		else 
			return 1;
}

/**
 * Verifica si la cadena no tiene datos.
 * @param  data  		Cadena a testar.
 * @return char			1 si no hay datos o 0 si hay datos
 */
char String_Is_Empty(char *data)
{
		if ( String_Has(data) ) 
			return 0;
		else 
			return 1;
}

/*******************************************************************************
* Function Name  : String_Compare
* Description    : Compares if two strings are alike.
* Input          : str1 - string 1.
* Input          : str2 - string 2.
* Output         : None.
* Return         : bool - true if is equal.
*******************************************************************************/
unsigned char String_Compare(char *str1, char *str2)
{
		if ( strcmp(str1, str2) == 0 ) 
			return 1;
		else 
			return 0;
}

/*******************************************************************************
* Function Name  : String_Read_TotalLinesNumber
* Description    : Read the lines number.
* Input          : src - the string source.
* Output         : None.
* Return         : int: the lines number.
*******************************************************************************/
int String_Read_TotalLinesNumber(char *src)
{
		int c = 0;
		char* scaner;
		scaner = src;
		
		if( String_Is_Empty(src) )
			return 0;

		while(1)
		{
			scaner = strstr(scaner, "\r\n");
			if(!scaner)
				break;
			c++;
		}
		
		return c;
}

/*******************************************************************************
* Function Name  : String_Read_Line
* Description    : Read the line in the position number.
* Input          : src - the string source.
* Input          : lineNum - the line number.
* Input          : line - buffer to return the line.
* Output         : None.
* Return         : None.
*******************************************************************************/
void String_Read_Line(char *src, int lineNum, char* line)
{
		int c, totalLines;
		char* scaner;
		scaner = src;
		

		strcpy(line, "");
		
		if( String_Is_Empty(src) )
			return;

		totalLines = String_Read_TotalLinesNumber(src);
		//T("totalLines=%d", totalLines);
		
		if (lineNum == 1)
		{
			String_Between(src, "", "\r\n", line);
			return;
		}
		
		for(c=0;c<lineNum;c++)
		{
			if(c >= totalLines)
				return;
			scaner = strstr(scaner, "\r\n"); 
		}
		
		String_Between(scaner, "\r\n", "\r\n", line);
}


/*******************************************************************************
* Function Name  : Str_IsEqual
* Description    : Compares if two strings are alike.
* Input          : str1 - string 1.
* Input          : str2 - string 2.
* Output         : None.
* Return         : bool - true if is equal.
*******************************************************************************/
unsigned char Str_NotEqual(char *str1, char *str2)
{
		if ( strcmp(str1, str2) == 0 ) 
			return 0;
		else 
			return 1;
}




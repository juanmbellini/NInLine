

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "nInLineBack.h"

#define BORRA_BUFFER while (getchar() != '\n');

#define DOS_JUGADORES 0
#define VS_PC 1

#define FILE_EXT ".sav"

#define BLOQUE 10

typedef char* string;


/*Imprime el menu y asigna opcion elegida a la estructura*/
int menu(void);

/*Pide un entero y lo devuelve. Imprime mensaje de peticion y mensaje de error*/
int leeEntero(int min,int max, string mensaje, string errorMsj);

/*Funcion para obtener un string sin maximo*/
string getLine(string mensaje);

/*Devuelve 1 si selecciono "yes" o 0 si selecciono "no". Permite elegir los caracteres para "yes" y "no"*/
int funcionYesNo(string mensaje, char yes, char no);

/*Setea las variables del juego: modo de juego, cantidad de fichas para hacer linea, tamaño del tablero, y establece el turno en jugador 1. Ademas genera el tablero mediante getDim*/
/*Devuelve 0 si salio todo correctamente, 1 si no pudo generar el tablero por problemas de dimension, 2 si no habia memoria, o -1 en caso de error de parametro. Imprime mensajes mediante getdim*/
int seteaVariables( tipoJuego * game , int opcion );

/*Devuelve 0 si salio todo correctamente, 1 si no tiene sentido generar el tablero, y 2 si no habia memoria. Imprime mensajes*/
/*Unicamente toma las dimensiones y el N. Las verificaciones de dimension las hace la funcion generarTablero*/
int getDim(tipoJuego * game);

/*Recupera una partida guardada. Devuelve 0 en caso de exito, 1 en caso de archivo corrupto, 2 en caso de memoria insuficiente, o -1 en caso de no haber podido abrir el archivo*/
int recuperaJuego(tipoJuego * game);

/*Toma el nombre del archivo y lo busca. Devuelve 1 si pudo tomar el nombre archivo y abrirlo, o 0 en caso contrario*/
FILE * getFile(void);

/*Devuelve 1 en caso de exito, 0 en caso de falta de memoria para generar el tablero con los datos obtenidos, o -1 en caso de archivo corrupto*/
int cargaDatos( tipoJuego * game, FILE * filePartida);

/*Lleva el control del juego*/
void motorJuego(tipoJuego * game);

/*Imprime el tablero con bordes*/
void printBoard( tipoJuego * game);

/*Imprime de quién es el turno*/		
void turnoDe(tipoJuego * game);

/*Imprime de quién es el turno*/
void turnoDe(tipoJuego * game);

/*Funcion para tomar la jugada de una persona, poner en la jugada en el tablero y verificar si gana en dicha jugada*/
/*Devuelve 0 en caso de querer salir o 1 si pudo realizar todas las tareas*/
int juegaHumano(tipoJuego * game);

/*Devuelve la columna a jugar, 0 si se quiere salir, o -1 en caso de memoria insuficiente para tomar la jugada. Verifica si la columna esta llena */
int getPlay(tipoJuego * game);

/*Lleva a cabo los ultimos procedimientos al terminar una jugada*/
/*Solo deberia ejecutarse si el estado del juego no es salir*/
void finDeJugada( tipoJuego * game);

/*Imprime el ganador*/
/*No chequea que el estado del juego sea GANA ya que unicamente la llamo si se cumple dicho estado*/
void ganador(tipoJuego * game);

/*Guarda la partida. Retorna 0 en caso de exito, 1 en caso de "no guardar", 2 en caso de memoria insuficiente, o -1 en caso de error"*/
int guardarPartida(tipoJuego * game);

/*Pide el nombre de un archivo, y si este no existe, o si existe y se quiere reescribir, crea el nuevo archivo*/
/*Retorna el puntero a dicho archivo o NULL en caso de no haber creado archivo. Tambien puede retornar NULL en caso de no haber memoria para generar el string con el nombre del archivo*/
FILE * newFile(void);

/*Verifica si existe el archivo. Retorna 1 si existe, o 0 en caso contrario*/
int existsFile(string fileName);

/*Toma los parametros del juego y los guarda en el vector parametros*/
void tomaParametros(tipoJuego * game, int parametros[]);

/*Crea un vector dinamico que contiene una secuencia de chars con el contenido del tablero, y retorna el puntero a dicho vector*/
unsigned char * tomaPosiciones(tipoJuego * game);

/*Retorna 1 si pudo guardar todos los datos, o 0 en caso contrario*/
int grabaDatos(FILE * file, int parametros[], unsigned char posiciones[], int cantPosiciones);


int
main(void)
{
	int opcion,flag;
	tipoJuego juego;
	
	putchar('\n');
	printf("Bienvenido a N en linea");
	do
	{
		printf("\n\nQue desea hacer?\n");
	
		opcion = menu();
		flag = 0;
		
		switch (opcion)
		{
		
			case 1 :
			
			case 2 : 
			{
				flag = seteaVariables( &juego , opcion );
				switch ( flag )
				{
					case  0 : break;
					
					case  1 : printf("Dimensiones invalidas\n"); break;
					
					case  2 : printf("Memoria insuficiente\n");break;
					
					default : printf("Error inesperado\n"); return 1;
				}
				break;
			}
			case 3 :
			{
				flag = recuperaJuego( &juego );
				switch ( flag )
				{
					case  0 : break;
					
					case  1 : printf("Archivo corrupto\n"); break;
					
					case  2 : printf("Memoria insuficiente\n");break;
					
					case -1 : printf("No se pudo abrir el archivo\n");break;
					
					default : printf("Error inesperado\n"); return 1;
				}
				break;
			}
			case 4 : printf("Hasta luego\n"); break;
	
			default : printf("Error inesperado\n"); return 1;	
		}
		if ( flag )
			printf("Error al intentar %s partida\n", (opcion != 3)?"iniciar una":"continuar la");  
		
		if ( opcion != 4 && !flag)
			motorJuego( &juego );
			
	}
	while(opcion != 4);
	
	return 0;
	
}


/*Imprime el menu y asigna opcion elegida a la estructura*/
int
menu(void)
{
	int opcion;

	printf("Ingrese una opcion:\n1- Juego de dos jugadores\n2- Juego contra la computadora\n3- Recuperar juego guardado\n4- Terminar\n\n");
		
	opcion = leeEntero(1,4,"Opcion: ","Ha ingresado una opcion invalida, intente nuevamente\n");
	
	return opcion;
}

/*Pide un entero y lo devuelve. Imprime mensaje de peticion y mensaje de error*/
int 
leeEntero(int min,int max, string mensaje, string errorMsj)
{
	
	int numero,flag;
	
	do
	{
		flag = 0;
		printf("%s", mensaje);

		if ( scanf("%d",&numero) != 1) /*Entra si no toma un entero*/
			flag = 1;
		else if ( numero < min || numero > max) /*Entra si no esta dentro del rango requerido*/
			flag = 1;
		else if ( (getchar() != '\n') ) /*Entra si lo ultimo que se tomo no fue un "enter"*/
			flag = 1;
		if (flag)
		{
			BORRA_BUFFER
			printf("%s", errorMsj);

		}		
	}
	while (flag);

	return numero;
}

/*Funcion para obtener un string sin maximo*/
string
getLine(string mensaje)
{
	int c;
	int i = 0;
	string aux;
	string st = NULL;
	
	printf("%s",mensaje);
	
	do
	{
		if ( i % BLOQUE == 0 )
		{
			aux=realloc(st,(i+BLOQUE)*sizeof(char));
			if ( aux == NULL)
			{
				free(st);
				return NULL;
			}
			st = aux;
		}

		st[i++] = ( (c=getchar()) == '\n')?0:c;
		
	}
	while (c != '\n');
	
	if ( (i % BLOQUE) != 0) 
		st = realloc(st,i*sizeof(char));
	
	return st;
}


/*Devuelve 1 si selecciono "yes" o 0 si selecciono "no". Permite elegir los caracteres para "yes" y "no"*/
int
funcionYesNo(string mensaje, char yes, char no)
{	
	char minuscYes;
	char minuscNo;
	int yesNo;
	int flag;
	
	yes = toupper(yes);
	minuscYes = tolower(yes);
	no = toupper(no);
	minuscNo = tolower(no);
	
	printf("%s (%c/%c): ",mensaje,minuscYes,minuscNo);
	
	do
	{
		flag = 0;
		
		if ( ((yesNo = getchar()) != minuscYes) && (yesNo != yes) && (yesNo != minuscNo) && (yesNo != no ))
			flag = 1; /* Prende un flag si se ingreso una opcion distinta de minuscYes, yes, minuscNo o no*/
			
		else if ( getchar() != '\n' ) /*Prende el flag si se ingreso algo mas que minuscYes, yes, minuscNo o no*/  
			flag = 1;
		if (flag)
		{
			printf("Opcion invalida\nIngrese una opcion valida: ");
			BORRA_BUFFER
		}
	}
	while ( flag );


	return ( (yesNo == minuscYes || yesNo == yes)?1:0 ); /*Como solo puede llegar con minuscYes, yes, minuscNo o no, solo verifico minuscYes o yes*/

}



/*Setea las variables del juego: modo de juego, cantidad de fichas para hacer linea, tamaño del tablero, y establece el turno en jugador 1. Ademas genera el tablero mediante getDim*/
/*Devuelve 0 si salio todo correctamente, 1 si no pudo generar el tablero por problemas de dimension, 2 si no habia memoria, o -1 en caso de error de parametro. Imprime mensajes*/
int
seteaVariables( tipoJuego * game , int opcion )
{
	
	switch ( opcion )
	{
		case 1 : game->modoJuego = DOS_JUGADORES; break;
		
		case 2 : game->modoJuego = VS_PC; break;
		
		default : return -1;
	
	}
	game->turno = JUGADOR1;
	
	return getDim( game );
}





/*Devuelve 0 si salio todo correctamente, 1 si no tiene sentido generar el tablero, y 2 si no habia memoria. Imprime mensajes*/
/*Unicamente toma las dimensiones y el N. Las verificaciones de dimension las hace la funcion generarTablero*/
int
getDim(tipoJuego * game)
{
	int codigo;
	int yesNo = 0;
	
	do
	{
		printf("Ingrese la cantidad de fichas a ubicar en linea, y las dimensiones del tablero\n");
			
		game->N = leeEntero(3,29,"Cantidad de fichas en linea (Entre 3 y 29): ","Opcion invalida. Intente nuevamente\n");
	
		game->tablero.fils = leeEntero(4,30,"Cantidad de filas (Entre 4 y 30): ","Opcion invalida. Intente nuevamente\n");

		game->tablero.cols = leeEntero(4,30,"Cantidad de columnas (Entre 4 y 30): ","Opcion invalida. Intente nuevamente\n");
		
		/*Dentro de este if pido generar el tablero. Si no se pudo generar pregunto si quiere volver a intentarlo*/
		if ( (codigo = generarTablero(game)) == 1 ) /* Si devuelve 1, las dimensiones no fueron las correctas*/
			yesNo = funcionYesNo("Dimensiones invalidas. Quiere volver a intentarlo?",'s','n');
		else if (codigo == 2) /*Si devuelve 2, no hubo memoria para dicho tablero*/
			yesNo = funcionYesNo("Memoria insuficiente para dichas dimensiones. Quiere volver a intentarlo?",'s','n');
	}
	while (codigo != 0 && yesNo); /* Repite todo lo anterior si se quiere ingresar nuevas dimensiones*/
	
	return codigo;
	
}


/*Recupera una partida guardada. Devuelve 0 en caso de exito, 1 en caso de archivo corrupto, 2 en caso de memoria insuficiente, o -1 en caso de no haber podido abrir el archivo*/
int
recuperaJuego(tipoJuego * game)
{
	
	FILE * filePartida;
	int carga;
	int result = -1;
	
	if ( (filePartida = getFile()) != NULL )
    {
		carga = cargaDatos(game, filePartida);
		fclose(filePartida);
	
		switch (carga)
		{
			case -1 : result = 1; break;
			case  0 : result = 2; break;
			case  1 : cargaPos( game ); result = 0; break;
		}
	}
	
	return result;
}	
	


/*Toma el nombre del archivo y lo busca. Devuelve 1 si pudo tomar el nombre archivo y abrirlo, o 0 en caso contrario*/
FILE *
getFile(void)
{
	int flag;
	FILE * file;
	string fileName;
	
	do
	{
		flag = 0;
		
		fileName = getLine("Ingrese el nombre del archivo sin la extension: "); /*Pide el nombre del archivo*/
		fileName = strcat(fileName,FILE_EXT); /*Concatena la extension*/
		
		if ( (file = fopen(fileName,"rb")) == NULL )
			flag = funcionYesNo("Archivo no encontrado. Desea ingresar otro nombre?",'s','n');
	}
	while( flag ); 	/* Repite si el flag llego en 1 ( no existe el archivo, y quiso ingresar otro nombre)*/

	
	return file;
}


/*Devuelve 1 en caso de exito, 0 en caso de falta de memoria para generar el tablero con los datos obtenidos, o -1 en caso de archivo corrupto*/
int
cargaDatos( tipoJuego * game, FILE * filePartida)
{
    int i, j;
    int parametro;
    int cantCeldas = 0;
    int flag = 0;
    
    fread( &(game->modoJuego) , sizeof(int),1,filePartida); /*Lee el modo de juego*/
    if ( ((game->modoJuego) != 0) && ((game->modoJuego) != 1) ) /*Verifica el dato leido*/
        return -1;
    
    fread( &parametro,sizeof(int),1,filePartida); /*Lee el turno*/
    game->turno = ((parametro == 1)?JUGADOR1:JUGADOR2); /*Traduce el dato leido del archivo*/
    if ( ((game->turno) != JUGADOR1) && ((game->turno) != JUGADOR2) ) /*Verifica el dato leido*/
        return -1;
    
    fread( &(game->tablero.fils),sizeof(int),1,filePartida); /*Lee la cantidad de filas y asigna a la estructura*/
    fread( &(game->tablero.cols),sizeof(int),1,filePartida); /*Lee la cantidad de columnas y asigna a la estructura*/
    fread( &(game->N),sizeof(int),1,filePartida); /*Lee la cantidad de fichas necesarias para hacer linea*/
    if ( ((game->tablero.fils) <= (game->N)) || ((game->tablero.cols) <= (game->N)) ) /*Verifica los datos leidos*/
        return -1;
    
    if ( generarTablero( game ) != 0 )
        return 0;
    
    for ( i = 0 ; (i < game->tablero.fils) && (!flag) ; i++)
    {
        for ( j = 0 ; (j < game->tablero.cols) && (!flag) ; j++ )
        {
            
            fread( &(game->tablero.matriz[i][j]),sizeof(char),1,filePartida);
            
            if ( ((game->tablero.matriz[i][j]) != FICHA_J1) && ((game->tablero.matriz[i][j]) != FICHA_J2) && ((game->tablero.matriz[i][j]) != 0) ) /*Verifica el dato leido*/
                flag = 1;
            else
                cantCeldas++;
        }
    }
    if ( (cantCeldas) != ((game->tablero.fils) * (game->tablero.cols)) ) /*Verifica los datos obtenidos*/
        return -1;
    
    return 1;
}


/*Lleva el control del juego*/
void
motorJuego(tipoJuego * game)
{

	game->estado = JUEGO;
	
	printBoard( game );
	
	while (game->estado == JUEGO)
	{
		turnoDe( game );
		
		/*Toma la jugada, y si devolvio un cero (correspondiente a una q), sale*/
		if ( (game->modoJuego == DOS_JUGADORES) || ( game->turno == JUGADOR1))
		{
			if ( !juegaHumano( game ) )
				game->estado = SALIR;
		}	
		else
		{
			inteligenciaPC( game );
			printf("Jugada: %d\n",game->jugada);
		}

		printBoard( game );
		
		if ( (game->estado) != SALIR )
			finDeJugada( game );
	}
    freeBoard(game->tablero.matriz,game->tablero.fils);
	return;
}


/*Imprime el tablero con bordes*/
void
printBoard( tipoJuego * game)
{
	int i,j;
	
	printf("\n\n\n");
	
	for (i = 0 ; i < 4*(game->tablero.cols) ; i++ )
	{
		putchar( (( i % 4) == 0)?' ':'_');
	}
	
	putchar('\n');
	
	for (i = 0 ; i < 3*(game->tablero.fils) ; i++)
	{
		
		if ( (i % 3) == 0)
		{
			for (j = 0 ; j < 4*(game->tablero.cols) ;j++ )
				putchar(( (j % 4) == 0)?'|':' ');
		}
		else if ( (i % 3) == 1)
		{
			for (j = 0 ; j < 4*(game->tablero.cols) ;j++ )
			{
				if ((j % 4) == 0)
					putchar('|');
				else if ((j % 4) == 2)
					printf("%c",(game->tablero.matriz[(i/3)][(j/4)]==0)?' ':( (game->tablero.matriz[(i/3)][j/4]==FICHA_J1)?'X':'O'));
				else
					putchar(' ');
			}
		}
		else
		{
			for (j = 0 ; j < 4*(game->tablero.cols) ; j++ )
			{
				if ((j % 4) == 0)
					putchar('|');
				else
					putchar('_');
			}
		}
		putchar('|');
		putchar('\n');
		
	}
	
	putchar('\n');
	for (i = 1 ; i <= game->tablero.cols ; i++) /*Ciclo que imprime el numero de cada columna*/
	{
		if ( i < 10)
			printf("  %d ",i); /*Hasta 9 deja dos espacios antes de imprimir el numero*/
		else
			printf(" %d ",i);/*A partir de 10, deja un solo espacio*/
	}				 /*No hago verificaciones para 3 o mas digitos ya que esta limitado a 30 columnas*/
	printf("\n\n");
	
	return;
}


/*Imprime de quién es el turno*/		
void		
turnoDe(tipoJuego * game)
{
		
	switch ( game->modoJuego )
	{
		case DOS_JUGADORES : printf("Es el turno del %s\n",(game->turno == JUGADOR1)?"jugador 1":"jugador 2");break;
		
		case VS_PC : printf("Es el turno de%s\n",(game->turno == JUGADOR1)?"l jugador":" la PC");break;
	
		default : printf("Error inesperado\n");break;
	}
}



/*Funcion para tomar la jugada de una persona, poner en la jugada en el tablero y verificar si gana en dicha jugada*/
/*Devuelve 0 en caso de querer salir o 1 si pudo realizar todas las tareas*/
int
juegaHumano(tipoJuego * game)
{
	int flag;
	int salir;
	int guardar;
	
	do
	{
		flag = 1;
		salir = NO;
	
		game->jugada = getPlay( game );
	
		if ( game->jugada == 0)
		{
			flag = funcionYesNo("Esta seguro que quiere salir?",'s','n');
		
			if ( flag ) 
			{
				salir = SI;
				if ( funcionYesNo("Desea guardar la partida?",'s','n') )
				{
					guardar = guardarPartida( game );
					
					switch (guardar)  /*Guarda la partida. Retorna 0 en caso de exito, 1 en caso de "no guardar", 2 en caso de memoria insuficiente, o -1 en caso de error"*/
					{
						case  0 : printf("Se ha guardado la partida\n");break;
						
						case  1 : printf("No se ha guardado la partida\n");break;
						
						case  2 : printf("Memoria insuficiente para guardar la partida\n"); break;
						
						case -1 : printf("Hubo un error al intentar guardar la partida\n"); break;
					}
				}
			}
		}

	}
	while( !flag );

	if ( salir )
		return 0;
	
	
	putPlay( game ); /*Ingresa la jugada a la matriz*/

	checkWin( game ); /*Se fija si se gano en dicha jugada*/
	
	return 1;
	
}




/*Devuelve la columna a jugar, 0 si se quiere salir, o -1 en caso de memoria insuficiente para tomar la jugada. Verifica si la columna esta llena */
int
getPlay(tipoJuego * game)
{
	string sJugada;
	int jugada = 0;
	int flag, len;
	
	
	do
	{
		flag = 0; /*Establece el flag en 0. Flag con valor 1 significa jugada invalida. Flag con valor 2 significa columna llena. Flag con valor -1 significa memoria insuficiente*/
		
		if ( (sJugada = getLine("Ingrese jugada: ")) == NULL )
			flag = -1;
			
		else if ( !strcmp(sJugada,"Q") || !strcmp(sJugada,"q") )
			jugada = 0; 
			
		else if (  (len = (int)strlen(sJugada)) > 2  ) /*Asigno a len el largo del string. Esto me permite retirar el limite de dos digitos para las jugadas unicamente agregando un if a continuacion*/
			flag = 1;
	
		else if ( (len == 2) && ( (!isdigit(sJugada[0])) || (!isdigit(sJugada[1])) )  ) /*Si len es 2, y alguno de los caracteres del string no es un numero entra al if*/
			flag = 1;
		
		else if ( (len == 1) && ( !isdigit(sJugada[0]) ) ) /*Si len es 1 y el caracter no es un digito, entra al if*/
			flag = 1; 
		
		else if ( len == 0 ) /*Si el largo de la cadena es 0 (no tomo ningun caracter) entra al if*/
			flag = 1;
	
		else /*Entra si se ingreso bien la jugada (en este caso: un caracter digito o dos caracteres digito)*/
		{
			if ( (jugada = atoi(sJugada)) < 1 || jugada > game->tablero.cols ) /*Verifica si la jugada ingresada esta dentro del rango 1 - #columnas*/
				flag = 1;
			else if ( (game->tablero.pos[jugada-1] + 1) > game->tablero.fils ) /*Verifica si la columna jugada esta llena*/
				flag = 2;
		}
	
		if ( flag )
			printf("%s\n",(flag == -1)?"Memoria insuficiente":((flag == 1)?"Jugada invalida. Intente nuevamente":"Columna llena. Intente nuevamente"));
			
		if (flag != -1)
			free(sJugada);
	}
	while( flag && (flag != -1) ); /*Repite mientras que se haya prendido un flag pero que no es -1*/
	
	return jugada;
	
}



/*Lleva a cabo los ultimos procedimientos al terminar una jugada*/
/*Solo deberia ejecutarse si el estado del juego no es salir*/
void
finDeJugada( tipoJuego * game)
{
	if ( (game->estado) != GANA )
	{
		checkDraw( game );
		if ( game->estado == EMPATE)
			printf("Hubo un empate\n");
		else 
			game->turno = !game->turno;
	}
			
	else
		ganador(game);
}

/*Imprime el ganador*/
/*No chequea que el estado del juego sea GANA ya que unicamente la llamo si se cumple dicho estado*/
void		
ganador(tipoJuego * game)
{
		
	switch ( game->modoJuego )
	{
		case DOS_JUGADORES : printf("Gana el %s\n",(game->turno == JUGADOR1)?"jugador 1":"jugador 2");break;
		
		case VS_PC : printf("Gana %s\n",(game->turno == JUGADOR1)?"el jugador 1":"la PC");break;
	
		default : printf("Error inesperado\n");break;
	}
}


/*Guarda la partida. Retorna 0 en caso de exito, 1 en caso de "no guardar", 2 en caso de memoria insuficiente, o -1 en caso de error"*/
int
guardarPartida(tipoJuego * game)
{
    int flag = 0;
    int cant;
    int parametros[5];
    unsigned char * posiciones;
    FILE * filePartida;
    
    if ( (filePartida = newFile()) == NULL )
        return 1;
    
    if ( (posiciones = tomaPosiciones(game)) == NULL )
    {
        fclose(filePartida);
        return 2;
    }
        
    tomaParametros(game, parametros);
    cant = game->tablero.fils*game->tablero.cols;
    
    if ( !grabaDatos(filePartida, parametros, posiciones, cant) )
        flag = 1;
    
    free(posiciones);
    fclose(filePartida); 
    
    return flag?-1:0;
}




/*Pide el nombre de un archivo, y si este no existe, o si existe y se quiere reescribir, crea el nuevo archivo*/
/*Retorna el puntero a dicho archivo o NULL en caso de no haber creado archivo. Tambien puede retornar NULL en caso de no haber memoria para generar el string con el nombre del archivo*/
FILE *
newFile(void)
{
	int flag, guardar;
	FILE * file = NULL;
	string fileName;
	
	do
	{
		guardar = SI;
		flag = 0;	
		if ( (fileName = getLine("Ingrese el nombre del archivo: ")) != NULL )
		{
			if ( (fileName = strcat(fileName,FILE_EXT)) != NULL )
			{
				if ( existsFile(fileName) )
				{
					flag = !(funcionYesNo("Archivo existente. Desea sobreescribirlo?",'s','n'));
					if ( flag ) 
					{
						if ( !(funcionYesNo("Desea ingresar otro nombre?",'s','n') ) )
						{
							flag = 0;
							guardar = NO;
						}
						free(fileName);
					}
				}
			}
			else
				guardar = NO;
		}
		else
			guardar = NO;
	}
	while( flag );
	
	if ( guardar )
	{
		file = fopen(fileName,"wb");
		free(fileName);
	}
	
	return file;
}



/*Verifica si existe el archivo. Retorna 1 si existe, o 0 en caso contrario*/
int
existsFile(string fileName)
{
    int flag = 0;
    FILE * file;
    
    if ( (file = fopen(fileName,"r")) != NULL )
    {
        flag = 1;
        fclose(file);
    }
    return flag;
}


/*Toma los parametros del juego y los guarda en el vector parametros*/
void
tomaParametros(tipoJuego * game, int parametros[])
{
    
    parametros[0] = game->modoJuego;
    parametros[1] = (game->turno == JUGADOR1)?1:2;
    parametros[2] = game->tablero.fils;
    parametros[3] = game->tablero.cols;
    parametros[4] = game->N;
    
    return;
    
}

/*Crea un vector dinamico que contiene una secuencia de chars con el contenido del tablero, y retorna el puntero a dicho vector*/
unsigned char *
tomaPosiciones(tipoJuego * game)
{
    int i, j, k = 0;
    unsigned char * posiciones;
    
    if ( (posiciones = malloc((game->tablero.fils * game->tablero.cols) * sizeof(char))) != NULL )
    {
        for ( i = 0 ; i < game->tablero.fils ; i++)
        {
            for ( j = 0 ; j < game->tablero.cols ; j++ )
                posiciones[k++] = game->tablero.matriz[i][j];
            
        }
    }
    
    return posiciones;
}



/*Retorna 1 si pudo guardar todos los datos, o 0 en caso contrario*/
int
grabaDatos(FILE * file, int parametros[], unsigned char posiciones[], int cantPosiciones)
{
    
    if ( fwrite(parametros,sizeof(int),5,file) < 5 )
        return 0;
    
    if ( fwrite(posiciones,sizeof(char),cantPosiciones,file) < cantPosiciones )
        return 0;
    
    return 1;
}









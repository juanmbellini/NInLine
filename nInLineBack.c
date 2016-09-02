#include <stdlib.h>
#include <time.h>
#include "nInLineBack.h"


/*Esta funcion va sumando fichas en la direccion que se le pase a partir de la posicion de los subindices i,j */
static void acumula(tipoJuego * game, int dirFila, int dirColumna, int i, int j, int * acum);

/*Devuelve 1 si gano, o 0 si no gano*/
static int trataGanar(tipoJuego * game);

/*Devuelve 1 si tapo, o 0 si no tapo*/
static int taparGanada(tipoJuego * game);

/* Devuelve 1 en caso de exito, o 0 en caso de memoria insuficiente para generar el vector de jugadas posibles */
static int randPlay(tipoJuego * game);

/*Funcion que selecciona jugada al azar verificando, cuando sea posible, que dicha jugada sea jugada posible*/
static void selectPlay(tipoJuego * game, int jugadasPosibles[]);

/*Setea la semilla de rand en base al tiempo*/
static void randomize(void);

/*Retorna un valor entero al azar entre izq y der*/
static int randInt(int izq, int der);

/*Verifica si la columna a probar es jugada posible o no*/
/*Una jugada posible es aquella que unicamente le queda un casillero libre, o aquella que tiene dos o mas casilleros libres pero, que al poner una ficha, el jugador humano no gana en dicha jugada*/
/*Retorna 0 si es jugada posible (no verifica) o 1 si es jugada no posible (verifica)*/
static int verifica( tipoJuego * game);







/*Genera el tablero creando vectores dinamicos. Tambien crea un vector (llamado pos) que contiene en su i-esima posicion la cantidad de fichas que hay en la i-esima columna (contadas a partir de 0)*/
/*Devuelve 0 si salio todo correctamente, 1 si hubo error de dimensiones, y 2 si no habia memoria*/
/*Las asignaciones de memoria se hacen con calloc para inicializar todo en 0*/
int
generarTablero(tipoJuego * game)
{
	int i;

	if ( ((game->tablero.fils) <= game->N) || ((game->tablero.cols) <= game->N) )
		return 1;
	
	if ( (game->tablero.matriz = calloc( game->tablero.fils , sizeof(char*) )) == NULL )
		return 2;
	
	for ( i = 0 ; i < game->tablero.fils ; i++)
	{
		if ( (game->tablero.matriz[i] = calloc( game->tablero.cols , sizeof(char) )) == NULL ) /* Trata de generar el i-esimo vector dinamico. Si alguno no se pudo generar, libera los anteriores, y devuelve codigo 2*/
		{
			freeBoard(game->tablero.matriz,i);
			return 2;
		}
	}

	if ( (game->tablero.pos = calloc( game->tablero.cols , sizeof(int) )) == NULL )
	{										
		freeBoard(game->tablero.matriz , game->tablero.fils);
		return 2; /*Si no puede generar este vector por falta de memoria, no se puede llevar a cabo el juego*/
	}
		
	return 0;
}


/*Libera hasta n vectores dentro de un vector de punteros, y luego libera este ultimo vector. No se debe llamar con un n menor a la cantidad de vectores creados dentro del vector de punteros*/
/*Definirlo asi me facilita la tarea de liberar los n vectores creados al momento de generar el tablero en caso de no haber espacio para el vector n+1 */
void
freeBoard( unsigned char ** matriz , int n)
{
	int i;
	
	for (i = 0 ; i < n ; i++) 
		free( matriz[i] );
	free(matriz);
}




/*Carga el vector pos*/
void
cargaPos(tipoJuego * game)
{
	int i, j, k, flag;
	
	for ( j = 0 ; j < game->tablero.cols ; j++)
	{
		flag = 0;
		k = 0;
	
		for (i = 0 ; i < game->tablero.fils && !flag ; i++)	
		{
			if (game->tablero.matriz[i][j] == 0)
				 k++;
			else
				flag = 1;
		
		}
		game->tablero.pos[j] = game->tablero.fils - k;
	}
	
	return;
}



/*Ingresa en el tablero la jugada tomada. Por fuera unicamente habria que ejecutarla en caso se haber tomado bien la jugada ya que solo la ingresa*/
void
putPlay( tipoJuego * game )
{
	int i,j;
	
	/* Asigno a i y a j la fila y la columna a modificar, respectivamente, para facilitar el acceso al elemento de la matriz*/
	i = game->tablero.fils - game->tablero.pos[game->jugada - 1] - 1;
	j = game->jugada - 1;
	
	game->tablero.matriz[i][j] = ((game->turno == JUGADOR1)?FICHA_J1:FICHA_J2);
	
	game->tablero.pos[j]++;
	
	return;

}


/*Verifica si una jugada fue ganadora o no. Modifica el campo estado de la estructura de tipoJuego*/
void
checkWin(tipoJuego * game)
{
	int posFil, posCol, k, acum, dirFila, dirColumna;
	int direcciones[2][4]= { {1,0,1,1} , {1,1,-1,0} }; /*Matriz con las 4 direcciones posibles: vertical, horizontal, y las dos diagonales*/

	/*Se para en la posicion a analizar*/
	posFil = game->tablero.fils - game->tablero.pos[game->jugada - 1];
	posCol = game->jugada - 1;
	
	k = 0;

	do /*Ejecuta el loop para las 4 direcciones, repitiendose si no se llegaron a contar N fichas*/
	{	
		/*Setea variables de la funcion*/
		acum = 1; /* Inicia en 1 asi se cuenta la ultima ficha puesta*/
		dirFila = direcciones[0][k];
		dirColumna = direcciones[1][k];
		
		/* Cuenta en la direccion hasta llegar a N, hasta que no haya una ficha propia, o hasta salirse del tablero */
		acumula(game, dirFila, dirColumna, posFil, posCol, &acum);
		 
		if ( acum < game->N ) /*Si no llego a contar N fichas, cuenta en la direccion opuesta*/
			acumula(game, -1*dirFila, -1*dirColumna, posFil, posCol, &acum);
	
		/*Si llego a contar N fichas, entonces cambia el estado a GANA; en caso contrario, verifica en la siguiente direccion*/
		if ( acum == game->N ) 
			game->estado = GANA;
		else 	
			k++;
	}
	while( (game->estado == JUEGO) && (k < 4) );
	
	return;
}

/*Esta funcion va sumando fichas en la direccion que se le pase a partir de la posicion de los subindices i,j */
static void
acumula(tipoJuego * game, int dirFila, int dirColumna, int i, int j, int * acum)
{	
	int flag = 0;
	
	do
	{
		i += dirFila;
		j += dirColumna;
	
		if ( (i < 0) || (i > (game->tablero.fils - 1)) || (j < 0) || (j > (game->tablero.cols - 1)) )
			flag = 1;
	
		else if ( game->tablero.matriz[i][j] != ((game->turno==JUGADOR1)?FICHA_J1:FICHA_J2) )
			flag = 1;
		else
			(* acum)++;
	}
	while (!flag && ((* acum) < (game->N)) );
	
	return;
}

/*Recibe un puntero a tipoJuego y dentro de este busca el vector pos dentro del campo de tipoTablero y recorre todos sus elementos. En caso de que haya alguna columna que no este llena, corta el ciclo. En caso contrario, cambia el estado del juego a EMPATE*/
void
checkDraw( tipoJuego * game )
{
	int i;
	int hayLugar = NO;
	
	for (i = 0 ; (i < game->tablero.cols) && !hayLugar ; i++)
	{
		if ( game->tablero.pos[i] < game->tablero.fils )
			hayLugar = SI;
	}
	
	if (!hayLugar)
		game->estado = EMPATE;
	
	return;
}


/*Funcion que le da el control del juego a la computadora*/
void
inteligenciaPC(tipoJuego * game)
{
	int flag = 0;
	
	flag = trataGanar( game ); /*Trata de ganar en la siguiente jugada*/
	
	if ( !flag )		
		flag = taparGanada( game ); /*Trata de tapar la posicion faltante para ganar*/	

	if ( !flag )
		randPlay( game ); /*Pone ficha al azar verificando que, al poner dicha ficha, el jugador humano no gane, cuando sea posible*/	
	return;
}


/*Devuelve 1 si gano, o 0 si no gano*/
static int
trataGanar(tipoJuego * game)
{
	int i, j;
	int flag;
	
	for (j = 0 , flag = 0 ; (j < game->tablero.cols) && !flag ; j++ )
	{
		game->jugada = j + 1;
		
		if ( game->tablero.pos[j] < game->tablero.fils )
		{ 	
			putPlay(game);
		
			checkWin(game);
			
			game->tablero.pos[j]--;
			
			if ( game->estado == GANA )
				flag = 1;

			else 
			{
				i = game->tablero.fils - game->tablero.pos[j] - 1 ;
				game->tablero.matriz[i][j] = 0;
			}
		}
	}
	
	return flag;	
}

/*Devuelve 1 si tapo, o 0 si no tapo*/
static int
taparGanada(tipoJuego * game)
{
	int i, j;
	int flag = 0;

	game->turno = !game->turno; /*Cambia el turno para verificar con la ficha del jugador*/
	for ( j = 0 ; (j < game->tablero.cols) && !flag ; j++ )
	{
		game->jugada = j + 1;
		
		if ( game->tablero.pos[j] < game->tablero.fils )
		{ 	
			putPlay(game);
		
			checkWin(game);
			
			i = game->tablero.fils - game->tablero.pos[j];
			
			if ( game->estado == GANA )
			{
				flag = 1;
				game->tablero.matriz[i][j] = FICHA_J2;/*Cambia la ficha con la que probo por la ficha propia*/
				game->estado = JUEGO;
			}
			else
			{
				game->tablero.matriz[i][j] = 0; /*Saca la ficha con la que probo*/
				game->tablero.pos[j]--; /*Decrementa en 1 ya que se saco la ficha*/
			}
		}

	}
	game->turno = !game->turno; /*Vuelve a establecer el turno a turno de la PC para no perder generalidad*/
	return flag;
}


/* Devuelve 1 en caso de exito, o 0 en caso de memoria insuficiente para generar el vector de jugadas posibles */
static int
randPlay(tipoJuego * game)
{

	int i;
	int * jugadasPosibles;
	
	if ( (jugadasPosibles = malloc(game->tablero.cols * sizeof(int))) == NULL )
		return 0;
	
	for ( i = 1 ; i <= game->tablero.cols ; i++)
		jugadasPosibles[i - 1] = i;

	
	selectPlay( game , jugadasPosibles);
		
	putPlay( game );
	
	free(jugadasPosibles);
	
	return 1;
}


/*Funcion que selecciona jugada al azar verificando, cuando sea posible, que dicha jugada sea jugada posible*/
static void
selectPlay(tipoJuego * game, int jugadasPosibles[])
{
	int i;
	int flag;
	int dim = game->tablero.cols - 1;
	
	randomize();
	
	do
	{
		flag = 0;
		
		i = randInt(0,dim);
		game->jugada = jugadasPosibles[i];
		if ( verifica(game) )
		{
			jugadasPosibles[i] = jugadasPosibles[dim--];
			flag = 1;
		}
	}
	while(flag && dim >= 0);
	
	if (dim < 0)
	{
		do
			game->jugada = randInt(1,game->tablero.cols);
		while (game->tablero.pos[game->jugada - 1] == game->tablero.fils);
	}
	
	return;
}

/*Setea la semilla de rand en base al tiempo*/
static void
randomize(void)
{
	return srand((int)time(NULL));
}

/*Retorna un valor entero al azar entre izq y der*/
static int
randInt(int izq, int der)
{
	return (rand() % (der - izq + 1) + izq);
}


/*Verifica si la columna a probar es jugada posible o no*/
/*Una jugada posible es aquella que unicamente le queda un casillero libre, o aquella que tiene dos o mas casilleros libres pero, que al poner una ficha, el jugador humano no gana en dicha jugada*/
/*Retorna 0 si es jugada posible (no verifica) o 1 si es jugada no posible (verifica)*/
static int
verifica( tipoJuego * game)
{
	int i, j;
	int aux;
	int flag = 0;
	
	j = game->jugada - 1;

	if (game->tablero.pos[j] == game->tablero.fils)
		flag = 1;
	
	else if ( game->tablero.pos[j] <= game->tablero.fils - 2 )
	{

		aux = (game->tablero.pos[j])++; /*Guarda el valor del vector pos en la posicion j, y luego incrementa en 1 el contenido de dicho elemento*/
		
		i = game->tablero.fils - game->tablero.pos[j] - 1;

		game->turno = !game->turno; /*Cambia el turno para verificar con la ficha del jugador*/

		putPlay(game);

		checkWin(game);
	
		game->turno = !game->turno; /*Vuelve a establecer el turno a turno de la PC para luego hacer un putPlay con la ficha de la misma*/

		game->tablero.matriz[i][j] = 0;/*Saca la ficha con la que probo*/
	
		game->tablero.pos[j] = aux;/*Vuelve a establecer el valor del vector pos*/

		if ( game->estado == GANA )
		{
			flag = 1;
			game->estado = JUEGO;
		}
	}
	
	return flag;
}




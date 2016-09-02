
#ifndef _nInLineBack_h
#define _nInLineBack_h

#define JUEGO 0
#define GANA 1
#define EMPATE 2
#define SALIR -1

#define NO 0
#define SI 1

#define JUGADOR1 0
#define JUGADOR2 1

#define FICHA_J1 1
#define FICHA_J2 2



typedef struct {
	unsigned char **matriz;
	int fils;
	int cols;
	int *pos;
	} tipoTablero;
	

typedef struct {
	tipoTablero tablero;
	int N;
	int modoJuego;
	int turno;
	int estado;
	int jugada;	
	} tipoJuego;



/*Devuelve 0 si salio todo correctamente, 1 si hubo error de dimensiones, y 2 si no habia memoria*/
int generarTablero(tipoJuego * game);

/*Libera hasta n vectores dentro de un vector de punteros, y luego libera este ultimo vector. No se debe llamar con un n menor a la cantidad de vectores creados dentro del vector de punteros*/
/*Definirlo asi me facilita la tarea de liberar los n vectores creados al momento de generar el tablero en caso de no haber espacio para el vector n+1 */
void freeBoard( unsigned char ** matriz , int n);

/*Carga el vector pos*/
void cargaPos(tipoJuego * game);

/*Ingresa en el tablero la jugada tomada. Por fuera unicamente habria que ejecutarla en caso se haber tomado bien la jugada ya que solo la ingresa*/
void putPlay( tipoJuego * game );

/*Verifica si una jugada fue ganadora o no. Modifica el campo estado de la estructura de tipoJuego*/
void checkWin(tipoJuego * game);

/*Recibe un puntero a tipoJuego y dentro de este busca el vector pos dentro del campo de tipoTablero y recorre todos sus elementos. En caso de que haya alguna columna que no este llena, corta el ciclo*/
void checkDraw( tipoJuego * game );

/*Funcion que le da el control del juego a la computadora*/
void inteligenciaPC(tipoJuego * game);





#endif

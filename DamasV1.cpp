#include <iostream>
#include <cstdlib> //para abs()
#include <cctype>  //para tolower()
#include <string>  //es para el parametro de leerEntero

using namespace std;

//arreglo global para guardar el historial de partidas (máximo 100 partidas guardadas)
string historialPartidas[100];
int totalPartidasHistorial = 0;

//función para leer números de forma segura (evita que se quede en bucle si escriben letras o palabras)
int leerEntero(string mensaje) {
    int valor;
    while (true) {
        cout << mensaje;
        cin >> valor;
        
        //en caso de que el usuario ingree letras o simbolos, cualquier cosa que no sea numero
        if (cin.fail()) {
            cin.clear(); //restaura el estado de cin
            cin.ignore(10000, '\n'); //con esto descarta la entrada basura del búfer
            cout << "\nError, ese no es un numero valido\n\n";
        } else {
            cin.ignore(10000, '\n'); //aqui se limpia cualquier residuo, como espacios o saltos de línea
            return valor;
        }
    }
}

//función para mostrar el tablero en pantalla con puros números de 0 a 7
void mostrarTablero(char tablero[8][8], int turno) {
    cout << "\n    0   1   2   3   4   5   6   7\n";
    cout << "  ---------------------------------\n";
    for (int i = 0; i < 8; i++) {
        cout << i << " |";
        for (int j = 0; j < 8; j++) {
            cout << " " << tablero[i][j] << " |";
        }
        cout << " " << i << "\n";
        cout << "  ---------------------------------\n";
    }
    cout << "    0   1   2   3   4   5   6   7\n\n";

    if (turno == 1) {
        cout << "Turno del Jugador 1 (fichas claras)\n";
    } else {
        cout << "Turno del Jugador 2 (fichas oscuras)\n";
    }
}

//función para inicializar el tablero con las posiciones de las fichas
void inicializarTablero(char tablero[8][8]) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            //las damas se juegan en las casillas oscuras (suma de fila + columna impar)
            if ((i + j) % 2 != 0) {
                if (i < 3) {
                    tablero[i][j] = 'x'; //fichas oscuras arriba
                } else if (i > 4) {
                    tablero[i][j] = 'o'; //fichas claras abajo
                } else {
                    tablero[i][j] = '.'; //espacios vacíos del centro
                }
            } else {
                tablero[i][j] = '.'; //casillas claras del tablero
            }
        }
    }
}

//revisa si la ficha en (f,c) puede comer alguna ficha enemiga en alguna diagonal permitida
bool piezaPuedeComer(char tablero[8][8], int f, int c, int turno) {
    char ficha = tablero[f][c];
    if (ficha == '.') return false;

    int direcciones[4][2] = { {-2, -2}, {-2, 2}, {2, -2}, {2, 2} };

    for (int i = 0; i < 4; i++) {
        int df = direcciones[i][0];
        int dc = direcciones[i][1];

        //las fichas normales solo pueden comer hacia adelante (igual que se mueven)
        if (ficha == 'o' && df != -2) continue;
        if (ficha == 'x' && df != 2) continue;
        //las damas pueden comer en cualquier direccion

        int fDestino = f + df;
        int cDestino = c + dc;

        if (fDestino < 0 || fDestino > 7 || cDestino < 0 || cDestino > 7) continue;
        if (tablero[fDestino][cDestino] != '.') continue;

        int fIntermedia = (f + fDestino) / 2;
        int cIntermedia = (c + cDestino) / 2;
        char rival1 = (turno == 1) ? 'x' : 'o';
        char rival2 = (turno == 1) ? 'X' : 'O';

        if (tablero[fIntermedia][cIntermedia] == rival1 || tablero[fIntermedia][cIntermedia] == rival2) {
            return true;
        }
    }
    return false;
}

//revisa si el jugador en turno tiene AL MENOS una captura posible en todo el tablero
bool jugadorTieneCaptura(char tablero[8][8], int turno) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            char ficha = tablero[i][j];
            bool esDeEsteJugador = (turno == 1 && (ficha == 'o' || ficha == 'O')) ||
                                    (turno == 2 && (ficha == 'x' || ficha == 'X'));
            if (esDeEsteJugador && piezaPuedeComer(tablero, i, j, turno)) {
                return true;
            }
        }
    }
    return false;
}

//revisa si la ficha en (f,c) tiene algun movimiento simple valido (1 paso en diagonal)
bool piezaPuedeMoverSimple(char tablero[8][8], int f, int c) {
    char ficha = tablero[f][c];
    if (ficha == '.') return false;

    int direcciones[4][2] = { {-1, -1}, {-1, 1}, {1, -1}, {1, 1} };

    for (int i = 0; i < 4; i++) {
        int df = direcciones[i][0];
        int dc = direcciones[i][1];

        //las fichas normales solo avanzan hacia adelante
        if (ficha == 'o' && df != -1) continue;
        if (ficha == 'x' && df != 1) continue;

        int fDestino = f + df;
        int cDestino = c + dc;

        if (fDestino < 0 || fDestino > 7 || cDestino < 0 || cDestino > 7) continue;
        if (tablero[fDestino][cDestino] == '.') return true;
    }
    return false;
}

//revisa si el jugador en turno tiene algun movimiento disponible, ya sea simple o de captura
bool jugadorTieneMovimiento(char tablero[8][8], int turno) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            char ficha = tablero[i][j];
            bool esDeEsteJugador = (turno == 1 && (ficha == 'o' || ficha == 'O')) ||
                                    (turno == 2 && (ficha == 'x' || ficha == 'X'));
            if (esDeEsteJugador) {
                if (piezaPuedeComer(tablero, i, j, turno) || piezaPuedeMoverSimple(tablero, i, j)) {
                    return true;
                }
            }
        }
    }
    return false;
}

//función que ejecuta la partida completa
void ejecutarPartida() {
    char tablero[8][8];
    int turno = 1; // 1 = fichas claras y 2= fichas oscuras jeje
    bool juegoActivo = true;

    //variables para manejar la cadena de capturas obligatorias
    bool capturaEnCurso = false;
    int fFija = -1, cFija = -1;

    inicializarTablero(tablero);

    //bucle principal del juego
    while (juegoActivo) {
        mostrarTablero(tablero, turno);

        int fOrigen, cOrigen, fDestino, cDestino;

        if (capturaEnCurso) {
            //si estamos en medio de una cadena de capturas, el origen queda fijo
            fOrigen = fFija;
            cOrigen = cFija;
            cout << "\nTienes que seguir comiendo con la misma ficha, desde Fila: " << fOrigen << ", Columna: " << cOrigen << "\n";
        } else {
            //pedir origen y validarlo de inmediato antes de pedir el destino
            while (true) {
                cout << "Ingresa tu movimiento:\n";
                fOrigen = leerEntero("Fila de origen (0-7): ");
                cOrigen = leerEntero("Columna de origen (0-7): ");

                //validar límites del origen
                if (fOrigen < 0 || fOrigen > 7 || cOrigen < 0 || cOrigen > 7) {
                    cout << "\nError, coordenadas de origen fuera del tablero. Intenta de nuevo.\n\n";
                    continue;
                }

                char fichaSeleccionada = tablero[fOrigen][cOrigen];

                //validar que la casilla no esté vacía o sea un espacio no jugable
                if (fichaSeleccionada == '.') {
                    cout << "\nError, la casilla de origen está vacía. Selecciona una ficha tuya.\n\n";
                    continue;
                }

                //validar que la ficha pertenezca al jugador en turno
                if (turno == 1 && fichaSeleccionada != 'o' && fichaSeleccionada != 'O') {
                    cout << "\nError, esa ficha no es tuya. Las tuyas son 'o' o 'O'.\n\n";
                    continue;
                }
                if (turno == 2 && fichaSeleccionada != 'x' && fichaSeleccionada != 'X') {
                    cout << "\nError, esa ficha no es tuya. Las tuyas son 'x' o 'X'.\n\n";
                    continue;
                }

                //si pasó todas las validaciones del origen, salimos de este ciclo
                break;
            }
        }

        //ahora pedimos el destino de forma segura
        fDestino = leerEntero("Fila de destino (0-7): ");
        cDestino = leerEntero("Columna de destino (0-7): ");

        //validar límites del destino
        if (fDestino < 0 || fDestino > 7 || cDestino < 0 || cDestino > 7) {
            cout << "\nError, coordenadas de destino fuera del tablero. Intenta de nuevo.\n";
            continue;
        }

        char fichaActual = tablero[fOrigen][cOrigen];
        char casillaDestino = tablero[fDestino][cDestino];

        //validar que la casilla de destino esté vacía
        if (casillaDestino != '.') {
            cout << "\nError, la casilla de destino está ocupada. Intenta de nuevo.\n";
            continue;
        }

        int diffFila = fDestino - fOrigen;
        int diffCol = abs(cDestino - cOrigen);
        bool esCaptura = (abs(diffFila) == 2 && diffCol == 2);

        //regla de captura obligatoria
        if (!capturaEnCurso) {
            bool debeComer = jugadorTieneCaptura(tablero, turno);
            if (debeComer && !esCaptura) {
                cout << "\nError, tienes captura obligatoria, tienes que comer con alguna ficha.\n";
                continue;
            }
        } else {
            if (!esCaptura) {
                cout << "\nError, estas en secuencia de captura, tienes que seguir comiendo.\n";
                continue;
            }
        }

        //EL MOVIIMIENTO SIMPLE
        if (!esCaptura) {
            if (abs(diffFila) == 1 && diffCol == 1) {
                if (fichaActual == 'o' && diffFila != -1) {
                    cout << "\nError, las fichas normales solo se mueven hacia adelante.\n";
                    continue;
                }
                if (fichaActual == 'x' && diffFila != 1) {
                    cout << "\nError, las fichas normales solo se mueven hacia adelante.\n";
                    continue;
                }

                tablero[fDestino][cDestino] = fichaActual;
                tablero[fOrigen][cOrigen] = '.';
            } else {
                cout << "\nError, movimiento no permitido. Solo puedes mover 1 casilla en diagonal.\n";
                continue;
            }
        }
        //EL MOVIMIENTO DE CAPTURAAAAAA
        else {
            if (fichaActual == 'o' && diffFila != -2) {
                cout << "\nError, las fichas normales solo pueden comer hacia adelante.\n";
                continue;
            }
            if (fichaActual == 'x' && diffFila != 2) {
                cout << "\nError, las fichas normales solo pueden comer hacia adelante.\n";
                continue;
            }

            int fIntermedia = (fOrigen + fDestino) / 2;
            int cIntermedia = (cOrigen + cDestino) / 2;
            char rival1 = (turno == 1) ? 'x' : 'o';
            char rival2 = (turno == 1) ? 'X' : 'O';

            if (tablero[fIntermedia][cIntermedia] == rival1 || tablero[fIntermedia][cIntermedia] == rival2) {
                tablero[fDestino][cDestino] = fichaActual;
                tablero[fOrigen][cOrigen] = '.';
                tablero[fIntermedia][cIntermedia] = '.';
            } else {
                cout << "\nError, no hay ninguna ficha enemiga para saltar en esa diagonal.\n";
                continue;
            }
        }

        //REGLA DE CORONACION
        if (fichaActual == 'o' && fDestino == 0) {
            tablero[fDestino][cDestino] = 'O';
            cout << "\nUna ficha clara se coronó como Dama ('O')\n";
        }
        if (fichaActual == 'x' && fDestino == 7) {
            tablero[fDestino][cDestino] = 'X';
            cout << "\nUna ficha oscura se coronó como Dama ('X')\n";
        }

        //     VERIFICAR SI HAY QUE SEGUIR COMIENDO CON LA MISMA FICHA
        if (esCaptura && piezaPuedeComer(tablero, fDestino, cDestino, turno)) {
            capturaEnCurso = true;
            fFija = fDestino;
            cFija = cDestino;
            cout << "\nPuedes seguir comiendo con esa misma ficha\n";
            continue;
        } else {
            capturaEnCurso = false;
        }

        //     VERIFICAR FIN DEL JUEGO POR FICHAS RESTANTES
        int totalClaras = 0;
        int totalOscuras = 0;
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                if (tablero[i][j] == 'o' || tablero[i][j] == 'O') totalClaras++;
                if (tablero[i][j] == 'x' || tablero[i][j] == 'X') totalOscuras++;
            }
        }

        if (totalClaras == 0) {
            mostrarTablero(tablero, turno);
            cout << "\n¡Juego terminado! El jugador 2 (oscuras) gana la partida.\n";
            if (totalPartidasHistorial < 100) {
                historialPartidas[totalPartidasHistorial] = "Partida " + to_string(totalPartidasHistorial + 1) + ": Gana el Jugador 2 (oscuras)";
                totalPartidasHistorial++;
            }
            juegoActivo = false;
            break;
        }
        if (totalOscuras == 0) {
            mostrarTablero(tablero, turno);
            cout << "\n¡Juego terminado! El jugador 1 (claras) gana la partida.\n";
            if (totalPartidasHistorial < 100) {
                historialPartidas[totalPartidasHistorial] = "Partida " + to_string(totalPartidasHistorial + 1) + ": Gana el Jugador 1 (claras)";
                totalPartidasHistorial++;
            }
            juegoActivo = false;
            break;
        }

        //     PREGUNTA PARA RENDIRSE 
        char respuestaRendirse;
        cout << "\n¿Prefieres rendirte? (s/n): ";
        cin >> respuestaRendirse;
        respuestaRendirse = tolower(respuestaRendirse);
        if (respuestaRendirse == 's') {
            juegoActivo = false;
            if (turno == 1) {
                cout << "\nEl jugador 1 se ha rendido. ¡Gana el Jugador 2!\n";
                if (totalPartidasHistorial < 100) {
                    historialPartidas[totalPartidasHistorial] = "Partida " + to_string(totalPartidasHistorial + 1) + ": Gana el Jugador 2 (Jugador 1 se rindio)";
                    totalPartidasHistorial++;
                }
            } else {
                cout << "\nEl jugador 2 se ha rendido. ¡Gana el Jugador 1!\n";
                if (totalPartidasHistorial < 100) {
                    historialPartidas[totalPartidasHistorial] = "Partida " + to_string(totalPartidasHistorial + 1) + ": Gana el Jugador 1 (Jugador 2 se rindio)";
                    totalPartidasHistorial++;
                }
            }
            break;
        }

        //cambiar de turno
        turno = (turno == 1) ? 2 : 1;

        //     VERIFICAR FIN DEL JUEGO POR JUGADOR ACORRALADO
        if (!jugadorTieneMovimiento(tablero, turno)) {
            mostrarTablero(tablero, turno);
            if (turno == 1) {
                cout << "\n¡Juego terminado! El Jugador 1 quedó acorralado sin movimientos. Gana el Jugador 2.\n";
                if (totalPartidasHistorial < 100) {
                    historialPartidas[totalPartidasHistorial] = "Partida " + to_string(totalPartidasHistorial + 1) + ": Gana el Jugador 2 (Jugador 1 acorralado)";
                    totalPartidasHistorial++;
                }
            } else {
                cout << "\n¡Juego terminado! El Jugador 2 quedó acorralado sin movimientos. Gana el Jugador 1.\n";
                if (totalPartidasHistorial < 100) {
                    historialPartidas[totalPartidasHistorial] = "Partida " + to_string(totalPartidasHistorial + 1) + ": Gana el Jugador 1 (Jugador 2 acorralado)";
                    totalPartidasHistorial++;
                }
            }
            juegoActivo = false;
        }
    }
}

int main() {
    int opcion = 0;

    while (opcion != 3) {
        cout << "\n-----------JUEGO DE LAS DAMAS-----------\n";
        cout << "1. Comenzar partida\n";
        cout << "2. Historial de partidas\n";
        cout << "3. Salir\n";
        opcion = leerEntero("Selecciona una opcion: ");

        if (opcion == 1) {
            cout << "\n---INICIANDO PARTIDA---\n";
            ejecutarPartida();
        } else if (opcion == 2) {
            cout << "\n--- HISTORIAL DE PARTIDAS ---\n";
            if (totalPartidasHistorial == 0) {
                cout << "Todavia no hay partidas registradas.\n";
            } else {
                for (int i = 0; i < totalPartidasHistorial; i++) {
                    cout << historialPartidas[i] << endl;
                }
            }
        } else if (opcion == 3) {
            cout << "\nSaliendo del juego...\n";
        } else {
            cout << "\nError, opcion invalida.\n";
        }
    }

    return 0;
}
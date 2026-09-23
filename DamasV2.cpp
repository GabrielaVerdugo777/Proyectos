#include <iostream>
#include <cstdlib> // para usar abs y system("pause")
#include <cctype>  // por si se rinden (tolower)
#include <string>  // para el historial de partidas y las direcciones

using namespace std;

// colores para las fichas en la terminal
const string rosa = "\033[38;5;213m";
const string verde = "\033[38;5;46m";
const string reset = "\033[0m";

// arreglo para el historial general de partidas
string hist[100];
int totalPartidas = 0;

// muestra el tablero en pantalla
void mostrarTab(char tab[8][8], int turno) {
    cout << "\n    0   1   2   3   4   5   6   7\n";
    cout << "  ---------------------------------\n";
    for (int i = 0; i < 8; i++) {
        cout << i << " |";
        for (int j = 0; j < 8; j++) {
            char f = tab[i][j];
            // pintamos de rosa las fichas 'o' y de verde las 'x'
            if (f == 'o' || f == 'O') {
                cout << " " << rosa << f << reset << " |";
            } else if (f == 'x' || f == 'X') {
                cout << " " << verde << f << reset << " |";
            } else {
                cout << " " << f << " |";
            }
        }
        cout << " " << i << "\n";
        cout << "  ---------------------------------\n";
    }
    cout << "    0   1   2   3   4   5   6   7\n\n";

    if (turno == 1) {
        cout << "Turno del jugador 1 (fichas rosas o)\n";
    } else {
        cout << "Turno del jugador 2 (fichas verdes x)\n";
    }
}

// coloca las fichas iniciales en el tablero
void iniciarTab(char tab[8][8]) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if ((i + j) % 2 != 0) {
                if (i < 3) {
                    tab[i][j] = 'x'; // fichas verdes arriba
                } else if (i > 4) {
                    tab[i][j] = 'o'; // fichas rosas abajo
                } else {
                    tab[i][j] = '.'; // espacios vacios del centro
                }
            } else {
                tab[i][j] = '.'; // casillas claras
            }
        }
    }
}

// comprueba si una ficha puede comer saltando
bool puedeCom(char tab[8][8], int f, int c, int turno) {
    char ficha = tab[f][c];
    if (ficha == '.') {
        return false;
    }

    int dirs[4][2] = { {-2, -2}, {-2, 2}, {2, -2}, {2, 2} };
    for (int i = 0; i < 4; i++) {
        int df = dirs[i][0];
        int dc = dirs[i][1];

        // las fichas normales solo comen hacia adelante, la dama no tiene restriccion
        if (ficha == 'o' && df != -2) {
            continue;
        }
        if (ficha == 'x' && df != 2) {
            continue;
        }

        int fDest = f + df;
        int cDest = c + dc;

        if (fDest < 0 || fDest > 7 || cDest < 0 || cDest > 7) {
            continue;
        }
        if (tab[fDest][cDest] != '.') {
            continue;
        }

        int fInter = (f + fDest) / 2;
        int cInter = (c + cDest) / 2;

        char r1, r2;
        if (turno == 1) {
            r1 = 'x';
            r2 = 'X';
        } else {
            r1 = 'o';
            r2 = 'O';
        }

        if (tab[fInter][cInter] == r1 || tab[fInter][cInter] == r2) {
            return true;
        }
    }
    return false;
}

// revisa si el jugador tiene alguna captura obligatoria
bool tieneCap(char tab[8][8], int turno) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            char ficha = tab[i][j];

            bool mio = false;
            if (turno == 1 && (ficha == 'o' || ficha == 'O')) {
                mio = true;
            }
            if (turno == 2 && (ficha == 'x' || ficha == 'X')) {
                mio = true;
            }

            if (mio && puedeCom(tab, i, j, turno)) {
                return true;
            }
        }
    }
    return false;
}

// revisa si una ficha puede hacer un movimiento simple
bool puedeMov(char tab[8][8], int f, int c) {
    char ficha = tab[f][c];
    if (ficha == '.') {
        return false;
    }

    int dirs[4][2] = { {-1, -1}, {-1, 1}, {1, -1}, {1, 1} };

    for (int i = 0; i < 4; i++) {
        int df = dirs[i][0];
        int dc = dirs[i][1];

        if (ficha == 'o' && df != -1) {
            continue;
        }
        if (ficha == 'x' && df != 1) {
            continue;
        }

        int fDest = f + df;
        int cDest = c + dc;

        if (fDest < 0 || fDest > 7 || cDest < 0 || cDest > 7) {
            continue;
        }
        if (tab[fDest][cDest] == '.') {
            return true;
        }
    }
    return false;
}

// revisa si al jugador le queda algun movimiento
bool tieneMov(char tab[8][8], int turno) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            char ficha = tab[i][j];

            bool mio = false;
            if (turno == 1 && (ficha == 'o' || ficha == 'O')) {
                mio = true;
            }
            if (turno == 2 && (ficha == 'x' || ficha == 'X')) {
                mio = true;
            }

            if (mio) {
                if (puedeCom(tab, i, j, turno) || puedeMov(tab, i, j)) {
                    return true;
                }
            }
        }
    }
    return false;
}

// funcion principal para jugar cada partida turno por turno
void jugar() {
    char tab[8][8];
    int turno = 1;
    bool activo = true;

    // variables para manejar la cadena de capturas
    bool capCurso = false;
    int fFija = -1;
    int cFija = -1;

    // Historial detallado de movimientos para esta partida específica
    string movsPartida = "";
    int numMovimiento = 1;

    iniciarTab(tab);

    while (activo) {
        mostrarTab(tab, turno);

        // revisamos de una vez si hay captura obligatoria en el tablero
        bool debeComer = tieneCap(tab, turno);

        // avisa si hay fichas que pueden comer obligatoriamente
        if (debeComer && !capCurso) {
            cout << "Tienes captura obligatoria con la siguiente ficha:\n";
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    char ficha = tab[i][j];

                    bool mio = false;
                    if (turno == 1 && (ficha == 'o' || ficha == 'O')) {
                        mio = true;
                    }
                    if (turno == 2 && (ficha == 'x' || ficha == 'X')) {
                        mio = true;
                    }

                    if (mio && puedeCom(tab, i, j, turno)) {
                        cout << " Fila " << i << ", columna: " << j << " puede comer\n";
                    }
                }
            }
            cout << "Estas obligado a comer\n\n";
        }

        int fOrig, cOrig;

        if (capCurso) {
            fOrig = fFija;
            cOrig = cFija;
            // si por alguna razón la ficha en capCurso ya no puede comer, rompemos el bucle de captura para evitar loops
            if (!puedeCom(tab, fOrig, cOrig, turno)) {
                cout << "\nLa ficha ya no tiene mas saltos posibles\n";
                capCurso = false;
                turno = (turno == 1) ? 2 : 1;
                continue;
            }
            cout << "Sigues comiendo con la misma ficha en fila " << fOrig << ", columna " << cOrig << "\n";
        } else {
            cout << "Ingresa tu origen (fila y columna): ";
            cin >> fOrig >> cOrig;

            // validar limites del origen
            if (fOrig < 0 || fOrig > 7 || cOrig < 0 || cOrig > 7) {
                cout << "\nError, coordenadas de origen fuera del tablero\n\n";
                continue;
            }

            char fichaOrig = tab[fOrig][cOrig];

            // validar que no este vacia
            if (fichaOrig == '.') {
                cout << "\nError, ahi no hay ninguna ficha\n\n";
                continue;
            }

            // validar que la ficha sea del jugador en turno
            if (turno == 1 && fichaOrig != 'o' && fichaOrig != 'O') {
                cout << "\nError, esa ficha no es tuya\n\n";
                continue;
            }
            if (turno == 2 && fichaOrig != 'x' && fichaOrig != 'X') {
                cout << "\nError, esa ficha no es tuya\n\n";
                continue;
            }

            // si hay captura obligatoria, esta ficha en concreto tiene que poder comer
            if (debeComer && !puedeCom(tab, fOrig, cOrig, turno)) {
                cout << "\nError, esa ficha no puede comer, elige una de las que se marcaron arriba\n\n";
                continue;
            }
        }

        char fichaAct = tab[fOrig][cOrig];

        // preguntamos adelante/atras solo si es dama
        int dirVer;
        if (fichaAct == 'O' || fichaAct == 'X') {
            string vert;
            cout << "Hacia adelante o hacia atras? (adelante/ade/atras): ";
            cin >> vert;

            if (vert == "adelante" || vert == "ade") {
                dirVer = (turno == 1) ? -1 : 1;
            } else if (vert == "atras") {
                dirVer = (turno == 1) ? 1 : -1;
            } else {
                cout << "\nError, escribe adelante/ade o atras.\n\n";
                continue;
            }
        } else {
            if (fichaAct == 'o') {
                dirVer = -1;
            } else {
                dirVer = 1;
            }
        }

        string horiz;
        cout << "Hacia la izquierda o hacia la derecha? (izq/der): ";
        cin >> horiz;

        int dirHor;
        if (horiz == "izquierda" || horiz == "izq") {
            dirHor = -1;
        } else if (horiz == "derecha" || horiz == "der") {
            dirHor = 1;
        } else {
            cout << "\nError, escribe izq/izquierda o der/derecha.\n\n";
            continue;
        }

        // si hay que comer el salto es de 2 casillas, si no pues es de 1
        int distancia;
        if (capCurso || debeComer) {
            distancia = 2;
        } else {
            distancia = 1;
        }

        int fDest = fOrig + dirVer * distancia;
        int cDest = cOrig + dirHor * distancia;

        // validar limites del destino
        if (fDest < 0 || fDest > 7 || cDest < 0 || cDest > 7) {
            cout << "\nError, por ahi te sales del tablero.\n\n";
            continue;
        }

        char casillaDest = tab[fDest][cDest];

        // validar casilla vacia
        if (casillaDest != '.') {
            cout << "\nError, la casilla de destino esta ocupada.\n\n";
            continue;
        }

        bool esCap = (distancia == 2);

        // movimiento simple o captura
        if (!esCap) {
            tab[fDest][cDest] = fichaAct;
            tab[fOrig][cOrig] = '.';
        } else {
            int fInter = (fOrig + fDest) / 2;
            int cInter = (cOrig + cDest) / 2;

            char r1, r2;
            if (turno == 1) {
                r1 = 'x';
                r2 = 'X';
            } else {
                r1 = 'o';
                r2 = 'O';
            }

            if (tab[fInter][cInter] == r1 || tab[fInter][cInter] == r2) {
                tab[fDest][cDest] = fichaAct;
                tab[fOrig][cOrig] = '.';
                tab[fInter][cInter] = '.';
            } else {
                cout << "\nError, no hay ficha rival para saltar hacia ese lado.\n\n";
                continue;
            }
        }

        // coronacion si llega al extremo
        if (fichaAct == 'o' && fDest == 0) {
            tab[fDest][cDest] = 'O';
            cout << "\nUna ficha rosa se corono como dama\n";
        }
        if (fichaAct == 'x' && fDest == 7) {
            tab[fDest][cDest] = 'X';
            cout << "\nUna ficha verde se corono como dama\n";
        }

        // registrar el movimiento en el historial de la partida
        string movDesc = "Jugador " + to_string(turno) + ": (" + to_string(fOrig) + "," + to_string(cOrig) + ") hacia (" + to_string(fDest) + "," + to_string(cDest) + ")";
        if (esCap) {
            movDesc += " (captura)";
        }
        movsPartida += "  " + to_string(numMovimiento) + ". " + movDesc + "\n";
        numMovimiento++;

        // verificar si puede seguir comiendo con la misma ficha
        if (esCap && puedeCom(tab, fDest, cDest, turno)) {
            capCurso = true;
            fFija = fDest;
            cFija = cDest;
            cout << "\nPuedes seguir comiendo con la misma ficha\n";
            
            // pausa entre turnos y saltos
            cout << "\nPresiona Enter para continuar al siguiente salto...";
            system("pause");
            continue;
        } else {
            capCurso = false;
        }

        // contar fichas para ver quien gana
        int rosas = 0;
        int verdes = 0;
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                if (tab[i][j] == 'o' || tab[i][j] == 'O') {
                    rosas++;
                }
                if (tab[i][j] == 'x' || tab[i][j] == 'X') {
                    verdes++;
                }
            }
        }

        if (rosas == 0) {
            mostrarTab(tab, turno);
            cout << "\nJuego terminado, gana el jugador 2 (fichas verdes)\n";
            if (totalPartidas < 100) {
                hist[totalPartidas] = "Partida " + to_string(totalPartidas + 1) + ": Gana Jugador 2\nMovimientos:\n" + movsPartida;
                totalPartidas++;
            }
            activo = false;
            break;
        }
        if (verdes == 0) {
            mostrarTab(tab, turno);
            cout << "\nJuego terminado, gana el Jugador 1 (fichas rosas)\n";
            if (totalPartidas < 100) {
                hist[totalPartidas] = "Partida " + to_string(totalPartidas + 1) + ": Gana Jugador 1\nMovimientos:\n" + movsPartida;
                totalPartidas++;
            }
            activo = false;
            break;
        }

        // preguntar si se quieren rendir antes de pasar el turno
        char rendirse;
        cout << "\nQuieres rendirte? (s/n): ";
        cin >> rendirse;
        rendirse = tolower(rendirse);
        if (rendirse == 's') {
            if (turno == 1) {
                cout << "\nEl jugador 1 se rindio, gana el jugador 2\n";
                if (totalPartidas < 100) {
                    hist[totalPartidas] = "Partida " + to_string(totalPartidas + 1) + ": Gana Jugador 2 (Rendicion)\nMovimientos:\n" + movsPartida;
                    totalPartidas++;
                }
            } else {
                cout << "\nEl jugador 2 se rindio, gana el jugador 1\n";
                if (totalPartidas < 100) {
                    hist[totalPartidas] = "Partida " + to_string(totalPartidas + 1) + ": Gana Jugador 1 (Rendicion)\nMovimientos:\n" + movsPartida;
                    totalPartidas++;
                }
            }
            activo = false;
            break;
        }

        // cambiar de turno
        if (turno == 1) {
            turno = 2;
        } else {
            turno = 1;
        }

        // verificar si el jugador se queda sin movimientos
        if (!tieneMov(tab, turno)) {
            mostrarTab(tab, turno);
            cout << "\nJuego terminado, jugador acorralado sin movimientos\n";
            if (totalPartidas < 100) {
                if (turno == 1) {
                    hist[totalPartidas] = "Partida " + to_string(totalPartidas + 1) + ": Gana Jugador 2 (Acorralado)\nMovimientos:\n" + movsPartida;
                } else {
                    hist[totalPartidas] = "Partida " + to_string(totalPartidas + 1) + ": Gana Jugador 1 (Acorralado)\nMovimientos:\n" + movsPartida;
                }
                totalPartidas++;
            }
            activo = false;
            break;
        }

        // pausa al finalizar cada turno completito
        cout << "\nFin del turno. Presiona Enter para pasar al siguiente jugador...";
        system("pause");
    }
}

int main() {
    int opcion = 0;

    while (opcion != 3) {
        cout << "\n----------- JUEGO DE LAS DAMAS -----------\n";
        cout << "1. Comenzar partida\n";
        cout << "2. Historial de partidas\n";
        cout << "3. Salir\n";
        cout << "Selecciona una opcion: ";
        cin >> opcion;

        if (opcion == 1) {
            jugar();
        } else if (opcion == 2) {
            cout << "\n--- HISTORIAL DE PARTIDAS ---\n";
            if (totalPartidas == 0) {
                cout << "Todavia no hay partidas registradas.\n";
            } else {
                for (int i = 0; i < totalPartidas; i++) {
                    cout << "----------------------------------------\n";
                    cout << hist[i] << endl;
                }
                cout << "----------------------------------------\n";
            }
        } else if (opcion == 3) {
            cout << "\nSaliendo del juego...\n";
        } else {
            cout << "\nOpcion no valida.\n";
        }
    }

    return 0;
}
#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>

#define IZQUIERDO 0
#define DERECHO 1

class Nodo {
public:
    int valor, FE;
    Nodo* hijo[2];
    Nodo* padre;

    Nodo(int v, Nodo* p = nullptr) : valor(v), padre(p), FE(0) {
        hijo[0] = hijo[1] = nullptr;
    }
};

typedef Nodo* pNodo;
typedef pNodo Arbol;

class AVL {
private:
    Arbol raiz;

public:
    AVL() : raiz(nullptr) {}

    Arbol getRaiz() const { return raiz; }

    bool insertar(int valor) {
        pNodo padre = nullptr, actual = raiz;
        int rama = IZQUIERDO;

        while (actual) {
            if (valor == actual->valor) return false;
            padre = actual;
            rama = (valor > actual->valor) ? DERECHO : IZQUIERDO;
            actual = actual->hijo[rama];
        }

        pNodo nuevo = new Nodo(valor, padre);
        if (!padre) raiz = nuevo;
        else padre->hijo[rama] = nuevo;

        Equilibrar(raiz, padre, rama, true);
        return true;
    }

    void Equilibrar(Arbol& a, pNodo nodo, int rama, bool nuevo) {
        bool salir = false;

        while (nodo && !salir) {
            if (nuevo)
                nodo->FE += (rama == DERECHO) ? 1 : -1;
            else
                nodo->FE += (rama == DERECHO) ? -1 : 1;

            if (nodo->FE == 0) salir = true;
            else if (nodo->FE == -2) {
                if (nodo->hijo[IZQUIERDO]->FE == 1) RDD(a, nodo);
                else RSD(a, nodo);
                salir = true;
            }
            else if (nodo->FE == 2) {
                if (nodo->hijo[DERECHO]->FE == -1) RDI(a, nodo);
                else RSI(a, nodo);
                salir = true;
            }

            if (nodo->padre)
                rama = (nodo->padre->hijo[DERECHO] == nodo) ? DERECHO : IZQUIERDO;
            nodo = nodo->padre;
        }
    }

    void RSD(Arbol& a, pNodo nodo) {
        pNodo Padre = nodo->padre;
        pNodo P = nodo;
        pNodo Q = P->hijo[IZQUIERDO];
        pNodo B = Q->hijo[DERECHO];

        if (Padre)
            Padre->hijo[(Padre->hijo[DERECHO] == P) ? DERECHO : IZQUIERDO] = Q;
        else
            a = Q;

        P->hijo[IZQUIERDO] = B;
        Q->hijo[DERECHO] = P;

        P->padre = Q;
        if (B) B->padre = P;
        Q->padre = Padre;

        P->FE = Q->FE = 0;
    }

    void RSI(Arbol& a, pNodo nodo) {
        pNodo Padre = nodo->padre;
        pNodo P = nodo;
        pNodo Q = P->hijo[DERECHO];
        pNodo B = Q->hijo[IZQUIERDO];

        if (Padre)
            Padre->hijo[(Padre->hijo[DERECHO] == P) ? DERECHO : IZQUIERDO] = Q;
        else
            a = Q;

        P->hijo[DERECHO] = B;
        Q->hijo[IZQUIERDO] = P;

        P->padre = Q;
        if (B) B->padre = P;
        Q->padre = Padre;

        P->FE = Q->FE = 0;
    }

    void RDD(Arbol& a, pNodo nodo) {
        pNodo Padre = nodo->padre;
        pNodo P = nodo;
        pNodo Q = P->hijo[IZQUIERDO];
        pNodo R = Q->hijo[DERECHO];
        pNodo B = R->hijo[IZQUIERDO];
        pNodo C = R->hijo[DERECHO];

        if (Padre)
            Padre->hijo[(Padre->hijo[DERECHO] == P) ? DERECHO : IZQUIERDO] = R;
        else
            a = R;

        Q->hijo[DERECHO] = B;
        P->hijo[IZQUIERDO] = C;
        R->hijo[IZQUIERDO] = Q;
        R->hijo[DERECHO] = P;

        R->padre = Padre;
        Q->padre = P->padre = R;
        if (B) B->padre = Q;
        if (C) C->padre = P;

        switch (R->FE) {
        case -1: Q->FE = 0; P->FE = 1; break;
        case  0: Q->FE = 0; P->FE = 0; break;
        case  1: Q->FE = -1; P->FE = 0; break;
        }
        R->FE = 0;
    }

    void RDI(Arbol& a, pNodo nodo) {
        pNodo Padre = nodo->padre;
        pNodo P = nodo;
        pNodo Q = P->hijo[DERECHO];
        pNodo R = Q->hijo[IZQUIERDO];
        pNodo B = R->hijo[IZQUIERDO];
        pNodo C = R->hijo[DERECHO];

        if (Padre)
            Padre->hijo[(Padre->hijo[DERECHO] == P) ? DERECHO : IZQUIERDO] = R;
        else
            a = R;

        Q->hijo[IZQUIERDO] = C;
        P->hijo[DERECHO] = B;
        R->hijo[IZQUIERDO] = P;
        R->hijo[DERECHO] = Q;

        R->padre = Padre;
        Q->padre = P->padre = R;
        if (B) B->padre = P;
        if (C) C->padre = Q;

        switch (R->FE) {
        case -1: P->FE = 0; Q->FE = 1; break;
        case  0: P->FE = 0; Q->FE = 0; break;
        case  1: P->FE = -1; Q->FE = 0; break;
        }
        R->FE = 0;
    }
};

// Clase para visualizar el árbol con SFML
class Graficador {
private:
    sf::Font fuente;

public:
    Graficador() {
        if (!fuente.loadFromFile("Sansation_Light.ttf")) {
            std::cerr << "No se pudo cargar la fuente.\n";
        }
    }

    void graficar(sf::RenderWindow& window, pNodo nodo, float x, float y, float dx) {
        if (!nodo) return;
        float childY = y + 80;

        for (int i = 0; i < 2; ++i) {
            if (nodo->hijo[i]) {
                float childX = x + (i == 0 ? -dx : dx);
                dibujarLinea(window, x, y, childX, childY);
                graficar(window, nodo->hijo[i], childX, childY, dx / 1.5f);
            }
        }

        sf::CircleShape circle(20);
        circle.setFillColor(sf::Color::Black);
        circle.setOutlineThickness(2);
        circle.setOutlineColor(sf::Color::White);
        circle.setPosition(x - 20, y - 20);
        window.draw(circle);

        sf::Text texto;
        texto.setFont(fuente);
        texto.setCharacterSize(16);
        texto.setFillColor(sf::Color::White);
        texto.setString(std::to_string(nodo->valor));
        texto.setPosition(x - 10, y - 10);
        window.draw(texto);
    }

private:
    void dibujarLinea(sf::RenderWindow& window, float x1, float y1, float x2, float y2) {
        sf::Vertex linea[] = {
            sf::Vertex(sf::Vector2f(x1, y1), sf::Color::White),
            sf::Vertex(sf::Vector2f(x2, y2), sf::Color::White)
        };
        window.draw(linea, 2, sf::Lines);
    }
};

int main() {
    AVL arbol;
    arbol.insertar(50);
    arbol.insertar(30);
    arbol.insertar(70);
    arbol.insertar(20);
    arbol.insertar(40);
    arbol.insertar(60);
    arbol.insertar(80);
    arbol.insertar(10); // Provoca rotaciones
    arbol.insertar(25);
    arbol.insertar(5);
    arbol.insertar(8);
    arbol.insertar(18);


    sf::RenderWindow ventana(sf::VideoMode(800, 600), "Árbol AVL con SFML");
    Graficador graficador;

    while (ventana.isOpen()) {
        sf::Event evento;
        while (ventana.pollEvent(evento)) {
            if (evento.type == sf::Event::Closed)
                ventana.close();
        }

        ventana.clear(sf::Color::Black);
        graficador.graficar(ventana, arbol.getRaiz(), 400, 100, 150);// nodos estan muy cerca
        ventana.display();
    }

    return 0;
}

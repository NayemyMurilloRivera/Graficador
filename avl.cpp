#include <SFML/Graphics.hpp>
#include <iostream>

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

    int altura(pNodo nodo) {
        if (!nodo) return 0;
        int hIzq = altura(nodo->hijo[IZQUIERDO]);
        int hDer = altura(nodo->hijo[DERECHO]);
        return 1 + std::max(hIzq, hDer);
    }

    void actualizarFE(pNodo nodo) {
        int hIzq = altura(nodo->hijo[IZQUIERDO]);
        int hDer = altura(nodo->hijo[DERECHO]);
        nodo->FE = hDer - hIzq;
    }

    pNodo rotarIzquierda(pNodo x) {
        pNodo y = x->hijo[DERECHO];
        pNodo T2 = y->hijo[IZQUIERDO];

        y->hijo[IZQUIERDO] = x;
        x->hijo[DERECHO] = T2;

        if (T2) T2->padre = x;

        y->padre = x->padre;
        x->padre = y;

        actualizarFE(x);
        actualizarFE(y);

        if (!y->padre) raiz = y;

        return y;
    }

    pNodo rotarDerecha(pNodo y) {
        pNodo x = y->hijo[IZQUIERDO];
        pNodo T2 = x->hijo[DERECHO];

        x->hijo[DERECHO] = y;
        y->hijo[IZQUIERDO] = T2;

        if (T2) T2->padre = y;

        x->padre = y->padre;
        y->padre = x;

        actualizarFE(y);
        actualizarFE(x);

        if (!x->padre) raiz = x;

        return x;
    }

    pNodo balancear(pNodo nodo) {
        actualizarFE(nodo);

        if (nodo->FE < -1) {
            if (nodo->hijo[IZQUIERDO] && nodo->hijo[IZQUIERDO]->FE <= 0)
                return rotarDerecha(nodo); // LL
            else {
                nodo->hijo[IZQUIERDO] = rotarIzquierda(nodo->hijo[IZQUIERDO]);
                nodo->hijo[IZQUIERDO]->padre = nodo;
                return rotarDerecha(nodo); // LR
            }
        }

        if (nodo->FE > 1) {
            if (nodo->hijo[DERECHO] && nodo->hijo[DERECHO]->FE >= 0)
                return rotarIzquierda(nodo); // RR
            else {
                nodo->hijo[DERECHO] = rotarDerecha(nodo->hijo[DERECHO]);
                nodo->hijo[DERECHO]->padre = nodo;
                return rotarIzquierda(nodo); // RL
            }
        }

        return nodo;
    }

    void balancearDesde(pNodo nodo) {
        while (nodo) {
            nodo = balancear(nodo);
            if (!nodo->padre)
                raiz = nodo; // por si rotamos en la raíz
            nodo = nodo->padre;
        }
    }

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
        if (!padre)
            raiz = nuevo;
        else
            padre->hijo[rama] = nuevo;

        balancearDesde(padre);
        return true;
    }
};

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

    sf::RenderWindow ventana(sf::VideoMode(800, 600), "Arbol AVL con SFML");
    Graficador graficador;

    while (ventana.isOpen()) {
        sf::Event evento;
        while (ventana.pollEvent(evento)) {
            if (evento.type == sf::Event::Closed)
                ventana.close();
        }

        ventana.clear(sf::Color::Black);
        graficador.graficar(ventana, arbol.getRaiz(), 400, 100, 150);
        ventana.display();
    }

    return 0;
}


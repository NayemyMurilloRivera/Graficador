#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>

#define IZQUIERDO 0
#define DERECHO 1
#define MAX_NODOS 100

class Nodo {
public:
    int valor;
    Nodo* hijo[2];
    Nodo* padre;

    Nodo(int v) : valor(v), padre(nullptr) {
        hijo[0] = hijo[1] = nullptr;
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

    void graficar(sf::RenderWindow& window, Nodo* nodo, float x, float y, float dx) {
        if (!nodo) return;
        float childY = y + 80;

        for (int i = 0; i < 2; ++i) {
            if (nodo->hijo[i]) {
                float childX = x + (i == 0 ? -dx : dx);
                dibujarLinea(window, x, y, childX, childY);
                graficar(window, nodo->hijo[i], childX, childY, dx * 0.5f);
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

void papa(Nodo* izq, Nodo* der, Nodo** destino) {
    Nodo* padre = new Nodo(izq->valor + der->valor);
    padre->hijo[IZQUIERDO] = izq;
    padre->hijo[DERECHO] = der;
    izq->padre = padre;
    der->padre = padre;
    *destino = padre;
}

Nodo* conhilo(const int valores[], int n) {
    Nodo* niveles[2][MAX_NODOS];
    int tam_actual = n;

    for (int i = 0; i < n; i++) {
        niveles[0][i] = new Nodo(valores[i]);
    }

    int nivel = 0;

    while (tam_actual > 1) {
        int sig_nivel = (nivel + 1) % 2;
        int tam_siguiente = 0;
        std::thread hilos[MAX_NODOS];
        Nodo* resultados[MAX_NODOS];

        for (int i = 0; i < tam_actual; i += 2) {
            Nodo* izq = niveles[nivel][i];
            Nodo* der = nullptr;

            if (i + 1 < tam_actual) {
                der = niveles[nivel][i + 1];
                hilos[tam_siguiente] = std::thread(papa, izq, der, &resultados[tam_siguiente]);
            }
            else {
                resultados[tam_siguiente] = izq;
            }

            tam_siguiente++;
        }

        for (int i = 0; i < tam_siguiente; ++i) {
            if (hilos[i].joinable())
                hilos[i].join();
            niveles[sig_nivel][i] = resultados[i];
        }

        nivel = sig_nivel;
        tam_actual = tam_siguiente;
    }

    return niveles[nivel][0];
}

int main() {
    int arreglo[] = { 1, 4, 9, 6, 7, 2, 10, 11, 13, 8, 12, 14, 15, 13, 23, 27 };

    int n = sizeof(arreglo) / sizeof(arreglo[0]);

    Nodo* raiz = conhilo(arreglo, n);

    sf::RenderWindow ventana(sf::VideoMode(1000, 700), "Árbol de Sumas con Threads");
    Graficador graficador;

    while (ventana.isOpen()) {
        sf::Event evento;
        while (ventana.pollEvent(evento)) {
            if (evento.type == sf::Event::Closed)
                ventana.close();
        }

        ventana.clear(sf::Color::Black);
        graficador.graficar(ventana, raiz, 500, 100, 250);
        ventana.display();
    }

    return 0;
}

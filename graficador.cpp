#include <SFML/Graphics.hpp>
#include <sstream>

// Nodo del árbol binario
class Nodo {
public:
    int valor;
    Nodo* hijo[2];
    Nodo* padre;

    Nodo(int v, Nodo* p = nullptr) : valor(v), padre(p) {
        hijo[0] = nullptr;
        hijo[1] = nullptr;
    }
};

// Árbol binario con inserción y búsqueda
class Arbol {
private:
    Nodo* raiz;

public:
    Arbol() : raiz(nullptr) {}

    Nodo* getRaiz() const { return raiz; }

    bool buscar(int valorBuscado, Nodo**& puntero) {
        for (puntero = &raiz;
             *puntero && (*puntero)->valor != valorBuscado;
             puntero = &((*puntero)->hijo[valorBuscado > (*puntero)->valor]));
        return *puntero != nullptr;
    }

    Nodo* buscarNodo(int valorBuscado) {
        Nodo* actual = raiz;
        while (actual && actual->valor != valorBuscado) {
            actual = actual->hijo[valorBuscado > actual->valor];
        }
        return actual;
    }

    bool insertar(int valorNuevo) {
        Nodo** puntero;
        Nodo* padre = nullptr;

        for (puntero = &raiz;
             *puntero && (*puntero)->valor != valorNuevo;
             puntero = &((*puntero)->hijo[valorNuevo > (*puntero)->valor])) {
            padre = *puntero;
        }

        if (*puntero) return false;

        *puntero = new Nodo(valorNuevo, padre);
        return true;
    }
};

// Graficador del árbol con SFML
class Graficador {
private:
    sf::Font fuente;

public:
    Graficador() {
        fuente.loadFromFile("Sansation_Light.ttf");
    }

    void graficar(sf::RenderWindow& window, Nodo* nodo, float x, float y, float dx, int nivel,
                  Nodo* seleccionado,
                  Nodo* tio1, Nodo* tio2,
                  Nodo* primo1, Nodo* primo2, Nodo* primo3, Nodo* primo4) {
        if (!nodo) return;

        float childY = y + 80;

        if (nodo->hijo[0]) {
            float childX = x - dx;
            dibujarLinea(window, x, y, childX, childY);
            graficar(window, nodo->hijo[0], childX, childY, dx / 1.8f, nivel + 1,
                     seleccionado, tio1, tio2, primo1, primo2, primo3, primo4);
        }

        if (nodo->hijo[1]) {
            float childX = x + dx;
            dibujarLinea(window, x, y, childX, childY);
            graficar(window, nodo->hijo[1], childX, childY, dx / 1.8f, nivel + 1,
                     seleccionado, tio1, tio2, primo1, primo2, primo3, primo4);
        }

        sf::Color color = sf::Color(128, 128, 128); // gris por defecto
        if (nodo == seleccionado) color = sf::Color::Yellow;
        else if (nodo == tio1 || nodo == tio2) color = sf::Color::Red;
        else if (nodo == primo1 || nodo == primo2 || nodo == primo3 || nodo == primo4) color = sf::Color::Green;

        sf::CircleShape circle(20);
        circle.setFillColor(sf::Color::Black);
        circle.setOutlineThickness(2);
        circle.setOutlineColor(color);
        circle.setPosition(x - 20, y - 20);
        window.draw(circle);

        sf::Text texto;
        texto.setFont(fuente);
        texto.setCharacterSize(16);
        texto.setFillColor(sf::Color::White);
        texto.setString(std::to_string(nodo->valor));

        sf::FloatRect bounds = texto.getLocalBounds();
        texto.setOrigin(bounds.width / 2, bounds.height / 2);
        texto.setPosition(x, y);
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

// Función para obtener tíos y primos de un nodo
void obtenerTiosYPrimos(Nodo* nodo,
                        Nodo*& tio1, Nodo*& tio2,
                        Nodo*& primo1, Nodo*& primo2, Nodo*& primo3, Nodo*& primo4) {
    tio1 = tio2 = primo1 = primo2 = primo3 = primo4 = nullptr;

    if (!nodo || !nodo->padre || !nodo->padre->padre) return;

    Nodo* abuelo = nodo->padre->padre;

    for (int j = 0; j < 2; ++j) {
        Nodo* posibleTio = abuelo->hijo[j];
        if (posibleTio && posibleTio != nodo->padre) {
            if (!tio1) tio1 = posibleTio;
            else tio2 = posibleTio;

            if (posibleTio->hijo[0]) {
                if (!primo1) primo1 = posibleTio->hijo[0];
                else if (!primo2) primo2 = posibleTio->hijo[0];
            }
            if (posibleTio->hijo[1]) {
                if (!primo3) primo3 = posibleTio->hijo[1];
                else if (!primo4) primo4 = posibleTio->hijo[1];
            }
        }
    }
}

int main() {
    Arbol arbol;
    arbol.insertar(5);
    arbol.insertar(3);
    arbol.insertar(7);
    arbol.insertar(2);
    arbol.insertar(4);
    arbol.insertar(6);
    arbol.insertar(8);

    Nodo* seleccionado = arbol.buscarNodo(6); // Nodo seleccionado (amarillo)

    Nodo* tio1, *tio2, *primo1, *primo2, *primo3, *primo4;
    obtenerTiosYPrimos(seleccionado, tio1, tio2, primo1, primo2, primo3, primo4);

    sf::RenderWindow ventana(sf::VideoMode(800, 600), "Árbol Binario - Tíos y Primos");
    Graficador graficador;

    while (ventana.isOpen()) {
        sf::Event evento;
        while (ventana.pollEvent(evento)) {
            if (evento.type == sf::Event::Closed)
                ventana.close();
        }

        ventana.clear(sf::Color::Black);
        graficador.graficar(ventana, arbol.getRaiz(), 400, 50, 150, 0,
                            seleccionado, tio1, tio2, primo1, primo2, primo3, primo4);
        ventana.display();
    }

    return 0;
}

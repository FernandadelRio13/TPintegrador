#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace sf;

const int MAX_SPRITES = 5;
const std::vector<Vector2f> POSICIONES = {
    {275, 171}, {407, 171}, {520, 171},
    {127, 405}, {285, 405}, {521, 405}
};

class Personaje {
protected:
    Texture textura;
    Sprite sprite;
    Clock relojCreacion;
    bool activo;

public:
    Personaje() : activo(true) {}

    void inicializar(const std::string& archivo, const Vector2f& posicion) {
        if (!textura.loadFromFile(archivo)) {
            std::cerr << "Error al cargar la textura: " << archivo << std::endl;
        }
        sprite.setTexture(textura);
        sprite.setScale(0.1f, 0.1f);

        FloatRect bounds = sprite.getGlobalBounds();

        sprite.setPosition(posicion.x - bounds.width / 2, posicion.y - bounds.height / 2);

        relojCreacion.restart();
        activo = true;
    }

    void dibujar(RenderWindow& ventana) {
        if (activo) {
            ventana.draw(sprite);
        }
    }

    bool fueTocado(const Vector2i& mousePos) {
        if (!activo) return false;
        FloatRect bounds = sprite.getGlobalBounds();
        return bounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
    }

    bool tiempoSuperado(float limite) {
        return relojCreacion.getElapsedTime().asSeconds() >= limite;
    }

    void desactivar() {
        activo = false;
    }

    bool estaActivo() const {
        return activo;
    }

    const Sprite& getSprite() const {
        return sprite;
    }
};

class Enemigo : public Personaje {
public:
    Enemigo(const Vector2f& posicion) {
        inicializar("Vaquero malo.png", posicion);
    }
};

class Aliado : public Personaje {
public:
    Aliado(const Vector2f& posicion) {
        inicializar("Vaquero bueno.png", posicion);
    }
};

int main() {
    RenderWindow App(VideoMode(800, 600), "Tp Integrador");
    App.setFramerateLimit(60);

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    Texture fondo_t;
    Sprite fondo;
    if (!fondo_t.loadFromFile("menu.jpg")) {
        std::cerr << "Error al cargar la textura del fondo inicial." << std::endl;
        return -1;
    }
    fondo.setTexture(fondo_t);

    Texture gameOver_t;
    Sprite gameOverFondo;
    if (!gameOver_t.loadFromFile("final.jpg")) {
        std::cerr << "Error al cargar la textura del fondo de Game Over." << std::endl;
        return -1;
    }
    gameOverFondo.setTexture(gameOver_t);

    std::vector<Enemigo> enemigos;
    std::vector<Aliado> aliados;

    int puntaje = 0;
    int enemigosCount = 0;
    int aliadosCount = 0;
    int vidas = 3;
    bool showVida = false;
    bool iniciarJuego = false;
    bool gameOver = false;

    Font font;
    if (!font.loadFromFile("ALGER.ttf")) {
        std::cerr << "Error al cargar la fuente." << std::endl;
        return -1;
    }

    Text enemigosText, aliadoText, puntajeText;
    enemigosText.setFont(font);
    aliadoText.setFont(font);
    puntajeText.setFont(font);
    enemigosText.setCharacterSize(30);
    aliadoText.setCharacterSize(30);
    puntajeText.setCharacterSize(150);
    enemigosText.setFillColor(Color::White);
    aliadoText.setFillColor(Color::White);
    puntajeText.setFillColor(Color::White);
    enemigosText.setPosition(250, 515);
    aliadoText.setPosition(250, 550);
    puntajeText.setPosition(350, 350);
    enemigosText.setString(std::to_string(enemigosCount));
    aliadoText.setString(std::to_string(aliadosCount));
    puntajeText.setString(std::to_string(puntaje));

    Texture vida_t;
    Sprite vidasSprites[3];
    if (!vida_t.loadFromFile("vida.png")) {
        std::cerr << "Error al cargar la textura de vida." << std::endl;
        return -1;
    }
    for (int i = 0; i < 3; ++i) {
        vidasSprites[i].setTexture(vida_t);
        vidasSprites[i].setScale(0.2f, 0.2f);
        vidasSprites[i].setPosition(720 - i * 40, 530);
    }

    Clock reloj;
    float tiempoTranscurrido = 0.0f;
    float intervalo = 2.0f;

    while (App.isOpen()) {
        Event evt;
        while (App.pollEvent(evt)) {
            switch (evt.type) {
            case sf::Event::Closed:
                App.close();
                break;
            case sf::Event::KeyPressed:
                if (evt.key.code == sf::Keyboard::Escape) {
                    App.close();
                }
                if (evt.key.code == sf::Keyboard::Enter && !gameOver) {
                    if (!fondo_t.loadFromFile("juego.png")) {
                        std::cerr << "Error al cargar la textura del fondo de juego." << std::endl;
                    }
                    else {
                        fondo.setTexture(fondo_t);
                    }
                    iniciarJuego = true;
                    showVida = true;
                }
                break;
            case sf::Event::MouseButtonPressed:
                if (!gameOver && evt.mouseButton.button == sf::Mouse::Left) {
                    Vector2i mousePos = Mouse::getPosition(App);
                    for (auto& enemigo : enemigos) {
                        if (enemigo.estaActivo() && enemigo.fueTocado(mousePos)) {
                            enemigo.desactivar();
                            enemigosCount++;
                            enemigosText.setString(std::to_string(enemigosCount));

                            puntaje = std::max(enemigosCount - (aliadosCount * 2), 0);
                            puntajeText.setString(std::to_string(puntaje));
                            break;
                        }
                    }
                    for (auto& aliado : aliados) {
                        if (aliado.estaActivo() && aliado.fueTocado(mousePos)) {
                            aliado.desactivar();
                            aliadosCount++;
                            aliadoText.setString(std::to_string(aliadosCount));

                            puntaje = std::max(enemigosCount - (aliadosCount * 2), 0);
                            puntajeText.setString(std::to_string(puntaje));

                            if (vidas > 0) {
                                vidas--;
                                vidasSprites[vidas].setColor(Color::Transparent);
                                if (vidas == 0) {
                                    gameOver = true;
                                }
                            }
                            break;
                        }
                    }
                }
                break;
            }
        }

        if (iniciarJuego && !gameOver) {
            tiempoTranscurrido = reloj.getElapsedTime().asSeconds();

            if (tiempoTranscurrido >= intervalo && vidas > 0) {
                Vector2f posicion;
                bool posicionValida;
                do {
                    posicion = POSICIONES[std::rand() % POSICIONES.size()];
                    posicionValida = true;

                    for (const auto& enemigo : enemigos) {
                        if (enemigo.estaActivo() && enemigo.getSprite().getGlobalBounds().contains(posicion)) {
                            posicionValida = false;
                            break;
                        }
                    }

                    for (const auto& aliado : aliados) {
                        if (aliado.estaActivo() && aliado.getSprite().getGlobalBounds().contains(posicion)) {
                            posicionValida = false;
                            break;
                        }
                    }
                } while (!posicionValida);

                if (std::rand() % 2 == 0) {
                    enemigos.emplace_back(posicion);
                }
                else {
                    aliados.emplace_back(posicion);
                }
                reloj.restart();
            }

            for (auto it = enemigos.begin(); it != enemigos.end();) {
                if (it->estaActivo() && it->tiempoSuperado(5.0f)) {
                    it->desactivar();
                    it = enemigos.erase(it);
                    vidas--;
                    if (vidas >= 0) {
                        vidasSprites[vidas].setColor(Color::Transparent);
                    }
                    if (vidas <= 0) {
                        gameOver = true;
                    }
                }
                else {
                    ++it;
                }
            }

            for (auto it = aliados.begin(); it != aliados.end();) {
                if (it->estaActivo() && it->tiempoSuperado(3.0f)) {
                    it->desactivar();
                    it = aliados.erase(it);
                }
                else {
                    ++it;
                }
            }
        }

        App.clear();
        if (gameOver) {
            App.draw(gameOverFondo);
            App.draw(puntajeText);
        }
        else {
            for (auto& enemigo : enemigos) {
                enemigo.dibujar(App);
            }
            for (auto& aliado : aliados) {
                aliado.dibujar(App);
            }
            App.draw(fondo);
            if (showVida) {
                for (int i = 0; i < 3; ++i) {
                    App.draw(vidasSprites[i]);
                }
                App.draw(enemigosText);
                App.draw(aliadoText);
            }
        }
        App.display();
    }

    return 0;
}
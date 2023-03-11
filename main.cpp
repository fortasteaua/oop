// grupa 151 Marcu Dragos Ionut
// tema 1 lab poo

#include <SFML/Graphics.hpp>
#include <utility>
#include <iostream>


class Entity {
    int tip = 0;
    std::string ent_nume="def";
    sf::Texture ent_txtr;
public:
    Entity() = default;
    Entity(std::string  nume, int tip_) : tip { tip_ }, ent_nume{std::move( nume )} {}
    Entity(const sf::Texture& texture_, std::string  nume, int tip_) : tip{ tip_ }, ent_nume{std::move( nume )}, ent_txtr { texture_ } {}
    Entity(const Entity& other) : tip{ other.tip }, ent_nume{ other.ent_nume }, ent_txtr{ other.ent_txtr } {}
    Entity& operator=(const Entity& other) {
        tip = other.tip;
        ent_nume = other.ent_nume;
        ent_txtr = other.ent_txtr;
        return *this;
    }
    ~Entity() = default;
    friend std::ostream& operator<<(std::ostream& os, const Entity& ent) {
        os << " Nume " << ent.ent_nume << " Tip " << ent.tip << '\n';
        return os;
    }
    int getTip() const { return tip; }
    sf::Texture& getTexture(){
        return ent_txtr;
    }

};

class Player {
    double poz_x, poz_y; // pozitie pe harta
    int jump = 0, podea = 0;
    std::vector<Entity> inv;
    sf::Texture player_txtr;
public:
    explicit Player(const sf::Texture& texture_, double poz_x_ = 0.0, double poz_y_ = 0.0) : poz_x { poz_x_ }, poz_y{ poz_y_ }, player_txtr{ texture_ } {}
    friend std::ostream& operator<<(std::ostream& os, const Player& plr) {
        os << " Poz player: " << plr.poz_x << ' ' << plr.poz_y << '\n';
        return os;
    }
    void AddItem(const Entity& e) {
        if (e.getTip() == 1 && inv.size() < 10)
        {
            inv.push_back(e);
        }
    }
    double getlowHeight(){
        return poz_y - double(player_txtr.getSize().y);
    }

    void setPodea(int podea_){
        podea = podea_;
    }

    void Inv(std::vector<Entity>& vc)
    {
        for (auto& el : inv)
        {
            vc.push_back(el);
            std::cout << "copiere " << *(vc.rbegin()) << '\n';
        }
    }
    sf::Sprite getSprite()
    {
        sf::Sprite sp;
        sp.setTexture(player_txtr);
        double spoz_x = poz_x, spoz_y = poz_y;
        if(podea == 0 && jump <= 0) poz_y += 1;
        if(jump > 3){
            poz_y -= 3;
        }
        if(jump > 0) jump--;
        sp.setPosition(spoz_x, spoz_y);
        return sp;
    }
    sf::FloatRect getBoundingbox(){
        sf::Sprite playersprite = getSprite();
        playersprite.setPosition(playersprite.getPosition().x, playersprite.getPosition().y + 3);
        return playersprite.getGlobalBounds();
    }
    void Command(const std::string& c){
        if(c=="jump" && jump == 0 && podea == 1) jump = 20;
        if(c=="a") poz_x -= 2;
        if(c=="d") poz_x += 2;
    }

};

class Npc {
    double orpoz_x, poz_x, poz_y;
    double dir = 1;
    std::string order = "idle";
    sf::Texture npc_txtr;
public:
    explicit Npc(const sf::Texture& txtr_, double poz_x_ = 0.0, double poz_y_ = 0.0)
            : orpoz_x{poz_x_ }, poz_x{ poz_x_ }, poz_y{ poz_y_ }, npc_txtr { txtr_ } {}
    friend std::ostream& operator<<(std::ostream& os, const Npc& npc) {
        os << " Poz npc: " << npc.poz_x << ' ' << npc.poz_y << '\n';
        return os;
    }
    sf::Sprite getSprite()
    {
        sf::Sprite sp;
        sp.setTexture(npc_txtr);
        double spoz_x = poz_x, spoz_y = poz_y, mvvar = 1.5;
        if(order=="patrol"){
            if(std::abs(orpoz_x-spoz_x) > 80) dir *= -1;
            spoz_x += dir*mvvar;
            poz_x = spoz_x;
        }
        sp.setPosition(spoz_x, spoz_y);
        return sp;
    }
    void setOrder(const std::string& ord){
        order = ord;
    }
};

class Harta{
    Player& plr;
    std::vector< std::pair< Entity, std::pair<double,double> > > Obj;
public:
    explicit Harta(Player& p) : plr(p) {}

    friend std::ostream& operator<<(std::ostream& os, const Harta& h) {
        os << " Nr obiecte: " << h.Obj.size() << '\n';
        return os;
    }

    void addObj(const Entity& obj, std::pair<double,double> p){
        Obj.emplace_back(obj,p);
    }
    void drawMap(sf::RenderWindow& window_){
        sf::FloatRect pb = plr.getBoundingbox();
        int podea = 0;
        for(auto& it:Obj){
            sf::Sprite sp;
            sp.setTexture(it.first.getTexture());
            sp.setPosition(it.second.first, it.second.second); //second.first = x second.second = y
            std::cout << plr.getlowHeight() << '\n';
            if( it.second.second >= plr.getlowHeight() ){
                std::cout<<"b";
                if( sp.getGlobalBounds().intersects(pb) ){
                    std::cout << "a";
                    podea = 1;
                }
            }
            window_.draw(sp);
        }
        plr.setPodea(podea);
    }
};

int main()
{
    sf::RenderWindow window(sf::VideoMode(640, 360), "", sf::Style::Close);
    window.setFramerateLimit(60);

    sf::Event event;

    sf::RectangleShape bg;
    bg.setSize(sf::Vector2f(500, 500));
    bg.setPosition(0, 0);
    bg.setFillColor(sf::Color::Black);

    sf::Texture p_txtr, n1_txtr, floor1_txtr;
    if (!p_txtr.loadFromFile("p.png")) std::cout << "p txtr\n";
    if (!n1_txtr.loadFromFile("n1.png")) std::cout << "n1 txtr\n";
    if (!floor1_txtr.loadFromFile("floor1.png")) std::cout << "floor1 txtr\n";

    Player p{p_txtr, 250.0, 200.0};

    Harta h(p);
    Entity floor1{floor1_txtr, "floor1", 2};
    for(double i = 0; i <= 700; i+=40){
        h.addObj(floor1, {i, 300.0});
    }

    Entity e1{"e1", 1};
    p.AddItem(e1);
    std::vector<Entity> v;
    p.Inv(v);
    for (const auto & i : v) std::cout << i << '\n';

    Npc n1{n1_txtr, 200, 60};
    n1.setOrder("patrol");
    while (window.isOpen()) {

        while (window.pollEvent(event)) {

            if (event.type == sf::Event::Closed || ( event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) ) {
                window.close();
            }
        }
        if(window.hasFocus()) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) p.Command("jump");
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) p.Command("d");
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) p.Command("a");
        }
        h.drawMap(window);
        window.draw(n1.getSprite());
        window.draw(p.getSprite());
        window.display();
        window.clear();
    }

    return 0;
}
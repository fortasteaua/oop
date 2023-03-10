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
    int jump = 0, podea = 0, doublejump = 1, jumpcd = 0;
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

    int getJumpCD(){
        return jumpcd;
    }

    void setJumpCD(int jumpcd_){
        jumpcd = jumpcd_;
    }

    void Inv(std::vector<Entity>& vc)
    {
        for (auto& el : inv)
        {
            vc.push_back(el);
            std::cout << "copiere " << *(vc.rbegin()) << '\n';
        }
    }
    sf::Sprite getSprite(const std::string& caz="render")
    {
        sf::Sprite sp;
        sp.setTexture(player_txtr);
        if(caz=="render") {
            if (podea == 0 && jump == 0) poz_y += 2;
            if (jump > 5) {
                poz_y -= 4;
            }
            if (jump > 0) jump--;
            if (podea == 1) doublejump = 0;
        }
        sp.setPosition(poz_x, poz_y);
        return sp;
    }

    void Command(const std::string& c){
        if(c=="jump" && (podea==1 || (jump < 3 && !podea && !doublejump)))
        {
            if(podea == 0) doublejump = 1;
            jump += 25;
        }
        if(c=="a") poz_x -= 2;
        if(c=="d") poz_x += 2;
    }

};

class Npc {
    double orpoz_x, poz_x, poz_y;
    double dir = 1;
    int podea = 0;
    std::string order = "idle";
    std::vector<Entity> drops;
    sf::Texture npc_txtr;
public:
    explicit Npc(const sf::Texture& txtr_, double poz_x_ = 0.0, double poz_y_ = 0.0)
            : orpoz_x{poz_x_ }, poz_x{ poz_x_ }, poz_y{ poz_y_ }, npc_txtr { txtr_ } {}
    friend std::ostream& operator<<(std::ostream& os, const Npc& npc) {
        os << " Poz npc: " << npc.poz_x << ' ' << npc.poz_y << '\n';
        return os;
    }
    sf::Sprite getSprite(const std::string& caz="render")
    {
        sf::Sprite sp;
        sp.setTexture(npc_txtr);
        double spoz_x = poz_x, mvvar = 1.5;
        if(caz=="render") {
            if (podea == 0) poz_y += 1;
            if (order == "patrol") {
                if (std::abs(orpoz_x - spoz_x) > 80) dir *= -1;
                spoz_x += dir * mvvar;
                poz_x = spoz_x;
            }
        }
        sp.setPosition(spoz_x, poz_y);
        return sp;
    }
    void setOrder(const std::string& ord){
        order = ord;
    }
    void setPodea(int podea_){
        podea = podea_;
    }
    double getlowHeight(){
        return poz_y - double(npc_txtr.getSize().y);
    }
};

class Harta{
    std::vector< std::pair< Entity, std::pair<double,double> > > Obj;
public:
    friend std::ostream& operator<<(std::ostream& os, const Harta& h) {
        os << " Nr obiecte: " << h.Obj.size() << '\n';
        return os;
    }

    void addObj(const Entity& obj, std::pair<double,double> p){
        Obj.emplace_back(obj,p);
    }
    void drawMap(sf::RenderWindow& window_, Player& plr, std::vector<Npc*> npcvect){
        sf::Sprite ps = plr.getSprite("collision");
        ps.setPosition(ps.getPosition().x, ps.getPosition().y+2);
        sf::FloatRect pb_low = ps.getGlobalBounds();
        plr.setPodea(0);
        for(auto& it:Obj){
            sf::Sprite sp;
            sp.setTexture(it.first.getTexture());
            sp.setPosition(it.second.first, it.second.second); //second.first = x second.second = y
            if (it.second.second >= plr.getlowHeight()) {
                if (sp.getGlobalBounds().intersects(pb_low)) {
                    plr.setPodea(1);
                }
            }
           for(auto& npc:npcvect) {
                if (it.second.second >= npc->getlowHeight()) {
                    sf::Sprite npcs = npc->getSprite("collision");
                    npcs.setPosition(npcs.getPosition().x, npcs.getPosition().y+1);
                    sf::FloatRect npcb_low = npcs.getGlobalBounds();
                    if (sp.getGlobalBounds().intersects(npcb_low)) {
                        npc->setPodea(1);
                    }
                }
            }
            window_.draw(sp);
        }

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

    Harta h;
    Entity floor1{floor1_txtr, "floor1", 2};
    for(double i = 0; i <= 700; i+=40){
        h.addObj(floor1, {i, 300.0});
    }

    Entity e1{"e1", 1};
    p.AddItem(e1);
    std::vector<Entity> v;
    p.Inv(v);

    std::vector<Npc*> npc_vector;

    Npc n1{n1_txtr, 200, 60};
    npc_vector.push_back(&n1);
    n1.setOrder("patrol");
    while (window.isOpen()) {

        while (window.pollEvent(event)) {

            if (event.type == sf::Event::Closed || ( event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) ) {
                window.close();
            }
            if(p.getJumpCD() == 1 && event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Space) {
                p.setJumpCD(0);
            }
            if (p.getJumpCD() == 0 && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                p.Command("jump");
                p.setJumpCD(1);
            }
            //pt ca vreau doar pe apasare folosesc event pt space nu sf::Keyboard::isKeyPressed
        }
        if(window.hasFocus()) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) p.Command("d");
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) p.Command("a");
        }
        h.drawMap(window, p, npc_vector);
        window.draw(n1.getSprite());
        window.draw(p.getSprite());
        window.display();
        window.clear();
    }

    return 0;
}
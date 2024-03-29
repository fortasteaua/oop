#include <SFML/Graphics.hpp>
#include "Gamemap.h"
#include "Characters.h"

void Harta::Item_pickup(sf::RenderWindow &window_, Player &plr, std::vector<Entity *> &itemvect, const sf::Sprite &ps) {
    for(auto item=itemvect.begin(); item != itemvect.end(); item++){
        if((*item)->status() == 0) {
            sf::Sprite spitem = (*item)->getSprite();
            if (ps.getGlobalBounds().intersects(spitem.getGlobalBounds())) {
                sf::Sprite spbuton;
                spbuton.setTexture(Textures["e"]);
                spbuton.setPosition(spitem.getPosition().x, spitem.getPosition().y - Textures["e"].getSize().y - 5);
                window_.draw(spbuton);
                if (plr.interaction()) {
                    plr.AddEff(**item);
                    plr.Command("e_release");
                    (*item)->sterge();
                }
            }
        }
    }
}

void Harta::Player_col(Player &plr, const sf::Sprite &ps, const sf::FloatRect &pb_low, const sf::FloatRect &pb_high,
                       const sf::FloatRect &pb_left, const sf::FloatRect &pb_right,
                       const std::pair<Entity, std::pair<double, double>> &it,
                       const sf::Sprite &sp, int &podea_, int &sus_, int &dreapta_, int &stanga_) {
    if (podea_ == 0 && sp.getGlobalBounds().intersects(pb_low)){
        if(it.first.getTip() == 3) lvlwon = 1;
        podea_ = plr.getGravity() - ( sp.getPosition().y - plr.getY() - ps.getTexture()->getSize().y);
    }
    if (sp.getGlobalBounds().intersects(pb_high)) {
        if(sus_ == 0) sus_ =  plr.getJP()+plr.getjpmod() - (plr.getY()- (sp.getPosition().y + sp.getTexture()->getSize().y));
    }
    if (sp.getGlobalBounds().intersects(pb_right))
    {
        if(it.first.getTip() == 3) lvlwon = 1;
        if(dreapta_ == 0)dreapta_ = plr.getMS()+plr.getmsmod() - (sp.getPosition().x-plr.getX()-pb_right.getSize().x);
    }
    if (sp.getGlobalBounds().intersects(pb_left))
    {
        if(it.first.getTip() == 3) lvlwon = 1;
        if(stanga_ == 0 ) stanga_ = plr.getMS()+plr.getmsmod() - (plr.getX()-sp.getPosition().x-sp.getTexture()->getSize().x);
    }
}

void
Harta::Item_col(const std::vector<Entity *> &itemvect, std::vector<int> &itemcollisions, const sf::Sprite &sp) const {
    for(unsigned int itemindex = 0; itemindex < itemvect.size(); itemindex++){
        sf::Sprite itemsp = itemvect[itemindex]->getSprite("collision");
        itemsp.setPosition(itemsp.getPosition().x, itemsp.getPosition().y + 2);
        if(itemsp.getGlobalBounds().intersects(sp.getGlobalBounds())) itemcollisions[itemindex] = 1;
    }

    for(unsigned int itemindex = 0; itemindex < itemvect.size(); itemindex++){
        itemvect[itemindex]->setPodea(itemcollisions[itemindex]);
    }
}

void Harta::Npc_col(Player &plr, const sf::Sprite &pscurent, const sf::FloatRect &pb_low,
                    std::vector<std::vector<int>> &npccollisions, const sf::Sprite &sp) const {
    for(unsigned int npcindex = 0; npcindex < npcvect.size(); npcindex++) {

        // collision box-uri
        sf::Sprite npcs = npcvect[npcindex]->getSprite("collision");
        npcs.setPosition(npcs.getPosition().x, npcs.getPosition().y+plr.getGravity());
        sf::FloatRect npcb_low = npcs.getGlobalBounds();
        npcs.setPosition(npcs.getPosition().x, npcs.getPosition().y-plr.getGravity()-npcvect[npcindex]->getJP());
        sf::FloatRect npcb_high = npcs.getGlobalBounds();
        npcs.setPosition(npcs.getPosition().x-+npcvect[npcindex]->getMS(),npcs.getPosition().y+2);
        sf::FloatRect npcb_left = npcs.getGlobalBounds();
        npcs.setPosition(npcs.getPosition().x+2*npcvect[npcindex]->getMS(),npcs.getPosition().y);
        sf::FloatRect npcb_right = npcs.getGlobalBounds();
        // collision box-uri


        if (sp.getGlobalBounds().intersects(npcb_low)) {
            npccollisions[npcindex][0] = plr.getGravity() - ( sp.getPosition().y - npcvect[npcindex]->getY() - npcs.getTexture()->getSize().y);
        }
        if (sp.getGlobalBounds().intersects(npcb_high)) npccollisions[npcindex][1] = 1;
        if (sp.getGlobalBounds().intersects(npcb_left)) npccollisions[npcindex][2] = 1;
        if (sp.getGlobalBounds().intersects(npcb_right)) npccollisions[npcindex][3] = 1;
        if(npcb_high.intersects(pb_low) && npcvect[npcindex]->getStatus() == 0 && plr.getStatus() == 0){
            if(pscurent.getPosition().y + pscurent.getTexture()->getSize().y <= npcs.getPosition().y) { // daca player-ul il calca pe npc, il omoara, altfel player-ul moare.
                npcvect[npcindex]->Kill();
                plr.IncreaseScore(npcvect[npcindex]->getScoreValue()); // adaugam punctele de pe npc la scor
            }
            else{
                plr.Kill();
            }
        }
    }
    for(unsigned int npcindex = 0; npcindex < npcvect.size(); npcindex++){
        npcvect[npcindex]->setCollisions(npccollisions[npcindex][0], npccollisions[npcindex][1], npccollisions[npcindex][2], npccollisions[npcindex][3]);
    }
}

void Harta::Drawnpcs(sf::RenderWindow &window_) {
    for(auto &it:npcvect){
        Npc* npcpointer= it;
        if(typeid(*npcpointer) != typeid(Mov))window_.draw(npcpointer->getSprite("render"));
        else{
            auto npcpointer2 = dynamic_cast<Mov*>(npcpointer);
            window_.draw(npcpointer2->getSprite("render"));
        }
    }
}

std::ostream &operator<<(std::ostream &os, const Harta &h) {
    os << " Nr obiecte: " << h.Obj.size() << '\n';
    return os;
}

Harta &Harta::get_map() {
    static Harta harta;
    return harta;
}

void Harta::addNpc(Npc &npc) {
    npcvect.push_back(&npc);
}

void Harta::addObj(const Entity &obj, std::pair<double, double> p) {
    Obj.emplace_back(obj,p);
}

void Harta::addTexture(const std::string &str, const sf::Texture &txtr) {
    Textures[str]=txtr;
}

void Harta::drawMap(sf::RenderWindow &window_, Player &plr, std::vector<Entity *> itemvect) {

    sf::Sprite ps = plr.getSprite("collision"), pscurent = ps;

    Item_pickup(window_, plr, itemvect, ps);

    //player collision box-uri
    ps.setPosition(ps.getPosition().x, ps.getPosition().y+plr.getGravity());
    sf::FloatRect pb_low = ps.getGlobalBounds();
    ps.setPosition(ps.getPosition().x, ps.getPosition().y-plr.getGravity()-plr.getJP()-plr.getjpmod());
    sf::FloatRect pb_high = ps.getGlobalBounds();
    ps.setPosition(ps.getPosition().x-plr.getMS()-plr.getmsmod(), ps.getPosition().y+plr.getJP()+plr.getjpmod());
    sf::FloatRect pb_left = ps.getGlobalBounds();
    ps.setPosition(ps.getPosition().x+2*plr.getMS()+2*plr.getmsmod(), ps.getPosition().y);
    sf::FloatRect pb_right = ps.getGlobalBounds();
    //player collision box-uri

    int podea_ = 0, sus_ = 0, dreapta_ = 0, stanga_ = 0;
    std::vector < std::vector<int> > npccollisions(npcvect.size(), {0, 0, 0, 0});
    std::vector < int > itemcollisions(itemvect.size(), 0);
    for(auto& it:Obj){
        sf::Sprite sp;
        sp.setTexture(it.first.getTexture());
        sp.setPosition(it.second.first, it.second.second); //second.first = x second.second = y

        Player_col(plr, ps, pb_low, pb_high, pb_left, pb_right, it, sp, podea_, sus_, dreapta_, stanga_);//player collision

        Item_col(itemvect, itemcollisions, sp); //item collisions

        Npc_col(plr, pscurent, pb_low, npccollisions, sp); //npc collisions

        window_.draw(sp);
    }
    plr.setCollisions(podea_, sus_, stanga_, dreapta_);
    Drawnpcs(window_);
    if(lvlwon == 1 && plr.getStatus() == 0){
        sf::Sprite spfinmesaj;
        spfinmesaj.setTexture(Textures["finmesaj"]);
        spfinmesaj.setPosition(window_.getView().getCenter().x - Textures["finmesaj"].getSize().x, window_.getView().getCenter().y - Textures["finmesaj"].getSize().y);
        window_.draw(spfinmesaj);
    }
    if(plr.getStatus() == 1){
        sf::Sprite mortmesaj;
        mortmesaj.setTexture(Textures["mortmesaj"]);
        mortmesaj.setPosition(window_.getView().getCenter().x - Textures["mortmesaj"].getSize().x, window_.getView().getCenter().y - Textures["mortmesaj"].getSize().y);
        window_.draw(mortmesaj);
    }
}


void LevelBuilder::level1(Harta& harta){
    for(double i = 0; i <= 2000; i+=40){
        if(i < 1250 || i > 1360)harta.addObj(*Entities["floor1"], {i, 300.0});
        if(i == 0 || i + 40 == 720) harta.addObj(*Entities["perete"], {i, 210});
    }
    for(double i = 400; i<= 560; i+=40){
        harta.addObj( *Entities["floor1"], {i, 180});
    }
    for(double i = 800; i<= 920; i+=40){
        harta.addObj( *Entities["floor1"], {i, 120});
    }
    for(double i = 1400; i<= 1520; i+=40){
        harta.addObj( *Entities["floor1"], {i, 120});
    }
    harta.addObj(*Entities["perete"], {1400, 210});
    for(double i = 1240; i>950 ; i-=120) harta.addObj(*Entities["peretemic"], {i, 255});
    for(double i = 1600; i<=1760 ; i+=40){
        harta.addObj(*Entities["floor1"], {i, 210});
    }
    harta.addObj(*Entities["finmark"], {2000, 120});
}

void LevelBuilder::addEntity(const std::string &nume_ent, Entity *ent) {
    Entities[nume_ent] = ent;
}

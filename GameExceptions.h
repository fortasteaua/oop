#include <stdexcept>

class eroare_joc : public std::runtime_error{
    using std::runtime_error::runtime_error;
};

class eroare_entitate : public eroare_joc{
public:
    explicit eroare_entitate(const std::string& m ) : eroare_joc(m){};
};

void pointerentitate(std::shared_ptr<Effect> ent){
    if(ent == nullptr) {
        throw eroare_entitate("pointerul de entitate este nullptr");
    }
}

class eroare_textura : public eroare_joc{
public:
    explicit eroare_textura(const std::string& m) : eroare_joc(m){};
};

void loadtxtr(bool ret){
    if(!ret){
        throw eroare_textura("nu s-a incarcat textura");
    }
}

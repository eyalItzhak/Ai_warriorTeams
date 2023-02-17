#include <iostream>
#include "Character.h"

Character::Character()
{

}
Character::~Character()
{

}
Character::Character(int h, int a, State* s) {
        hp = h;
        ammo = a;
        state = s;
    }

    int Character::getHp() {
        return hp;
    }

    int Character::getAmmo() {
        return ammo;
    }

    void Character::setHp(int h) {
        hp = h;
    }

    void Character::setAmmo(int a) {
        ammo = a;
    }
    void Character::setLocation(Cell* loc)
    {
        location = loc;
    }

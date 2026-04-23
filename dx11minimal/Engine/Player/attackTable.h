#ifndef _ATTACK_TABLE_H_
#define _ATTACK_TABLE_H_

//////////////
// INCLUDES //
//////////////

#include "../ECS_Base/entity.h"
#include "../BasicComponents/Transform.h"

#include "weapons.h"
#include "../Compute/Combat/elements.h"


/////////////
// STRUCTS //
/////////////

struct AttackDesc {
    Entity* entity;
    int weapon;
    int element;

    AttackDesc(Entity* onEntity, PlayerWeapons Weapon, Elements Element)
        : entity(onEntity), weapon((int)Weapon), element((int)Element)
    {
    }
};

struct AttackLogic {
    AttackDesc(*start)(EntityStorage*, const Transform&, const point3d&);
    void (*update)(EntityStorage*, Entity*);
    void (*end)(EntityStorage*, Entity*, const CollisionInfo&);
};

///////////
// TABLE //
///////////

extern AttackLogic PlayerAttackTable[3][5];


#endif
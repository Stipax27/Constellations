#ifndef _ATTACK_TABLE_H_
#define _ATTACK_TABLE_H_


#include "../ECS_Base/entity.h"
#include "../BasicComponents/Transform.h"

#include "weapons.h"
#include "../Compute/Combat/elements.h"


struct AttackLogic {
    Entity* (*start)(EntityStorage*, const Transform&, const point3d&);
    void (*update)(Entity*);
    void (*end)(Entity*);
};


extern AttackLogic PlayerAttackTable[3][4];


#endif
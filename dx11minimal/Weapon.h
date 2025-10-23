DWORD attackTime;
enum class weapon_name {
    Fists, Sword, Shield, Bow, Staff
};

struct weapon_ {
public:
    float damage;
    float defense;
    float attackSpeed;
    std::string name;
    Constellation* constellation;

};

weapon_ weapon[5];
//bool backMorphLock = false;



weapon_name current_weapon = weapon_name::Fists;

void initWeapon() {
    weapon[(int)weapon_name::Fists].damage = 0;
    weapon[(int)weapon_name::Fists].defense = 0;
    weapon[(int)weapon_name::Fists].attackSpeed = 0;
    weapon[(int)weapon_name::Fists].name = "Fist";
    weapon[(int)weapon_name::Fists].constellation = &Sword;

    weapon[(int)weapon_name::Sword].damage = 0.4f;
    weapon[(int)weapon_name::Sword].defense = 0.4f;
    weapon[(int)weapon_name::Sword].attackSpeed = 750;
    weapon[(int)weapon_name::Sword].name = "sword";
    weapon[(int)weapon_name::Sword].constellation = &Sword;

    weapon[(int)weapon_name::Shield].damage = 0.2f;
    weapon[(int)weapon_name::Shield].defense = 0.5f;
    weapon[(int)weapon_name::Shield].attackSpeed = 750;
    weapon[(int)weapon_name::Shield].name = "shield";
    weapon[(int)weapon_name::Shield].constellation = &Shield;

    weapon[(int)weapon_name::Bow].damage = 0.5f;
    weapon[(int)weapon_name::Bow].defense = 0.2f;
    weapon[(int)weapon_name::Bow].attackSpeed = 750;
    weapon[(int)weapon_name::Bow].name = "bow";
    weapon[(int)weapon_name::Bow].constellation = &Bow;

    weapon[(int)weapon_name::Staff].damage = 0.4f;
    weapon[(int)weapon_name::Staff].defense = 0.3f;
    weapon[(int)weapon_name::Staff].attackSpeed = 0.4f;
    weapon[(int)weapon_name::Staff].name = "staff";
    weapon[(int)weapon_name::Staff].constellation = &Staff;
}

void SelectWeapon(Constellation* playerConst) {
    weapon_name lastWeapon = current_weapon;
    if (GetAsyncKeyState('1')) current_weapon = weapon_name::Sword;
    if (GetAsyncKeyState('2')) current_weapon = weapon_name::Shield;
    if (GetAsyncKeyState('3')) current_weapon = weapon_name::Bow;
    //if (GetAsyncKeyState('4')) current_weapon = weapon_name::Staff;

    /*if (lastWeapon != current_weapon && (int)current_weapon > 0)
        backMorphLock = true;*/

    if ((int)current_weapon > 0)
        playerConst->Morph(starSet[11 + (int)current_weapon]);
    else
        playerConst->Morph(playerConst);
}


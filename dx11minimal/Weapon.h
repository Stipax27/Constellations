DWORD attackTime;
enum class weapon_name {
    Sword, Shield, Bow, Staff
};

struct weapon_ {
public:
    float damage;
    float defense;
    float attackSpeed;
    std::string name;
    Constellation* constellation;

};

weapon_ weapon[4];



Constellation* currentWeapon = &Sword;
weapon_name current_weapon = weapon_name::Sword;

void initWeapon() {
    weapon[(int)weapon_name::Sword].damage = 0.4;
    weapon[(int)weapon_name::Sword].defense = 0.4;
    weapon[(int)weapon_name::Sword].attackSpeed = 750;
    weapon[(int)weapon_name::Sword].name = "sword";
    weapon[(int)weapon_name::Sword].constellation = &Sword;

    weapon[(int)weapon_name::Shield].damage = 0.2;
    weapon[(int)weapon_name::Shield].defense = 0.5;
    weapon[(int)weapon_name::Shield].attackSpeed = 750;
    weapon[(int)weapon_name::Shield].name = "shield";
    weapon[(int)weapon_name::Shield].constellation = &Shield;

    weapon[(int)weapon_name::Bow].damage = 0.5;
    weapon[(int)weapon_name::Bow].defense = 0.2;
    weapon[(int)weapon_name::Bow].attackSpeed = 750;
    weapon[(int)weapon_name::Bow].name = "bow";
    weapon[(int)weapon_name::Bow].constellation = &Bow;

    weapon[(int)weapon_name::Staff].damage = 0.4;
    weapon[(int)weapon_name::Staff].defense = 0.3;
    weapon[(int)weapon_name::Staff].attackSpeed = 0.4;
    weapon[(int)weapon_name::Staff].name = "staff";
    weapon[(int)weapon_name::Staff].constellation = &Staff;
}

void SelectWeapon() {

    if (GetAsyncKeyState('1')) current_weapon = weapon_name::Sword;
    if (GetAsyncKeyState('2')) current_weapon = weapon_name::Shield;
    if (GetAsyncKeyState('3')) current_weapon = weapon_name::Bow;
    if (GetAsyncKeyState('4')) current_weapon = weapon_name::Staff;

}


struct weapon_ {
    float damage;
    std::string name;
};

weapon_ weapon[4];

enum class weapon_name {
    sword, shield, bow, staff
};


weapon_name current_weapon = weapon_name::sword;

void initWeapon()
{
    weapon[(int)weapon_name::sword].damage = .4;
    weapon[(int)weapon_name::sword].name = "sword";

    weapon[(int)weapon_name::staff].damage = .4;
    weapon[(int)weapon_name::staff].name = "staff";

    weapon[(int)weapon_name::shield].damage = .2;
    weapon[(int)weapon_name::shield].name = "shield";

    weapon[(int)weapon_name::bow].damage = .5;
    weapon[(int)weapon_name::bow].name = "bow";

}

void SelectWeapon()
{
        
    if (GetAsyncKeyState('1'))
    {
        current_weapon = weapon_name::sword;
    }

    if (GetAsyncKeyState('2'))
    {
        current_weapon = weapon_name::shield;
    }

    if (GetAsyncKeyState('3'))
    {
        current_weapon = weapon_name::bow;
    }

    if (GetAsyncKeyState('4'))
    {
        current_weapon = weapon_name::staff;
    }



}

bool isConstellationDead(Constellation& Constellation)
{
    std::vector <float>& starHealth = Constellation.starsHealth;

    int starsCount = starHealth.size();
    float health = 0;
    for (int i = 0; i < starsCount; i++)
    {
        health += starHealth[i];
    }

    return health > 0 ? false : true;


}
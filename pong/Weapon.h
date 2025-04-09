struct weapon_ {
    float damage;
    float defense;
    std::string name;
    Constellation* constellation;

    void attack() {

    }
};

weapon_ weapon[4];

enum class weapon_name {
    Sword, Shield, Bow, Staff
};

Constellation* currentWeapon = &Sword;
weapon_name current_weapon = weapon_name::Sword;

void initWeapon() {
    weapon[(int)weapon_name::Sword].damage = 0.4;
    weapon[(int)weapon_name::Sword].defense = 0.4;
    weapon[(int)weapon_name::Sword].name = "sword";
    weapon[(int)weapon_name::Sword].constellation = &Sword;

    weapon[(int)weapon_name::Shield].damage = 0.2;
    weapon[(int)weapon_name::Shield].defense = 0.5;
    weapon[(int)weapon_name::Shield].name = "shield";
    weapon[(int)weapon_name::Shield].constellation = &Shield;

    weapon[(int)weapon_name::Bow].damage = 0.5;
    weapon[(int)weapon_name::Bow].defense = 0.2;
    weapon[(int)weapon_name::Bow].name = "bow";
    weapon[(int)weapon_name::Bow].constellation = &Bow;

    weapon[(int)weapon_name::Staff].damage = 0.4;
    weapon[(int)weapon_name::Staff].defense = 0.3;
    weapon[(int)weapon_name::Staff].name = "staff";
    weapon[(int)weapon_name::Staff].constellation = &Staff;
}

void SelectWeapon() {

    if (GetAsyncKeyState('1')) current_weapon = weapon_name::Sword;
    if (GetAsyncKeyState('2')) current_weapon = weapon_name::Shield;
    if (GetAsyncKeyState('3')) current_weapon = weapon_name::Bow;
    if (GetAsyncKeyState('4')) current_weapon = weapon_name::Staff;

}

float getConstellationHP(Constellation& Constellation)
{
    std::vector <float>& starHealth = Constellation.starsHealth;

    int starsCount = starHealth.size();
    float health = 0;
    for (int i = 0; i < starsCount; i++)
    {
        health += starHealth[i];
    }

    return health;

    

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

void enemyAttack(Constellation& Constellation)
{


    std::vector <float>& starHealth = Constellation.starsHealth;

    int starsCount = starHealth.size();
    int attackedStar = rand() % starsCount;

    starHealth[attackedStar] -= .4;

    

}

point3d attack[2];

bool is_attack;



void AttackSwordVector()
{

    if (GetAsyncKeyState(VK_LBUTTON))
    {
        /*float dx = mouse.x - window.width / 2.;
        float dy = mouse.y - window.height / 2.;
        float lenght = sqrt(dx * dx + dy * dy);*/
        if (!lmb)
        {
            lmb = true;
            oldmouse.x = mouse.x;
            oldmouse.y = mouse.y;
            oldmouseAngle = mouseAngle;
        }
        float dx, dy;
        dx = mouse.x - oldmouse.x;
        dy = mouse.y - oldmouse.y;



        HPEN pen = CreatePen(PS_SOLID, 3, RGB(0, 191, 255));
        SelectObject(window.context, pen);
        MoveToEx(window.context, oldmouse.x, oldmouse.y, NULL);
        LineTo(window.context, mouse.x, mouse.y);
        attack[0].x = oldmouse.x;
        attack[0].y = oldmouse.y;
        attack[1].x = mouse.x;
        attack[1].y = mouse.y;


        /*if (is_attack) {
            SaveCurrentState();
        }
        */
    }
    else
    {
        lmb = false;
        is_attack = false;
    }


}
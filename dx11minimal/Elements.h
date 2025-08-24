using namespace std;

struct Element_ {
public:
    float damageMod;
    float defenseMod;
    std::string name;
    COLORREF color;

    float swordMod = 1.0f;
    float shieldMod = 1.0f;
    float bowMod = 1.0f;
    float staffMod = 1.0f;
};

Element_ elements[4];

enum class element_name
{
    Fire, Earth, Air, Water
};

element_name current_element = element_name::Fire;

void initElements() {

    elements[(int)element_name::Fire].damageMod = 1.3f;
    elements[(int)element_name::Fire].defenseMod = 0.8f;
    elements[(int)element_name::Fire].name = "Fire";
    elements[(int)element_name::Fire].color = RGB(255, 100, 0);
    elements[(int)element_name::Fire].swordMod = 1.5f;  // Меч + Огонь = +50% урона
    elements[(int)element_name::Fire].bowMod = 1.3f;    // Лук + Огонь = +30% урона

    elements[(int)element_name::Earth].damageMod = 0.9f;
    elements[(int)element_name::Earth].defenseMod = 1.4f;
    elements[(int)element_name::Earth].name = "Earth";
    elements[(int)element_name::Earth].color = RGB(139, 69, 19);
    elements[(int)element_name::Earth].shieldMod = 1.6f; // Щит + Земля = +60% защиты

    elements[(int)element_name::Air].damageMod = 1.1f;
    elements[(int)element_name::Air].defenseMod = 0.9f;
    elements[(int)element_name::Air].name = "Air";
    elements[(int)element_name::Air].color = RGB(173, 216, 230);
    elements[(int)element_name::Air].bowMod = 1.4f;     // Лук + Воздух = +40% урона

    elements[(int)element_name::Water].damageMod = 1.0f;
    elements[(int)element_name::Water].defenseMod = 1.2f;
    elements[(int)element_name::Water].name = "Water";
    elements[(int)element_name::Water].color = RGB(0, 105, 148);
    elements[(int)element_name::Water].staffMod = 1.3f;  // Посох + Вода = +30% урона
}

float CalculateCombinedDamage() {
    float baseDamage = weapon[(int)current_weapon].damage;
    float elementDamage = elements[(int)current_element].damageMod;

    float weaponElementMod = 1.0f;

    switch (current_weapon) {
    case weapon_name::Sword:
        weaponElementMod = elements[(int)current_element].swordMod;
        break;
    case weapon_name::Shield:
        weaponElementMod = elements[(int)current_element].shieldMod;
        break;
    case weapon_name::Bow:
        weaponElementMod = elements[(int)current_element].bowMod;
        break;
    case weapon_name::Staff:
        weaponElementMod = elements[(int)current_element].staffMod;
        break;
    }

    return baseDamage * elementDamage * weaponElementMod;
}

void SelectElement() {
    static bool keysPressed[4] = { false };

    if (GetAsyncKeyState('Z') & 0x8000) {
        if (!keysPressed[0]) {
            current_element = element_name::Fire;
            keysPressed[0] = true;
        }
    }
    else {
        keysPressed[0] = false;
    }

    if (GetAsyncKeyState('X') & 0x8000) {
        if (!keysPressed[1]) {
            current_element = element_name::Earth;
            keysPressed[1] = true;
        }
    }
    else {
        keysPressed[1] = false;
    }

    if (GetAsyncKeyState('C') & 0x8000) {
        if (!keysPressed[2]) {
            current_element = element_name::Air;
            keysPressed[2] = true;
        }
    }
    else {
        keysPressed[2] = false;
    }

    if (GetAsyncKeyState('V') & 0x8000) {
        if (!keysPressed[3]) {
            current_element = element_name::Water;
            keysPressed[3] = true;
        }
    }
    else {
        keysPressed[3] = false;
    }
}


void drawCurrentElement() {
    std::string elementText = "Current element: " + elements[(int)current_element].name;


    COLORREF oldColor = SetTextColor(window.context, elements[(int)current_element].color);
    int oldBkMode = SetBkMode(window.context, TRANSPARENT);


    drawString(elementText.c_str(), window.width / 2, window.height - window.height / 5., 1, true);


    SetTextColor(window.context, oldColor);
    SetBkMode(window.context, oldBkMode);
}


void DrawCombatStats() {
    float combinedDamage = CalculateCombinedDamage();

    std::string damageText = "Damage: " + std::to_string(combinedDamage);
    drawString(damageText.c_str(), window.width / 4, window.height - 400, 1, true);

    COLORREF oldColor = SetTextColor(window.context, elements[(int)current_element].color);
    std::string elementText = elements[(int)current_element].name + " " + weapon[(int)current_weapon].name;
    drawString(elementText.c_str(), window.width / 4, window.height - 180, 1.2f, true);
    SetTextColor(window.context, oldColor);
}

float getConstellationHP(Constellation& Constellation)
{
    std::vector <float>& starHealth = Constellation.starsHealth;

    int starsCount = starHealth.size();
    float health = 0.f;

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

void enemyAttack(Constellation& Constellation) {
    std::vector<float>& starHealth = Constellation.starsHealth;
    float combinedDamage = CalculateCombinedDamage() * 0.05f;

    for (int i = 0; i < starHealth.size(); i++) {
        starHealth[i] -= combinedDamage;
        if (starHealth[i] < 0) starHealth[i] = 0;
    }

    //isDamageHero = true;
}

void enemyFight()
{
    float e = 10000;
    if (currentTime > attackTime + e)
    {
        attackTime = currentTime;
        enemyAttack(*starSet[player_sign]);

    }

    if (getConstellationHP(*starSet[player_sign]) <= 0.f)
    {
        gameState = gameState_::EndFight;
    }
}

point3d attack[5];
point3d mouseposWhenAttack;
point3d mousePos;
bool is_attack;

void AttackVector()
{

    if (GetAsyncKeyState(VK_LBUTTON))
    {

        if (!lmb)
        {

            lmb = true;
            mouse.oldPos.x = mouse.pos.x;
            mouse.oldPos.y = mouse.pos.y;

        }

        MoveToEx(window.context, mouse.oldPos.x, mouse.oldPos.y, NULL);
        LineTo(window.context, mouse.pos.x, mouse.pos.y);


    }
    else
    {
        lmb = false;
        is_attack = false;
    }

    attack[0].x = mouse.oldPos.x;
    attack[0].y = mouse.oldPos.y;
    attack[1].x = mouse.pos.x;
    attack[1].y = mouse.pos.y;

    attack[2].x = mouse.oldPos.x;
    attack[2].y = mouse.oldPos.y;
    attack[3].x = mouse.pos.x;
    attack[3].y = mouse.pos.y;

    attack[4].x = mouse.oldPos.x;
    attack[4].y = mouse.oldPos.y;
    attack[5].x = mouse.pos.x;
    attack[5].y = mouse.pos.y;

}







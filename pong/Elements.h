using namespace std;

struct Element_ {
public:
    float damageMod;
    float defenseMod;
    std::string name;
    COLORREF color;
    int swordMod = 1;
    int shieldMod = 1;
    int bowMod = 1;
    int staffMod = 1;
};

Element_ elements[4];

enum class element_name 
{
    Fire, Earth, Air, Water
};

element_name current_element = element_name::Fire;

void initElements() {
    elements[(int)element_name::Fire].damageMod = 2;
    elements[(int)element_name::Fire].defenseMod = 1;
    elements[(int)element_name::Fire].name = "Fire";
    elements[(int)element_name::Fire].color = RGB(255, 100, 0); 
    elements[(int)element_name::Fire].swordMod = 2;
    elements[(int)element_name::Fire].bowMod = 1;

    elements[(int)element_name::Earth].damageMod = 1;
    elements[(int)element_name::Earth].defenseMod = 1;
    elements[(int)element_name::Earth].name = "Earth";
    elements[(int)element_name::Earth].color = RGB(139, 69, 19);
    elements[(int)element_name::Earth].shieldMod = 2;

    elements[(int)element_name::Air].damageMod = 1;
    elements[(int)element_name::Air].defenseMod = 1;
    elements[(int)element_name::Air].name = "Air";
    elements[(int)element_name::Air].color = RGB(173, 216, 230);
    elements[(int)element_name::Air].bowMod = 1;

    elements[(int)element_name::Water].damageMod = 1;
    elements[(int)element_name::Water].defenseMod = 1;
    elements[(int)element_name::Water].name = "Water";
    elements[(int)element_name::Water].color = RGB(0, 105, 148);
    elements[(int)element_name::Water].staffMod = 2;
}

float CalculateCombinedDamage() {
    
    float baseDamage = weapon[(int)current_weapon].damage;
    float elementDamageMod = elements[(int)current_element].damageMod;

    float weaponElementMod = 1;

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

    return baseDamage * elementDamageMod * weaponElementMod;
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

    
    HBRUSH currentBrush = CreateSolidBrush(elements[(int)current_element].color);

    
    HBRUSH oldBrush = (HBRUSH)SelectObject(window.context, currentBrush);

    
    RECT rect;
    rect.left = window.width / 2 - 50; 
    rect.top = window.height - window.height / 5 - 20; 
    rect.right = window.width / 2 + 50;
    rect.bottom = window.height - window.height / 5 + 20; 
    FillRect(window.context, &rect, currentBrush); 

    drawString(elementText.c_str(), window.width / 2, window.height - window.height / 4, 1, true);

    SelectObject(window.context, oldBrush);
    SetTextColor(window.context, oldColor);
    SetBkMode(window.context, oldBkMode);
}


void DrawCombatStats() {
    int combinedDamage = CalculateCombinedDamage();

    std::string damageText = "Damage: " + std::to_string(combinedDamage);
    drawString(damageText.c_str(), window.width / 4, window.height - 400, 1, true);

    // Установка цвета текста на цвет стихии
    COLORREF oldColor = SetTextColor(window.context, elements[(int)current_element].color);

    // Текст элемента и оружия
    std::string elementText = elements[(int)current_element].name + " " + weapon[(int)current_weapon].name;
    drawString(elementText.c_str(), window.width / 4, window.height - 180, 1.2f, true);

    // Восстановление старого цвета текста
    SetTextColor(window.context, oldColor);
}

float getConstellationHP()
{
    Entity* playerEntity = &entities[static_cast<int>(player_sign)];

    std::vector <int>& starHealth = playerEntity->healthSystem->starsHealth;

    int starsCount = starHealth.size();
    int health = 0;

    for (int i = 0; i < starsCount; i++)
    {
        health += starHealth[i];

    }

    return health;



}

bool isConstellationDead()
{
    Entity* playerEntity = &entities[static_cast<int>(player_sign)];

    std::vector <int>& starHealth = playerEntity->healthSystem->starsHealth;

    int starsCount = starHealth.size();
    int health = 0;
    for (int i = 0; i < starsCount; i++)
    {
        health += starHealth[i];

    }

    return health > 0 ? false : true;



}

void enemyAttack()
{
    Entity& playerEntity = entities[static_cast<size_t>(player_sign)];  // Предполагая, что entities — std::vector

    for (int& health : playerEntity.healthSystem->starsHealth)
    {
        health -= 1;
        if (health < 0) health = 0;
    }

    isDamageHero = true;
}

void enemyFight()
{
    
    float e = 5000.;

    float progressTimeAttack = 1. - (currentTime - attackTime) / e;
    progressTimeAttack *= 5;
    std::string cdTimeOutText = std::to_string((int)progressTimeAttack);
    if (progressTimeAttack > 0)
    {
        drawString("recharge Attack", window.width / 6 - 150, window.height / 4, 1, true);
        drawString(cdTimeOutText.c_str(), window.width / 6 + 50, window.height / 4, 1, true);
    }



    if (currentTime > attackTime + e)
    {
        attackTime = currentTime;
        enemyAttack();

    }

    if (getConstellationHP(entities[player_sign]) <= 0.f)
    {
        gameState = gameState_::EndFight;
    }
}

point3d attack[5];
point3d mouseposWhenAttack;
point3d mousePos;
bool is_attack;
bool isAttacking = false;
point3d attackStartPos;
float attackStartTime;

void AttackVector() 
{
    static bool lmb = false;

    if (GetAsyncKeyState(VK_LBUTTON))
    {
        if (!lmb) 
        {
            // Начало атаки
            lmb = true;
            isAttacking = true;
            attackStartTime = currentTime;
            attackStartPos = { (float)mouse.pos.x, (float)mouse.pos.y, 0 };

            // Инициализация точек атаки в зависимости от оружия
                point3d playerPos = getPlayerPosition();
                switch (current_weapon) 
                {
                case weapon_name::Sword:
                    attack[0] = playerPos;
                    attack[1] = playerPos;
                    break;
                case weapon_name::Shield:
                    attack[2] = attackStartPos;
                    attack[3] = attackStartPos;
                    break;
                case weapon_name::Bow:
                    attack[4] = playerPos;
                    break;
                }
        }

        // Отрисовка атаки
        HPEN pen = CreatePen(PS_SOLID, 3, RGB(0, 191, 255));
        SelectObject(window.context, pen);

        switch (current_weapon) {
        case weapon_name::Sword: {
            point3d playerPos = getPlayerPosition();
            MoveToEx(window.context, (int)playerPos.x, (int)playerPos.y, NULL);
            LineTo(window.context, mouse.pos.x, mouse.pos.y);
            attack[1] = { (float)mouse.pos.x, (float)mouse.pos.y, 0 };
            break;
        }
        case weapon_name::Shield: {
            float radius = getDistance(attackStartPos, { (float)mouse.pos.x, (float)mouse.pos.y, 0 });
            attack[3] = { attackStartPos.x + radius, attackStartPos.y, 0 };

            // Рисуем круг
            for (int i = 0; i < 36; i++) {
                float angle = i * (2 * 3.14159f / 36);
                int x = (int)(attackStartPos.x + radius * cos(angle));
                int y = (int)(attackStartPos.y + radius * sin(angle));
                if (i == 0) MoveToEx(window.context, x, y, NULL);
                else LineTo(window.context, x, y);
            }
            break;
        }
        case weapon_name::Bow: {
            int crossSize = 15;
            MoveToEx(window.context, mouse.pos.x - crossSize, mouse.pos.y, NULL);
            LineTo(window.context, mouse.pos.x + crossSize, mouse.pos.y);
            MoveToEx(window.context, mouse.pos.x, mouse.pos.y - crossSize, NULL);
            LineTo(window.context, mouse.pos.x, mouse.pos.y + crossSize);
            attack[4] = { (float)mouse.pos.x, (float)mouse.pos.y, 0 };
            break;
        }
        }
        DeleteObject(pen);
    }
    else if (lmb) {
        // Завершение атаки
        lmb = false;
        isAttacking = false;
        attack_start = true;
        attack_time = currentTime;
    }
}







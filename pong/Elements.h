using namespace std;

struct Element_ {
    float damageMod;
    float defenseMod;
    float attackSpeedMod;
    std::string name;
    COLORREF color;
};

Element_ elements[4];


enum class element_name {
    Fire, Earth, Air, Water
};

element_name current_element = element_name::Fire;


void initElements() {
    
    elements[(int)element_name::Fire].damageMod = 1.3f;
    elements[(int)element_name::Fire].defenseMod = 0.9f;
    elements[(int)element_name::Fire].attackSpeedMod = 1.0f;
    elements[(int)element_name::Fire].name = "Fire";
    elements[(int)element_name::Fire].color = RGB(255, 100, 0);

    
    elements[(int)element_name::Earth].damageMod = 0.9f;
    elements[(int)element_name::Earth].defenseMod = 1.4f;
    elements[(int)element_name::Earth].attackSpeedMod = 0.8f;
    elements[(int)element_name::Earth].name = "Earth";
    elements[(int)element_name::Earth].color = RGB(139, 69, 19);

    
    elements[(int)element_name::Air].damageMod = 1.1f;
    elements[(int)element_name::Air].defenseMod = 0.8f;
    elements[(int)element_name::Air].attackSpeedMod = 1.2f;
    elements[(int)element_name::Air].name = "Air";
    elements[(int)element_name::Air].color = RGB(173, 216, 230);

    
    elements[(int)element_name::Water].damageMod = 1.0f;
    elements[(int)element_name::Water].defenseMod = 1.2f;
    elements[(int)element_name::Water].attackSpeedMod = 1.1f;
    elements[(int)element_name::Water].name = "Water";
    elements[(int)element_name::Water].color = RGB(0, 105, 148);
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

    
    drawString(elementText.c_str(),
        window.width / 2,
        window.height - window.height / 5.,
        1, true);

  
    SetTextColor(window.context, oldColor);
    SetBkMode(window.context, oldBkMode);
}


void applyElementalEffects(float& damage, float& defense, float& attackSpeed) {
    damage *= elements[(int)current_element].damageMod;
    defense *= elements[(int)current_element].defenseMod;
    attackSpeed *= elements[(int)current_element].attackSpeedMod;
}



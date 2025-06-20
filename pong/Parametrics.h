HBRUSH brush;
HBRUSH brush2;

float finalStarRad = 0;
float attackStartTime = 0;
float line_hit;
float attack_time;
float attack_cooldown;

bool check_attack = true;
bool attack_collision = false;
bool attack_speed = false;
bool attack_start = false;
bool attackCooldown = true;



void genRandSphere(point3d& p) {
    float amp = 1.25;
    float angleX, angleY;
    angleX = rand() % 360;
    angleY = rand() % 360;

    p.x = 0;
    p.y = 0;
    p.z = window.width;

    p.rotateX(p, angleX);
    p.rotateY(p, angleY);
}

void genWaySphere(point3d& p) {
    float amp = 1.25;
    float angleX, angleY;
    angleX = rand() % 360;
    angleY = rand() % 360;

    p.x = window.width/2;
    p.y = window.height/2;
    p.z = 100;

    p.rotateX(p, angleX);
    p.rotateY(p, angleY);
}

void placeToWorld(point3d& p, Constellation& Constellation) {
    p.rotateX(p, mouse.Angle.y * 0.1);
    p.rotateY(p, mouse.Angle.x * 0.1);
}

void fightProject(point3d& p) {
    int fadeInTime = currentTime - startTime;
    float startCamDist = 100;
    float finCamDist = 3000;

    float lerpCoef = smoothstep(0., 1, min(fadeInTime * 4, finCamDist) / (finCamDist - startCamDist));
    if (lerpCoef < .99) {
        camDist = lerpCoef * (finCamDist - startCamDist) + startCamDist;
    }
    camDist = clamp(camDist, 100, 3000);
    float x = window.width / 2. + p.x * camDist / (p.z + camDist);
    float y = window.height / 2. + p.y * camDist / (p.z + camDist);

    p.x = x;
    p.y = y;
}

void menuProject(point3d& p) {
    float camDist = 30000;
    float x = window.width / 2. + p.x * camDist / (p.z + camDist);
    float y = window.height / 2. + p.y * camDist / (p.z + camDist);
    p.x = x;
    p.y = y;
}
float getDistanceToMouse(point3d& point)
{
    float dx = point.x - mouse.pos.x;
    float dy = point.y - mouse.pos.y;
    return sqrt(dx * dx + dy * dy);
   
}

float calcStarRad(float length)
{
    return saturate(1.2 - length * .05) * fabs(sin(currentTime * .01));
    
}

void constSelectUI(point3d& point, Constellation& Constellation, Entity* entities, int i) {
    
    float lenght = getDistanceToMouse(point);
    float rad = calcStarRad(lenght);

    if (GetAsyncKeyState(VK_LBUTTON)) {
        if (lenght < starSize) {
            SelectObject(window.context, brush2);
            gameState = gameState_::Fight;
            entities[currentEnemyID].constellation;
        }
    }
    else {
        SelectObject(window.context, brush);
    }
    finalStarRad = starSize/50 * entities[currentEnemyID].healthSystem->starsHealth[i] + rad * 15;
}

float get_lenghts(point3d& point1, point3d& point2) {
    float a = point1.x - point2.x;
    float b = point1.y - point2.y;
    return sqrt(a * a + b * b);
}
point3d getPlayerPosition();
float getDistance(const point3d& a, const point3d& b);

point3d getPlayerPosition() {
    // Возвращаем позицию игрока - например, центр экрана
    return { window.width / 2.0f, window.height / 2.0f, 0 };
}

float getDistance(const point3d& a, const point3d& b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return sqrtf(dx * dx + dy * dy);
}

point3d normalize(const point3d& v) {
    float length = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (length > 0) {
        return { v.x / length, v.y / length, v.z / length };
    }
    return { 0, 0, 0 };
}

float distanceToLine(const point3d& point, const point3d& lineStart, const point3d& lineEnd) {
    float A = point.x - lineStart.x;
    float B = point.y - lineStart.y;
    float C = lineEnd.x - lineStart.x;
    float D = lineEnd.y - lineStart.y;

    float dot = A * C + B * D;
    float len_sq = C * C + D * D;
    float param = (len_sq != 0) ? dot / len_sq : -1;

    point3d projection;
    if (param < 0) {
        projection = lineStart;
    }
    else if (param > 1) {
        projection = lineEnd;
    }
    else {
        projection = { lineStart.x + param * C, lineStart.y + param * D, 0 };
    }

    float dx = point.x - projection.x;
    float dy = point.y - projection.y;
    return sqrt(dx * dx + dy * dy);
}

float getDistance(const point3d& a, const point3d& b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return sqrt(dx * dx + dy * dy);
}


void starIntersectUI(point3d& point, Constellation& Constellation, Entity* entities, int i) {
    // Получаем позицию курсора
    POINT cursorPos;
    GetCursorPos(&cursorPos);
    ScreenToClient(window.hWnd, &cursorPos);
    point3d cursorPoint = { (float)cursorPos.x, (float)cursorPos.y, 0 };

    // Обновляем радиус звезды (ваша оригинальная логика)
    float lenght = getDistanceToMouse(point);
    float rad = calcStarRad(lenght);
    finalStarRad = starSize / 50 * entities->healthSystem->starsHealth[i] + rad * 15;

    // Проверяем атаку только если прошло достаточно времени
    if (currentTime > attack_time + weapon[(int)current_weapon].attackSpeed && attack_start) {
        float totalDamage = CalculateCombinedDamage();
        bool isHit = false;

        switch (current_weapon) {
        case weapon_name::Sword: {
            // Меч - линия длиной 100 пикселей от игрока к курсору
            point3d playerPos = getPlayerPosition();
            point3d attackVector = normalize(cursorPoint - playerPos);
            attackVector.x *= 100;
            attackVector.y *= 100;
            point3d attackEnd = { playerPos.x + attackVector.x, playerPos.y + attackVector.y, 0 };

            // Сохраняем точки атаки для отрисовки
            attack[0] = playerPos;
            attack[1] = attackEnd;

            // Проверяем попадание
            float distToLine = distanceToLine(point, playerPos, attackEnd);
            isHit = (distToLine < 20.0f); // 20 пикселей - ширина удара мечом
            line_hit = distToLine / 20.0f; // Нормализованное значение для совместимости
            break;
        }

        case weapon_name::Shield: {
            // Щит - круг радиусом 100 пикселей вокруг курсора
            float distToCursor = getDistance(point, cursorPoint);
            isHit = (distToCursor <= 100.0f);

            // Сохраняем параметры щита для отрисовки
            attack[2] = { cursorPoint.x - 100, cursorPoint.y, 0 };
            attack[3] = { cursorPoint.x + 100, cursorPoint.y, 0 };
            break;
        }

        case weapon_name::Bow: {
            // Лук - точечная атака с небольшим радиусом
            float distToCursor = getDistance(point, cursorPoint);
            isHit = (distToCursor <= 5.0f);

            // Сохраняем точку атаки для отрисовки
            attack[4] = cursorPoint;
            break;
        }
        }

        if (isHit) {
            entities->healthSystem->damageStar(i, totalDamage);
            attack_collision = true;
            SelectObject(window.context, brush2);
        }
        else {
            attack_collision = false;
            SelectObject(window.context, brush);
        }
    }

    // Обработка ввода для начала атаки
    if (GetAsyncKeyState(VK_LBUTTON) && !check_attack) {
        attack_start = true;
        attack_time = currentTime;
    }
}

void heroUI(point3d& point, Constellation& Constellation,Entity* entities,int i)
{
    float lenght = getDistanceToMouse(point);
    float rad = calcStarRad(lenght);

    if (GetAsyncKeyState(VK_LBUTTON)) 
    {
        if (lenght < starSize) {
            SelectObject(window.context, brush2);
            gameState = gameState_::Helper;
            
            
        }
    }
    else {
        SelectObject(window.context, brush);
    }
    //finalStarRad = 3 * constellation.healthSystem->getHP() + rad * 15;
    finalStarRad = starSize / 50 * entities->healthSystem->starsHealth[i] + rad * 15;
}

void StarQuestUi(point3d& point)
{
    float dx = point.x - mouse.pos.x;
    float dy = point.y - mouse.pos.y;
    float length = sqrt(dx * dx + dy * dy);

    float rad = saturate(1.2 - length * .05) * fabs(sin(currentTime * .01));

    if (GetAsyncKeyState(VK_LBUTTON)) {
        if (length < starSize) {
            SelectObject(window.context, brush2);
            gameState = gameState_::Quest;
            
        }
    }
    else {
        SelectObject(window.context, brush);
    }
}

void QuestExit() 
{
    if (GetAsyncKeyState('T')) {

        SelectObject(window.context, brush2);
        gameState = gameState_::selectEnemy;

    }
    else {
        SelectObject(window.context, brush);
    }
}

void StarRiddleUi(point3d& point)
{
    float dx = point.x - mouse.pos.x;
    float dy = point.y - mouse.pos.y;
    float length = sqrt(dx * dx + dy * dy);

    float rad = saturate(1.2 - length * .05) * fabs(sin(currentTime * .01));

    if (GetAsyncKeyState(VK_LBUTTON)) {
        if (length < starSize) {
            SelectObject(window.context, brush2);
            gameState = gameState_::Riddle;

        }
    }
    else {
        SelectObject(window.context, brush);
    }
}

void StarFightUi(point3d& point)
{
    float dx = point.x - mouse.pos.x;
    float dy = point.y - mouse.pos.y;
    float length = sqrt(dx * dx + dy * dy);

    float rad = saturate(1.2 - length * .05) * fabs(sin(currentTime * .01));

    if (GetAsyncKeyState(VK_LBUTTON)) {
        if (length < starSize) {
            SelectObject(window.context, brush2);
            gameState = gameState_::StarFight;

        }
    }
    else {
        SelectObject(window.context, brush);
    }
}

void menuUI(point3d& point, Constellation& Constellation, Entity* entity, int i) {
    finalStarRad = 5;
}

void placeConstToStartMenu(point3d& p, Constellation& Constellation) {
    p *= 200;
    p.rotateY(p, currentTime * 0.1);
    p.move(0, 0, 1300);
}

void HeroUITransform(point3d& p, Constellation& Constellation) {
    p.move(0, 0, 0.);
    fightMove(p);
    p *= .13;
    p *= Constellation.scale;
}

void projectSingleConst(point3d& p) {
    float camDist = 100;
    float x = window.width / 2. + 1000 * p.x * camDist / (p.z + camDist);
    float y = window.height / 2. + 1000 * p.y * camDist / (p.z + camDist);
    p.x = x;
    p.y = y;
}

void placeConstToWorld(point3d& p, Constellation& Constellation) {
    Shaking(p);
    p.move(0, 0, 3000. / Constellation.scale);
    p.rotateX(p, Constellation.angle.x);
    p.rotateY(p, Constellation.angle.y);
    p.rotateZ(p, Constellation.angle.z);
    p *= Constellation.scale;

    p.rotateX(p, mouse.Angle.y * 0.1);
    p.rotateY(p, mouse.Angle.x * 0.1);
}

void placeHeroToWorld(point3d& p, Constellation& Constellation) {
    ShakingHero(p);
    p.move(0, 0, 0. / Constellation.scale);
    fightMove(p);
    p *= .13;
    p *= Constellation.scale;

    p.rotateX(p, mouse.Angle.y * 0.1);
    p.rotateY(p, mouse.Angle.x * 0.1);
}

void NullProject(point3d& p) {}

std::vector<point3d> stars;

void initStars(int count) {
    stars.resize(count);
    for (auto& star : stars) {
        genRandSphere(star);
        
        star *= 1000.0f; 
    }
}


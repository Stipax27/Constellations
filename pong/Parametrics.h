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


void starIntersectUI(point3d& point,Constellation& Constellation,Entity* entities, int i) {
    //Entity* enemyEntity = &entities[static_cast<int>(currentEnemyID)];

    float lenght = getDistanceToMouse(point);
    float rad = calcStarRad(lenght);

    finalStarRad = starSize / 50 * entities->healthSystem->starsHealth[i] + rad * 15;

    float line_x = get_lenghts(attack[0], attack[1]);
    float line_y = get_lenghts(attack[0], point);
    float line_z = get_lenghts(attack[1], point);
    line_hit = (line_z + line_y) / line_x;

    float centerX = (attack[3].x + attack[2].x) / 2;
    float centerY = (attack[3].y + attack[2].y) / 2;

    float dxs = attack[3].x - attack[2].x;
    float dys = attack[3].y - attack[2].y;
    float shieldRadius = sqrt(dxs * dxs + dys * dys);

    float stardx = point.x - centerX;
    float stardy = point.y - centerY;
    float distToCenter = sqrt(stardx * stardx + stardy * stardy);

    float dxb = point.x - attack[4].x;
    float dyb = point.y - attack[4].y;
    float distToStart = sqrt(dxb * dxb + dyb * dyb);
    float hitRadius = 20;

    if (currentTime > attack_time + weapon[(int)current_weapon].attackSpeed && attack_start) {
       
        float totalDamage = CalculateCombinedDamage();

        if ((current_weapon == weapon_name::Sword && line_hit < 1.01) ||
            (current_weapon == weapon_name::Shield && distToCenter <= shieldRadius) ||
            (current_weapon == weapon_name::Bow && distToStart <= hitRadius)) {

           
            entities->healthSystem->damageStar(i, totalDamage);
        }
    }

    if (current_weapon == weapon_name::Sword) {
        if (GetAsyncKeyState(VK_LBUTTON)) {
            if (line_hit < 1.001) attack_collision = true;
            SelectObject(window.context, brush);
        }
        else if (line_hit < 1.01 && !check_attack) {
            attack_start = true;
            attack_time = currentTime;
        }
    }

    if (current_weapon == weapon_name::Shield) {
        if (GetAsyncKeyState(VK_LBUTTON)) {
            if (distToCenter <= shieldRadius) attack_collision = true;
            SelectObject(window.context, brush);
        }
        else if (distToCenter <= shieldRadius && !check_attack) {
            attack_start = true;
            attack_time = currentTime;
        }
    }

    if (current_weapon == weapon_name::Bow) {
        if (GetAsyncKeyState(VK_LBUTTON)) {
            if (distToStart <= hitRadius) attack_collision = true;
            SelectObject(window.context, brush);
        }
        else if (distToStart <= hitRadius && !check_attack) {
            attack_start = true;
            attack_time = currentTime;
        }
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

    static float lastTime = currentTime;
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    if (deltaTime > 100.0f) deltaTime = 100.0f;

    updateFlyDirection();
    updateFlySpeed(deltaTime);
    updatePlayerPosition(deltaTime, p, Constellation);

   
    p.x += (constellationOffset.x)/10;
    p.y += (constellationOffset.y)/10;
    p.z += (constellationOffset.z)/10;

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

    static float lastTime = currentTime;
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    if (deltaTime > 100.0f) deltaTime = 100.0f;

    updateFlyDirection();
    updateFlySpeed(deltaTime);
    updatePlayerPosition(deltaTime, p, Constellation);

    p.x += (constellationOffset.x)/10;
    p.y += (constellationOffset.y)/10;
    p.z += (constellationOffset.z)/10;

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



float attackStartTime = 0;
float line_hit;
float attack_time;
float attack_cooldown;

bool check_attack = true;
bool attack_collision = false;
bool attack_speed = false;
bool attack_start = false;
bool attackCooldown = true;
bool T = false;

void (*modelTransform)(point3d& p, Constellation& Constellation);
void (*modelProject)(point3d& p);
void (*uiFunc)(point3d& point, Constellation& Constellation, int i);


void placeToWorld(point3d& p, Constellation& Constellation)
{
    p.rotateX(p, mouse.Angle.y * 0.1);
    p.rotateY(p, mouse.Angle.x * 0.1);
}


void placeConstToWorld(point3d& p, Constellation& Constellation)
{
    Shaking(p);

    p.move(0, 0, 3000. / Constellation.scale);
    p.rotateX(p, Constellation.angle.x);
    p.rotateY(p, Constellation.angle.y);
    p *= Constellation.scale;

    p.rotateX(p, mouse.Angle.y * 0.1);
    p.rotateY(p, mouse.Angle.x * 0.1);
}


void placeHeroToWorld(point3d& p, Constellation& Constellation)
{
    Constellation.scale = 0.13f;
    //ShakingHero(p);
    p.move(0, 0, 0. / Constellation.scale);

    // fightMove();

    p *= Constellation.scale;

    p.rotateX(p, mouse.Angle.y * 0.1);
    p.rotateY(p, mouse.Angle.x * 0.1);
}



XMMATRIX CreateFightProjectionMatrix(float camDist, float width, float height)
{
    // Простая перспективная проекция вручную (без FOV)
    float nearZ = 0.1f;
    float farZ = 10000.f;

    // 2D-подобная матрица с проекцией "на экран"
    float aspect = width / height;

    // Вручную задаём перспективную матрицу (приближённая логика fightProject)
    XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect, nearZ, farZ);
    return proj;
}
void fightProject(point3d& p)
{
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

void menuProject(point3d& p)
{
    float camDist = 30000;
    float x = window.width / 2. + p.x * camDist / (p.z + camDist);
    float y = window.height / 2. + p.y * camDist / (p.z + camDist);
    p.x = x;
    p.y = y;
}

void constSelectUI(point3d& point, Constellation& Constellation, int i)
{
    float dx = point.x - mouse.pos.x;
    float dy = point.y - mouse.pos.y;
    float lenght = sqrt(dx * dx + dy * dy);

    if (GetAsyncKeyState(VK_LBUTTON))
    {
        if (lenght < starSize)
        {
            // Only transition if this is an enemy constellation (not player's)
            if (Constellation.ID != player_sign)
            {
                gameState = gameState_::Fight;
                currentEnemy = &Constellation;
                currentEnemyID = (ZodiacSign)Constellation.ID;

                // Initialize fight-specific variables
                    isBattleActive = true;
                battleStartTime = currentTime;

                // Reset camera for fight
                Camera::state.camDist = 100;
                Camera::state.mouse = true;
            }
        }
    }
}

float get_lenghts(point3d& point1, point3d& point2)
{
    float a = point1.x - point2.x;
    float b = point1.y - point2.y;
    float c = sqrt(a * a + b * b);
    return c;

}

void starIntersectUI(point3d& point, Constellation& Constellation, int i)
{
    std::vector <float>& starHealth = Constellation.starsHealth;

    float dx = point.x - mouse.pos.x;
    float dy = point.y - mouse.pos.y;
    float lenght = sqrt(dx * dx + dy * dy);

    float rad = saturate(1.2 - lenght * .05) * fabs(sin(currentTime * .01));
    

    float line_x = get_lenghts(attack[0], attack[1]);
    float line_y = get_lenghts(attack[0], point);
    float line_z = get_lenghts(attack[1], point);
    float line_yz = line_z + line_y;
    line_hit = line_yz / line_x;

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



    if (currentTime > attack_time + weapon[(int)current_weapon].attackSpeed and attack_start == true)
    {
        if (current_weapon == weapon_name::Sword and line_hit < 1.01
            or current_weapon == weapon_name::Shield and distToCenter <= shieldRadius
            or current_weapon == weapon_name::Bow and distToStart <= hitRadius)
        {
            starHealth[i] -= weapon[(int)current_weapon].damage;

        }
    }

    if (current_weapon == weapon_name::Sword)
    {
        if (GetAsyncKeyState(VK_LBUTTON))
        {
            if (line_hit < 1.001)
            {
                attack_collision = true;
            }
        }
        else
        {
            if (line_hit < 1.01 and check_attack == false)
            {

                attack_start = true;
                attack_time = currentTime;

            }
        }
    }

    if (current_weapon == weapon_name::Shield)
    {
        if (GetAsyncKeyState(VK_LBUTTON))
        {
            if (distToCenter <= shieldRadius)
            {
                attack_collision = true;

            }
        }
        else
        {
            if (distToCenter <= shieldRadius and check_attack == false)
            {
                attack_start = true;
                attack_time = currentTime;
            }
        }
    }

    if (current_weapon == weapon_name::Bow)
    {
        if (GetAsyncKeyState(VK_LBUTTON))
        {
            if (distToStart <= hitRadius)
            {
                attack_collision = true;
            }
        }
        else
        {
            if (distToStart <= hitRadius and check_attack == false)
            {
                attack_start = true;
                attack_time = currentTime;
            }
        }
    }

}

void heroUI(point3d& point, Constellation& Constellation, int i)
{
    std::vector <float>& starHealth = Constellation.starsHealth;

    float dx = point.x - mouse.pos.x;
    float dy = point.y - mouse.pos.y;
    float lenght = sqrt(dx * dx + dy * dy);

    float rad = saturate(1.2 - lenght * .05) * fabs(sin(currentTime * .01));

    if (GetAsyncKeyState(VK_LBUTTON))
    {
        if (lenght < starSize)
        {
            //SelectObject(window.context, brush2);
        }
    }
    else
    {
        //SelectObject(window.context, brush);
    }
   
}

void menuUI(point3d& point, Constellation& Constellation, int i)
{
    
}

void placeConstToStartMenu(point3d& p, Constellation& Constellation)
{
    p *= 200;

    float a = currentTime;
    p.rotateY(p, a * 0.1);
    p.move(0, 0, 1300);
}

void HeroUITransform(point3d& p, Constellation& Constellation)
{
    p.move(0, 0, 0.);

    //fightMove();

    p *= .13;

    p *= Constellation.scale;
}

void projectSingleConst(point3d& p)
{
    float camDist = 100;

    float x = window.width / 2. + 1000 * p.x * camDist / (p.z + camDist);
    float y = window.height / 2. + 1000 * p.y * camDist / (p.z + camDist);

    p.x = x;
    p.y = y;
}

void NullProject(point3d& p) {

}
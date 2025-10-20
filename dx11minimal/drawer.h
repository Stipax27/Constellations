bool t = true;
point3d attackDirection;

struct StarProjectile {
    point3d position;
    point3d direction;
    float radius;
    point3d up;
    point3d right;
    weapon_name weapon;
    float Speed;
    bool isNew;
    DWORD creationTime;
    float lifetime;

};
float finalRadius;
static DWORD lastAttackTime = 0;
std::vector<StarProjectile> attackStars;

struct BowTracerParticle {
    point3d position;
    float size;
    DWORD creationTime;
    float lifetime;
};

std::vector<BowTracerParticle> bowTracerParticles;


const float MIN_CAMERA_DIST = 1000.f;
const float MAX_CAMERA_DIST = 10000.f;

float currentStarRadius = 20.0f;
float starRadiusReturnStart = 500.0f;
DWORD starRadiusReturnStartTime = 0;
bool isStarRadiusReturning = false;
float starRadiusReturnSpeed = 2.f;
const float MIN_STAR_RADIUS = 20.0f;
const float MAX_STAR_RADIUS = 500.0f;

#include "TransitionsStateLOGICK.h"
#include "SmoothTransformObject.h"
namespace drawer
{

    int GetRandom(int min = 1, int max = 0)
    {
        if (max > min)
        {
            return rand() % (max - min) + min;
        }
        else
        {
            return rand() % min;
        }
    }

    
    void drawLine(point3d& p1, point3d& p2 , float sz = 2.f )
    {
        point3d vector = p2 - p1;
        point3d mid = p1.lerp(p2, 0.5f);

        

        ConstBuf::ConstToVertex(4);
        ConstBuf::ConstToPixel(4);


        ConstBuf::global[0] = XMFLOAT4(p1.x, p1.y, p1.z, sz);
        ConstBuf::global[1] = XMFLOAT4(p2.x, p2.y, p2.z, sz);

        ConstBuf::Update(5, ConstBuf::global);
        ConstBuf::ConstToVertex(5);

        Draw::elipse(1);
    }

    void drawLinks(Constellation& Constellation, float sz)
    {
        std::vector <point3d>& starArray = Constellation.starsCords;
        std::vector<std::vector<float>>& starEdges = Constellation.constellationEdges;
        std::vector <float>& starHealth = Constellation.starsHealth;

        int starsEdgesCount = starEdges.size();
        for (int i = 0; i < starsEdgesCount; i++)
        {
            point3d point1 = TransformPoint(starArray[starEdges[i][0]], Constellation.Transform);
            point3d point2 = TransformPoint(starArray[starEdges[i][1]], Constellation.Transform);

            if (!Constellation.morphing && Constellation.starsCords.size() == Constellation.originStarsCords.size())
            {
                ConstBuf::global[2] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); // Белый
                ConstBuf::Update(5, ConstBuf::global);
                ConstBuf::ConstToPixel(5);

                drawLine(point1, point2, sz * 1.5f); // Увеличиваем толщину линии

                // Восстанавливаем стандартный цвет
                ConstBuf::global[2] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
                ConstBuf::Update(5, ConstBuf::global);
                //if (starHealth[starEdges[i][0]] > 0 && starHealth[starEdges[i][1]] > 0)
                //{
                //    // Устанавливаем яркий цвет для линий между неповрежденными звездами
                //    
                //}
                //else if (starHealth[starEdges[i][0]] > 0 || starHealth[starEdges[i][1]] > 0)
                //{
                //    // Полуповрежденные линии - тонкие и бледные
                //    ConstBuf::global[2] = XMFLOAT4(0.7f, 0.7f, 0.7f, 0.5f);
                //    ConstBuf::Update(5, ConstBuf::global);
                //    ConstBuf::ConstToPixel(5);

                //    drawLine(point1, point2, sz * 0.5f);

                //    ConstBuf::global[2] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
                //    ConstBuf::Update(5, ConstBuf::global);
                //}
            }
            else
            {
                ConstBuf::global[2] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); // Белый
                ConstBuf::Update(5, ConstBuf::global);
                ConstBuf::ConstToPixel(5);

                drawLine(point1, point2, sz * 1.5f); // Увеличиваем толщину линии

                // Восстанавливаем стандартный цвет
                ConstBuf::global[2] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
                ConstBuf::Update(5, ConstBuf::global);
            }
        }
    }

    void drawStarPulse(Constellation& Constellation, bool colorOverride = false, float finalStarRad = 10.f)
    {
        std::vector <point3d>& starArray = Constellation.starsCords;
        std::vector <float>& starHealth = Constellation.starsHealth;

        int starsCount = starArray.size();

        for (int i = 0; i < starsCount; i++)
        {
            point3d point;
            point = starArray[i];
            point3d worldPoint = TransformPoint(point, Constellation.Transform);

            float currentRadius = (i < Constellation.starsRadius) ? Constellation.starsRadius : finalStarRad;

            XMVECTOR p = XMVECTOR{ worldPoint.x,worldPoint.y,worldPoint.z,1 };
            p = XMVector4Transform(p, XMMatrixTranspose(ConstBuf::camera.view[0]) * XMMatrixTranspose(ConstBuf::camera.proj[0]));

            float px = .5f * XMVectorGetX(p) / XMVectorGetW(p) + .5f;
            float py = -.5f * XMVectorGetY(p) / XMVectorGetW(p) + .5f;

            point3d screenPoint = { px,py,0 };
            screenPoint.x *= window.width;
            screenPoint.y *= window.height;

            // Изменяем цвет или размер для неповрежденных звезд
            //float currentRadius = finalStarRad;
            XMFLOAT4 starColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); // Белый по умолчанию

            if (i < starHealth.size())
            {
                if (starHealth[i] > 0)
                {
                    // Для неповрежденных звезд - пульсация и другой цвет
                    float pulse = 0.5f + 0.5f * sinf(currentTime * 0.005f);
                    currentRadius += pulse * 5.0f;

                    // Можно использовать другой цвет, например желтый
                    starColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); // Желтый
                }
                else
                {
                    // Для поврежденных звезд - серый цвет
                    starColor = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.25f); // Серый с прозрачностью
                }
            }

            // Устанавливаем цвет звезды
            ConstBuf::global[1] = starColor;
            ConstBuf::Update(5, ConstBuf::global);
            ConstBuf::ConstToPixel(5);

            if (currentRadius > 0) {
                point.draw(worldPoint, currentRadius);
            }

            // Восстанавливаем стандартный цвет
            ConstBuf::global[1] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
            ConstBuf::Update(5, ConstBuf::global);
        }
    }

    void drawConstellation(Constellation& Constellation, bool colorOverride = false , float finalStarRad = 10.f, float sz =2.f)
    {
        if (&Constellation == starSet[player_sign]) {
            finalStarRad *= heroScale;
            sz *= heroScale;
        }
        Blend::Blending(Blend::blendmode::on, Blend::blendop::add);
        Shaders::vShader(1);
        Shaders::pShader(1);
        Shaders::gShader(0);

        drawStarPulse(Constellation, colorOverride, finalStarRad);

        Shaders::vShader(4);
        Shaders::pShader(4);

        drawLinks(Constellation,sz);
    }

    void DrawRenderObject(RenderObject* object)
    {
        Shaders::vShader(object->vs_id);
        Shaders::pShader(object->ps_id);
        Shaders::gShader(object->gs_id);
        Blend::Blending(object->blendmode, object->blendop);

        point3d camPos = point3d(XMVectorGetX(Camera::state.Eye), XMVectorGetY(Camera::state.Eye), XMVectorGetZ(Camera::state.Eye));
        float camDist = (camPos - object->pos).magnitude();

        if (camDist < object->renderDistance || object->renderDistance < 0)
        {
            if (object->instances > 1)
                //context->DrawInstanced(object->vertexes, object->instances * (1 - min(camDist / object->lodDistanceStep, 1) / object->lodCount), 0, 0);
                context->DrawInstanced(object->vertexes, object->instances, 0, 0);
            else
                context->Draw(object->instances * object->vertexes, 0);
        }
    }

    const COLORREF colors[] =
    {
        RGB(255, 0, 0),    // Красный
        RGB(255, 165, 0),  // Оранжевый
        RGB(255, 255, 0),  // Желтый
        RGB(0, 255, 0),    // Зеленый
        RGB(0, 255, 255),  // Голубой
        RGB(0, 0, 255),    // Синий
        RGB(128, 0, 128)   // Фиолетовый
    };




    void drawPlayerСonstellationToMenu()
    {
        Shaders::vShader(1);
        Shaders::pShader(1);
        startTime = currentTime;
        int n = (currentTime / 1000) % starSet.size();
        uiFunc = &menuUI;
        //linksDivider = 15;
        if (gameState == gameState_::confirmSign) n = player_sign;
        Constellation& c = *starSet[n];
        float t = currentTime * 0.001;
        c.Transform = XMMatrixScaling(120, 120, 120) * XMMatrixRotationY(t) * XMMatrixTranslation(0, 0, 500);
        drawConstellation(*starSet[player_sign]);
    }



   


    

    void DrawStarsHP(HDC hdc) {

        for (size_t i = 0; i < starSet[currentEnemyID]->starsCords.size(); ++i) {
            auto pos = starSet[currentEnemyID]->starsCords[i];
            float hp = starSet[currentEnemyID]->starsHealth[i];

            modelTransform(pos, *starSet[currentEnemyID]);
            modelProject(pos);

            if (hp > 0)
            {
                std::string hpText = "HP: " + std::to_string(hp);
                drawString(hpText.c_str(), pos.x, pos.y - 40, .5, true, false, 7);
            }
        }
    }
    void DrawHpEnemyBar()
    {
        auto enemy_const = *starSet[currentEnemyID];
        auto maxHP = enemy_const.maxHP;
        auto progress = getConstellationHP(*starSet[currentEnemyID]) / maxHP;
        auto progressText = "HP: " + std::to_string(progress);

        //linksDivider = 15;
        modelTransform = &placeConstToWorld;
        uiFunc = starIntersectUI;
        modelProject = &fightProject;

        // Параметры временного бара
        const float barWidth = 300;
        const float barHeight = 60;
        const float barX = 0;
        const float barY = .3;
        const float starCount = 10;
        const float starSize = 5.0f;

        point3d Bar = { barX, barY,0 };

        modelTransform(Bar, *starSet[currentEnemyID]);
        modelProject(Bar);

        // Создаем точки для рамки
        point3d topLeft = { Bar.x - barWidth / 2, Bar.y, 0 };
        point3d topRight = { Bar.x + barWidth / 2, Bar.y, 0 };
        point3d bottomLeft = { Bar.x - barWidth / 2, Bar.y + barHeight, 0 };
        point3d bottomRight = { Bar.x + barWidth / 2, Bar.y + barHeight, 0 };

        modelProject = NULL;
        // Рисуем рамку
        drawLine(topLeft, topRight);    // Верх
        drawLine(bottomLeft, bottomRight); // Низ
        drawLine(topLeft, bottomLeft);  // Лево
        drawLine(bottomRight, topRight); // Право

        // Рисуем заполнение из звёзд
        int activeStars = (int)(starCount * progress);
        float starSpacing = barWidth / (float)(starCount + 1);

        for (int i = 0; i < starCount; i++) {
            point3d star = {
                Bar.x + (i + 1) * starSpacing - barWidth / 2,
                Bar.y + barHeight / 2,
                Bar.z
            };

            float currentSize = (i < activeStars) ? starSize * 1.5f : starSize * .0;

            if (i < activeStars) {
                float pulse = 0.5f + 0.5f * sinf(currentTime * 0.005f);
                currentSize += pulse * 2.0f;
            }

            star.draw(star, currentSize);
        }

        // Соединяем звёзды
        for (int i = 0; i < starCount - 1; i++) {
            point3d star1 = {
                Bar.x + (i + 1) * starSpacing - barWidth / 2,
                Bar.y + barHeight / 2,
                Bar.z
            };

            point3d star2 = {
                Bar.x + (i + 2) * starSpacing - barWidth / 2,
                Bar.y + barHeight / 2,
                Bar.z
            };

            drawLine(star1, star2);
        }
        modelProject = &fightProject;
        point3d p = { 0,.3,0 };
        modelTransform(p, enemy_const);
        modelProject(p);
        drawString(progressText.c_str(), p.x, p.y - 50, 1, true);

    }

    void DrawHpHeroBar()
    {
        //Shaders::vShader(1);
        //Shaders::pShader(1);
        //drawString("TEST", (1. / 2) * window.width, (1. / 2) * window.height, .7f, false);


        auto player_const = *starSet[player_sign];
        auto maxHP = player_const.maxHP;
        auto progress = getConstellationHP(*starSet[player_sign]) / maxHP;
        auto progressText = "HP: " + std::to_string(progress);

        //linksDivider = 15;
        modelTransform = &HeroUITransform;
        uiFunc = NULL;
        modelProject = &fightProject;

        // Параметры временного бара
        const float barWidth = 4;
        const float barHeight = .25;
        const float barX = -barWidth / 2;
        const float barY = -barHeight / 2 + 2.5;
        const float starCount = 20;
        const float starSize = 5.0f;

        // Создаем точки для рамки
        point3d topLeft = { barX, barY, 0 };
        point3d topRight = { barX + barWidth, barY, 0 };
        point3d bottomLeft = { barX, barY + barHeight, 0 };
        point3d bottomRight = { barX + barWidth, barY + barHeight, 0 };

        modelTransform(topLeft, player_const);
        modelTransform(topRight, player_const);
        modelTransform(bottomLeft, player_const);
        modelTransform(bottomRight, player_const);

        // Рисуем рамку
        drawer::drawLine(topLeft, topRight);    // Верх
        drawer::drawLine(bottomLeft, bottomRight); // Низ
        drawer::drawLine(topLeft, bottomLeft);  // Лево
        drawer::drawLine(bottomRight, topRight); // Право

        // Рисуем заполнение из звёзд
        int activeStars = (int)(starCount * progress);
        float starSpacing = barWidth / (float)(starCount + 1);

        for (int i = 0; i < starCount; i++) {
            point3d star = {
                barX + (i + 1) * starSpacing,
                barY + barHeight / 2,
                0
            };

            float currentSize = (i < activeStars) ? starSize * 1.5f : starSize * .0;

            if (i < activeStars) {
                float pulse = 0.5f + 0.5f * sinf(currentTime * 0.005f);
                currentSize += pulse * 2.0f;
            }

            modelTransform(star, player_const);
            modelProject(star);

            star.draw(star, currentSize);
        }

        // Соединяем звёзды
        for (int i = 0; i < starCount - 1; i++) {
            point3d star1 = {
                barX + (i + 1) * starSpacing,
                barY + barHeight / 2,
                0
            };

            point3d star2 = {
                barX + (i + 2) * starSpacing,
                barY + barHeight / 2,
                0
            };

            modelTransform(star1, player_const);
            modelTransform(star2, player_const);

            drawer::drawLine(star1, star2);
        }

        point3d p = { 0,2,0 };
        modelTransform(p, player_const);
        modelProject(p);
        drawString(progressText.c_str(), p.x, p.y, 1, true);
    }

    


    void UpdateGame() {
        static const DWORD MAX_BATTLE_TIME = 4 * 60 * 1000;
        static const DWORD MAX_REWIND = 30 * 1000;
        //static DWORD battleTime = 60 * 5 * 1000;
        static DWORD battleTime = 120 * 1000;

        static DWORD timeModifier = 0;
        static DWORD lastInputTime = 0;
        const DWORD inputRepeatDelay = 100;

        /*if (GetAsyncKeyState('Q')) {
            if (currentTime - lastInputTime > inputRepeatDelay) {
                lastInputTime = currentTime;
                if (battleStartTime + battleTime + timeModifier + 1000 - currentTime <= MAX_BATTLE_TIME) {
                    timeModifier += 1000;
                }
            }
        }

        if (GetAsyncKeyState('E') & 0x8000) {
            if (currentStateIndex > 0) {
                RewindOneStepBack();
                isRewind = true;
            }
            else {
                isRewind = false;
            }
        }
        else {
            isRewind = false;
        }*/

        if (isBattleActive) {
            LONG remainingTime = (LONG)((battleStartTime + battleTime + timeModifier) - currentTime);

            DWORD totalBattleTime = battleTime + timeModifier;

            if (totalBattleTime > MAX_BATTLE_TIME) {
                timeModifier = MAX_BATTLE_TIME - battleTime;
                remainingTime = (LONG)((battleStartTime + MAX_BATTLE_TIME) - currentTime);
            }

            //if (remainingTime > 0) {
            //
            //    //DrawHpHeroBar();
            //    std::string timeStr = "Time " + (std::to_string(remainingTime / 1000 / 60)) + ":" + std::to_string(remainingTime / 1000 % 60 );
            //    drawString(timeStr.c_str(), window.width / 1.1, 45, 1.f, true);
            //
            //    if (getConstellationHP(*starSet[currentEnemyID]) < 0)
            //    {
            //        timeModifier = 0;
            //        isBattleActive = false;
            //        gameState = gameState_::WinFight;
            //    }
            //}
            //else
            //{
            //    timeModifier = 0;
            //    isBattleActive = false;
            //    gameState = gameState_::EndFight;
            //
            //}
        }
    }

    struct ReflectionEffect {
        point3d position;
        point3d direction;
        float radius;
        DWORD creationTime;
        float lifetime;
        XMFLOAT4 color;
        bool isActive;
    };

    std::vector<ReflectionEffect> reflectionEffects;
    bool isShieldReflecting = false;
    DWORD shieldReflectionStartTime = 0;
    const float SHIELD_REFLECTION_DURATION = 500.0f;

    // Функция для создания эффекта отражения
    void CreateShieldReflectionEffect(point3d shieldPos, point3d reflectionDirection) {
        ReflectionEffect effect;
        effect.position = shieldPos;
        effect.direction = reflectionDirection.normalized();
        effect.radius = 500.0f;
        effect.creationTime = currentTime;
        effect.lifetime = 800.0f;
        effect.color = XMFLOAT4(0.2f, 0.6f, 1.0f, 1.0f); // Синий цвет отражения
        effect.isActive = true;

        reflectionEffects.push_back(effect);

        // Создаем дополнительные частицы для эффекта
        const int particleCount = 12;
        for (int i = 0; i < particleCount; i++) {
            ReflectionEffect particle;
            particle.position = shieldPos;

            // Разброс направления для частиц
            float angle = (i / (float)particleCount) * 2 * PI;
            point3d randomDir = point3d(cos(angle), sin(angle), 0).normalized();
            particle.direction = (reflectionDirection + randomDir * 0.3f).normalized();

            particle.radius = GetRandom(100, 300);
            particle.creationTime = currentTime;
            particle.lifetime = GetRandom(400, 800);
            particle.color = XMFLOAT4(0.3f, 0.7f, 1.0f, 0.8f);
            particle.isActive = true;

            reflectionEffects.push_back(particle);
        }

        //ProcessSound("..\\dx11minimal\\Resourses\\Sounds\\ShieldReflect.wav");
    }

    // Обновление эффектов отражения
    void UpdateReflectionEffects(float deltaTime) {
        for (auto it = reflectionEffects.begin(); it != reflectionEffects.end();) {
            ReflectionEffect& effect = *it;

            if (!effect.isActive || (currentTime - effect.creationTime) > effect.lifetime) {
                it = reflectionEffects.erase(it);
                continue;
            }

            // Движение эффекта отражения
            effect.position += effect.direction * 25.0f * deltaTime;

            // Уменьшение размера со временем
            float lifeProgress = (currentTime - effect.creationTime) / effect.lifetime;
            effect.radius *= 0.98f;

            // Изменение прозрачности
            effect.color.w = 1.0f - lifeProgress;

            ++it;
        }
    }

    // Отрисовка эффектов отражения
    void RenderReflectionEffects() {
        if (reflectionEffects.empty()) return;

        Shaders::vShader(1);
        Shaders::pShader(1);
        Blend::Blending(Blend::blendmode::on, Blend::blendop::add);

        for (auto& effect : reflectionEffects) {
            if (!effect.isActive) continue;

            // Устанавливаем цвет эффекта
            ConstBuf::global[1] = effect.color;
            ConstBuf::Update(5, ConstBuf::global);
            ConstBuf::ConstToPixel(5);

            // Рисуем основную звезду отражения
            effect.position.draw(effect.position, effect.radius);

            // Добавляем свечение
            float glowRadius = effect.radius * 1.5f;
            XMFLOAT4 glowColor = effect.color;
            glowColor.w *= 0.4f;

            ConstBuf::global[1] = glowColor;
            ConstBuf::Update(5, ConstBuf::global);
            effect.position.draw(effect.position, glowRadius);

            // Линия траектории отражения
            Shaders::vShader(4);
            Shaders::pShader(4);

            point3d trailEnd = effect.position - effect.direction * effect.radius * 2.0f;
            float trailAlpha = effect.color.w * 0.3f;

            ConstBuf::global[2] = XMFLOAT4(effect.color.x, effect.color.y, effect.color.z, trailAlpha);
            ConstBuf::Update(5, ConstBuf::global);
            ConstBuf::ConstToPixel(5);

            drawLine(effect.position, trailEnd, effect.radius * 0.5f);

            Shaders::vShader(1);
            Shaders::pShader(1);
        }

        // Восстанавливаем стандартный цвет
        ConstBuf::global[1] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
        ConstBuf::Update(5, ConstBuf::global);
    }


    

    // Теперь храним звёзды с их направлениями

    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    bool CheckWeaponCollision(const StarProjectile& weapon, const point3d& sphereCenter, float sphereRadius)
    {
        switch (weapon.weapon) {

        case weapon_name::Fists: {
            // Сфера для кулака
            float distance = (weapon.position - sphereCenter).magnitude();
            return distance <= (weapon.radius + sphereRadius);
        }

        case weapon_name::Sword: {
            // Капсула для меча (линия с радиусом)
            point3d swordStart = weapon.position;
            point3d swordEnd = weapon.position + weapon.direction * weapon.radius * 3.f; // Длина меча
            point3d swordDir = (swordEnd - swordStart).normalized();
            float swordLength = (swordEnd - swordStart).magnitude();

            point3d toSphere = sphereCenter - swordStart;
            float projection = toSphere.dot(swordDir);

            // Ограничиваем проекцию длиной меча
            projection = max(0.0f, min(projection, swordLength));

            point3d closestPoint = swordStart + swordDir * projection;
            float distance = (closestPoint - sphereCenter).magnitude();

            return distance <= (weapon.radius + sphereRadius); // weapon.radius - толщина меча
        }

        case weapon_name::Shield: {
            // Диск/круг для щита
            point3d toSphere = sphereCenter - weapon.position;

            // Проекция на плоскость щита
            float distanceToPlane = fabs(toSphere.dot(weapon.up));

            // Если сфера слишком далеко от плоскости щита - нет коллизии
            if (distanceToPlane > sphereRadius) return false;

            // Проекция на плоскость щита
            point3d projected = sphereCenter - weapon.up * toSphere.dot(weapon.up);
            float distanceInPlane = (projected - weapon.position).magnitude();

            // Проверяем попадание в диск щита
            bool collision = distanceInPlane <= (weapon.radius + sphereRadius);

            // Если есть коллизия и это атака врага - создаем эффект отражения
            if (collision && weapon.weapon == weapon_name::Fists) {
                point3d reflectionDir = (sphereCenter - weapon.position).normalized() * -1.0f;
                CreateShieldReflectionEffect(weapon.position, reflectionDir);
            }

            return collision;
        }

        case weapon_name::Bow: {
            // Сфера для стрелы (как кулак, но можно сделать более сложную)
            float distance = (weapon.position - sphereCenter).magnitude();
            return distance <= (weapon.radius + sphereRadius);
        }

        default:
            return false;
        }
    }

    void UpdateAttackStars(float deltaTime) {
        
        for (auto& star : attackStars) {
            star.position += star.direction.normalized() * 20.0f * deltaTime;
            point3d posrand = point3d{ ((float)GetRandom(-200,200)),((float)GetRandom(-200,200)),((float)GetRandom(-200,200)) };
            // Для лука создаем новые частицы трассера вдоль пути
            if (star.weapon == weapon_name::Bow && (currentTime - star.creationTime) % 30 == 0) {
                BowTracerParticle tracer;
                tracer.position = star.position + posrand;
                tracer.size = star.radius * 0.2f;
                tracer.creationTime = currentTime;
                tracer.lifetime = 4000.0f;
                bowTracerParticles.push_back(tracer);
            }
        }

        // Обновляем частицы трассера
        for (auto it = bowTracerParticles.begin(); it != bowTracerParticles.end();) {
            float elapsed = currentTime - it->creationTime;
            if (elapsed > it->lifetime) {
                it = bowTracerParticles.erase(it);
            }
            else {
                // Постепенно уменьшаем размер
                it->size *= 0.98f;
                ++it;
            }
        }
    }
    

    void DrawAttackStars() {

        for (auto& star : attackStars) 
        {

             switch (star.weapon)
             {

             case weapon_name::Fists: {

                 Shaders::vShader(15);
                 Shaders::pShader(15);

                 // Проверяем, новая ли это частица (создана при нажатии ЛКМ)
                 if (star.isNew) {
                     float timeSinceCreation = (currentTime - star.creationTime);

                     // Анимация взрыва только для новых частиц в течение 600ms
                     if (timeSinceCreation < 600) {
                         // Волна
                         float waveRadius = star.radius * (timeSinceCreation / 200.0f);
                         if (waveRadius > 0 && waveRadius < star.radius * 3) {
                             for (int i = 0; i < 36; i++) {
                                 float angle = i * (2 * PI / 36);
                                 point3d wavePoint = star.position +
                                     (star.right * cos(angle) + star.up * sin(angle)) * waveRadius;

                                 float waveAlpha = 1.0f - (waveRadius / (star.radius * 3));
                                 wavePoint.draw(wavePoint, 15.0f * waveAlpha);
                             }
                         }

                         for (int i = 0; i < 12; i++) {
                             point3d randomDir(dist(gen), dist(gen), 0);
                             randomDir = randomDir.normalized();

                             float particleDist = star.radius * (timeSinceCreation / 400.0f) * (0.8f + dist(gen) * 0.4f);
                             point3d particlePos = star.position + randomDir * particleDist;

                             float particleAlpha = 1.0f - (timeSinceCreation / 600.0f);
                             if (particleAlpha > 0) {
                                 particlePos.draw(particlePos, 30.0f * particleAlpha);
                             }
                         }
                     }

                     // Снимаем флаг "новой" после завершения анимации
                     if (timeSinceCreation >= 600) {
                         star.isNew = false;
                     }
                 }

                 // Обычная отрисовка частицы (всегда)
                 star.position.draw(star.position, star.radius * 0.2f);
                 break;
             }

             case weapon_name::Sword: {

                    Shaders::vShader(15);
                    Shaders::pShader(15);
                    float timeSinceCreation = (currentTime - star.creationTime);

                    point3d end = star.position + star.direction;
                        drawLine(star.position, end, 3.f);
                        star.position.draw(star.position , finalRadius);
                    
                    break;
                }

                case weapon_name::Shield: {

                    Shaders::vShader(15);
                    Shaders::pShader(15);
                   

                        for (int i = 0; i < 36; i++) {
                            float angle = i * (2 * PI / 36);
                            float nextAngle = (i + 1) * (2 * PI / 36);

                            point3d local1(cos(angle), sin(angle), 0);
                            point3d local2(cos(nextAngle), sin(nextAngle), 0);

                            point3d shield1 = star.position + (star.right * local1.x + star.up * local1.y) * star.radius;
                            point3d shield2 = star.position + (star.right * local2.x + star.up * local2.y) * star.radius;

                            drawLine(shield1, shield2, 30.f);
                        }
                        star.position.draw(star.position, finalRadius);
                    
                    break;
                }

                case weapon_name::Bow: {

                    Shaders::vShader(15);
                    Shaders::pShader(15);

                    // Отрисовываем трассерные частицы сначала (чтобы были под стрелой)
                    for (auto& tracer : bowTracerParticles) {
                        float elapsed = currentTime - tracer.creationTime;
                        float alpha = 1.0f - (elapsed / tracer.lifetime);

                        // Устанавливаем прозрачность через размер
                        float renderSize = tracer.size * alpha;
                        if (renderSize > 0.5f) {
                            tracer.position.draw(tracer.position, renderSize);
                        }
                    }

                    // Отрисовываем основную стрелу
                    point3d arrowStart = star.position;
                    point3d arrowEnd = star.position + star.direction * finalRadius * 3.f;

                    // Ядро стрелы - яркая звезда
                    star.position.draw(star.position, finalRadius * 0.8f);

                    // Внешнее свечение
                    star.position.draw(star.position, finalRadius * 1.2f);

                    // Линия трассера
                    Shaders::vShader(4);
                    Shaders::pShader(4);
                    drawLine(arrowStart, arrowEnd, finalRadius * 2.f);

                    // Наконечник стрелы
                    Shaders::vShader(1);
                    Shaders::pShader(1);
                    point3d tip = arrowEnd - star.direction * finalRadius * 0.5f;
                    tip.draw(tip, finalRadius * 0.4f);

                    break;
                }

             }
        }
       
    }
    struct AccumulationStar {
        point3d startPosition;  // Начальная позиция звезды
        point3d currentPosition; // Текущая позиция
        point3d targetPosition;  // Целевая позиция в сфере
        float radius;
        float speed;
        DWORD creationTime;
        bool isActive;
        XMFLOAT4 color;
    };

    std::vector<AccumulationStar> accumulationStars;
    point3d accumulationCenter; // Центр накопления
    bool isAccumulating = false;
    DWORD accumulationStartTime = 0;
    const float ACCUMULATION_DURATION = 1000.0f; // Время накопления в ms

    // Функция для создания эффекта накопления
    void CreateSpiralAccumulationEffect(point3d center, float sphereRadius) {
        accumulationStars.clear();
        accumulationCenter = center;
        isAccumulating = true;
        accumulationStartTime = currentTime;

        const int spiralArms = 8;    // Количество спиральных рукавов
        const int starsPerArm = 25;  // Звёзд на рукав

        for (int arm = 0; arm < spiralArms; arm++) {
            for (int i = 0; i < starsPerArm; i++) {
                AccumulationStar star;

                // Спиральное расположение начальных позиций
                float armAngle = (arm / (float)spiralArms) * 2 * PI;
                float spiralProgress = (i / (float)starsPerArm);
                float spiralRadius = sphereRadius * 8.0f * (1.0f - spiralProgress * 0.3f);

                star.startPosition = {
                    center.x + spiralRadius * cosf(armAngle + spiralProgress * PI * 4),
                    center.y + GetRandom(-spiralRadius * 0.5f, spiralRadius * 0.5f),
                    center.z + spiralRadius * sinf(armAngle + spiralProgress * PI * 4)
                };

                // Целевая позиция - равномерно на сфере
                float goldenRatio = (1.0f + sqrt(5.0f)) / 2.0f;
                float targetIndex = arm * starsPerArm + i;
                float targetY = 1.0f - (targetIndex / (float)(spiralArms * starsPerArm)) * 2.0f;
                float targetRadius = sqrt(1.0f - targetY * targetY);
                float targetTheta = 2 * PI * targetIndex / goldenRatio;

                star.targetPosition = {
                    center.x + sphereRadius * targetRadius * cosf(targetTheta),
                    center.y + sphereRadius * targetY,
                    center.z + sphereRadius * targetRadius * sinf(targetTheta)
                };

                star.currentPosition = star.startPosition;
                star.radius = GetRandom(150, 450);
                star.speed = 0.5f + spiralProgress * 1.5f; // Внешние звёзды движутся быстрее
                star.creationTime = currentTime;
                star.isActive = true;

                // Цвет зависит от рукава спирали
                float hue = (arm / (float)spiralArms);
                star.color = XMFLOAT4(
                    1.0f,                    // R
                    0.6f + hue * 0.4f,       // G  
                    0.3f + hue * 0.7f,       // B
                    1.0f                     // A
                );

                accumulationStars.push_back(star);
            }
        }
    }

    // Обновление эффекта накопления
    void UpdateAccumulationEffect(float deltaTime) {
        if (!isAccumulating) return;

        float progress = (currentTime - accumulationStartTime) / ACCUMULATION_DURATION;
        progress = min(progress, 1.0f);

        // Нелинейная интерполяция для более динамичного движения
        float easeProgress = 1.0f - pow(1.0f - progress, 3.0f);

        for (auto& star : accumulationStars) {
            if (!star.isActive) continue;

            // Движение к целевой позиции
            star.currentPosition = star.startPosition.lerp(star.targetPosition, easeProgress);

            // Вращение вокруг центра
            float rotationAngle = progress * PI * 2.0f;
            point3d toCenter = accumulationCenter - star.currentPosition;
            toCenter.rotateY(toCenter,rotationAngle * star.speed);
            star.currentPosition = accumulationCenter - toCenter;

            // Пульсация размера
            float pulse = 0.8f + 0.4f * sinf(currentTime * 0.01f + star.speed * 10.0f);
            star.radius *= pulse;

            // Деактивация звёзд при достижении цели
            float distanceToTarget = (star.currentPosition - star.targetPosition).magnitude();
            if (progress >= 0.95f && distanceToTarget < 100.0f) {
                star.isActive = false;
            }
        }

        // Завершение эффекта
        if (progress >= 1.0f) {
            isAccumulating = false;
            // Здесь можно запустить взрыв
            //CreateExplosionEffects(accumulationCenter, 2000.0f);
        }
    }

    // Отрисовка эффекта накопления
    void RenderAccumulationEffect() {
        if (!isAccumulating) return;

        Shaders::vShader(1);
        Shaders::pShader(1);
        Blend::Blending(Blend::blendmode::on, Blend::blendop::add);

        float progress = (currentTime - accumulationStartTime) / ACCUMULATION_DURATION;

        for (auto& star : accumulationStars) {
            if (!star.isActive) continue;

            // Прозрачность зависит от прогресса
            float alpha = 1.0f;
            if (progress > 0.8f) {
                alpha = 1.0f - (progress - 0.8f) / 0.2f;
            }

            XMFLOAT4 finalColor = star.color;
            finalColor.w = alpha;

            ConstBuf::global[1] = finalColor;
            ConstBuf::Update(5, ConstBuf::global);
            ConstBuf::ConstToPixel(5);

            // Рисуем звезду
            star.currentPosition.draw(star.currentPosition, star.radius);

            // Линии к центру для некоторых звёзд
            if (GetRandom(0, 100) < 30) { // 30% звёзд имеют линии
                Shaders::vShader(4);
                Shaders::pShader(4);

                float lineAlpha = alpha * 0.3f;
                ConstBuf::global[2] = XMFLOAT4(1.0f, 0.9f, 0.7f, lineAlpha);
                ConstBuf::Update(5, ConstBuf::global);
                ConstBuf::ConstToPixel(5);

                drawLine(star.currentPosition, accumulationCenter, 50.0f);

                Shaders::vShader(1);
                Shaders::pShader(1);
            }
        }

        // Восстанавливаем стандартный цвет
        ConstBuf::global[1] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
        ConstBuf::Update(5, ConstBuf::global);
    }

    bool isAttacking = false;
    
    const float projectileSpeed = 2.0f;
    point3d mouseRay;
    point3d start;
    

    float chargeStartTime = 0;
    bool isCharging = false;
    const float MAX_CHARGE_TIME = 2000.0f; 
    const float MIN_ATTACK_RADIUS = 30.0f; 
    const float MAX_ATTACK_RADIUS = 2000.0f; 
    
    static bool wasPressed = false;
    static bool useLeftFist = true;

    const float MIN_ATTACK_DAMAGE = 0.1f;
    const float MAX_ATTACK_DAMAGE = 0.5f;
    
    float CameraScale = 1.f;
    float chargeRatio;

    
    std::string P;

    
    bool isCameraReturning = false;

    float currentCameraDistance = MIN_CAMERA_DIST;
    float cameraReturnStartDistance = MIN_CAMERA_DIST;
    DWORD cameraReturnStartTime = 0;
    
    float cameraReturnSpeed = 2.0f;


    float currentHeroScale = MIN_HERO_SCALE;
    float heroScaleReturnStart = MIN_HERO_SCALE;
    DWORD heroScaleReturnStartTime = 0;
    bool isHeroScaleReturning = false;
    float heroScaleReturnSpeed = 2.0f;


   
    string C;

    void HandleMouseClick(XMVECTOR heroPosition, Constellation& player) {
        bool isPressed = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

        // Обнаружение момента нажатия
        if (isPressed && !wasPressed) {
            useLeftFist = !useLeftFist;
        }
        wasPressed = isPressed;

        if (isPressed) {
            if (!isCharging) {
                isCharging = true;
                chargeStartTime = currentTime;
                isCameraReturning = false;
                isHeroScaleReturning = false;
                isStarRadiusReturning = false;
            }

            float chargeDuration = currentTime - chargeStartTime;
            chargeRatio = min(chargeDuration / MAX_CHARGE_TIME, 1.0f);
            finalRadius = MIN_ATTACK_RADIUS + chargeRatio * (MAX_ATTACK_RADIUS - MIN_ATTACK_RADIUS);

            // ПРЯМОЕ ПРИМЕНЕНИЕ ТРЯСКИ К МАТРИЦЕ
            if (chargeRatio > 0.3f) {
                float intensity = 0.1f + (chargeRatio - 0.3f) * 0.3f;

                // Немедленно применяем тряску к матрице
                float shakeX = GetRandom(-intensity * 100, intensity * 100);
                float shakeY = GetRandom(-intensity * 100, intensity * 100);
                float shakeZ = GetRandom(-intensity * 100, intensity * 100);

                XMMATRIX shakeMatrix = XMMatrixTranslation(shakeX, shakeY, shakeZ);
                player.Transform = player.Transform * shakeMatrix;
            }

           
            // Остальной код зарядки...
            targetHeroScale = MIN_HERO_SCALE + chargeRatio * (MAX_HERO_SCALE - MIN_HERO_SCALE);
            targetCameraDistance = MIN_CAMERA_DIST + chargeRatio * (MAX_CAMERA_DIST - MIN_CAMERA_DIST);
            currentStarRadius = MIN_STAR_RADIUS + chargeRatio * (MAX_STAR_RADIUS - MIN_STAR_RADIUS);
            C = to_string(currentStarRadius);

            currentCameraDistance = targetCameraDistance;
            currentHeroScale = targetHeroScale;

            Camera::state.distanceOffset = currentCameraDistance;
            player.SetStarSpacing(currentHeroScale);

            for (int i = 0; i < player.starsCords.size(); i++) {
                player.SetStarRadius(i, currentStarRadius);
            }

            currentDamage = MIN_ATTACK_DAMAGE + chargeRatio * (MAX_ATTACK_DAMAGE - MIN_ATTACK_DAMAGE);

            
        }
        else {
            if (isCharging) {
                isCharging = false;
                isCameraReturning = true;
                isHeroScaleReturning = true;
                isStarRadiusReturning = true;

                cameraReturnStartTime = currentTime;
                heroScaleReturnStartTime = currentTime;
                starRadiusReturnStartTime = currentTime;

                // Сохраняем текущие значения для плавного возврата
                cameraReturnStartDistance = currentCameraDistance;
                heroScaleReturnStart = currentHeroScale;
                starRadiusReturnStart = currentStarRadius;

                if (currentTime - lastAttackTime > 400) {
                    if (gameState == gameState_::selectEnemy) {
                        gameState = gameState_::Fight;
                        mciSendString(TEXT("stop ..\\dx11minimal\\Resourses\\Sounds\\FREEFLY.mp3"), NULL, 0, NULL);
                        mciSendString(TEXT("play ..\\dx11minimal\\Resourses\\Sounds\\Oven_NEW.mp3"), NULL, 0, NULL);
                    }
                   lastAttackTime = currentTime;

                    float attackCost = 0.0f;
                    switch (current_weapon) {
                    case weapon_name::Fists: attackCost = energyCost.fists; break;
                    case weapon_name::Sword: attackCost = energyCost.sword; break;
                    case weapon_name::Shield: attackCost = energyCost.shield; break;
                    case weapon_name::Bow: attackCost = energyCost.bow; break;
                    }

                    // Умножаем на множитель зарядки
                    attackCost *= (1.0f + chargeRatio * (energyCost.chargeMultiplier - 1.0f));

                    // ПРОВЕРКА ДОСТАТОЧНОСТИ ЭНЕРГИИ
                    if (energy < attackCost) {
                        // Недостаточно энергии - атака не выполняется
                        //ProcessSound("..\\dx11minimal\\Resourses\\Sounds\\NoEnergy.wav"); // Добавьте звук
                        isCameraReturning = true;
                        isHeroScaleReturning = true;
                        return;
                    }
                    energy -= (LONG)attackCost;

                    start = point3d(
                        XMVectorGetX(heroPosition),
                        XMVectorGetY(heroPosition),
                        XMVectorGetZ(heroPosition)
                    );

                    point3d camPos = point3d(
                        XMVectorGetX(Camera::state.Eye),
                        XMVectorGetY(Camera::state.Eye),
                        XMVectorGetZ(Camera::state.Eye)
                    );

                    mouseRay = GetMouseRay(mouse.pos);
                    point3d mousePos = (camPos + mouseRay * 6000 * Camera::state.distanceOffset);
                    point3d newDirection = (mousePos - start).normalized();

                    // Определяем позицию кулака в зависимости от текущей руки
                    point3d fistPosition;
                    if (useLeftFist) {
                        fistPosition = start + point3d{ 200, 0, 0 };
                    }
                    else {
                        fistPosition = start - point3d{ 200, 0, 0 };
                    }

                    switch (current_weapon) {
                    case weapon_name::Fists: {
                        StarProjectile newStar;
                        newStar.position = fistPosition;
                        newStar.direction = newDirection;
                        newStar.radius = finalRadius;
                        newStar.weapon = weapon_name::Fists;
                        newStar.isNew = true;
                        newStar.creationTime = currentTime;

                        newStar.up = point3d(XMVectorGetX(Camera::state.Up),
                            XMVectorGetY(Camera::state.Up),
                            XMVectorGetZ(Camera::state.Up));
                        newStar.right = newDirection.cross(newStar.up).normalized();
                        newStar.up = newStar.right.cross(newDirection).normalized();

                        attackStars.push_back(newStar);
                        break;
                    }
                    case weapon_name::Sword: {
                        Hero::state.isAttackRotating = true;
                        Hero::state.attackStartTime = currentTime;
                        Hero::state.attackRotationProgress = 0.0f;

                        for (int i = 0; i < 25; i++) {
                            StarProjectile newStar;
                            newStar.position = start - point3d{ -finalRadius,-finalRadius,0 };
                            newStar.direction = newDirection;
                            newStar.radius = finalRadius;
                            newStar.weapon = weapon_name::Sword;

                            point3d up = point3d(XMVectorGetX(Camera::state.Up),
                                XMVectorGetY(Camera::state.Up),
                                XMVectorGetZ(Camera::state.Up));
                            point3d right = point3d(XMVectorGetX(Camera::state.Right),
                                XMVectorGetY(Camera::state.Right),
                                XMVectorGetZ(Camera::state.Right));

                            newStar.position += up * (i * finalRadius - (finalRadius * 10.f));
                            newStar.position += right * (i * finalRadius - (finalRadius * 10.f));

                            attackStars.push_back(newStar);
                        }

                        ProcessSound("..\\dx11minimal\\Resourses\\Sounds\\Sword.wav");
                        break;
                    }

                    case weapon_name::Shield: {
                        StarProjectile newStar;
                        newStar.position = start;
                        newStar.direction = newDirection;
                        newStar.radius = finalRadius;
                        newStar.weapon = weapon_name::Shield;

                        newStar.up = point3d(XMVectorGetX(Camera::state.Up),
                            XMVectorGetY(Camera::state.Up),
                            XMVectorGetZ(Camera::state.Up));
                        newStar.right = newDirection.cross(newStar.up).normalized();
                        newStar.up = newStar.right.cross(newDirection).normalized();

                        attackStars.push_back(newStar);

                       
                        //CreateShieldReflectionEffect(start, newDirection);
                        ProcessSound("..\\dx11minimal\\Resourses\\Sounds\\ShieldStan3.wav");
                        break;
                    }

                    case weapon_name::Bow: {
                        point3d fixedUp = point3d(XMVectorGetX(Camera::state.Up),
                            XMVectorGetY(Camera::state.Up),
                            XMVectorGetZ(Camera::state.Up));
                        point3d fixedRight = newDirection.cross(fixedUp).normalized();
                        fixedUp = fixedRight.cross(newDirection).normalized();

                        StarProjectile newStar;
                        newStar.position = start;
                        newStar.direction = newDirection;
                        newStar.radius = finalRadius;
                        newStar.weapon = weapon_name::Bow;
                        newStar.up = fixedUp;
                        newStar.right = fixedRight;
                        newStar.creationTime = currentTime;

                        attackStars.push_back(newStar);

                        for (int i = 0; i < 8; i++) {
                            BowTracerParticle tracer;
                            tracer.position = start;
                            tracer.size = finalRadius * 0.3f;
                            tracer.creationTime = currentTime;
                            tracer.lifetime = 300.0f + (rand() % 200);
                            bowTracerParticles.push_back(tracer);
                        }

                        ProcessSound("..\\dx11minimal\\Resourses\\Sounds\\Bow.wav");
                        break;
                    }
                    }
                }

                isAttacking = true;
                current_weapon = weapon_name::Fists;
            }
        }
    }

    void UpdateCameraScaleAndStarRadiusReturn(float deltaTime, Constellation& player) {
        // Обработка возврата камеры
        if (isCameraReturning) {
            float returnProgress = (currentTime - cameraReturnStartTime) / 1000.0f * cameraReturnSpeed;
            returnProgress = min(returnProgress, 1.0f);

            currentCameraDistance = lerp(cameraReturnStartDistance, MIN_CAMERA_DIST, returnProgress);
            Camera::state.distanceOffset = currentCameraDistance;

            if (returnProgress >= 1.0f) {
                isCameraReturning = false;
                currentCameraDistance = MIN_CAMERA_DIST;
                Camera::state.distanceOffset = currentCameraDistance;
            }
        }

        // Обработка возврата размера героя
        if (isHeroScaleReturning) {
            float returnProgress = (currentTime - heroScaleReturnStartTime) / 1000.0f * heroScaleReturnSpeed;
            returnProgress = min(returnProgress, 1.0f);

            currentHeroScale = lerp(heroScaleReturnStart, MIN_HERO_SCALE, returnProgress);
            player.SetStarSpacing(currentHeroScale);

            if (returnProgress >= 1.0f) {
                isHeroScaleReturning = false;
                currentHeroScale = MIN_HERO_SCALE;
                player.SetStarSpacing(MIN_HERO_SCALE);
            }
        }

        // Обработка возврата размера звезд - ПРИМЕНЯЕМ ТОЛЬКО К ИГРОКУ
        if (isStarRadiusReturning) {
            float returnProgress = (currentTime - starRadiusReturnStartTime) / 1000.0f * starRadiusReturnSpeed;
            returnProgress = min(returnProgress, 1.0f);

            // Плавный возврат к нормальному размеру звезд
            currentStarRadius = lerp(starRadiusReturnStart, MIN_STAR_RADIUS, returnProgress);

            // Применяем ТОЛЬКО к звездам игрока
            for (int i = 0; i < player.starsCords.size(); i++) {
                player.SetStarRadius(i, currentStarRadius);
            }

            if (returnProgress >= 1.0f) {
                isStarRadiusReturning = false;
                currentStarRadius = MIN_STAR_RADIUS;
                for (int i = 0; i < player.starsCords.size(); i++) {
                    player.SetStarRadius(i, MIN_STAR_RADIUS);
                }
            }
        }
    }

    float EnemyHpB;
    void CreateConstellationsDamageEffect(point3d heroPos, point3d damageDirection , XMFLOAT4 ColEff);

        std::string enemyH;
        std::string indexStar;
        void UpdateAttack(float deltaTime) {
            if (starSet.empty() || currentEnemyID < 0 || currentEnemyID >= starSet.size() ||
                !starSet[currentEnemyID] || starSet[currentEnemyID]->starsCords.empty()) {
                OutputDebugStringA("Invalid attack state - resetting\n");
                return;
            }

            if (!starSet[currentEnemyID]) return;

            Constellation& enemy = *starSet[currentEnemyID];
            Constellation& Hero = *starSet[player_sign];

            // Получаем общее HP врага
            float enemyTotalHP = getConstellationHP(enemy);

            point3d HeroPosTrans;
            for (int j = 0; j < Hero.starsCords.size(); j++) {
                HeroPosTrans = TransformPoint(Hero.starsCords[j], Hero.Transform);
            }

            for (int i = 0; i < enemy.starsCords.size(); i++) {
                if (enemy.starsHealth[i] <= 0) continue;

                point3d starWorldPos = TransformPoint(enemy.starsCords[i], enemy.Transform);

                // Проверяем все выстрелы на приближение к этой звезде
                for (auto& star : attackStars) {
                    if (CheckWeaponCollision(star, starWorldPos, 2000.f)) {
                        point3d damageDirection = (HeroPosTrans - starWorldPos).normalized();

                        if (current_weapon == weapon_name::Sword) {
                            enemy.starsHealth[i] -= currentDamage * .5f;
                        }
                        else if (current_weapon == weapon_name::Shield) {
                            enemy.starsHealth[i] -= currentDamage * .1f;
                        }
                        else if (current_weapon == weapon_name::Bow) {
                            enemy.starsHealth[i] -= currentDamage * 1.f;
                        }
                        else if (current_weapon == weapon_name::Fists) {
                            enemy.starsHealth[i] -= currentDamage * .2f;
                        }
                        float colorVariant = GetRandom(0, 100) * 0.01f;
                        XMFLOAT4 ColEnemyEff(
                            1.0f,                    // R
                            0.3f + colorVariant * 0.4f, // G (от 0.3 до 0.7)
                            0.1f,                    // B
                            1.0f                     // A
                        );
                        CreateConstellationsDamageEffect(starWorldPos, damageDirection , ColEnemyEff);
                        ProcessSound("..\\dx11minimal\\Resourses\\Sounds\\Damage.wav");
                        break;
                    }
                }
            }

            // Обновляем общее HP врага после нанесения урона
            enemyTotalHP = getConstellationHP(enemy);

            isAttacking = false;
            UpdateAttackStars(deltaTime);

        if (current_weapon == weapon_name::Shield) {

            attackStars.erase(
                std::remove_if(attackStars.begin(), attackStars.end(),
                    [](const StarProjectile& star) {
                        return (star.position - start).magnitude() > 3000.0f;
                    }),
                attackStars.end());
        
        }

        attackStars.erase(
            std::remove_if(attackStars.begin(), attackStars.end(),
                [](const StarProjectile& star) {
                    return (star.position - start).magnitude() > 40000.0f;
                }),
            attackStars.end());

        bowTracerParticles.erase(
            std::remove_if(bowTracerParticles.begin(), bowTracerParticles.end(),
                [](const BowTracerParticle& tracer) {
                    return (currentTime - tracer.creationTime) > tracer.lifetime;
                }),
            bowTracerParticles.end()
        );
    }


    void DrawSwordAttack() {
        if (isAttacking) return;

        Blend::Blending(Blend::blendmode::on);
       
        DrawAttackStars();
    }

    


    struct Particle
    {
        point3d pos;
        point3d vel;
        DWORD startTime;
        DWORD lifetime;
    };

    vector<Particle*> uiParticles = vector<Particle*>{};
    bool isPressed = false;
    void CreateCursorParticles()
    {
        if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
        {
           
            if (!isPressed)
            {
                isPressed = true;
                //ProcessSound("..\\dx11minimal\\Resourses\\Sounds\\Mouse_click1.wav");
                point3d mousePos = point3d(mouse.pos.x / width * 2 - 1, -(mouse.pos.y / height * 2 - 1),0);
                DWORD curTime = timer::GetCounter();

                for (int i = 0; i < 20; i++)
                {
                    Particle* particle = new Particle;
                    particle->pos = mousePos;
                    particle->vel = point3d(GetRandom(-100, 100), GetRandom(-100, 100), 0).normalized()* point3d(aspect, 1, 0) * (float)GetRandom(8, 30) / 100.0f * 0.002f;
                    particle->lifetime = GetRandom(500, 1500);
                    particle->startTime = curTime;

                    uiParticles.push_back(particle);
                }
            }
        }
        else
        {
            isPressed = false;
        }
    }

    void DrawUiParticles(float deltaTime)
    {
        Shaders::vShader(12);
        Shaders::pShader(12);
        Blend::Blending(Blend::blendmode::on, Blend::blendop::add);

        int i = 0;
        DWORD curTime = timer::GetCounter();
        while (i < uiParticles.size())
        {
            Particle* particle = uiParticles[i];

            if (curTime - particle->startTime < particle->lifetime)
            {
                ConstBuf::global[0] = XMFLOAT4(particle->pos.x, particle->pos.y, 0.0f, 1 - (float)(curTime - particle->startTime) / (float)particle->lifetime);

                ConstBuf::Update(5, ConstBuf::global);
                ConstBuf::ConstToVertex(5);
                ConstBuf::Update(1, ConstBuf::drawerP);
                ConstBuf::ConstToPixel(1);

                context->Draw(6, 0);

                particle->pos += particle->vel * deltaTime;
                particle->vel *= 0.92f;

                i++;
            }
            else
            {
                uiParticles.erase(uiParticles.begin() + i);
            }
        }
    }


    vector<Particle*> speedParticles = vector<Particle*>{};

    int sp_rate = 50;
    float sp_minFlySpeed = maxFlySpeed;
    DWORD sp_lastEmitTime = 0;

    void CreateSpeedParticles()
    {
        DWORD curTime = timer::GetCounter();
        if (currentFlySpeed > sp_minFlySpeed && flyDirection.magnitude() > 0.5)
        {
            float speedRatio = currentFlySpeed / maxFlySpeed;
            float sp_emitDelta = 500 / (sp_rate * speedRatio);
            DWORD timeDelta = curTime - sp_lastEmitTime;
            if (timeDelta >= sp_emitDelta)
            {
                sp_lastEmitTime = curTime;

                point3d camPos = point3d(
                    XMVectorGetX(Camera::state.Eye),
                    XMVectorGetY(Camera::state.Eye),
                    XMVectorGetZ(Camera::state.Eye)
                );
                point3d forward = point3d(
                    XMVectorGetX(Camera::state.Forward),
                    XMVectorGetY(Camera::state.Forward),
                    XMVectorGetZ(Camera::state.Forward)
                );

                for (int i = 0; i < min((int)(timeDelta / sp_emitDelta), 256); i++)
                {
                    Particle* particle = new Particle;
                    particle->pos = camPos + forward * 7000 + flyDirection * 6000 + (flyUpDirection * GetRandom(-100, 100) + flyRightDirection * GetRandom(-100, 100)).normalized() * 5000;
                    particle->lifetime = GetRandom(600, 1000) / pow(speedRatio, 0.25);
                    particle->startTime = curTime;
                    particle->vel = flyDirection;

                    speedParticles.push_back(particle);
                }
            }
        }
        else if (!wasShiftPressed)
        {
            sp_lastEmitTime = curTime;
        }
        else
        {
            sp_lastEmitTime -= deltaTime;
        }
    }

    void DrawSpeedParticles()
    {
        Shaders::vShader(8);
        Shaders::pShader(8);
        Blend::Blending(Blend::blendmode::alpha, Blend::blendop::add);
        Depth::Depth(Depth::depthmode::off);

        point3d forward = point3d(
            XMVectorGetX(Camera::state.Forward),
            XMVectorGetY(Camera::state.Forward),
            XMVectorGetZ(Camera::state.Forward)
        );

        int i = 0;
        DWORD curTime = timer::GetCounter();
        while (i < speedParticles.size())
        {
            Particle* particle = speedParticles[i];

            if (curTime - particle->startTime < particle->lifetime)
            {
                if (flyDirection.magnitude() > 0)
                {
                    //particle->vel = flyDirection * currentFlySpeed / 5;
                    particle->vel = particle->vel.normalized() * currentFlySpeed / 5;
                }
                particle->pos += particle->vel * deltaTime;

                ConstBuf::global[0] = XMFLOAT4(particle->pos.x, particle->pos.y, particle->pos.z, 1.0f - (float)(curTime - particle->startTime) / (float)particle->lifetime);
                ConstBuf::global[2] = XMFLOAT4(particle->vel.x, particle->vel.y, particle->vel.z, 0);

                ConstBuf::Update(5, ConstBuf::global);
                ConstBuf::ConstToVertex(5);
                ConstBuf::Update(1, ConstBuf::drawerP);
                ConstBuf::ConstToPixel(1);
                ConstBuf::ConstToPixel(5);

                context->Draw(6, 0);

                i++;
            }
            else
            {
                speedParticles.erase(speedParticles.begin() + i);
            }
        }
    }

    std::vector<StarProjectile> Wave;

    void CreateAdvancedShockwave(point3d& center, float initialRadius) {
        const int layers = 2; // Количество слоёв волны
        const int starsPerLayer = 30;

        for (int layer = 0; layer < layers; layer++) {
            float layerRadius = initialRadius + layer * 2000.0f;
            float layerHeight = 1000.0f * layer;

            for (int i = 0; i < starsPerLayer; i++) {
                StarProjectile wave;

                float angle = (i / (float)starsPerLayer) * 2 * PI;
                float spiralAngle = angle + layer * 0.5f; // Спиральный эффект

                wave.position = {
                    center.x + cosf(spiralAngle) * layerRadius,
                    center.y + layerHeight + sinf(angle * 4.0f) * 3000.0f, // Сложная форма волны
                    center.z + sinf(spiralAngle) * layerRadius
                };

                wave.radius = GetRandom(500, 1800);
                wave.creationTime = currentTime;
                wave.lifetime = 4000.0f - layer * 50.0f; // Внешние слои живут дольше

                // Направление - вверх и наружу
                wave.direction = (wave.position - center).normalized() + point3d(0, 0.3f, 0);
                wave.direction = -wave.direction.normalized();
                wave.Speed = GetRandom(15, 25);

                Wave.push_back(wave);
            }
        }
    }

    void UpdateShockwave(float deltaTime) {
        for (auto it = Wave.begin(); it != Wave.end();) {
            auto& wave = *it;

            // Движение звёзд наружу
            wave.position += wave.direction * wave.Speed * deltaTime;

            // Увеличение радиуса для эффекта расширения
            wave.radius *= 1.01f;

            // Пульсация размера
            float pulse = 0.5f + 0.5f * sinf(currentTime * 0.01f);
            wave.radius += pulse * 2.0f;

            // Проверка времени жизни
            if (currentTime - wave.creationTime > wave.lifetime) {
                it = Wave.erase(it);
            }
            else {
                ++it;
            }
        }
    }

    void RenderShockwave() {
        Shaders::vShader(1); // Шейдер для звёзд
        Shaders::pShader(1);
        Blend::Blending(Blend::blendmode::on, Blend::blendop::add);

        for (auto& wave : Wave) {
            // Вычисляем прозрачность в зависимости от времени жизни
            float lifeProgress = (currentTime - wave.creationTime) / wave.lifetime;
            float alpha = 1.0f - lifeProgress;

            // Цвет звезды - может меняться от белого к синему/фиолетовому
            XMFLOAT4 starColor = XMFLOAT4(
                1.0f, // R
                0.7f + 0.3f * alpha, // G
                1.0f, // B
                alpha * 0.8f // Alpha
            );

            ConstBuf::global[1] = starColor;
            ConstBuf::Update(5, ConstBuf::global);
            ConstBuf::ConstToPixel(5);

            // Рисуем звезду
            wave.position.draw(wave.position, wave.radius);

            // Добавляем линии между соседними звёздами для эффекта созвездия
            Shaders::vShader(4);
            Shaders::pShader(4);

            // Можно добавить соединения между близкими звёздами
            for (auto& otherWave : Wave) {
                float distance = (wave.position - otherWave.position).magnitude();
                if (distance < 3000.0f && distance > 500.0f) {
                    float lineAlpha = alpha * (1.0f - distance / 3000.0f);
                    ConstBuf::global[2] = XMFLOAT4(0.8f, 0.8f, 1.0f, lineAlpha * 0.3f);
                    ConstBuf::Update(5, ConstBuf::global);
                    ConstBuf::ConstToPixel(5);

                    drawLine(wave.position, otherWave.position, 100.0f);
                }
            }

            Shaders::vShader(1);
            Shaders::pShader(1);
        }

        // Восстанавливаем стандартный цвет
        ConstBuf::global[1] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
        ConstBuf::Update(5, ConstBuf::global);
    }

    std::vector<StarProjectile> boomStars;

    void CreateExplosionEffects(point3d center, float radius) {
        // Очищаем старые частицы
        boomStars.clear();

        const int particleCount = 5000; // Сохраняем количество частиц
        const float goldenAngle = PI * (3.0f - sqrt(5.0f)); // Золотой угол для равномерного распределения

        // Создаем частицы взрыва с равномерным распределением по сфере
        for (int i = 0; i < particleCount; i++) {
            StarProjectile particle;

            // Равномерное распределение точек на сфере (фибоначчиева спираль)
            float y = 1.0f - (i / float(particleCount - 1)) * 2.0f; // y от 1 до -1
            float radiusAtY = sqrtf(1.0f - y * y);

            float theta = goldenAngle * i;
            float x = cosf(theta) * radiusAtY;
            float z = sinf(theta) * radiusAtY;

            // Начальное положение - все частицы в центре
            particle.position = center;

            // Направление разлета (нормализованный вектор)
            particle.direction = point3d(x, y, z).normalized();
            particle.Speed = GetRandom(1, 50);
            // Фиксированные параметры (убираем случайность)
            particle.radius = 1000.0f; // Базовый размер без вариаций
            particle.up = point3d(0, 1, 0);
            particle.right = particle.direction.cross(particle.up).normalized();

            boomStars.push_back(particle);
        }
    }

    void UpdateExplosionEffects(float deltaTime) {
        // Обновляем частицы взрыва
        for (auto& particle : boomStars) {
            // Движение частиц от центра с постоянной скоростью
            particle.position += particle.direction * deltaTime * particle.Speed; // Фиксированная скорость

            // Уменьшение размера со временем
            particle.radius = max(particle.radius * 0.95f, 1.0f);
        }

        // Удаляем слишком маленькие частицы
        boomStars.erase(
            std::remove_if(boomStars.begin(), boomStars.end(),
                [](const StarProjectile& p) { return p.radius < 2.0f; }),
            boomStars.end()
        );
    }

    void RenderExplosionEffects() {

        Shaders::vShader(15);
        Shaders::pShader(15);
        Blend::Blending(Blend::blendmode::on, Blend::blendop::add);

        // Рисуем частицы взрыва
        for (auto& particle : boomStars) {
            // Цвет остается без изменений (как в оригинале)
            ConstBuf::global[1] = XMFLOAT4(
                0, // R
                0, // G
                0, // B
                min(1.0f, particle.radius / 1.0f) // Alpha
            );

            ConstBuf::Update(5, ConstBuf::global);
            ConstBuf::ConstToPixel(5);

            // Рисуем частицу
            particle.position.draw(particle.position, particle.radius);
        }

        // Восстанавливаем стандартный цвет

    }

    struct EnemyAccumulationStar {
        point3d startPosition;  // Начальная позиция звезды
        point3d currentPosition; // Текущая позиция
        point3d targetPosition;  // Целевая позиция в сфере
        float radius;
        float speed;
        DWORD creationTime;
        bool isActive;
        XMFLOAT4 color;
    };

    std::vector<EnemyAccumulationStar> enemyAccumulationStars;
    point3d enemyAccumulationCenter; // Центр накопления врага
    bool isEnemyAccumulating = false;
    DWORD enemyAccumulationStartTime = 0;
    const float ENEMY_ACCUMULATION_DURATION = 5000.0f; // Время накопления врага

    // Функция для создания эффекта накопления врага
    void CreateEnemyAccumulationEffect(point3d center, float sphereRadius) {
        enemyAccumulationStars.clear();
        enemyAccumulationCenter = center;
        isEnemyAccumulating = true;
        enemyAccumulationStartTime = currentTime;

        const int spiralArms = 6;    // Количество спиральных рукавов
        const int starsPerArm = 20;  // Звёзд на рукав

        for (int arm = 0; arm < spiralArms; arm++) {
            for (int i = 0; i < starsPerArm; i++) {
                EnemyAccumulationStar star;

                // Спиральное расположение начальных позиций (красные тоны для врага)
                float armAngle = (arm / (float)spiralArms) * 2 * PI;
                float spiralProgress = (i / (float)starsPerArm);
                float spiralRadius = sphereRadius * 6.0f * (1.0f - spiralProgress * 0.3f);

                star.startPosition = {
                    center.x + spiralRadius * cosf(armAngle + spiralProgress * PI * 3),
                    center.y + GetRandom(-spiralRadius * 0.3f, spiralRadius * 0.3f),
                    center.z + spiralRadius * sinf(armAngle + spiralProgress * PI * 3)
                };

                // Целевая позиция - равномерно на сфере
                float goldenRatio = (1.0f + sqrt(5.0f)) / 2.0f;
                float targetIndex = arm * starsPerArm + i;
                float targetY = 1.0f - (targetIndex / (float)(spiralArms * starsPerArm)) * 2.0f;
                float targetRadius = sqrt(1.0f - targetY * targetY);
                float targetTheta = 2 * PI * targetIndex / goldenRatio;

                star.targetPosition = {
                    center.x + sphereRadius * targetRadius * cosf(targetTheta),
                    center.y + sphereRadius * targetY,
                    center.z + sphereRadius * targetRadius * sinf(targetTheta)
                };

                star.currentPosition = star.startPosition;
                star.radius = GetRandom(200, 500);
                star.speed = 0.3f + spiralProgress * 1.2f;
                star.creationTime = currentTime;
                star.isActive = true;

                // Красные тона для врага
                float hue = (arm / (float)spiralArms);
                star.color = XMFLOAT4(
                    1.0f,                    // R
                    0.2f + hue * 0.3f,       // G  
                    0.1f + hue * 0.2f,       // B
                    1.0f                     // A
                );

                enemyAccumulationStars.push_back(star);
            }
        }
    }

    // Обновление эффекта накопления врага
    void UpdateEnemyAccumulationEffect(float deltaTime) {
        if (!isEnemyAccumulating) return;

        float progress = (currentTime - enemyAccumulationStartTime) / ENEMY_ACCUMULATION_DURATION;
        progress = min(progress, 1.0f);

        // Нелинейная интерполяция для более динамичного движения
        float easeProgress = 1.0f - pow(1.0f - progress, 2.5f);

        for (auto& star : enemyAccumulationStars) {
            if (!star.isActive) continue;

            // Движение к целевой позиции
            star.currentPosition = star.startPosition.lerp(star.targetPosition, easeProgress);

            // Вращение вокруг центра
            float rotationAngle = progress * PI * 3.0f;
            point3d toCenter = enemyAccumulationCenter - star.currentPosition;

            // Исправляем вращение
            point3d rotated = toCenter;
            rotated.rotateY(rotated,rotationAngle * star.speed);
            star.currentPosition = enemyAccumulationCenter - rotated;

            // Пульсация размера
            float pulse = 0.7f + 0.5f * sinf(currentTime * 0.015f + star.speed * 8.0f);
            float currentRadius = star.radius * pulse;

            // Увеличение размера по мере накопления
            currentRadius *= (1.0f + progress * 2.0f);

            // Деактивация звёзд при достижении цели
            float distanceToTarget = (star.currentPosition - star.targetPosition).magnitude();
            if (progress >= 0.9f && distanceToTarget < 150.0f) {
                star.isActive = false;
            }
        }

        // Завершение эффекта - запуск взрыва
        if (progress >= 1.0f) {
            isEnemyAccumulating = false;
            // Запускаем взрыв врага
            CreateExplosionEffects(enemyAccumulationCenter, 3000.0f);

            // Наносим урон игроку
            Constellation& player = *starSet[player_sign];
            for (int i = 0; i < player.starsHealth.size(); i++) {
                if (player.starsHealth[i] > 0) {
                    player.starsHealth[i] -= 0.3f; // Урон от взрыва врага
                }
            }

           // ProcessSound("..\\dx11minimal\\Resourses\\Sounds\\Damage.wav");
        }
    }

    // Отрисовка эффекта накопления врага
    void RenderEnemyAccumulationEffect() {
        if (!isEnemyAccumulating) return;

        Shaders::vShader(15);
        Shaders::pShader(1);
        Blend::Blending(Blend::blendmode::on, Blend::blendop::add);

        float progress = (currentTime - enemyAccumulationStartTime) / ENEMY_ACCUMULATION_DURATION;

        for (auto& star : enemyAccumulationStars) {
            if (!star.isActive) continue;

            // Прозрачность зависит от прогресса
            float alpha = 1.0f;
            if (progress > 0.8f) {
                alpha = 1.0f - (progress - 0.8f) / 0.2f;
            }

            XMFLOAT4 finalColor = star.color;
            finalColor.w = alpha * 0.9f;

            ConstBuf::global[1] = finalColor;
            ConstBuf::Update(5, ConstBuf::global);
            ConstBuf::ConstToPixel(5);

            // Пульсация размера для отрисовки
            float pulse = 0.7f + 0.5f * sinf(currentTime * 0.015f + star.speed * 8.0f);
            float renderRadius = star.radius * pulse * (1.0f + progress * 2.0f);

            // Рисуем звезду
            star.currentPosition.draw(star.currentPosition, renderRadius);

            // Линии к центру для создания эффекта энергии
            if (GetRandom(0, 100) < 40) {
                Shaders::vShader(4);
                Shaders::pShader(4);

                float lineAlpha = alpha * 0.4f * (0.5f + 0.5f * sinf(currentTime * 0.02f));
                ConstBuf::global[2] = XMFLOAT4(1.0f, 0.3f, 0.1f, lineAlpha);
                ConstBuf::Update(5, ConstBuf::global);
                ConstBuf::ConstToPixel(5);

                drawLine(star.currentPosition, enemyAccumulationCenter, 80.0f);

                Shaders::vShader(15);
                Shaders::pShader(15);
            }
        }

        // Рисуем ядро накопления
        if (progress > 0.3f) {
            float coreAlpha = progress * 0.8f;
            float corePulse = 1.0f + 0.3f * sinf(currentTime * 0.025f);
            float coreRadius = 800.0f * progress * corePulse;

            XMFLOAT4 coreColor = XMFLOAT4(1.0f, 0.1f, 0.05f, coreAlpha);
            ConstBuf::global[1] = coreColor;
            ConstBuf::Update(5, ConstBuf::global);
            ConstBuf::ConstToPixel(5);

            enemyAccumulationCenter.draw(enemyAccumulationCenter, coreRadius);
        }

        // Восстанавливаем стандартный цвет
        ConstBuf::global[1] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
        ConstBuf::Update(5, ConstBuf::global);
    }
    void DrawDashDirection(point3d& enemyPos, const point3d& direction) {
        // Сохраняем текущие настройки шейдера
        Shaders::vShader(4);
        Shaders::pShader(4);

        // Устанавливаем красный цвет для линии
        ConstBuf::global[2] = XMFLOAT4(1.0f, 0.0f, 0.0f, 0.8f); // Красный с прозрачностью
        ConstBuf::Update(5, ConstBuf::global);
        ConstBuf::ConstToPixel(5);

        // Вычисляем конечную точку линии (в направлении рывка)
        float dashLength = 5000.0f; // Длина линии предсказания
        point3d dashEnd = enemyPos + direction * dashLength;

        // Рисуем линию
        drawer::drawLine(enemyPos, dashEnd, 50.0f); // Толстая красная линия

        // Добавляем pulsating эффект на конец линии
        float pulse = 0.5f + 0.5f * sinf(currentTime * 0.01f);
        point3d pulsePoint = enemyPos + direction * (dashLength + pulse * 500.0f);

        // Рисуем pulsating точку на конце
        Shaders::vShader(1);
        Shaders::pShader(1);
        ConstBuf::global[1] = XMFLOAT4(1.0f, 0.0f, 0.0f, pulse * 0.8f);
        ConstBuf::Update(5, ConstBuf::global);
        ConstBuf::ConstToPixel(5);

        pulsePoint.draw(pulsePoint, 100.0f * pulse);

        // Восстанавливаем стандартный цвет
        ConstBuf::global[2] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
        ConstBuf::Update(5, ConstBuf::global);
    }
    void CreateDashPreparationEffect(point3d& enemyPos, const point3d& direction) {
        // Создаем частицы подготовки вокруг врага
        const int particleCount = 20;

        for (int i = 0; i < particleCount; i++) {
            StarProjectile particle;

            // Распределяем частицы в направлении рывка
            float angle = (i / (float)particleCount) * 2 * PI;
            float distance = 1000.0f + (i % 5) * 500.0f;

            // Создаем небольшое отклонение от основного направления
            point3d right = direction.cross(point3d(0, 1, 0)).normalized();
            point3d up = right.cross(direction).normalized();

            point3d offset = right * cosf(angle) * 300.0f + up * sinf(angle) * 300.0f;
            particle.position = enemyPos + direction * distance + offset;
            particle.direction = direction;
            particle.radius = 200.0f;
            particle.creationTime = currentTime;
            particle.lifetime = Enemy::enemyData.MAX_DASH_PREPARATION;

            
        }

    }

    void InputHook(float deltaTime, point3d _hero, point3d _enemy) {

        Shaders::vShader(4);
        Shaders::pShader(4);

        static bool isHooked = false;
        static float currentSpeed = 0.0f;
        const float maxSpeed = 30.0f;
        const float acceleration = 2000.0f;
        const float minDistance = 500.0f;

        
        XMFLOAT3 heroFloat3(_hero.x, _hero.y, _hero.z);
        XMFLOAT3 enemyFloat3(_enemy.x, _enemy.y, _enemy.z);

        XMVECTOR heroPos = XMLoadFloat3(&heroFloat3);
        XMVECTOR enemyPos = XMLoadFloat3(&enemyFloat3);

        XMVECTOR Dir = enemyPos - heroPos;
        float distance = XMVectorGetX(XMVector3Length(Dir));

       
        if (GetAsyncKeyState('F') & 0x8000 && distance < 30000.0f && !isHooked) {
            isHooked = true;
            currentSpeed = maxSpeed * 0.5f;
        }

        

        if (isHooked) {
            drawLine(_hero, _enemy, 10.f);
            if (currentSpeed < maxSpeed) {
                currentSpeed += acceleration * deltaTime;
                currentSpeed = min(currentSpeed, maxSpeed);
            }

           
            if (distance <= minDistance) {
                isHooked = false;
                currentSpeed = 0.0f;
            }
            else {
               
                XMVECTOR FlyDir = XMVector3Normalize(Dir);
                Hero::state.position += FlyDir * currentSpeed * deltaTime;
            }
        }

       
        Hero::state.constellationOffset = XMMatrixRotationQuaternion(Hero::state.currentRotation) *
            XMMatrixTranslationFromVector(Hero::state.position);

        Hero::state.worldMatrix = XMMatrixRotationQuaternion(Hero::state.currentRotation) *
            XMMatrixTranslationFromVector(Hero::state.position);
    }

    void ShieldBlock() {
        static DWORD lastShieldEnergyDrain = 0;
        static DWORD lastEnemyAttackTime = 0;
        const DWORD shieldDrainInterval = 100;
        const DWORD reflectionCooldown = 1000;

        if (current_weapon == weapon_name::Shield) {
            // Потребление энергии при активном щите
            if (currentTime - lastShieldEnergyDrain > shieldDrainInterval) {
                lastShieldEnergyDrain = currentTime;

                float maintenanceCost = energyCost.shield * .2f;
                if (energy >= maintenanceCost) {
                    energy -= (LONG)maintenanceCost;
                }
                else {
                    current_weapon = weapon_name::Fists;
                    return;
                }
            }

            // Проверка столкновений с атаками врага для отражения
            if (currentTime - lastEnemyAttackTime > reflectionCooldown) {
                Constellation& enemy = *starSet[currentEnemyID];
                point3d shieldPos = point3d(
                    XMVectorGetX(Hero::state.position),
                    XMVectorGetY(Hero::state.position),
                    XMVectorGetZ(Hero::state.position)
                );

                // Проверяем все атаки врага
                for (auto& attack : attackStars) {
                    if (attack.weapon != weapon_name::Fists) continue;

                    float distance = (attack.position - shieldPos).magnitude();

                    // Если атака близко к щиту - отражаем её
                    if (distance < 2000.0f) {
                        // Вычисляем направление отражения
                        point3d toAttack = (attack.position - shieldPos).normalized();
                        point3d reflectionDir = toAttack * -1.0f; // Отражаем обратно

                        // Создаем эффект отражения
                        CreateShieldReflectionEffect(shieldPos, reflectionDir);

                        // Меняем направление атаки врага
                        attack.direction = reflectionDir;
                        attack.radius *= 1.2f; // Увеличиваем урон отраженной атаки

                        lastEnemyAttackTime = currentTime;
                        break;
                    }
                }
            }

            // Визуальный эффект активного щита
            if ((currentTime / 200) % 2 == 0) { // Мигание каждые 200ms
                point3d shieldPos = point3d(
                    XMVectorGetX(Hero::state.position),
                    XMVectorGetY(Hero::state.position),
                    XMVectorGetZ(Hero::state.position)
                );

                // Рисуем защитное поле вокруг щита
                Shaders::vShader(1);
                Shaders::pShader(1);
                Blend::Blending(Blend::blendmode::on, Blend::blendop::add);

                XMFLOAT4 shieldColor = XMFLOAT4(0.1f, 0.3f, 0.8f, 0.3f);
                ConstBuf::global[1] = shieldColor;
                ConstBuf::Update(5, ConstBuf::global);
                ConstBuf::ConstToPixel(5);

                shieldPos.draw(shieldPos, 1500.0f);

                // Восстанавливаем цвет
                ConstBuf::global[1] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
                ConstBuf::Update(5, ConstBuf::global);
            }
        }
    }
    

    struct DamageParticle {
        point3d position;
        point3d velocity;
        float radius;
        DWORD creationTime;
        float lifetime;
        XMFLOAT4 color;
        bool isActive;
    };

    std::vector<DamageParticle> heroDamageParticles;
    bool shouldCreateDamageEffect = false;
    point3d lastDamageDirection;
    point3d lastDamagePosition;

    void CreateConstellationsDamageEffect(point3d heroPos, point3d damageDirection, XMFLOAT4 ColEff) {
        const int particleCount = 100; // Количество вылетающих звезд

        for (int i = 0; i < particleCount; i++) {
            DamageParticle particle;

            // Начальная позиция - вокруг героя
            float angle = (i / (float)particleCount) * 2 * PI;
            float distance = 100.0f + GetRandom(0, 300);

            point3d right = damageDirection.cross(point3d(0, 1, 0)).normalized();
            point3d up = right.cross(damageDirection).normalized();

            point3d offset = right * cosf(angle) * distance + up * sinf(angle) * distance;
            particle.position = heroPos + offset;

            // Направление вылета - от центра с небольшим разбросом
            point3d baseDirection = (particle.position - heroPos).normalized();
            float spread = 0.3f;
            point3d randomSpread = point3d(
                GetRandom(-100, 100) * 0.01f * spread,
                GetRandom(-100, 100) * 0.01f * spread,
                GetRandom(-100, 100) * 0.01f * spread
            );

            particle.velocity = (baseDirection + randomSpread).normalized() * GetRandom(5, 15);
            particle.radius = GetRandom(50, 200);
            particle.creationTime = currentTime;
            particle.lifetime = GetRandom(1000, 2000); // 1-2 секунды жизни

            // Цвета от красного к оранжевому
            
            particle.color = ColEff;

            particle.isActive = true;
            heroDamageParticles.push_back(particle);
        }
    }


    void UpdateHeroDamageParticles(float deltaTime) {
        for (auto it = heroDamageParticles.begin(); it != heroDamageParticles.end();) {
            DamageParticle& particle = *it;

            if (!particle.isActive || (currentTime - particle.creationTime) > particle.lifetime) {
                it = heroDamageParticles.erase(it);
                continue;
            }

            // Движение частицы
            particle.position += particle.velocity * deltaTime;

            // Замедление со временем
            particle.velocity *= 0.98f;

            // Уменьшение размера
            float lifeProgress = (currentTime - particle.creationTime) / particle.lifetime;
            particle.radius *= 0.99f;

            ++it;
        }
    }

    void RenderHeroDamageParticles() {
        if (heroDamageParticles.empty()) return;

        Shaders::vShader(1);
        Shaders::pShader(1);
        Blend::Blending(Blend::blendmode::on, Blend::blendop::add);

        for (auto& particle : heroDamageParticles) {
            if (!particle.isActive) continue;

            float lifeProgress = (currentTime - particle.creationTime) / particle.lifetime;
            float alpha = 1.0f - lifeProgress;

            // Пульсация размера
            float pulse = 0.7f + 0.3f * sinf(currentTime * 0.01f);
            float renderRadius = particle.radius * pulse * alpha;

            // Устанавливаем цвет частицы
            XMFLOAT4 finalColor = particle.color;
            finalColor.w = alpha * 0.8f;

            ConstBuf::global[1] = finalColor;
            ConstBuf::Update(5, ConstBuf::global);
            ConstBuf::ConstToPixel(5);

            // Рисуем звезду
            particle.position.draw(particle.position, renderRadius);

            // Добавляем след для некоторых частиц
            if (GetRandom(0, 100) < 30) {
                Shaders::vShader(4);
                Shaders::pShader(4);

                point3d trailEnd = particle.position - particle.velocity.normalized() * 200.0f;
                float trailAlpha = alpha * 0.3f;

                ConstBuf::global[2] = XMFLOAT4(
                    particle.color.x,
                    particle.color.y,
                    particle.color.z,
                    trailAlpha
                );
                ConstBuf::Update(5, ConstBuf::global);
                ConstBuf::ConstToPixel(5);

                drawLine(particle.position, trailEnd, 100.0f);

                Shaders::vShader(1);
                Shaders::pShader(1);
            }
        }

        // Восстанавливаем стандартный цвет
        ConstBuf::global[1] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
        ConstBuf::Update(5, ConstBuf::global);
    }

    std::vector<StarProjectile> enemyPathStars;

    // Функция для создания дорожки из звезд
    void CreateEnemyPathEffect(point3d startPos, point3d targetPos) {
        enemyPathStars.clear();

        // Вычисляем направление и расстояние
        point3d direction = (targetPos - startPos).normalized();
        float distance = (targetPos - startPos).magnitude();

        // Создаем звезды вдоль пути
        int starCount = 20; // Количество звезд в дорожке
        float step = distance / starCount;

        for (int i = 0; i < starCount; i++) {
            StarProjectile pathStar;

            // Позиция вдоль пути
            pathStar.position = startPos + direction * (step * i);
            pathStar.radius = 300.0f + GetRandom(-50, 50); // Разный размер
            pathStar.creationTime = currentTime;
            pathStar.lifetime = 2000.0f; // 2 секунды жизни

            // Случайное смещение для естественности
            point3d randomOffset = point3d(
                GetRandom(-500, 500),
                GetRandom(-100, 100),
                GetRandom(-200, 200)
            );
            pathStar.position += randomOffset;

            enemyPathStars.push_back(pathStar);
        }
    }

    // Обновление дорожки
    void UpdateEnemyPathEffect(float deltaTime) {
        for (auto it = enemyPathStars.begin(); it != enemyPathStars.end();) {
            StarProjectile& star = *it;

            // Уменьшаем размер со временем
            star.radius *= 0.99f;

            // Удаляем старые звезды
            if (currentTime - star.creationTime > star.lifetime || star.radius < 10.0f) {
                it = enemyPathStars.erase(it);
            }
            else {
                ++it;
            }
        }
    }

    // Отрисовка дорожки
    void RenderEnemyPathEffect() {
        if (enemyPathStars.empty()) return;

        Shaders::vShader(15);
        Shaders::pShader(15);
        Blend::Blending(Blend::blendmode::on, Blend::blendop::add);

        // Устанавливаем красный цвет для ВСЕХ звезд дорожки
        ConstBuf::global[1] = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
        ConstBuf::Update(5, ConstBuf::global);
        ConstBuf::ConstToPixel(5);

        for (auto& star : enemyPathStars) {
            float lifeProgress = (currentTime - star.creationTime) / star.lifetime;
            float alpha = 1.0f - lifeProgress;

            // Передаем прозрачность через размер
            float renderRadius = star.radius * alpha;
            star.position.draw(star.position, renderRadius);
        }

        // Восстанавливаем
        ConstBuf::global[1] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
        ConstBuf::Update(5, ConstBuf::global);
    }

    struct MovementParticle {
        point3d position;
        point3d velocity;
        float size;
        DWORD creationTime;
        DWORD lifetime;
        XMFLOAT4 color;
        float initialSize;
    };

    std::vector<MovementParticle> movementParticles;

    // Настройки партиклов движения
    struct MovementParticleSettings {
        float emitRate = 30.0f;        // Частота генерации (частиц/секунду)
        float minSpeedThreshold = 5.0f; // Минимальная скорость для генерации
        float maxSpeedForEmission = 25.0f; // Максимальная скорость для расчета интенсивности
        float minSize = 90.0f;
        float maxSize = 270.0f;
        float minLifetime = 800.0f;
        float maxLifetime = 1500.0f;
        float spreadAngle = 45.0f;     // Угол разброса в градусах
        float distanceFromPlayer = 300.0f; // Дистанция от игрока
    } movementSettings;

    DWORD lastMovementEmitTime = 0;
    float movementEmitAccumulator = 0.0f;

    void CreateMovementParticles() {
        if (currentFlySpeed < 3.0f) return;

        DWORD currentTime = timer::GetCounter();
        float deltaTime = currentTime - lastMovementEmitTime;
        lastMovementEmitTime = currentTime;

        // Простая эмиссия без сложных расчетов
        movementEmitAccumulator += movementSettings.emitRate * (deltaTime / 1000.0f);

        while (movementEmitAccumulator >= 1.0f) {
            movementEmitAccumulator -= 1.0f;

            MovementParticle particle;

            point3d heroPos = point3d(
                XMVectorGetX(Hero::state.position),
                XMVectorGetY(Hero::state.position),
                XMVectorGetZ(Hero::state.position)
            );

            // Простая позиция позади игрока
            particle.position = heroPos + flyDirection.normalized() * 5000.0f +
                point3d(GetRandom(-2000, 2000), GetRandom(-2000, 2000), GetRandom(-2000, 2000));

            particle.velocity = flyDirection * currentFlySpeed * 300.0f;
            particle.initialSize = GetRandom(50, 150);
            particle.size = particle.initialSize;
            particle.lifetime = GetRandom(800, 1200);
            particle.creationTime = currentTime;
            particle.color = XMFLOAT4(0.4f, 0.6f, 1.0f, 0.7f);

            movementParticles.push_back(particle);
        }
    }

    void UpdateMovementParticles(float deltaTime) {
        for (auto it = movementParticles.begin(); it != movementParticles.end();) {
            MovementParticle& particle = *it;

            float elapsed = currentTime - particle.creationTime;
            float lifeProgress = elapsed / particle.lifetime;

            if (elapsed > particle.lifetime) {
                it = movementParticles.erase(it);
                continue;
            }

            // Обновление позиции
            particle.position += particle.velocity * (deltaTime / 1000.0f);

            // Затухание размера и прозрачности
            particle.size = particle.initialSize * (1.0f - lifeProgress);
            particle.color.w = 0.8f * (1.0f - lifeProgress);

            // Медленное затухание скорости
            particle.velocity *= 0.995f;

            ++it;
        }
    }

    void DrawMovementParticles() {
        if (movementParticles.empty()) return;

        Shaders::vShader(1); // Используем шейдер для звезд
        Shaders::pShader(1);
        Blend::Blending(Blend::blendmode::on, Blend::blendop::add);
        Depth::Depth(Depth::depthmode::on);

        for (auto& particle : movementParticles) {
            // Устанавливаем цвет и размер частицы
            ConstBuf::global[1] = particle.color;
            ConstBuf::Update(5, ConstBuf::global);
            ConstBuf::ConstToPixel(5);

            // Рисуем частицу как звезду
            particle.position.draw(particle.position, particle.size);

            // Добавляем свечение для некоторых частиц
            if (GetRandom(0, 100) < 30) { // 30% частиц имеют свечение
                float glowSize = particle.size * 1.5f;
                XMFLOAT4 glowColor = particle.color;
                glowColor.w *= 0.3f; // Более прозрачное свечение

                ConstBuf::global[1] = glowColor;
                ConstBuf::Update(5, ConstBuf::global);

                particle.position.draw(particle.position, glowSize);
            }
        }

        // Восстанавливаем стандартный цвет
        ConstBuf::global[1] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
        ConstBuf::Update(5, ConstBuf::global);
    }

    void TeleportEnemy(point3d newPosition) {
        // Обновляем матрицу трансформации
        Enemy::enemyData.enemyConstellationOffset = XMMatrixTranslation(
            newPosition.x, newPosition.y, newPosition.z
        );

        // Сбрасываем состояние ИИ
        enemyAI.splineInitialized = false;
        enemyAI.splineProgress = 0.0f;

        // Можно также сбросить состояние атаки
        Enemy::enemyData.currentState = Enemy::AIState::PATROL;
    }

    void ApplyChargingShake(Constellation& constellation, float chargeRatio) {
        if (chargeRatio > 0.3f) {
            float intensity = 0.1f + (chargeRatio - 0.3f) * 0.3f;
            float shakeX = GetRandom(-intensity * 100, intensity * 100);
            float shakeY = GetRandom(-intensity * 100, intensity * 100);
            float shakeZ = GetRandom(-intensity * 100, intensity * 100);

            XMMATRIX shakeMatrix = XMMatrixTranslation(shakeX, shakeY, shakeZ);
            constellation.Transform = constellation.Transform * shakeMatrix;
        }
    }

    Constellation& enemy = *starSet[currentEnemyID];
    Constellation& player = *starSet[player_sign];

    float playerHP = getConstellationHP(player);
    float EnemyHP = getConstellationHP(enemy);
    void drawWorld(float deltaTime)
    {
        textStyle.color = RGB(0, 191, 255);

        XMVECTOR heroPosition = Hero::state.constellationOffset.r[3];
        XMVECTOR enemyPositions = Enemy::enemyData.enemyConstellationOffset.r[3];
        
        float enemyTotalHP = getConstellationHP(enemy);
        // ПРИМЕНЯЕМ РАЗМЕР ТОЛЬКО К ИГРОКУ, НЕ К ОРУЖИЯМ
        for (int i = 0; i < player.starsCords.size(); i++) {
            player.SetStarRadius(i, currentStarRadius);
        }

        // Оружия должны иметь свои собственные размеры
        // УБЕРИТЕ эти строки или задайте фиксированные размеры для оружий:
        /*
        for (int i = 0; i < Sword.starsCords.size(); i++) {
            Sword.SetStarRadius(i, currentStarRadius); // УБРАТЬ
        }
        for (int i = 0; i < Shield.starsCords.size(); i++) {
            Shield.SetStarRadius(i, currentStarRadius); // УБРАТЬ
        }
        for (int i = 0; i < Bow.starsCords.size(); i++) {
            Bow.SetStarRadius(i, currentStarRadius); // УБРАТЬ
        }
        */

        // Вместо этого задайте фиксированные размеры для оружий:
        const float WEAPON_STAR_RADIUS = 1000.f; // или любой другой подходящий размер

        for (int i = 0; i < Sword.starsCords.size(); i++) {
            Sword.SetStarRadius(i, WEAPON_STAR_RADIUS);
        }
        for (int i = 0; i < Shield.starsCords.size(); i++) {
            Shield.SetStarRadius(i, WEAPON_STAR_RADIUS);
        }
        for (int i = 0; i < Bow.starsCords.size(); i++) {
            Bow.SetStarRadius(i, WEAPON_STAR_RADIUS);
        }
        ShieldBlock();
       //drawString(C.c_str(), 1500, 500, 1.f, true);
       //string DistCam = to_string(Camera::state.distanceOffset);
       //drawString(DistCam.c_str(), 2000, 1000, 1.f, true);
        //d2dRenderTarget->BeginDraw();
        CreateSpeedParticles();
        DrawSpeedParticles();

        UpdateReflectionEffects(deltaTime);
        RenderReflectionEffects();

        UpdateTransition(deltaTime);
        //d2dRenderTarget->BeginDraw();
        switch (gameState)
        {
        case gameState_::MainMenu: {

            StartMenu();
            Constellation& l = *starSet[18];
            TeleportEnemy(point3d{ 3700.f,-10000.f,-120000.f });
            Enemy::enemyData.currentRotation = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), PI);
            
            l.Transform = CreateEnemyToWorldMatrix(l,7000.f);
            Enemy::enemyData.currentRotation = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), 0);

            drawConstellation(l, false, 200.f, 30);
            mciSendString(TEXT("play ..\\dx11minimal\\Resourses\\Sounds\\GG_C.mp3"), NULL, 0, NULL);
            break;
        }

        case gameState_::Settings:
            SettingsState();

            break;

        case gameState_::Authors:
            AuthorsState();
            break;

        case gameState_::MonthSelection:
            menuMonthprocessing();
            break;

        case gameState_::DaySelection:
            menuMonthprocessing();
            menuDayprocessing();
            break;



        case gameState_::confirmSign:
            Depth::Depth(Depth::depthmode::off);
            Blend::Blending(Blend::blendmode::on, Blend::blendop::add);
            drawPlayerСonstellationToMenu();
            menuMonthprocessing();
            menuDayprocessing();
            menuConfirmationButton();
            break;

        case gameState_::selectEnemy:
        {
            for (int i = 0; i < enemy.starsCords.size(); i++) {

                enemy.SetStarRadius(i, 1000.f);
            }
            //DrawTEST();
            drawStaminaBar(energy);

            
            CreateMovementParticles();
            UpdateMovementParticles(deltaTime);
            DrawMovementParticles();

            Camera::state.mouse = true;
            Depth::Depth(Depth::depthmode::off);
            Blend::Blending(Blend::blendmode::on, Blend::blendop::add);

            ConstBuf::Update(0, ConstBuf::drawerV);
            ConstBuf::ConstToVertex(0);
            ConstBuf::Update(1, ConstBuf::drawerP);
            ConstBuf::ConstToPixel(1);

            DrawRenderObject(backgroundStars);
            DrawRenderObject(spaceStars);
            DrawRenderObject(ariesNebula);
            //DrawRenderObject(blackHole);

            Constellation& playerConst = *starSet[player_sign];
            playerConst.Transform = CreateHeroToWorldMatrix(playerConst);


            Constellation& q = *starSet[0];
            TeleportEnemy(point3d{ 0.f,-10000.f,0.f });
            q.Transform = CreateEnemyToWorldMatrix(q);
           
            drawConstellation(q,false, 200.f,30);
          

            if (!playerConst.morphing)
                HandleMouseClick(heroPosition, playerConst);
            UpdateAttack(deltaTime);
            DrawSwordAttack();

            drawConstellation(playerConst,false,10.f , 7.f );

            std::string curentSignstring = zodiacSignToString(player_sign);
            TextOutA(window.context, window.width * 5 / 6, window.height - window.height / 20., curentSignstring.c_str(), curentSignstring.size());

            //drawString("Find Constallations and click on it", window.width / 2, (200. / 1440) * window.height, 1, true);
            //drawString("Features:\nMouse wheel to zoom in and out", (1700. / 2560) * window.width, (1200. / 1440) * window.height, .7f, false);

            playerConst.RenderMorph(deltaTime);

            drawString("Press < T > to enter tutorial", (100. / 2560)* window.width, (100. / 1440)* window.height, 1.f, false);
            if (GetAsyncKeyState('T')) {
                gameState = gameState_::Exploring;
            }
            drawHPBar(playerHP);
            isBattleActive = false;
            break;

            // drawString("X", 0, 0, 1, false);
            // drawString(std::to_string(mouse.pos.y).c_str(), 0, 0, 1, false);

        }
        case gameState_::DialogStruct: {


            Constellation& c = *starSet[16];
            TeleportEnemy(point3d{ 6000.f,-5000.f,-115000.f });
            c.Transform = CreateEnemyToWorldMatrix(c);

            Constellation& h = *starSet[17];
            Enemy::enemyData.currentRotation = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), PI);
            TeleportEnemy(point3d{ -6000.f,-5000.f,-115000.f });
            h.Transform = CreateEnemyToWorldMatrix(h);
            Enemy::enemyData.currentRotation = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), 0);

            drawConstellation(c, false, 200.f, 80);
            drawConstellation(h, false, 200.f, 40);
            initContentData();
            renderContent();
            handleInput();

            break;
        }

        case gameState_::Fight:
        {

            for (int i = 0; i < enemy.starsCords.size();i++) {

                enemy.SetStarRadius(i, 1000.f);
            }
            CreateMovementParticles();
            UpdateMovementParticles(deltaTime);
            DrawMovementParticles();
            //CreateParticledText("ANOTHER TEST", (1700. / 2560) * window.width, (600. / 1440) * window.height + 200.f, .7f, false);
            std::string P = std::to_string(chargeRatio);
            //drawString(P.c_str(), 600, 750, 1.f, true);
            //drawString("Power", 450, 750, 1.f, true);
            
            drawStaminaBar(energy);


            if (isBattleActive == false) {
                isBattleActive = true;
                battleStartTime = currentTime;
            }

            if (starSet.empty() || currentEnemyID < 0 || currentEnemyID >= starSet.size()) {
                gameState = gameState_::selectEnemy;
                break;
            }

            Constellation& playerConst = *starSet[player_sign];
            playerConst.RenderMorph(deltaTime);

            if (t) {
                t = false;
                Camera::state.camDist = 1000.0f;  // Используйте большое значение
                Camera::state.distanceOffset = 1000.0f;
            }
            Camera::state.mouse = true;
            Depth::Depth(Depth::depthmode::off);
            SelectWeapon(&playerConst);
            SelectElement();

            DrawCombatStats();

            if (attackCooldown == true) {

                AttackVector();
            }

            //StartBattle();
            //enemyFight();

            modelTransform = &placeToWorld;
            modelProject = &fightProject;
            uiFunc = &starIntersectUI;


            ConstBuf::Update(0, ConstBuf::drawerV);
            ConstBuf::ConstToVertex(0);
            ConstBuf::Update(1, ConstBuf::drawerP);
            ConstBuf::ConstToPixel(1);

            DrawRenderObject(backgroundStars);
            DrawRenderObject(spaceStars);
            DrawRenderObject(ariesNebula);
            //DrawRenderObject(blackHole);


            modelTransform = &placeConstToWorld;

            srand(currentTime);

            //DrawHpEnemyBar();
            modelTransform = &placeConstToWorld;//Враг

            /*if (isShakingHero) {

                float beamTime = 4. * (currentTime - shakeStartTimeHero) / shakeDurationHero;
                if (beamTime > 1.) beamTime = 1;
                for (int i = 0;i < starSet[currentEnemyID]->starsCords.size();i++)
                {
                    auto p1 = starSet[currentEnemyID]->starsCords[i];
                    point3d p2 = starSet[player_sign]->starsCords[i % starSet[player_sign]->starsCords.size()];

                   placeConstToWorld(p1, *starSet[currentEnemyID]);
                   placeHeroToWorld(p2, *starSet[currentEnemyID]);

                    p2.x = lerp(p1.x, p2.x, beamTime);
                    p2.y = lerp(p1.y, p2.y, beamTime);
                    p2.z = lerp(p1.z, p2.z, beamTime);


                    drawLine(p1, p2);
                }
            }*/


            //ProjectileUpdate(deltaTime);
            //FireProjectile();
            //WeaponRender();

            

            if (currentTime > attack_cooldown + 5000)
            {

                attackCooldown = true;
            }



            if (attack_collision == true and attackCooldown == true)
            {
                attack_cooldown = currentTime;
                attackCooldown = false;
                check_attack = false;
                attackStartTime = currentTime;

            }
            Constellation& h = *starSet[currentEnemyID];
            h.Transform = CreateEnemyToWorldMatrix(h);
            Blend::Blending(Blend::blendmode::on, Blend::blendop::add);
            drawConstellation(*starSet[currentEnemyID], false, 1000.f, 100.f);

            //linksDivider = 15;
            modelTransform = &placeHeroToWorld;
            uiFunc = &heroUI;
            Blend::Blending(Blend::blendmode::on, Blend::blendop::add);


            point3d Heropos = point3d(
                XMVectorGetX(heroPosition),
                XMVectorGetY(heroPosition),
                XMVectorGetZ(heroPosition)
            );

            point3d Enemypos = point3d(
                XMVectorGetX(enemyPositions),
                XMVectorGetY(enemyPositions),
                XMVectorGetZ(enemyPositions)
            );

            lastFrameTime = currentTime;

            if (isDamageTaken)
            {
                isDamageTaken = false;
                isShaking = true;
                shakeStartTime = currentTime;
                

            }

            if (currentTime > shakeStartTime + shakeDuration)
            {
                isShaking = false;
            }

            if (enemyAI.data.isAttacking == true) {
                //playerConst.StartShaking();
                enemyAI.data.isAttacking = false;

                // СОЗДАЕМ ЭФФЕКТ УРОНА
                float colorVariantHERO = GetRandom(0, 100) * 0.01f;
                XMFLOAT4 ColorHEROEff(
                    0.1f,                    // R
                    0.3f + colorVariantHERO * 0.4f, // G (от 0.3 до 0.7)
                    1.0f,                    // B
                    1.0f                     // A
                );
                point3d damageDirection = (Heropos - Enemypos).normalized();
                CreateConstellationsDamageEffect(Heropos, damageDirection, ColorHEROEff);

                // Визуализация направления атаки (опционально)
                //DwarHeroGetDamage(Heropos, damageDirection);
            }
            bool wasFPressed = false;
            DWORD lastFToggleTime = 0;
            const DWORD F_TOGGLE_COOLDOWN = 500; // Задержка 300ms

            if (GetAsyncKeyState(VK_MBUTTON) & 0x8000)
            {
                if (!CameraTargeting::data.wasTogglePressed &&
                    (currentTime - CameraTargeting::data.lastToggleTime) > CameraTargeting::data.TOGGLE_COOLDOWN)
                {
                    if (!CameraTargeting::IsTargeting())
                    {
                        // Найти ближайшего врага
                        float closestDistance = FLT_MAX;
                        int closestEnemy = -1;

                        for (int i = 0; i < starSet.size(); i++)
                        {
                            if (i == player_sign || !starSet[i]) continue;

                            Constellation& enemy = *starSet[i];
                            XMMATRIX enemyTransform = CreateEnemyToWorldMatrix(enemy);
                            XMVECTOR enemyPos = enemyTransform.r[3];

                            float distance = XMVectorGetX(XMVector3Length(enemyPos - Hero::state.position));

                            if (distance < closestDistance && distance < 40000.0f)
                            {
                                closestDistance = distance;
                                closestEnemy = i;
                            }
                        }

                        if (closestEnemy != -1)
                        {
                            CameraTargeting::StartTargeting(closestEnemy);
                            //ProcessSound("..\\dx11minimal\\Resourses\\Sounds\\TargetLock.wav");
                        }
                    }
                    else
                    {
                        CameraTargeting::StopTargeting();
                        //ProcessSound("..\\dx11minimal\\Resourses\\Sounds\\TargetUnlock.wav");
                    }

                    CameraTargeting::data.wasTogglePressed = true;
                    CameraTargeting::data.lastToggleTime = currentTime;
                }
            }
            else
            {
                CameraTargeting::data.wasTogglePressed = false;
            }
            // ОБНОВЛЯЕМ И ОТРИСОВЫВАЕМ ЧАСТИЦЫ УРОНА
            UpdateHeroDamageParticles(deltaTime);
            RenderHeroDamageParticles();
            playerConst.UpdateShaking();

            if (enemyAI.data.isPreparingAttack) {
                CreateEnemyPathEffect(Enemypos, enemyAI.data.delayedAttackTarget);
            }
            UpdateEnemyPathEffect(deltaTime);
            RenderEnemyPathEffect();

            if (enemyAI.data.isShockwaveActive == true) {
                CreateAdvancedShockwave(Enemypos, enemyAI.data.shockwaveRadius);
                wasFPressed = false;
                CameraTargeting::StopTargeting();
            }
            UpdateShockwave(deltaTime);
            RenderShockwave();

            if (enemyAI.data.shouldStartAccumulation) {
                CreateEnemyAccumulationEffect(enemyAI.data.accumulationPos, 2000.0f);
                enemyAI.data.shouldStartAccumulation = false;
                
            }

            UpdateEnemyAccumulationEffect(deltaTime);
            RenderEnemyAccumulationEffect();

            UpdateExplosionEffects(deltaTime);
            RenderExplosionEffects();

            //Constellation& c = *starSet[player_sign];
            //c.Transform = CreateHeroToWorldMatrix(c);
            //drawСonstellation(*starSet[player_sign]);//Игрок

            if (attack_collision == true)
            {
                check_attack = true;
                attack_collision = false;
                attack_speed = false;
            }



            if (GetAsyncKeyState('P')) {
                StartTransition(gameState_::WinFight, 2000.0f);
                mciSendString(TEXT("stop ..\\dx11minimal\\Resourses\\Sounds\\Oven_NEW.mp3"), NULL, 0, NULL);
                mciSendString(TEXT("play ..\\dx11minimal\\Resourses\\Sounds\\FREEFLY.mp3"), NULL, 0, NULL);
            }

            if (currentTime > attack_time + weapon[(int)current_weapon].attackSpeed and attack_start == true)
            {
                isDamageTaken = true;
                attack_start = false;
            }

            modelTransform = &placeConstToWorld;
            //DrawStarsHP(window.context);

            //linksDivider = 15;
            modelTransform = &placeHeroToWorld;
            uiFunc = &heroUI;

            modelTransform = NULL;
            uiFunc = NULL;

            /*if (isDamageHero)
            {
                isDamageHero = false;
                isShakingHero = true;
                shakeStartTimeHero = currentTime;
            }

            if (currentTime > shakeStartTimeHero + shakeDurationHero)
            {
                isShakingHero = false;
            }*/
            Blend::Blending(Blend::blendmode::on, Blend::blendop::add);

            if (starSet.empty() || currentEnemyID < 0 || currentEnemyID >= starSet.size()) {
                gameState = gameState_::selectEnemy;
                break;
            }

            // Безопасный вызов функций атаки


            // Обновление атак
            if (!playerConst.morphing) {
                HandleMouseClick(heroPosition, playerConst);
                //Hero::UpdateAttackRotation(deltaTime);
                
            }
            Constellation& c = *starSet[player_sign];
            c.Transform = CreateHeroToWorldMatrix(c); // Базовая матрица

            // Применяем тряску поверх базовой матрицы
            if (isCharging) {
                ApplyChargingShake(c, chargeRatio);
            }
            UpdateCameraScaleAndStarRadiusReturn(deltaTime, player);
            UpdateAttack(deltaTime);

            UpdateAccumulationEffect(deltaTime);
            RenderAccumulationEffect();



            DrawSwordAttack();


            // Проверка условий победы/поражения
            if (getConstellationHP(enemy) <= 0) {
                gameState = gameState_::WinFight;
                mciSendString(TEXT("stop ..\\dx11minimal\\Resourses\\Sounds\\Oven_NEW.mp3"), NULL, 0, NULL);
                mciSendString(TEXT("play ..\\dx11minimal\\Resourses\\Sounds\\FREEFLY.mp3"), NULL, 0, NULL);
            }

            updateEnemyPosition(deltaTime, Heropos, Enemypos, playerHP);
            static DWORD deathStartTime = 0;
            static bool deathAnimationStarted = false;

            if (playerHP <= 0.f && !deathAnimationStarted) {
                deathAnimationStarted = true;
                deathStartTime = currentTime;
                ProcessSound("..\\dx11minimal\\Resourses\\Sounds\\PlayerDeath.wav");
            }

            if (deathAnimationStarted) {
                float progress = (currentTime - deathStartTime) / 2000.0f; // 2 секунды

                // Простой эффект затемнения и уменьшения
                float darken = progress;
                heroScale = max(0.1f, MIN_HERO_SCALE * (1.0f - progress));
                player.SetStarSpacing(heroScale);
                
                if (progress >= 1.0f) {
                    deathAnimationStarted = false;
                    gameState = gameState_::EndFight;
                    mciSendString(TEXT("stop ..\\dx11minimal\\Resourses\\Sounds\\Oven_NEW.mp3"), NULL, 0, NULL);
                }
            }
            InputHook(deltaTime, Heropos, Enemypos);

            //string heroHP = std::to_string(playerHP);
            //drawString(heroHP.c_str(), window.width / 2, window.height - 100, 1, true);
            //drawString("HP", (1150.f / 2560)* window.width, (1300.f /1440)* window.height, 2.f, true);
            drawHPBar(playerHP);
            drawEnemyBar(enemyTotalHP);
            
            drawConstellation(*starSet[player_sign], false , 10.f,7.f);
            drawString(enemyH.c_str(), window.width / 2, 100, 2.f, true);
            drawString("ARIES", window.width / 2, 50, 2.f, true);
            drawString("Press < T > to enter tutorial", (100. / 2560) * window.width, (100. / 1440) * window.height, 1.f, false);
            if (GetAsyncKeyState('T')) {
                gameState = gameState_::Exploring;
            }
            //std::string enemyHP= std::to_string(EnemyHP);
            //drawString(enemyHP.c_str(), window.width / 2, 100,2.f,true);
            //std::string curentSignstring = zodiacSignToString(currentEnemyID);
            //drawString(curentSignstring.c_str(), window.width / 1.1, window.height / 10., 1, true);
            //
            //curentSignstring = zodiacSignToString(player_sign);
            //drawString(curentSignstring.c_str(), window.width / 2, window.height - window.height / 7., 1, true);
            //
            //curentSignstring = "current weapon: " + weapon[(int)current_weapon].name;
            //drawString(curentSignstring.c_str(), window.width / 2, window.height - window.height / 10., 1, true);
            // 
            //std::string H = std::to_string(heroScale);
            //drawString(H.c_str(),1200,100,1.f,true);

            //drawCurrentElement();

           //drawRect(20.0f, 20.0f, 1000.0f, 800.0f); // прямоугольник шириной 200px и высотой 100px по центру экрана, на пикселях 1000 по абсциссе и 800 по ординате

           //drawString("Weapon selection:\nButton 1 - Sword \nButton 2 - Shield \nButton 3 - Bow ", (1700. / 2560) * window.width, (1100. / 1440) * window.height, .7f, false);
           //drawString("Rewind time:\nbutton - R", (500. / 2560) * window.width, (1250. / 1440) * window.height, .7f, false);
           //drawString("TUTORIAL:\nTo hit an enemy with a sword,\npress LMB and draw a line along the enemy star\nTo hit with a shield,\npress LMB and draw a line that will draw a circle that attacks stars\nTo hit with a bow,\npress LMB on the star and draw a vector in any direction from the star.", (60. / 2560) * window.width, (60. / 1440) * window.height, .7f, false);
           //
           // float cdTimeOut = 1. - (currentTime - attack_cooldown) / 5000.;
           // cdTimeOut *= 10;
           // std::string cdTimeOutText = std::to_string((int)cdTimeOut);
           // if (cdTimeOut > 0)
           // {
           //    drawString("recharge", window.width * .9, window.height * .85, 1., false);
           //    drawString(cdTimeOutText.c_str(), window.width * .9, window.height * .9, 3., false);
           // }
            if (Enemy::enemyData.isShockwaveActive) {
                point3d shockwaveCenter = point3d(Enemypos.x, Enemy::enemyData.lastOrbitPosition.y, Enemypos.z);

                // Визуализация радиуса (можно убрать после отладки)
                Shaders::vShader(4);
                Shaders::pShader(4);
                ConstBuf::global[2] = XMFLOAT4(1.0f, 0.0f, 0.0f, 0.3f); // Красный полупрозрачный
                ConstBuf::Update(5, ConstBuf::global);
                ConstBuf::ConstToPixel(5);

                // Рисуем круг ударной волны
                for (int i = 0; i < 36; i++) {
                    float angle1 = i * (2 * PI / 36);
                    float angle2 = (i + 1) * (2 * PI / 36);

                    point3d p1 = shockwaveCenter + point3d(cos(angle1), 0, sin(angle1)) * Enemy::enemyData.shockwaveRadius;
                    point3d p2 = shockwaveCenter + point3d(cos(angle2), 0, sin(angle2)) * Enemy::enemyData.shockwaveRadius;

                    drawLine(p1, p2, 100.0f);
                }
            }
            UpdateGame();

            break;
        }
        case gameState_::EndFight:
        {
            endFight();

            break;
        }
        case gameState_::WinFight:
        {
            //winFight();
            CameraTargeting::StopTargeting();
            if (AriesNebula.defeatTime >= 0)
            {
                if (ConstBuf::factors.AriesNebulaLerpFactor < 1)
                {
                    ConstBuf::factors.AriesNebulaLerpFactor = min((ConstBuf::frame.time.x - AriesNebula.defeatTime) / 150.0f, 1);
                    ConstBuf::UpdateFactors();
                    ConstBuf::ConstToVertex(6);
                    ConstBuf::ConstToPixel(6);
                }
            }
            else
            {
                AriesNebula.defeatTime = ConstBuf::frame.time.x;
            }

            if (t) {
                t = false;
                Camera::state.camDist = 10000.0f;  // Используйте большое значение
                Camera::state.distanceOffset = 10000.0f;
            }
            Camera::state.mouse = true;
            Depth::Depth(Depth::depthmode::off);

            modelTransform = &placeToWorld;
            modelProject = &fightProject;
            uiFunc = &starIntersectUI;


            ConstBuf::Update(0, ConstBuf::drawerV);
            ConstBuf::ConstToVertex(0);
            ConstBuf::Update(1, ConstBuf::drawerP);
            ConstBuf::ConstToPixel(1);

            DrawRenderObject(backgroundStars);
            DrawRenderObject(spaceStars);
            DrawRenderObject(ariesNebula);
            //DrawRenderObject(blackHole);


            modelTransform = &placeConstToWorld;

            srand(currentTime);

            //DrawHpEnemyBar();
            modelTransform = &placeConstToWorld;//Враг

            lastFrameTime = currentTime;

            if (!GetAsyncKeyState(VK_LBUTTON))
            {
                if (attack_collision == true and attackCooldown == true)
                {
                    attack_cooldown = currentTime;
                    attackCooldown = false;
                    check_attack = false;
                    attackStartTime = currentTime;

                }
                Constellation& h = *starSet[currentEnemyID];
                h.Transform = CreateEnemyToWorldMatrix(h);
                Blend::Blending(Blend::blendmode::on, Blend::blendop::add);
                //drawConstellation(*starSet[currentEnemyID]);

                //linksDivider = 15;
                modelTransform = &placeHeroToWorld;
                uiFunc = &heroUI;
                Blend::Blending(Blend::blendmode::on, Blend::blendop::add);

                //Constellation& c = *starSet[player_sign];
                //c.Transform = CreateHeroToWorldMatrix(c);
                //drawСonstellation(*starSet[player_sign]);//Игрок

                if (attack_collision == true)
                {
                    check_attack = true;
                    attack_collision = false;
                    attack_speed = false;
                }
            }
            else
            {
                uiFunc = NULL;
                Blend::Blending(Blend::blendmode::on, Blend::blendop::add);
                Constellation& c = *starSet[currentEnemyID];

                c.Transform = CreateEnemyToWorldMatrix(c);
                //drawConstellation(*starSet[currentEnemyID]);
            }

            modelTransform = &placeConstToWorld;
            //DrawStarsHP(window.context);

            //linksDivider = 15;
            modelTransform = &placeHeroToWorld;
            uiFunc = &heroUI;

            modelTransform = NULL;
            uiFunc = NULL;

            /*if (isDamageHero)
            {
                isDamageHero = false;//
                isShakingHero = true;//
                shakeStartTimeHero = currentTime;
            }//

            if (currentTime > shakeStartTimeHero + shakeDurationHero)
            {
                isShakingHero = false;
            }*/
            Blend::Blending(Blend::blendmode::on, Blend::blendop::add);

            Constellation& c = *starSet[player_sign];
            c.Transform = CreateHeroToWorldMatrix(c);
            drawConstellation(*starSet[player_sign],false, 10.f,7.f);

            std::string curentSignstring = zodiacSignToString(currentEnemyID);
            //drawString(curentSignstring.c_str(), window.width / 1.1, window.height / 10., 1, true);

            drawString("You Win!", window.width / 2., window.height / 4-100, 7.f, true);

            drawString("Aries Is Defeated!", window.width / 2., window.height / 6-100, 4.f, true);
            curentSignstring = zodiacSignToString(player_sign);
            //drawString(curentSignstring.c_str(), window.width / 2, window.height - window.height / 7., 1, true);


            //drawCurrentElement();

            //UpdateGame();

            break;
        }
        case gameState_::Exploring:
        {
            //std::string enemyHP = std::to_string(EnemyHP);
            //drawString(enemyHP.c_str(), window.width / 2 + 100, 100, 2.f, true);
            //std::string curentSignstring = zodiacSignToString(currentEnemyID);
            //drawString(curentSignstring.c_str(), window.width / 2, 200., 2.f, false);
            //drawString("You'r Enemy is ", (750. / 2560) * window.width, (259. / 1440) * window.height, 2.f, false);
            //drawString("HP Enemy ", (820. / 2560) * window.width, (130. / 1440) * window.height, 2.f, false);
            //curentSignstring = zodiacSignToString(player_sign);
            //drawString(curentSignstring.c_str(), window.width / 2 + 120, window.height - window.height / 7., 1.2f, true);
            //drawString("You Constellation is:", window.width / 2 - 150, window.height - window.height / 7., 1.f, true);

            //curentSignstring = "current weapon: " + weapon[(int)current_weapon].name;
            //drawString(curentSignstring.c_str(), window.width / 2, window.height - window.height / 10., 1, true);
            //drawString("Destroy all the stars", (1250. / 2560) * window.width, (1100. / 1440) * window.height, 2.f, true);


            //drawRect(20.0f, 20.0f, 1000.0f, 800.0f); // прямоугольник шириной 200px и высотой 100px по центру экрана, на пикселях 1000 по абсциссе и 800 по ординате


            //drawString("Rewind time:\nbutton - R", (500. / 2560) * window.width, (1250. / 1440) * window.height, .7f, false);

            drawString("<<CONTROLS>>", (1280. / 2560)* window.width, (300. / 1440)* window.height, 2.f, true);
            drawString("<WASD> to move", (1280. / 2560) * window.width, (500. / 1440) * window.height, 1.f, true);
            drawString("To move Up press <Space> and To move Down press <Ctrl> ", (1280. / 2560) * window.width, (600. / 1440) * window.height, 1.f, true);
            drawString("To rotate You'r Constellation press <Q> or <E>", (1280. / 2560) * window.width, (700. / 1440) * window.height, 1.f, true);
            drawString("To rotate Camera move the <Mouse>", (1280. / 2560) * window.width, (800. / 1440) * window.height, 1.f, true);
            drawString("Hold <Shift> to make a Dash", (1280. / 2560) * window.width, (900. / 1440) * window.height, 1.f, true);

            drawString("Press < ESC > to exit tutorial", (100. / 2560) * window.width, (50. / 1440) * window.height, 1.f, false);

            if (GetAsyncKeyState(VK_ESCAPE)) {
                gameState = gameState_::Fight;
            }
            drawString("Press < Right arrow > to flip the page forward", (1280. / 2560) * window.width, (1100. / 1440) * window.height, 1.f, true);
            drawString("Press < Left arrow > to flip the page back", (1280. / 2560)* window.width, (1200. / 1440)* window.height, 1.f, true);

            drawString("Next Page", (2300. / 2560)* window.width, (1350. / 1440)* window.height, 1.f, true);
            if (GetAsyncKeyState(VK_RIGHT)) {
                gameState = gameState_::Exploring1;
            }
            break;
        }
        case gameState_::Exploring1:
            drawString("Press < ESC > to exit tutorial", (100. / 2560) * window.width, (50. / 1440) * window.height, 1.f, false);
            if (GetAsyncKeyState(VK_ESCAPE)) {
                gameState = gameState_::Fight;
            }
            drawString("Previous Page", (300. / 2560)* window.width, (1350. / 1440)* window.height, 1.f, true);

            
            if (GetAsyncKeyState(VK_LEFT)) {
                gameState = gameState_::Exploring;
            }
            drawString("<<COMBAT MECHANICS>>", (1280. / 2560)* window.width, (300. / 1440)* window.height, 2.f, true);
            drawString("Weapon selection:\n\nButton <1> - Sword \nButton <2> - Shield \nButton <3> - Bow ", (1300. / 2560) * window.width, (600. / 1440) * window.height, .7f, false);//drawString("TUTORIAL:\nTo hit an enemy with a sword,\npress LMB to strike with sword\nTo hit with a shield,\npress LMB and draw a line that will draw a circle that attacks stars\nTo hit with a bow,\npress LMB on the star and draw a vector in any direction from the star.", (500. / 2560) * window.width, (500. / 1440) * window.height, .7f, false);
            drawString("To increase the force of your strike, hold down the <LMB>.", (600. / 2560) * window.width, (800. / 1440) * window.height, .7f, false);
            drawString("Weapon attack:", (700. / 2560)* window.width, (600. / 1440)* window.height, .7f, false);
            drawString("<LMB> to Attack", (700. / 2560) * window.width, (650. / 1440) * window.height, .7f, false);
            drawString("<RMB> to use the Lasso", (700. / 2560) * window.width, (700. / 1440) * window.height, .7f, false);

            break;

        }
        //case gameState_::Exploring2:
        //{
            /*drawString("Press < TAB > to exit tutorial", (100. / 2560)* window.width, (50. / 1440)* window.height, 1.f, false);
            if (GetAsyncKeyState(VK_TAB)) {
                gameState = gameState_::Fight;
            }
            
            drawString("Press < Left arrow > to flip the page back", (100. / 2560)* window.width, (50. / 1440)* window.height, 1.f, false);
            if (GetAsyncKeyState(VK_LEFT)) {
                gameState = gameState_::Exploring1;
            }
            break;*/
        //}

        Depth::Depth(Depth::depthmode::off);
        if (gameState != gameState_::selectEnemy && gameState != gameState_::Fight && gameState != gameState_::WinFight)
        {
            cursor->vs_id = 6;
            cursor->ps_id = 6;
            CreateCursorParticles();
        }
        else
        {
            cursor->vs_id = 11;
            cursor->ps_id = 11;
        }

        ConstBuf::global[0] = XMFLOAT4(mouse.pos.x / width * 2 - 1, -(mouse.pos.y / height * 2 - 1), 0.0f, 1.0f);
        ConstBuf::Update(5, ConstBuf::global);
        ConstBuf::ConstToVertex(5);
        ConstBuf::Update(1, ConstBuf::drawerP);
        ConstBuf::ConstToPixel(1);
        DrawRenderObject(cursor);

        //CreateParticledText("ANOTHER TEST", (1700. / 2560)* window.width, (600. / 1440)* window.height + 200.f, .7f, false);

        DrawUiParticles(deltaTime);
        RenderParticledText(deltaTime);

        // ОТРИСОВЫВАЕМ ПЕРЕХОД ПОВЕРХ ВСЕГО
        DrawTransitionOverlay();
    }
}


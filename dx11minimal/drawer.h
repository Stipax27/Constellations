bool t = true;
point3d attackDirection;

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
                if (starHealth[starEdges[i][0]] > 0 && starHealth[starEdges[i][1]] > 0)
                {
                    // Устанавливаем яркий цвет для линий между неповрежденными звездами
                    ConstBuf::global[2] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); // Белый
                    ConstBuf::Update(5, ConstBuf::global);
                    ConstBuf::ConstToPixel(5);

                    drawLine(point1, point2, sz * 1.5f); // Увеличиваем толщину линии

                    // Восстанавливаем стандартный цвет
                    ConstBuf::global[2] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
                    ConstBuf::Update(5, ConstBuf::global);
                }
                else if (starHealth[starEdges[i][0]] > 0 || starHealth[starEdges[i][1]] > 0)
                {
                    // Полуповрежденные линии - тонкие и бледные
                    ConstBuf::global[2] = XMFLOAT4(0.7f, 0.7f, 0.7f, 0.5f);
                    ConstBuf::Update(5, ConstBuf::global);
                    ConstBuf::ConstToPixel(5);

                    drawLine(point1, point2, sz * 0.5f);

                    ConstBuf::global[2] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
                    ConstBuf::Update(5, ConstBuf::global);
                }
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

            XMVECTOR p = XMVECTOR{ worldPoint.x,worldPoint.y,worldPoint.z,1 };
            p = XMVector4Transform(p, XMMatrixTranspose(ConstBuf::camera.view[0]) * XMMatrixTranspose(ConstBuf::camera.proj[0]));

            float px = .5f * XMVectorGetX(p) / XMVectorGetW(p) + .5f;
            float py = -.5f * XMVectorGetY(p) / XMVectorGetW(p) + .5f;

            point3d screenPoint = { px,py,0 };
            screenPoint.x *= window.width;
            screenPoint.y *= window.height;

            // Изменяем цвет или размер для неповрежденных звезд
            float currentRadius = finalStarRad;
            XMFLOAT4 starColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); // Белый по умолчанию

            if (i < starHealth.size())
            {
                if (starHealth[i] > 0)
                {
                    // Для неповрежденных звезд - пульсация и другой цвет
                    float pulse = 0.5f + 0.5f * sinf(currentTime * 0.005f);
                    currentRadius += pulse * 5.0f;

                    // Можно использовать другой цвет, например желтый
                    starColor = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f); // Желтый
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

            if (currentRadius > 0)
            {
                point.draw(worldPoint, currentRadius);
            }

            // Восстанавливаем стандартный цвет
            ConstBuf::global[1] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
            ConstBuf::Update(5, ConstBuf::global);
        }
    }

    void drawСonstellation(Constellation& Constellation, bool colorOverride = false , float finalStarRad = 10.f, float sz =2.f)
    {
        Shaders::vShader(1);
        Shaders::pShader(1);

        drawStarPulse(Constellation, colorOverride, finalStarRad);

        Shaders::vShader(4);
        Shaders::pShader(4);

        drawLinks(Constellation,sz);
    }

    void DrawRenderObject(RenderObject* object)
    {
        Shaders::vShader(object->vs_id);
        Shaders::pShader(object->ps_id);
        Blend::Blending(object->blendmode, object->blendop);

        point3d camPos = point3d(XMVectorGetX(Camera::state.Eye), XMVectorGetY(Camera::state.Eye), XMVectorGetZ(Camera::state.Eye));
        float camDist = (camPos - object->pos).magnitude();

        if (camDist < object->renderDistance || object->renderDistance < 0)
        {
            if (object->instances > 1)
                context->DrawInstanced(object->vertexes, object->instances * (1 - min(camDist / object->lodDistanceStep, 1) / object->lodCount), 0, 0);
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
        drawСonstellation(*starSet[player_sign]);
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
        static DWORD battleTime = 60 * 5 * 1000;
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

            if (remainingTime > 0) {

                DrawHpHeroBar();
                std::string timeStr = "Time: " + std::to_string(remainingTime / 1000);
                drawString(timeStr.c_str(), window.width / 1.1, 45, 1.f, true);

                if (getConstellationHP(*starSet[currentEnemyID]) < 0)
                {
                    timeModifier = 0;
                    isBattleActive = false;
                    gameState = gameState_::WinFight;
                }
            }
            else
            {
                timeModifier = 0;
                isBattleActive = false;
                gameState = gameState_::EndFight;

            }
        }
    }

    bool CheckRaySphereCollision(const point3d& rayStart, const point3d& rayDir,
        const point3d& sphereCenter, float sphereRadius)
    {
      
        if (sphereRadius <= 0 || isnan(rayDir.x))return false;
        

            point3d toSphere = sphereCenter - rayStart;
            float projection = toSphere.dot(rayDir);

            
            if (projection < 0 && toSphere.magnitude() > sphereRadius) {
                return false;
            }

        point3d closestPoint = rayStart + rayDir * projection;
            float distanceSquared = (closestPoint - sphereCenter).dot(closestPoint - sphereCenter);

        return distanceSquared < (sphereRadius * sphereRadius);
    }

    struct StarProjectile {
        point3d position;
        point3d direction; 
        float radius;
        point3d up;
        point3d right;
        weapon_name weapon;
    };

    std::vector<StarProjectile> attackStars; // Теперь храним звёзды с их направлениями

    void UpdateAttackStars(float deltaTime) {
        
        for (auto& star : attackStars) {
            star.position += star.direction.normalized() * 5.0f * deltaTime;
        }
    }

    void DrawAttackStars() {

        for (auto& star : attackStars) 
        {

             switch (star.weapon)
             {

             case weapon_name::Sword: {

                    Shaders::vShader(1);
                    Shaders::pShader(1);
                    
                        point3d end = star.position + star.direction * 500.f;
                        drawLine(star.position, end, 3.f);
                        star.position.draw(star.position, 15.0f);
                    
                    break;
                }

                case weapon_name::Shield: {

                    Shaders::vShader(1);
                    Shaders::pShader(1);
                   

                        for (int i = 0; i < 36; i++) {
                            float angle = i * (2 * PI / 36);
                            float nextAngle = (i + 1) * (2 * PI / 36);

                            point3d local1(cos(angle), sin(angle), 0);
                            point3d local2(cos(nextAngle), sin(nextAngle), 0);

                            point3d shield1 = star.position + (star.right * local1.x + star.up * local1.y) * star.radius;
                            point3d shield2 = star.position + (star.right * local2.x + star.up * local2.y) * star.radius;

                            drawLine(shield1, shield2, 10.f);
                        }
                        star.position.draw(star.position, 15.0f);
                    
                    break;
                }

                case weapon_name::Bow: {
                    Shaders::vShader(4);
                    Shaders::pShader(4);

                    point3d arrowStart = star.position;
                    point3d arrowEnd = star.position + star.direction * 500.f;

                    drawLine(arrowStart, arrowEnd, 3.f);

                    point3d tip1 = arrowEnd + star.right * 10.f - star.direction * 20.f;
                    point3d tip2 = arrowEnd - star.right * 10.f - star.direction * 20.f;

                    drawLine(arrowEnd, tip1, 2.f);
                    drawLine(arrowEnd, tip2, 2.f);
                    drawLine(tip1, tip2, 2.f);
                    break;
                }

             }
        }
       
    }

    bool isAttacking = false;
    static DWORD lastAttackTime = 0;
    const float projectileSpeed = 2.0f;
    point3d mouseRay;
    point3d start;
    
    void HandleMouseClick(XMVECTOR heroPosition) {
        if (GetAsyncKeyState(VK_LBUTTON) && 0x8000 && currentTime - lastAttackTime > 500) {

            if (gameState == gameState_::selectEnemy) {
                gameState = gameState_::Fight;
                mciSendString(TEXT("stop ..\\dx11minimal\\GG_C.mp3"), NULL, 0, NULL);
                mciSendString(TEXT("play ..\\dx11minimal\\Oven_NEW.mp3"), NULL, 0, NULL);
            }
            lastAttackTime = currentTime;
           
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
            point3d mousePos = camPos + mouseRay * 6000;
            point3d newDirection = (mousePos - start).normalized();

         
            //attackStars.clear();

            
            switch (current_weapon) {
                case weapon_name::Sword: {
                
                    for (int i = 0; i < 25; i++) {
                        StarProjectile newStar;
                        newStar.position = start;
                        newStar.direction = newDirection;
                        newStar.radius = 15.0f;
                        newStar.weapon = weapon_name::Sword;

                        point3d up = point3d(XMVectorGetX(Camera::state.Up),
                            XMVectorGetY(Camera::state.Up),
                            XMVectorGetZ(Camera::state.Up));
                        point3d right = point3d(XMVectorGetX(Camera::state.Right),
                            XMVectorGetY(Camera::state.Right),
                            XMVectorGetZ(Camera::state.Right));

                        newStar.position += up * (i * 15 - 150);
                        newStar.position += right * (i * 15 - 150);

                        attackStars.push_back(newStar);
                    }
                    ProcessSound("Sword.wav");
                    break;
                }

                case weapon_name::Shield: {
                
                    StarProjectile newStar;
                    newStar.position = start;
                    newStar.direction = newDirection;
                    newStar.radius = 300.0f;
                    newStar.weapon = weapon_name::Shield;

                    newStar.up = point3d(XMVectorGetX(Camera::state.Up),
                        XMVectorGetY(Camera::state.Up),
                        XMVectorGetZ(Camera::state.Up));
                    newStar.right = newDirection.cross(newStar.up).normalized();
                    newStar.up = newStar.right.cross(newDirection).normalized();

                    attackStars.push_back(newStar);
                    ProcessSound("ShieldStan3.wav");
                    break;
                }

                case weapon_name::Bow: {
                    
                    point3d fixedUp = point3d(XMVectorGetX(Camera::state.Up),
                        XMVectorGetY(Camera::state.Up),
                        XMVectorGetZ(Camera::state.Up));
                    point3d fixedRight = newDirection.cross(fixedUp).normalized();
                    fixedUp = fixedRight.cross(newDirection).normalized();

                    for (int i = 0; i < 5; i++) {
                        StarProjectile newStar;
                        newStar.position = start;
                        newStar.direction = newDirection;
                        newStar.radius = 10.0f;
                        newStar.weapon = weapon_name::Bow;
                        
                        newStar.up = fixedUp;
                        newStar.right = fixedRight;
                        

                        attackStars.push_back(newStar);
                    }
                    ProcessSound("Bow.wav");
                    break;
                }
            }

            isAttacking = true;
            //current_weapon = weapon_name::None;
        }
    }

    void UpdateAttack(float deltaTime) {

        if (starSet.empty() ||
            currentEnemyID < 0 ||
            currentEnemyID >= starSet.size() ||
            !starSet[currentEnemyID] ||
            starSet[currentEnemyID]->starsCords.empty()) {
            OutputDebugStringA("Invalid attack state - resetting\n");
           // attackStars.clear();
            return;
        }
        
        if (!starSet[currentEnemyID]) return;
        if (isAttacking) {
            Constellation& enemy = *starSet[currentEnemyID];

            for (int i = 0; i < enemy.starsCords.size(); i++) {
                if (enemy.starsHealth[i] <= 0) continue;

              
                point3d starWorldPos = TransformPoint(enemy.starsCords[i], enemy.Transform);

                
                for (auto& star : attackStars) {
                    float distance = (star.position - starWorldPos).magnitude();

                
                    if (CheckRaySphereCollision(star.position, star.direction,
                        starWorldPos, 1000.f)) { 
                        enemy.starsHealth[i] -= 1.f;
                        std::string enemyH = "HP: " + std::to_string(enemy.starsHealth[i]);
                        drawString(enemyH.c_str(), window.width / 4, window.height / 4, 1.f,true);
                        ProcessSound("Damage.wav");
                        break;
                    }
                }
            }

            isAttacking = false;
        }

        UpdateAttackStars(deltaTime);

        attackStars.erase(
            std::remove_if(attackStars.begin(), attackStars.end(),
                [](const StarProjectile& star) {
                    return (star.position - start).magnitude() > 18000.0f;
                }),
            attackStars.end());
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
                ProcessSound("Mouse_click1.wav");
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
    float sp_minFlySpeed = 0.0f;
    float sp_emitDelta = 1000 / sp_rate;
    DWORD sp_lastEmitTime = 0;

    void CreateSpeedParticles()
    {
        DWORD curTime = timer::GetCounter();
        if (currentFlySpeed >= sp_minFlySpeed && flyDirection.magnitude() > 0)
        {
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

                for (int i = 0; i < (int)(timeDelta / sp_emitDelta); i++)
                {
                    Particle* particle = new Particle;
                    particle->pos = camPos + forward * 6000 + flyDirection * 4000 + (flyUpDirection * GetRandom(-100, 100) + flyRightDirection * GetRandom(-100, 100)).normalized() * 5000;
                    particle->lifetime = GetRandom(750, 1250);
                    particle->startTime = curTime;

                    speedParticles.push_back(particle);
                }
            }
        }
        else
        {
            sp_lastEmitTime = curTime;
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
                    particle->vel = -flyDirection * 0.1f;
                }
                //particle->pos += particle->vel * deltaTime;

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





    void drawWorld(float deltaTime)
    {
        textStyle.color = RGB(0, 191, 255);

        XMVECTOR heroPosition = Hero::state.constellationOffset.r[3];
        XMVECTOR enemyPositions = Enemy::enemyData.enemyConstellationOffset.r[3];

       //d2dRenderTarget->BeginDraw();
        CreateSpeedParticles();
        DrawSpeedParticles();

        //d2dRenderTarget->BeginDraw();
        switch (gameState)
        {
        case gameState_::MainMenu:
            StartMenu();
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

            Constellation& playerConst = *starSet[player_sign];
            playerConst.Transform = CreateHeroToWorldMatrix(playerConst);

            
            Constellation& c = *starSet[0]; // Используем текущего врага

            c.Transform = CreateEnemyToWorldMatrix(c);
            
           
            drawСonstellation(c,false,1000.f,100.f);

            if (!playerConst.morphing)
                HandleMouseClick(heroPosition);
            UpdateAttack(deltaTime);
            DrawSwordAttack();

            drawСonstellation(playerConst);

            std::string curentSignstring = zodiacSignToString(player_sign);
            TextOutA(window.context, window.width * 5 / 6, window.height - window.height / 20., curentSignstring.c_str(), curentSignstring.size());

            //drawString("Find Constallations and click on it", window.width / 2, (200. / 1440) * window.height, 1, true);
            drawString("Features:\nMouse wheel to zoom in and out", (1700. / 2560) * window.width, (1200. / 1440) * window.height, .7f, false);

            playerConst.RenderMorph(deltaTime);

            isBattleActive = false;
            break;

           // drawString("X", 0, 0, 1, false);
           // drawString(std::to_string(mouse.pos.y).c_str(), 0, 0, 1, false);

        }
        case gameState_::DialogStruct:
            initContentData();
            renderContent();
            handleInput();

            break;

        case gameState_::Fight:
        {
            if (starSet.empty() || currentEnemyID < 0 || currentEnemyID >= starSet.size()) {
                gameState = gameState_::selectEnemy;
                break;
            }

            Constellation& playerConst = *starSet[player_sign];
            playerConst.RenderMorph(deltaTime);
            
            if (t) {
                t = false;
                Camera::state.camDist = 100;
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


            modelTransform = &placeConstToWorld;

            srand(currentTime);

            DrawHpEnemyBar();
            modelTransform = &placeConstToWorld;//Враг

            if (isShakingHero) {

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
            }
            
            
            //ProjectileUpdate(deltaTime);
            //FireProjectile();
            //WeaponRender();

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

            if (currentTime > attack_cooldown + 5000)
            {
                
                attackCooldown = true;
            }
            
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
                drawСonstellation(*starSet[currentEnemyID],false, 1000.f, 100.f);

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

                updateEnemyPosition(deltaTime , Heropos, Enemypos);

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
                drawСonstellation(*starSet[currentEnemyID],false, 1000.f, 100.f);
            }

            if (GetAsyncKeyState('P')) {
                gameState = gameState_::WinFight;
                mciSendString(TEXT("stop ..\\dx11minimal\\Oven_NEW.mp3"), NULL, 0, NULL);
                mciSendString(TEXT("play ..\\dx11minimal\\GG_C.mp3"), NULL, 0, NULL);
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

            if (isDamageHero)
            {
                isDamageHero = false;
                isShakingHero = true;
                shakeStartTimeHero = currentTime;
            }

            if (currentTime > shakeStartTimeHero + shakeDurationHero)
            {
                isShakingHero = false;
            }
            Blend::Blending(Blend::blendmode::on, Blend::blendop::add);

            if (starSet.empty() || currentEnemyID < 0 || currentEnemyID >= starSet.size()) {
                gameState = gameState_::selectEnemy;
                break;
            }

            // Безопасный вызов функций атаки
            Constellation& enemy = *starSet[currentEnemyID];
            Constellation& player = *starSet[player_sign];

            // Обновление атак
            if (!playerConst.morphing)
                HandleMouseClick(heroPosition);
            UpdateAttack(deltaTime);
            DrawSwordAttack();

            // Проверка условий победы/поражения
            if (getConstellationHP(enemy) <= 0) {
                gameState = gameState_::WinFight;
                mciSendString(TEXT("stop ..\\dx11minimal\\Oven_NEW.mp3"), NULL, 0, NULL);
                mciSendString(TEXT("play ..\\dx11minimal\\GG_C.mp3"), NULL, 0, NULL);
            }
            else if (getConstellationHP(player) <= 0) {
                gameState = gameState_::EndFight;
            }


            Constellation& c = *starSet[player_sign];
            c.Transform = CreateHeroToWorldMatrix(c);
            drawСonstellation(*starSet[player_sign]);

            std::string curentSignstring = zodiacSignToString(currentEnemyID);
            drawString(curentSignstring.c_str(), window.width / 1.1, window.height / 10., 1, true);

            curentSignstring = zodiacSignToString(player_sign);
            drawString(curentSignstring.c_str(), window.width / 2, window.height - window.height / 7., 1, true);

            curentSignstring = "current weapon: " + weapon[(int)current_weapon].name;
            drawString(curentSignstring.c_str(), window.width / 2, window.height - window.height / 10., 1, true);

            drawCurrentElement();

           drawString("Weapon selection:\nButton 1 - Sword \nButton 2 - Shield \nButton 3 - Bow ", (1700. / 2560) * window.width, (1100. / 1440) * window.height, .7f, false);
           drawString("Rewind time:\nbutton - R", (500. / 2560) * window.width, (1250. / 1440) * window.height, .7f, false);
           drawString("TUTORIAL:\nTo hit an enemy with a sword,\npress LMB and draw a line along the enemy star\nTo hit with a shield,\npress LMB and draw a line that will draw a circle that attacks stars\nTo hit with a bow,\npress LMB on the star and draw a vector in any direction from the star.", (60. / 2560) * window.width, (60. / 1440) * window.height, .7f, false);

            float cdTimeOut = 1. - (currentTime - attack_cooldown) / 5000.;
            cdTimeOut *= 10;
            std::string cdTimeOutText = std::to_string((int)cdTimeOut);
            if (cdTimeOut > 0)
            {
               drawString("recharge", window.width * .9, window.height * .85, 1., false);
               drawString(cdTimeOutText.c_str(), window.width * .9, window.height * .9, 3., false);
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
                Camera::state.camDist = 100;
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
                drawСonstellation(*starSet[currentEnemyID]);

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
                drawСonstellation(*starSet[currentEnemyID]);
            }

            modelTransform = &placeConstToWorld;
            //DrawStarsHP(window.context);

            //linksDivider = 15;
            modelTransform = &placeHeroToWorld;
            uiFunc = &heroUI;

            modelTransform = NULL;
            uiFunc = NULL;

            if (isDamageHero)
            {
                isDamageHero = false;
                isShakingHero = true;
                shakeStartTimeHero = currentTime;
            }

            if (currentTime > shakeStartTimeHero + shakeDurationHero)
            {
                isShakingHero = false;
            }
            Blend::Blending(Blend::blendmode::on, Blend::blendop::add);

            Constellation& c = *starSet[player_sign];
            c.Transform = CreateHeroToWorldMatrix(c);
            drawСonstellation(*starSet[player_sign]);

            std::string curentSignstring = zodiacSignToString(currentEnemyID);
            drawString(curentSignstring.c_str(), window.width / 1.1, window.height / 10., 1, true);

            curentSignstring = zodiacSignToString(player_sign);
            drawString(curentSignstring.c_str(), window.width / 2, window.height - window.height / 7., 1, true);


            //drawCurrentElement();

            //UpdateGame();

            break;
        }
        case gameState_::Exploring:
        {

            break;
        }

        }

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
            DeleteParticledText("TEST TEXT RENDER");
        }

        ConstBuf::global[0] = XMFLOAT4(mouse.pos.x / width * 2 - 1, -(mouse.pos.y / height * 2 - 1), 0.0f, 1.0f);
        ConstBuf::Update(5, ConstBuf::global);
        ConstBuf::ConstToVertex(5);
        ConstBuf::Update(1, ConstBuf::drawerP);
        ConstBuf::ConstToPixel(1);
        DrawRenderObject(cursor);

        DrawUiParticles(deltaTime);
        RenderParticledText(deltaTime);
    }
}


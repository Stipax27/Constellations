namespace WorldRender
{
    void drawLine(point3d& p1, point3d& p2, int count)
    {
        if (count < 1) return;

        for (int i = 0; i < count; i++)
        {
            float dx = p2.x - p1.x;
            float dy = p2.y - p1.y;
            float dz = p2.z - p1.z;
            float length = sqrt(dx * dx + dy * dy + dz * dz);

            point3d point;
            float t = static_cast<float>(i) / count;
            point.x = p1.x + dx * t;
            point.y = p1.y + dy * t;
            point.z = p1.z + dz * t;

            float sz = 6 + 3 * sinf(i * 117 + currentTime * .0031);
            point.draw(point, sz);
            // Ğèñîâàíèå Ëèíèé.
        }
    }

    float linksDivider = 5;

    void drawLinks(Constellation& Constellation)
    {
        std::vector <point3d>& starArray = Constellation.starsCords;
        std::vector<std::vector<float>>& starEdges = Constellation.constellationEdges;
        std::vector <float>& starHealth = Constellation.starsHealth;

        int starsEdgesCount = starEdges.size();
        for (int i = 0; i < starsEdgesCount; i++)
        {

            point3d point1 = TransformPoint(starArray[starEdges[i][0]], Constellation.Transform);
            point3d point2 = TransformPoint(starArray[starEdges[i][1]], Constellation.Transform);

            if (starHealth[starEdges[i][0]] > 0 && starHealth[starEdges[i][1]] > 0) // - Ñòàëî
            {
                float dx = point2.x - point1.x;
                float dy = point2.y - point1.y;
                float dz = point2.z - point1.z;
                float length = sqrt(dx * dx + dy * dy + dz * dz);
                int x = static_cast<int>(length) / linksDivider;
                drawLine(point1, point2, x);// Ğèñîâàíèå çâ¸çäíûõ ëèíèé ñîçâåçäèÿ.
            }
        }
    }

    void drawStarPulse(Constellation& Constellation, bool colorOverride = false)
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

            // Ïóëüñèğîâàíèå Çâ¸çä ïğè íàâåäåíèå ìûøè.s
            finalStarRad = 1;
            if (uiFunc)
            {
                uiFunc(screenPoint, Constellation, i);
            }


            if (finalStarRad > 0)
            {
                point.draw(worldPoint, finalStarRad);
            }
        }
    }

    void drawÑonstellation(Constellation& Constellation, bool colorOverride = false)
    {
        Shaders::vShader(1);
        Shaders::pShader(1);

        drawLinks(Constellation);
        drawStarPulse(Constellation, colorOverride);
    }

    void drawStarField()
    {
        Shaders::vShader(2);
        Shaders::pShader(2);
        Blend::Blending(Blend::blendmode::on, Blend::blendop::add);
        Draw::Starfield(1);
    }

    void drawStars()
    {
        Shaders::vShader(3);
        Shaders::pShader(2);
        Blend::Blending(Blend::blendmode::on, Blend::blendop::add);
        Draw::Starfield(1);
    }

    const COLORREF colors[] =
    {
        RGB(255, 0, 0),    // Êğàñíûé
        RGB(255, 165, 0),  // Îğàíæåâûé
        RGB(255, 255, 0),  // Æåëòûé
        RGB(0, 255, 0),    // Çåëåíûé
        RGB(0, 255, 255),  // Ãîëóáîé
        RGB(0, 0, 255),    // Ñèíèé
        RGB(128, 0, 128)   // Ôèîëåòîâûé
    };



    void morphWepon(std::vector <point3d>& starArray1, std::vector<std::vector<float>> starEdges1, std::vector <point3d>& starArray2, std::vector<std::vector<float>> starEdges2, std::vector <point3d>& morphArray, std::vector <std::vector <float>> Morp_indices, std::vector <float> Morp_health)
    {
        morphArray.clear();
        Morp_indices.clear();
        Morp_health.clear();
        int sz1 = starArray1.size();
        int sz2 = starArray2.size();
        int sz3 = starEdges1.size();
        int sz4 = starEdges2.size();
        if (sz1 < sz2)
        {
            for (int i = 0; i < sz1; i++)
            {
                float morphSpeed = 0.01;
                morphArray.push_back(point3d::lerp(starArray1[i], starArray2[i], (0.5 + 0.5 * sin(currentTime * morphSpeed))));
            }
        }
        else
        {
            for (int i = 0; i < sz2; i++)
            {
                float morphSpeed = 0.01;
                morphArray.push_back(point3d::lerp(starArray1[i], starArray2[i], (0.5 + 0.5 * sin(currentTime * morphSpeed))));
            }
        }
        if (sz3 > sz4)
        {
            for (float i = 0; i < sz4; i++)
            {
                Morp_indices.push_back({ i, i + 1 });
            }
        }
        else
        {
            for (float i = 0; i < sz3; i++)
            {
                Morp_indices.push_back({ i, i + 1 });
            }
        }
        for (int i = 0; i < 15; i++)
        {
            Morp_health.push_back(1);
        }

        //drawÑonstellation(morphArray, Morp_indices, Morp_health); Îòêëş÷åíî
    }

    void drawPlayerÑonstellationToMenu()
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
        drawÑonstellation(*starSet[player_sign]);
    }

    void drawShieldCircle()
    {
        float dx = mouse.pos.x - mouse.oldPos.x;
        float dy = mouse.pos.y - mouse.oldPos.y;
        float shieldRadius = sqrt(dx * dx + dy * dy);

        // Öåíòğ = oldmouse (êğóã ğàñò¸ò èç ıòîé òî÷êè)
        float centerX = mouse.oldPos.x;
        float centerY = mouse.oldPos.y;
        modelProject = &NullProject;
        if (GetAsyncKeyState(VK_LBUTTON)) {
            for (int i = 0; i < 36; i++) {
                float angle = i * (2 * PI / 36);  // 36 òî÷åê äëÿ ãëàäêîãî êğóãà
                float nextAngle = (i + 1) * (2 * PI / 36);

                point3d shield1, shield2;

                shield1.x = centerX + shieldRadius * cos(angle);
                shield1.y = centerY + shieldRadius * sin(angle);
                shield1.z = 0;

                shield2.x = centerX + shieldRadius * cos(nextAngle);
                shield2.y = centerY + shieldRadius * sin(nextAngle);
                shield2.z = 0;

                drawLine(shield1, shield2, 5);
            }
        }
    }

    struct WeaponFlight {
        point3d startPos;
        point3d endPos;
        point3d currentPos;
        float progress;
        float speed;
        bool isFlying;
    };

    WeaponFlight constellationFlight;

    point3d startPoint;

    float lerp(float start, float end, float t) {
        return start + (end - start) * t;
    }
    void VectorWeapons(point3d& p, Constellation& Constellations)
    {

        point3d enemyPosition = { 0,0,0 };


        if (weapon[(int)current_weapon].constellation)
        {
            p = weapon[(int)current_weapon].constellation->getPosition();

            startPoint = { 0, 0, 0 };
            placeHeroToWorld(startPoint, *starSet[player_sign]);

            int ñount = 100;
            placeConstToWorld(enemyPosition, *starSet[currentEnemyID]);
            modelProject = &fightProject;

            drawLine(startPoint, enemyPosition, ñount);

        }
    }

    void InitConstellationAttack() {

        point3d mouseAngleBackup = mouse.Angle;
        mouse.Angle.x = 0;
        mouse.Angle.y = 0;

        point3d heroPos = { 0, 0, 0 };

        placeHeroToWorld(heroPos, *starSet[player_sign]);

        point3d enemyPos = { 0, 0, 0 };
        placeConstToWorld(enemyPos, *starSet[currentEnemyID]);

        constellationFlight.startPos = heroPos;
        constellationFlight.endPos = enemyPos;
        constellationFlight.currentPos = heroPos;
        constellationFlight.progress = 0.0f;
        constellationFlight.speed = 0.05f;
        constellationFlight.isFlying = true;

        mouse.Angle = mouseAngleBackup;
    }

    void ReturnHeroConstellation() {
        point3d homePos = { 0, 0, 0 };
        placeHeroToWorld(homePos, *starSet[player_sign]);

        for (auto& star : starSet[player_sign]->starsCords) {
            star = homePos;
        }
    }


    void Update()
    {
        textStyle.color = RGB(0, 191, 255);
        Draw::Clear({ 0.0f, 0.0588f, 0.1176f, 1.0f });
        Draw::ClearDepth();

        //d2dRenderTarget->BeginDraw();
        switch (gameState)
        {

        case gameState_::selectEnemy:
        {
            Camera::state.mouse = true;
            Depth::Depth(Depth::depthmode::off);
            Blend::Blending(Blend::blendmode::on, Blend::blendop::add);
            uiFunc = &constSelectUI;
            //linksDivider = 15;

            drawStarField();
            Shaders::vShader(1);
            Shaders::pShader(1);
            //modelProject = &fightProject;
            //modelTransform = &placeConstToWorld;


            for (int i = 0; i < 12; i++)
            {
                Constellation& c = *starSet[i];
                c.Transform = CreateConstToWorldMatrix(c);
                drawÑonstellation(*starSet[i]);
            }

            std::string curentSignstring = zodiacSignToString(player_sign);
            TextOutA(window.context, window.width * 5 / 6, window.height - window.height / 20., curentSignstring.c_str(), curentSignstring.size());

            break;
        }

        case gameState_::Fight:
        {

            if (t) {
                t = false;
                Camera::state.camDist = 100;
            }
            Camera::state.mouse = true;
            Depth::Depth(Depth::depthmode::off);
            SelectWeapon();
            SelectElement();


            DrawCombatStats();

            if (attackCooldown == true) {

                AttackVector();

                if (current_weapon == weapon_name::Shield)
                {
                    drawShieldCircle();
                }
            }

            //StartBattle();
            //enemyFight();

            modelTransform = &placeToWorld;
            modelProject = &fightProject;
            uiFunc = &starIntersectUI;
            //linksDivider = 50;

            drawStarField();
            drawStars();


            modelTransform = &placeConstToWorld;

            srand(currentTime);

            modelTransform = &placeConstToWorld;//Âğàã

            if (isShakingHero) {

                float beamTime = 4. * (currentTime - shakeStartTimeHero) / shakeDurationHero;
                if (beamTime > 1.) beamTime = 1;
                for (int i = 0; i < starSet[currentEnemyID]->starsCords.size(); i++)
                {
                    auto p1 = starSet[currentEnemyID]->starsCords[i];
                    point3d p2 = starSet[player_sign]->starsCords[i % starSet[player_sign]->starsCords.size()];

                    placeConstToWorld(p1, *starSet[currentEnemyID]);
                    placeHeroToWorld(p2, *starSet[currentEnemyID]);

                    p2.x = lerp(p1.x, p2.x, beamTime);
                    p2.y = lerp(p1.y, p2.y, beamTime);
                    p2.z = lerp(p1.z, p2.z, beamTime);


                    drawLine(p1, p2, 50);
                }
            }

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
                drawÑonstellation(*starSet[currentEnemyID]);

                //linksDivider = 15;
                modelTransform = &placeHeroToWorld;
                uiFunc = &heroUI;

                Blend::Blending(Blend::blendmode::on, Blend::blendop::add);

                Constellation& c = *starSet[player_sign];
                c.Transform = CreateHeroToWorldMatrix(c);
                drawÑonstellation(*starSet[player_sign]);//Èãğîê

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
                drawÑonstellation(*starSet[currentEnemyID]);
            }

            if (currentTime > attack_time + weapon[(int)current_weapon].attackSpeed and attack_start == true)
            {
                isDamageTaken = true;
                attack_start = false;
            }

            modelTransform = &placeConstToWorld;

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
            drawÑonstellation(*starSet[player_sign]);

            break;
        }

        }

    }
}


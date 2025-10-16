
GameTransition currentTransition;

void StartTransition(gameState_ toState, float transitionDuration) {
    currentTransition.state = TransitionState::FADE_OUT;
    currentTransition.fromState = gameState;
    currentTransition.toState = toState;
    currentTransition.duration = transitionDuration;
    currentTransition.startTime = currentTime;
    currentTransition.progress = 0.0f;
    currentTransition.fadeAlpha = 0.0f;
}

void UpdateTransition(float deltaTime) {
    if (currentTransition.state == TransitionState::NONE) return;

    float elapsed = (float)(currentTime - currentTransition.startTime);
    currentTransition.progress = min(elapsed / currentTransition.duration, 1.0f);

    switch (currentTransition.state) {
    case TransitionState::FADE_OUT:
        currentTransition.fadeAlpha = currentTransition.progress;
        if (currentTransition.progress >= 1.0f) {
            // Меняем состояние игры
            gameState = currentTransition.toState;
            InitializeState(currentTransition.toState);
            currentTransition.state = TransitionState::FADE_IN;
            currentTransition.startTime = currentTime;
            currentTransition.progress = 0.0f;
        }
        break;

    case TransitionState::FADE_IN:
        currentTransition.fadeAlpha = 1.0f - currentTransition.progress;
        if (currentTransition.progress >= 1.0f) {
            currentTransition.state = TransitionState::COMPLETE;
        }
        break;

    case TransitionState::COMPLETE:
        currentTransition.state = TransitionState::NONE;
        break;
    }
}

void DrawTransitionOverlay() {
    if (currentTransition.state == TransitionState::NONE) return;

    Shaders::vShader(1);
    Shaders::pShader(1);
    Blend::Blending(Blend::blendmode::on, Blend::blendop::sub);

    // Черный квадрат на весь экран с альфой7
    XMFLOAT4 fadeColor = XMFLOAT4(1.0f, 1.0f, 1.0f, currentTransition.fadeAlpha);
    ConstBuf::global[1] = fadeColor;
    ConstBuf::Update(5, ConstBuf::global);
    ConstBuf::ConstToPixel(5);

    // Рисуем полноэкранный квадрат
    point3d screenCenter = { 0, 0, 0 };
    screenCenter.draw(screenCenter, 1000000.0f);

    // Восстанавливаем цвет
    ConstBuf::global[1] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    ConstBuf::Update(5, ConstBuf::global);
}

void InitializeState(gameState_ newState) {
    switch (newState) {
    case gameState_::Fight:
        // Инициализация боя
        isBattleActive = true;
        battleStartTime = currentTime;
        energy = 300;
        current_weapon = weapon_name::Fists;

        // Сбрасываем атаки
        attackStars.clear();
        bowTracerParticles.clear();

        // Сброс камеры для боя
        Camera::state.distanceOffset = MIN_CAMERA_DIST;
        currentCameraDistance = MIN_CAMERA_DIST;
        break;

    case gameState_::selectEnemy:
        // Инициализация выбора врага
        isBattleActive = false;
        energy = 300;

        // Плавный сброс масштабов
        currentHeroScale = MIN_HERO_SCALE;
        currentStarRadius = MIN_STAR_RADIUS;

        // Сброс камеры для свободного обзора
        Camera::state.distanceOffset = 5000.0f;
        break;

    case gameState_::WinFight:
        // Инициализация экрана победы
        mciSendString(TEXT("stop ..\\dx11minimal\\Resourses\\Sounds\\Oven_NEW.mp3"), NULL, 0, NULL);
        mciSendString(TEXT("play ..\\dx11minimal\\Resourses\\Sounds\\FREEFLY.mp3"), NULL, 0, NULL);
        break;

    case gameState_::EndFight:
        // Инициализация экрана поражения
        mciSendString(TEXT("stop ..\\dx11minimal\\Resourses\\Sounds\\Oven_NEW.mp3"), NULL, 0, NULL);
        mciSendString(TEXT("play ..\\dx11minimal\\Resourses\\Sounds\\FREEFLY.mp3"), NULL, 0, NULL);
        break;

    case gameState_::Exploring:
    case gameState_::Exploring1:
        // Инициализация tutorial - ничего не делаем
        break;

    case gameState_::MainMenu:
        // Сброс к состоянию меню
        isBattleActive = false;
        energy = 300;
        attackStars.clear();
        break;
    }
}
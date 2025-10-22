void drawRect(float width, float height, float x, float y, XMFLOAT4 color, int vShader = 13, int pShader = 13)
{
    Shaders::vShader(vShader); // ID прямоугольного шейдера
    Shaders::pShader(pShader);
    Blend::Blending(Blend::blendmode::alpha, Blend::blendop::add);

    ConstBuf::global[0] = XMFLOAT4(width, height, 0, 0);
    ConstBuf::global[1] = XMFLOAT4((float)window.width, (float)window.height, 0, 0);
    ConstBuf::global[2] = XMFLOAT4(x, y, 0, 0);
    ConstBuf::global[3] = color;

    ConstBuf::Update(5, ConstBuf::global);
    ConstBuf::ConstToVertex(5);
    ConstBuf::ConstToPixel(5);
    context->DrawInstanced(6, 1, 0, 0);
}

// Функция для рисования звездных частиц на фоне (полный бар)
void drawBackgroundParticles(float barX, float barY, float barWidth, float barHeight, XMFLOAT4 baseColor, int particleCount, float time) {
    for (int i = 0; i < particleCount; i++) {
        float seed = (i * 123.456f) + time;
        float posX = barX + (fmod(seed * 47.3f, barWidth)); // На всем фоне
        float posY = barY + (fmod(seed * 73.1f, barHeight));

        float size = 1.0f + fmod(seed * 29.7f, 1.0f);
        float alpha = 0.3f + 0.4f * (0.5f + 0.5f * sinf(time * 2.0f + seed * 5.0f));

        XMFLOAT4 particleColor = XMFLOAT4(
            baseColor.x * (0.8f + 0.4f * fmod(seed * 13.7f, 1.0f)),
            baseColor.y * (0.8f + 0.4f * fmod(seed * 17.3f, 1.0f)),
            baseColor.z * (0.8f + 0.4f * fmod(seed * 19.1f, 1.0f)),
            baseColor.w * alpha * 0.1f // Более прозрачные для фона
        );

        drawRect(size, size, posX, posY, particleColor);
    }
}
// Добавляем структуру для управления состоянием змейки
// Добавляем структуру для управления состоянием змейки
struct SnakeEffect {
    float position = 0.0f;
    bool isActive = false;
    float fadeAlpha = 1.0f; // Новое: прозрачность для плавного исчезания
    float fadeSpeed = 0.02f; // Скорость исчезания
};

// Глобальная переменная для змейки
SnakeEffect staminaSnake;

// Функция для рисования звездных частиц на заполненной части
void drawFillParticles(float barX, float barY, float fillWidth, float barHeight, XMFLOAT4 baseColor, int particleCount, float time) {
    // Статические переменные для глобальных эффектов
    static float globalPulse = 0.0f;
    globalPulse += time * 0.02f;

    float energyFlow = sinf(globalPulse) * 0.5f + 0.5f;

    // УПРАВЛЕНИЕ ЗМЕЙКОЙ - с проверкой активности
    staminaSnake.position += 0.005f; // АБСОЛЮТНАЯ постоянная скорость

    // Если змейка дошла до 90% бара, начинаем исчезать
    if (staminaSnake.position > 0.9f && staminaSnake.isActive) {
        staminaSnake.isActive = false; // Начинаем процесс исчезания
    }

    // Если змейка не активна и еще не полностью исчезла, продолжаем уменьшать прозрачность
    if (!staminaSnake.isActive && staminaSnake.fadeAlpha > 0.0f) {
        staminaSnake.fadeAlpha -= staminaSnake.fadeSpeed;
        if (staminaSnake.fadeAlpha <= 0.0f) {
            staminaSnake.fadeAlpha = 0.0f;
            staminaSnake.position = 0.0f; // Сбрасываем позицию для следующего появления
        }
    }

    // Если змейка полностью исчезла и нужно начать новую, активируем ее
    if (staminaSnake.fadeAlpha <= 0.0f && staminaSnake.position == 0.0f) {
        staminaSnake.isActive = true;
        staminaSnake.fadeAlpha = 1.0f;
    }

    // Рисуем змейку только если она видима (альфа > 0)
    if (staminaSnake.fadeAlpha > 0.0f) {
        // Рисуем змейку из сегментов
        int snakeSegments = 45;
        float snakeLength = fillWidth * 0.6f;

        for (int seg = 0; seg < snakeSegments; seg++) {
            // Постоянное смещение для каждого сегмента
            float segmentOffset = staminaSnake.position - (seg * 0.03f);

            // Если сегмент вышел за пределы 90% бара, пропускаем его
            if (segmentOffset < 0.0f || segmentOffset > 0.9f) {
                continue;
            }

            // Плавное волнообразное движение вверх-вниз
            float waveOffset = sinf(segmentOffset * PI * 4.0f + seg * 0.8f) * (barHeight * 0.15f);
            float segmentX = barX + (segmentOffset * fillWidth);
            float segmentY = barY + (barHeight * 0.5f) + waveOffset;

            // Размер сегмента уменьшается к хвосту
            float segmentSize = barHeight * (0.9f - (seg * 0.05f));

            // НОВОЕ: Учитываем общую прозрачность змейки при исчезании
            float segmentAlpha = 0.7f * (1.0f - (seg * 0.08f)) * staminaSnake.fadeAlpha;

            // Цвет змейки с учетом прозрачности
            XMFLOAT4 snakeColor = XMFLOAT4(
                baseColor.x * 1.6f,
                baseColor.y * 1.6f,
                baseColor.z * 1.6f,
                segmentAlpha
            );

            // Рисуем сегмент змейки
            drawRect(segmentSize * 0.7f, segmentSize, segmentX, segmentY - segmentSize * 0.5f, snakeColor);

            // Свечение вокруг сегмента (тоже с учетом прозрачности)
            if (seg < 5) {
                float glowSize = segmentSize * 1.4f;
                float glowAlpha = segmentAlpha * 0.4f;
                XMFLOAT4 glowColor = XMFLOAT4(
                    snakeColor.x,
                    snakeColor.y,
                    snakeColor.z,
                    glowAlpha
                );
                drawRect(glowSize * 0.7f, glowSize, segmentX, segmentY - glowSize * 0.5f, glowColor);
            }
        }
    }

    // Остальной код частиц остается без изменений
    for (int i = 0; i < particleCount; i++) {
        float seed = (i * 137.123f) + time;

        // Более сложное распределение частиц
        float normalizedPos = fmod(seed * 37.3f, 1.0f);
        float waveOffset = sinf(normalizedPos * PI * 4.0f + time * 3.0f) * (barHeight * 0.3f);

        float posX = barX + (normalizedPos * fillWidth);
        float posY = barY + (fmod(seed * 73.1f, barHeight)) + waveOffset;

        // Динамический размер частиц
        float pulse = 0.5f + 0.5f * sinf(time * 4.0f + seed * 7.0f);
        float flowEffect = 1.0f + sinf(normalizedPos * PI * 2.0f - time * 2.0f) * 0.3f;
        float size = (1.5f + fmod(seed * 23.7f, 2.0f)) * pulse * flowEffect;

        // Сложное мерцание с несколькими частотами
        float fastFlicker = 0.7f + 0.3f * sinf(time * 8.0f + seed * 3.0f);
        float slowFlicker = 0.5f + 0.5f * sinf(time * 1.5f + seed * 11.0f);
        float rhythmicPulse = 0.8f + 0.2f * sinf(normalizedPos * PI * 6.0f + time * 5.0f);

        float alpha = (0.4f + 0.6f * fastFlicker) * slowFlicker * rhythmicPulse * energyFlow;

        // Градиент цвета по длине бара
        float colorPos = normalizedPos;
        XMFLOAT4 gradientColor;

        if (colorPos < 0.33f) {
            float t = colorPos / 0.33f;
            gradientColor = XMFLOAT4(
                baseColor.x * (0.7f + 0.3f * t),
                baseColor.y * (0.7f + 0.3f * t),
                baseColor.z * (0.7f + 0.3f * t),
                baseColor.w
            );
        }
        else if (colorPos < 0.66f) {
            gradientColor = XMFLOAT4(
                baseColor.x * 1.2f,
                baseColor.y * 1.2f,
                baseColor.z * 1.2f,
                baseColor.w
            );
        }
        else {
            float t = (colorPos - 0.66f) / 0.34f;
            gradientColor = XMFLOAT4(
                baseColor.x * (1.2f - 0.5f * t),
                baseColor.y * (1.2f - 0.5f * t),
                baseColor.z * (1.2f - 0.5f * t),
                baseColor.w
            );
        }

        // Вариации цвета с теплыми/холодными тонами
        float hueShift = sinf(time * 1.0f + normalizedPos * PI * 2.0f) * 0.1f;
        XMFLOAT4 particleColor = XMFLOAT4(
            min(1.0f, gradientColor.x * (0.9f + 0.2f * fmod(seed * 13.7f, 1.0f)) + hueShift),
            min(1.0f, gradientColor.y * (0.9f + 0.2f * fmod(seed * 17.3f, 1.0f)) - hueShift * 0.5f),
            min(1.0f, gradientColor.z * (0.9f + 0.2f * fmod(seed * 19.1f, 1.0f)) - hueShift),
            baseColor.w * alpha
        );

        // Рисуем основную частицу
        drawRect(size, size, posX, posY, particleColor);

        // 30% шанс нарисовать дополнительное свечение
        if (fmod(seed * 57.3f, 1.0f) < 0.3f) {
            float glowSize = size * (1.8f + 0.4f * sinf(time * 6.0f + seed));
            float glowAlpha = alpha * 0.3f;
            XMFLOAT4 glowColor = XMFLOAT4(
                particleColor.x,
                particleColor.y,
                particleColor.z,
                glowAlpha
            );
            drawRect(glowSize, glowSize, posX, posY, glowColor);
        }

        // 15% шанс нарисовать след/хвост
        if (fmod(seed * 43.1f, 1.0f) < 0.15f) {
            float trailLength = 3.0f + 2.0f * sinf(time * 0.01f + seed);
            float trailWidth = size * 0.5f;
            float trailAlpha = alpha * 0.4f;

            // След против движения (влево)
            float trailX = posX - trailLength;
            XMFLOAT4 trailColor = XMFLOAT4(
                particleColor.x * 0.7f,
                particleColor.y * 0.7f,
                particleColor.z * 0.7f,
                trailAlpha
            );
            drawRect(trailLength, trailWidth, trailX, posY, trailColor);
        }
    }
}


float maxHP = 6.0f;
float normalizedHP = maxHP;

void drawHPBar(float HP) {
    normalizedHP = HP / maxHP;
    float barWidth = 240.0f;
    float barHeight = 18.0f;
    float x = (250.0f / 2560.0f) * window.width;
    float y = (1300.0f / 1440.0f) * window.height;
    static float time = 0.0f;
    time += 0.016f;

    // Фон бара
    drawRect(barWidth + 6, barHeight + 6, x - 3, y - 3, XMFLOAT4(0.1f, 0.05f, 0.05f, 0.9f));

    // Фоновые частицы (на всей ширине фона)
    drawBackgroundParticles(x, y, barWidth, barHeight, XMFLOAT4(0.6f, 0.3f, 0.3f, 0.4f), 5, time);

    // Рамка
    drawRect(barWidth + 4, barHeight + 4, x - 2, y - 2, XMFLOAT4(0.4f, 0.2f, 0.2f, 1.0f));

    // Основной бар HP
    float currentWidth = normalizedHP * barWidth;
    if (currentWidth > 0) {
        XMFLOAT4 color;
        if (normalizedHP > 0.6f) {
            float t = (normalizedHP - 0.6f) / 0.4f;
            color = XMFLOAT4(1.0f - t, 1.0f, 0.0f, 1.0f);
        }
        else if (normalizedHP > 0.3f) {
            float t = (normalizedHP - 0.3f) / 0.3f;
            color = XMFLOAT4(1.0f, t, 0.0f, 1.0f);
        }
        else {
            float t = normalizedHP / 0.3f;
            color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
        }

        drawRect(currentWidth, barHeight, x, y, color);

        // Звездные частицы ТОЛЬКО на заполненной части!
        int particleCount = (int)(normalizedHP * 8);
        XMFLOAT4 particleColor;
        if (normalizedHP > 0.6f) {
            particleColor = XMFLOAT4(1.0f, 1.0f, 0.8f, 0.6f);
        }
        else {
            particleColor = XMFLOAT4(1.0f, 0.8f, 0.8f, 0.6f);
        }
        drawFillParticles(x, y, currentWidth * 1.2f, barHeight, particleColor, particleCount, time);

        // Пульсация с частицами при низком HP
        if (normalizedHP < 0.3f) {
            float pulse = (sinf(time * 8.0f) + 1.0f) * 0.5f;
            drawRect(currentWidth, barHeight, x, y, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.2f * pulse));

            // Красные тревожные частицы только на заполненной части
            drawFillParticles(x, y, currentWidth, barHeight, XMFLOAT4(1.0f, 0.3f, 0.3f, pulse), 4, time * 3.0f);
        }
    }
}

float lastCharge = 0;
void drawStrengthStar(float charge) {
    lastCharge = lerp(lastCharge, charge, 0.1);
    float size = lerp(90, 180, lastCharge);

    float barWidth = size;
    float barHeight = size / iaspect;
    float x = ((110.0f - size / 2) / 2560.0f) * window.width;
    float y = ((1280.0f - size / 2) / 1440.0f) * window.height;
    static float time = 0.011f;
    time += 0.016f;

    drawRect(barWidth, barHeight, x, y, XMFLOAT4(1.0f, 1.0f, 0.8f, 0.6f), 13, 16);
}

void drawEnemyBar(float enemyHP) {
    Constellation& enemy = *starSet[currentEnemyID];
    float normalizedHP = enemyHP / enemy.maxHP;
    float barWidth = 400.0f;
    float barHeight = 22.0f;
    float x = (880.0f / 2560.0f) * window.width;
    float y = (150.0f / 1440.0f) * window.height;
    static float time = 0.0f;
    time += 0.016f;

    // Темный фон с космическим оттенком
    drawRect(barWidth + 8, barHeight + 8, x - 4, y - 4, XMFLOAT4(0.08f, 0.02f, 0.1f, 0.3f));

    // Фоновые частицы (на всей ширине фона) - звездная пыль
    drawBackgroundParticles(x, y, barWidth, barHeight, XMFLOAT4(0.9f, 0.7f, 0.3f, 0.1f), 10, time);

    // Рамка вражеского бара с градиентом от золотого к темному
    drawRect(barWidth + 6, barHeight + 6, x - 3, y - 3, XMFLOAT4(0.6f, 0.4f, 0.1f, .4f));

    // Основной бар врага с эффектом гаснущей звезды
    float currentWidth = normalizedHP * barWidth;
    if (currentWidth > 0) {
        XMFLOAT4 color;

        // Градиент гаснущей звезды:
        if (normalizedHP > 0.8f) {
            // Яркая звезда - бело-голубой (максимальная температура)
            float t = (normalizedHP - 0.8f) / 0.2f;
            color = XMFLOAT4(1.0f, 0.9f + t * 0.1f, 0.8f + t * 0.2f, 1.0f);
        }
        else if (normalizedHP > 0.6f) {
            // Желтая звезда - еще горячая
            float t = (normalizedHP - 0.6f) / 0.2f;
            color = XMFLOAT4(1.0f, 0.8f + t * 0.1f, 0.4f + t * 0.4f, 1.0f);
        }
        else if (normalizedHP > 0.4f) {
            // Оранжевый гигант - звезда остывает
            float t = (normalizedHP - 0.4f) / 0.2f;
            color = XMFLOAT4(1.0f, 0.5f + t * 0.3f, 0.2f + t * 0.2f, 1.0f);
        }
        else if (normalizedHP > 0.2f) {   
            // Красный гигант - звезда расширяется и остывает
            float t = (normalizedHP - 0.2f) / 0.2f;
            color = XMFLOAT4(1.0f, 0.3f + t * 0.2f, 0.1f + t * 0.1f, 1.0f);
        }
        else {
            // Коричневый карлик - почти погасшая звезда
            float t = normalizedHP / 0.2f;
            color = XMFLOAT4(1.0f + t * 0.1f, 0.3f + t * 0.2f, 0.1f, 1.0f);
        }

        drawRect(currentWidth, barHeight, x, y, color);

        // Звездные частицы с цветом, соответствующим текущей фазе звезды
        int particleCount = (int)(normalizedHP * 15);
        XMFLOAT4 particleColor;

        if (normalizedHP > 0.6f) {
            // Яркие бело-голубые частицы для горячей звезды
            particleColor = XMFLOAT4(0.8f, 0.9f, 1.0f, 0.8f);
        }
        else if (normalizedHP > 0.3f) {
            // Золотистые частицы для остывающей звезды
            particleColor = XMFLOAT4(1.0f, 0.8f, 0.4f, 0.7f);
        }
        else {
            // Тусклые красные частицы для умирающей звезды
            particleColor = XMFLOAT4(1.0f, 0.4f, 0.2f, 0.6f);
        }

        //drawFillParticles(x, y, currentWidth, barHeight, particleColor, particleCount, time);

        // Эффект "вспышек" при низком HP - как последние вспышки умирающей звезды
        if (normalizedHP < 0.25f) {
            float pulse = (sinf(time * 6.0f) + 1.0f) * 0.5f;

            for (int i = 0; i < 2; i++) {
                float flashPos = fmod(time * 3.0f + i * 2.0f, currentWidth);
                float flashWidth = 8.0f + 4.0f * sinf(time * 4.0f + i);
                float flashAlpha = 0.3f + 0.4f * pulse;

               /*drawRect(flashWidth, barHeight - 2, x + flashPos, y + 1,
                    XMFLOAT4(1.0f, 0.6f, 0.2f, flashAlpha));*/
            }

            
            drawFillParticles(x, y, currentWidth, barHeight, XMFLOAT4(1.0f, 0.1f, 0.1f, pulse), 8, time * 5.0f);
        }

        // Эффект "суперновой" для боссов при критическом HP
        //if (enemy.maxHP > 100.0f && normalizedHP < 0.15f) {
        //    float supernovaPulse = (sinf(time * 8.0f) + 1.0f) * 0.5f;

        //    // Яркое свечение вокруг бара
        //    drawRect(currentWidth + 4, barHeight + 4, x - 2, y - 2,
        //        XMFLOAT4(1.0f, 0.9f, 0.4f, 0.3f * supernovaPulse));

        //    // Особые частицы сверхновой
        //    drawFillParticles(x, y, currentWidth, barHeight,
        //        XMFLOAT4(1.0f, 1.0f, 0.7f, 0.9f * supernovaPulse),
        //        10, time * 6.0f);
        //}

        //// Особые частицы для боссов (только на заполненной части)
        //if (enemy.maxHP > 100.0f && normalizedHP > 0.15f) {
        //    // Золотистые сияющие частицы для здоровых боссов-звезд
        //    drawFillParticles(x, y, currentWidth, barHeight,
        //        XMFLOAT4(1.0f, 0.9f, 0.5f, 0.8f),
        //        6, time * 2.0f);
        //}
    }

    // Эффект мерцания рамки при очень низком HP
    if (normalizedHP < 0.4f) {
        float framePulse = (sinf(time * 10.0f) + 1.0f) * 0.5f;
        drawRect(barWidth + 6, barHeight + 6, x - 3, y - 3,
            XMFLOAT4(1.0f, 0.6f, 0.1f, framePulse));
    }
}

// Структура для иконки оружия
struct WeaponIcon {
    float x, y;
    float size;
    weapon_name weaponType;
    bool isSelected;
    XMFLOAT4 baseColor;
    XMFLOAT4 glowColor;
};

// СУПЕР-ДЕТАЛИЗИРОВАННАЯ функция для рисования иконки кулаков
// УЛЬТРА-ДЕТАЛИЗИРОВАННЫЙ КУЛАК
void DrawFistIcon(float x, float y, float size, XMFLOAT4 color, bool isSelected, float time) {
    float centerX = x;
    float centerY = y;

    // ОСНОВНОЙ КУЛАК - четкая форма сжатой руки
    float fistSize = size * 0.9f;

    // Основная форма кулака (квадратная с закруглениями)
    drawRect(fistSize, fistSize, centerX - fistSize / 2, centerY - fistSize / 2, color);

    // КОСТЯШКИ - 4 выраженные линии
    float knuckleWidth = size * 0.4f;
    float knuckleHeight = size * 0.05f;
    float knuckleSpacing = size * 0.15f;

    for (int i = 0; i < 4; i++) {
        float knuckleX = centerX - knuckleWidth / 2;
        float knuckleY = centerY - fistSize / 3 + (i * knuckleSpacing);
        drawRect(knuckleWidth, knuckleHeight, knuckleX, knuckleY,
            XMFLOAT4(color.x * 0.8f, color.y * 0.8f, color.z * 0.8f, color.w));
    }

    // БОЛЬШОЙ ПАЛЕЦ - четко видимый
    float thumbWidth = size * 0.3f;
    float thumbHeight = size * 0.2f;
    float thumbX = centerX + fistSize * 0.25f;
    float thumbY = centerY + fistSize * 0.1f;
    drawRect(thumbWidth, thumbHeight, thumbX - thumbWidth / 2, thumbY - thumbHeight / 2, color);

    // СУСТАВ БОЛЬШОГО ПАЛЬЦА
    float thumbJointSize = size * 0.08f;
    drawRect(thumbJointSize, thumbJointSize, thumbX - thumbWidth * 0.3f, thumbY,
        XMFLOAT4(color.x * 0.7f, color.y * 0.7f, color.z * 0.7f, color.w));

    // МЫШЕЧНЫЕ ВЫПУКЛОСТИ
    float muscleSize = size * 0.1f;
    drawRect(muscleSize, muscleSize, centerX - fistSize * 0.2f, centerY + fistSize * 0.15f,
        XMFLOAT4(color.x * 0.9f, color.y * 0.9f, color.z * 0.9f, color.w));
    drawRect(muscleSize, muscleSize, centerX + fistSize * 0.1f, centerY + fistSize * 0.05f,
        XMFLOAT4(color.x * 0.9f, color.y * 0.9f, color.z * 0.9f, color.w));

    // ЭФФЕКТ УДАРА
    if (isSelected) {
        float pulse = 0.6f + 0.4f * sinf(time * 10.0f);

        // Волна удара
        float impactSize = size * 1.8f;
        XMFLOAT4 impactColor = XMFLOAT4(color.x, color.y, color.z, 0.6f * pulse);
        drawRect(impactSize, impactSize, centerX - impactSize / 2, centerY - impactSize / 2, impactColor);

        // Энергетические трещины
        for (int i = 0; i < 4; i++) {
            float crackLength = size * 0.6f;
            float crackWidth = size * 0.02f;
            float angle = (i * PI / 4) + time;
            float crackX = centerX + cosf(angle) * crackLength * 0.3f;
            float crackY = centerY + sinf(angle) * crackLength * 0.3f;
            drawRect(crackLength, crackWidth, centerX - crackLength / 2, crackY,
                XMFLOAT4(1.0f, 1.0f, 1.0f, 0.8f * pulse));
        }
    }
}

// УЛЬТРА-ДЕТАЛИЗИРОВАННЫЙ МЕЧ
void DrawSwordIcon(float x, float y, float size, XMFLOAT4 color, bool isSelected, float time) {
    float centerX = x;
    float centerY = y;

    // КЛИНОК - длинный и острый
    float bladeWidth = size * 0.15f;
    float bladeHeight = size * 1.0f;
    drawRect(bladeWidth, bladeHeight, centerX - bladeWidth / 2, centerY - bladeHeight / 2, color);

    // ЛЕЗВИЕ - яркие острые края
    float edgeWidth = bladeWidth * 1.8f;
    float edgeHeight = bladeHeight * 0.02f;
    XMFLOAT4 edgeColor = XMFLOAT4(1.0f, 1.0f, 1.0f, color.w);

    // Двойная заточка
    drawRect(edgeWidth, edgeHeight, centerX - edgeWidth / 2, centerY - bladeHeight / 2 + size * 0.1f, edgeColor);
    drawRect(edgeWidth, edgeHeight, centerX - edgeWidth / 2, centerY + bladeHeight / 2 - size * 0.1f, edgeColor);

    // ДОЛ - центральная канавка
    float fullerWidth = bladeWidth * 0.5f;
    float fullerHeight = bladeHeight * 0.7f;
    XMFLOAT4 fullerColor = XMFLOAT4(color.x * 0.6f, color.y * 0.6f, color.z * 0.6f, color.w);
    drawRect(fullerWidth, fullerHeight, centerX - fullerWidth / 2, centerY - fullerHeight / 3, fullerColor);

    // ОСТРИЕ - четкий треугольник
    float tipHeight = size * 0.3f;
    for (int i = 0; i < 4; i++) {
        float tipSegmentWidth = bladeWidth * (1.0f - i * 0.2f);
        float tipY = centerY + bladeHeight / 2 - tipHeight + (i * tipHeight * 0.25f);
        drawRect(tipSegmentWidth, tipHeight * 0.08f, centerX - tipSegmentWidth / 2, tipY, color);
    }

    // ГАРДА - выраженная крестовина
    float guardWidth = size * 0.6f;
    float guardHeight = size * 0.1f;
    drawRect(guardWidth, guardHeight, centerX - guardWidth / 2, centerY - size * 0.05f, color);

    // Боковые крылья гарды
    float wingWidth = size * 0.08f;
    float wingHeight = size * 0.25f;
    drawRect(wingWidth, wingHeight, centerX - guardWidth / 2, centerY - wingHeight / 2, color);
    drawRect(wingWidth, wingHeight, centerX + guardWidth / 2 - wingWidth, centerY - wingHeight / 2, color);

    // РУКОЯТЬ - с выраженной текстурой
    float handleWidth = size * 0.12f;
    float handleHeight = size * 0.35f;
    XMFLOAT4 handleColor = XMFLOAT4(0.3f, 0.2f, 0.1f, color.w);
    drawRect(handleWidth, handleHeight, centerX - handleWidth / 2, centerY - size * 0.45f, handleColor);

    // ОБМОТКА - четкие витки
    for (int i = 0; i < 5; i++) {
        float wrapY = centerY - size * 0.45f + (i * handleHeight / 5);
        drawRect(handleWidth * 1.3f, handleHeight * 0.06f, centerX - handleWidth * 0.65f, wrapY,
            XMFLOAT4(0.5f, 0.4f, 0.3f, color.w));
    }

    // НАВЕРШИЕ - массивное
    float pommelWidth = size * 0.25f;
    float pommelHeight = size * 0.12f;
    drawRect(pommelWidth, pommelHeight, centerX - pommelWidth / 2, centerY - size * 0.6f, color);

    // ЭФФЕКТ ОСТРОТЫ
    if (isSelected) {
        float pulse = 0.5f + 0.5f * sinf(time * 12.0f);

        // Свечение лезвия
        float bladeGlowWidth = bladeWidth * 3.0f;
        float bladeGlowHeight = bladeHeight * 1.1f;
        XMFLOAT4 bladeGlowColor = XMFLOAT4(color.x, color.y, color.z, 0.7f * pulse);
        drawRect(bladeGlowWidth, bladeGlowHeight, centerX - bladeGlowWidth / 2, centerY - bladeGlowHeight / 2, bladeGlowColor);

        // Движущаяся энергия по клинку
        float energyY = centerY - bladeHeight / 2 + fmod(time * 0.3f, 1.0f) * bladeHeight;
        drawRect(bladeWidth * 2.0f, bladeHeight * 0.05f, centerX - bladeWidth, energyY,
            XMFLOAT4(1.0f, 1.0f, 1.0f, 0.9f * pulse));
    }
}

// УЛЬТРА-ДЕТАЛИЗИРОВАННЫЙ КРУГЛЫЙ ЩИТ
void DrawShieldIcon(float x, float y, float size, XMFLOAT4 color, bool isSelected, float time) {
    float centerX = x;
    float centerY = y;

    // ОСНОВНОЙ ЩИТ - идеальный круг
    float shieldSize = size * 1.2f;
    drawRect(shieldSize, shieldSize, centerX - shieldSize / 2, centerY - shieldSize / 2, color);

    // ВНЕШНИЙ ОБОД - толстый и прочный
    float rimSize = shieldSize * 0.9f;
    XMFLOAT4 rimColor = XMFLOAT4(color.x * 0.7f, color.y * 0.7f, color.z * 0.7f, color.w);
    drawRect(rimSize, rimSize, centerX - rimSize / 2, centerY - rimSize / 2, rimColor);

    // ВНУТРЕННЯЯ ЧАСТЬ - выпуклая
    float innerSize = shieldSize * 0.8f;
    XMFLOAT4 innerColor = XMFLOAT4(color.x * 0.9f, color.y * 0.9f, color.z * 0.9f, color.w);
    drawRect(innerSize, innerSize, centerX - innerSize / 2, centerY - innerSize / 2, innerColor);

    // ЦЕНТРАЛЬНЫЙ УМБОН - массивный
    float bossSize = size * 0.4f;
    XMFLOAT4 bossColor = XMFLOAT4(0.8f, 0.6f, 0.3f, color.w);
    drawRect(bossSize, bossSize, centerX - bossSize / 2, centerY - bossSize / 2, bossColor);

    // УКРАШЕНИЯ УМБОНА - концентрические кольца
    float ring1Size = bossSize * 0.7f;
    float ring2Size = bossSize * 0.4f;
    drawRect(ring1Size, ring1Size, centerX - ring1Size / 2, centerY - ring1Size / 2,
        XMFLOAT4(0.9f, 0.7f, 0.4f, color.w));
    drawRect(ring2Size, ring2Size, centerX - ring2Size / 2, centerY - ring2Size / 2,
        XMFLOAT4(1.0f, 0.8f, 0.5f, color.w));

    // ЗАКЛЕПКИ - по кругу
    int rivetCount = 8;
    for (int i = 0; i < rivetCount; i++) {
        float angle = (i / (float)rivetCount) * 2 * PI;
        float rivetDist = shieldSize * 0.35f;
        float rivetX = centerX + cosf(angle) * rivetDist;
        float rivetY = centerY + sinf(angle) * rivetDist;
        float rivetSize = size * 0.05f;
        drawRect(rivetSize, rivetSize, rivetX - rivetSize / 2, rivetY - rivetSize / 2,
            XMFLOAT4(0.6f, 0.6f, 0.7f, color.w));
    }

    // РУЧКА ЩИТА (с обратной стороны)
    float gripWidth = size * 0.15f;
    float gripHeight = size * 0.4f;
    drawRect(gripWidth, gripHeight, centerX - gripWidth / 2, centerY - gripHeight / 2,
        XMFLOAT4(0.3f, 0.3f, 0.3f, color.w));

    // ЭФФЕКТ ЗАЩИТЫ
    if (isSelected) {
        float pulse = 0.5f + 0.5f * sinf(time * 8.0f);

        // Защитное поле
        float fieldSize = shieldSize * 1.5f;
        XMFLOAT4 fieldColor = XMFLOAT4(0.2f, 0.4f, 1.0f, 0.6f * pulse);
        drawRect(fieldSize, fieldSize, centerX - fieldSize / 2, centerY - fieldSize / 2, fieldColor);

        // Концентрические кольца защиты
        for (int ring = 0; ring < 2; ring++) {
            float ringSize = shieldSize * (1.3f + ring * 0.2f);
            XMFLOAT4 ringColor = XMFLOAT4(0.3f, 0.5f, 1.0f, 0.4f * pulse);
            drawRect(ringSize, ringSize, centerX - ringSize / 2, centerY - ringSize / 2, ringColor);
        }

        // Блики на металле
        float shineSize = size * 0.25f;
        float shineX = centerX - size * 0.2f;
        float shineY = centerY - size * 0.2f;
        drawRect(shineSize, shineSize * 0.3f, shineX - shineSize / 2, shineY,
            XMFLOAT4(1.0f, 1.0f, 1.0f, 0.7f * pulse));
    }
}

// УЛЬТРА-ДЕТАЛИЗИРОВАННЫЙ ЛУК С ТЕТИВОЙ
void DrawBowIcon(float x, float y, float size, XMFLOAT4 color, bool isSelected, float time) {
    float centerX = x;
    float centerY = y;

    // ДУГА ЛУКА - плавный изгиб
    float bowWidth = size * 1.5f;
    float bowHeight = size * 0.6f;

    // Рисуем изогнутую форму сегментами
    int segments = 7;
    for (int i = 0; i < segments; i++) {
        float progress = i / (float)(segments - 1);
        float xPos = centerX - bowWidth / 2 + (progress * bowWidth);

        // Синусоидальный изгиб
        float curve = sinf(progress * PI) * bowHeight;

        float segmentSize = size * 0.12f;

        // Верхняя часть дуги
        drawRect(segmentSize, segmentSize, xPos - segmentSize / 2, centerY - curve - segmentSize / 2, color);
        // Нижняя часть дуги
        drawRect(segmentSize, segmentSize, xPos - segmentSize / 2, centerY + curve - segmentSize / 2, color);
    }

    // РУКОЯТЬ - центральная утолщенная часть
    float gripWidth = size * 0.2f;
    float gripHeight = size * 0.35f;
    XMFLOAT4 gripColor = XMFLOAT4(color.x * 0.8f, color.y * 0.8f, color.z * 0.8f, color.w);
    drawRect(gripWidth, gripHeight, centerX - gripWidth / 2, centerY - gripHeight / 2, gripColor);

    // ТЕТИВА - туго натянутая
    float stringWidth = size * 0.02f;
    float stringHeight = bowHeight * 2.2f;
    XMFLOAT4 stringColor = XMFLOAT4(0.9f, 0.9f, 0.9f, color.w);
    drawRect(stringWidth, stringHeight, centerX - stringWidth / 2, centerY - stringHeight / 2, stringColor);

    // УЗЛЫ ТЕТИВЫ на концах
    float knotSize = size * 0.06f;
    drawRect(knotSize, knotSize, centerX - bowWidth / 2 - knotSize / 4, centerY - knotSize / 2,
        XMFLOAT4(0.8f, 0.8f, 0.8f, color.w));
    drawRect(knotSize, knotSize, centerX + bowWidth / 2 - knotSize * 3 / 4, centerY - knotSize / 2,
        XMFLOAT4(0.8f, 0.8f, 0.8f, color.w));

    // ДЕКОРАТИВНЫЕ ЭЛЕМЕНТЫ на концах лука
    float tipDecorSize = size * 0.08f;
    drawRect(tipDecorSize, tipDecorSize, centerX - bowWidth / 2 - tipDecorSize / 3, centerY - bowHeight - tipDecorSize / 3,
        XMFLOAT4(0.7f, 0.5f, 0.3f, color.w));
    drawRect(tipDecorSize, tipDecorSize, centerX + bowWidth / 2 - tipDecorSize * 2 / 3, centerY - bowHeight - tipDecorSize / 3,
        XMFLOAT4(0.7f, 0.5f, 0.3f, color.w));
    drawRect(tipDecorSize, tipDecorSize, centerX - bowWidth / 2 - tipDecorSize / 3, centerY + bowHeight - tipDecorSize * 2 / 3,
        XMFLOAT4(0.7f, 0.5f, 0.3f, color.w));
    drawRect(tipDecorSize, tipDecorSize, centerX + bowWidth / 2 - tipDecorSize * 2 / 3, centerY + bowHeight - tipDecorSize * 2 / 3,
        XMFLOAT4(0.7f, 0.5f, 0.3f, color.w));

    // ЭФФЕКТ НАТЯЖЕНИЯ
    if (isSelected) {
        float pulse = 0.5f + 0.5f * sinf(time * 15.0f);

        // Аура вокруг лука
        float auraSize = size * 1.8f;
        XMFLOAT4 auraColor = XMFLOAT4(color.x, color.y, color.z, 0.5f * pulse);
        drawRect(auraSize, auraSize, centerX - auraSize / 2, centerY - auraSize / 2, auraColor);

        // Эффект натянутой тетивы
        float tensionWidth = size * 0.08f;
        float tensionHeight = stringHeight * 0.8f;
        XMFLOAT4 tensionColor = XMFLOAT4(1.0f, 1.0f, 0.8f, 0.7f * pulse);
        drawRect(tensionWidth, tensionHeight, centerX - tensionWidth / 2, centerY - tensionHeight / 2, tensionColor);

        // Линии силы от тетивы
        for (int i = 0; i < 3; i++) {
            float lineLength = size * 0.8f;
            float lineWidth = size * 0.01f;
            float angle = (i - 1) * 0.3f;
            float lineX = centerX + cosf(angle) * lineLength * 0.5f;
            drawRect(lineLength, lineWidth, centerX - lineLength / 2, centerY + sinf(angle) * 10.0f,
                XMFLOAT4(1.0f, 1.0f, 1.0f, 0.6f * pulse));
        }
    }
}

// ФУНКЦИЯ ОТРИСОВКИ ВСЕХ ИКОНОК
void DrawWeaponIcons(float deltaTime) {
    static float time = 0.0f;
    time += deltaTime;

    // Сохраняем текущее расположение
    float startX = (200.0f / 2560.0f) * window.width;
    float y = (1350.0f / 1440.0f) * window.height;
    float spacing = 100.0f;
    float iconSize = 70.0f;

    // Яркие контрастные цвета
    XMFLOAT4 selectedColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    XMFLOAT4 fistColor = XMFLOAT4(0.2f, 0.4f, 1.0f, 1.0f);      // Синий
    XMFLOAT4 swordColor = XMFLOAT4(1.0f, 0.2f, 0.2f, 1.0f);     // Красный
    XMFLOAT4 shieldColor = XMFLOAT4(1.0f, 0.8f, 0.1f, 1.0f);    // Золотой
    XMFLOAT4 bowColor = XMFLOAT4(0.2f, 0.8f, 0.2f, 1.0f);       // Зеленый

    // Фон панели
    float panelWidth = spacing * 4 + 40.0f;
    float panelHeight = iconSize + 30.0f;
    drawRect(panelWidth, panelHeight, startX - 20.0f, y - 15.0f, XMFLOAT4(0.1f, 0.1f, 0.2f, 0.9f));

    // Иконки с правильными интервалами
    DrawFistIcon(startX, y, iconSize,
        (current_weapon == weapon_name::Fists) ? selectedColor : fistColor,
        (current_weapon == weapon_name::Fists), time);

    DrawSwordIcon(startX + spacing, y, iconSize,
        (current_weapon == weapon_name::Sword) ? selectedColor : swordColor,
        (current_weapon == weapon_name::Sword), time);

    DrawShieldIcon(startX + spacing * 2, y, iconSize,
        (current_weapon == weapon_name::Shield) ? selectedColor : shieldColor,
        (current_weapon == weapon_name::Shield), time);

    DrawBowIcon(startX + spacing * 3, y, iconSize,
        (current_weapon == weapon_name::Bow) ? selectedColor : bowColor,
        (current_weapon == weapon_name::Bow), time);

    // Подписи
    float textY = y + iconSize / 2 + 20.0f;
    drawString("FIST", startX, textY, 0.4f, true);
    drawString("SWORD", startX + spacing, textY, 0.4f, true);
    drawString("SHIELD", startX + spacing * 2, textY, 0.4f, true);
    drawString("BOW", startX + spacing * 3, textY, 0.4f, true);
}

// Обновленная функция drawStaminaBar с добавлением иконок оружий
void drawStaminaBar(float energy) {
    float normalizedEnergy = energy / 100.0f;
    float barWidth = 75.0f;
    float barHeight = 12.0f;
    float x = (1080.0f / 2560.0f) * window.width;
    float y = (1350.0f / 1440.0f) * window.height;
    static float time = 0.011f;
    time += 0.016f;
    float currentWidth = (normalizedEnergy * barWidth);

    // Фон бара
    drawRect(currentWidth + 4, barHeight + 4, x - 2, y - 2, XMFLOAT4(0.05f, 0.05f, 0.15f, 0.9f));

    // Фоновые частицы (на всей ширине фона)
    drawBackgroundParticles(x, y, currentWidth, barHeight, XMFLOAT4(0.3f, 0.3f, 0.6f, 0.4f), 3, time);

    // Рамка
    drawRect(currentWidth + 2, barHeight + 2, x - 1, y - 1, XMFLOAT4(0.2f, 0.2f, 0.4f, 1.0f));

    // Основной бар
    if (currentWidth > 0) {
        XMFLOAT4 barColor;
        if (normalizedEnergy > 0.7f) {
            barColor = XMFLOAT4(0.2f, 0.6f, 1.0f, 1.0f);
        }
        else if (normalizedEnergy > 0.3f) {
            barColor = XMFLOAT4(0.4f, 0.7f, 1.0f, 1.0f);
        }
        else {
            barColor = XMFLOAT4(0.6f, 0.8f, 1.0f, 1.0f);
        }
        drawRect(currentWidth, barHeight, x, y, barColor);

        // Звездные частицы ТОЛЬКО на заполненной части!
        int particleCount = (int)(normalizedEnergy * 8);
        drawFillParticles(x, y, currentWidth * 2.f, barHeight, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), particleCount, time);
    }

    // Отрисовываем иконки оружий
    DrawWeaponIcons(0.016f); // Передаем deltaTime
}


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
    float barWidth = (240.0f / 2560.0f) * window.width;
    float barHeight = (18.0f / 1440.0f) * window.height;
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
    float x = ((180.0f - size / 2) / 2560.0f) * window.width;
    float y = ((1325.0f - size / 2) / 1440.0f) * window.height;
    static float time = 0.011f;
    time += 0.016f;

    drawRect(barWidth, barHeight, x, y, XMFLOAT4(lastCharge, 0, 0, 0), 13, 16);
}

void drawEnemyBar(float enemyHP) {
    Constellation& enemy = *starSet[currentEnemyID];
    float normalizedHP = enemyHP / enemy.maxHP;
    float barWidth = (600.0f / 2560.0f) * window.width;
    float barHeight = (22.0f / 1440.0f) * window.height;
    float x = (950.0f / 2560.0f) * window.width;
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
    if (normalizedHP <= 0.4f) {
        float framePulse = (sinf(time * 10.0f) + 1.0f) * 0.5f;
        drawRect(barWidth + 6, barHeight + 6, x - 3, y - 3,
            XMFLOAT4(1.0f, 0.6f, 0.1f, framePulse));
    }
}

// Обновленная функция drawStaminaBar с добавлением иконок оружий
void drawStaminaBar(float energy) {
    float normalizedEnergy = energy / 100.0f;
    float barWidth = (90.0f / 2560.0f) * window.width;
    float barHeight = (12.0f / 1440.0f) * window.height;
    float x = (250.0f / 2560.0f) * window.width;
    float y = (1350.0f / 1440.0f) * window.height;
    static float time = 0.011f;
    time += 0.016f;

    // Фон бара
    //drawRect(barWidth + 4, barHeight + 4, x - 2, y - 2, XMFLOAT4(0.05f, 0.05f, 0.15f, 0.9f));

    // Фоновые частицы (на всей ширине фона)
    //drawBackgroundParticles(x, y, barWidth, barHeight, XMFLOAT4(0.3f, 0.3f, 0.6f, 0.4f), 3, time);

    // Рамка
    //drawRect(barWidth + 2, barHeight + 2, x - 1, y - 1, XMFLOAT4(0.2f, 0.2f, 0.4f, 1.0f));

    // Основной бар
    float currentWidth = (normalizedEnergy * barWidth);
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
        drawFillParticles(x, y, currentWidth * 1.2f, barHeight, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), particleCount, time);

        // Эффект свечения с частицами
        if (normalizedEnergy > 1.0f) {
            drawRect(currentWidth - 2, barHeight - 2, x + 1, y + 1, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.3f));
            // Интенсивные частицы при максимальной энергии
           // drawFillParticles(x, y, currentWidth, barHeight, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 4, time * 2.0f);
        }
    }
}
// Функция для рисования иконки кулаков из звезд - СУПЕР ДЕТАЛИЗИРОВАННАЯ
// Функция для рисования звездной рамки вокруг иконки
void drawStarFrame(float x, float y, float size, XMFLOAT4 baseColor, bool isActive, float time) {
    float frameSize = size * 1.4f; // Размер рамки больше иконки
    int frameStars = 12; // Уменьшил количество звезд для большей выразительности

    for (int i = 0; i < frameStars; i++) {
        float angle = (float)i / frameStars * PI * 2.0f;
        float distance = frameSize * 0.45f;
        float starX = x + cosf(angle) * distance;
        float starY = y + sinf(angle) * distance;

        // УВЕЛИЧИЛ размер звезд - теперь они значительно больше
        float baseStarSize = size * 0.15f; // Было 0.08f
        float starSize = baseStarSize * (1.0f + 0.4f * sinf(time * 3.0f + i * 1.5f));

        // УСИЛИЛ мерцание звезд
        float starAlpha = 0.9f * (0.6f + 0.4f * sinf(time * 2.5f + i));
        if (isActive) {
            starAlpha *= (1.2f + 0.8f * sinf(time * 5.0f + i)); // Более интенсивное мерцание для активных
            starSize *= (1.1f + 0.3f * sinf(time * 4.0f + i)); // И увеличение размера
        }

        // Более насыщенные цвета
        XMFLOAT4 starColor = XMFLOAT4(
            baseColor.x * (0.8f + 0.4f * sinf(time * 1.8f + i)),
            baseColor.y * (0.8f + 0.4f * sinf(time * 2.2f + i)),
            baseColor.z * (1.0f + 0.3f * sinf(time * 1.5f + i)),
            starAlpha
        );

        drawRect(starSize, starSize, starX - starSize / 2, starY - starSize / 2, starColor, 13, 16);

        // ДОБАВИЛ дополнительное свечение вокруг больших звезд
        if (isActive) {
            float glowSize = starSize * (1.5f + 0.5f * sinf(time * 6.0f + i));
            float glowAlpha = starAlpha * 0.4f;
            drawRect(glowSize, glowSize, starX - glowSize / 2, starY - glowSize / 2,
                XMFLOAT4(starColor.x, starColor.y, starColor.z, glowAlpha * 10.f), 13, 16);
        }
    }

    // УВЕЛИЧИЛ декоративные звезды между основными
    int decorStars = 6;
    for (int i = 0; i < decorStars; i++) {
        float angle = (float)i / decorStars * PI * 2.0f + PI / decorStars;
        float distance = frameSize * 0.42f;
        float starX = x + cosf(angle) * distance;
        float starY = y + sinf(angle) * distance;

        // Большие декоративные звезды
        float decorSize = size * 0.08f * (0.7f + 0.5f * sinf((time) * 4.0f + i)); // Было 0.05f
        float decorAlpha = 0.7f * (0.4f + 0.6f * sinf((time) * 3.5f + i * 2.0f));

        drawRect(decorSize, decorSize, starX - decorSize / 2, starY - decorSize / 2,
            XMFLOAT4(1.0f, 1.0f, 1.0f, decorAlpha*10.f), 13, 16);
    }

    // ДОБАВИЛ внешнее кольцо из очень больших звезд для акцента
    if (isActive) {
        int accentStars = 4;
        for (int i = 0; i < accentStars; i++) {
            float angle = (float)i / accentStars * PI * 2.0f + time * 0.5f;
            float distance = frameSize * 0.5f;
            float starX = x + cosf(angle) * distance;
            float starY = y + sinf(angle) * distance;

            float accentSize = size * 0.2f * (0.8f + 0.6f * sinf(time * 2.0f + i));
            float accentAlpha = 0.6f * (0.5f + 0.5f * sinf(time * 3.0f + i));

            drawRect(accentSize, accentSize, starX - accentSize / 2, starY - accentSize / 2,
                XMFLOAT4(1.0f, 0.9f, 0.7f, accentAlpha), 13, 16);
        }
    }
}

// Функция для рисования иконки кулаков из звезд с рамкой
void drawFistsIcon(float x, float y, float size, XMFLOAT4 baseColor, bool isActive = true) {
    float iconSize = size;
    float centerX = x;
    float centerY = y;

    float alpha = isActive ? 1.0f : 0.4f;
    static float fistTime = 0.0f;
    fistTime += 0.016f;

    // ЗВЕЗДНАЯ РАМКА
    drawStarFrame(centerX, centerY, iconSize, baseColor, isActive, fistTime);

    // ОСНОВНАЯ ФОРМА КУЛАКА - более анатомически правильная
    // Костяшки (четыре основных сустава)
    float knuckleSize = iconSize * 0.2f;
    float knuckleSpacing = iconSize * 0.15f;

    for (int i = 0; i < 4; i++) {
        float knuckleX = centerX - knuckleSpacing * 1.5f + i * knuckleSpacing;
        float knuckleY = centerY;

        float knuckleAlpha = alpha * (0.9f - i * 0.1f);
        drawRect(knuckleSize, knuckleSize, knuckleX - knuckleSize / 2, knuckleY - knuckleSize / 2,
            XMFLOAT4(baseColor.x, baseColor.y, baseColor.z, knuckleAlpha), 13, 16);
    }

    // ПАЛЬЦЫ - три фаланги на каждом пальце
    int fingers = 4;
    float fingerWidth = iconSize * 0.08f;
    float fingerSegmentHeight = iconSize * 0.1f;

    for (int finger = 0; finger < fingers; finger++) {
        float fingerBaseX = centerX - knuckleSpacing * 1.5f + finger * knuckleSpacing;
        float fingerBaseY = centerY - knuckleSize * 0.3f;

        // Три сегмента пальца (фаланги)
        for (int segment = 0; segment < 3; segment++) {
            float segmentY = fingerBaseY - segment * fingerSegmentHeight * 0.8f;
            float segmentAlpha = alpha * (0.8f - segment * 0.2f);

            // Эффект сгиба пальцев
            float bendOffset = 0.0f;
            if (segment > 0 && isActive) {
                bendOffset = sinf(fistTime * 4.0f + finger) * iconSize * 0.02f;
            }

            drawRect(fingerWidth, fingerSegmentHeight,
                fingerBaseX - fingerWidth / 2 + bendOffset, segmentY - fingerSegmentHeight / 2,
                XMFLOAT4(baseColor.x * 0.9f, baseColor.y * 0.9f, baseColor.z, segmentAlpha), 13, 16);
        }

        // НОГОТИ - маленькие звездочки на кончиках пальцев
        float nailSize = fingerWidth * 0.6f;
        float nailY = fingerBaseY - 2.5f * fingerSegmentHeight;
        drawRect(nailSize, nailSize, fingerBaseX - nailSize / 2, nailY - nailSize / 2,
            XMFLOAT4(1.0f, 1.0f, 1.0f, alpha * 0.7f), 13, 16);
    }

    // БОЛЬШОЙ ПАЛЕЦ - отдельная сложная структура
    float thumbBaseX = centerX + iconSize * 0.25f;
    float thumbBaseY = centerY + iconSize * 0.1f;

    // Основание большого пальца
    drawRect(knuckleSize * 1.2f, knuckleSize * 1.2f,
        thumbBaseX - knuckleSize * 0.6f, thumbBaseY - knuckleSize * 0.6f,
        XMFLOAT4(baseColor.x * 0.8f, baseColor.y * 0.8f, baseColor.z, alpha * 0.9f), 13, 16);

    // Две фаланги большого пальца
    float thumbSegmentWidth = iconSize * 0.12f;
    float thumbSegmentHeight = iconSize * 0.15f;

    for (int segment = 0; segment < 2; segment++) {
        float thumbX = thumbBaseX - segment * thumbSegmentWidth * 0.7f;
        float thumbY = thumbBaseY - segment * thumbSegmentHeight * 0.6f;
        float thumbAlpha = alpha * (0.8f - segment * 0.3f);

        drawRect(thumbSegmentWidth, thumbSegmentHeight,
            thumbX - thumbSegmentWidth / 2, thumbY - thumbSegmentHeight / 2,
            XMFLOAT4(baseColor.x * 0.85f, baseColor.y * 0.85f, baseColor.z, thumbAlpha), 13, 16);
    }

    // Ноготь большого пальца
    float thumbNailSize = thumbSegmentWidth * 0.7f;
    float thumbNailX = thumbBaseX - thumbSegmentWidth * 1.2f;
    float thumbNailY = thumbBaseY - thumbSegmentHeight * 1.1f;
    drawRect(thumbNailSize, thumbNailSize, thumbNailX - thumbNailSize / 2, thumbNailY - thumbNailSize / 2,
        XMFLOAT4(1.0f, 1.0f, 1.0f, alpha * 0.6f), 13, 16);

    // ЗАПЯСТЬЕ - основание руки
    float wristWidth = iconSize * 0.4f;
    float wristHeight = iconSize * 0.15f;
    float wristY = centerY + iconSize * 0.25f;
    drawRect(wristWidth, wristHeight, centerX - wristWidth / 2, wristY - wristHeight / 2,
        XMFLOAT4(baseColor.x * 0.7f, baseColor.y * 0.7f, baseColor.z, alpha * 0.8f), 13, 16);
}

// Функция для рисования иконки щита-гоплона с рамкой
void drawShieldIcon(float x, float y, float size, XMFLOAT4 baseColor, bool isActive = true) {
    float iconSize = size;
    float centerX = x;
    float centerY = y;

    float alpha = isActive ? 1.0f : 0.4f;
    static float shieldTime = 0.0f;
    shieldTime += 0.016f;

    // ЗВЕЗДНАЯ РАМКА
    drawStarFrame(centerX, centerY, iconSize, baseColor, isActive, shieldTime);

    // ЦЕНТРАЛЬНЫЙ УМБОН (металлическая бляха) - сложная структура
    float umboSize = iconSize * 0.25f;
    float umboAlpha = alpha * (0.95f + 0.1f * sinf(shieldTime * 4.0f));

    // Основа умбона
    drawRect(umboSize, umboSize, centerX - umboSize / 2, centerY - umboSize / 2,
        XMFLOAT4(0.9f, 0.9f, 1.0f, umboAlpha), 13, 16);

    // Внутренняя часть умбона
    float umboInnerSize = umboSize * 0.7f;
    drawRect(umboInnerSize, umboInnerSize, centerX - umboInnerSize / 2, centerY - umboInnerSize / 2,
        XMFLOAT4(0.8f, 0.8f, 0.9f, umboAlpha * 0.9f), 13, 16);

    // Центральная точка умбона
    float umboCenterSize = umboSize * 0.3f;
    drawRect(umboCenterSize, umboCenterSize, centerX - umboCenterSize / 2, centerY - umboCenterSize / 2,
        XMFLOAT4(1.0f, 1.0f, 1.0f, umboAlpha), 13, 16);

    // УКРЕПЛЯЮЩИЕ РЕБРА щита - 8 основных направлений
    int ribs = 8;
    float ribLength = iconSize * 0.4f;
    float ribWidth = iconSize * 0.04f;

    for (int i = 0; i < ribs; i++) {
        float angle = (float)i / ribs * PI * 2.0f;
        float cosA = cosf(angle);
        float sinA = sinf(angle);

        // Временное решение - рисуем звезды вдоль линии
        int ribSegments = 3;
        for (int seg = 0; seg < ribSegments; seg++) {
            float segPos = (float)(seg + 1) / (ribSegments + 1);
            float segX = centerX + cosA * ribLength * segPos;
            float segY = centerY + sinA * ribLength * segPos;
            float segSize = ribWidth * (1.2f - seg * 0.2f);

            drawRect(segSize, segSize, segX - segSize / 2, segY - segSize / 2,
                XMFLOAT4(0.6f, 0.6f, 0.7f, alpha * (0.8f - seg * 0.2f)), 13, 16);
        }
    }

    // ДЕКОРАТИВНЫЕ ЭЛЕМЕНТЫ - греческие узоры
    int decorations = 12;
    float decorationSize = iconSize * 0.08f;

    for (int i = 0; i < decorations; i++) {
        float angle = (float)i / decorations * PI * 2.0f;
        float decoX = centerX + cosf(angle) * iconSize * 0.35f;
        float decoY = centerY + sinf(angle) * iconSize * 0.35f;
        float decoAlpha = alpha * (0.7f + 0.3f * sinf(shieldTime * 2.0f + i));

        // Чередуем узоры
        if (i % 3 == 0) {
            // Треугольные узоры
            drawRect(decorationSize, decorationSize, decoX - decorationSize / 2, decoY - decorationSize / 2,
                XMFLOAT4(0.9f, 0.8f, 0.6f, decoAlpha), 13, 16);
        }
        else if (i % 3 == 1) {
            // Круговые узоры
            drawRect(decorationSize * 0.8f, decorationSize * 0.8f, decoX - decorationSize * 0.4f, decoY - decorationSize * 0.4f,
                XMFLOAT4(0.8f, 0.7f, 0.5f, decoAlpha), 13, 16);
        }
        else {
            // Ромбовидные узоры
            drawRect(decorationSize * 0.6f, decorationSize * 0.6f, decoX - decorationSize * 0.3f, decoY - decorationSize * 0.3f,
                XMFLOAT4(1.0f, 0.9f, 0.7f, decoAlpha), 13, 16);
        }
    }

    // РУЧКА щита на обратной стороне (видна по краям)
    float handleSize = iconSize * 0.15f;
    float handleX = centerX;
    float handleY = centerY + iconSize * 0.3f;
    drawRect(handleSize, handleSize, handleX - handleSize / 2, handleY - handleSize / 2,
        XMFLOAT4(0.4f, 0.3f, 0.2f, alpha * 0.8f), 13, 16);
}

// Функция для рисования иконки меча с рамкой
void drawSwordIcon(float x, float y, float size, XMFLOAT4 baseColor, bool isActive = true) {
    float iconSize = size;
    float centerX = x;
    float centerY = y;

    float alpha = isActive ? 1.0f : 0.4f;
    static float swordTime = 0.0f;
    swordTime += 0.016f;

    // ЗВЕЗДНАЯ РАМКА
    drawStarFrame(centerX, centerY, iconSize, baseColor, isActive, swordTime);

    // КЛИНОК - детализированный с долами и заточкой
    float bladeWidth = iconSize * 0.15f;
    float bladeHeight = iconSize * 0.7f;
    float bladeX = centerX;
    float bladeY = centerY - iconSize * 0.25f;

    // Основной клинок
    drawRect(bladeWidth, bladeHeight, bladeX - bladeWidth / 2, bladeY - bladeHeight / 2,
        XMFLOAT4(0.95f, 0.95f, 1.0f, alpha * 0.9f), 13, 16);

    // ДОЛЫ (желобки на клинке) - два параллельных
    float fullerWidth = bladeWidth * 0.3f;
    float fullerHeight = bladeHeight * 0.8f;
    float fullerY = bladeY - bladeHeight * 0.1f;

    // Левый дол
    drawRect(fullerWidth, fullerHeight, bladeX - bladeWidth * 0.35f - fullerWidth / 2, fullerY - fullerHeight / 2,
        XMFLOAT4(0.85f, 0.85f, 0.95f, alpha * 0.8f), 13, 16);

    // Правый дол
    drawRect(fullerWidth, fullerHeight, bladeX + bladeWidth * 0.35f - fullerWidth / 2, fullerY - fullerHeight / 2,
        XMFLOAT4(0.85f, 0.85f, 0.95f, alpha * 0.8f), 13, 16);

    // ЛЕЗВИЕ - режущие кромки
    float edgeWidth = bladeWidth * 0.05f;
    float edgeHeight = bladeHeight * 0.9f;
    float edgeY = bladeY - bladeHeight * 0.05f;

    // Левая кромка
    drawRect(edgeWidth, edgeHeight, bladeX - bladeWidth * 0.45f - edgeWidth / 2, edgeY - edgeHeight / 2,
        XMFLOAT4(1.0f, 1.0f, 1.0f, alpha * 0.7f), 13, 16);

    // Правая кромка
    drawRect(edgeWidth, edgeHeight, bladeX + bladeWidth * 0.45f - edgeWidth / 2, edgeY - edgeHeight / 2,
        XMFLOAT4(1.0f, 1.0f, 1.0f, alpha * 0.7f), 13, 16);

    // ОСТРИЕ клинка
    float tipHeight = bladeHeight * 0.15f;
    float tipY = bladeY - bladeHeight * 0.5f + tipHeight * 0.5f;
    drawRect(bladeWidth, tipHeight, bladeX - bladeWidth / 2, tipY - tipHeight / 2,
        XMFLOAT4(1.0f, 1.0f, 1.0f, alpha), 13, 16);

    // ГАРДА (перекрестие) - сложная структура
    float guardWidth = iconSize * 0.5f;
    float guardHeight = iconSize * 0.08f;
    float guardY = centerY + iconSize * 0.15f;

    // Основная гарда
    drawRect(guardWidth, guardHeight, centerX - guardWidth / 2, guardY - guardHeight / 2,
        XMFLOAT4(0.5f, 0.4f, 0.3f, alpha * 0.9f), 13, 16);

    // Украшения на гарде
    float guardDecorationSize = guardHeight * 1.2f;
    // Левое украшение
    drawRect(guardDecorationSize, guardDecorationSize,
        centerX - guardWidth * 0.4f - guardDecorationSize / 2, guardY - guardDecorationSize / 2,
        XMFLOAT4(0.7f, 0.6f, 0.4f, alpha), 13, 16);
    // Правое украшение
    drawRect(guardDecorationSize, guardDecorationSize,
        centerX + guardWidth * 0.4f - guardDecorationSize / 2, guardY - guardDecorationSize / 2,
        XMFLOAT4(0.7f, 0.6f, 0.4f, alpha), 13, 16);

    // РУКОЯТЬ - с текстурой обмотки
    float gripWidth = iconSize * 0.12f;
    float gripHeight = iconSize * 0.25f;
    float gripY = centerY + iconSize * 0.3f;

    // Основа рукояти
    drawRect(gripWidth, gripHeight, centerX - gripWidth / 2, gripY - gripHeight / 2,
        XMFLOAT4(0.4f, 0.3f, 0.2f, alpha * 0.9f), 13, 16);

    // Обмотка рукояти - полосы
    int wrapSegments = 5;
    float wrapSegmentHeight = gripHeight / wrapSegments;
    for (int i = 0; i < wrapSegments; i++) {
        float wrapY = gripY - gripHeight * 0.5f + (i + 0.5f) * wrapSegmentHeight;
        float wrapAlpha = alpha * (0.7f + 0.3f * (i % 2));

        drawRect(gripWidth * 1.1f, wrapSegmentHeight * 0.6f,
            centerX - gripWidth * 0.55f, wrapY - wrapSegmentHeight * 0.3f,
            XMFLOAT4(0.3f, 0.2f, 0.1f, wrapAlpha), 13, 16);
    }

    // НАВЕРШИЕ (поммель) - декоративное
    float pommelSize = iconSize * 0.2f;
    float pommelY = gripY + gripHeight * 0.5f + pommelSize * 0.3f;

    // Основное навершие
    drawRect(pommelSize, pommelSize, centerX - pommelSize / 2, pommelY - pommelSize / 2,
        XMFLOAT4(0.6f, 0.5f, 0.3f, alpha), 13, 16);

    // Декор навершия
    float pommelDecorationSize = pommelSize * 0.5f;
    drawRect(pommelDecorationSize, pommelDecorationSize,
        centerX - pommelDecorationSize / 2, pommelY - pommelDecorationSize / 2,
        XMFLOAT4(0.8f, 0.7f, 0.5f, alpha * 0.9f), 13, 16);
}

// Функция для рисования иконки лука с рамкой
void drawBowIcon(float x, float y, float size, XMFLOAT4 baseColor, bool isActive = true) {
    float iconSize = size;
    float centerX = x;
    float centerY = y;

    float alpha = isActive ? 1.0f : 0.4f;
    static float bowTime = 0.0f;
    bowTime += 0.016f;

    // ЗВЕЗДНАЯ РАМКА
    drawStarFrame(centerX, centerY, iconSize, baseColor, isActive, bowTime);

    // ДУГА ЛУКА - сложная изогнутая форма
    int bowSegments = 9;
    float bowRadius = iconSize * 0.4f;
    float bowWidth = iconSize * 0.08f;

    for (int i = 0; i < bowSegments; i++) {
        float angle = PI * 0.8f + (float)i / (bowSegments - 1) * PI * 0.6f;
        float segmentX = centerX + cosf(angle) * bowRadius;
        float segmentY = centerY + sinf(angle) * bowRadius;
        float segmentSize = bowWidth * (1.0f - 0.3f * fabs((float)i / (bowSegments - 1) - 0.5f));

        // Градиент цвета по длине лука
        float segmentAlpha = alpha * (0.7f + 0.3f * (float)i / bowSegments);
        XMFLOAT4 segmentColor = XMFLOAT4(
            baseColor.x * 0.8f - 0.2f * (float)i / bowSegments,
            baseColor.y * 0.7f - 0.2f * (float)i / bowSegments,
            baseColor.z * 0.3f,
            segmentAlpha
        );

        drawRect(segmentSize, segmentSize, segmentX - segmentSize / 2, segmentY - segmentSize / 2,
            segmentColor, 13, 16);
    }

    // РУКОЯТЬ лука - центральная часть
    float gripSize = iconSize * 0.15f;
    drawRect(gripSize, gripSize, centerX - gripSize / 2, centerY - gripSize / 2,
        XMFLOAT4(0.3f, 0.2f, 0.1f, alpha * 0.9f), 13, 16);

    // ТЕТИВА - натянутая струна
    int stringSegments = 5;
    float stringSegmentSize = iconSize * 0.03f;

    for (int i = 0; i < stringSegments; i++) {
        float stringPos = (float)i / (stringSegments - 1);
        float stringX = centerX;
        float stringY = centerY - bowRadius * 0.3f + i * bowRadius * 0.15f;
        float stringAlpha = alpha * (0.8f - 0.3f * fabs(stringPos - 0.5f));

        drawRect(stringSegmentSize, stringSegmentSize, stringX - stringSegmentSize / 2, stringY - stringSegmentSize / 2,
            XMFLOAT4(0.9f, 0.9f, 0.9f, stringAlpha), 13, 16);
    }

    // НАКОНЕЧНИКИ лука - роговые/костяные
    float tipSize = iconSize * 0.1f;
    // Левый наконечник
    float leftTipX = centerX - bowRadius * 0.9f;
    float leftTipY = centerY;
    drawRect(tipSize, tipSize, leftTipX - tipSize / 2, leftTipY - tipSize / 2,
        XMFLOAT4(0.8f, 0.7f, 0.6f, alpha), 13, 16);
    // Правый наконечник
    float rightTipX = centerX + bowRadius * 0.9f;
    float rightTipY = centerY;
    drawRect(tipSize, tipSize, rightTipX - tipSize / 2, rightTipY - tipSize / 2,
        XMFLOAT4(0.8f, 0.7f, 0.6f, alpha), 13, 16);

    float arrowOffset = fmod(bowTime * 3.0f, 1.0f) - 0.5f;
    float arrowLength = iconSize * 0.8f;
    float arrowShaftWidth = iconSize * 0.03f;
    float arrowHeadSize = iconSize * 0.08f;
    float arrowFeatherSize = iconSize * 0.06f;
    float arrowBaseX = centerX - arrowLength * 0.3f + arrowOffset * iconSize * 0.1f;
    float arrowBaseY = centerY;

    // СТРЕЛА - полностью детализированная
    if (isActive) {
        drawRect(arrowLength, arrowShaftWidth, arrowBaseX - arrowLength / 2, arrowBaseY - arrowShaftWidth / 2,
            XMFLOAT4(0.9f, 0.8f, 0.6f, alpha * 0.9f), 13, 16);

        // НАКОНЕЧНИК стрелы
        float arrowHeadX = arrowBaseX + arrowLength * 0.35f;
        drawRect(arrowHeadSize, arrowHeadSize, arrowHeadX - arrowHeadSize / 2, arrowBaseY - arrowHeadSize / 2,
            XMFLOAT4(0.7f, 0.7f, 0.8f, alpha), 13, 16);

        // ОПЕРЕНИЕ - три пера
        float featherX = arrowBaseX - arrowLength * 0.4f;
        for (int feather = 0; feather < 3; feather++) {
            float featherY = arrowBaseY - arrowFeatherSize * 0.8f + feather * arrowFeatherSize * 0.8f;
            float featherAlpha = alpha * (0.8f - feather * 0.2f);

            drawRect(arrowFeatherSize, arrowFeatherSize, featherX - arrowFeatherSize / 2, featherY - arrowFeatherSize / 2,
                XMFLOAT4(1.0f, 1.0f, 1.0f, featherAlpha), 13, 16);
        }
    }
}

// Обновленная функция отрисовки панели оружий
void drawWeaponPanel(int currentWeapon, float stamina) {
    float panelX = (2200.0f / 2560.0f) * window.width;
    float panelY = (1200.0f / 1440.0f) * window.height;
    float iconSize = (150.0f / 2560.0f) * window.width;
    float spacing = (250.0f / 2560.0f) * window.width;

    // Иконки оружий
    XMFLOAT4 weaponColor = XMFLOAT4(0.8f, 0.8f, 1.0f, 1.0f);

    // Кулаки (0)
    drawFistsIcon(panelX , panelY + iconSize / 2 - spacing * 0.5f, iconSize, weaponColor, currentWeapon == 0);
    drawString("Fists", panelX, panelY + iconSize / 2 - spacing * 0.2f,1.f,true);
    // Меч (1)  
    drawSwordIcon(panelX , panelY + iconSize / 2 - spacing * 1.5f, iconSize, weaponColor, currentWeapon == 1);
    drawString("Sword", panelX, panelY + iconSize / 2 - spacing * 1.2f, 1.f, true);
    // Щит (2)
    drawShieldIcon(panelX , panelY + iconSize / 2 - spacing * 2.5f, iconSize, weaponColor, currentWeapon == 2);
    drawString("Shield", panelX, panelY + iconSize / 2 - spacing * 2.2f, 1.f, true);
    // Лук (3)
    drawBowIcon(panelX, panelY + iconSize / 2 - spacing * 3.5f, iconSize, weaponColor, currentWeapon == 3);
    drawString("Bow", panelX, panelY + iconSize / 2 - spacing * 3.2f, 1.f, true);
}

void DiologUI() {

    float panelHEROX = (1900.0f / 2560.0f) * window.width;
    float panelHEROY = (1200.0f / 1440.0f) * window.height;
    drawString("ASPID", panelHEROX, panelHEROY,2.f,true);

    float panelZenithX = (500.0f / 2560.0f) * window.width;
    float panelZenithY = (1200.0f / 1440.0f) * window.height;

    drawString("ZENITH", panelZenithX, panelZenithY, 2.f, true);
}
void drawRect(float width, float height, float x, float y, XMFLOAT4 color)
{
    Shaders::vShader(13); // ID прямоугольного шейдера
    Shaders::pShader(13);
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
        float alpha = 0.3f + 0.7f * (0.5f + 0.5f * sinf(time * 2.0f + seed * 5.0f));

        XMFLOAT4 particleColor = XMFLOAT4(
            baseColor.x * (0.8f + 0.4f * fmod(seed * 13.7f, 1.0f)),
            baseColor.y * (0.8f + 0.4f * fmod(seed * 17.3f, 1.0f)),
            baseColor.z * (0.8f + 0.4f * fmod(seed * 19.1f, 1.0f)),
            baseColor.w * alpha * 0.3f // Более прозрачные для фона
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

void drawStaminaBar(float energy) {
    float normalizedEnergy = energy / 100.0f;
    float barWidth = 75.0f;
    float barHeight = 12.0f;
    float x = (1080.0f / 2560.0f) * window.width;
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
        drawFillParticles(x, y, currentWidth*2.f, barHeight, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), particleCount, time);

        // Эффект свечения с частицами
        if (normalizedEnergy > 1.0f) {
            drawRect(currentWidth - 2, barHeight - 2, x + 1, y + 1, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.3f));
            // Интенсивные частицы при максимальной энергии
           // drawFillParticles(x, y, currentWidth, barHeight, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 4, time * 2.0f);
        }
    }
}

void drawHPBar(float HP) {
    float maxHP = 6.0f;
    float normalizedHP = HP / maxHP;
    float barWidth = 180.0f;
    float barHeight = 18.0f;
    float x = (1080.0f / 2560.0f) * window.width;
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
        drawFillParticles(x, y, currentWidth*2.f, barHeight, particleColor, particleCount, time);

        // Пульсация с частицами при низком HP
        if (normalizedHP < 0.3f) {
            float pulse = (sinf(time * 8.0f) + 1.0f) * 0.5f;
            drawRect(currentWidth, barHeight, x, y, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.2f * pulse));

            // Красные тревожные частицы только на заполненной части
            drawFillParticles(x, y, currentWidth, barHeight, XMFLOAT4(1.0f, 0.3f, 0.3f, pulse), 4, time * 3.0f);
        }
    }
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
    drawRect(barWidth + 8, barHeight + 8, x - 4, y - 4, XMFLOAT4(0.08f, 0.02f, 0.1f, 0.6f));

    // Фоновые частицы (на всей ширине фона) - звездная пыль
    drawBackgroundParticles(x, y, barWidth, barHeight, XMFLOAT4(0.9f, 0.7f, 0.3f, 0.4f), 10, time);

    // Рамка вражеского бара с градиентом от золотого к темному
    drawRect(barWidth + 6, barHeight + 6, x - 3, y - 3, XMFLOAT4(0.6f, 0.4f, 0.1f, .8f));

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
            color = XMFLOAT4(0.6f + t * 0.4f, 0.3f + t * 0.2f, 0.1f, 1.0f);
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

            // Случайные вспышки по длине бара
            /*for (int i = 0; i < 2; i++) {
                float flashPos = fmod(time * 3.0f + i * 2.0f, currentWidth);
                float flashWidth = 8.0f + 4.0f * sinf(time * 4.0f + i);
                float flashAlpha = 0.3f + 0.4f * pulse;

                drawRect(flashWidth, barHeight - 2, x + flashPos, y + 1,
                    XMFLOAT4(1.0f, 0.6f, 0.2f, flashAlpha));
            }*/

            // Интенсивные частицы - последние вспышки звезды
            //drawFillParticles(x, y, currentWidth, barHeight, XMFLOAT4(1.0f, 0.8f, 0.3f, pulse), 8, time * 5.0f);
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
void drawRect(float width, float height, float x, float y, XMFLOAT4 color)
{
    Shaders::vShader(13); // ID прямоугольного шейдера
    Shaders::pShader(13);
    Blend::Blending(Blend::blendmode::alpha, Blend::blendop::add);

    ConstBuf::global[0] = XMFLOAT4(width, height, 0, 0);
    ConstBuf::global[1] = XMFLOAT4((float)window.width, (float)window.height, 0, 0);
    ConstBuf::global[2] = XMFLOAT4(x, y, 0, 0);
    ConstBuf::global[3] = color; // Передаем цвет

    ConstBuf::Update(5, ConstBuf::global);
    ConstBuf::ConstToVertex(5);
    ConstBuf::ConstToPixel(5); // Не забудь передать в пиксельный шейдер
    context->DrawInstanced(6, 1, 0, 0);
}

void drawStaminaBar(float energy) {

    drawRect(energy / 2, 10.0f, (1080. / 2560) * window.width, (1265. / 1440) * window.height - 50.f, XMFLOAT4(0.5f, 0.5f, 1.0f, 1.0f)); // прямоугольник шириной 200px и высотой 100px по центру экрана, на пикселях 1000 по абсциссе и 800 по ординате
    drawRect(150, 1.0f, (1080. / 2560) * window.width, (1240./ 1440) * window.height - 25.f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
};

void drawHPBar(float HP) {
  
    XMFLOAT4 color;
    if (HP > 4.f) {
        color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f); // Зеленый
    }
    else if (HP > 2.f) {
        color = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f); // Желтый
    }
    else {
        color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f); // Красный
    }

    drawRect(HP * 30, 10.0f, (1080. / 2560) * window.width, (1300. / 1440) * window.height - 50.f, color);

    drawRect(180, 1.0f, (1080. / 2560) * window.width, (1267. / 1440) * window.height - 25.f, XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
    drawRect(180, 1.0f, (1080. / 2560) * window.width, (1300. / 1440) * window.height - 25.f, XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
};
void drawEnemyBar(float enemyHP) {
    // Нормализуем HP врага от 0 до 1
    Constellation& enemy = *starSet[currentEnemyID];
    float normalizedHP = enemyHP / enemy.maxHP;

    
    XMFLOAT4 color;
    if (normalizedHP > 0.6f) {
        color = XMFLOAT4(1.0f, 0.3f, 0.3f, 1.0f); // Светло-красный
    }
    else if (normalizedHP > 0.3f) {
        color = XMFLOAT4(1.0f, 0.6f, 0.0f, 1.0f); // Оранжевый
    }
    else {
        color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f); // Красный
    }

    // Рисуем бар врага
    drawRect(normalizedHP * 400, 10.0f, (880. / 2560) * window.width, (150. / 1440) * window.height, color);

    // Рамка
    drawRect(400, 1.0f, (880. / 2560) * window.width, (150. / 1440) * window.height + 25.f,
        XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
}

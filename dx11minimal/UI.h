void drawRect(float width, float height, float x, float y)
{
    Shaders::vShader(13); // ID �������������� �������
    Shaders::pShader(13);

    //memset(ConstBuf::global, 0, sizeof(ConstBuf::global));


    ConstBuf::global[0] = XMFLOAT4(width, height, 0, 0);
    ConstBuf::global[1] = XMFLOAT4((float)window.width, (float)window.height, 0, 0);
    ConstBuf::global[2] = XMFLOAT4(x, y, 0, 0);


    ConstBuf::Update(5, ConstBuf::global);
    ConstBuf::ConstToVertex(5);


    context->DrawInstanced(6, 1, 0, 0);

    //Draw::NullDrawer(6, 1); // 6 ������ = ���� �������������
}

void drawStaminaBar(float energy) {

    drawRect(energy / 2, 10.0f, (1700. / 2560) * window.width, (1100. / 1440) * window.height - 50.f); // ������������� ������� 200px � ������� 100px �� ������ ������, �� �������� 1000 �� �������� � 800 �� ��������
    drawRect(150, 1.0f, (1700. / 2560) * window.width, (1100. / 1440) * window.height - 25.f);
};
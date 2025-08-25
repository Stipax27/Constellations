void drawRect(float width, float height, float x, float y)
{
    ConstBuf::global[0] = XMFLOAT4(width, height, 0, 0);
    ConstBuf::global[1] = XMFLOAT4((float)window.width, (float)window.height, 0, 0);
    ConstBuf::global[2] = XMFLOAT4(x, y, 0, 0);


    ConstBuf::Update(5, ConstBuf::global);
    ConstBuf::ConstToVertex(5);

    Shaders::vShader(13); // ID прямоугольного шейдера
    Shaders::pShader(13);

    Draw::NullDrawer(6, 1); // 6 вершин = один прямоугольник
}

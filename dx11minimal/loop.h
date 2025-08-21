bool Camerainit = false;

void mainLoop(float deltaTime)
{
	frameConst();

	InputAssembler::IA(InputAssembler::topology::triList);
	Blend::Blending(Blend::blendmode::alpha, Blend::blendop::add);
	Depth::Depth(Depth::depthmode::off);
	Rasterizer::Cull(Rasterizer::cullmode::off);

	Textures::RenderTarget(3, 0);
	Shaders::vShader(10);
	Shaders::pShader(200);
	Draw::Clear({ 0.0f, 0.0f, 0.0f, 1.0f });
	context->Draw(6, 0);
	Textures::CreateMipMap();

	Textures::RenderTarget(1, 0);
	context->VSSetShaderResources(0, 1, &Textures::Texture[3].TextureResView);
	Depth::Depth(Depth::depthmode::on);

	Shaders::vShader(0);
	Shaders::pShader(0);
	ConstBuf::ConstToVertex(4);
	ConstBuf::ConstToPixel(4);
	if (!Camerainit)
	{
		Camerainit = true;
		Camera::Camera();//��� �������� ��� ����� ����� ������������ (�� ���� � ����������� �����(dx11minimal.cpp) ����� ���������(��� ������))
	}
	Camera::Camera();//добавили общий вызов обновления камеры, чтобы везде не коллить

	Draw::Clear({ 0.0f, 0.0588f, 0.1176f, 1.0f });
	Draw::ClearDepth();

	drawer::drawWorld(deltaTime);

	//--------------------------------
	Textures::CreateMipMap();

	//Draw::SwitchRenderTextures();
	Draw::OutputRenderTextures();

	Blend::Blending(Blend::blendmode::off, Blend::blendop::add);
	Depth::Depth(Depth::depthmode::off);
	Rasterizer::Cull(Rasterizer::cullmode::off);

	/*InputAssembler::IA(InputAssembler::topology::triList);
	Blend::Blending(Blend::blendmode::alpha, Blend::blendop::add);
	Textures::RenderTarget(0, 0);*/

	Shaders::vShader(10);
	Shaders::pShader(10);
	context->Draw(6, 0);

	/*Shaders::pShader(100);
	Draw::OutputRenderTextures();
	context->Draw(6, 0);*/
	//--------------------------------

	Draw::Present();
}
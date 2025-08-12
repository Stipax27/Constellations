bool Camerainit = false;

void mainLoop(float deltaTime)
{
	frameConst();

	InputAssembler::IA(InputAssembler::topology::triList);
	Blend::Blending(Blend::blendmode::alpha, Blend::blendop::add);
	Textures::RenderTarget(0, 0);

	Depth::Depth(Depth::depthmode::on);
	Rasterizer::Cull(Rasterizer::cullmode::off);
	Shaders::vShader(0);
	Shaders::pShader(0);
	ConstBuf::ConstToVertex(4);
	ConstBuf::ConstToPixel(4);
	if (!Camerainit)
	{
		Camerainit = true;
		Camera::Camera();//��� �������� ��� ����� ����� ������������ (�� ���� � ����������� �����(dx11minimal.cpp) ����� ���������(��� ������))
	}

	drawer::drawWorld(deltaTime);

	//--------------------------------
	//Textures::CreateMipMap();

	////Draw::SwitchRenderTextures();
	//Draw::OutputRenderTextures();

	//Blend::Blending(Blend::blendmode::off, Blend::blendop::add);
	//Depth::Depth(Depth::depthmode::off);
	//Rasterizer::Cull(Rasterizer::cullmode::off);

	///*InputAssembler::IA(InputAssembler::topology::triList);
	//Blend::Blending(Blend::blendmode::alpha, Blend::blendop::add);
	//Textures::RenderTarget(0, 0);*/

	//Shaders::vShader(10);
	//Shaders::pShader(10);
	//context->Draw(6, 0);

	////Draw::OutputRenderTextures();
	////Draw::NullDrawer(1, 1);
	//--------------------------------

	Draw::Present();
}
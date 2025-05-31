void mainLoop()
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
	Camera::Camera();


	drawer::drawWorld();
	/*
		InputAssembler::IA(InputAssembler::topology::triList);
		Blend::Blending(Blend::blendmode::alpha, Blend::blendop::add);
		Textures::RenderTarget(0, 0);

		Draw::Clear({ 0.0f, 0.0588f, 0.1176f, 1.0f });
		Draw::ClearDepth();
		Depth::Depth(Depth::depthmode::on);
		Rasterizer::Cull(Rasterizer::cullmode::off);
		Shaders::vShader(0);
		Shaders::pShader(0);
		ConstBuf::ConstToVertex(4);
		ConstBuf::ConstToPixel(4);
		Camera::Camera();*/

	Draw::Present();
}
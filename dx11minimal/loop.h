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

	drawer::drawWorld();

	//--------------------------------
	Textures::CreateMipMap();

	Draw::SwitchRenderTextures();

	Blend::Blending(Blend::blendmode::off, Blend::blendop::add);
	Depth::Depth(Depth::depthmode::off);
	Rasterizer::Cull(Rasterizer::cullmode::off);

	Shaders::vShader(10);
	Shaders::pShader(10);
	Draw::NullDrawer(1, 1);

	Draw::OutputRenderTextures();
	//Shaders::pShader(3);
	Draw::NullDrawer(1, 1);
	//--------------------------------

	Draw::Present();
}
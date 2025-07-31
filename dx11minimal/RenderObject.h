#ifndef _RENDER_OBJECT_H_
#define _RENDER_OBJECT_H_


struct RenderObject {
	int vs_id;
	int ps_id;
	float renderDistance;
	int lodCount;

	int vertexes;
	int instances;

	Blend::blendmode blendmode;
	Blend::blendop blendop;

	RenderObject(int VS_Id = 0, int PS_Id = 0, float RenderDistance = 100000.0f, int LodCount = 4, int Vertexes = 6, int Instances = 1,
		Blend::blendmode Blendmode = Blend::blendmode::on, Blend::blendop Blendop = Blend::blendop::add)

		: vs_id(VS_Id), ps_id(PS_Id), renderDistance(RenderDistance), lodCount(LodCount), vertexes(Vertexes), instances(Instances),
		blendmode(Blendmode), blendop(Blendop)

	{
	}

	void UpdateBuffers();
};

#endif
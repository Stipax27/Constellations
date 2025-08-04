#ifndef _RENDER_OBJECT_H_
#define _RENDER_OBJECT_H_


struct RenderObject {
	int vs_id;
	int ps_id;
	float renderDistance;
	float lodDistance;
	int lodCount;

	float lodDistanceStep;

	int vertexes;
	int instances;

	Blend::blendmode blendmode;
	Blend::blendop blendop;

	point3d pos;

	RenderObject(int VS_Id = 0, int PS_Id = 0, float RenderDistance = 100000.0f, int LodCount = 4, float LodDistance = 50000.0f, int Vertexes = 6, int Instances = 1,
		Blend::blendmode Blendmode = Blend::blendmode::on, Blend::blendop Blendop = Blend::blendop::add, point3d Pos = point3d())

		: vs_id(VS_Id), ps_id(PS_Id), renderDistance(RenderDistance), lodCount(LodCount), lodDistance(LodDistance), vertexes(Vertexes), instances(Instances),
		blendmode(Blendmode), blendop(Blendop), pos(Pos)

	{
		lodDistanceStep = min(renderDistance, lodDistance) / LodCount;
	}

	void UpdateBuffers();
};

#endif
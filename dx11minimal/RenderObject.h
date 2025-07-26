#ifndef _RENDER_OBJECT_H_
#define _RENDER_OBJECT_H_


struct RenderObject {
	int vs_id;
	int ps_id;
	float renderDistance;
	int lodCount;

	RenderObject(int VS_Id = 0, int PS_Id = 0, int LodCount = 4, float RenderDistance = 100000.0f)
		: vs_id(VS_Id), ps_id(PS_Id), lodCount(LodCount), renderDistance(RenderDistance)
	{
	}

	void UpdateBuffers();
};

#endif
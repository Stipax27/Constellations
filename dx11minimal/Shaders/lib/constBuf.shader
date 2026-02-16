#define constCount 1024


cbuffer objParams : register(b0)
{
    float drawerV[constCount];
};

cbuffer objParams : register(b1)
{
    float drawerP[constCount];
};

cbuffer drawMat : register(b2)
{
    float4x4 modelMat;
    float hilight;
};

cbuffer camera : register(b3)
{
    float4x4 world;
    float4x4 view;
    float4x4 proj;
    float4 cPos;
};

cbuffer frame : register(b4)
{
    float4 time;
    float4 aspect;
};

cbuffer global : register(b5)
{
    float4 gConst[constCount];
};

cbuffer factors : register(b6)
{
    float AriesNebulaLerpFactor;
};

cbuffer drawerInt : register(b7)
{
    int drawInt[constCount];
}

cbuffer drawerMatrix : register(b8)
{
    float4x4 model[constCount];
}

cbuffer particlesDesc : register(b9)
{
	float2 pSize;
	float2 pOpacity;
	float3 pColor;
	float pLifetime;
	float2 pSpeed;
    float pTimescale;
};

cbuffer drawerFloat4x4 : register(b10)
{
    float4x4 fConst[constCount];
};

cbuffer locationDesc : register(b11)
{
    float4x4 lModel;
    int lgX;
    int lgY;
    int lMode;
    int lSkipper;
    float4 lBase_color;
}
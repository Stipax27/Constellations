float DEG_TO_RAD = 0.0174699591855f;
float MAX = 10000.0f;

int   GALAXY_FIELD_VOXEL_STEPS = 10;
float GALAXY_FIELD_VOXEL_STEP_SIZE = 250000.0f; // 2,500,000 AL
float GALAXY_RADIUS = 0.015f;  // (% of 250000)  50,000 AL

int   STAR_FIELD_VOXEL_STEPS = 13;
float STAR_FIELD_VOXEL_STEP_SIZE = 0.5f;  // 5AL 
float STAR_RADIUS = 0.01f; // 2e-8 in true life !   // (% of 5)   1e-8


float kU2G = 500000.0f;
float kG2U = 0.000002f;


float fract(float x)
{
    return x - floor(x);
}

float2 fract(float2 x)
{
    return x - floor(x);
}

float3 fract(float3 x)
{
    return x - floor(x);
}

float4 fract(float4 x)
{
    return x - floor(x);
}


// Time spend traveling to clicked point
#define TRAVEL_DELAY 4.0f

#define IN_UNIVERSE     1.0f
#define IN_GALAXY       2.0f
#define IN_SOLAR_SYSTEM 3.0f

#define MOVING     1.0f
#define STATIONARY 2.0f

#define NONE     0.0f
#define GALAXY   1.0f
#define STAR     2.0f
#define PLANET   3.0f
    
    
#define HASHSCALE1 0.1031f
float hash(float3 p3) {
	p3  = fract(p3 * HASHSCALE1);
    p3 += dot(p3, p3.yzx + 19.19f);
    return fract((p3.x + p3.y) * p3.z);
}


#define HASHSCALE3 float3(0.1031f, 0.1030f, 0.0973f)
float3 hash33(float3 p3){
	p3 = fract(p3 * HASHSCALE3);
    p3 += dot(p3, p3.yxz+19.19f);
    return fract(float3((p3.x + p3.y)*p3.z, (p3.x+p3.z)*p3.y, (p3.y+p3.z)*p3.x));
}

//--------------------------------------------------------------------

// Centr? sur le centre de la galaxie
// Echelle 1 pour la grille des etoiles
float3 universeToGalaxy(float3 galaxyPosU, float3 coord) {
    return (coord-galaxyPosU)*kU2G;
}

#define R(p, a) p=cos(a)*p+sin(a)*vec2(p.y, -p.x)

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    uint   starID : COLOR0;
    float4 worldpos : POSITION1;
};

VS_OUTPUT VS( out vec4 fragColor, in vec2 fragCoord ) {

    VS_OUTPUT output;
		
    if (fragCoord.y > 0.5f || fragCoord.x > 10.0f) discard;
            vec2 uv = iMouse.xy / iResolution.xy;
        vec2 p = -1.0f + 2.0f * uv;
        p.x *= iResolution.x/iResolution.y;

// - Initialisation si besoin -------------------------  
    if(iFrame < 10) { 
        
        output.rd_cam = normalize(float3(1.0f, 0.0f, 0.0f));
 
        float3 pos = floor(float3(10.0f));
        float3 id = hash33(pos),
        offset = clamp(id, GALAXY_RADIUS, 1.0f - GALAXY_RADIUS);
        output.ro_cam = pos + offset- 0.03f * output.rd_cam;
        float3 mov = 0.03f * output.rd_cam;
        float3 center = output.ro_cam + mov; 
        R(mov.yz, 1.0f);
        R(mov.xy, 1.0f);
        output.ro_cam = center - mov;
        output.rd_cam = normalize(mov);
        
        output.ro_from = output.ro_cam;
        output.rd_from = output.rd_cam;
        output.ro_to = output.ro_cam;
        output.rd_to = output.rd_cam;
        output.galaxy_pos = float3(0);
        output.target_pos = float3(1);

    } else {
        

// - Est t on dans une galaxie ? --------------------------------     
        float3 galaxyId, galaxyPosU;


// - Generate new Configuration ----------------------------

        output.ro_cam = ro;
        output.rd_cam = rd_cam;

        if (isU) {
            // On vient de rentrer dans une galaxie, on change de coordonnes pour garder la precision
            output = universeToGalaxy(galaxyPosU, output);
        }

    }
    
    
// - Save new Configuration -----------------------------------
    fragColor = float4(0.0f);
    //saveConfig(output, fragColor, fragCoord);

    return output;
}
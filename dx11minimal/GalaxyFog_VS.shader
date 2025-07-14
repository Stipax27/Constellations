float DEG_TO_RAD = 0.0174699591855f;
float MAX = 10000.0f;

int   GALAXY_FIELD_VOXEL_STEPS = 10;
float GALAXY_FIELD_VOXEL_STEP_SIZE = 250000.0f; // 2,500,000 AL
float GALAXY_RADIUS = 0.015f;  // (% of 250000)  50,000 AL

int   STAR_FIELD_VOXEL_STEPS = 13;
float STAR_FIELD_VOXEL_STEP_SIZE = 0.5f;  // 5AL 
float STAR_RADIUS = 0.01f; // 2e-8 in true life !   // (% of 5)   1e-8


float time;


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

#define IN_UNIVERSE 1.0f
#define IN_GALAXY 2.0f
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

  
float isInside( float2 p, float2 c ) { float2 d = abs(p - 0.5f - c) - .5; return -max(d.x,d.y); }
void store( in float2 re, in float4 va, inout float4 fragColor, in float2 fragCoord) {
    fragColor = ( isInside(fragCoord, re) > 0. ) ? va : fragColor;
}
void store( in float2 re, in float3 va, inout float4 fragColor, in float2 fragCoord) {
    fragColor = ( isInside(fragCoord,re) > 0.0f ) ? float4(va, 0.0f) : fragColor;
}
//--------------------------------------------------------------------

// Centr? sur le centre de la galaxie
// Echelle 1 pour la grille des etoiles
float3 universeToGalaxy(float3 galaxyPosU, float3 coord) {
    return (coord-galaxyPosU)*kU2G;
}

Config universeToGalaxy(float3 galaxyPosU, Config cfg) {
	cfg.coordSystem = IN_GALAXY;
    cfg.galaxy_pos = galaxyPosU;
    cfg.ro_cam =  universeToGalaxy(galaxyPosU, cfg.ro_cam);
    cfg.ro_from = universeToGalaxy(galaxyPosU, cfg.ro_from);
    cfg.ro_to =   universeToGalaxy(galaxyPosU, cfg.ro_to);
    cfg.target_pos = universeToGalaxy(galaxyPosU, cfg.target_pos);
	return cfg;
}

#define R(p, a) p=cos(a)*p+sin(a)*vec2(p.y, -p.x)

void VS( out vec4 fragColor, in vec2 fragCoord ) {
		
    if (fragCoord.y > 0.5f || fragCoord.x > 10.0f) discard;
            vec2 uv = iMouse.xy / iResolution.xy;
        vec2 p = -1.0f + 2.0f * uv;
        p.x *= iResolution.x/iResolution.y;

// - Initialisation si besoin -------------------------
    Config cfg;    
    if(iFrame < 10) { 
        
        cfg.rd_cam = normalize(float3(1.0f,0.0f,0.0f));
 
        float3 pos = floor(float3(10.0f));
        float3 id = hash33(pos),
        offset = clamp(id, GALAXY_RADIUS, 1.0f - GALAXY_RADIUS);
        cfg.ro_cam = pos + offset- 0.03f * cfg.rd_cam;
        float3 mov = 0.03f * cfg.rd_cam;
        float3 center = cfg.ro_cam + mov; 
        R(mov.yz, 1.0f);
        R(mov.xy, 1.0f);
        cfg.ro_cam = center - mov;
        cfg.rd_cam = normalize(mov);
        
        cfg.movingMode = STATIONARY;
        cfg.targetType = GALAXY;
        cfg.coordSystem = IN_UNIVERSE;
        cfg.time = iTime;
        cfg.ro_from = cfg.ro_cam;
        cfg.rd_from = cfg.rd_cam;
        cfg.ro_to = cfg.ro_cam;
        cfg.rd_to = cfg.rd_cam;
        cfg.galaxy_pos = float3(0);
        cfg.target_pos = float3(1);

    } else {

// - Lecture de la configuration -----------------------

		bool isU = (cfg.coordSystem == IN_UNIVERSE),
			 isG = (cfg.coordSystem == IN_GALAXY);

        float time = iTime - cfg.time;
        

// - Est t on dans une galaxie ? --------------------------------     
        float3 galaxyId, galaxyPosU;


// - Generate new Configuration ----------------------------

        cfg.ro_cam = ro;
        cfg.rd_cam = rd_cam;

        if (isU) {
            // On vient de rentrer dans une galaxie, on change de coordonnes pour garder la precision
            cfg = universeToGalaxy(galaxyPosU, cfg);
        }
          
        if (iTime - cfg.time > TRAVEL_DELAY+1.) {
            cfg.movingMode = STATIONARY;            
            cfg.time = iTime;  
        }

    }
    
    
// - Save new Configuration -----------------------------------
    fragColor = float4(0.0f);
    saveConfig(cfg, fragColor, fragCoord);
}
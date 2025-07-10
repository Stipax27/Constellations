// in progress
//#define WITH_PLANETS  


const float PI = 3.14159265359;
const float DEG_TO_RAD = (PI / 180.0);
const float MAX = 10000.0;

// Unit = 10 UA
const int   GALAXY_FIELD_VOXEL_STEPS = 10;
const float GALAXY_FIELD_VOXEL_STEP_SIZE = 250000.; // 2,500,000 AL
const float GALAXY_RADIUS = .015;  // (% of 250000)  50,000 AL

const int   STAR_FIELD_VOXEL_STEPS = 13;
const float STAR_FIELD_VOXEL_STEP_SIZE = .5;  // 5AL 
const float STAR_RADIUS = .01; // 2e-8 in true life !   // (% of 5)   1e-8

const float PLANET_FIELD_SCALE = 75.;
const int   PLANET_FIELD_VOXEL_STEPS = 10;
const float PLANET_FIELD_VOXEL_STEP_SIZE = .5;  // 5AL 
const float PLANET_RADIUS = .04; 


const float kU2G = GALAXY_FIELD_VOXEL_STEP_SIZE/STAR_FIELD_VOXEL_STEP_SIZE;
const float kG2U = STAR_FIELD_VOXEL_STEP_SIZE/GALAXY_FIELD_VOXEL_STEP_SIZE;




float time;

float keyPress(int ascii) {
	return texture(iChannel2,vec2((.5+float(ascii))/256.,0.25)).x ;
}





// Time spend traveling to clicked point
#define TRAVEL_DELAY 4.

#define IN_UNIVERSE 1.
#define IN_GALAXY 2.
#define IN_SOLAR_SYSTEM 3.

#define MOVING     1.
#define STATIONARY 2.

#define NONE     0.
#define GALAXY   1.
#define STAR     2.
#define PLANET   3.

struct Config {
 	float movingMode;
    float targetType;
    float coordSystem;
    float time;
    vec3 ro_cam;
    vec3 rd_cam;
    vec3 target_pos;
    vec3 galaxy_pos;
    vec3 ro_from;
    vec3 ro_to;
    vec3 rd_from;
    vec3 rd_to;
};
    
    
#define HASHSCALE1 .1031
float hash(vec3 p3) {
	p3  = fract(p3 * HASHSCALE1);
    p3 += dot(p3, p3.yzx + 19.19);
    return fract((p3.x + p3.y) * p3.z);
}

/*
vec3 hash33( const in vec3 p) {
    return fract(vec3(
        sin(dot(p,    vec3(127.1,311.7,758.5453123))),
        sin(dot(p.zyx,vec3(127.1,311.7,758.5453123))),
        sin(dot(p.yxz,vec3(127.1,311.7,758.5453123))))*43758.5453123);
}
*/

#define HASHSCALE3 vec3(.1031, .1030, .0973)
//#define HASHSCALE4 vec4(1031, .1030, .0973, .1099)
vec3 hash33(vec3 p3){
	p3 = fract(p3 * HASHSCALE3);
    p3 += dot(p3, p3.yxz+19.19);
    return fract(vec3((p3.x + p3.y)*p3.z, (p3.x+p3.z)*p3.y, (p3.y+p3.z)*p3.x));
}

float distanceRayPoint(vec3 ro, vec3 rd, vec3 p, out float h) {
    h = dot(p-ro,rd);
    return length(p-ro-rd*h);
}

// Toujours en coordonnes Univers
bool renderGalaxyField( in vec3 roU, in vec3 rd, out vec3 out_posU, out vec3 out_id) { 
    float d, dint;
    vec3 ros = roU + rd*d,   
		pos = floor(ros),
		ri = 1./rd,
		rs = sign(rd),
		dis = (pos-ros + 0.5 + rs*0.5) * ri,
		offset, id, galaxyro;
    
    float pitch = 10./iResolution.x;
    
	for( int i=0; i<GALAXY_FIELD_VOXEL_STEPS; i++ ) {
        id = hash33(pos);
        offset = clamp(id, GALAXY_RADIUS, 1.-GALAXY_RADIUS);
        
        // Si on intersectionne avec la boundingbox (sphere)
        d = distanceRayPoint(ros, rd, pos+offset, dint);
        if( dint > 0. && d < GALAXY_RADIUS*.5+dint*pitch ) {
            galaxyro = pos+offset;
            out_posU = galaxyro;
        	out_id = id;
        	return true;	    
        }
        
		vec3 mm = step(dis.xyz, dis.yxy) * step(dis.xyz, dis.zzx);
		dis += mm * rs * ri;
        pos += mm * rs;
	}
    
	return false;
}



// Toujours en coordonnes Galaxy
bool renderStarField(vec3 galaxyId, in vec3 roG, in vec3 rd, out vec3 out_posG, out vec3 out_id) { 
    out_id = vec3(9);

    float d, dint;
    vec3 ros = roG + rd*d,
	 	pos = floor(ros),
		ri = 1./rd,
		rs = sign(rd),
		dis = (pos-ros + 0.5 + rs*0.5) * ri,	
		mm, offset = vec3(0.), id, galaxyro;

    float pitch = 10./iResolution.x;
    
	for( int i=0; i<STAR_FIELD_VOXEL_STEPS; i++ ) {
        id = hash33(pos);
        offset = clamp(id, STAR_RADIUS, 1.-STAR_RADIUS);
        d = distanceRayPoint(ros, rd, pos+offset, dint);
        if (dint > 0. && d < STAR_RADIUS*.5+dint*pitch) {
	        out_posG = pos+offset;
   	     	out_id = id;
        	return true;
        }
		mm = step(dis.xyz, dis.yxy) * step(dis.xyz, dis.zzx);
		dis += mm * rs * ri;
        pos += mm * rs;
	}

	return false;
}

  
float isInside( vec2 p, vec2 c ) { vec2 d = abs(p-.5-c) - .5; return -max(d.x,d.y); }
void store( in vec2 re, in vec4 va, inout vec4 fragColor, in vec2 fragCoord) {
    fragColor = ( isInside(fragCoord,re) > 0. ) ? va : fragColor;
}
void store( in vec2 re, in vec3 va, inout vec4 fragColor, in vec2 fragCoord) {
    fragColor = ( isInside(fragCoord,re) > 0. ) ? vec4(va,0.) : fragColor;
}
//--------------------------------------------------------------------


void saveConfig(Config cfg, inout vec4 c, in vec2 f) {
	store(vec2(0.,0.), vec4(cfg.movingMode, cfg.targetType, cfg.coordSystem, cfg.time), c, f);
	store(vec2(1.,0.), cfg.ro_cam,    c, f);
	store(vec2(2.,0.), cfg.rd_cam,    c, f);
	store(vec2(3.,0.), cfg.target_pos,c, f);
    store(vec2(4.,0.), cfg.galaxy_pos,c, f);
	store(vec2(5.,0.), cfg.ro_from,   c, f);
    store(vec2(6.,0.), cfg.ro_to,     c, f);
    store(vec2(7.,0.), cfg.rd_from,   c, f);
    store(vec2(8.,0.), cfg.rd_to,     c, f);
}

#define CONF(id)  texture(iChannel0, vec2(id+.5,.5)/ iChannelResolution[0].xy, -100.0).xyz;
#define CONF4(id) texture(iChannel0, vec2(id+.5,.5)/ iChannelResolution[0].xy, -100.0);
Config getConfig() { 
    vec4 v1        = CONF4(0.);
    Config cfg;
    cfg.movingMode = v1.x > 1.5 ? STATIONARY : 
                     MOVING;
    cfg.targetType = v1.y > 2.5 ? PLANET : 
    				 v1.y > 1.5 ? STAR:
    				 v1.y > 0.5 ? GALAXY:
    				 NONE;
    cfg.coordSystem =v1.z > 2.5 ? IN_SOLAR_SYSTEM :
    				 v1.z > 1.5 ? IN_GALAXY :				
                     IN_UNIVERSE;
    cfg.time = v1.w;
    cfg.ro_cam     = CONF(1.);
    cfg.rd_cam     = CONF(2.);
    cfg.target_pos = CONF(3.);
    cfg.galaxy_pos = CONF(4.);
    cfg.ro_from    = CONF(5.);
    cfg.ro_to      = CONF(6.);
    cfg.rd_from    = CONF(7.);
    cfg.rd_to      = CONF(8.);
    return cfg;
}


bool isInGalaxy(in vec3 roU, out vec3 out_GalaxyId, out vec3 out_GalaxyPosU) {
    vec3 pos = floor(roU);
    out_GalaxyId = hash33(pos);
    
    vec3 offset = clamp(out_GalaxyId, GALAXY_RADIUS, 1.-GALAXY_RADIUS);
    out_GalaxyPosU = (pos+offset);
    
    return length(roU - out_GalaxyPosU) < GALAXY_RADIUS;
}

// Echelle 1 pour la grille des galaxies 
vec3 galaxyToUniverse(vec3 galaxyPosU, vec3 coord) {
    return coord*kG2U + galaxyPosU;
}

// Centré sur le centre de la galaxie
// Echelle 1 pour la grille des etoiles
vec3 universeToGalaxy(vec3 galaxyPosU, vec3 coord) {
    return (coord-galaxyPosU)*kU2G;
}

Config galaxyToUniverse(vec3 galaxyPosU, Config cfg) {
	cfg.coordSystem = IN_UNIVERSE;
    cfg.galaxy_pos = galaxyPosU;
    cfg.ro_cam =  galaxyToUniverse(galaxyPosU, cfg.ro_cam);
    cfg.ro_from = galaxyToUniverse(galaxyPosU, cfg.ro_from);
    cfg.ro_to =   galaxyToUniverse(galaxyPosU, cfg.ro_to);
    cfg.target_pos = galaxyToUniverse(galaxyPosU, cfg.target_pos);
    return cfg;
}

Config universeToGalaxy(vec3 galaxyPosU, Config cfg) {
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
		
    if (fragCoord.y > 0.5 || fragCoord.x > 10.) discard;
            vec2 uv = iMouse.xy / iResolution.xy;
        vec2 p = -1.0 + 2.0 * uv;
        p.x *= iResolution.x/iResolution.y;

// - Initialisation si besoin -------------------------
    Config cfg;    
    if(iFrame < 10) { 
        
        cfg.rd_cam = normalize(vec3(1.,0.,0.));
 
        vec3 pos = floor(vec3(10.));
        vec3 id = hash33(pos),
        offset = clamp(id, GALAXY_RADIUS, 1.-GALAXY_RADIUS);
        cfg.ro_cam = pos + offset-.03*cfg.rd_cam;
        vec3 mov = .03*cfg.rd_cam;
        vec3 center = cfg.ro_cam + mov; 
        R(mov.yz, 1.);
        R(mov.xy, 1.);
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
        cfg.galaxy_pos = vec3(0);
        cfg.target_pos = vec3(1);

    } else {

// - Lecture de la configuration -----------------------
        cfg = getConfig();

		bool isU = (cfg.coordSystem == IN_UNIVERSE),
			 isG = (cfg.coordSystem == IN_GALAXY);

        float time = iTime - cfg.time;
        

// - Est t on dans une galaxie ? --------------------------------     
        vec3 galaxyId, galaxyPosU;
        bool inGalaxy = isInGalaxy(isU ? ro : galaxyToUniverse(cfg.galaxy_pos, ro), galaxyId, galaxyPosU);

// - Recherche des clicks sur les objets
        vec3 targetPosU, targetPosG, targetId;
     
        // - Click Galaxy Field ------------------------------------------

        bool isHitGalaxy = false, isHitStar = false, isHitPlanet = false;

        // Le calcul se fait toujours en coordonnees univers
        vec3 roU = isG ? galaxyToUniverse(cfg.galaxy_pos, ro) : ro;
        isHitGalaxy = renderGalaxyField( roU, rd, targetPosU, targetId);
        if (isG && length(roU - targetPosU) > 3.) isHitGalaxy = false;
        if (isHitGalaxy) {
            targetPosG = universeToGalaxy(cfg.galaxy_pos,targetPosU);    
        }
// - Click Star Field ------------------------------------------

        if (isG && !isHitGalaxy) {
            // Le calcul se fait toujours en coordonnees Galaxie       
            vec3 roG = ro;    
            isHitStar = renderStarField(galaxyId, roG, rd, targetPosG, targetId );
			if (isHitStar) targetPosU = galaxyToUniverse(cfg.galaxy_pos,targetPosG);
        }
        


// - Generate new Configuration ----------------------------

        cfg.ro_cam = ro;
        cfg.rd_cam = rd_cam;

        // On est en mode attente
        if (cfg.movingMode == STATIONARY) { // stationary
            
            // click en cours
            bool isClick = (iMouse.z != 0. && abs(iMouse.z - iMouse.x) < 3. && abs(iMouse.w - iMouse.y) <3.)
                           || (keyPress(32) >.5);

            if (isClick) {
                // On va declancher un mouvement vers le point cliqué
				cfg.targetType = 
                    	isHitGalaxy ? GALAXY :
                		isHitStar ? STAR :
                		isHitPlanet ? PLANET : NONE;
                cfg.ro_from = ro;
                cfg.ro_to = 
                    isHitGalaxy ? (isU ? targetPosU-(cfg.target_pos==vec3(1)||targetPosU==cfg.target_pos?0.:.03)*rd : targetPosG-.03*rd*kU2G) : // vers la cible
                    isHitStar ? targetPosG-.03*rd :
                	isHitPlanet ? targetPosG-.06*rd/PLANET_FIELD_SCALE :
                    ro + 3.*rd; // 3 unitees dans la direction du click

                cfg.rd_from = rd_cam;
                cfg.rd_to = rd;
                cfg.movingMode = MOVING;            
                cfg.time = iTime;
                cfg.target_pos = isU ? targetPosU : targetPosG;
                //((isHitGalaxy && targetId!=cfg.target_id)) ? targetId : isHitStar ? vec3(.5) : isHitPlanet ? vec3(9.) : vec3(0.);
            }
            
        } else { 
          // En mouvement vers une cible
            if (isU && inGalaxy) {
                // On vient de rentrer dans une galaxie, on change de coordonnes pour garder la precision
                cfg = universeToGalaxy(galaxyPosU, cfg);
                
            } else if (isG && !inGalaxy) {
                if (length(ro)*kG2U > GALAXY_RADIUS*3.) {
                // On vient de sortir de la galaxy, on change de systeme de coordonnes pour garder la precision   
       				cfg = galaxyToUniverse(galaxyPosU, cfg);
                }
            } 
          
            if (iTime - cfg.time > TRAVEL_DELAY+1.) {
                cfg.movingMode = STATIONARY;            
                cfg.time = iTime;  
            }
        }

    }
    
    
// - Save new Configuration -----------------------------------
    fragColor = vec4(0.);
    saveConfig(cfg, fragColor, fragCoord);
}
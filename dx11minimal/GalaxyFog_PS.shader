#define WITH_INTERGALACTIC_CLOUDS

#define R(p, a) p=cos(a)*p+sin(a)*vec2(p.y, -p.x)


#define SPIRAL_NOISE_ITER 6

const float PI = 3.14159265359;
const float DEG_TO_RAD = (PI / 180.0);

// Unit = 10 UA
const int   GALAXY_FIELD_VOXEL_STEPS = 16;
const int   GALAXY_FIELD_VOXEL_STEPS_HD = 7;
const float GALAXY_FIELD_VOXEL_STEP_SIZE = 250000.; // 2,500,000 AL
const float GALAXY_RADIUS = .015;  // (% of 250000)  50,000 AL

const int   STAR_FIELD_VOXEL_STEPS = 22;
const float STAR_FIELD_VOXEL_STEP_SIZE = .5;  // 5AL 
const float STAR_RADIUS = .01; // 2e-8 in true life !   // (% of 5)   1e-8

const float PLANET_FIELD_SCALE = 75.;
const int   PLANET_FIELD_VOXEL_STEPS = 10;
const float PLANET_FIELD_VOXEL_STEP_SIZE = .5;  // 5AL 
const float PLANET_RADIUS = .04;


const float kU2G = GALAXY_FIELD_VOXEL_STEP_SIZE/STAR_FIELD_VOXEL_STEP_SIZE;
const float kG2U = STAR_FIELD_VOXEL_STEP_SIZE/GALAXY_FIELD_VOXEL_STEP_SIZE;

const vec3  SUN_COLOR = vec3(.3, .21, .165);

float time;


//-----------------------------------------------------


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
};


#define CONF(id)  texture(iChannel0, vec2(id+.5,.5)/ iChannelResolution[0].xy, -100.0).xyz;
#define CONF4(id) texture(iChannel0, vec2(id+.5,.5)/ iChannelResolution[0].xy, -100.0);

Config getConfig() { 
    vec4 v1 = CONF4(0.);
    Config cfg;
    
    cfg.movingMode = v1.x > 1.5 ? STATIONARY : 
                     MOVING;
    cfg.targetType = v1.y > 2.5 ? PLANET : 
    				 v1.y > 1.5 ? STAR:
    				 v1.y > 0.5 ? GALAXY:
    				 NONE;
    cfg.coordSystem = v1.z > 2.5 ? IN_SOLAR_SYSTEM :
    				  v1.z > 1.5 ? IN_GALAXY :				
                      IN_UNIVERSE;
    cfg.time = v1.w;
    cfg.ro_cam = CONF(1.);
    cfg.rd_cam = CONF(2.);
    cfg.target_pos = CONF(3.);
    cfg.galaxy_pos = CONF(4.);
    return cfg;
}

//-----------------------------------------------------
// Paletes functions
//-----------------------------------------------------

// from FabriceNeyret2 shader Black Body Spectrum plank [https://www.shadertoy.com/view/4tdGWM]
vec3 blackBodyColor(float k) {
    float T = (k*2.)*16000.;
    vec3 c = vec3(1.,3.375,8.)/(exp((19e3*vec3(1.,1.5,2.)/T)) - 1.); // Planck law
    return c / max(c.r,max(c.g,c.b));  // chrominance
}

//-----------------------------------------------------
// Noise functions
//----------------------------------------------------------------------------------------

#define HASHSCALE3 vec3(.1031, .1030, .0973)
#define HASHSCALE4 vec4(1031, .1030, .0973, .1099)
#define HASHSCALE1 .1031

float hash(float p)
{
	vec3 p3  = fract(vec3(p) * HASHSCALE1);
    p3 += dot(p3, p3.yzx + 19.19);
    return fract((p3.x + p3.y) * p3.z);
}

//----------------------------------------------------------------------------------------
//  1 out, 2 in...
float hash(vec2 p)
{
	vec3 p3  = fract(vec3(p.xyx) * HASHSCALE1);
    p3 += dot(p3, p3.yzx + 19.19);
    return fract((p3.x + p3.y) * p3.z);
}

//----------------------------------------------------------------------------------------
//  1 out, 3 in...
float hash(vec3 p3)
{
	p3  = fract(p3 * HASHSCALE1);
    p3 += dot(p3, p3.yzx + 19.19);
    return fract((p3.x + p3.y) * p3.z);
}

vec4 hash4(const in vec4 n) { return fract(sin(n)*1399763.5453123); }


vec3 hash33(vec3 p3) {
	p3 = fract(p3 * HASHSCALE3);
    p3 += dot(p3, p3.yxz+19.19);
    return fract(vec3((p3.x + p3.y)*p3.z, (p3.x+p3.z)*p3.y, (p3.y+p3.z)*p3.x));
}

vec4 hash43(vec3 p) {
	vec4 p4 = fract(vec4(p.xyzx)  * HASHSCALE4);
    p4 += dot(p4, p4.wzxy+19.19);
	return fract(vec4((p4.x + p4.y)*p4.z, (p4.x + p4.z)*p4.y, (p4.y + p4.z)*p4.w, (p4.z + p4.w)*p4.x));
}

//----------------------------------------------------------------------------------------


float noise(const in vec3 x) {
    vec3 p = floor(x), f = fract(x);
	f *= f*(3.-f-f);
	vec2 uv = (p.xy+vec2(37.,17.)*p.z) + f.xy,
	     rg = textureLod( iChannel1, (uv+.5)/256., -100.).yx;
	return mix(rg.x, rg.y, f.z);
}

float pn(const in vec3 x) {
    vec3 p = floor(x), f = fract(x);
	f *= f*(3.-f-f);
	vec2 uv = (p.xy+vec2(37.,17.)*p.z) + f.xy,
	     rg = textureLod( iChannel1, (uv+.5)/256., -100.).yx;
	return 2.4*mix(rg.x, rg.y, f.z)-1.;
}

float bm(const in vec3 x) {
    vec3 p = floor(x), f = fract(x);
	f *= f*(3.-f-f);
	vec2 uv = (p.xy+vec2(37.,17.)*p.z) + f.xy,
	     rg = textureLod( iChannel1, (uv+ .5)/256., -100.).yx;
	return 1.-.82*mix(rg.x, rg.y, f.z);
}

float fpn(const in vec3 p) { 
    return pn(p*.06125)*.5 + 
           pn(p*.125)*.25 + 
           pn(p*.25)*.125;// + pn(p*.5)*.625;
}

float fbm(const in vec3 p) {
   return bm(p*.06125)*.5 + 
          bm(p*.125)*.25 + 
          bm(p*.25)*.125 + 
          bm(p*.4)*.2;
}

const mat3 msun = mat3(0., .8, .6, -.8, .36, -.48, -.6, -.48, .64);

float smoothNoise(in vec3 q){
	float f  = .5000*noise(q); q=msun*q*2.01;
          f += .2500*noise(q); q=msun*q*2.02;
          f += .1250*noise(q); q=msun*q*2.03;
          f += .0625*noise(q);
	return f;
}


// This spiral noise works by successively adding and rotating sin waves while increasing frequency.
// It should work the same on all computers since it's not based on a hash function like some other noises.
// It can be much faster than other noise functions if you're ok with some repetition.
const float nudge = 20.;	// size of perpendicular vector
float normalizer = 1.0 / sqrt(1.0 + nudge*nudge);	// pythagorean theorem on that perpendicular to maintain scale
float SpiralNoiseC(vec3 p, vec4 id) {
    float iter = 2., n = 2.-id.x; // noise amount
    for (int i = 0; i < SPIRAL_NOISE_ITER; i++) {
        n += -abs(sin(p.y*iter) + cos(p.x*iter)) / iter; // add sin and cos scaled inverse with the frequency (abs for a ridged look)
        p.xy += vec2(p.y, -p.x) * nudge; // rotate by adding perpendicular and scaling down
        p.xy *= normalizer;
        p.xz += vec2(p.z, -p.x) * nudge; // rotate on other axis
        p.xz *= normalizer;  
        iter *= id.y + .733733;          // increase the frequency
    }
    return n;
}

float mapIntergalacticCloud(vec3 p, vec4 id) {
	float k = 2.*id.w +.1;  // p/=k;
    return k*(.5 + SpiralNoiseC(p.zxy*.4132+333., id)*3. + pn(p*8.5)*.12);
}


//-----------------------------------------------------
// Intersection functions (mainly adapted from iq ones)
//-----------------------------------------------------

bool intersectSphere(in vec3 ro, in vec3 rd, in float r, out float dist, out float edge) {
	float b = dot(rd,-ro), d = b*b - dot(ro,ro) + r*r;
	if (d < 0.) return false;
    edge = sqrt(d);
	dist = b - edge;
	return dist > 0.;
}

bool cylinder(vec3 ro, vec3 rd, float r, float h, out float tn, out float tf) {
	float a = dot(rd.xy, rd.xy), b = dot(ro.xy, rd.xy),
		  d = b*b - a*(dot(ro.xy, ro.xy) - r*r);    
	if(d < 0.) return false;
	d = sqrt(d);
	tn = (-b - d)/a; tf = (-b + d)/a;
	a = min(tf, tn); tf = max(tf, tn); tn = a; // order roots
	a = ro.z + tn * rd.z;
    b = ro.z + tf * rd.z;
	// top, bottom
	vec2 zcap = h*vec2(.5, -.5), cap = (zcap - ro.z) / rd.z;
    tn = a < zcap.y ? cap.y : a > zcap.x ? cap.x : tn;
	tf = b < zcap.y ? cap.y : b > zcap.x ? cap.x : tf;
    return tf > 0. && tf > tn;
}

float distanceRayPoint(vec3 ro, vec3 rd, vec3 p, out float h) {
    h = dot(p-ro,rd);
    return length(p-ro-rd*h);
	//return length(cross(p-ro,rd));
}


// +------------------------------------------------------+
// |                 Star ray effect                      |
// +------------------------------------------------------+

 
float noise4q(vec4 x) {
	vec4 n3 = vec4(0,.25,.5,.75);
	vec4 p2 = floor(x.wwww+n3);
	vec4 b = floor(x.xxxx+n3) + floor(x.yyyy+n3)*157. + floor(x.zzzz +n3)*113.;
	vec4 p1 = b + fract(p2*.00390625)*vec4(164352., -164352., 163840., -163840.);
	p2 = b + fract((p2+1.0)*.00390625)*vec4(164352., -164352., 163840., -163840.);
	vec4 f1 = fract(x.xxxx+n3),f2 = fract(x.yyyy+n3);
	f1 *= f1*(3.0-f1-f1);
	f2 *= f2*(3.0-f2-f2);
	vec4 n1 = vec4(0,1.,157.,158.), 
         n2 = vec4(113.,114.,270.0,271.);	
	vec4 vs1 = mix(hash4(p1), hash4(n1.yyyy+p1), f1),
         vs2 = mix(hash4(n1.zzzz+p1), hash4(n1.wwww+p1), f1),
         vs3 = mix(hash4(p2), hash4(n1.yyyy+p2), f1),
         vs4 = mix(hash4(n1.zzzz+p2), hash4(n1.wwww+p2), f1);	
	vs1 = mix(vs1, vs2, f2);
	vs3 = mix(vs3, vs4, f2);
	vs2 = mix(hash4(n2.xxxx+p1), hash4(n2.yyyy+p1), f1);
	vs4 = mix(hash4(n2.zzzz+p1), hash4(n2.wwww+p1), f1);
	vs2 = mix(vs2, vs4, f2);
	vs4 = mix(hash4(n2.xxxx+p2), hash4(n2.yyyy+p2), f1);
	vec4 vs5 = mix(hash4(n2.zzzz+p2), hash4(n2.wwww+p2), f1);
	vs4 = mix(vs4, vs5, f2);
	f1 = fract(x.zzzz+n3);
	f2 = fract(x.wwww+n3);
	f1 *= f1*(3.-f1-f1);
	f2 *= f2*(3.-f2-f2);
	vs1 = mix(vs1, vs2, f1);
	vs3 = mix(vs3, vs4, f1);
	vs1 = mix(vs1, vs3, f2);
	float r = dot(vs1,vec4(.25));
	return r*r*(3.-r-r);
}

// rays of a star
float ringRayNoise(vec3 ray, vec3 pos, float r, float size, float anim) {
  	float b = dot(ray,pos);
    vec3 pr = ray*b-pos;
    float c = length(pr),
     	  s = max(0.,(1.-size*abs(r-c)));
    pr = pr/c;
    float n=.4, ns=1., nd = noise4q(vec4(pr*1.0,-anim+c))*2.0;
    if (c > r) {
        n  = noise4q(vec4(pr*10.0,-anim+c));
        ns = noise4q(vec4(pr*50.0,-anim*2.5+ c+c))*2.;
    }
    n *= n*nd*nd*ns;
    return s*s*(s*s+n);
}


// +------------------------------------------------------+
// |                  Sun Lava effect                     |
// +------------------------------------------------------+

vec3 getSunColor(in vec3 p, in vec4 id, in float time) {
    float lava = smoothNoise((p+vec3(time*.03))*50.*(.5+id.z) );
    return blackBodyColor(.02+3.*clamp(id.x*id.x,.05,1.)*(1. - sqrt(lava)));
}

vec4 renderSun(in vec3 ro, in vec3 rd, in vec4 id, in float time) {
	// Rotate view to integrate sun rotation 
    // R(ro.zx, 1.6-time*.5*id.w);
    // R(rd.zx, 1.6-time*.5*id.w);
    vec4 color = vec4(0.);
    float dist, edge;   
    if (intersectSphere(ro, rd, 1., dist, edge)) {
        vec3  pos = ro+rd*dist,
        	  nor = normalize(pos);
    	color = vec4(getSunColor(pos, id, time), smoothstep(0., .2, edge));
    }
    // Rays
    float s3 = ringRayNoise(rd,ro,1.0,5.-4.*id.y,time);
	color.a = max(color.a, clamp(s3,0.,.98));
    color.rgb += blackBodyColor(id.x)*s3;
    color.rgb *=  1.-.03*cos(5.*time+2.*hash(time));  // twinkling;
	return clamp(color, vec4(0.),vec4(1.));
}


// +----------------------------------------+
// |               Galaxy                   |
// +----------------------------------------+

float spiralArm(in vec3 p, in float thickness, in float blurAmout, in float blurStyle) {
    float dephase = 2., loop = 4.;
    float a = atan(p.x,p.z),  // angle     
		  r = length(p.xz), lr = log(r), // distance to center
    	  th = .1-.25*r, // thickness according to distance
    	  d = fract(.5*(a-lr*loop)/PI); //apply rotation and scaling.
    d = (.5/dephase - abs(d-.5))*2.*PI*r;
  	d *= (1.-lr)/thickness;  // space fct of distance
    // Perturb distance fiel1
    float radialBlur = blurAmout*mix(fpn(8.*vec3(r*43.,40.*d,24.*p.y)),fpn(p*400.0), blurStyle);
    return sqrt(d*d+10.*p.y*p.y/thickness)-th*r*.2-radialBlur;
}

void galaxyTransForm(inout vec3 ro, const in vec4 id ) {
    R(ro.yz, (id.y-.5));
 // R(ro.xy, .25*id.x*iTime);
}

float mapGalaxy(vec3 p, vec4 id) {  
	float d1 = spiralArm(p.xzy*.2, 10.*(.5+.5*id.x), .2+.3*id.y, id.z);
#ifdef WITH_DOUBLE_GALAXY
    if (id.z<.25) {
   		float d2 = spiralArm(vec3(-p.y,p.z,p.x)*.205, 10.*(.5+.5*id.x), .2+.3*id.y, id.z);        
   		return min(d2, d1);
     } 
#endif    
     return d1;
}

// assign color to the media
vec3 computeColor( float density, float radius ) {
	// color based on density alone, gives impression of occlusion within the media
	return  mix(vec3(.25,.22,.2), vec3(.1,.0375,.025), density )*
	        mix(vec3(4.8,6.,6.),  vec3(.96,1.06,1.), min((radius+.5)*.5, 1.15)); // color added for disk
}

vec4 renderGalaxy(in vec3 ro, in vec3 rd, in vec4 id, in bool fast) { 
	vec4 col, sum = vec4(0);
    float min_dist=0., max_dist=100.;
 
    galaxyTransForm(ro,id);
    galaxyTransForm(rd,id);
    
    if (cylinder(ro, rd, 3.,3.5, min_dist, max_dist)) {
        float ld, td=0., d, t;
		float lDist, lDist2;
        vec3 pos, ldst, ldst2, lightColor;
        const float h = .1;

        t = max(min_dist,t) + .2*hash(rd+iTime);;

        // raymarch loop
        for (int i=0; i<48; i++)  {
            // Loop break conditions.
            if ((fast&&i>20) || td>.9 ||  sum.a > 0.99 || t>max_dist) break;
		
            pos = ro + t*rd;
            
            d = mapGalaxy(3.5*pos, id); // evaluate distance function
            d = abs(d)+.05;  // kind of countour drawing
            d = max(d,.005); // change this string to control density 
            
            if (d<h) {
                // compute local density 
                ld = h - d;         
                ld +=  clamp((ld - mapGalaxy(pos*3.5-.2*normalize(pos), id))*2.5, 0., 1. );
                td += (1. - td) * ld + .005;
                col = vec4(computeColor(td,length(pos)), td*.25 );
                col.rgb *= col.a;
                sum += col*(1. - sum.a);  
            }

            td += .014;
            // point light calculations
            ldst = pos*.25;
            ldst2 = pos*.05;
            ldst2.z *= 2.5;
            lDist  = max(length(ldst),.0001); //max(length(ldst), 0.001);
            lDist2 = max(length(ldst2),.0001);
            // star in center
            lightColor = (1.-smoothstep(3.,4.5,lDist*lDist))*
                mix(.07*vec3(1.,.5,.25)/(lDist),
                    .008*vec3(1.,1.7,2.)/(lDist2), 
                    smoothstep(.2,.7,lDist));

            // star in center
            sum.rgb += lightColor/(lDist*20.); //add a bloom around the light
            d = max(d, .04); 
            t +=  max(d * .3, .02);
        }
   		sum = clamp(sum, 0., 1.);
      	sum.xyz *= sum.xyz*(3.-sum.xyz-sum.xyz);
    }
    
    return sum;
}


// Toujours en coordonnes Univers
vec4 renderGalaxyField(in vec3 roU, in vec3 rd, out vec3 out_posU, out vec3 out_id, in bool fast) { 
    out_id = vec3(9);
      
    float dint, d=0.;
    vec3 offset, id, galaxyro,
         ros = roU + rd*d,
         pos = floor(ros),
         ri = 1./rd,
         rs = sign(rd),
         dis = (pos-ros + .5 + rs*.5) * ri;
	vec4 col, sum = vec4(0);
    
	for( int i=0; i<GALAXY_FIELD_VOXEL_STEPS_HD; i++ ) {
        
        if (!fast || i!=0 ) {//galaxyId != id) {
            id = hash33(pos);
        	offset = clamp(id, GALAXY_RADIUS, 1.-GALAXY_RADIUS);
       		d = distanceRayPoint(ros, rd, pos+offset, dint);
        	if (dint > 0. && d<GALAXY_RADIUS) {
                galaxyro = ros-(pos+offset);
                col = renderGalaxy(galaxyro/GALAXY_RADIUS*3., rd, vec4(id,.5), fast);
            	col.rgb *= smoothstep(float(GALAXY_FIELD_VOXEL_STEPS),0.,length(roU -pos));
                out_id = id;
                sum += (1.-sum.a)*col;
                if (sum.a>.99)
                    break;
            }
        }
		vec3 mm = step(dis.xyz, dis.yxy) * step(dis.xyz, dis.zzx);
		dis += mm * rs * ri;
        pos += mm * rs;
	}
    
    if (!fast && sum.a<.99) {
        for( int i=GALAXY_FIELD_VOXEL_STEPS_HD; i<GALAXY_FIELD_VOXEL_STEPS; i++ ) {
            id = hash33(pos);
            offset = clamp(id, GALAXY_RADIUS, 1.-GALAXY_RADIUS);
            d = distanceRayPoint(ros, rd, pos+offset, dint);
            if (dint > 0.) { 
                col = vec4(.9,.9,.8, 1.)*(1.-smoothstep(GALAXY_RADIUS*.25,GALAXY_RADIUS*.5,d));
                col.rgb *= smoothstep(float(GALAXY_FIELD_VOXEL_STEPS),0.,length(roU -pos));
                out_id = id;
                sum += (1.-sum.a)*col;
                if (sum.a>.99)
                    break;
            }

            vec3 mm = step(dis.xyz, dis.yxy) * step(dis.xyz, dis.zzx);
            dis += mm * rs * ri;
            pos += mm * rs;
        }
    }

	return sum; 
}


// ---------------------------------------------------
// Render intergalactic clouds
// ---------------------------------------------------

#ifdef WITH_INTERGALACTIC_CLOUDS


vec3 hsv2rgb(float x, float y, float z) {	
	return z+z*y*(clamp(abs(mod(x*6.+vec3(0,4,2),6.)-3.)-1.,0.,1.)-1.);
}


vec4 renderIntergalacticClouds(vec3 ro, vec3 rd, float tmax, const vec4 id) {
    
    float max_dist= min(tmax, float(STAR_FIELD_VOXEL_STEPS)),
		  td=0., d, t, noi, lDist, a, sp = 9.,         
    	  rRef = 2.*id.x,
          h = .05+.25*id.z;
    vec3 pos, lightColor;   
    vec4 sum = vec4(0);
   	
    t = .1*hash(hash(rd)); 

    for (int i=0; i<100; i++)  {
	    if(td>.9 ||  sum.a > .99 || t>max_dist) break;
        a = smoothstep(max_dist,0.,t);
        pos = ro + t*rd;
        d = abs(mapIntergalacticCloud(pos, id))+.07;

        // Light calculations
        lDist = max(length(mod(pos+sp*.5,sp)-sp*.5), .001); // TODO add random offset
        noi = pn(.05*pos);
        lightColor = mix(hsv2rgb(noi,.5,.6), 
                         hsv2rgb(noi+.3,.5,.6), 
                         smoothstep(rRef*.5,rRef*2.,lDist));
        sum.rgb += a*lightColor/exp(lDist*lDist*lDist*.08)/30.;
		// Edges coloring
        if (d<h) {
			td += (1.-td)*(h-d)+.005;  // accumulate density
            sum.rgb += sum.a * sum.rgb * .25 / lDist;  // emission	
			sum += (1.-sum.a)*.02*td*a;  // uniform scale density + alpha blend in contribution 
        } 
        td += .015;
        t += max(d * .08 * max(min(lDist,d),2.), .01);  // trying to optimize step size
    }
    
   	sum = clamp(sum, 0., 1.);   
    sum.xyz *= sum.xyz*(3.-sum.xyz-sum.xyz);
	return sum;
}

#endif 

//-----------------------------------------------------
//        Coordinate system conversions
//-----------------------------------------------------

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


void PS( out vec4 fragColor, in vec2 fragCoord ) {
	vec2 uv = fragCoord.xy / iResolution.xy;
    vec2 p = -1. + 2. * uv;
    p.x *= iResolution.x/iResolution.y;
    
    vec3 col = vec3(0);
    
    
// Lecture de la configuration -------------------------
        Config cfg = getConfig();

		bool isU = cfg.coordSystem == IN_UNIVERSE,
			 isG = cfg.coordSystem == IN_GALAXY;

// camera ----------------------------------------------

    vec3 ro, rdcam, ta, up;
    ro = cfg.ro_cam; 
    rdcam = cfg.rd_cam;

    // Leger tangage
    up = normalize(vec3(.1*cos(.1*iTime), .3*cos(.1*iTime), 1.));
    vec3 ww = normalize( rdcam ),
         uu = normalize( cross(ww,up) ),
         vv = normalize( cross(uu,ww)),
         rd = normalize( -p.x*uu + p.y*vv + 2.*ww );

// - Est t on dans une galaxie ? --------------------------------     
    vec3 galaxyId, galaxyPosU;
    bool inGalaxy = isInGalaxy(isU ? ro : galaxyToUniverse(cfg.galaxy_pos, ro), galaxyId, galaxyPosU);
       
// - rendu des etoiles -----------------------------------

    bool isHitStar = false;
	vec3 starPosG, starId = vec3(90);  
	vec4 star = vec4(0);
    if (inGalaxy) {
        // Le calcul se fait toujours en coordonnees Galaxie       
        vec3 roG = isU ? universeToGalaxy(cfg.galaxy_pos, ro) : ro;    
        float dStar = 9999.;
        star = renderStarField(roG, rd, dStar, starId );
    }

// - rendu des galaxies ----------------------------------

    vec3 targetPosU, targetId = vec3(90);
    // Le calcul se fait toujours en coordonnees univers
	vec3 roU = isG ? galaxyToUniverse(cfg.galaxy_pos, ro) : ro;
	vec4 colGalaxy = renderGalaxyField(roU, rd, targetPosU, targetId, isG);

    star.rgb += colGalaxy.rgb* (1. - star.a);

    col = star.rgb;
    
    
 //   float digit = PrintValue(fragCoord, iResolution.xy*vec2(.0,.7), vec2(20.), cfg.galaxy_pos.x, 8., 10.);
 //   digit += PrintValue(fragCoord, iResolution.xy*vec2(.0,.6), vec2(20.), cfg.galaxy_pos.y, 8., 10.);
 //   digit += PrintValue(fragCoord, iResolution.xy*vec2(.0,.5), vec2(20.), cfg.galaxy_pos.z, 8., 10.);
 //	col = mix(col, vec3(1,0,0), digit);
    
    fragColor = vec4((isU ? vec3(0.03,0.,.1)+col : col), 1.);
 
}

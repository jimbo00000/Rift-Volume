// volume.frag
#version 330

varying vec2 vfFragCoord;

// ShaderToy Inputs:
uniform vec3 iResolution;
uniform vec3 iChannelResolution[4]; // channel resolution (in pixels)

// Oculus-specific additions:
uniform float u_eyeballCenterTweak;
uniform float u_fov_y_scale;
uniform float u_opacityScale;
uniform mat4 mvmtx;
uniform mat4 prmtx;
uniform mat4 obmtx;

uniform sampler3D volume;
uniform sampler2D opacitymap;
uniform sampler2D colormap;

// Simple ray marching example
// @var url https://www.shadertoy.com/view/ldB3Rw
// @var author gltracy
// @var license CC BY-NC-SA 3.0

// @var headSize 6.0
// @var eyePos -2.5952096 5.4259381 -20.277588

const int max_iterations = 255;
const float stop_threshold = 0.001;
const float grad_step = 0.1;
const float clip_far = 1000.0;

// math
const float PI = 3.14159265359;
const float DEG_TO_RAD = PI / 180.0;

// distance function
float dist_sphere( vec3 pos, float r ) {
    return length( pos ) - r;
}

float dist_box( vec3 pos, vec3 size ) {
    return length( max( abs( pos ) - size, 0.0 ) );
}

float dist_box( vec3 v, vec3 size, float r ) {
    return length( max( abs( v ) - size, 0.0 ) ) - r;
}

// Returns both t parameters for intersection points
vec2 rayIntersectsSphere( in vec3 ro, in vec3 rd, in vec3 center, float radius )
{
    vec3 oc = ro - center;
    float b = 2.0 * dot( rd, oc );
    float c = dot( oc, oc ) - radius * radius;
    float disc = b * b - 4.0 * c;

    if ( disc < 0.0 )
    {
        return vec2( -9999.0 ); // no hit
    }

    float q;
    if ( b < 0.0 )
        q = ( -b - sqrt(disc) ) / 2.0;
    else
        q = ( -b + sqrt(disc) ) / 2.0;

    float t0 = q;
    float t1 = c / q;

    if ( t0 < t1 )
    {
        // Ensure t0 < t1
        float temp = t0;
        t0 = t1;
        t1 = temp;
    }
    return vec2( t0, t1 );
}

///////////////////////////////////////////////////////////////////////////////
// Agreeing on certain entry points, we can patch in the Rift's heading:
//
// vec3 getEyePoint( mat4 mvmtx );
// vec3 getEyeDirection( vec2 uv, mat4 mvmtx );
// vec2 getSamplerUV( vec2 fragCoord );
// vec3 getSceneColor( in vec3 ro, in vec3 rd );
//
///////////////////////////////////////////////////////////////////////////////
vec3 getSceneColor( in vec3 ro, in vec3 rd, inout float depth )
{
    // Raytrace a sphere
    vec3 center = vec3( 0.0, 1.5, 0.0 );
    vec2 hits = rayIntersectsSphere( ro, rd, center, 1.0 );
    if ( hits.x < -9998.0 )
        discard;

    if ( (hits.x < 0.0) && (hits.y < 0.0) )
        discard;

    // Clip off the portion of the volume raymarch path behind the viewpoint.
    // I thought this would have been x to clip, but that's just one more thing that's backwards...
    hits.y = max( 0.0, hits.y );

    vec3 startPt = ro + hits.x * rd;
    vec3 endPt = ro + hits.y * rd;

    startPt -= center;
    endPt -= center;

    // Manually tease out translation component
    vec3 trans = obmtx[3].xyz;
    startPt -= trans;
    endPt -= trans;

    // Multiplying this in reverse order(vec*mtx) is equivalent to transposing the matrix.
    startPt = vec4( vec4(startPt,1.0) * obmtx ).xyz;
    endPt = vec4( vec4(endPt,1.0) * obmtx ).xyz;

    // Manually tease out scale component
    float scale = length(obmtx[0].xyz);
    startPt /= scale * scale;
    endPt /= scale * scale;

    // Rotate about center of unit cube
    // Order of operations are also flipped in raymarching, watch out as this can get confusing.
    startPt += vec3( 0.5 );
    endPt += vec3( 0.5 );

    vec3 rayPath = endPt - startPt;

    depth = 9999.0;

    // Step through the volume accumulating opacity
    vec4 accum = vec4(0.0);
    for ( int i = 0; i < max_iterations; i++ )
    {
        vec3 samplePt = startPt + (float(i) / float(max_iterations)) * rayPath;

        // Cull samples outside of volume.
        // This appears to require the else if clause to apply clipping on both octants.
        if ( any( lessThan(samplePt, vec3(0.0)) ) )
            continue;
        else if ( any( greaterThan(samplePt, vec3(1.0)) ) )
            continue;

        float voxelVal = texture( volume, samplePt ).r;
        vec4 txfrCol = texture2D( colormap, vec2(voxelVal, 0.5) );
        float texOpacity = texture2D( opacitymap, vec2(voxelVal, 0.5) ).r;
        accum.rgb += u_opacityScale * texOpacity * txfrCol.rgb;
        accum.a += u_opacityScale * texOpacity * voxelVal;

        if ((accum.a > 0.2) && (depth > 9990.0))
        {
            float travel = float(i) / float(max_iterations);
            depth = length( (startPt + travel*rayPath) - ro);
        }
    }
    return accum.xyz;
}

// http://blog.hvidtfeldts.net/
// Translate the origin to the camera's location in world space.
vec3 getEyePoint( mat4 mvmtx )
{
    return -( mvmtx[3].xyz ) * mat3( mvmtx );
}

// Construct the usual eye ray frustum oriented down the negative z axis,
// then transform it by the modelview matrix to go from camera to world space.
vec3 getEyeDirection( vec2 uv, mat4 mvmtx )
{
    float aspect = iResolution.x / iResolution.y;

    vec3 dir = vec3(
        uv.x * u_fov_y_scale * aspect,
        uv.y * u_fov_y_scale,
        -1.0);
    dir *= mat3( mvmtx );
    return normalize( dir );
}

// Get a per-fragment location value in [-1,1].
// Also apply a stereo tweak based on a uniform variable.
vec2 getSamplerUV( vec2 fragCoord )
{
    vec2 uv = fragCoord.xy;
    uv = -1.0 + 2.0*uv;
    uv.x += u_eyeballCenterTweak;
    return uv;
}

void main()
{
    vec2 uv  = getSamplerUV( vfFragCoord );
    vec3 ro  = getEyePoint( mvmtx );
    vec3 rd  = getEyeDirection( uv, mvmtx );
    float depth = 9999.0;
    vec3 col = getSceneColor( ro, rd, depth );

    // Write to depth buffer
    vec3 eyeFwd = getEyeDirection( vec2(0.0), mvmtx );
    float eyeHitZ = -depth;// * dot( rd, eyeFwd );

    float p10 = prmtx[2].z;
    float p11 = prmtx[3].z;
    // A little bit of algebra...
    float ndcDepth = -p10 + -p11 / eyeHitZ;

    gl_FragDepth = ((gl_DepthRange.diff * ndcDepth) + gl_DepthRange.near + gl_DepthRange.far) / 2.0;
    gl_FragColor = vec4(col, 1.0);
}
// blaws
// November 7, 2014
// Tree

varying vec3 v_vNormal;

const vec3 color = vec3( 0.5451, 0.2706, 0.0745 );
const vec3 lightDir = vec3( -1.0, -1.0, 1.0 );

//==========================================================

void main()
{
    vec3 L = normalize( lightDir );
    vec3 N = normalize( v_vNormal );
    float NdotL = dot( N, L );
    
    gl_FragColor = vec4( NdotL * color, 1.0 );
}

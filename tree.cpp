// blaws
// November 7, 2014
// Tree

#include "Angel.h"
#include <vector>

//////////////////////////////////////////////////////////////////////////////

// window size and framerate
int windowW = 750;
int windowH = 750;

// options
const vec3 rootLoc( 0.0, 0.0, 0.0 );
const float rootR( 1.0 );
const float rootAzimuth( 0.0 );
const float rootAltitude( 0.0 );
const int pointsPerCircle( 13 );
const int minSplitDistance( 5 );
bool wireFrame( false );

// object data
std::vector<vec3> vert;
std::vector<vec2> texcoord;
std::vector<vec3> normal;
std::vector<GLushort> indices;

// view
vec3 lookAt( 0.0, 0.0, 0.0 );
vec3 camera;
const vec3 up( 0.0, 0.0, 1.0 );
float cameraR = 5.0;
float thetaXY( 1.5 * M_PI );
float thetaZ;
int mouseX;
int mouseY;

// matrices
mat4 projMatrix;
mat4 mvMatrix;

// shader handles
GLuint objectBuffer;
GLuint indexBuffer;
GLuint mvMatrixLoc;
GLuint vertLoc;
GLuint texCoordLoc;
GLuint normalLoc;

//////////////////////////////////////////////////////////////////////////////

void init()
{
    srand( time( NULL ) );
    glClearColor( 1.0, 1.0, 1.0, 1.0 );
    glEnable( GL_DEPTH_TEST );
    //glEnable( GL_TEXTURE_2D );
    glPixelStorei( GL_PACK_ALIGNMENT, 1 );
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
}

void setView()
{
    camera.x = lookAt.x + cameraR * cos( thetaXY ) * cos( thetaZ );
    camera.y = lookAt.y + cameraR * sin( thetaXY ) * cos( thetaZ );
    camera.z = lookAt.z + cameraR * sin( thetaZ );
    
    projMatrix = Perspective( 45.0, 1.0, 0.1, 2.0 * std::fmax( cameraR, 1.0 ) );
    mvMatrix = LookAt( camera, lookAt, up );
}

float extendBranch( vec3 center, float r, float startAngle, float azimuth, float altitude, int prevLayer, int distanceFromSplit )
{
    // base case
    if( r < 0.01 || center.z < 0.0 || ( r < 0.1 && !(rand()%100) ))
    {
        return center.z;
    }

    // save place in vertex buffer
    int nextPrevLayer( vert.size() );
    int curPlace( prevLayer );
    
    // create one new ring
    float inc = 2.0 * M_PI / ( pointsPerCircle - 1 );
    vec3 n( cos( azimuth ) * sin( altitude ),
            sin( azimuth ) * sin( altitude ),
            cos( altitude ) );
    vec3 u( -sin( azimuth ), cos( azimuth ), 0.0 );
    vec3 nCrossU( cos( altitude ) * cos( azimuth ),
                  cos( altitude ) * sin( azimuth ),
                 -sin( altitude ) );
    for( float th = startAngle; th < startAngle + 2.0 * M_PI; th += inc, ++curPlace )
    {
        vert.push_back( r * cos( th ) * u + r * sin( th ) * nCrossU + center );
        normal.push_back( r * cos( th ) * u + r * sin( th ) * nCrossU );
        
        if( curPlace >= 0 )
        {
            // lower triangle
            indices.push_back( vert.size() - 1 );
            indices.push_back( curPlace );
            indices.push_back( curPlace + 1 );
            
            if( curPlace > prevLayer )
            {
                // upper triangle
                indices.push_back( vert.size() - 1 );
                indices.push_back( vert.size() - 2 );
                indices.push_back( curPlace );
            }
        }
    }
    
    // calculate next step
    vec3 nextCenter( n * 0.25 + center );
    float nextR = ( ( rand() % 100 + 850 ) / 1000.0 ) * r;  // 85% - 95% of current r
    float nextAngle( startAngle + inc / 2.0 >= 2.0 * M_PI ? startAngle + inc / 2.0 - 2.0 * M_PI : startAngle + inc / 2.0 );
    float nextAzimuth( rand() % 10 ? azimuth : azimuth + ( rand() % 2 ? 1.0 : -1.0 ) * ( rand() % 1000 / 1000.0 * M_PI / 10.0 ) );
    float nextAltitude( rand() % 10 ? altitude : altitude + ( rand() % 2 ? 1.0 : -1.0 ) * ( rand() % 100 * 0.002 ) );
    
    // recurse
    if( distanceFromSplit < minSplitDistance || rand() % 5 )
    {
        return extendBranch( nextCenter, nextR, nextAngle, nextAzimuth, nextAltitude, nextPrevLayer, distanceFromSplit + 1 );
    }
    else  // branch
    {
        nextAzimuth = rand() % 1000 / 1000.0 * ( 2.0 * M_PI );
        float branchAngle = rand() % 1000 / 1000.0 * M_PI / 3.0;
        return std::fmax( extendBranch( center, r, nextAngle, nextAzimuth, nextAltitude - branchAngle,nextPrevLayer, 0 ),
                          extendBranch( center, r, nextAngle, nextAzimuth, nextAltitude + branchAngle, nextPrevLayer, 0 ) );
    }
}

void rebuildTree()
{
    vert.clear();
    texcoord.clear();
    normal.clear();
    indices.clear();
    
    float height = extendBranch( rootLoc, rootR, 0.0, rootAzimuth, rootAltitude, -1, 0 );
    
    lookAt.z = height * 0.5;
    cameraR = height * 1.5;
}

void sendData()
{
    // vertices
    glBufferData( GL_ARRAY_BUFFER, ( vert.size() + normal.size() ) * sizeof( vec3 ) /*+ texcoord.size() * sizeof( vec2 )*/, NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, vert.size() * sizeof( vec3 ), vert[0] );
    glEnableVertexAttribArray( vertLoc );
    glVertexAttribPointer( vertLoc, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET( 0 ) );
    
    // indices
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof( GLushort ), &indices[0], GL_STATIC_DRAW );
    
    // texture coordinates
    /*glBufferSubData( GL_ARRAY_BUFFER, vert.size() * sizeof( vec3 ), texcoord.size() * sizeof( vec2 ), texcoord[0] );
     glEnableVertexAttribArray( texCoordLoc );
     glVertexAttribPointer( texCoordLoc, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET( vert.size() * sizeof( vec3 ) ) );*/
     
     // normals
     glBufferSubData( GL_ARRAY_BUFFER, vert.size() * sizeof( vec3 ) + texcoord.size() * sizeof( vec2 ), normal.size() * sizeof( vec3 ), normal[0] );
     glEnableVertexAttribArray( normalLoc );
     glVertexAttribPointer( normalLoc, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET( vert.size() * sizeof( vec3 ) + texcoord.size() * sizeof( vec2 ) ) );
}

void setupData()
{
    // build tree
    rebuildTree();

    // matrices
    setView();
    
    // create shader program
    GLuint sProgram = InitShader( "vert.glsl", "frag.glsl" );
    
    // object buffer
    glGenBuffers( 1, &objectBuffer );
    glBindBuffer( GL_ARRAY_BUFFER, objectBuffer );
    
    // vertices
    vertLoc = glGetAttribLocation( sProgram, "a_vPosition" );

    // indices
    glGenBuffers( 1, &indexBuffer );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indexBuffer );

    // texture coordinates
    //texCoordLoc = glGetAttribLocation( sProgram, "a_vTexCoord" );
    
    // normals
    normalLoc = glGetAttribLocation( sProgram, "a_vNormal" );
    
    // buffer to GPU
    sendData();
}

void display()
{
    // clear
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    // draw tree
    glUniformMatrix4fv( mvMatrixLoc, 1, GL_TRUE, projMatrix * mvMatrix );
    glDrawElements( GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, BUFFER_OFFSET( 0 ) );

    // flush to screen
    glutSwapBuffers();
}

void resize( int w, int h )
{
    windowW = w;
    windowH = h;
}

void mouse( int button, int state, int x, int y )
{
    if( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
    {
        mouseX = x;
        mouseY = y;
    }
}

void mouseMove( int x, int y )
{
    int dx = mouseX - x;
    int dy = y - mouseY;
    
    thetaXY += dx / static_cast<float>( windowW / 2 );
    thetaZ += dy / static_cast<float>( windowH / 2 );

    if( thetaXY >= 2.0 * M_PI )
    {
        thetaXY -= 2.0 * M_PI;
    }
    else if( thetaXY < 0.0 )
    {
        thetaXY += 2.0 * M_PI;
    }
    if( thetaZ < -0.5 * M_PI + 0.01 )
    {
        thetaZ = -0.5 * M_PI + 0.01;
    }
    else if( thetaZ > 0.5 * M_PI - 0.01 )
    {
        thetaZ = 0.5 * M_PI - 0.01;
    }

    setView();
    
    mouseX = x;
    mouseY = y;
    
    glutPostRedisplay();
}

void keyboard( unsigned char key, int x, int y )
{
    switch( key )
    {
        case 'i':
        case 'I':
            cameraR -= 0.5;
            if( cameraR < 0.5 )
            {
                cameraR = 0.5;
            }
            break;
        case 'o':
        case 'O':
            cameraR += 0.5;
            break;
            
        case 'a':
        case 'A':
            --lookAt.z;
            break;
        case 's':
        case 'S':
            ++lookAt.z;
            break;
            
        case 'r':
        case 'R':
            rebuildTree();
            sendData();
            setView();
            break;
            
        case 'w':
        case 'W':
            wireFrame = !wireFrame;
            glPolygonMode( GL_FRONT_AND_BACK, wireFrame ? GL_LINE : GL_FILL );
            break;
            
        case 033:  // ESC
        case 'q':
        case 'Q':
            exit( EXIT_SUCCESS );
    }
    
    setView();
    glutPostRedisplay();
}

//////////////////////////////////////////////////////////////////////////////

int main( int argc, char** argv )
{
    // set up GLUT
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize( windowW, windowH );
    glutCreateWindow( "Tree" );
    
    // change default settings
    init();

    // create view
    setupData();
    
    // set callbacks
    glutDisplayFunc( display );
    glutReshapeFunc( resize );
    glutMouseFunc( mouse );
    glutMotionFunc( mouseMove );
    glutKeyboardFunc( keyboard );
    
    // run
    glutMainLoop();
    return EXIT_SUCCESS;
}

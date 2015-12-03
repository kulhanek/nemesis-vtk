// =============================================================================
// NEMESIS - Molecular Modelling Package
// -----------------------------------------------------------------------------
//    Copyright (C) 2010 Petr Kulhanek, kulhanek@chemi.muni.cz
//
//     This program is free software; you can redistribute it and/or modify
//     it under the terms of the GNU General Public License as published by
//     the Free Software Foundation; either version 2 of the License, or
//     (at your option) any later version.
//
//     This program is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.
//
//     You should have received a copy of the GNU General Public License along
//     with this program; if not, write to the Free Software Foundation, Inc.,
//     51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// =============================================================================

#include <GL/glew.h>
#include <hpmc.h>
//#include "/home/xstepan3/work/Development/thirdparty/hpmc/1.0.1/src/bin/common/common.cpp"

#include <iostream>
#include <fstream>


#include <PluginObject.hpp>
#include <CategoryUUID.hpp>
#include <ErrorSystem.hpp>
#include <GraphicsObjectList.hpp>
#include <GL/gl.h>
#include <GL/glu.h>
#include <Project.hpp>
#include <GraphicsProfileList.hpp>
#include <GLWidget.hpp>

#include <NemesisCoreModule.hpp>
#include <VolumeObject.hpp>
#include <VolumeSetup.hpp>



#ifdef __APPLE__

#else
//#include <GL/glut.h>
#endif
//#include <glut.h>
//#include "../common/common.cpp"


int volume_size_x;
int volume_size_y;
int volume_size_z;
using namespace std;
std::vector<GLubyte> dataset;

GLuint volume_tex;

struct HPMCConstants* hpmc_c;
struct HPMCHistoPyramid* hpmc_h;

// -----------------------------------------------------------------------------

GLuint shaded_v;
GLuint shaded_f;
GLuint shaded_p;
struct HPMCTraversalHandle* hpmc_th_flat;

GLuint flat_v;
GLuint flat_p;
struct HPMCTraversalHandle* hpmc_th_shaded;

std::string shaded_vertex_shader =
        "varying vec3 normal;\n"
        "void\n"
        "main()\n"
        "{\n"
        "    vec3 p, n;\n"
        "    extractVertex( p, n );\n"
        "    gl_Position = gl_ModelViewProjectionMatrix * vec4( p, 1.0 );\n"
        "    normal = gl_NormalMatrix * n;\n"
        "    gl_FrontColor = gl_Color;\n"
        "}\n";
std::string shaded_fragment_shader =
        "varying vec3 normal;\n"
        "void\n"
        "main()\n"
        "{\n"
        "    const vec3 v = vec3( 0.0, 0.0, 1.0 );\n"
        "    vec3 l = normalize( vec3( 1.0, 1.0, 1.0 ) );\n"
        "    vec3 h = normalize( v+l );\n"
        "    vec3 n = normalize( normal );\n"
        "    float diff = max( 0.1, dot( n, l ) );\n"
        "    float spec = pow( max( 0.0, dot(n, h)), 20.0);\n"
        "    gl_FragColor = diff * gl_Color +\n"
        "                   spec * vec4(1.0);\n"
        "}\n";

std::string flat_vertex_shader =
        "void\n"
        "main()\n"
        "{\n"
        "    vec3 p, n;\n"
        "    extractVertex( p, n );\n"
        "    gl_Position = gl_ModelViewProjectionMatrix * vec4( p, 1.0 );\n"
        "    gl_FrontColor = gl_Color;\n"
        "}\n";


//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

QObject* VolumeObjectCB(void* p_data);

CExtUUID        VolumeObjectID(
                    "{VOLUME_OBJECT:be31a437-9b46-403c-bf1d-3e1992f4e479}",
                    "Volume 3D object");

CPluginObject   VolumeObject(&NemesisCorePlugin,
                     VolumeObjectID,GRAPHICS_OBJECT_CAT,
                    ":/images/NemesisCore/graphics/Volume.svg",
                     VolumeObjectCB);

// -----------------------------------------------------------------------------

QObject* VolumeObjectCB(void* p_data)
{
    return(new CVolumeObject(static_cast<CGraphicsObjectList*>(p_data)));
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

CVolumeObject::CVolumeObject(CGraphicsObjectList* p_gl)
    : CGraphicsObject(&VolumeObject,p_gl,DP_NORMAL_PRIORITY_OBJECT)
{
    Setup = NULL;
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

bool CVolumeObject::CanBeObjectAdded(CProObject* p_object)
{
    if( p_object == NULL ) return(false);
    return(false);
}

//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

void CVolumeObject::Draw(void)
{
    if( Setup != dynamic_cast<CVolumeSetup*>(GetSetup()) ) {
        Setup = dynamic_cast<CVolumeSetup*>(GetSetup());
    }
    if( Setup == NULL ){
        ES_ERROR("setup is not available");
        return;
    }

    if( IsFlagSet(EPOF_VISIBLE) == false ) return;
    //if( GLGetMode() == GL_SELECT ) return;

    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);

    glColor4fv(Setup->Color.ForGL());
    glLineWidth(Setup->Width);

    DrawVolumeData(NULL);

}

//------------------------------------------------------------------------------

void CVolumeObject::DrawVolumeData(char* p_data)
{

    // draw it here
    // glutInit( &argc, argv );


    volume_size_x = 200;
    volume_size_y = 200;
    volume_size_z = 200;

    dataset.resize( volume_size_x * volume_size_y * volume_size_z );
    ifstream datafile( "/home/xstepan3/work/Development/projects/nemesis/11.x/data/bonsai.raw", std::ios::in | std::ios::binary );
    if( datafile.good() ) {
        datafile.read( reinterpret_cast<char*>( &dataset[0] ),
                       dataset.size() );
    }
    else {
        ES_ERROR("Not able to open data file.");
        exit( EXIT_FAILURE );
    }

    glewInit();

    // init
    GLint alignment;
    glGetIntegerv( GL_UNPACK_ALIGNMENT, &alignment );
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
    glGenTextures( 1, &volume_tex );
    glBindTexture( GL_TEXTURE_3D, volume_tex );
    glTexImage3D( GL_TEXTURE_3D, 0, GL_ALPHA,
                  volume_size_x, volume_size_y, volume_size_z, 0,
                  GL_ALPHA, GL_UNSIGNED_BYTE, &dataset[0] );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glBindTexture( GL_TEXTURE_3D, 0 );
    glPixelStorei( GL_UNPACK_ALIGNMENT, alignment );

    // --- create HistoPyramid -------------------------------------------------
    hpmc_c = HPMCcreateConstants();
    hpmc_h = HPMCcreateHistoPyramid( hpmc_c );

    HPMCsetLatticeSize( hpmc_h,
                        volume_size_x,
                        volume_size_y,
                        volume_size_z );

    HPMCsetGridSize( hpmc_h,
                     volume_size_x-1,
                     volume_size_y-1,
                     volume_size_z-1 );

    float max_size = max( volume_size_x, max( volume_size_y, volume_size_z ) );
    HPMCsetGridExtent( hpmc_h,
                       volume_size_x / max_size,
                       volume_size_y / max_size,
                       volume_size_z / max_size );

    HPMCsetFieldTexture3D( hpmc_h,
                           volume_tex,
                           GL_FALSE );

    // --- create traversal vertex shader --------------------------------------
    hpmc_th_shaded = HPMCcreateTraversalHandle( hpmc_h );

    char *traversal_code = HPMCgetTraversalShaderFunctions( hpmc_th_shaded );
    const char* shaded_vsrc[2] =
    {
        traversal_code,
        shaded_vertex_shader.c_str()
    };
    shaded_v = glCreateShader( GL_VERTEX_SHADER );
    glShaderSource( shaded_v, 2, &shaded_vsrc[0], NULL );
    //compileShader( shaded_v, "shaded vertex shader" );
    free( traversal_code );

    const char* shaded_fsrc[1] =
    {
        shaded_fragment_shader.c_str()
    };
    shaded_f = glCreateShader( GL_FRAGMENT_SHADER );
    glShaderSource( shaded_f, 1, &shaded_fsrc[0], NULL );
    // compileShader( shaded_f, "shaded fragment shader" );

    // link program
    shaded_p = glCreateProgram();
    glAttachShader( shaded_p, shaded_v );
    glAttachShader( shaded_p, shaded_f );
    // linkProgram( shaded_p, "shaded program" );

    // associate program with traversal handle
    HPMCsetTraversalHandleProgram( hpmc_th_shaded,
                                   shaded_p,
                                   0, 1, 2 );

    hpmc_th_flat = HPMCcreateTraversalHandle( hpmc_h );

    traversal_code = HPMCgetTraversalShaderFunctions( hpmc_th_flat );
    const char* flat_src[2] =
    {
        traversal_code,
        flat_vertex_shader.c_str()
    };
    flat_v = glCreateShader( GL_VERTEX_SHADER );
    glShaderSource( flat_v, 2, &flat_src[0], NULL );
    // compileShader( flat_v, "flat vertex shader" );
    free( traversal_code );

    // link program
    flat_p = glCreateProgram();
    glAttachShader( flat_p, flat_v );
    // linkProgram( flat_p, "flat program" );

    // associate program with traversal handle
    HPMCsetTraversalHandleProgram( hpmc_th_flat,
                                   flat_p,
                                   0, 1, 2 );


    glPolygonOffset( 1.0, 1.0 );


    // glutMainLoop();



    // main konec





    // -----------------------------------------------------------------------------
       // ASSERT_GL;

        // --- clear screen and set up view ----------------------------------------
//        glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
//        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
//
//        glMatrixMode( GL_PROJECTION );
//        glLoadIdentity();
//        glFrustum( -0.14*aspect_x, 0.14*aspect_x, -0.14*aspect_y, 0.14*aspect_y, 0.5, 3.0 );
//
//        glMatrixMode( GL_MODELVIEW );
//        glLoadIdentity();
//        glTranslatef( 0.0f, 0.0f, -2.0f );
//        glRotatef( 20, 1.0, 0.0, 0.0 );
//        glRotatef( 20.0*t, 0.0, 1.0, 0.0 );
//        glRotatef( -90, 1.0, 0.0, 0.0 );

        //float max_size = max( volume_size_x, max( volume_size_y, volume_size_z ) );
        glTranslatef( -0.5f*volume_size_x / max_size,
                      -0.5f*volume_size_y / max_size,
                      -0.5f*volume_size_z / max_size );

        // --- build HistoPyramid --------------------------------------------------
        float iso = 0.5 + 0.48*cosf( 1 );
        HPMCbuildHistopyramid( hpmc_h, iso );

        HPMCextractVertices( hpmc_th_flat );

        // --- render surface ------------------------------------------------------
//        glEnable( GL_DEPTH_TEST );
//        if( true ) { // !wireframe ) {
//            glColor3f( 1.0-iso, 0.0, iso );
//            HPMCextractVertices( hpmc_th_shaded );
//        }
//        else {
//            glColor3f( 0.2*(1.0-iso), 0.0, 0.2*iso );
//            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//            glEnable( GL_POLYGON_OFFSET_FILL );
//
//            glDisable( GL_POLYGON_OFFSET_FILL );
//
//            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE );
//            glColor3f( 1.0, 1.0, 1.0 );
//            HPMCextractVertices( hpmc_th_flat );
//            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//        }

        // --- render text string --------------------------------------------------
//       static char message[512] = "";
//        if( floor(5.0*(t-dt)) != floor(5.0*(t)) ) {
//            snprintf( message, 512,
//                      "%.1f fps, %dx%dx%d samples, %d mvps, %d triangles, iso=%.2f%s",
//                      fps,
//                      volume_size_x,
//                      volume_size_y,
//                      volume_size_z,
//                      (int)( ((volume_size_x-1)*(volume_size_y-1)*(volume_size_z-1)*fps)/1e6 ),
//                      HPMCacquireNumberOfVertices( hpmc_h )/3,
//                      iso,
//                      wireframe ? " [wireframe]" : "");
//        }
//        glUseProgram( 0 );
//        glMatrixMode( GL_PROJECTION );
//        glLoadIdentity();
//        glMatrixMode( GL_MODELVIEW );
//        glLoadIdentity();
//        glDisable( GL_DEPTH_TEST );
//        glColor3f( 1.0, 1.0, 1.0 );
//        glRasterPos2f( -0.99, 0.95 );
//        for(int i=0; i<255 && message[i] != '\0'; i++) {
//            glutBitmapCharacter( GLUT_BITMAP_8_BY_13, (int)message[i] );
//        }

}








// --- compile shader and check for errors -------------------------------------
void CVolumeObject::compileShader( GLuint shader, const string& what )
{
    glCompileShader( shader );

    GLint compile_status;
    glGetShaderiv( shader, GL_COMPILE_STATUS, &compile_status );
    if( compile_status != GL_TRUE ) {
        cerr << "Compilation of " << what << " failed, infolog:" << endl;

        GLint logsize;
        glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logsize );

        if( logsize > 0 ) {
            vector<GLchar> infolog( logsize+1 );
            glGetShaderInfoLog( shader, logsize, NULL, &infolog[0] );
            cerr << string( infolog.begin(), infolog.end() ) << endl;
        }
        else {
            cerr << "Empty log message" << endl;
        }
        cerr << "Exiting." << endl;
        exit( EXIT_FAILURE );
    }
}

// --- compile program and check for errors ------------------------------------
void
CVolumeObject::linkProgram( GLuint program, const string& what )
{
    glLinkProgram( program );

    GLint linkstatus;
    glGetProgramiv( program, GL_LINK_STATUS, &linkstatus );
    if( linkstatus != GL_TRUE ) {
        cerr << "Linking of " << what << " failed, infolog:" << endl;

        GLint logsize;
        glGetProgramiv( program, GL_INFO_LOG_LENGTH, &logsize );

        if( logsize > 0 ) {
            vector<GLchar> infolog( logsize+1 );
            glGetProgramInfoLog( program, logsize, NULL, &infolog[0] );
            cerr << string( infolog.begin(), infolog.end() ) << endl;
        }
        else {
            cerr << "Empty log message" << endl;
        }
        cerr << "Exiting." << endl;
        exit( EXIT_FAILURE );
    }
}










//==============================================================================
//------------------------------------------------------------------------------
//==============================================================================

#ifdef false

#include <GL/glew.h>
#ifdef __APPLE__
#include <glut.h>
#else
#include <GL/glut.h>
#endif
#include "hpmc.h"
#include "../common/common.cpp"


int volume_size_x;
int volume_size_y;
int volume_size_z;
vector<GLubyte> dataset;

GLuint volume_tex;

struct HPMCConstants* hpmc_c;
struct HPMCHistoPyramid* hpmc_h;

// -----------------------------------------------------------------------------
GLuint shaded_v;
GLuint shaded_f;
GLuint shaded_p;
struct HPMCTraversalHandle* hpmc_th_flat;
std::string shaded_vertex_shader =
        "varying vec3 normal;\n"
        "void\n"
        "main()\n"
        "{\n"
        "    vec3 p, n;\n"
        "    extractVertex( p, n );\n"
        "    gl_Position = gl_ModelViewProjectionMatrix * vec4( p, 1.0 );\n"
        "    normal = gl_NormalMatrix * n;\n"
        "    gl_FrontColor = gl_Color;\n"
        "}\n";
std::string shaded_fragment_shader =
        "varying vec3 normal;\n"
        "void\n"
        "main()\n"
        "{\n"
        "    const vec3 v = vec3( 0.0, 0.0, 1.0 );\n"
        "    vec3 l = normalize( vec3( 1.0, 1.0, 1.0 ) );\n"
        "    vec3 h = normalize( v+l );\n"
        "    vec3 n = normalize( normal );\n"
        "    float diff = max( 0.1, dot( n, l ) );\n"
        "    float spec = pow( max( 0.0, dot(n, h)), 20.0);\n"
        "    gl_FragColor = diff * gl_Color +\n"
        "                   spec * vec4(1.0);\n"
        "}\n";

GLuint flat_v;
GLuint flat_p;
struct HPMCTraversalHandle* hpmc_th_shaded;
std::string flat_vertex_shader =
        "void\n"
        "main()\n"
        "{\n"
        "    vec3 p, n;\n"
        "    extractVertex( p, n );\n"
        "    gl_Position = gl_ModelViewProjectionMatrix * vec4( p, 1.0 );\n"
        "    gl_FrontColor = gl_Color;\n"
        "}\n";

// -----------------------------------------------------------------------------
void
init()
{
    // --- upload volume ------------------------------------------------------

    GLint alignment;
    glGetIntegerv( GL_UNPACK_ALIGNMENT, &alignment );
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
    glGenTextures( 1, &volume_tex );
    glBindTexture( GL_TEXTURE_3D, volume_tex );
    glTexImage3D( GL_TEXTURE_3D, 0, GL_ALPHA,
                  volume_size_x, volume_size_y, volume_size_z, 0,
                  GL_ALPHA, GL_UNSIGNED_BYTE, &dataset[0] );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glBindTexture( GL_TEXTURE_3D, 0 );
    glPixelStorei( GL_UNPACK_ALIGNMENT, alignment );

    // --- create HistoPyramid -------------------------------------------------
    hpmc_c = HPMCcreateConstants();
    hpmc_h = HPMCcreateHistoPyramid( hpmc_c );

    HPMCsetLatticeSize( hpmc_h,
                        volume_size_x,
                        volume_size_y,
                        volume_size_z );

    HPMCsetGridSize( hpmc_h,
                     volume_size_x-1,
                     volume_size_y-1,
                     volume_size_z-1 );

    float max_size = max( volume_size_x, max( volume_size_y, volume_size_z ) );
    HPMCsetGridExtent( hpmc_h,
                       volume_size_x / max_size,
                       volume_size_y / max_size,
                       volume_size_z / max_size );

    HPMCsetFieldTexture3D( hpmc_h,
                           volume_tex,
                           GL_FALSE );

    // --- create traversal vertex shader --------------------------------------
    hpmc_th_shaded = HPMCcreateTraversalHandle( hpmc_h );

    char *traversal_code = HPMCgetTraversalShaderFunctions( hpmc_th_shaded );
    const char* shaded_vsrc[2] =
    {
        traversal_code,
        shaded_vertex_shader.c_str()
    };
    shaded_v = glCreateShader( GL_VERTEX_SHADER );
    glShaderSource( shaded_v, 2, &shaded_vsrc[0], NULL );
    compileShader( shaded_v, "shaded vertex shader" );
    free( traversal_code );

    const char* shaded_fsrc[1] =
    {
        shaded_fragment_shader.c_str()
    };
    shaded_f = glCreateShader( GL_FRAGMENT_SHADER );
    glShaderSource( shaded_f, 1, &shaded_fsrc[0], NULL );
    compileShader( shaded_f, "shaded fragment shader" );

    // link program
    shaded_p = glCreateProgram();
    glAttachShader( shaded_p, shaded_v );
    glAttachShader( shaded_p, shaded_f );
    linkProgram( shaded_p, "shaded program" );

    // associate program with traversal handle
    HPMCsetTraversalHandleProgram( hpmc_th_shaded,
                                   shaded_p,
                                   0, 1, 2 );

    hpmc_th_flat = HPMCcreateTraversalHandle( hpmc_h );

    traversal_code = HPMCgetTraversalShaderFunctions( hpmc_th_flat );
    const char* flat_src[2] =
    {
        traversal_code,
        flat_vertex_shader.c_str()
    };
    flat_v = glCreateShader( GL_VERTEX_SHADER );
    glShaderSource( flat_v, 2, &flat_src[0], NULL );
    compileShader( flat_v, "flat vertex shader" );
    free( traversal_code );

    // link program
    flat_p = glCreateProgram();
    glAttachShader( flat_p, flat_v );
    linkProgram( flat_p, "flat program" );

    // associate program with traversal handle
    HPMCsetTraversalHandleProgram( hpmc_th_flat,
                                   flat_p,
                                   0, 1, 2 );


    glPolygonOffset( 1.0, 1.0 );
}

// -----------------------------------------------------------------------------
void
render( float t, float dt, float fps )
{
    ASSERT_GL;

    // --- clear screen and set up view ----------------------------------------
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glFrustum( -0.14*aspect_x, 0.14*aspect_x, -0.14*aspect_y, 0.14*aspect_y, 0.5, 3.0 );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glTranslatef( 0.0f, 0.0f, -2.0f );
    glRotatef( 20, 1.0, 0.0, 0.0 );
    glRotatef( 20.0*t, 0.0, 1.0, 0.0 );
    glRotatef( -90, 1.0, 0.0, 0.0 );

    float max_size = max( volume_size_x, max( volume_size_y, volume_size_z ) );
    glTranslatef( -0.5f*volume_size_x / max_size,
                  -0.5f*volume_size_y / max_size,
                  -0.5f*volume_size_z / max_size );

    // --- build HistoPyramid --------------------------------------------------
    float iso = 0.5 + 0.48*cosf( t );
    HPMCbuildHistopyramid( hpmc_h, iso );

    // --- render surface ------------------------------------------------------
    glEnable( GL_DEPTH_TEST );
    if( !wireframe ) {
        glColor3f( 1.0-iso, 0.0, iso );
        HPMCextractVertices( hpmc_th_shaded );
    }
    else {
        glColor3f( 0.2*(1.0-iso), 0.0, 0.2*iso );
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable( GL_POLYGON_OFFSET_FILL );
        HPMCextractVertices( hpmc_th_flat );
        glDisable( GL_POLYGON_OFFSET_FILL );

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE );
        glColor3f( 1.0, 1.0, 1.0 );
        HPMCextractVertices( hpmc_th_flat );
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // --- render text string --------------------------------------------------
    static char message[512] = "";
    if( floor(5.0*(t-dt)) != floor(5.0*(t)) ) {
        snprintf( message, 512,
                  "%.1f fps, %dx%dx%d samples, %d mvps, %d triangles, iso=%.2f%s",
                  fps,
                  volume_size_x,
                  volume_size_y,
                  volume_size_z,
                  (int)( ((volume_size_x-1)*(volume_size_y-1)*(volume_size_z-1)*fps)/1e6 ),
                  HPMCacquireNumberOfVertices( hpmc_h )/3,
                  iso,
                  wireframe ? " [wireframe]" : "");
    }
    glUseProgram( 0 );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glDisable( GL_DEPTH_TEST );
    glColor3f( 1.0, 1.0, 1.0 );
    glRasterPos2f( -0.99, 0.95 );
    for(int i=0; i<255 && message[i] != '\0'; i++) {
        glutBitmapCharacter( GLUT_BITMAP_8_BY_13, (int)message[i] );
    }
}


// -----------------------------------------------------------------------------
int
main(int argc, char **argv)
{
    glutInit( &argc, argv );

    if( argc != 5 ) {
        cerr << "HPMC demo application that visualizes raw volumes."<<endl<<endl;
        cerr << "Usage: " << argv[0] << " xsize ysize zsize rawfile"<<endl<<endl;
        cerr << "where: xsize    The number of samples in the x-direction."<<endl;
        cerr << "       ysize    The number of samples in the y-direction."<<endl;
        cerr << "       zsize    The number of samples in the z-direction."<<endl;
        cerr << "       rawfile  Filename of a raw set of bytes describing"<<endl;
        cerr << "                the volume."<<endl<<endl;
        cerr << "Raw volumes can be found e.g. at http://www.volvis.org."<<endl<<endl;
        cerr << "Example usage:"<<endl;
        cerr << "    " << argv[0] << " 64 64 64 neghip.raw"<< endl;
        cerr << "    " << argv[0] << " 256 256 256 foot.raw"<< endl;
        cerr << "    " << argv[0] << " 256 256 178 BostonTeapot.raw"<< endl;
        cerr << "    " << argv[0] << " 301 324 56 lobster.raw"<< endl;
        exit( EXIT_FAILURE );
    }
    else {
        volume_size_x = atoi( argv[1] );
        volume_size_y = atoi( argv[2] );
        volume_size_z = atoi( argv[3] );

        dataset.resize( volume_size_x * volume_size_y * volume_size_z );
        ifstream datafile( argv[4], std::ios::in | std::ios::binary );
        if( datafile.good() ) {
            datafile.read( reinterpret_cast<char*>( &dataset[0] ),
                           dataset.size() );
        }
        else {
            cerr << "Error opening \"" << argv[4] << "\" for reading." << endl;
            exit( EXIT_FAILURE );
        }
    }
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize( 1280, 720 );
    glutCreateWindow( argv[0] );
    glewInit();
    glutReshapeFunc( reshape );
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
    glutIdleFunc( idle );
    init();
    glutMainLoop();
    return EXIT_SUCCESS;
}


#endif

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

//MAC
#include <OpenGL/gl.h>

//Raspberry
//#include <SDL_opengles2.h>

typedef int bool;
#define true 1
#define false 0

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const Uint32 window_flags = SDL_WINDOW_OPENGL;

SDL_Window *sdl_window;
SDL_GLContext sdl_gl_context;

//Starts up SDL and creates window
bool init();

//Loads media
bool load();

//Frees media and shuts down SDL
void close();

void close(){
    //Quit SDL subsystems
    SDL_GL_DeleteContext(sdl_gl_context);
    SDL_DestroyWindow(sdl_window);
    SDL_Quit();
}


bool init(){
    //The surface contained by the window
    SDL_Surface* screenSurface = NULL;

    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0){
     printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
    }
    else
    {
#ifdef USE_OPENGLES
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
	SDL_GL_CONTEXT_PROFILE_ES);
#endif
        //Create window
        sdl_window = SDL_CreateWindow(
                "GL SDL TEST",
                SDL_WINDOWPOS_CENTERED,
                SDL_WINDOWPOS_CENTERED,
                SCREEN_WIDTH, SCREEN_HEIGHT,
                window_flags);

        if(!sdl_window)
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
            exit(1);
        }

        sdl_gl_context = SDL_GL_CreateContext(sdl_window);
        if(!sdl_gl_context){
            printf( "Failed to create OpenGL window: %s\n", SDL_GetError() );
            exit(2);
//            //Get window surface
//            screenSurface = SDL_GetWindowSurface( sdl_window );
//
//
//            //Fill the surface white
//            SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0xFF, 0xFF, 0xFF ) );
//            printf( "rect: %s\n", SDL_GetError() );
//
//            //Update the surface
//            SDL_UpdateWindowSurface( sdl_window );
//            printf( "update: %s\n", SDL_GetError() );

            bool quit = false ;
            SDL_Event e;
            while (!quit) {
                while (SDL_PollEvent(&e)) {
                    if (e.type == SDL_QUIT) {
                        quit = true;
                    }
                }
            }
        }
    }
    return true;
}

//GLfloat point2d = {1.0, 0.3, 0.0};

int main (int argc, char* args[] )
{
    init();

    //create shader
    const char * shader_vert_src =
            "uniform float u_time;\n"
            "attribute vec2 va_position;\n"
            "varying vec3 v_color;\n"
            "void main() {\n"
            "	v_color = vec3(1.0 - 0.5*(va_position.x+va_position.y),va_position);\n"
            "	float c = cos(u_time), s = sin(u_time);"
            "	vec2 t = mat2(c, s, -s, c)*(va_position-vec2(0.33));\n"
            "	gl_Position = vec4(t.x*3.0/5.0, t.y, 0.0, 1.0);\n"
            "}\n";

    const char *shader_frag_src =
            "#ifdef GL_ES\n"
            "precision mediump float;\n"
            "#endif\n"
            "varying vec3 v_color;\n"
            "void main() {\n"
            "	gl_FragColor = vec4(v_color, 1.0);\n"
            "}\n";

    GLint is_compiled;
    GLuint program, shader_vert, shader_frag;
    program = glCreateProgram();
    shader_vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(shader_vert, 1, &shader_vert_src, NULL);
    glCompileShader(shader_vert);
    glGetShaderiv(shader_vert, GL_COMPILE_STATUS, &is_compiled);
    printf("vert shader compiled %d\n", is_compiled);
    glAttachShader(program, shader_vert);
    shader_frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(shader_frag, 1, &shader_frag_src, NULL);
    glCompileShader(shader_frag);
    glGetShaderiv(shader_frag, GL_COMPILE_STATUS, &is_compiled);
    printf("frag shader compiled %d\n", is_compiled);
    glAttachShader(program, shader_frag);
    glLinkProgram(program);
    glUseProgram(program);
    GLuint u_time_loc = glGetUniformLocation(program, "u_time");
    float u_time = 0.0f;

    //create vbo TODO
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    float vertex_data[] = {0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f};


    //setup vertex attributes TODO

    return 0;
}

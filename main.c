#include <stdio.h>
#include <stdlib.h>
//#include <stdbool.h>
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
        }
    }
    return true;
}

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

    //create vbo
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    float vertex_data[] = {0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f};
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);

    //setup vertex attributes
    GLuint  va_position = 0;
    glEnableVertexAttribArray(va_position);
    glVertexAttribPointer(va_position, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

    glClearColor(0.4, 0.6, 0.8, 1.0);

    bool running = true;
    do{
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            running = !(event.type == SDL_QUIT
                    || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE ));
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUniform1f(u_time_loc, u_time += 1.0f/60.0f);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        SDL_GL_SwapWindow(sdl_window);

    } while (running);

    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDeleteProgram(program);
    glDeleteBuffers(1, &vbo);

    return 0;
}

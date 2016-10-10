/* nuklear - v1.09 - public domain */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_thread.h>

#include <soundpipe.h>
#include <sporth.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_SDL_GL2_IMPLEMENTATION
#include "nuklear.h"
#include "nuklear_sdl_gl2.h"
#include "scheme-private.h"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

#define MAX_VERTEX_MEMORY 512 * 1024
#define MAX_ELEMENT_MEMORY 128 * 1024

#define UNUSED(a) (void)a
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) < (b) ? (b) : (a))
#define LEN(a) (sizeof(a)/sizeof(a)[0])

/* macros needed for car/cdr operations */
#define car(p) ((p)->_object._cons._car)
#define cdr(p) ((p)->_object._cons._cdr)

/* ===============================================================
 *
 *                          EXAMPLE
 *
 * ===============================================================*/
/* This are some code examples to provide a small overview of what can be
 * done with this library. To try out an example uncomment the include
 * and the corresponding function. */
#include "style.c"
/* #include "../calculator.c" */
/*#include "../overview.c"*/
/* #include "../node_editor.c" */

/* ===============================================================
 *
 *                          DEMO
 *
 * ===============================================================*/

typedef struct {
    /* Platform */
    SDL_Window *win;
    SDL_GLContext glContext;
    struct nk_color background;
    int win_width, win_height;
    int running;
    nk_flags flags;


    /* GUI */
    struct nk_context *ctx;
    float val;
    struct nk_panel layout;

    /* Thread */
    SDL_Thread *thread;

    /* Scheme */
    scheme *sc;
    pointer cb;
} my_struct;

static int run_program(void *ud)
{
    my_struct *dat = ud;

    /* SDL setup */
    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    dat->win = SDL_CreateWindow("Demo",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN|SDL_WINDOW_ALLOW_HIGHDPI);
    dat->glContext = SDL_GL_CreateContext(dat->win);
    SDL_GetWindowSize(dat->win, &dat->win_width, &dat->win_height);
    dat->ctx = nk_sdl_init(dat->win);

    /* GUI */
    /* Load Fonts: if none of these are loaded a default font will be used  */
    /* Load Cursor: if you uncomment cursor loading please hide the cursor */
    {struct nk_font_atlas *atlas;
    nk_sdl_font_stash_begin(&atlas);
    /*struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "../../../extra_font/DroidSans.ttf", 14, 0);*/
    /*struct nk_font *roboto = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Roboto-Regular.ttf", 16, 0);*/
    /*struct nk_font *future = nk_font_atlas_add_from_file(atlas, "../../../extra_font/kenvector_future_thin.ttf", 13, 0);*/
    /*struct nk_font *clean = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyClean.ttf", 12, 0);*/
    /*struct nk_font *tiny = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyTiny.ttf", 10, 0);*/
    /*struct nk_font *cousine = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Cousine-Regular.ttf", 13, 0);*/
    nk_sdl_font_stash_end();
    /*nk_style_load_all_cursors(ctx, atlas->cursors);*/
    /*nk_style_set_font(ctx, &roboto->handle)*/;}

    /* style.c */
    /* set_style(dat.ctx, THEME_WHITE); */
    /*set_style(ctx, THEME_RED);*/
    /* set_style(dat.ctx, THEME_BLUE); */
    set_style(dat->ctx, THEME_DARK);
    dat->val = 0;
    dat->background = nk_rgb(128,128,128);
    
    struct nk_context *ctx = dat->ctx;
    while (dat->running)
    {
        /* Input */
        SDL_Event evt;
        nk_input_begin(ctx);
        while (SDL_PollEvent(&evt)) {
            if (evt.type == SDL_QUIT) {
                dat->running = 0;
            }
            nk_sdl_handle_event(&evt);
        }
        nk_input_end(dat->ctx);

        /* GUI */
        {
            if(dat->cb != dat->sc->NIL) {
                scheme_call(dat->sc, dat->cb, dat->sc->NIL);
            }
        }

        /* -------------- EXAMPLES ---------------- */
        /* calculator(ctx); */
        /*overview(ctx);*/
        /* node_editor(ctx); */
        /* ----------------------------------------- */

        /* Draw */
        {float bg[4];
        nk_color_fv(bg, dat->background);
        SDL_GetWindowSize(dat->win, &dat->win_width, &dat->win_height);
        glViewport(0, 0, dat->win_width, dat->win_height);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(bg[0], bg[1], bg[2], bg[3]);
        /* IMPORTANT: `nk_sdl_render` modifies some global OpenGL state
         * with blending, scissor, face culling, depth test and viewport and
         * defaults everything back into a default state.
         * Make sure to either a.) save and restore or b.) reset your own state after
         * rendering the UI. */
        nk_sdl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY, MAX_ELEMENT_MEMORY);
        SDL_GL_SwapWindow(dat->win);}
    }

    return 0;
}
/*
int main(int argc, char* argv[])
{
    my_struct dat;
    dat.running = 1;
    dat.val = 0;
    dat.thread = SDL_CreateThread(run_program, "thread", &dat);
    fgetc(stdin);
    dat.running = 0;
    SDL_WaitThread(dat.thread, NULL);

    nk_sdl_shutdown();
    SDL_GL_DeleteContext(dat.glContext);
    SDL_DestroyWindow(dat.win);
    SDL_Quit();
    return 0;
}
*/

static my_struct g_dat;

static pointer nuklear_start(scheme *sc, pointer args) {
    g_dat.running = 1;
    g_dat.val = 0;
    g_dat.cb = car(args);
    g_dat.thread = SDL_CreateThread(run_program, "thread", &g_dat);
    return sc->NIL;
}

static pointer nuklear_stop(scheme *sc, pointer args) {

    SDL_WaitThread(g_dat.thread, NULL);
    nk_sdl_shutdown();
    SDL_GL_DeleteContext(g_dat.glContext);
    SDL_DestroyWindow(g_dat.win);
    SDL_Quit();
    return sc->NIL;
}

static pointer nuklear_slider(scheme *sc, pointer args) {
    SPFLOAT *val = (SPFLOAT *)string_value(car(args));
    args = cdr(args);
    const char *name = string_value(car(args));
    nk_label(g_dat.ctx, name, NK_TEXT_LEFT);
    nk_slider_float(g_dat.ctx, 0, val, 1, 0.01);
    return sc->NIL;
}

static pointer nuklear_begin(scheme *sc, pointer args) {
    const char *name = string_value(car(args));
    args = cdr(args);
    int x = ivalue(car(args));
    args = cdr(args);
    int y = ivalue(car(args));
    args = cdr(args);
    int w = ivalue(car(args));
    args = cdr(args);
    int h = ivalue(car(args));
    args = cdr(args);

    int rc = nk_begin(g_dat.ctx, &g_dat.layout, name, nk_rect(x, y, w, h), g_dat.flags);
    if(!rc) {
        fprintf(stderr, "error!\n");
    }
    /* TODO: return rc */
    return sc->NIL;
}

static pointer nuklear_end(scheme *sc, pointer args) {
    nk_end(g_dat.ctx);
    return sc->NIL;
}

static pointer nuklear_row_dynamic(scheme *sc, pointer args) {
    float height = rvalue(car(args));
    args = cdr(args);
    int columns = ivalue(car(args));
    nk_layout_row_dynamic(g_dat.ctx, height, columns);
    return sc->NIL;
}

void init_nuklear(scheme *sc) 
{
    g_dat.sc = sc;
    g_dat.cb = sc->NIL;
    g_dat.flags = NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
        NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE;
    scheme_define(sc, sc->global_env, 
        mk_symbol(sc, "nuklear-start"), 
        mk_foreign_func(sc, nuklear_start));
    scheme_define(sc, sc->global_env, 
        mk_symbol(sc, "nuklear-stop"), 
        mk_foreign_func(sc, nuklear_start));
    scheme_define(sc, sc->global_env, 
        mk_symbol(sc, "nuklear-slider"), 
        mk_foreign_func(sc, nuklear_slider));
    scheme_define(sc, sc->global_env, 
        mk_symbol(sc, "nuklear-begin"), 
        mk_foreign_func(sc, nuklear_begin));
    scheme_define(sc, sc->global_env, 
        mk_symbol(sc, "nuklear-end"), 
        mk_foreign_func(sc, nuklear_end));
    scheme_define(sc, sc->global_env, 
        mk_symbol(sc, "nuklear-row-dynamic"), 
        mk_foreign_func(sc, nuklear_row_dynamic));
}

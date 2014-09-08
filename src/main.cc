#include <iostream>
#include <string>
#include <algorithm>
#include <cstdio>
#include <cmath>

#include <SDL2/SDL.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include "src/exception.h"
#include "src/bsp.h"
#include "src/archive.h"
#include "src/time.h"
#include "src/math/vector3.h"
#include "src/math/vector4.h"
#include "src/math/matrix4.h"
#include "src/math/util.h"

class Render
{
    public:
        Render(int, int);
        ~Render() noexcept;

        void new_frame() const;
        void end_frame() const;

        void resize(int, int);

    private:
        int             m_width;
        int             m_height;
        SDL_Window*     m_window;
        SDL_GLContext   m_context;
};

Render::Render(const int width, const int height)
    : m_width(width), m_height(height)
{
    m_window = SDL_CreateWindow(
            "q3bsp",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            m_width,
            m_height,
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!m_window) {
        throwf("SDL_CreateWindow: %s", SDL_GetError());
    }

    m_context = SDL_GL_CreateContext(m_window);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 0);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);

    glEnable(GL_COLOR_ARRAY);
    glEnable(GL_TEXTURE_COORD_ARRAY);
    glEnable(GL_VERTEX_ARRAY);

    glShadeModel(GL_SMOOTH);

    glClearDepth(1.0);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);

    glEnable(GL_TEXTURE_2D);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    resize(m_width, m_height);
}

Render::~Render() noexcept
{
    SDL_GL_DeleteContext(m_context);
}

void Render::resize(int width, int height)
{
    m_width = width;
    m_height = height;

    glViewport(0, 0, m_width, m_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(75.0, m_width / double(m_height), 4.0, 15000.0);
}

void Render::new_frame() const
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Render::end_frame() const
{
    SDL_GL_SwapWindow(m_window);
}

class Simulation
{
    public:
        vec3 speed;
        vec3 position;

        void step(vec3 dir, const float dt)
        {
            const float m = dir.length();
            if (m > 1.0f) {
                dir *= 1.0f / m;
            }
            speed += dir * dt;
            speed *= std::pow(0.87f, dt);
            position += speed * dt;
        }
};

namespace
{
    void process_events(bool* done, Render* render, float* yaw, float* pitch,
            float*)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: {
                    *done = true;
                    break;
                }
                case SDL_WINDOWEVENT: {
                    switch (event.window.event) {
                        case SDL_WINDOWEVENT_RESIZED: {
                            render->resize(event.window.data1,
                                    event.window.data2);
                            break;
                        }
                    }
                    break;
                }
                case SDL_KEYUP: {
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        *done = true;
                    }
                    else if (event.key.keysym.sym == SDLK_f) {
                        SDL_SetRelativeMouseMode(SDL_FALSE);
                    }
                    break;
                }
                case SDL_MOUSEBUTTONDOWN: {
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        SDL_SetRelativeMouseMode(SDL_TRUE);
                    }
                    break;
                }
                case SDL_MOUSEMOTION: {
                    static int old_mode = SDL_GetRelativeMouseMode();
                    int cur_mode = SDL_GetRelativeMouseMode();
                    if (cur_mode != old_mode) {
                        // After a switch, SDL2 generates a spurious
                        // MOUSEMOTION event with *huge* deltas.
                    }
                    else if (cur_mode) {
                        float xr = event.motion.xrel / 6.0f;
                        float yr = event.motion.yrel / 6.0f;
                        *yaw += std::fmod(xr, 360.0f);
                        *pitch = std::min(90.0f, std::max(-90.0f, *pitch + yr));
                    }
                    old_mode = cur_mode;
                    break;
                }
            }
        }
    }

    float step()
    {
        static std::uint64_t total_frames = 0;
        static std::int64_t start_ticks = get_ticks();
        static std::int64_t last_update = 0;

        static TickQueue tq;
        float dt = tq.new_frame(60);

        ++total_frames;
        std::int64_t curr_ticks = get_ticks() - start_ticks;
        if (get_ticks() - last_update >= TICKS_PER_SECOND / 10) {
            float avg_fps = total_frames / (curr_ticks /
                    float(TICKS_PER_SECOND));
            std::printf(
                    "\r"
                    "%0.2f frames/sec, "
                    "%0.3f msec/frame, "
                    "%0.2f frames/sec avg  \b\b",
                    tq.get_frames_per_second(),
                    tq.get_seconds_per_frame() * 1000.0f,
                    avg_fps);
            std::fflush(stdout);
            last_update = get_ticks();
        }

        return dt;
    }

    mat4 simulate(const float dt, const mat4& mdir, Simulation* sim)
    {
        const Uint8* keys = SDL_GetKeyboardState(nullptr);
        vec4 movement;
        if (keys[SDL_SCANCODE_W]) {
            movement += vec4(0.0f, 0.0f, -1.0f) * mdir;
        }
        if (keys[SDL_SCANCODE_S]) {
            movement += vec4(0.0f, 0.0f, 1.0f) * mdir;
        }
        if (keys[SDL_SCANCODE_A]) {
            movement += vec4(-1.0f, 0.0f, 0.0f) * mdir;
        }
        if (keys[SDL_SCANCODE_D]) {
            movement += vec4(1.0f, 0.0f, 0.0f) * mdir;
        }
        if (keys[SDL_SCANCODE_SPACE]) {
            movement += vec4(0.0f, 1.0f, 0.0f);
        }
        if (keys[SDL_SCANCODE_LCTRL] || keys[SDL_SCANCODE_C]) {
            movement += vec4(0.0f, -1.0f, 0.0f);
        }
        sim->step(vec3(movement), dt * 50.0f);

        mat4 mat;
        mat4_translate(mat, -sim->position.x, -sim->position.y, -sim->position.z);
        return mat * mdir;
    }

    void loop(Render& render, const MapBSP46& map)
    {
        float yaw = 0.0f, pitch = 0.0f, roll = 0.0f;
        Simulation sim;

        SDL_SetRelativeMouseMode(SDL_TRUE);

        bool done = false;
        while (!done) {
            float dt = step();

            process_events(&done, &render, &yaw, &pitch, &roll);
            mat4 mdir;
            mat4_rotate_y(mdir, yaw);
            mat4_rotate_x(mdir, pitch);
            mat4_rotate_z(mdir, roll);
            mat4 mat = simulate(dt, mdir, &sim);

            render.new_frame();
            glMatrixMode(GL_MODELVIEW);
            glLoadMatrixf(mat.get_floats());
            map.draw(sim.position, GLFrustum<float>());
            render.end_frame();
        }

        printf("\n");
    }
}

int main(int argc, char* argv[])
{
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <path> <map>" << std::endl;
        return 1;
    }
    std::string bsp_filename = "maps/";
    bsp_filename += argv[2];
    bsp_filename += ".bsp";

    SDL_Init(SDL_INIT_EVERYTHING);
    try {
        Uint32 mticks = SDL_GetTicks();
        PAK3Archive pak(argv[1]);

        /* Render render(1440, 900); */
        Render render(1440, 800);
        MapBSP46 map(bsp_filename.c_str(), pak);

        std::printf("Init: %0.2f sec\n", (SDL_GetTicks() - mticks) / 1000.0f);
        loop(render, map);
    }
    catch (const QException& e) {
        std::cerr << argv[0] << ": Error: " << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << argv[0] << ": Error: Unknown exception" << std::endl;
        return 1;
    }
    SDL_Quit();
    return 0;
}

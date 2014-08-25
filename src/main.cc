#include <iostream>
#include <string>
#include <algorithm>
#include <cstdio>
#include <cmath>

#include <SDL2/SDL.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include "exception.h"
#include "bsp.h"
#include "vec.h"
#include "archive.h"
#include "tex.h"
#include "frustum.h"
#include "time.h"

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
    gluPerspective(60.0, m_width / double(m_height), 4.0, 15000.0);
}

void Render::new_frame() const
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Render::end_frame() const
{
    SDL_GL_SwapWindow(m_window);
}

class Physics : public CVec
{
    private:
        CVec m_speed;

    public:
        Physics(const float px = 0.0f, const float py = 0.0f,
                const float pz = 0.0f)
            : CVec(px, py, pz)
        {}

        void apply_acceleration(const CVec& dir, const float)
        {
            m_speed += dir;
            const float m = m_speed.Magnitude();
            if (m > 1.0f) {
                m_speed *= 1.0f / m;
            }
        }

        void apply_friction(const float /* acc */)
        {
            m_speed *= 0.85f;
        }

        void apply_speed(const float acc)
        {
            *this += m_speed * acc;
        }
};

void process_events(bool* done, Render* render, float* yaw, float* pitch, float*)
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
                        render->resize(event.window.data1, event.window.data2);
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
                    // MOUSEMOTION event with *huge* deltas,
                    // that should be ignored.
                }
                else if (cur_mode) {
                    *yaw += std::fmod(
                            event.motion.xrel / 7.0f,
                            360.0f);
                    *pitch = std::min(
                            90.0f,
                            std::max(
                                -90.0f,
                                *pitch + event.motion.yrel / 7.0f));
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
    static std::uint64_t start_ticks = get_ticks();
    static std::uint64_t last_update = 0;

    static TickQueue tq;
    float delta = tq.new_frame(75) * 60.0f;

    ++total_frames;
    std::uint64_t curr_ticks = get_ticks() - start_ticks;
    if (get_ticks() - last_update >= TICKS_PER_SECOND / 10) {
        float avg_fps = total_frames / (curr_ticks / float(TICKS_PER_SECOND));
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

    return delta;
}

CMat physics(const float acc, const CMat& mdir, Physics* cam)
{
    const Uint8* keys = SDL_GetKeyboardState(nullptr);
    if (keys[SDL_SCANCODE_W]) {
        CVec delta(0.0f, 0.0f, -0.2f);
        delta *= mdir;
        cam->apply_acceleration(delta, acc);
    }
    if (keys[SDL_SCANCODE_S]) {
        CVec delta(0.0f, 0.0f, 0.2f);
        delta *= mdir;
        cam->apply_acceleration(delta, acc);
    }
    if (keys[SDL_SCANCODE_A]) {
        CVec delta(-0.2f, 0.0f, 0.0f);
        delta *= mdir;
        cam->apply_acceleration(delta, acc);
    }
    if (keys[SDL_SCANCODE_D]) {
        CVec delta(0.2f, 0.0f, 0.0f);
        delta *= mdir;
        cam->apply_acceleration(delta, acc);
    }
    if (keys[SDL_SCANCODE_SPACE]) {
        CVec delta(0.0f, 0.2f, 0.0f);
        cam->apply_acceleration(delta, acc);
    }
    if (keys[SDL_SCANCODE_LCTRL] || keys[SDL_SCANCODE_C]) {
        CVec delta(0.0f, -0.2f, 0.0f);
        cam->apply_acceleration(delta, acc);
    }

    cam->apply_speed(acc);
    cam->apply_friction(acc);

    CMat mat;
    MatTrans(mat, -cam->x, -cam->y, -cam->z);
    return mat * mdir;
}

void loop(Render& render, const CBspQ3& bsp)
{
    float yaw = 0.0f, pitch = 0.0f, roll = 0.0f;
    Physics cam;

    SDL_SetRelativeMouseMode(SDL_TRUE);

    bool done = false;
    while (!done) {
        float acc = step();

        process_events(&done, &render, &yaw, &pitch, &roll);
        CMat mdir;
        MatYRot(mdir, yaw);
        MatXRot(mdir, pitch);
        MatZRot(mdir, roll);
        CMat mat = physics(acc, mdir, &cam);

        render.new_frame();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glLoadMatrixf(mat.GetMatrix());
        CFrustum frustum;
        bsp.Render(cam, frustum);
        render.end_frame();
    }

    printf("\n");
}

int main(int argc, char* argv[])
{
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] <<
            " <path> <map> [<overbright bits>]" << std::endl;
        return 1;
    }
    if (argc >= 4) {
        int overbrightBits = atoi(argv[3]);
        if (overbrightBits < 0 || overbrightBits > 8) {
            std::cerr << argv[0] << ": overbright bits: allowed range is 0-8" <<
                std::endl;
            return 1;
        }
        CLightmapTex::SetOverbrightBits(overbrightBits);
    }

    SDL_Init(SDL_INIT_EVERYTHING);

    try {
        Uint32 mticks = SDL_GetTicks();
        PAK3Archive pak(argv[1]);

        Render render(1440, 900);

        std::string filename = "maps/";
        filename += argv[2];
        filename += ".bsp";
        CBspQ3 bsp(filename.c_str(), pak);

        std::printf("Init: %0.2f sec\n", (SDL_GetTicks() - mticks) / 1000.0f);
        loop(render, bsp);
    }
    catch (const QException& e) {
        std::cerr << argv[0] << ": Error: " << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << argv[0] << ": Error: Unknown exception" << std::endl;
        return 1;
    }

    CTexManager::Destroy();
    SDL_Quit();

    return 0;
}

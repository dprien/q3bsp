#include <iostream>
#include <string>
#include <deque>
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

class Render
{
    private:
        int             m_width;
        int             m_height;
        SDL_Window*     m_window;
        SDL_GLContext   m_context;

    public:
        Render(int, int);
        ~Render() noexcept;

        void new_frame() const;
        void end_frame() const;
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
            SDL_WINDOW_OPENGL);
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

    glViewport(0, 0, m_width, m_height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, m_width / double(m_height), 4.0, 15000.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

Render::~Render() noexcept
{
    SDL_GL_DeleteContext(m_context);
}

void Render::new_frame() const
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Render::end_frame() const
{
    SDL_GL_SwapWindow(m_window);
}

class FrameTimer
{
    private:
        Uint32  m_start_ticks;
        Uint32  m_total_ticks;
        Uint32  m_frame_ticks;

    public:
        FrameTimer()
            : m_start_ticks(SDL_GetTicks()), m_total_ticks(0), m_frame_ticks(0)
        {}

        Uint32 get_total_ticks() const
        {
            return m_total_ticks;
        }

        Uint32 get_frame_ticks() const
        {
            return m_frame_ticks;
        }

        void update()
        {
            Uint32 cur_ticks = SDL_GetTicks() - m_start_ticks;
            m_frame_ticks = cur_ticks - m_total_ticks;
            m_total_ticks = cur_ticks;
        }
};

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

class FrameStats
{
    typedef std::deque<Uint32> tick_queue_t;

    const Uint32    m_max_ticks;
    tick_queue_t    m_tick_queue;
    Uint32          m_ticks;
    unsigned long   m_frames;

    public:
        FrameStats(const Uint32 max_ticks = 1000)
            : m_max_ticks(max_ticks), m_ticks(0), m_frames(0)
        {}

        Uint32 get_ticks() const
        {
            return m_ticks;
        };

        unsigned long get_frames() const
        {
            return m_frames;
        }

        void new_frame(const Uint32 ticks)
        {
            m_tick_queue.push_back(ticks);
            m_ticks += ticks;
            ++m_frames;
            while (m_ticks > m_max_ticks) {
                m_ticks -= m_tick_queue.front();
                m_tick_queue.pop_front();
                --m_frames;
            }
        }
};

void process_events(bool* done, float* yaw, float* pitch, float*)
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT: {
                *done = true;
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
    static unsigned long total_frames = 0;
    static unsigned long refresh_time = 0;
    static FrameTimer ft;
    static FrameStats fps_stats;

    ft.update();
    fps_stats.new_frame(ft.get_frame_ticks());
    refresh_time += ft.get_frame_ticks();

    if (refresh_time >= 250) {
        refresh_time = 0;

        float fps = fps_stats.get_frames() / (fps_stats.get_ticks() / 1000.0f);
        float mspf = fps_stats.get_ticks() / float(fps_stats.get_frames());
        float avg = total_frames / (ft.get_total_ticks() / 1000.0f);
        std::printf(
                "\r"
                "%0.2f frames/sec, "
                "%0.2f msec/frame, "
                "%0.2f frames/sec avg  \b\b",
                fps, mspf, avg);
        std::fflush(stdout);
    }
    ++total_frames;

    return ft.get_frame_ticks() / 3.0f;
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

void loop(const CBspQ3& bsp, const Render& render)
{
    float yaw = 0.0f, pitch = 0.0f, roll = 0.0f;
    Physics cam;

    SDL_SetRelativeMouseMode(SDL_TRUE);

    bool done = false;
    while (!done) {
        process_events(&done, &yaw, &pitch, &roll);
        CMat mdir;
        MatYRot(mdir, yaw);
        MatXRot(mdir, pitch);
        MatZRot(mdir, roll);

        float acc = step();
        CMat mat = physics(acc, mdir, &cam);

        render.new_frame();
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

        Render render(1280, 800);

        std::string filename = "maps/";
        filename += argv[2];
        filename += ".bsp";
        CBspQ3 bsp(filename.c_str(), pak);

        std::printf("Init: %0.2f sec\n", (SDL_GetTicks() - mticks) / 1000.0f);
        loop(bsp, render);
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

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <locale>
#include <deque>

using std::cout;
using std::cerr;
using std::endl;

#include <clocale>

#include <GL/gl.h>
#include <GL/glu.h>

#include "SDL.h"

#include "exception.h"
#include "sdlsentry.h"
#include "bsp.h"
#include "vec.h"
#include "zipfile.h"
#include "tex.h"
#include "frustum.h"

class CSdlGl
{
    int         m_width;
    int         m_height;
    int         m_bpp;

    CSdlSentry  m_videoInit;

    SDL_Surface *m_surf;

public:
    CSdlGl(int, int);
    ~CSdlGl() throw ();

    void NewFrame() const;
    void EndFrame() const;
};

CSdlGl::CSdlGl(int width, int height)
    : m_width(width), m_height(height), m_videoInit(SDL_INIT_VIDEO)
{
    const SDL_VideoInfo *info = SDL_GetVideoInfo();

    if (!info)
        Throwf<CException>("SDL_GetVideoInfo: %s", SDL_GetError());

    m_bpp = info->vfmt->BitsPerPixel;

    std::ostringstream caption;
    caption << m_width << "x" << m_height << " pixels, " << m_bpp << " bpp";

    SDL_WM_SetCaption(caption.str().c_str(), "");
    SDL_ShowCursor(SDL_DISABLE);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 0);

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    Uint32 flags = SDL_OPENGL | SDL_HWPALETTE; // | SDL_FULLSCREEN;
    m_surf = SDL_SetVideoMode(m_width, m_height, m_bpp, flags);

    if (!m_surf)
        Throwf<CException>("SDL_SetVideoMode: %s", SDL_GetError());

    SDL_SetGamma(1.4f, 1.4f, 1.4f);

    // Initialize OpenGL
    // =====================================================================
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

CSdlGl::~CSdlGl() throw ()
{
}

void CSdlGl::NewFrame() const
{
    glClear(GL_DEPTH_BUFFER_BIT);
}

void CSdlGl::EndFrame() const
{
    SDL_GL_SwapBuffers();
}

// =======================================================================
// =======================================================================

class CFrameTimer
{
    Uint32  m_startTicks;
    Uint32  m_totalTicks;
    Uint32  m_frameTicks;

public:
	CFrameTimer()
		: m_startTicks(SDL_GetTicks()), m_totalTicks(0), m_frameTicks(0)
	{
	}

	Uint32 GetTotalTicks() const
	{
		return m_totalTicks;
	}

	Uint32 GetFrameTicks() const
	{
		return m_frameTicks;
	}

    void Update();
};

void CFrameTimer::Update()
{
    Uint32 curTicks = SDL_GetTicks() - m_startTicks;

    m_frameTicks = curTicks - m_totalTicks;
    m_totalTicks = curTicks;
}

// ==========================================================================
// ==========================================================================

class CMovement : public CVec
{
    CVec    m_speed;

public:
    CMovement(float = 0.0f, float = 0.0f, float = 0.0f);

    void ApplyAcceleration(const CVec &, float);
    void ApplyFriction(float);
    void ApplySpeed(float);
};

inline CMovement::CMovement(float px, float py, float pz)
    : CVec(px, py, pz)
{
}

inline void CMovement::ApplyAcceleration(const CVec &dir, float)
{
    m_speed += dir;
    float m = m_speed.Magnitude();

    if (m > 1.0f)
        m_speed *= 1.0f / m;
}

inline void CMovement::ApplyFriction(float)
{
    m_speed *= 0.85f;
}

inline void CMovement::ApplySpeed(float acc)
{
    *this += m_speed * acc;
}

// ==========================================================================
// ==========================================================================

class CFrameStats
{
	typedef std::deque<Uint32> tickQueue_t;

	const Uint32	m_maxTicks;
	
	tickQueue_t		m_tickQueue;
	Uint32			m_cumulTicks;
	
	unsigned long	m_cumulFrames;

	public:
	CFrameStats(Uint32 maxTicks = 1000)
		: m_maxTicks(maxTicks), m_cumulTicks(0), m_cumulFrames(0)
	{
	}	
	
	Uint32 GetCumulTicks() const
	{
		return m_cumulTicks;
	};
	
	unsigned long GetCumulFrames() const
	{
		return m_cumulFrames;
	}
	
	void NewFrame(Uint32);
};

void CFrameStats::NewFrame(Uint32 ticks)
{
	m_tickQueue.push_back(ticks);
	m_cumulTicks  += ticks;
	
	++m_cumulFrames;

	while (m_cumulTicks > m_maxTicks)
	{
		m_cumulTicks -= m_tickQueue.front();
		m_tickQueue.pop_front();

		--m_cumulFrames;
	}
}

// ==========================================================================
// ==========================================================================

void Loop(CBspQ3 &bsp, CSdlGl &render)
{
    unsigned long totalFrames = 0, refreshTime = 0;
	
	bool isDone = false;
    float pitch = 0.0f, yaw = 0.0f;
    CMovement cam;

    CFrameTimer ft;
	CFrameStats fpsStats;

    while (!isDone)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    isDone = true;
                    break;

                case SDL_KEYUP:
                    if (event.key.keysym.sym == SDLK_ESCAPE)
                        isDone = true;

                    break;

                case SDL_MOUSEMOTION:
                    yaw     += event.motion.xrel / 4.0f;
                    pitch   += event.motion.yrel / 4.0f;

                    if (pitch >  90.0f) pitch =  90.0f;
                    if (pitch < -90.0f) pitch = -90.0f;

                    break;
            }
        }

        ft.Update();
		float acc = ft.GetFrameTicks() / 3.0f;

		fpsStats.NewFrame(ft.GetFrameTicks());
		refreshTime += ft.GetFrameTicks();

		if (refreshTime >= 250)
		{
	        cout << "\r" << std::fixed << std::setprecision(2);

			cout <<
				fpsStats.GetCumulFrames() / (fpsStats.GetCumulTicks() / 1000.0) <<
				" frames/sec | ";

			cout <<
				fpsStats.GetCumulTicks() / double(fpsStats.GetCumulFrames()) <<
				" msec/frame | ";

			cout << "avg: " <<
				totalFrames / (ft.GetTotalTicks() / 1000.0) <<
				" frames/sec";
			
			cout << "  \b\b" << std::flush;

			refreshTime = 0;
		}

		const Uint32 minFrameTicks = 1000 / 1000;

		static Uint32 prevTotalTicks = SDL_GetTicks();		
		Uint32 curTotalTicks = SDL_GetTicks();

		Uint32 curTickDiff = curTotalTicks - prevTotalTicks;
		prevTotalTicks = curTotalTicks;

		if (curTickDiff < minFrameTicks)
		{
			SDL_Delay(minFrameTicks - curTickDiff);			
			prevTotalTicks = SDL_GetTicks();
		}

		++totalFrames;

        CMat mdir;

        MatYRot(mdir, yaw);
        MatXRot(mdir, pitch);

        int button = SDL_GetMouseState(0, 0);
        Uint8 *keys = SDL_GetKeyState(0);

        if (keys[SDLK_SPACE])
        {
            CVec delta(0.0f, 0.2f, 0.0f);
            cam.ApplyAcceleration(delta, acc);
        }

        if (keys[SDLK_LCTRL] || keys[SDLK_c])
        {
            CVec delta(0.0f, -0.2f, 0.0f);
            cam.ApplyAcceleration(delta, acc);
        }

        if (keys[SDLK_a])
        {
            CVec delta(-0.2f, 0.0f, 0.0f);
            delta *= mdir;

            cam.ApplyAcceleration(delta, acc);
        }

        if (keys[SDLK_d])
        {
            CVec delta(0.2f, 0.0f, 0.0f);
            delta *= mdir;

            cam.ApplyAcceleration(delta, acc);
        }

        if (keys[SDLK_s])
        {
            CVec delta(0.0f, 0.0f, 0.2f);
            delta *= mdir;

            cam.ApplyAcceleration(delta, acc);
        }

        if (keys[SDLK_w])
        {
            CVec delta(0.0f, 0.0f, -0.2f);
            delta *= mdir;

            cam.ApplyAcceleration(delta, acc);
        }

        cam.ApplySpeed(acc);
        cam.ApplyFriction(acc);

        CMat mat;
        MatTrans(mat, -cam.x, -cam.y, -cam.z);
        mat *= mdir;

        render.NewFrame();
		
		glLoadMatrixf(mat.GetMatrix());
		CFrustum frustum;

		if (keys[SDLK_x])
			glLoadIdentity();

		//render.NewFrame();
        bsp.Render(cam, frustum);

		render.EndFrame();
    }

    cout << endl;
}

int main(int argc, char *argv[])
{
	// C++ locale
    std::locale loc("");
    std::locale::global(loc);

    std::cin.imbue(loc);
    cout.imbue(loc);
    cerr.imbue(loc);
    std::clog.imbue(loc);

    // C locale
    std::setlocale(LC_ALL, "");

    if (argc < 3)
    {
        cerr << "Usage: " << argv[0] << " <path> <map> [<overbright bits>]" << endl;
        return 1;
    }

	if (argc >= 4)
	{
		int overbrightBits = atoi(argv[3]);

		if (overbrightBits < 0 || overbrightBits > 8)
		{
			cerr << argv[0] << ": overbright bits: allowed range is 0-8" << endl;
			return 1;
		}

		CLightmapTex::SetOverbrightBits(overbrightBits);
	}

    SDL_Init(SDL_INIT_NOPARACHUTE);

    try
    {
		Uint32 mticks = SDL_GetTicks();
        PAK3Archive pak(argv[1]);

        CSdlGl render(1280, 800);
        SDL_WM_GrabInput(SDL_GRAB_ON);

        std::string filename = "maps/";
        filename += argv[2];
        filename += ".bsp";

        CBspQ3 bsp(filename.c_str(), pak);

        cout << "Init: " << std::fixed << std::setprecision(2) <<
            ((SDL_GetTicks() - mticks) / 1000.0f) << " sec" << endl;

        Loop(bsp, render);
    }
    catch (const CException &e)
    {
        cerr << argv[0] << ": Error: " << e.What() << endl;
        return 1;
    }
    catch (...)
    {
        cerr << argv[0] << ": Error: Unknown exception" << endl;
        return 1;
    }

    CTexManager::Destroy();
    SDL_Quit();

    return 0;
}

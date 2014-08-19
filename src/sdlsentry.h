#ifndef SDLSENTRY__H
#define SDLSENTRY__H

#include "SDL.h"

#include "exception.h"

class CSdlSentry
{
    const Uint32    m_flags;
    const Uint32    m_wasInit;

public:
    explicit CSdlSentry(Uint32 flags)
        : m_flags(flags), m_wasInit(SDL_WasInit(flags))
    {
        if (m_wasInit & m_flags)
            return;

        if (SDL_InitSubSystem(m_flags) == -1)
            throwf<QException>("SDL_InitSubSystem: %s", SDL_GetError());
    }

    ~CSdlSentry() throw ()
    {
        if (!(m_wasInit & m_flags))
            SDL_QuitSubSystem(m_flags);
    }
};

#endif

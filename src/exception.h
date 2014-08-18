// $Id$

#ifndef EXCEPTION__H
#define EXCEPTION__H

#include <string>

#include <cstdio>
#include <cstdarg>

// =================================================
// =================================================

class CException
{
    const std::string  m_message;

public:
    explicit CException(const char *message)
        : m_message(message)
    {
    }

    virtual ~CException() throw ()
    {
    }

    const std::string &What() const
    {
        return m_message;
    }
};

// =================================================
// =================================================

template <class T>
void Throwf(const char *fmt, ...)
{
    const int bufLen = 1024;
    char buf[bufLen];

    va_list ap;
    va_start(ap, fmt);

    std::vsnprintf(buf, bufLen, fmt, ap);
    va_end(ap);

    throw T(buf);
}

#endif

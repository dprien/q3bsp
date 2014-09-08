#ifndef Q3BSP__EXCEPTION_H
#define Q3BSP__EXCEPTION_H

#include <string>

#include <cstdio>
#include <cstdarg>

class QException
{
    public:
        explicit QException(const char* message)
            : m_message(message)
        {}

        const std::string& what() const
        {
            return m_message;
        }

    private:
        const std::string m_message;
};

template <class T = QException>
__attribute__((__format__ (__printf__, 1, 0)))
void throwf(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    char buf[1024];
    std::vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    throw T(buf);
}

#endif

#include <cstdint>

#if defined( _WIN32 )

#include <windows.h>

int64_t GetRunningMicroSecs()
{
    FILETIME now;

    GetSystemTimeAsFileTime( &now );
    uint64_t t = ( UINT64( now.dwHighDateTime ) << 32 ) + now.dwLowDateTime;
    t /= 10;

    return int64_t( t );
}

#elif defined( HAVE_CLOCK_GETTIME )

#include <ctime>

int64_t GetRunningMicroSecs()
{
    struct timespec now;

    clock_gettime( CLOCK_MONOTONIC, &now );

    int64_t usecs = (int64_t) now.tv_sec * 1000000 + now.tv_nsec / 1000;
    //    unsigned msecs = (now.tv_nsec / (1000*1000)) + now.tv_sec * 1000;

    return usecs;
}


#elif defined( HAVE_GETTIMEOFDAY_FUNC )

#include <sys/time.h>
int64_t GetRunningMicroSecs()
{
    timeval tv;

    gettimeofday( &tv, 0 );

    return (int64_t) tv.tv_sec * 1000000 + tv.tv_usec;
}

#endif

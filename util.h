#ifndef _UTIL_H
#define _UTIL_H

#include <string.h>

static inline int is_number(char *str)
{
    for (size_t i = 0; i < strlen(str); i++)
    {
        if (str[i] < '0' || str[i] > '9')
        {
            return 0;
        }
    }
    return 1;
}

static inline const char *get_filename(const char *path)
{
    const char *filename = strrchr(path, '/');
    if (filename == NULL)
    {
        return path;
    }
    return filename + 1;
}

#endif // _UTIL_H

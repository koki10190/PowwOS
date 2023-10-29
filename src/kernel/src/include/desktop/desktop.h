#ifndef DESKTOP_H
#define DESKTOP_H

#include <math/math.h>

typedef struct __desktop_icon_t {
    vector2_t position;
    void *init_function;
    void *icon_render_function;
} desktop_icon_t;

extern desktop_icon_t desktop_icons[128];
extern int desktop_icon_count;

void new_desktop_icon(vector2_t position, void *init_function, void *icon_render_function);

#endif
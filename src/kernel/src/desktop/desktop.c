#include <desktop/desktop.h>

desktop_icon_t desktop_icons[128];
int desktop_icon_count = 0;

void new_desktop_icon(vector2_t position, void *init_function, void *icon_render_function) {
    desktop_icon_t icon;
    icon.position = position;
    icon.init_function = init_function;
    icon.icon_render_function = icon_render_function;
    desktop_icons[desktop_icon_count++] = icon;
}

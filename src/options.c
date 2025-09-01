#include <ctype.h>
#include <string.h>

#include "options.h"

char *get_resolution(GtkDropDown *dropdown)
{
        GtkStringObject *res_obj = gtk_drop_down_get_selected_item(dropdown);
        char *res = g_strdup(gtk_string_object_get_string(res_obj));
        res[strlen(res) - 1] = '\0';
        return res;
}

unsigned int get_audio_quality(GtkDropDown *dropdown)
{
        unsigned int quality = gtk_drop_down_get_selected(dropdown);
        quality *= 2;
        return quality;
}

char *get_format(GtkDropDown *dropdown)
{
        GtkStringObject *format_obj = gtk_drop_down_get_selected_item(dropdown);
        char *format = g_strdup(gtk_string_object_get_string(format_obj));

        for (size_t i = 0; i < strlen(format); i++) {
                format[i] = tolower(format[i]);
        }

        return format;
}

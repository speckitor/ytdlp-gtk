#pragma once

#include <glib.h>
#include <gtk/gtk.h>

char *get_resolution(GtkDropDown *dropdown);
unsigned int get_audio_quality(GtkDropDown *dropdown);
char *get_format(GtkDropDown *dropdown);

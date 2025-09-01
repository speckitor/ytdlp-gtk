#pragma once

#include <gtk/gtk.h>
#include <glib.h>

char *get_resolution(GtkDropDown *dropdown);
unsigned int get_audio_quality(GtkDropDown *dropdown);
char *get_format(GtkDropDown *dropdown);

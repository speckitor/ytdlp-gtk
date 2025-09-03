#pragma once

#include <gtk/gtk.h>
#include <adwaita.h>

typedef enum {
        DOWNLOAD_VIDEO,
        DOWNLOAD_AUDIO
} DownloadType;

typedef struct {
        GtkWindow *win;
        AdwToastOverlay *overlay;
        GtkStack *stack;
        GtkDropDown *video_resolution_drop_down;
        GtkDropDown *video_format_drop_down;
        GtkDropDown *audio_quality_drop_down;
        GtkDropDown *audio_format_drop_down;
        GtkEntry *link_entry;
        GtkEntry *path_entry;
        GtkWidget *downloading_box;
        GtkProgressBar *pbar;
        double pbar_fraction;
        DownloadType type;
        char *url;
        char *format;
        char *resolution;
        unsigned int audio_quality;
} ApplicationContext;

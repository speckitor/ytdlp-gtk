#pragma once

#include <gtk/gtk.h>
#include <adwaita.h>

typedef struct {
        GtkWindow *win;
        AdwToastOverlay *overlay;
        GtkStack *stack;
        GtkDropDown *resolution;
        GtkDropDown *video_format;
        GtkDropDown *audio_quality;
        GtkDropDown *audio_format;
        GtkEntry *link;
        GtkEntry *path;
} ApplicationContext;

typedef enum {
        DOWNLOAD_VIDEO,
        DOWNLOAD_AUDIO
} DownloadType;

typedef struct {
        DownloadType type;
        char *link;
        char *path;
        char *format;
        char *resolution;
        unsigned int audio_quality;
} DownloadConfig;

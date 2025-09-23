#pragma once

#include <adwaita.h>
#include <gtk/gtk.h>

typedef enum {
    DOWNLOAD_VIDEO,
    DOWNLOAD_AUDIO
} DownloadType;

typedef enum {
    DOWNLOAD_RESULT_SUCCESS,
    DOWNLOAD_RESULT_DOWNLOAD_CANCELED,
    DOWNLOAD_RESULT_ALREADY_DOWNLOADED,
    DOWNLOAD_RESULT_UNKNOWN_ERROR
} DownloadResult;

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
    GtkLabel *downloading_label;
    GtkProgressBar *pbar;
    double pbar_fraction;
    DownloadType type;
    DownloadResult result;
    char *url;
    char *format;
    char *resolution;
    unsigned int audio_quality;
} ApplicationContext;

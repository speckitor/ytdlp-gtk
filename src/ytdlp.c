#include <gtk/gtk.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdatomic.h>
#include <stdlib.h>

#include "ytdlp.h"
#include "glib.h"
#include "types.h"

atomic_bool stop_thread = false;

static char *create_url(ApplicationContext *ctx)
{
        char *url;
        char *path = gtk_editable_get_text(GTK_EDITABLE(ctx->path_entry));
        char *link = gtk_editable_get_text(GTK_EDITABLE(ctx->link_entry));
        if (ctx->type == DOWNLOAD_VIDEO) {
                asprintf(
                        &url,
                        "yt-dlp --newline "
                        "-f \"bv[height<=%s]+ba/b[height<=%s]\" "
                        "--merge-output-format %s "
                        "-o \"%s/%(title)s.%(ext)s\" "
                        "%s",
                        ctx->resolution,
                        ctx->resolution,
                        ctx->format,
                        path,
                        link
                );
        } else {
                asprintf(
                        &url,
                        "yt-dlp --newline "
                        "-x --audio-format %s "
                        "--audio-quality %u "
                        "-o \"%s/%(title)s.%(ext)s\" "
                        "%s",
                        ctx->format,
                        ctx->audio_quality,
                        path,
                        link
                );
        }

        return url;
}

static gboolean print_ytdlp_logs(gpointer data) {
        char *line = data;
        g_print(line);
        return FALSE;
}

static gboolean update_pbar(gpointer data)
{
        ApplicationContext *ctx = data;

        if (gtk_widget_get_visible(GTK_WIDGET(ctx->downloading_label)))
                gtk_widget_set_visible(GTK_WIDGET(ctx->downloading_label), false);

        gtk_progress_bar_set_fraction(ctx->pbar, ctx->pbar_fraction / 100.0);

        return FALSE;
}

static gboolean on_download_end(gpointer data)
{
        ApplicationContext *ctx = data;
        gtk_widget_set_visible(ctx->downloading_box, false);

        char *message;
        switch (ctx->result) {
                case DOWNLOAD_RESULT_SUCCESS:
                        message = "File downloaded";
                        break;
                case DOWNLOAD_RESULT_DOWNLOAD_CANCELED:
                        message = "Download canceled";
                        break;
                case DOWNLOAD_RESULT_ALREADY_DOWNLOADED:
                        message = "File already downloaded";
                        break;
                case DOWNLOAD_RESULT_UNKNOWN_ERROR:
                        message = "Unknown error occured";
                        break;
        }


        AdwToast *toast = adw_toast_new(message);
        adw_toast_overlay_add_toast(ctx->overlay, toast);
        return FALSE;
}

static gpointer run_ytdlp_thread(gpointer data)
{
        ApplicationContext *ctx = data;
        FILE *f = popen(ctx->url, "r");
        char buf[1024];

        while (fgets(buf, sizeof(buf), f) && !stop_thread) {
                g_idle_add((GSourceFunc)print_ytdlp_logs, buf);
                if (sscanf(buf, "[download] %lf%", &ctx->pbar_fraction) == 1) {
                        g_idle_add((GSourceFunc)update_pbar, ctx);
                }
        }

        int status = pclose(f);

        char filename[1024];
        if (stop_thread) {
                atomic_store(&stop_thread, false);
                ctx->result = DOWNLOAD_RESULT_DOWNLOAD_CANCELED;
        } else if (sscanf(buf, "[download] %s has already been downloaded", filename)) {
                ctx->result = DOWNLOAD_RESULT_ALREADY_DOWNLOADED;
        } else if (WIFEXITED(status) && WEXITSTATUS(status) == 0){
                ctx->result = DOWNLOAD_RESULT_SUCCESS;
        } else {
                ctx->result = DOWNLOAD_RESULT_UNKNOWN_ERROR;
        }

        
        g_idle_add((GSourceFunc)on_download_end, ctx);
        return NULL;
}

void on_stop_thread_clicked(GtkButton *self)
{
        (void)self;
        atomic_store(&stop_thread, true);
}

int exec_ytdlp(ApplicationContext *ctx)
{
        ctx->url = create_url(ctx);

        g_thread_new("yt-dlp", run_ytdlp_thread, ctx);

        return 0;
}

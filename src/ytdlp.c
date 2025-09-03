#include <gtk/gtk.h>

#include <stdio.h>
#include <stdatomic.h>

#include "ytdlp.h"

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

static gboolean update_pbar(gpointer data)
{
        ApplicationContext *ctx = data;

        gtk_progress_bar_set_fraction(ctx->pbar, ctx->pbar_fraction / 100.0);

        return FALSE;
}

static gboolean hide_downloading_box(gpointer data)
{
        ApplicationContext *ctx = data;
        gtk_widget_set_visible(ctx->downloading_box, false);
        return FALSE;
}

static gpointer run_ytdlp_thread(gpointer data)
{
        ApplicationContext *ctx = data;
        FILE *f = popen(ctx->url, "r");
        char buf[1024];

        while (fgets(buf, sizeof(buf), f) && !stop_thread) {
                if (sscanf(buf, "[download] %lf%", &ctx->pbar_fraction) == 1) {
                        g_idle_add((GSourceFunc)update_pbar, ctx);
                }
        }

        if (stop_thread)
                atomic_store(&stop_thread, false);

        g_idle_add((GSourceFunc)hide_downloading_box, ctx);
        pclose(f);
        return NULL;
}

void on_stop_thread_clicked(GtkButton *self)
{
        atomic_store(&stop_thread, true);
}

int exec_ytdlp(ApplicationContext *ctx)
{
        ctx->url = create_url(ctx);

        g_thread_new("yt-dlp", run_ytdlp_thread, ctx);

        return 0;
}

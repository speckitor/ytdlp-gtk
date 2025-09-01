#include <gtk/gtk.h>

#include <stdio.h>

#include "ytdlp.h"

static char *create_url(DownloadConfig *cfg)
{
        char *url;
        if (cfg->type == DOWNLOAD_VIDEO)
                asprintf(&url, "yt-dlp --newline -f \"bv[height<=%s]+ba/b[height<=%s]\" --merge-output-format %s %s -o \"%s/%(title)s.%(ext)s\"", cfg->resolution, cfg->resolution, cfg->format, cfg->link, cfg->path);
        else
                asprintf(&url, "yt-dlp --newline -x --audio-format %s --audio-quality %u %s -o \"%s/%(title)s.%(ext)s\"", cfg->format, cfg->audio_quality, cfg->link, cfg->path);

        return url;
}

static gboolean update_output(gpointer data)
{
        char *line = data;
        g_print("%s", line);
        g_free(line);
        return FALSE;
}

static gpointer run_ytdlp_thread(gpointer data)
{
        DownloadConfig *cfg = data;
        char *url = create_url(cfg);
        FILE *f = popen(url, "r");
        char buf[1024];

        while (fgets(buf, sizeof(buf), f)) {
                char *line = g_strdup(buf);
                g_idle_add((GSourceFunc)update_output, line);
        }

        pclose(f);
        g_free(url);
        g_free(cfg);
        return NULL;
}

int exec_ytdlp(DownloadConfig *cfg)
{
        g_thread_new("yt-dlp", run_ytdlp_thread, cfg);
        return 0;
}

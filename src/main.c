#include "types.h"
#include "options.h"
#include "ytdlp.h"

static ApplicationContext *ctx;

static void download_file(GtkButton *self, gpointer data)
{
        AdwToast *toast;

        if (gtk_editable_get_text(GTK_EDITABLE(ctx->link))[0] == '\0') { 
                toast = adw_toast_new("No link provided");
                adw_toast_overlay_add_toast(ctx->overlay, toast);
        } else if (gtk_editable_get_text(GTK_EDITABLE(ctx->path))[0] == '\0') {
                toast = adw_toast_new("No path provided");
                adw_toast_overlay_add_toast(ctx->overlay, toast);
        }

        DownloadConfig *cfg = g_new(DownloadConfig, 1);

        cfg->link = gtk_editable_get_text(GTK_EDITABLE(ctx->link));
        cfg->path = gtk_editable_get_text(GTK_EDITABLE(ctx->path));

        if (cfg->path[strlen(cfg->path) - 1] == '/' && strlen(cfg->path) > 0)
                cfg->path[strlen(cfg->path) - 1] = '\0';

        const char *page_name = gtk_stack_get_visible_child_name(ctx->stack);

        if (strcmp(page_name, "Video") == 0) {
                cfg->type = DOWNLOAD_VIDEO;
                cfg->format = get_format(ctx->video_format);
                cfg->resolution = get_resolution(ctx->resolution);
        } else {
                cfg->type = DOWNLOAD_AUDIO;
                cfg->format = get_format(ctx->audio_format);
                cfg->audio_quality = get_audio_quality(ctx->audio_quality);
        }

        exec_ytdlp(cfg);
}

static void dialog_finish(GObject *source_object, GAsyncResult *res, gpointer data)
{
        GtkFileDialog *dialog = GTK_FILE_DIALOG(source_object);
        GFile *dir = gtk_file_dialog_select_folder_finish(dialog, res, NULL);

        if (dir) {
                const char *path = g_file_get_path(dir);
                gtk_editable_set_text(GTK_EDITABLE(ctx->path), path);
                g_object_unref(dir);
        } else {
                g_warning("Dialog was cancelled or failed\n");
        }

        g_object_unref(dialog);
}

static void browse_path(GtkButton *self, gpointer data)
{
        GtkFileDialog *dialog = gtk_file_dialog_new();
        gtk_file_dialog_set_title(dialog, "Select Download Directory");
        gtk_file_dialog_select_folder(dialog, ctx->win, NULL, dialog_finish, NULL);
}

static void activate(GApplication *app)
{
        GtkBuilder *build;
        GtkWindow *win;
        AdwToastOverlay *overlay;
        GtkEntry *link;
        GtkStack *stack;
        GtkStackSwitcher *switcher;
        GtkDropDown *resolution;
        GtkDropDown *video_format;
        GtkDropDown *audio_quality;
        GtkDropDown *audio_format;
        GtkEntry *path;
        GtkButton *browse;
        GtkButton *download;

        build = gtk_builder_new_from_resource("/org/speckitor/ytdlpgtk/window.ui");
        win = GTK_WINDOW(gtk_builder_get_object(build, "win"));
        overlay = ADW_TOAST_OVERLAY(gtk_builder_get_object(build, "overlay"));
        link = GTK_ENTRY(gtk_builder_get_object(build, "link"));
        stack = GTK_STACK(gtk_builder_get_object(build, "stack"));
        switcher = GTK_STACK_SWITCHER(gtk_builder_get_object(build, "switcher"));
        resolution = GTK_DROP_DOWN(gtk_builder_get_object(build, "resolution"));
        video_format = GTK_DROP_DOWN(gtk_builder_get_object(build, "video_format"));
        audio_quality = GTK_DROP_DOWN(gtk_builder_get_object(build, "audio_quality"));
        audio_format = GTK_DROP_DOWN(gtk_builder_get_object(build, "audio_format"));
        path = GTK_ENTRY(gtk_builder_get_object(build, "path"));
        browse = GTK_BUTTON(gtk_builder_get_object(build, "browse"));
        download = GTK_BUTTON(gtk_builder_get_object(build, "download"));

        ctx->win = win;
        ctx->overlay = overlay;
        ctx->stack = stack;
        ctx->resolution = resolution;
        ctx->video_format = video_format;
        ctx->audio_quality = audio_quality;
        ctx->audio_format = audio_format;
        ctx->link = link;
        ctx->path = path;

        gtk_stack_switcher_set_stack(switcher, stack);

        gtk_drop_down_set_selected(resolution, 2);

        g_signal_connect(browse, "clicked", G_CALLBACK(browse_path), NULL);
        g_signal_connect(download, "clicked", G_CALLBACK(download_file), NULL);

        gtk_window_set_application(win, GTK_APPLICATION(app));
        gtk_window_present(win);
}

int main(int argc, char **argv)
{
        AdwApplication *app; 
        int status;
        ctx = g_new(ApplicationContext, 1);

        gtk_init();

        app = adw_application_new("org.speckitor.YtdlpGtk", G_APPLICATION_DEFAULT_FLAGS);
        g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
        status = g_application_run(G_APPLICATION(app), argc, argv);

        g_free(ctx);
        g_object_unref(app);
        return status;
}

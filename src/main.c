#include "gtk/gtk.h"
#include "types.h"
#include "options.h"
#include "ytdlp.h"

static void download_callback(gpointer data)
{
        ApplicationContext *ctx = data;
        AdwToast *toast;

        if (gtk_editable_get_text(GTK_EDITABLE(ctx->link_entry))[0] == '\0') { 
                toast = adw_toast_new("No link provided");
                adw_toast_overlay_add_toast(ctx->overlay, toast);
                return;
        } else if (gtk_editable_get_text(GTK_EDITABLE(ctx->path_entry))[0] == '\0') {
                toast = adw_toast_new("No path provided");
                adw_toast_overlay_add_toast(ctx->overlay, toast);
                return;
        } else if (gtk_widget_get_visible(ctx->downloading_box)) {
                toast = adw_toast_new("You're already downloading a video");
                adw_toast_overlay_add_toast(ctx->overlay, toast);
                return;
        }

        gtk_progress_bar_set_fraction(ctx->pbar, 0.0);

        gtk_widget_set_visible(ctx->downloading_box, true);

        gtk_label_set_text(ctx->downloading_label, "Initializing...");
        gtk_widget_set_visible(GTK_WIDGET(ctx->downloading_label), true);

        const char *page_name = gtk_stack_get_visible_child_name(ctx->stack);

        if (strcmp(page_name, "Video") == 0) {
                ctx->type = DOWNLOAD_VIDEO;
                ctx->format = get_format(ctx->video_format_drop_down);
                ctx->resolution = get_resolution(ctx->video_resolution_drop_down);
        } else {
                ctx->type = DOWNLOAD_AUDIO;
                ctx->format = get_format(ctx->audio_format_drop_down);
                ctx->audio_quality = get_audio_quality(ctx->audio_quality_drop_down);
        }

        exec_ytdlp(ctx);
}

static void dialog_finish(GObject *source_object, GAsyncResult *res, gpointer data)
{
        ApplicationContext *ctx = data;
        GtkFileDialog *dialog = GTK_FILE_DIALOG(source_object);
        GFile *dir = gtk_file_dialog_select_folder_finish(dialog, res, NULL);

        if (dir) {
                const char *path = g_file_get_path(dir);
                gtk_editable_set_text(GTK_EDITABLE(ctx->path_entry), path);
                g_object_unref(dir);
        } else {
                g_warning("Dialog was cancelled or failed\n");
        }

        g_object_unref(dialog);
}

static void browse_path(gpointer data)
{
        ApplicationContext *ctx = data;
        GtkFileDialog *dialog = gtk_file_dialog_new();
        gtk_file_dialog_set_title(dialog, "Select Download Directory");
        gtk_file_dialog_select_folder(dialog, ctx->win, NULL, dialog_finish, ctx);
}

static void activate(GApplication *app)
{
        GtkBuilder *build;
        GtkWindow *win;
        AdwToastOverlay *overlay;
        GtkEntry *link_entry;
        GtkStack *stack;
        GtkStackSwitcher *switcher;
        GtkDropDown *video_resolution_drop_down;
        GtkDropDown *video_format_drop_down;
        GtkDropDown *audio_quality_drop_down;
        GtkDropDown *audio_format_drop_down;
        GtkEntry *path_entry;
        GtkButton *browse;
        GtkWidget *downloading_box;
        GtkLabel *downloading_label;
        GtkProgressBar *pbar;
        GtkButton *stop;
        GtkButton *download;

        build = gtk_builder_new_from_resource("/org/speckitor/ytdlpgtk/window.ui");
        win = GTK_WINDOW(gtk_builder_get_object(build, "win"));
        overlay = ADW_TOAST_OVERLAY(gtk_builder_get_object(build, "overlay"));
        link_entry = GTK_ENTRY(gtk_builder_get_object(build, "link_entry"));
        stack = GTK_STACK(gtk_builder_get_object(build, "stack"));
        switcher = GTK_STACK_SWITCHER(gtk_builder_get_object(build, "switcher"));
        video_resolution_drop_down = GTK_DROP_DOWN(gtk_builder_get_object(build, "video_resolution_drop_down"));
        video_format_drop_down = GTK_DROP_DOWN(gtk_builder_get_object(build, "video_format_drop_down"));
        audio_quality_drop_down = GTK_DROP_DOWN(gtk_builder_get_object(build, "audio_quality_drop_down"));
        audio_format_drop_down = GTK_DROP_DOWN(gtk_builder_get_object(build, "audio_format_drop_down"));
        path_entry = GTK_ENTRY(gtk_builder_get_object(build, "path_entry"));
        browse = GTK_BUTTON(gtk_builder_get_object(build, "browse"));
        downloading_box = GTK_WIDGET(gtk_builder_get_object(build, "downloading_box"));
        downloading_label = GTK_LABEL(gtk_builder_get_object(build, "downloading_label"));
        pbar = GTK_PROGRESS_BAR(gtk_builder_get_object(build, "pbar"));
        stop = GTK_BUTTON(gtk_builder_get_object(build, "stop"));
        download = GTK_BUTTON(gtk_builder_get_object(build, "download"));

        ApplicationContext *ctx = g_new(ApplicationContext, 1);

        ctx->win = win;
        ctx->overlay = overlay;
        ctx->stack = stack;
        ctx->video_resolution_drop_down = video_resolution_drop_down;
        ctx->video_format_drop_down = video_format_drop_down;
        ctx->audio_quality_drop_down = audio_quality_drop_down;
        ctx->audio_format_drop_down = audio_format_drop_down;
        ctx->link_entry = link_entry;
        ctx->downloading_box = downloading_box;
        ctx->downloading_label = downloading_label;
        ctx->path_entry = path_entry;
        ctx->pbar = pbar;

        gtk_stack_switcher_set_stack(switcher, stack);

        gtk_drop_down_set_selected(video_resolution_drop_down, 2);

        g_signal_connect_swapped(browse, "clicked", G_CALLBACK(browse_path), ctx);
        g_signal_connect(stop, "clicked", G_CALLBACK(on_stop_thread_clicked), NULL);
        g_signal_connect_swapped(download, "clicked", G_CALLBACK(download_callback), ctx);
        g_signal_connect_swapped(win, "destroy", G_CALLBACK(g_free), ctx);

        gtk_window_set_application(win, GTK_APPLICATION(app));
        gtk_window_present(win);
}

int main(int argc, char **argv)
{
        AdwApplication *app; 
        int status;

        gtk_init();

        app = adw_application_new("org.speckitor.YtdlpGtk", G_APPLICATION_DEFAULT_FLAGS);
        g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
        status = g_application_run(G_APPLICATION(app), argc, argv);

        g_object_unref(app);
        return status;
}

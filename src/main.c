#include <gtk/gtk.h>
#include <adwaita.h>

static void activate(GApplication *app)
{
        GtkBuilder *build;
        GtkWindow *win;
        GtkStack *stack;
        GtkStackSwitcher *switcher;
        GtkStackPage *video;
        GtkStackPage *audio;

        build = gtk_builder_new_from_resource("/org/speckitor/ytdlpgtk/window.ui");
        win = GTK_WINDOW(gtk_builder_get_object(build, "win"));
        stack = GTK_STACK(gtk_builder_get_object(build, "stack"));
        switcher = GTK_STACK_SWITCHER(gtk_builder_get_object(build, "switcher"));
        video = GTK_STACK_PAGE(gtk_builder_get_object(build, "video"));
        audio = GTK_STACK_PAGE(gtk_builder_get_object(build, "audio"));

        gtk_stack_switcher_set_stack(switcher, stack);

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

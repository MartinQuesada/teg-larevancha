#include <gtk/gtk.h>

int main(int argc, char *argv[])
{
    printf("Testing GTK3...\n");
    
    // Initialize GTK
    gtk_init(&argc, &argv);
    printf("GTK3 initialized successfully\n");
    
    // Create a simple window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "GTK3 Test");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    
    // Create a label
    GtkWidget *label = gtk_label_new("GTK3 is working!");
    gtk_widget_set_visible(label, TRUE);
    gtk_container_add(GTK_CONTAINER(window), label);
    
    // Connect the destroy signal
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    // Show the window
    gtk_widget_set_visible(window, TRUE);
    
    printf("Window created, entering main loop...\n");
    
    // Run the main loop
    gtk_main();
    
    printf("GTK3 test completed successfully\n");
    return 0;
} 
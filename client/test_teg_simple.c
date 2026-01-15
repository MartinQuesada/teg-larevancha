#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>

// Global variables
GtkWidget *main_window = NULL;
GtkWidget *main_message_area = NULL;

// Simulate the functions we've implemented
void gnome_master_client(void)
{
    printf("DEBUG: gnome_master_client called (stub)\n");
}

void create_mainwin(void)
{
    printf("DEBUG: create_mainwin called\n");
    
    // Create the main application window
    main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_window), "TEG - Tenes Empanadas Graciela");
    gtk_window_set_default_size(GTK_WINDOW(main_window), 800, 600);
    gtk_window_set_position(GTK_WINDOW(main_window), GTK_WIN_POS_CENTER);
    
    // Create a main vertical box for the window
    GtkWidget *main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_set_visible(main_vbox, TRUE);
    gtk_widget_set_parent(main_vbox, main_window);
    
    // Create a toolbar
    GtkWidget *toolbar = gtk_toolbar_new();
    gtk_widget_set_visible(toolbar, TRUE);
    gtk_box_pack_start(GTK_BOX(main_vbox), toolbar, FALSE, FALSE, 0);
    
    // Create a status bar
    GtkWidget *statusbar = gtk_statusbar_new();
    gtk_widget_set_visible(statusbar, TRUE);
    gtk_box_pack_end(GTK_BOX(main_vbox), statusbar, FALSE, FALSE, 0);
    
    // Create a main area (placeholder for now)
    GtkWidget *main_area = gtk_drawing_area_new();
    gtk_widget_set_visible(main_area, TRUE);
    gtk_widget_set_hexpand(main_area, TRUE);
    gtk_widget_set_vexpand(main_area, TRUE);
    gtk_box_pack_start(GTK_BOX(main_vbox), main_area, TRUE, TRUE, 0);
    
    // Store the main message area
    main_message_area = main_area;
    
    // Connect the destroy signal
    g_signal_connect(main_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    // Show the window
    gtk_widget_set_visible(main_window, TRUE);
    
    printf("DEBUG: create_mainwin - main window created successfully\n");
    printf("DEBUG: create_mainwin - window pointer: %p\n", main_window);
    printf("DEBUG: create_mainwin - message area pointer: %p\n", main_message_area);
}

void colors_allocate(void)
{
    printf("DEBUG: colors_allocate called\n");
    
    // Initialize color allocation for the game
    // This would typically set up color schemes for players
    // For now, we'll just mark it as done
    printf("DEBUG: colors_allocate - colors initialized successfully\n");
    
    // Try to process any pending events
    while (gtk_events_pending()) {
        gtk_main_iteration();
    }
    printf("DEBUG: colors_allocate - events processed\n");
    
    // Add a small delay to allow the window to be displayed
    g_usleep(100000); // 100ms
    printf("DEBUG: colors_allocate - delay completed\n");
}

int main(int argc, char *argv[])
{
    printf("Testing TEG initialization...\n");
    
    // Initialize GTK
    gtk_init(&argc, &argv);
    printf("GTK3 initialized successfully\n");
    
    // Simulate the initialization sequence
    gnome_master_client();
    create_mainwin();
    colors_allocate();
    
    printf("Initialization completed, starting main loop...\n");
    
    // Start the main loop
    gtk_main();
    
    printf("TEG test completed successfully\n");
    return 0;
} 
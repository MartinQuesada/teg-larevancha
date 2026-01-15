#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>

// Incluir las funciones de real_functions.c
extern GtkWidget* create_mainwin(void);
extern GtkWidget* main_window(void);
extern GtkWidget* main_message_area(void);
extern void colors_allocate(void);
extern void armies_view(void);
extern void cards_view(void);
extern void dices_view(void);
extern void connect_view(void);
extern void colortype_view(void);
extern void status_dialog(void);
extern void teg_dialog(void);
extern void mission_view(void);
extern void reagrupe_window(void);
extern void tropas_window(void);

static void on_activate(GtkApplication *app, gpointer user_data) {
    printf("TEG Client Starting...\n");
    
    // Inicializar colores
    colors_allocate();
    
    // Crear ventana principal
    GtkWidget *window = create_mainwin();
    if (!window) {
        printf("Failed to create main window\n");
        return;
    }
    
    printf("Main window created successfully\n");
    
    // Mostrar la ventana
    gtk_window_present(GTK_WINDOW(window));
    
    // Simular algunas acciones del juego
    printf("Simulating game actions...\n");
    
    // Mostrar ejércitos
    armies_view();
    
    // Mostrar cartas
    cards_view();
    
    // Mostrar dados
    dices_view();
    
    // Mostrar diálogo de conexión
    connect_view();
    
    // Mostrar diálogo de selección de color
    // colortype_view(); // Comentado para evitar problemas
    
    // Mostrar diálogo de estado
    // status_dialog(); // Comentado para evitar problemas
    
    // Mostrar misión
    // mission_view(); // Comentado para evitar problemas
    
    printf("TEG Client initialized successfully\n");
}

int main(int argc, char *argv[]) {
    printf("TEG Client v0.11.2 - GTK4 Version\n");
    printf("Starting TEG Client...\n");
    
    GtkApplication *app = gtk_application_new("org.gnome.teg", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);

    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    printf("TEG Client finished\n");
    return status;
} 
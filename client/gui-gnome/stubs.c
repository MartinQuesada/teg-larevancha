/**
 * @file stubs.c
 * Stub functions for modules not yet migrated to GTK3
 * These provide minimal implementations to allow compilation
 */

#include <gtk/gtk.h>
#include <stdio.h>
#include "gnome-compat.h"
#include "client.h"
#include "interface.h"
#include "country.h"

/* armies.c y cards.c ahora están migrados - stubs removidos */

/* gametype_view ya está en connect.c */

/* status.c y mission.c ahora están migrados - stubs removidos */

/* connect.c ya está migrado */

/* locate_country.c ahora está migrado - stubs removidos */

/* g_scores.c ahora está migrado - stubs removidos */

/* preferences.c ahora está migrado - stubs removidos */

/* stock.c ahora está migrado - stubs removidos */

/* === Funciones de support.c (pendiente migración) === */
char* translate_to_utf8(const char *str) {
    /* Para UTF-8, simplemente devolver una copia */
    return g_strdup(str ? str : "");
}

/**
 * Busca un archivo pixmap en los directorios de datos
 */
char* load_pixmap_file(const char *name) {
    char *filename = NULL;
    char *f = NULL;

    if (!name)
        return NULL;

    f = g_strconcat("teg_pix/", name, NULL);
    if (f == NULL)
        return NULL;

    /* Intentar encontrar en el directorio de pixmaps */
    filename = g_strconcat(PIXMAPDIR, f, NULL);
    
    /* Verificar si existe, si no intentar en directorio actual */
    if (!g_file_test(filename, G_FILE_TEST_EXISTS)) {
        g_free(filename);
        filename = g_strdup(f);
    }
    
    g_free(f);
    return filename;
}

/* colortype_view ya está en connect.c */

/* === Diálogos genéricos === */
void teg_dialog(const char *title, const char *message) {
    GtkWidget *dialog;
    
    dialog = gtk_message_dialog_new(
        GTK_WINDOW(main_window),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "%s",
        message ? message : "");
    
    gtk_window_set_title(GTK_WINDOW(dialog), title ? title : "TEG");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void teg_dialog_gameover(int numjug, int mission) {
    char buf[256];
    snprintf(buf, sizeof(buf) - 1, "Game Over! Player %d wins with mission %d", numjug, mission);
    teg_dialog("Game Over", buf);
}

/* === GNOME Canvas stubs (para código que aún las use) === */
GType gnome_canvas_text_get_type(void) { return G_TYPE_OBJECT; }
GType gnome_canvas_pixbuf_get_type(void) { return G_TYPE_OBJECT; }
GType gnome_canvas_group_get_type(void) { return G_TYPE_OBJECT; }
GType gnome_canvas_ellipse_get_type(void) { return G_TYPE_OBJECT; }

void gnome_canvas_item_set(GtkWidget* item, const char* property, ...) {}
void gnome_canvas_item_show(GtkWidget* item) {
    if (item && GTK_IS_WIDGET(item)) {
        gtk_widget_set_visible(item, TRUE);
    }
}
void gnome_canvas_item_hide(GtkWidget* item) {
    if (item && GTK_IS_WIDGET(item)) {
        gtk_widget_set_visible(item, FALSE);
    }
}

GtkWidget* gnome_canvas_item_new(GtkWidget* canvas, GType type, ...) {
    return gtk_label_new("Canvas Item Stub");
}

void gnome_canvas_set_scroll_region(GtkWidget* canvas, double x1, double y1, double x2, double y2) {}
void gnome_canvas_update_now(GtkWidget* canvas) {}

/* === GNOME stock stubs === */
void gnome_stock_init(void) {}
char* gnome_pixmap_file(const char* filename) {
    return g_strdup(filename ? filename : "");
}

/* === GNOME master client stub === */
GnomeClient* gnome_master_client(void) {
    return NULL;
}

/* === GNOME About dialog - use GtkAboutDialog === */
GtkWidget* gnome_about_new(const char* name, const char* version,
                           const char* copyright, const char* comments,
                           const char** authors, const char** documenters,
                           const char* translator_credits, GdkPixbuf* logo) {
    GtkWidget *dialog = gtk_about_dialog_new();
    
    if (name)
        gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), name);
    if (version)
        gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), version);
    if (copyright)
        gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), copyright);
    if (comments)
        gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), comments);
    if (authors)
        gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(dialog), authors);
    if (documenters)
        gtk_about_dialog_set_documenters(GTK_ABOUT_DIALOG(dialog), documenters);
    if (translator_credits)
        gtk_about_dialog_set_translator_credits(GTK_ABOUT_DIALOG(dialog), translator_credits);
    if (logo)
        gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), logo);
    
    return dialog;
}

/* === GNOME Question dialog === */
GtkWidget* gnome_question_dialog_parented(const char* message,
                                          void (*callback)(int, gpointer),
                                          gpointer data, GtkWindow* parent) {
    GtkWidget *dialog;
    
    dialog = gtk_message_dialog_new(parent,
                                    GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                    GTK_MESSAGE_QUESTION,
                                    GTK_BUTTONS_YES_NO,
                                    "%s", message);
    
    /* Note: The old callback signature was different - caller must adapt */
    return dialog;
}

/* === GNOME Dialog set parent === */
void gnome_dialog_set_parent(GtkWidget* dialog, GtkWindow* parent) {
    if (dialog && GTK_IS_DIALOG(dialog) && parent) {
        gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);
    }
}

/* === GDK deprecated functions stubs === */
guint gdk_input_add(gint source, GdkInputCondition condition, 
                    GIOFunc function, gpointer data) {
    /* TODO: Implement using GLib IO channels */
    return 0;
}

guint gdk_input_remove(guint tag) {
    return 0;
}

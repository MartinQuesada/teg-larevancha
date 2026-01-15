/*
 * GNOME compatibility functions for GTK4
 */

#include "gnome-compat.h"
#include "gui.h"
#include "client.h"

// Replace gnome_about_new with GTK4 equivalent
GtkWidget* gnome_about_new(const char* title, const char* version, const char* copyright, const char* comments, const char** authors, const char** documenters, const char* translator_credits, GdkPixbuf* logo)
{
    GtkWidget *about_dialog;
    
    about_dialog = gtk_about_dialog_new();
    gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(about_dialog), title);
    gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about_dialog), version);
    gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(about_dialog), copyright);
    gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(about_dialog), comments);
    
    if (authors) {
        gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(about_dialog), authors);
    }
    
    if (documenters) {
        gtk_about_dialog_set_documenters(GTK_ABOUT_DIALOG(about_dialog), documenters);
    }
    
    if (translator_credits) {
        gtk_about_dialog_set_translator_credits(GTK_ABOUT_DIALOG(about_dialog), translator_credits);
    }
    
    if (logo) {
        gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(about_dialog), GDK_PAINTABLE(logo));
    }
    
    return about_dialog;
}

// Replace gnome_question_dialog_parented with GTK4 equivalent
GtkWidget* gnome_question_dialog_parented(const char* message, void (*callback)(int, gpointer), gpointer data, GtkWindow* parent)
{
    GtkWidget *dialog;
    GtkWidget *content_area;
    GtkWidget *label;
    
    dialog = gtk_dialog_new();
    gtk_window_set_title(GTK_WINDOW(dialog), "Question");
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    
    if (parent) {
        gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);
    }
    
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    
    label = gtk_label_new(message);
    gtk_widget_set_visible(label, TRUE);
    gtk_box_append(GTK_BOX(content_area), label);
    
    gtk_dialog_add_button(GTK_DIALOG(dialog), "OK", GTK_RESPONSE_OK);
    gtk_dialog_add_button(GTK_DIALOG(dialog), "Cancel", GTK_RESPONSE_CANCEL);
    
    // Connect the callback
    if (callback) {
        g_signal_connect(dialog, "response", G_CALLBACK(callback), data);
    }
    
    return dialog;
}

// GNOME Canvas compatibility functions (stubs for GTK4)
GType gnome_canvas_text_get_type(void)
{
    // In GTK4, we don't have GNOME Canvas, so return a dummy type
    return G_TYPE_NONE;
}

GType gnome_canvas_pixbuf_get_type(void)
{
    // In GTK4, we don't have GNOME Canvas, so return a dummy type
    return G_TYPE_NONE;
}

GType gnome_canvas_group_get_type(void)
{
    // In GTK4, we don't have GNOME Canvas, so return a dummy type
    return G_TYPE_NONE;
}

void gnome_canvas_item_set(GtkWidget* item, const char* property, ...)
{
    // In GTK4, we don't have GNOME Canvas items
    // This is a stub
}

void gnome_canvas_item_show(GtkWidget* item)
{
    // In GTK4, we don't have GNOME Canvas items
    // This is a stub
}

void gnome_canvas_item_hide(GtkWidget* item)
{
    // In GTK4, we don't have GNOME Canvas items
    // This is a stub
}

GtkWidget* gnome_canvas_item_new(GtkWidget* canvas, GType type, ...)
{
    // In GTK4, we don't have GNOME Canvas items
    // This is a stub
    return NULL;
}

// GTK Icon Factory compatibility functions (stubs for GTK4)
GtkIconFactory* gtk_icon_factory_new(void)
{
    // In GTK4, we don't have icon factories
    // This is a stub
    return NULL;
}

GtkIconSet* gtk_icon_set_new(void)
{
    // In GTK4, we don't have icon sets
    // This is a stub
    return NULL;
}

GtkIconSource* gtk_icon_source_new(void)
{
    // In GTK4, we don't have icon sources
    // This is a stub
    return NULL;
}

void gtk_icon_source_set_pixbuf(GtkIconSource* source, GdkPixbuf* pixbuf)
{
    // In GTK4, we don't have icon sources
    // This is a stub
}

void gtk_icon_source_set_size(GtkIconSource* source, GtkIconSize size)
{
    // In GTK4, we don't have icon sources
    // This is a stub
}

void gtk_icon_set_add_source(GtkIconSet* set, GtkIconSource* source)
{
    // In GTK4, we don't have icon sets
    // This is a stub
}

void gtk_icon_factory_add(GtkIconFactory* factory, const char* stock_id, GtkIconSet* set)
{
    // In GTK4, we don't have icon factories
    // This is a stub
}

void gtk_icon_source_free(GtkIconSource* source)
{
    // In GTK4, we don't have icon sources
    // This is a stub
}

void gtk_icon_factory_add_default(GtkIconFactory* factory)
{
    // In GTK4, we don't have icon factories
    // This is a stub
}

void gtk_icon_set_unref(GtkIconSet* set)
{
    // In GTK4, we don't have icon sets
    // This is a stub
}

void gtk_stock_add_static(GtkStockItem* items, guint n_items)
{
    // In GTK4, we don't have stock items
    // This is a stub
} 
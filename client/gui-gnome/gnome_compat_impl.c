/*
 * GNOME compatibility implementation for GTK3
 */

#include "gnome-compat.h"
#include <stdio.h>
#include <stdlib.h>

// Global variables
// GSettings* g_settings = NULL; // Commented out to avoid duplicate symbol

// GNOME master client stub
GnomeClient* gnome_master_client(void)
{
    printf("DEBUG: gnome_master_client called (stub)\n");
    return NULL;
}

// GDK input remove stub
guint gdk_input_remove(guint tag)
{
    printf("DEBUG: gdk_input_remove called with tag=%u (stub)\n", tag);
    return 0;
}

// GNOME Canvas type functions (stubs)
GType gnome_canvas_text_get_type(void)
{
    printf("DEBUG: gnome_canvas_text_get_type called (stub)\n");
    return GTK_TYPE_LABEL;
}

GType gnome_canvas_pixbuf_get_type(void)
{
    printf("DEBUG: gnome_canvas_pixbuf_get_type called (stub)\n");
    return GTK_TYPE_LABEL;
}

GType gnome_canvas_group_get_type(void)
{
    printf("DEBUG: gnome_canvas_group_get_type called (stub)\n");
    return GTK_TYPE_WIDGET;
}

void gnome_canvas_item_set(GtkWidget* item, const char* property, ...)
{
    printf("DEBUG: gnome_canvas_item_set called with property=%s (stub)\n", property ? property : "NULL");
    // This is a stub - in a real implementation, this would set properties on the widget
}

void gnome_canvas_item_show(GtkWidget* item)
{
    printf("DEBUG: gnome_canvas_item_show called (stub)\n");
    if (item) {
        gtk_widget_set_visible(item, TRUE);
    }
}

void gnome_canvas_item_hide(GtkWidget* item)
{
    printf("DEBUG: gnome_canvas_item_hide called (stub)\n");
    if (item) {
        gtk_widget_set_visible(item, FALSE);
    }
}

GtkWidget* gnome_canvas_item_new(GtkWidget* canvas, GType type, ...)
{
    printf("DEBUG: gnome_canvas_item_new called with type=%s (stub)\n", g_type_name(type));
    // This is a stub - in a real implementation, this would create canvas items
    return gtk_label_new("");
}

// GNOME Dialog functions
GtkWidget* gnome_question_dialog_parented(const char* message, void (*callback)(int, gpointer), gpointer data, GtkWindow* parent)
{
    printf("DEBUG: gnome_question_dialog_parented called with message=%s (stub)\n", message ? message : "NULL");
    
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
    gtk_box_pack_start(GTK_BOX(content_area), label, FALSE, FALSE, 0);
    
    gtk_dialog_add_button(GTK_DIALOG(dialog), "Yes", GTK_RESPONSE_YES);
    gtk_dialog_add_button(GTK_DIALOG(dialog), "No", GTK_RESPONSE_NO);
    
    // Connect the callback
    if (callback) {
        g_signal_connect(dialog, "response", G_CALLBACK(callback), data);
    }
    
    return dialog;
}

GtkWidget* gnome_about_new(const char* title, const char* version, const char* copyright, const char* comments, const char** authors, const char** documenters, const char* translator_credits, GdkPixbuf* logo)
{
    printf("DEBUG: gnome_about_new called with title=%s (stub)\n", title ? title : "NULL");
    
    GtkWidget *dialog;
    
    dialog = gtk_about_dialog_new();
    
    if (title) gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), title);
    if (version) gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), version);
    if (copyright) gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), copyright);
    if (comments) gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), comments);
    if (authors) gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(dialog), authors);
    if (documenters) gtk_about_dialog_set_documenters(GTK_ABOUT_DIALOG(dialog), documenters);
    if (translator_credits) gtk_about_dialog_set_translator_credits(GTK_ABOUT_DIALOG(dialog), translator_credits);
    if (logo) gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), logo);
    
    return dialog;
}

// GNOME Dialog helper functions
GtkWidget* gnome_dialog_get_vbox(GtkWidget* dialog)
{
    printf("DEBUG: gnome_dialog_get_vbox called (stub)\n");
    return gtk_dialog_get_content_area(GTK_DIALOG(dialog));
}

void gnome_dialog_append_buttons(GtkWidget* dialog, ...)
{
    printf("DEBUG: gnome_dialog_append_buttons called (stub)\n");
    // This is a stub - in a real implementation, this would add buttons to the dialog
}

void gnome_dialog_close_hides(GtkWidget* dialog, gboolean close_hides)
{
    printf("DEBUG: gnome_dialog_close_hides called with close_hides=%d (stub)\n", close_hides);
    // This is a stub - in a real implementation, this would set the close behavior
}

void gnome_dialog_set_default(GtkWidget* dialog, int button)
{
    printf("DEBUG: gnome_dialog_set_default called with button=%d (stub)\n", button);
    // This is a stub - in a real implementation, this would set the default button
}

void gnome_dialog_button_connect(GtkWidget* dialog, int button, GCallback callback, gpointer data)
{
    printf("DEBUG: gnome_dialog_button_connect called with button=%d (stub)\n", button);
    // This is a stub - in a real implementation, this would connect the button signal
}

void gnome_dialog_set_sensitive(GtkWidget* dialog, int button, gboolean sensitive)
{
    printf("DEBUG: gnome_dialog_set_sensitive called with button=%d, sensitive=%d (stub)\n", button, sensitive);
    // This is a stub - in a real implementation, this would set button sensitivity
}

void gnome_dialog_set_parent(GtkWidget* dialog, GtkWidget* parent)
{
    printf("DEBUG: gnome_dialog_set_parent called (stub)\n");
    if (dialog && parent) {
        gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(parent));
    }
}

// GNOME Stock init
void gnome_stock_init(void)
{
    printf("DEBUG: gnome_stock_init called (stub)\n");
    // This is a stub - in a real implementation, this would initialize stock icons
}

// GNOME Canvas compatibility
void gnome_canvas_set_scroll_region(GtkWidget* canvas, double x1, double y1, double x2, double y2)
{
    printf("DEBUG: gnome_canvas_set_scroll_region called with x1=%f, y1=%f, x2=%f, y2=%f (stub)\n", x1, y1, x2, y2);
    // This is a stub - in a real implementation, this would set the scroll region
}

// GNOME pixmap file
char* gnome_pixmap_file(const char* filename)
{
    printf("DEBUG: gnome_pixmap_file called with filename=%s (stub)\n", filename ? filename : "NULL");
    // This is a stub - in a real implementation, this would return the full path to the pixmap
    return g_strdup(filename);
}

// GTK3 Grid compatibility
void gtk_grid_resize(GtkGrid* grid, int rows, int cols)
{
    printf("DEBUG: gtk_grid_resize called with rows=%d, cols=%d (stub)\n", rows, cols);
    // This is a stub - in GTK3, grids don't need explicit resizing
}

// GTK3 Tree View compatibility
void gtk_tree_view_set_rules_hint(GtkTreeView* treeview, gboolean setting)
{
    printf("DEBUG: gtk_tree_view_set_rules_hint called with setting=%d (stub)\n", setting);
    // This is a stub - in a real implementation, this would set the rules hint
}

// GTK3 Icon Factory stubs
GtkIconFactory* gtk_icon_factory_new(void)
{
    printf("DEBUG: gtk_icon_factory_new called (stub)\n");
    return NULL;
}

GtkIconSet* gtk_icon_set_new(void)
{
    printf("DEBUG: gtk_icon_set_new called (stub)\n");
    return NULL;
}

GtkIconSource* gtk_icon_source_new(void)
{
    printf("DEBUG: gtk_icon_source_new called (stub)\n");
    return NULL;
}

void gtk_icon_source_set_pixbuf(GtkIconSource* source, GdkPixbuf* pixbuf)
{
    printf("DEBUG: gtk_icon_source_set_pixbuf called (stub)\n");
    // This is a stub
}

void gtk_icon_source_set_size(GtkIconSource* source, GtkIconSize size)
{
    printf("DEBUG: gtk_icon_source_set_size called with size=%d (stub)\n", size);
    // This is a stub
}

void gtk_icon_factory_add(GtkIconFactory* factory, const char* stock_id, GtkIconSet* set)
{
    printf("DEBUG: gtk_icon_factory_add called with stock_id=%s (stub)\n", stock_id ? stock_id : "NULL");
    // This is a stub
}

void gtk_icon_source_free(GtkIconSource* source)
{
    printf("DEBUG: gtk_icon_source_free called (stub)\n");
    // This is a stub
}

void gtk_icon_factory_add_default(GtkIconFactory* factory)
{
    printf("DEBUG: gtk_icon_factory_add_default called (stub)\n");
    // This is a stub
}

void gtk_icon_set_unref(GtkIconSet* set)
{
    printf("DEBUG: gtk_icon_set_unref called (stub)\n");
    // This is a stub
}

// GTK3 drawing compatibility stubs
GdkPixmap* gdk_pixmap_new(GtkWidget* window, int width, int height, int depth)
{
    printf("DEBUG: gdk_pixmap_new called with width=%d, height=%d, depth=%d (stub)\n", width, height, depth);
    return NULL;
}

void gdk_gc_set_foreground(GdkGC* gc, GdkColor* color)
{
    printf("DEBUG: gdk_gc_set_foreground called (stub)\n");
    // This is a stub
}

void gdk_draw_rectangle(GdkPixmap* pixmap, GdkGC* gc, gboolean filled, int x, int y, int width, int height)
{
    printf("DEBUG: gdk_draw_rectangle called with filled=%d, x=%d, y=%d, width=%d, height=%d (stub)\n", filled, x, y, width, height);
    // This is a stub
}

void gdk_draw_string(GdkPixmap* pixmap, GdkFont* font, GdkGC* gc, int x, int y, const char* text)
{
    printf("DEBUG: gdk_draw_string called with text=%s (stub)\n", text ? text : "NULL");
    // This is a stub
}

int gdk_string_width(GdkFont* font, const char* text)
{
    printf("DEBUG: gdk_string_width called with text=%s (stub)\n", text ? text : "NULL");
    return text ? strlen(text) * 8 : 0; // Rough estimate
}

int gdk_string_height(GdkFont* font, const char* text)
{
    printf("DEBUG: gdk_string_height called with text=%s (stub)\n", text ? text : "NULL");
    return 12; // Rough estimate
} 
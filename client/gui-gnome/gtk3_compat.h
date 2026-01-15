/*
 * GTK3 compatibility header
 * This provides a smoother migration path from GTK2 to GTK3
 */

#ifndef __GTK3_COMPAT_H
#define __GTK3_COMPAT_H

#include <gtk/gtk.h>

// GTK3 compatibility macros
#define GTK_DIALOG_NEW() gtk_dialog_new()
#define GTK_DIALOG_GET_CONTENT_AREA(dialog) gtk_dialog_get_content_area(dialog)
#define GTK_DIALOG_ADD_BUTTON(dialog, text, response) gtk_dialog_add_button(dialog, text, response)

// GTK3 widget functions
#define gtk_widget_show(widget) gtk_widget_set_visible(widget, TRUE)
#define gtk_widget_hide(widget) gtk_widget_set_visible(widget, FALSE)

// GTK3 container functions
#define gtk_container_add(container, child) gtk_widget_set_parent(child, container)

// GTK3 box functions
#define gtk_box_pack_start(box, child, expand, fill, padding) gtk_box_pack_start(box, child, expand, fill, padding)

// GTK3 window functions
#define gtk_window_set_position(window, pos) gtk_window_set_position(window, pos)

// GTK3 dialog functions
#define gtk_dialog_run(dialog) gtk_dialog_run(dialog)

// GTK3 about dialog functions
#define gtk_about_dialog_new() gtk_about_dialog_new()
#define gtk_about_dialog_set_program_name(dialog, name) gtk_about_dialog_set_program_name(dialog, name)
#define gtk_about_dialog_set_version(dialog, version) gtk_about_dialog_set_version(dialog, version)
#define gtk_about_dialog_set_copyright(dialog, copyright) gtk_about_dialog_set_copyright(dialog, copyright)
#define gtk_about_dialog_set_comments(dialog, comments) gtk_about_dialog_set_comments(dialog, comments)
#define gtk_about_dialog_set_authors(dialog, authors) gtk_about_dialog_set_authors(dialog, authors)
#define gtk_about_dialog_set_documenters(dialog, documenters) gtk_about_dialog_set_documenters(dialog, documenters)
#define gtk_about_dialog_set_translator_credits(dialog, credits) gtk_about_dialog_set_translator_credits(dialog, credits)
#define gtk_about_dialog_set_logo(dialog, logo) gtk_about_dialog_set_logo(dialog, logo)

// GTK3 icon functions (stubs for GTK3)
typedef struct _GtkIconFactory GtkIconFactory;
typedef struct _GtkIconSet GtkIconSet;
typedef struct _GtkIconSource GtkIconSource;

GtkIconFactory* gtk_icon_factory_new(void);
GtkIconSet* gtk_icon_set_new(void);
GtkIconSource* gtk_icon_source_new(void);
void gtk_icon_source_set_pixbuf(GtkIconSource* source, GdkPixbuf* pixbuf);
void gtk_icon_source_set_size(GtkIconSource* source, GtkIconSize size);
void gtk_icon_set_add_source(GtkIconSet* set, GtkIconSource* source);
void gtk_icon_factory_add(GtkIconFactory* factory, const char* stock_id, GtkIconSet* set);
void gtk_icon_source_free(GtkIconSource* source);
void gtk_icon_factory_add_default(GtkIconFactory* factory);
void gtk_icon_set_unref(GtkIconSet* set);

// GTK3 stock functions
typedef struct _GtkStockItem GtkStockItem;
struct _GtkStockItem {
    const char* stock_id;
    const char* label;
    GdkModifierType modifier;
    guint keyval;
    const char* translation_domain;
};

void gtk_stock_add_static(GtkStockItem* items, guint n_items);

// GTK3 icon size constants
#define GTK_ICON_SIZE_BUTTON 1
#define GTK_ICON_SIZE_MENU 2
#define GTK_ICON_SIZE_SMALL_TOOLBAR 3
#define GTK_ICON_SIZE_LARGE_TOOLBAR 4
#define GTK_ICON_SIZE_DND 5
#define GTK_ICON_SIZE_DIALOG 6

// GTK3 box compatibility
#define gtk_vbox_new(homogeneous, spacing) gtk_box_new(GTK_ORIENTATION_VERTICAL, spacing)
#define gtk_hbox_new(homogeneous, spacing) gtk_box_new(GTK_ORIENTATION_HORIZONTAL, spacing)
#define gtk_box_pack_start_defaults(box, child) gtk_box_pack_start(box, child, FALSE, FALSE, 0)

// GTK3 table compatibility (GTK3 uses GtkGrid)
typedef GtkGrid GtkTable;
#define GTK_TABLE(grid) (grid)
#define gtk_table_new(rows, cols, homogeneous) gtk_grid_new()
#define gtk_table_set_col_spacings(table, spacing) gtk_grid_set_column_spacing(table, spacing)
#define gtk_table_set_row_spacings(table, spacing) gtk_grid_set_row_spacing(table, spacing)
#define gtk_table_resize(table, rows, cols) gtk_grid_resize(table, rows, cols)
#define gtk_table_attach_defaults(table, child, left, right, top, bottom) gtk_grid_attach(table, child, left, top, right-left, bottom-top)

// GTK3 scrolled window compatibility
#define gtk_scrolled_window_new(hadjustment, vadjustment) gtk_scrolled_window_new()

// GTK3 signal compatibility
#define GTK_SIGNAL_FUNC(func) G_CALLBACK(func)
#define gtk_object_destroy(obj) gtk_widget_destroy(obj)

// GTK3 widget visibility
#define GTK_WIDGET_VISIBLE(widget) gtk_widget_get_visible(widget)

// GTK3 window position constants
#define GTK_WIN_POS_NONE 0

// GTK3 drawing functions (stubs)
typedef struct _GdkGC GdkGC;
typedef struct _GdkFont GdkFont;
typedef struct _GdkColor GdkColor;

#define gdk_window_get_origin(window, x, y) gdk_window_get_origin(window, x, y)
#define gdk_window_get_size(window, w, h) gdk_window_get_size(window, w, h)
#define gtk_widget_set_uposition(widget, x, y) gtk_widget_set_uposition(widget, x, y)

#endif /* __GTK3_COMPAT_H */ 
/*
 * GNOME compatibility header for GTK3
 * Provides compatibility macros for code migrated from GNOME2/GTK2
 */

#ifndef __GNOME_COMPAT_H
#define __GNOME_COMPAT_H

#include <gtk/gtk.h>
#include <gio/gio.h>
#include <libintl.h>

/* Gettext macros */
#ifndef _
#define _(msgid) gettext(msgid)
#endif
#ifndef N_
#define N_(msgid) (msgid)
#endif

/* GTK2 to GTK3 compatibility */

/* GtkObject no longer exists in GTK3 */
typedef GObject GtkObject;
#define GTK_OBJECT(obj) G_OBJECT(obj)

/* gtk_object_destroy -> gtk_widget_destroy */
#define gtk_object_destroy(obj) gtk_widget_destroy(GTK_WIDGET(obj))

/* Signal compatibility */
#define GTK_SIGNAL_FUNC(func) G_CALLBACK(func)
#define gtk_signal_connect(obj, sig, func, data) g_signal_connect(obj, sig, func, data)

/* Box compatibility - GTK3 still has these but with different signature */
#define gtk_vbox_new(homogeneous, spacing) gtk_box_new(GTK_ORIENTATION_VERTICAL, spacing)
#define gtk_hbox_new(homogeneous, spacing) gtk_box_new(GTK_ORIENTATION_HORIZONTAL, spacing)

/* Table to Grid compatibility */
#define GtkTable GtkGrid
#define GTK_TABLE(obj) GTK_GRID(obj)
#define gtk_table_new(rows, cols, homogeneous) gtk_grid_new()
#define gtk_table_set_col_spacings(table, spacing) gtk_grid_set_column_spacing(GTK_GRID(table), spacing)
#define gtk_table_set_row_spacings(table, spacing) gtk_grid_set_row_spacing(GTK_GRID(table), spacing)
#define gtk_table_attach_defaults(table, child, left, right, top, bottom) \
    gtk_grid_attach(GTK_GRID(table), child, left, top, (right)-(left), (bottom)-(top))

/* Widget visibility macros */
#define GTK_WIDGET_VISIBLE(widget) gtk_widget_get_visible(GTK_WIDGET(widget))
#define GTK_WIDGET_REALIZED(widget) gtk_widget_get_realized(GTK_WIDGET(widget))

/* GNOME Canvas types - now just stubs since we use Cairo */
typedef GtkWidget GnomeCanvas;
typedef GtkWidget GnomeCanvasGroup;
typedef GtkWidget GnomeCanvasItem;

/* GNOME Canvas stub functions */
#define GNOME_CANVAS(obj) (obj)
#define GNOME_CANVAS_GROUP(obj) (obj)

/* GnomeAppBar replacement - we use GtkLabel */
#define GNOME_APPBAR(obj) (obj)
#define gnome_appbar_set_status(appbar, msg) \
    do { if (GTK_IS_LABEL(appbar)) gtk_label_set_text(GTK_LABEL(appbar), msg); } while(0)

/* GConf to GSettings */
typedef GSettings GConfClient;

/* Anchor type for compatibility */
#ifndef GTK_ANCHOR_NW
typedef enum {
    GTK_ANCHOR_CENTER,
    GTK_ANCHOR_NORTH,
    GTK_ANCHOR_NW,
    GTK_ANCHOR_NE,
    GTK_ANCHOR_SOUTH,
    GTK_ANCHOR_SW,
    GTK_ANCHOR_SE,
    GTK_ANCHOR_WEST,
    GTK_ANCHOR_EAST
} GtkAnchorType;
#endif

/* GNOME stock button constants - map to GTK stock */
#define GNOME_STOCK_BUTTON_CLOSE "gtk-close"
#define GNOME_STOCK_BUTTON_OK "gtk-ok"
#define GNOME_STOCK_BUTTON_CANCEL "gtk-cancel"
#define GNOME_STOCK_BUTTON_YES "gtk-yes"
#define GNOME_STOCK_BUTTON_NO "gtk-no"
#define GNOME_STOCK_BUTTON_PREV "gtk-go-back"
#define GNOME_STOCK_BUTTON_NEXT "gtk-go-forward"

/* GNOME pixmap stock constants */
#define GNOME_STOCK_PIXMAP_NEW "document-new"
#define GNOME_STOCK_PIXMAP_TIMER "appointment-soon"

/* GdkColor to GdkRGBA helpers */
/* Note: Most code should be migrated to use GdkRGBA directly */

/* Deprecated GDK types - provide empty structs for compilation */
typedef struct _GdkGC { int dummy; } GdkGC;
typedef struct _GdkFont { int dummy; } GdkFont;
typedef struct _GdkPixmap { int dummy; } GdkPixmap;
typedef struct _GdkVisual { int dummy; } GdkVisual;

/* GdkColor still exists in GTK3 but is deprecated */
#ifndef __GDK_COLOR_H__
typedef struct {
    guint32 pixel;
    guint16 red;
    guint16 green;
    guint16 blue;
} GdkColor;
#endif

/* GNOME UI Info structures (for menu compatibility) - not used in migrated code */
typedef struct _GnomeUIInfo {
    int type;
    const char *label;
    const char *hint;
    void *callback;
    void *user_data;
    void *unused;
    int pixmap_type;
    const char *pixmap_info;
    int accelerator_key;
    int ac_mods;
    GtkWidget *widget;
} GnomeUIInfo;

/* GNOME UI Info macros - empty, menus are created differently in GTK3 */
#define GNOMEUIINFO_END { 0, NULL, NULL, NULL, NULL, NULL, 0, NULL, 0, 0, NULL }
#define GNOMEUIINFO_SEPARATOR { 0, NULL, NULL, NULL, NULL, NULL, 0, NULL, 0, 0, NULL }

/* POPT compatibility - not used in GTK3 */
typedef struct poptOption {
    const char *longName;
    char shortName;
    int argInfo;
    void *arg;
    int val;
    const char *descrip;
    const char *argDescrip;
} poptOption;

#define POPT_ARG_NONE 0

/* GNOME client - not used in GTK3 */
typedef struct _GnomeClient { int dummy; } GnomeClient;
GnomeClient* gnome_master_client(void);

/* GDK input - use GLib IO channels instead */
typedef gint GdkInputCondition;
#define GDK_INPUT_READ G_IO_IN

/* gdk_input_* functions are deprecated - use GLib IO channels */
guint gdk_input_add(gint source, GdkInputCondition condition, 
                    GIOFunc function, gpointer data);
guint gdk_input_remove(guint tag);

/* Stub functions declared in stubs.c or gnome_compat_impl.c */
void gnome_stock_init(void);
char* gnome_pixmap_file(const char* filename);

/* GNOME About dialog - replaced by GtkAboutDialog */
GtkWidget* gnome_about_new(const char* name, const char* version,
                           const char* copyright, const char* comments,
                           const char** authors, const char** documenters,
                           const char* translator_credits, GdkPixbuf* logo);

/* GNOME Question dialog */
GtkWidget* gnome_question_dialog_parented(const char* message,
                                          void (*callback)(int, gpointer),
                                          gpointer data, GtkWindow* parent);

/* GNOME Dialog set parent */
void gnome_dialog_set_parent(GtkWidget* dialog, GtkWindow* parent);

#endif /* __GNOME_COMPAT_H */

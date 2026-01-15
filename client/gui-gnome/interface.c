/*	$Id: interface.c,v 1.96 2002/08/31 17:52:54 riq Exp $	*/
/* Tenes Empanadas Graciela
 *
 * Copyright (C) 2000 Ricardo Quesada
 *
 * Author: Ricardo Calixto Quesada <rquesada@core-sdi.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; only version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */
/**
 * @file interface.c
 * Maneja las ventanas que se crean - Migrado a GTK3
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gtk/gtk.h>

#include "gnome-compat.h"

#include "gui.h"
#include "client.h"
#include "chatline.h"
#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "g_country.h"
#include "g_cont.h"
#include "stock.h"
#include "status.h"
#include "armies.h"
#include "dices.h"
#include "themes.h"
#include "fonts.h"
#include "reagrupe.h"

extern TTheme gui_theme;

/* Widgets globales */
GtkTextView *main_message_area = NULL;
GtkWidget *main_window = NULL;
GtkWidget *statusbar = NULL;
GtkWidget *canvas_map = NULL;
GtkWidget *window_mainstatus = NULL;

double canvas_map_zoom = 1.00;

/* Diálogos de tropas y reagrupamiento */
static GtkWidget *tropas_dialog = NULL;
static GtkWidget *tropas_hscale_cant = NULL;
static int tropas_cant;
static int tropas_src;
static int tropas_dst;

static GtkWidget *reagrupe_dialog = NULL;
static GtkWidget *reagrupe_hscale_cant = NULL;
static int reagrupe_cant = -1;
static int reagrupe_src = -1;
static int reagrupe_dst = -1;

/* Barra de herramientas */
static GtkWidget *toolbar = NULL;

/* Items del menú que necesitan sensibilidad dinámica */
static GtkWidget *menu_connect = NULL;
static GtkWidget *menu_disconnect = NULL;
static GtkWidget *menu_start = NULL;
static GtkWidget *menu_status = NULL;
static GtkWidget *menu_enviarfichas = NULL;
static GtkWidget *menu_reagrupe = NULL;
static GtkWidget *menu_getcard = NULL;
static GtkWidget *menu_endturn = NULL;

/* Botones de toolbar que necesitan sensibilidad */
static GtkToolItem *toolbar_connect_button = NULL;
static GtkToolItem *toolbar_start_button = NULL;


/**
 * Actualiza el mensaje de la barra de estado
 */
void statusbar_set_message(const char *msg)
{
	if (statusbar && GTK_IS_LABEL(statusbar)) {
		gtk_label_set_text(GTK_LABEL(statusbar), msg ? msg : "");
	}
}


/**
 * Libera imagen imlib (compatibilidad, ahora usa g_object_unref)
 */
void free_imlib_image(GObject *object, gpointer data)
{
	if (data) {
		g_object_unref(G_OBJECT(data));
	}
}


/**
 * Libera imagen imlib (versión puntero)
 */
void free_imlib_image_ptr(GObject *object, gpointer *data)
{
	if (data && *data) {
		g_object_unref(G_OBJECT(*data));
		*data = NULL;
	}
}


/**
 * Callback para zoom in
 */
static void canvas_map_zoom_in(GtkWidget *widget, gpointer data)
{
	canvas_map_zoom *= 1.05;
	G_country_redraw();
}


/**
 * Callback para zoom out
 */
static void canvas_map_zoom_out(GtkWidget *widget, gpointer data)
{
	canvas_map_zoom /= 1.05;
	G_country_redraw();
}


/**
 * Callback para zoom 1:1
 */
static void canvas_map_zoom_1(GtkWidget *widget, gpointer data)
{
	canvas_map_zoom = 1.0;
	G_country_redraw();
}


/**
 * Crea la barra de menú
 */
static GtkWidget* create_menubar(GtkWidget *window)
{
	GtkWidget *menubar;
	GtkWidget *menuitem;
	GtkWidget *submenu;

	menubar = gtk_menu_bar_new();

	/* Menú Game */
	menuitem = gtk_menu_item_new_with_mnemonic(_("_Game"));
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), menuitem);
	
	submenu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem), submenu);
	
	menu_connect = gtk_menu_item_new_with_mnemonic(_("_Connect"));
	g_signal_connect(menu_connect, "activate", G_CALLBACK(on_connect_activate), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menu_connect);
	
	menu_disconnect = gtk_menu_item_new_with_mnemonic(_("_Disconnect"));
	g_signal_connect(menu_disconnect, "activate", G_CALLBACK(on_disconnect_activate), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menu_disconnect);
	gtk_widget_set_sensitive(menu_disconnect, FALSE);
	
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), gtk_separator_menu_item_new());
	
	menuitem = gtk_menu_item_new_with_mnemonic(_("_Launch robot"));
	g_signal_connect(menuitem, "activate", G_CALLBACK(on_launchrobot_activate), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menuitem);
	
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), gtk_separator_menu_item_new());
	
	menuitem = gtk_menu_item_new_with_mnemonic(_("_Scores"));
	g_signal_connect(menuitem, "activate", G_CALLBACK(on_scores_activate), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menuitem);
	
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), gtk_separator_menu_item_new());
	
	menuitem = gtk_menu_item_new_with_mnemonic(_("E_xit"));
	g_signal_connect(menuitem, "activate", G_CALLBACK(on_exit_activate), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menuitem);

	/* Menú Actions */
	menuitem = gtk_menu_item_new_with_mnemonic(_("_Actions"));
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), menuitem);
	
	submenu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem), submenu);
	
	menu_start = gtk_menu_item_new_with_mnemonic(_("_Start"));
	g_signal_connect(menu_start, "activate", G_CALLBACK(on_start_activate), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menu_start);
	
	menu_enviarfichas = gtk_menu_item_new_with_mnemonic(_("Send _armies"));
	g_signal_connect(menu_enviarfichas, "activate", G_CALLBACK(on_enviarfichas_activate), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menu_enviarfichas);
	
	menuitem = gtk_menu_item_new_with_mnemonic(_("_Update Map"));
	g_signal_connect(menuitem, "activate", G_CALLBACK(on_updatemap_activate), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menuitem);
	
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), gtk_separator_menu_item_new());
	
	menuitem = gtk_menu_item_new_with_mnemonic(_("_Reattack"));
	g_signal_connect(menuitem, "activate", G_CALLBACK(on_enviarreattack_activate), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menuitem);
	
	menu_reagrupe = gtk_menu_item_new_with_mnemonic(_("Re_group armies"));
	g_signal_connect(menu_reagrupe, "activate", G_CALLBACK(on_reagrupe_activate), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menu_reagrupe);
	
	menu_getcard = gtk_menu_item_new_with_mnemonic(_("Get _card"));
	g_signal_connect(menu_getcard, "activate", G_CALLBACK(on_pedirtarjeta_activate), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menu_getcard);
	
	menu_endturn = gtk_menu_item_new_with_mnemonic(_("_End turn"));
	g_signal_connect(menu_endturn, "activate", G_CALLBACK(on_endturn_activate), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menu_endturn);
	
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), gtk_separator_menu_item_new());
	
	menuitem = gtk_menu_item_new_with_mnemonic(_("_Surrender"));
	g_signal_connect(menuitem, "activate", G_CALLBACK(on_surrender_activate), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menuitem);

	/* Menú View */
	menuitem = gtk_menu_item_new_with_mnemonic(_("_View"));
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), menuitem);
	
	submenu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem), submenu);
	
	menuitem = gtk_menu_item_new_with_mnemonic(_("View _players"));
	g_signal_connect(menuitem, "activate", G_CALLBACK(on_status_activate), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menuitem);
	
	menuitem = gtk_menu_item_new_with_mnemonic(_("View _mission"));
	g_signal_connect(menuitem, "activate", G_CALLBACK(on_viewmission_activate), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menuitem);
	
	menuitem = gtk_menu_item_new_with_mnemonic(_("View _cards"));
	g_signal_connect(menuitem, "activate", G_CALLBACK(on_viewcards_activate), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menuitem);
	
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), gtk_separator_menu_item_new());
	
	menuitem = gtk_menu_item_new_with_mnemonic(_("Zoom _In"));
	g_signal_connect(menuitem, "activate", G_CALLBACK(canvas_map_zoom_in), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menuitem);
	
	menuitem = gtk_menu_item_new_with_mnemonic(_("Zoom _Out"));
	g_signal_connect(menuitem, "activate", G_CALLBACK(canvas_map_zoom_out), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menuitem);
	
	menuitem = gtk_menu_item_new_with_mnemonic(_("Zoom _1:1"));
	g_signal_connect(menuitem, "activate", G_CALLBACK(canvas_map_zoom_1), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menuitem);

	/* Menú Settings */
	menuitem = gtk_menu_item_new_with_mnemonic(_("_Settings"));
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), menuitem);
	
	submenu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem), submenu);
	
	menuitem = gtk_menu_item_new_with_mnemonic(_("_Preferences"));
	g_signal_connect(menuitem, "activate", G_CALLBACK(on_preferences1_activate), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menuitem);

	/* Menú Help */
	menuitem = gtk_menu_item_new_with_mnemonic(_("_Help"));
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), menuitem);
	
	submenu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem), submenu);
	
	menuitem = gtk_menu_item_new_with_mnemonic(_("_About"));
	g_signal_connect(menuitem, "activate", G_CALLBACK(on_about_activate), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(submenu), menuitem);

	return menubar;
}


/**
 * Crea la barra de herramientas
 */
static GtkWidget* create_toolbar(void)
{
	GtkWidget *toolbar_widget;
	GtkToolItem *item;
	GtkWidget *icon;

	toolbar_widget = gtk_toolbar_new();
	gtk_toolbar_set_style(GTK_TOOLBAR(toolbar_widget), GTK_TOOLBAR_BOTH);

	/* Connect - uses theme icon as fallback */
	toolbar_connect_button = gtk_tool_button_new(NULL, _("Connect"));
	gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(toolbar_connect_button), "network-server-symbolic");
	g_signal_connect(toolbar_connect_button, "clicked", G_CALLBACK(on_connect_activate), NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_widget), toolbar_connect_button, -1);

	/* Start - uses theme icon as fallback */
	toolbar_start_button = gtk_tool_button_new(NULL, _("Start"));
	gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(toolbar_start_button), "media-playback-start-symbolic");
	g_signal_connect(toolbar_start_button, "clicked", G_CALLBACK(on_start_activate), NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_widget), toolbar_start_button, -1);

	/* Separator */
	item = gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_widget), item, -1);

	/* Send armies - uses custom XPM icon */
	icon = stock_create_image(STOCK_SENDARMIES);
	item = gtk_tool_button_new(icon, _("Send armies"));
	g_signal_connect(item, "clicked", G_CALLBACK(on_enviarfichas_activate), NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_widget), item, -1);

	/* Regroup - uses theme icon */
	item = gtk_tool_button_new(NULL, _("Regroup"));
	gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(item), "view-refresh-symbolic");
	g_signal_connect(item, "clicked", G_CALLBACK(on_reagrupe_activate), NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_widget), item, -1);

	/* Get card - uses custom XPM icon */
	icon = stock_create_image(STOCK_GETCARD);
	item = gtk_tool_button_new(icon, _("Get card"));
	g_signal_connect(item, "clicked", G_CALLBACK(on_pedirtarjeta_activate), NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_widget), item, -1);

	/* End turn - uses custom XPM icon */
	icon = stock_create_image(STOCK_ENDTURN);
	item = gtk_tool_button_new(icon, _("End turn"));
	g_signal_connect(item, "clicked", G_CALLBACK(on_endturn_activate), NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_widget), item, -1);

	/* Separator */
	item = gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_widget), item, -1);

	/* View players - uses custom XPM icon */
	icon = stock_create_image(STOCK_VIEWPLAYERS);
	item = gtk_tool_button_new(icon, _("Players"));
	g_signal_connect(item, "clicked", G_CALLBACK(on_status_activate), NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar_widget), item, -1);

	toolbar = toolbar_widget;
	return toolbar_widget;
}


/**
 * Crea el área de mensajes (chat)
 */
static GtkWidget* create_message_area(void)
{
	GtkWidget *scrolled;
	GtkWidget *text_view;
	GtkTextBuffer *buffer;

	scrolled = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
	                               GTK_POLICY_AUTOMATIC,
	                               GTK_POLICY_AUTOMATIC);
	gtk_widget_set_size_request(scrolled, -1, 100);

	text_view = gtk_text_view_new();
	gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(text_view), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD);

	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
	
	/* Crear tags de colores para los jugadores */
	gtk_text_buffer_create_tag(buffer, "red_tag", "foreground", "red", NULL);
	gtk_text_buffer_create_tag(buffer, "yellow_tag", "foreground", "yellow", "background", "black", NULL);
	gtk_text_buffer_create_tag(buffer, "blue_tag", "foreground", "blue", NULL);
	gtk_text_buffer_create_tag(buffer, "black_tag", "foreground", "black", NULL);
	gtk_text_buffer_create_tag(buffer, "pink_tag", "foreground", "pink", NULL);
	gtk_text_buffer_create_tag(buffer, "green_tag", "foreground", "green", NULL);
	gtk_text_buffer_create_tag(buffer, "default_tag", "foreground", "black", NULL);

	gtk_container_add(GTK_CONTAINER(scrolled), text_view);

	main_message_area = GTK_TEXT_VIEW(text_view);
	return scrolled;
}


/**
 * Crea la ventana principal
 */
GtkWidget* create_mainwin(void)
{
	GtkWidget *vbox;
	GtkWidget *menubar;
	GtkWidget *toolbar_widget;
	GtkWidget *hpaned;
	GtkWidget *scrolled_map;
	GtkWidget *right_vbox;
	GtkWidget *message_area;

	/* Crear ventana principal */
	main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(main_window), _("TEG - Tenes Empanadas Graciela"));
	gtk_window_set_default_size(GTK_WINDOW(main_window), 
	                            gui_theme.screen_size_x > 0 ? gui_theme.screen_size_x : 1024,
	                            gui_theme.screen_size_y > 0 ? gui_theme.screen_size_y : 768);

	g_signal_connect(main_window, "destroy", G_CALLBACK(on_exit_activate), NULL);

	/* Contenedor principal vertical */
	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(main_window), vbox);

	/* Barra de menú */
	menubar = create_menubar(main_window);
	gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);

	/* Barra de herramientas */
	toolbar_widget = create_toolbar();
	gtk_box_pack_start(GTK_BOX(vbox), toolbar_widget, FALSE, FALSE, 0);

	/* Panel dividido horizontalmente */
	hpaned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_box_pack_start(GTK_BOX(vbox), hpaned, TRUE, TRUE, 0);

	/* Lado izquierdo: Mapa en scrolled window */
	scrolled_map = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_map),
	                               GTK_POLICY_AUTOMATIC,
	                               GTK_POLICY_AUTOMATIC);
	gtk_paned_pack1(GTK_PANED(hpaned), scrolled_map, TRUE, TRUE);

	/* Crear widget del mapa */
	canvas_map = G_country_create_map_widget();
	
	/* Crear un viewport para contener el mapa y habilitar eventos */
	GtkWidget *viewport = gtk_viewport_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(viewport), canvas_map);
	gtk_container_add(GTK_CONTAINER(scrolled_map), viewport);
	
	/* Asegurar que el canvas puede recibir eventos */
	gtk_widget_set_can_focus(canvas_map, TRUE);

	/* Lado derecho: Panel con información */
	right_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_widget_set_size_request(right_vbox, 250, -1);
	gtk_paned_pack2(GTK_PANED(hpaned), right_vbox, FALSE, TRUE);

	/* Área de mensajes */
	message_area = create_message_area();
	gtk_box_pack_start(GTK_BOX(right_vbox), message_area, TRUE, TRUE, 0);

	/* Barra de estado */
	statusbar = gtk_label_new(_("Welcome to TEG!"));
	gtk_label_set_xalign(GTK_LABEL(statusbar), 0.0);
	gtk_box_pack_start(GTK_BOX(vbox), statusbar, FALSE, FALSE, 2);

	return main_window;
}


/**
 * Crea los items del canvas (países)
 */
int create_canvas_items(void)
{
	int i;

	/* Inicializar continentes y países */
	G_cont_init();
	G_country_init();

	/* Cargar gráficos de cada país */
	for (i = 0; i < COUNTRIES_CANT; i++) {
		G_country_draw(i);
	}

	return 0;
}


/**
 * Actualiza la sensibilidad de los botones de la toolbar según el estado del juego
 */
TEG_STATUS set_sensitive_tb(void)
{
	PLAYER_STATUS e = ESTADO_GET();

	/* CONNECT & START */
	if (e == PLAYER_STATUS_DESCONECTADO) {
		if (menu_connect) gtk_widget_set_sensitive(menu_connect, TRUE);
		if (menu_disconnect) gtk_widget_set_sensitive(menu_disconnect, FALSE);
		if (menu_start) gtk_widget_set_sensitive(menu_start, FALSE);
		if (toolbar_connect_button) gtk_widget_set_sensitive(GTK_WIDGET(toolbar_connect_button), TRUE);
		if (toolbar_start_button) gtk_widget_set_sensitive(GTK_WIDGET(toolbar_start_button), FALSE);

	} else if (e == PLAYER_STATUS_HABILITADO) {
		if (menu_connect) gtk_widget_set_sensitive(menu_connect, FALSE);
		if (menu_disconnect) gtk_widget_set_sensitive(menu_disconnect, TRUE);
		if (menu_start) gtk_widget_set_sensitive(menu_start, TRUE);
		if (toolbar_connect_button) gtk_widget_set_sensitive(GTK_WIDGET(toolbar_connect_button), FALSE);
		if (toolbar_start_button) gtk_widget_set_sensitive(GTK_WIDGET(toolbar_start_button), TRUE);

	} else {
		if (menu_connect) gtk_widget_set_sensitive(menu_connect, FALSE);
		if (menu_disconnect) gtk_widget_set_sensitive(menu_disconnect, TRUE);
		if (menu_start) gtk_widget_set_sensitive(menu_start, FALSE);
		if (toolbar_connect_button) gtk_widget_set_sensitive(GTK_WIDGET(toolbar_connect_button), FALSE);
		if (toolbar_start_button) gtk_widget_set_sensitive(GTK_WIDGET(toolbar_start_button), FALSE);
	}

	/* STATUS view menu */
	if (menu_status) {
		if (e == PLAYER_STATUS_DESCONECTADO) {
			gtk_widget_set_sensitive(menu_status, FALSE);
		} else {
			gtk_widget_set_sensitive(menu_status, TRUE);
		}
	}

	return TEG_STATUS_SUCCESS;
}


/**
 * Ventana de tropas (mover ejércitos después de conquista)
 */
void tropas_window(int src, int dst, int cant)
{
	GtkWidget *dialog;
	GtkWidget *content_area;
	GtkWidget *label;
	GtkWidget *scale;
	char buf[256];
	gint response;

	tropas_src = src;
	tropas_dst = dst;
	tropas_cant = cant;

	snprintf(buf, sizeof(buf) - 1,
	         _("You conquered '%s' in your attack from '%s'!!\n"
	           "Choose how many armies to move:"),
	         countries_get_name(dst), countries_get_name(src));

	dialog = gtk_dialog_new_with_buttons(_("Move armies"),
	                                      GTK_WINDOW(main_window),
	                                      GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
	                                      _("_OK"), GTK_RESPONSE_OK,
	                                      _("_Cancel"), GTK_RESPONSE_CANCEL,
	                                      NULL);

	content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

	label = gtk_label_new(buf);
	gtk_box_pack_start(GTK_BOX(content_area), label, FALSE, FALSE, 10);

	scale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 1, cant, 1);
	gtk_range_set_value(GTK_RANGE(scale), 1);
	gtk_box_pack_start(GTK_BOX(content_area), scale, FALSE, FALSE, 10);

	tropas_hscale_cant = scale;

	gtk_widget_show_all(dialog);
	response = gtk_dialog_run(GTK_DIALOG(dialog));

	if (response == GTK_RESPONSE_OK) {
		int armies = (int)gtk_range_get_value(GTK_RANGE(scale));
		out_tropas(tropas_src, tropas_dst, armies);
	}

	gtk_widget_destroy(dialog);
	tropas_dialog = NULL;
}


/**
 * Ventana de reagrupamiento
 */
void reagrupe_window(int src, int dst, int cant)
{
	GtkWidget *dialog;
	GtkWidget *content_area;
	GtkWidget *label;
	GtkWidget *scale;
	char buf[256];
	gint response;

	reagrupe_src = src;
	reagrupe_dst = dst;
	reagrupe_cant = cant;

	snprintf(buf, sizeof(buf) - 1,
	         _("Moving armies from '%s' to '%s'\n"
	           "Choose how many armies to move:"),
	         countries_get_name(src), countries_get_name(dst));

	dialog = gtk_dialog_new_with_buttons(_("Regroup armies"),
	                                      GTK_WINDOW(main_window),
	                                      GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
	                                      _("_OK"), GTK_RESPONSE_OK,
	                                      _("_Cancel"), GTK_RESPONSE_CANCEL,
	                                      NULL);

	content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

	label = gtk_label_new(buf);
	gtk_box_pack_start(GTK_BOX(content_area), label, FALSE, FALSE, 10);

	scale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, cant, 1);
	gtk_range_set_value(GTK_RANGE(scale), 0);
	gtk_box_pack_start(GTK_BOX(content_area), scale, FALSE, FALSE, 10);

	reagrupe_hscale_cant = scale;

	gtk_widget_show_all(dialog);
	response = gtk_dialog_run(GTK_DIALOG(dialog));

	if (response == GTK_RESPONSE_OK) {
		int armies = (int)gtk_range_get_value(GTK_RANGE(scale));
		if (armies > 0) {
			reagrupe_out(reagrupe_src, reagrupe_dst, armies);
		}
	}

	gtk_widget_destroy(dialog);
	reagrupe_dialog = NULL;
}

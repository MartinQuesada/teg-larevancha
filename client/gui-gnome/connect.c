/*	$Id: connect.c,v 1.6 2004/10/04 19:47:49 wfx Exp $	*/
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
 * @file connect.c
 * Connection dialogs, color dialogs, and game type - Migrado a GTK3
 */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <assert.h>
#include <gtk/gtk.h>

#include "gnome-compat.h"
#include "gui.h"
#include "client.h"
#include "support.h"
#include "interface.h"
#include "callbacks.h"
#include "connect.h"
#include "priv.h"
#include "colors.h"
#include "outputfd.h"

/* Declaración correcta de out_color (el header tiene bug) */
TEG_STATUS out_color(int color);

/* Widgets del diálogo de conexión */
static GtkWidget *connect_window = NULL;
static GtkWidget *con_entry_name = NULL;
static GtkWidget *con_spinner_port = NULL;
static GtkWidget *con_entry_server = NULL;
static GtkWidget *button_launch = NULL;
static GtkWidget *button_observe = NULL;

/* Widgets del diálogo de selección de color */
static GtkWidget *colortype_dialog = NULL;
static GtkWidget *boton_color[TEG_MAX_PLAYERS] = { NULL };

/* Widgets del diálogo de tipo de juego */
static GtkWidget *gametype_dialog = NULL;
static GtkWidget *gametype_button_conqworld = NULL;
static GtkWidget *gametype_button_secret = NULL;
static GtkWidget *gametype_button_viewall = NULL;
static GtkWidget *gametype_button_fow = NULL;
static GtkWidget *gametype_button_commonmission = NULL;
static GtkWidget *gametype_spinner_armies1 = NULL;
static GtkWidget *gametype_spinner_armies2 = NULL;

/* Metaserver */
static GtkListStore *metaserver_store = NULL;

enum {
	METASERVER_NAME,
	METASERVER_PORT,
	METASERVER_VERSION,
	METASERVER_COMMENT,
};


/**
 * Realiza la conexión real al servidor
 */
static TEG_STATUS connect_real(void)
{
	if (teg_connect() == TEG_STATUS_SUCCESS) {
		/* En GTK3 usamos GIOChannel en lugar de gdk_input_add */
		GIOChannel *channel = g_io_channel_unix_new(g_game.fd);
		g_io_channel_set_encoding(channel, NULL, NULL);  /* Binary mode */
		g_io_channel_set_buffered(channel, FALSE);
		
		gui_private.tag = g_io_add_watch(channel, 
		                                  G_IO_IN | G_IO_ERR | G_IO_HUP,
		                                  pre_client_recv_gio, NULL);
		g_io_channel_unref(channel);

		if (!g_game.with_ggz) {
			out_id();
			if (connect_window) {
				gtk_widget_destroy(connect_window);
				connect_window = NULL;
			}
		}

		countries_redraw_all();
		return TEG_STATUS_SUCCESS;
	}
	return TEG_STATUS_ERROR;
}


/**
 * Callback del botón conectar
 */
static void connect_button_clicked(GtkWidget *button, gpointer user_data)
{
	const char *name, *server;
	
	name = gtk_entry_get_text(GTK_ENTRY(con_entry_name));
	server = gtk_entry_get_text(GTK_ENTRY(con_entry_server));
	
	strncpy(g_game.myname, name, PLAYERNAME_MAX_LEN);
	strncpy(g_game.sername, server, SERVER_NAMELEN);
	g_game.serport = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(con_spinner_port));

	g_game.observer = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button_observe));

	/* Si se marcó "iniciar servidor local" */
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button_launch))) {
		launch_server(g_game.serport);
	}

	/* Guardar en GSettings si está disponible */
	if (g_settings) {
		g_settings_set_int(g_settings, "port", g_game.serport);
		g_settings_set_string(g_settings, "servername", g_game.sername);
		g_settings_set_string(g_settings, "playername", g_game.myname);
	}

	connect_real();
}


/**
 * Callback cuando se marca "iniciar servidor local"
 */
static void launch_toggled(GtkToggleButton *button, gpointer user_data)
{
	if (gtk_toggle_button_get_active(button)) {
		gtk_entry_set_text(GTK_ENTRY(con_entry_server), "localhost");
	}
}


/**
 * Callback para cerrar el diálogo
 */
static void connect_dialog_response(GtkDialog *dialog, gint response_id, gpointer user_data)
{
	if (response_id == GTK_RESPONSE_OK) {
		connect_button_clicked(NULL, NULL);
	} else {
		gtk_widget_destroy(GTK_WIDGET(dialog));
		connect_window = NULL;
	}
}


/**
 * Muestra el diálogo de conexión
 */
void connect_view(void)
{
	GtkWidget *content_area;
	GtkWidget *grid;
	GtkWidget *label;
	GtkWidget *frame;
	GtkWidget *vbox;
	GtkAdjustment *adj;

	/* Si ya existe, mostrar y enfocar */
	if (connect_window != NULL) {
		gtk_widget_show_all(connect_window);
		gtk_window_present(GTK_WINDOW(connect_window));
		return;
	}

	/* Crear diálogo */
	connect_window = gtk_dialog_new_with_buttons(
		_("Connect to server"),
		GTK_WINDOW(main_window),
		GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
		_("_Connect"), GTK_RESPONSE_OK,
		_("_Cancel"), GTK_RESPONSE_CANCEL,
		NULL);

	gtk_window_set_default_size(GTK_WINDOW(connect_window), 400, 300);
	
	g_signal_connect(connect_window, "response", 
	                 G_CALLBACK(connect_dialog_response), NULL);
	g_signal_connect(connect_window, "destroy",
	                 G_CALLBACK(gtk_widget_destroyed), &connect_window);

	content_area = gtk_dialog_get_content_area(GTK_DIALOG(connect_window));

	/* Frame de opciones del servidor */
	frame = gtk_frame_new(_("Server Options"));
	gtk_container_set_border_width(GTK_CONTAINER(frame), 10);
	gtk_box_pack_start(GTK_BOX(content_area), frame, FALSE, FALSE, 0);

	grid = gtk_grid_new();
	gtk_grid_set_row_spacing(GTK_GRID(grid), 6);
	gtk_grid_set_column_spacing(GTK_GRID(grid), 12);
	gtk_container_set_border_width(GTK_CONTAINER(grid), 10);
	gtk_container_add(GTK_CONTAINER(frame), grid);

	/* Puerto del servidor */
	label = gtk_label_new(_("Server port:"));
	gtk_widget_set_halign(label, GTK_ALIGN_END);
	gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

	adj = gtk_adjustment_new(g_game.serport, 1.0, 65536.0, 1.0, 100.0, 0.0);
	con_spinner_port = gtk_spin_button_new(adj, 1.0, 0);
	gtk_widget_set_hexpand(con_spinner_port, TRUE);
	gtk_grid_attach(GTK_GRID(grid), con_spinner_port, 1, 0, 1, 1);

	/* Nombre del servidor */
	label = gtk_label_new(_("Server name:"));
	gtk_widget_set_halign(label, GTK_ALIGN_END);
	gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);

	con_entry_server = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(con_entry_server), g_game.sername);
	gtk_widget_set_hexpand(con_entry_server, TRUE);
	gtk_grid_attach(GTK_GRID(grid), con_entry_server, 1, 1, 1, 1);

	/* Nombre del jugador */
	label = gtk_label_new(_("Player name:"));
	gtk_widget_set_halign(label, GTK_ALIGN_END);
	gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);

	con_entry_name = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(con_entry_name), g_game.myname);
	gtk_widget_set_hexpand(con_entry_name, TRUE);
	gtk_grid_attach(GTK_GRID(grid), con_entry_name, 1, 2, 1, 1);

	/* Opciones adicionales */
	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
	gtk_box_pack_start(GTK_BOX(content_area), vbox, FALSE, FALSE, 0);

	/* Iniciar servidor local */
	button_launch = gtk_check_button_new_with_label(_("Start server locally"));
	gtk_box_pack_start(GTK_BOX(vbox), button_launch, FALSE, FALSE, 0);
	g_signal_connect(button_launch, "toggled", G_CALLBACK(launch_toggled), NULL);

	/* Modo observador */
	button_observe = gtk_check_button_new_with_label(_("Don't play, just observe"));
	gtk_box_pack_start(GTK_BOX(vbox), button_observe, FALSE, FALSE, 0);

	gtk_widget_show_all(connect_window);
}


/**
 * Callback OK del diálogo de selección de color
 */
static void colortype_ok_clicked(GtkWidget *button, gpointer user_data)
{
	int i;

	for (i = 0; i < TEG_MAX_PLAYERS; i++) {
		if (boton_color[i] && 
		    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(boton_color[i]))) {
			break;
		}
	}

	out_color(i);

	gtk_widget_destroy(colortype_dialog);
	colortype_dialog = NULL;
}


/**
 * Muestra el diálogo de selección de color
 */
void colortype_view(char *c)
{
	GtkWidget *content_area;
	GtkWidget *frame;
	GtkWidget *grid;
	GtkWidget *image;
	GtkWidget *vbox;
	int i;
	int first_active = 1;
	GSList *group = NULL;

	if (colortype_dialog != NULL) {
		gtk_window_present(GTK_WINDOW(colortype_dialog));
		return;
	}

	colortype_dialog = gtk_dialog_new_with_buttons(
		_("Select your color"),
		GTK_WINDOW(main_window),
		GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
		_("_OK"), GTK_RESPONSE_OK,
		NULL);

	g_signal_connect(colortype_dialog, "destroy",
	                 G_CALLBACK(gtk_widget_destroyed), &colortype_dialog);

	content_area = gtk_dialog_get_content_area(GTK_DIALOG(colortype_dialog));

	frame = gtk_frame_new(_("Select your desired color"));
	gtk_container_set_border_width(GTK_CONTAINER(frame), 10);
	gtk_box_pack_start(GTK_BOX(content_area), frame, TRUE, TRUE, 0);

	grid = gtk_grid_new();
	gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
	gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
	gtk_container_set_border_width(GTK_CONTAINER(grid), 10);
	gtk_container_add(GTK_CONTAINER(frame), grid);

	for (i = 0; i < TEG_MAX_PLAYERS; i++) {
		vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);

		/* Imagen del color */
		if (g_color_players[i]) {
			image = gtk_image_new_from_pixbuf(g_color_players[i]);
			gtk_box_pack_start(GTK_BOX(vbox), image, FALSE, FALSE, 0);
		}

		/* Radio button - g_colores es un array de strings en common/common.h */
		boton_color[i] = gtk_radio_button_new_with_label(group, _(g_colores[i]));
		group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(boton_color[i]));
		gtk_box_pack_start(GTK_BOX(vbox), boton_color[i], FALSE, FALSE, 0);

		/* Posicionar en grid 2x3 */
		gtk_grid_attach(GTK_GRID(grid), vbox, i % 3, i / 3, 1, 1);

		/* Habilitar/deshabilitar según disponibilidad */
		gtk_widget_set_sensitive(vbox, !c[i]);
		if (!c[i] && first_active) {
			first_active = 0;
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(boton_color[i]), TRUE);
		}
	}

	g_signal_connect(colortype_dialog, "response",
	                 G_CALLBACK(colortype_ok_clicked), NULL);

	gtk_widget_show_all(colortype_dialog);
}


/**
 * Callback cuando se habilita/deshabilita misiones secretas
 */
static void button_secret_toggled(GtkToggleButton *button, gpointer user_data)
{
	gboolean active = gtk_toggle_button_get_active(button);
	gtk_widget_set_sensitive(gametype_button_commonmission, active);
}


/**
 * Callback OK del diálogo de tipo de juego
 */
static void gametype_ok_clicked(GtkWidget *button, gpointer user_data)
{
	int a, b, c, ar1, ar2;

	a = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gametype_button_conqworld));
	b = !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gametype_button_viewall));
	c = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gametype_button_commonmission));
	ar1 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(gametype_spinner_armies1));
	ar2 = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(gametype_spinner_armies2));

	out_set_typeofgame(a, b, c, ar1, ar2);
	out_start();

	gtk_widget_destroy(gametype_dialog);
	gametype_dialog = NULL;
}


/**
 * Muestra el diálogo de selección de tipo de juego
 */
void gametype_view(void)
{
	GtkWidget *content_area;
	GtkWidget *frame;
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *label;
	GtkAdjustment *adj;
	GSList *group;

	if (gametype_dialog != NULL) {
		gtk_window_present(GTK_WINDOW(gametype_dialog));
		return;
	}

	gametype_dialog = gtk_dialog_new_with_buttons(
		_("Select type of game"),
		GTK_WINDOW(main_window),
		GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
		_("_OK"), GTK_RESPONSE_OK,
		NULL);

	g_signal_connect(gametype_dialog, "destroy",
	                 G_CALLBACK(gtk_widget_destroyed), &gametype_dialog);
	g_signal_connect(gametype_dialog, "response",
	                 G_CALLBACK(gametype_ok_clicked), NULL);

	content_area = gtk_dialog_get_content_area(GTK_DIALOG(gametype_dialog));

	/* Frame de Misiones */
	frame = gtk_frame_new(_("Missions"));
	gtk_container_set_border_width(GTK_CONTAINER(frame), 10);
	gtk_box_pack_start(GTK_BOX(content_area), frame, FALSE, FALSE, 0);

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
	gtk_container_add(GTK_CONTAINER(frame), vbox);

	gametype_button_conqworld = gtk_radio_button_new_with_label(
		NULL, _("Play to conquer the world"));
	gtk_box_pack_start(GTK_BOX(vbox), gametype_button_conqworld, FALSE, FALSE, 0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gametype_button_conqworld), TRUE);

	group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(gametype_button_conqworld));
	gametype_button_secret = gtk_radio_button_new_with_label(
		group, _("Play with secret missions"));
	gtk_box_pack_start(GTK_BOX(vbox), gametype_button_secret, FALSE, FALSE, 0);
	g_signal_connect(gametype_button_secret, "toggled",
	                 G_CALLBACK(button_secret_toggled), NULL);

	gametype_button_commonmission = gtk_check_button_new_with_label(
		_("Play with common mission\n(One wins, also, conquering 45 countries)"));
	gtk_widget_set_sensitive(gametype_button_commonmission, FALSE);
	gtk_widget_set_margin_start(gametype_button_commonmission, 20);
	gtk_box_pack_start(GTK_BOX(vbox), gametype_button_commonmission, FALSE, FALSE, 0);

	/* Frame de Visibilidad */
	frame = gtk_frame_new(_("Visibility"));
	gtk_container_set_border_width(GTK_CONTAINER(frame), 10);
	gtk_box_pack_start(GTK_BOX(content_area), frame, FALSE, FALSE, 0);

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
	gtk_container_add(GTK_CONTAINER(frame), vbox);

	gametype_button_viewall = gtk_radio_button_new_with_label(
		NULL, _("Standard (You see all countries)"));
	gtk_box_pack_start(GTK_BOX(vbox), gametype_button_viewall, FALSE, FALSE, 0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gametype_button_viewall), TRUE);

	group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(gametype_button_viewall));
	gametype_button_fow = gtk_radio_button_new_with_label(
		group, _("Fog of War (You see only border countries)"));
	gtk_box_pack_start(GTK_BOX(vbox), gametype_button_fow, FALSE, FALSE, 0);

	/* Frame de Ejércitos */
	frame = gtk_frame_new(_("Armies"));
	gtk_container_set_border_width(GTK_CONTAINER(frame), 10);
	gtk_box_pack_start(GTK_BOX(content_area), frame, FALSE, FALSE, 0);

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
	gtk_container_add(GTK_CONTAINER(frame), vbox);

	/* Ejércitos paso 1 */
	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

	label = gtk_label_new(_("Armies to place in the 1st step:"));
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);

	adj = gtk_adjustment_new(8, 1.0, 30.0, 1.0, 5.0, 0.0);
	gametype_spinner_armies1 = gtk_spin_button_new(adj, 1.0, 0);
	gtk_box_pack_end(GTK_BOX(hbox), gametype_spinner_armies1, FALSE, FALSE, 0);

	/* Ejércitos paso 2 */
	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

	label = gtk_label_new(_("Armies to place in the 2nd step:"));
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);

	adj = gtk_adjustment_new(4, 1.0, 25.0, 1.0, 5.0, 0.0);
	gametype_spinner_armies2 = gtk_spin_button_new(adj, 1.0, 0);
	gtk_box_pack_end(GTK_BOX(hbox), gametype_spinner_armies2, FALSE, FALSE, 0);

	gtk_widget_show_all(gametype_dialog);
}

/*	$Id: mission.c,v 1.2 2002/08/31 17:29:31 riq Exp $	*/
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
 * @file mission.c
 * Crea las tarjetas que fueron obtenidas
 */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "gnome-compat.h"

#include "gui.h"
#include "client.h"

#include "chatline.h"
#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "mission.h"
#include "fonts.h"

static GdkPixbuf *tar_mission = NULL;
static int mission_number = -1;

static TEG_STATUS mission_load()
{
	char *filename=NULL;

	if(!tar_mission) {
		filename = load_pixmap_file ("objetivo.png");
		tar_mission = gdk_pixbuf_new_from_file(filename, NULL);
		if(filename) g_free( filename );
		if( tar_mission == NULL ) {
			g_warning(_("Error, couldn't find file: objetivo.png"));
			return TEG_STATUS_ERROR;
		}
	}

	return TEG_STATUS_SUCCESS;
}

/* Widgets estáticos para el diálogo de misión */
static GtkWidget *mission_image = NULL;
static GtkWidget *mission_label1 = NULL;
static GtkWidget *mission_label2 = NULL;

static void mission_update_content(void)
{
	if (mission_number == -1)
		return;

	/* Actualizar imagen */
	if (mission_image && tar_mission) {
		gtk_image_set_from_pixbuf(GTK_IMAGE(mission_image), tar_mission);
	}

	/* Actualizar texto de la misión */
	if (mission_label1) {
		gtk_label_set_text(GTK_LABEL(mission_label1), missions_get_name(mission_number));
	}

	/* Actualizar texto secundario */
	if (mission_label2) {
		if (mission_number == g_game.secret_mission || mission_number == MISSION_COMMON) {
			char *text;
			if (mission_number == g_game.secret_mission)
				text = _("[This is your secret mission.]");
			else
				text = _("[This is the common mission.]");
			gtk_label_set_text(GTK_LABEL(mission_label2), text);
			gtk_widget_show(mission_label2);
		} else {
			gtk_widget_hide(mission_label2);
		}
	}
}

static TEG_STATUS mission_view_number( GtkWidget *dialog )
{
	mission_update_content();
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS mission_view_fake_number( GtkWidget *frame, int mission )
{
	GtkWidget	*image_widget;
	GtkWidget	*label1;
	GtkWidget	*label2;
	char *text;

	if( mission_load() != TEG_STATUS_SUCCESS )
		return TEG_STATUS_ERROR;

	/* GTK3: Usamos GtkImage con GdkPixbuf */
	image_widget = gtk_image_new_from_pixbuf(tar_mission);

	/* Creamos etiquetas para el texto */
	label1 = gtk_label_new(missions_get_name(mission));

	text = _("[Accomplished mission]");
	label2 = gtk_label_new(text);

	/* Creamos un contenedor vertical */
	GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_box_pack_start(GTK_BOX(vbox), image_widget, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), label1, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), label2, FALSE, FALSE, 0);

	gtk_container_add(GTK_CONTAINER(frame), vbox);

	return TEG_STATUS_SUCCESS;
}

void mission_view_prev( GtkWidget *button, gpointer data )
{
	GtkWidget *dialog = (GtkWidget*) data;
	int last = missions_cant();
	int first_mission;

	if( mission_number == -1 )
		return;

	first_mission = g_game.with_common_mission ? MISSION_COMMON : MISSION_COMMON + 1;

	mission_number--;
	if( mission_number < first_mission )
		mission_number = last -1;

	mission_view_number( dialog );
	return;
}

void mission_view_next( GtkWidget *button, gpointer data )
{
	GtkWidget *dialog = (GtkWidget*) data;
	int last = missions_cant();
	int first_mission;

	if( mission_number == -1 )
		return;

	first_mission = g_game.with_common_mission ? MISSION_COMMON : MISSION_COMMON + 1;

	mission_number++;
	if( mission_number >= last-1 )
		mission_number = first_mission;

	mission_view_number( dialog );
	return;
}

/* funciones de creacion */
void mission_view()
{
	static GtkWidget *dialog = NULL;
	static GtkWidget *button_prev = NULL;
	static GtkWidget *button_next = NULL;

	if (g_game.secret_mission < 0) {
		textmsg(M_INF, _("Requesting a mission..."));
		out_missions();
		return;
	}

	mission_number = g_game.secret_mission;

	if (tar_mission == NULL) {
		if (mission_load() != TEG_STATUS_SUCCESS) {
			textmsg(M_INF, _("Your mission is: %s"), missions_get_name(mission_number));
			return;
		}
	}

	if (dialog == NULL) {
		/* GTK3: Crear ventana */
		dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
		gtk_window_set_title(GTK_WINDOW(dialog), _("Secret mission"));
		gtk_window_set_default_size(GTK_WINDOW(dialog), 450, 350);
		gtk_container_set_border_width(GTK_CONTAINER(dialog), 10);
		
		/* Hacer que aparezca sobre la ventana principal */
		if (main_window) {
			gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(main_window));
		}
		gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
		gtk_window_set_keep_above(GTK_WINDOW(dialog), TRUE);

		g_signal_connect(dialog, "destroy", G_CALLBACK(gtk_widget_destroyed), &dialog);

		/* Contenedor principal */
		GtkWidget *main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
		gtk_container_add(GTK_CONTAINER(dialog), main_vbox);

		/* Área de contenido de la misión */
		GtkWidget *content_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
		gtk_box_pack_start(GTK_BOX(main_vbox), content_box, TRUE, TRUE, 0);

		/* Imagen de la misión */
		mission_image = gtk_image_new_from_pixbuf(tar_mission);
		gtk_box_pack_start(GTK_BOX(content_box), mission_image, FALSE, FALSE, 5);

		/* Etiqueta con el nombre de la misión */
		mission_label1 = gtk_label_new(missions_get_name(mission_number));
		gtk_label_set_line_wrap(GTK_LABEL(mission_label1), TRUE);
		gtk_label_set_justify(GTK_LABEL(mission_label1), GTK_JUSTIFY_CENTER);
		gtk_box_pack_start(GTK_BOX(content_box), mission_label1, FALSE, FALSE, 5);

		/* Etiqueta secundaria */
		mission_label2 = gtk_label_new("");
		gtk_box_pack_start(GTK_BOX(content_box), mission_label2, FALSE, FALSE, 5);

		/* Botones de navegación */
		GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
		gtk_widget_set_halign(button_box, GTK_ALIGN_CENTER);
		gtk_box_pack_end(GTK_BOX(main_vbox), button_box, FALSE, FALSE, 0);

		button_prev = gtk_button_new_with_label(_("Previous"));
		button_next = gtk_button_new_with_label(_("Next"));
		GtkWidget *button_close = gtk_button_new_with_label(_("Close"));

		gtk_box_pack_start(GTK_BOX(button_box), button_prev, FALSE, FALSE, 5);
		gtk_box_pack_start(GTK_BOX(button_box), button_next, FALSE, FALSE, 5);
		gtk_box_pack_start(GTK_BOX(button_box), button_close, FALSE, FALSE, 5);

		g_signal_connect(button_prev, "clicked", G_CALLBACK(mission_view_prev), dialog);
		g_signal_connect(button_next, "clicked", G_CALLBACK(mission_view_next), dialog);
		g_signal_connect_swapped(button_close, "clicked", G_CALLBACK(gtk_widget_destroy), dialog);
	}

	/* Habilitar/deshabilitar botones según la misión */
	if (g_game.secret_mission == MISSION_CONQWORLD) {
		if (button_prev) gtk_widget_set_sensitive(button_prev, FALSE);
		if (button_next) gtk_widget_set_sensitive(button_next, FALSE);
	} else {
		if (button_prev) gtk_widget_set_sensitive(button_prev, TRUE);
		if (button_next) gtk_widget_set_sensitive(button_next, TRUE);
	}

	/* Actualizar contenido */
	mission_update_content();

	gtk_widget_show_all(dialog);
	gtk_window_present(GTK_WINDOW(dialog));
}

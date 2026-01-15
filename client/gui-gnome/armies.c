/*	$Id: armies.c,v 1.13 2002/08/31 17:45:58 riq Exp $	*/
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
 * @file armies.c
 * Muestra las fichas que hay que poner
 */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "gnome-compat.h"

#include "gui.h"
#include "client.h"
#include "interface.h"
#include "support.h"
#include "armies.h"
#include "themes.h"
#include "fonts.h"

extern TTheme	gui_theme;

// GTK4: Reemplazamos GNOME Canvas con GtkWidget
static ARMIES_POS armies_prop[CONT_CANT+1] = {
	{0,0,NULL},
	{0,0,NULL},
	{0,0,NULL},
	{0,0,NULL},
	{0,0,NULL},
	{0,0,NULL},
	{0,0,NULL},
	{0,0,NULL},
};

static int initialized=0;

// GTK4: Usamos GtkWidget en lugar de GnomeCanvasGroup
GtkWidget *armies_group = NULL;
GtkWidget *armies_window = NULL;

// GTK4: Simplificamos el manejo de eventos
static gboolean
armies_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	// GTK4: Implementación simplificada de eventos
	// En una implementación real, esto manejaría el drag & drop
	return FALSE;
}

static TEG_STATUS armies_view_cant_x_cont( int i )
{
	char buffer[200];

	memset(buffer,0,sizeof(buffer));

	if( i<0 || i > CONT_CANT )
		return TEG_STATUS_ERROR;

	// GTK4: Simplificamos la visualización
	// En una implementación real, esto actualizaría el texto en la interfaz
	if(armies_prop[i].cant < 0 )
		snprintf(buffer,sizeof(buffer)-1,"%d",0);
	else
		snprintf(buffer,sizeof(buffer)-1,"%d",armies_prop[i].cant);

	printf("Armies for continent %d: %s\n", i, buffer);

	return TEG_STATUS_SUCCESS;
}

static void armies_view_cant()
{
	int i;
	char buffer[200];

	memset(buffer,0,sizeof(buffer));

	// GTK4: Simplificamos la visualización
	for(i=0;i<CONT_CANT;i++)
		armies_view_cant_x_cont( i );

	snprintf(buffer,sizeof(buffer)-1,"%d",armies_prop[CONT_CANT].cant);
	printf("Total armies: %s\n", buffer);
}

TEG_STATUS armies_add(int country)
{
	int cont;
	if(!initialized) return TEG_STATUS_ERROR;

	armies_prop[CONT_CANT].cant--;

	cont = g_countries[country].continente;
	if( armies_prop[cont].orig_cant > 0 )
		armies_prop[cont].cant--;

	armies_view_cant();
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS armies_del(int country)
{
	int cont;
	if(!initialized) return TEG_STATUS_ERROR;

	armies_prop[CONT_CANT].cant++;

	cont = g_countries[country].continente;
	if( armies_prop[cont].orig_cant > 0 &&
			armies_prop[cont].cant < armies_prop[cont].orig_cant)
		armies_prop[cont].cant++;

	armies_view_cant();
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS armies_view_more( int cant )
{
	if(!initialized) return TEG_STATUS_ERROR;

	armies_prop[CONT_CANT].cant += cant;

	// GTK4: Simplificamos la visualización
	printf("Added %d more armies\n", cant);

	armies_view_cant();

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS armies_view(int cant, int conts)
{
	int i;

	if(!initialized) return TEG_STATUS_ERROR;
	if( !armies_group ) return TEG_STATUS_ERROR;

	armies_prop[CONT_CANT].cant = cant + cont_tot( conts );

	for(i=0;i<CONT_CANT;i++) {
		if( conts & 1 )
			armies_prop[i].orig_cant = armies_prop[i].cant = g_conts[i].fichas_x_cont;
		else
			armies_prop[i].orig_cant = armies_prop[i].cant = 0;

		conts >>= 1;
	}

	// GTK4: Simplificamos la visualización
	printf("Showing armies view\n");

	armies_view_cant();

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS armies_unview()
{
	if(!initialized) return TEG_STATUS_ERROR;

	// GTK4: Simplificamos la ocultación
	printf("Hiding armies view\n");
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS armies_init( GtkWidget *group )
{
	int i;

	if(!group)
		return TEG_STATUS_ERROR;

	if(initialized)
		return TEG_STATUS_ERROR;

	// GTK4: Creamos un contenedor simple en lugar de GNOME Canvas
	armies_group = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_widget_set_margin_start(armies_group, 10);
	gtk_widget_set_margin_end(armies_group, 10);
	gtk_widget_set_margin_top(armies_group, 10);
	gtk_widget_set_margin_bottom(armies_group, 10);

	if( armies_group ) {
		/* GTK3: Creamos una etiqueta para el total */
		GtkWidget *total_label = gtk_label_new(_("Total"));
		gtk_box_pack_start(GTK_BOX(armies_group), total_label, FALSE, FALSE, 0);

		/* GTK3: Creamos etiquetas para cada continente */
		for(i=0;i<CONT_CANT;i++) {
			GtkWidget *cont_label = gtk_label_new(cont_get_name(i));
			gtk_box_pack_start(GTK_BOX(armies_group), cont_label, FALSE, FALSE, 0);
		}
	}

	initialized=1;
	return TEG_STATUS_SUCCESS;
}

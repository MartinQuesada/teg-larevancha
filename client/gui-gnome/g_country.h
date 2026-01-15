/*	$Id: g_country.h,v 1.2 2002/09/09 03:52:06 riq Exp $	*/
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
 * @file g_country.h
 * Sistema de países del mapa - Migrado a GTK3/Cairo
 */
#ifndef __TEG_GUI_GNOME_GCOUNTRY_H
#define __TEG_GUI_GNOME_GCOUNTRY_H

#include <gtk/gtk.h>
#include "country.h"  /* Para COUNTRIES_CANT y PCOUNTRY */

/**
 * Estructura de datos gráficos de un país
 * En GTK3/Cairo usamos GdkPixbuf en lugar de GnomeCanvasItem
 */
struct _G_countries {
	char *gfx_name;        /**< Nombre del archivo gráfico del país */
	gint x;                /**< X relativo al continente */
	gint y;                /**< Y relativo al continente */
	gint x_len;            /**< Ancho de la imagen */
	gint y_len;            /**< Alto de la imagen */
	gint x_center;         /**< Offset X para centrar ejércitos */
	gint y_center;         /**< Offset Y para centrar ejércitos */
	GdkPixbuf *pixbuf;     /**< Imagen del país */
};

extern struct _G_countries G_countries[];

/* Funciones de inicialización y carga */
void G_country_init(void);
void G_country_create(int country);
void G_country_draw(int country);
void G_country_draw_ejer(int country);

/* Funciones del widget del mapa */
GtkWidget* G_country_create_map_widget(void);
GtkWidget* G_country_get_map_widget(void);
void G_country_redraw(void);

/* Funciones GUI */
TEG_STATUS gui_country_select(int country);
TEG_STATUS g_country_init(void);

#endif /* __TEG_GUI_GNOME_GCOUNTRY_H */

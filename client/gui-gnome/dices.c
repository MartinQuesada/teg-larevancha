/*	$Id: dices.c,v 1.27 2002/08/31 17:45:58 riq Exp $	*/
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
 * @file dices.c
 * Muestra los dados - Migrado a GTK3 con Cairo
 */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <string.h>

#include "gui.h"
#include "client.h"

#include "dices.h"

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "priv.h"
#include "themes.h"
#include "fonts.h"

#include "common.h"
#include "country.h"
#include "g_country.h"

extern TTheme gui_theme;

/* Pixbufs para las 6 caras de los dados */
static GdkPixbuf *dices[DICES_CANT] = { NULL, NULL, NULL, NULL, NULL, NULL };

/* Widget del área de dibujo */
static GtkWidget *dices_drawing_area = NULL;

/* Estado de inicialización */
static int dices_initialized = 0;

/* Offset del grupo (posición del área de dados en el canvas principal) */
static double group_offset_x = 0;
static double group_offset_y = 0;

/* Estructura para coordenadas */
struct _dices_coord {
	double x;
	double y;
};

/* Posiciones de los dados */
struct _dices_pos {
	struct _dices_coord attacker[3];
	struct _dices_coord attacker_text;
	struct _dices_coord defender[3];
	struct _dices_coord defender_text;
} dices_pos;

/* Tipo de dados */
typedef enum dices_type {
	DICES_ATTACKER,
	DICES_DEFENDER
} dices_type_t;

/* Estado actual de los dados a mostrar */
static struct {
	int visible;
	int attacker_country;
	int defender_country;
	int attacker_dices[3];  /* valores de dados (1-6), 0 = no mostrar */
	int defender_dices[3];
} dices_state = { 0, -1, -1, {0,0,0}, {0,0,0} };


/*
 * Función de dibujo GTK3 - dibuja los dados usando Cairo
 */
static gboolean dices_draw_function(GtkWidget *widget, cairo_t *cr,
                                     gpointer user_data)
{
	int i;
	PangoLayout *layout;
	PangoFontDescription *font_desc;

	if (!dices_state.visible) {
		return FALSE;
	}

	/* Crear layout de Pango para texto */
	layout = pango_cairo_create_layout(cr);
	font_desc = pango_font_description_from_string(HELVETICA_12_BFONT);
	pango_layout_set_font_description(layout, font_desc);

	/* Configurar color del texto desde el tema */
	if (gui_theme.dices_color) {
		GdkRGBA color;
		if (gdk_rgba_parse(&color, gui_theme.dices_color)) {
			cairo_set_source_rgba(cr, color.red, color.green, color.blue, color.alpha);
		} else {
			cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);  /* Negro por defecto */
		}
	} else {
		cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
	}

	/* Dibujar texto del atacante */
	if (dices_state.attacker_country >= 0) {
		const char *name = countries_get_name(dices_state.attacker_country);
		if (name) {
			int text_width, text_height;
			pango_layout_set_text(layout, name, -1);
			pango_layout_get_pixel_size(layout, &text_width, &text_height);
			
			/* Centrar texto en la posición indicada */
			cairo_move_to(cr, 
			              dices_pos.attacker_text.x - text_width / 2.0,
			              dices_pos.attacker_text.y);
			pango_cairo_show_layout(cr, layout);
		}
	}

	/* Dibujar texto del defensor */
	if (dices_state.defender_country >= 0) {
		const char *name = countries_get_name(dices_state.defender_country);
		if (name) {
			int text_width, text_height;
			pango_layout_set_text(layout, name, -1);
			pango_layout_get_pixel_size(layout, &text_width, &text_height);
			
			cairo_move_to(cr,
			              dices_pos.defender_text.x - text_width / 2.0,
			              dices_pos.defender_text.y);
			pango_cairo_show_layout(cr, layout);
		}
	}

	/* Dibujar dados del atacante */
	for (i = 0; i < 3; i++) {
		int dice_value = dices_state.attacker_dices[i];
		if (dice_value > 0 && dice_value <= DICES_CANT && dices[dice_value - 1]) {
			gdk_cairo_set_source_pixbuf(cr, dices[dice_value - 1],
			                            dices_pos.attacker[i].x,
			                            dices_pos.attacker[i].y);
			cairo_paint(cr);
		}
	}

	/* Dibujar dados del defensor */
	for (i = 0; i < 3; i++) {
		int dice_value = dices_state.defender_dices[i];
		if (dice_value > 0 && dice_value <= DICES_CANT && dices[dice_value - 1]) {
			gdk_cairo_set_source_pixbuf(cr, dices[dice_value - 1],
			                            dices_pos.defender[i].x,
			                            dices_pos.defender[i].y);
			cairo_paint(cr);
		}
	}

	/* Limpiar recursos de Pango */
	pango_font_description_free(font_desc);
	g_object_unref(layout);

	return FALSE;
}


/*
 * Carga una imagen de dado desde archivo
 */
static TEG_STATUS dices_load_from_file(char *name, int i)
{
	char *filename = NULL;

	filename = theme_load_file(name);

	dices[i] = gdk_pixbuf_new_from_file(filename, NULL);
	if (dices[i] == NULL) {
		g_warning(_("Error, couldn't find file: %s"), name);
		return TEG_STATUS_ERROR;
	}

	return TEG_STATUS_SUCCESS;
}


/*
 * Carga todas las imágenes de dados
 */
static TEG_STATUS dices_load(void)
{
	int i;

	for (i = 0; i < DICES_CANT; i++) {
		if (!dices[i]) {
			char name[40];
			memset(name, 0, sizeof(name));
			snprintf(name, sizeof(name) - 1, "dice-%d.png", i + 1);
			if (dices_load_from_file(name, i) != TEG_STATUS_SUCCESS) {
				return TEG_STATUS_ERROR;
			}
		}
	}

	return TEG_STATUS_SUCCESS;
}


/*
 * Libera los recursos de los dados
 */
static void free_dices(void)
{
	int i;

	for (i = 0; i < DICES_CANT; i++) {
		if (dices[i]) {
			g_object_unref(dices[i]);
			dices[i] = NULL;
		}
	}

	dices_initialized = 0;
}


/*
 * Callback cuando se destruye el widget
 */
static void on_dices_destroy(GtkWidget *widget, gpointer user_data)
{
	free_dices();
	dices_drawing_area = NULL;
}


/*
 * Inicializa el módulo de dados
 * En GTK3, creamos un GtkDrawingArea que se posiciona sobre el canvas principal
 */
void dices_init(GtkWidget *parent)
{
	int i;

	if (dices_initialized) {
		return;
	}

	if (!parent) {
		return;
	}

	/* Guardar offset del grupo desde el tema */
	group_offset_x = (double)gui_theme.dices_x;
	group_offset_y = (double)gui_theme.dices_y;

	/* Cargar imágenes de dados */
	if (dices_load() != TEG_STATUS_SUCCESS) {
		g_warning(_("Error loading dices images"));
		return;
	}

	/* Calcular posiciones de los dados */
	if (!theme_using_extended_dices()) {
		/* Comportamiento básico */
		double x = 0;
		int dice_width = dices[0] ? gdk_pixbuf_get_width(dices[0]) : 32;
		int dice_height = dices[0] ? gdk_pixbuf_get_height(dices[0]) : 32;

		for (i = 0; i < 3; i++) {
			dices_pos.attacker[i].x = x;
			dices_pos.attacker[i].y = 4;
			dices_pos.defender[i].x = x;
			dices_pos.defender[i].y = dice_height + 14;

			x += dice_width;
		}
		dices_pos.attacker_text.x = (double)(dice_width * 3) / 2;
		dices_pos.attacker_text.y = dices_pos.attacker[0].y - 10;
		dices_pos.defender_text.x = (double)(dice_width * 3) / 2;
		dices_pos.defender_text.y = dices_pos.defender[0].y - 10;
	} else {
		/* Características extendidas del tema */
		for (i = 0; i < 3; i++) {
			dices_pos.attacker[i].x = gui_theme.dices_ext_x[i];
			dices_pos.attacker[i].y = gui_theme.dices_ext_y[i];

			dices_pos.defender[i].x = gui_theme.dices_ext_x[i + 3];
			dices_pos.defender[i].y = gui_theme.dices_ext_y[i + 3];
		}

		dices_pos.attacker_text.x = gui_theme.dices_ext_text_x[0];
		dices_pos.attacker_text.y = gui_theme.dices_ext_text_y[0];
		dices_pos.defender_text.x = gui_theme.dices_ext_text_x[1];
		dices_pos.defender_text.y = gui_theme.dices_ext_text_y[1];
	}

	/* Crear el área de dibujo GTK3 */
	dices_drawing_area = gtk_drawing_area_new();

	/* Calcular tamaño necesario para el área de dados */
	{
		int dice_width = dices[0] ? gdk_pixbuf_get_width(dices[0]) : 32;
		int dice_height = dices[0] ? gdk_pixbuf_get_height(dices[0]) : 32;
		int area_width = dice_width * 3 + 20;
		int area_height = dice_height * 2 + 30;
		gtk_widget_set_size_request(dices_drawing_area, area_width, area_height);
	}

	/* Configurar la función de dibujo GTK3 */
	g_signal_connect(dices_drawing_area, "draw",
	                 G_CALLBACK(dices_draw_function), NULL);

	/* Conectar señal de destrucción */
	g_signal_connect(dices_drawing_area, "destroy",
	                 G_CALLBACK(on_dices_destroy), NULL);

	/* Inicialmente oculto */
	gtk_widget_set_visible(dices_drawing_area, FALSE);

	/* Inicializar estado */
	dices_state.visible = 0;
	dices_state.attacker_country = -1;
	dices_state.defender_country = -1;
	memset(dices_state.attacker_dices, 0, sizeof(dices_state.attacker_dices));
	memset(dices_state.defender_dices, 0, sizeof(dices_state.defender_dices));

	dices_initialized = 1;
}


/* ID del timeout para ocultar dados */
static guint dices_hide_timeout_id = 0;

/*
 * Callback del timeout para ocultar dados
 */
static gboolean dices_hide_timeout_cb(gpointer user_data)
{
	int i;

	/* Limpiar estado */
	dices_state.visible = 0;
	for (i = 0; i < 3; i++) {
		dices_state.attacker_dices[i] = 0;
		dices_state.defender_dices[i] = 0;
	}
	dices_state.attacker_country = -1;
	dices_state.defender_country = -1;

	/* Redibujar el mapa sin los dados */
	G_country_redraw();

	dices_hide_timeout_id = 0;
	return G_SOURCE_REMOVE;
}

/*
 * Oculta los dados (con delay de 2 segundos para que el usuario pueda verlos)
 */
void dices_unview(void)
{
	/* Cancelar timeout anterior si existe */
	if (dices_hide_timeout_id > 0) {
		g_source_remove(dices_hide_timeout_id);
		dices_hide_timeout_id = 0;
	}

	/* Si los dados están visibles, programar timeout para ocultarlos */
	if (dices_state.visible) {
		dices_hide_timeout_id = g_timeout_add(2000, dices_hide_timeout_cb, NULL);
	}
}


/*
 * Muestra los dados con los valores actuales de g_game
 */
void dices_view(void)
{
	int i;

	/* Cargar imágenes si no están cargadas */
	if (dices[0] == NULL) {
		dices_load_images();
	}

	if (g_game.dados_srccountry == -1 || g_game.dados_dstcountry == -1) {
		return;
	}

	/* Actualizar estado con los valores del juego */
	dices_state.attacker_country = g_game.dados_srccountry;
	dices_state.defender_country = g_game.dados_dstcountry;

	/* Copiar dados del atacante */
	for (i = 0; i < 3; i++) {
		dices_state.attacker_dices[i] = g_game.dados_src[i];
	}

	/* Copiar dados del defensor */
	for (i = 0; i < 3; i++) {
		dices_state.defender_dices[i] = g_game.dados_dst[i];
	}

	dices_state.visible = 1;

	/* Solicitar redibujado del mapa (los dados se dibujan en el mapa) */
	G_country_redraw();
}


/*
 * Obtiene el widget del área de dibujo de dados
 * Útil para integrarlo en el layout principal
 */
GtkWidget *dices_get_widget(void)
{
	return dices_drawing_area;
}


/*
 * Obtiene las coordenadas de offset del grupo de dados
 */
void dices_get_offset(double *x, double *y)
{
	if (x) *x = group_offset_x;
	if (y) *y = group_offset_y;
}


/*
 * Carga las imágenes de dados (función pública)
 */
void dices_load_images(void)
{
	if (dices[0] == NULL) {
		/* Cargar offset desde el tema */
		group_offset_x = (double)gui_theme.dices_x;
		group_offset_y = (double)gui_theme.dices_y;
		
		/* Cargar imágenes */
		dices_load();
		
		/* Inicializar posiciones */
		if (!theme_using_extended_dices()) {
			double x = 0;
			int dice_width = dices[0] ? gdk_pixbuf_get_width(dices[0]) : 32;
			int dice_height = dices[0] ? gdk_pixbuf_get_height(dices[0]) : 32;
			int i;

			for (i = 0; i < 3; i++) {
				dices_pos.attacker[i].x = x;
				dices_pos.attacker[i].y = 4;
				dices_pos.defender[i].x = x;
				dices_pos.defender[i].y = dice_height + 14;
				x += dice_width;
			}
			dices_pos.attacker_text.x = (double)(dice_width * 3) / 2;
			dices_pos.attacker_text.y = dices_pos.attacker[0].y - 10;
			dices_pos.defender_text.x = (double)(dice_width * 3) / 2;
			dices_pos.defender_text.y = dices_pos.defender[0].y - 10;
		} else {
			int i;
			for (i = 0; i < 3; i++) {
				dices_pos.attacker[i].x = gui_theme.dices_ext_x[i];
				dices_pos.attacker[i].y = gui_theme.dices_ext_y[i];
				dices_pos.defender[i].x = gui_theme.dices_ext_x[i + 3];
				dices_pos.defender[i].y = gui_theme.dices_ext_y[i + 3];
			}
			dices_pos.attacker_text.x = gui_theme.dices_ext_text_x[0];
			dices_pos.attacker_text.y = gui_theme.dices_ext_text_y[0];
			dices_pos.defender_text.x = gui_theme.dices_ext_text_x[1];
			dices_pos.defender_text.y = gui_theme.dices_ext_text_y[1];
		}
		
		dices_initialized = 1;
	}
}


/*
 * Dibuja los dados en un contexto cairo (para integrar en el mapa)
 */
void dices_draw_on_cairo(cairo_t *cr)
{
	int i;
	
	if (!dices_state.visible)
		return;
	
	/* Asegurar que las imágenes estén cargadas */
	if (dices[0] == NULL)
		dices_load_images();
	
	if (dices[0] == NULL)
		return;
	
	/* Guardar estado de cairo y aplicar offset */
	cairo_save(cr);
	cairo_translate(cr, group_offset_x, group_offset_y);
	
	/* Dibujar dados del atacante */
	for (i = 0; i < 3; i++) {
		int dice_value = dices_state.attacker_dices[i];
		if (dice_value > 0 && dice_value <= DICES_CANT && dices[dice_value - 1]) {
			gdk_cairo_set_source_pixbuf(cr, dices[dice_value - 1],
			                            dices_pos.attacker[i].x,
			                            dices_pos.attacker[i].y);
			cairo_paint(cr);
		}
	}
	
	/* Dibujar dados del defensor */
	for (i = 0; i < 3; i++) {
		int dice_value = dices_state.defender_dices[i];
		if (dice_value > 0 && dice_value <= DICES_CANT && dices[dice_value - 1]) {
			gdk_cairo_set_source_pixbuf(cr, dices[dice_value - 1],
			                            dices_pos.defender[i].x,
			                            dices_pos.defender[i].y);
			cairo_paint(cr);
		}
	}
	
	/* Dibujar texto de países */
	if (dices_state.attacker_country >= 0 && dices_state.defender_country >= 0) {
		PangoLayout *layout;
		PangoFontDescription *font_desc;
		GdkRGBA text_color;
		
		layout = pango_cairo_create_layout(cr);
		font_desc = pango_font_description_from_string(HELVETICA_10_FONT);
		pango_layout_set_font_description(layout, font_desc);
		
		gdk_rgba_parse(&text_color, gui_theme.dices_color ? gui_theme.dices_color : "black");
		gdk_cairo_set_source_rgba(cr, &text_color);
		
		/* Texto del atacante */
		pango_layout_set_text(layout, countries_get_name(dices_state.attacker_country), -1);
		cairo_move_to(cr, dices_pos.attacker_text.x, dices_pos.attacker_text.y);
		pango_cairo_show_layout(cr, layout);
		
		/* Texto del defensor */
		pango_layout_set_text(layout, countries_get_name(dices_state.defender_country), -1);
		cairo_move_to(cr, dices_pos.defender_text.x, dices_pos.defender_text.y);
		pango_cairo_show_layout(cr, layout);
		
		pango_font_description_free(font_desc);
		g_object_unref(layout);
	}
	
	cairo_restore(cr);
}

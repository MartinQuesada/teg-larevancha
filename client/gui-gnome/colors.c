/*	$Id: colors.c,v 1.16 2004/10/04 19:47:49 wfx Exp $	*/
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
 * @file colors.c
 * Helpers functions to manage the colors in the GUI
 * Migrado a GTK3 con GdkRGBA
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <string.h>
#include <libintl.h>

#define _(String) gettext(String)
#define N_(String) (String)

#include "gui.h"
#include "client.h"

#include "colors.h"
#include "support.h"
#include "interface.h"
#include "fonts.h"
#include "themes.h"

extern TTheme gui_theme;

static int allocated = 0;

/* Colores de los jugadores - ellip_color es el fondo, text_color es el texto */
struct _G_colores G_colores[] = {
	{"red", "white"},
	{"yellow", "black"},
	{"blue", "white"},
	{"grey27", "white"},
	{"PaleVioletRed1", "black"},
	{"green", "black"},
	{"grey", "black"},		/* Color no usado por jugadores */
};
#define NR_COLORS (sizeof(G_colores)/sizeof(G_colores[0]))

/* Colores parseados como GdkRGBA */
static GdkRGBA colors_players[NR_COLORS];

/* Colores comunes */
static char *G_colores_common[] = {
	"white", "black"
};
#define NR_COLORS_COMMON (2)
static GdkRGBA colors_common[NR_COLORS_COMMON];

/* Índice del color de texto para cada color de jugador */
static int colors_foreground[] = { 0, 1, 0, 0, 1, 1, 1 };

/* Imágenes de colores */
GdkPixbuf *g_color_players[TEG_MAX_PLAYERS];
GdkPixbuf *g_color_circles[TEG_MAX_PLAYERS + 1];
GdkPixbuf *g_color_circle_over = NULL;


/**
 * Carga las imágenes de colores de los jugadores
 */
TEG_STATUS colors_load_images(void)
{
	static int images_loaded = 0;
	char name[512];
	char *filename = NULL;
	int i;

	if (images_loaded)
		return TEG_STATUS_SUCCESS;

	for (i = 0; i < TEG_MAX_PLAYERS; i++) {
		memset(name, 0, sizeof(name));

		/* Load player colors */
		if (!gui_theme.choose_colors_custom) {
			snprintf(name, sizeof(name) - 1, "color_player_%s.png", g_colores[i]);
			filename = load_pixmap_file(name);
			g_color_players[i] = gdk_pixbuf_new_from_file(filename, NULL);
			if (filename)
				g_free(filename);
		} else {
			snprintf(name, sizeof(name) - 1, "%s_%s.png", 
			         gui_theme.choose_colors_prefix, g_colores[i]);
			filename = theme_load_file(name);
			g_color_players[i] = gdk_pixbuf_new_from_file(filename, NULL);
		}

		if (g_color_players[i] == NULL) {
			g_warning(_("Error, couldn't find file: %s"), name);
			return TEG_STATUS_ERROR;
		}

		/* Load color circles */
		snprintf(name, sizeof(name) - 1, "disc_%s.png", g_colores[i]);
		filename = load_pixmap_file(name);
		g_color_circles[i] = gdk_pixbuf_new_from_file(filename, NULL);
		if (filename)
			g_free(filename);

		if (g_color_circles[i] == NULL) {
			g_warning(_("Error, couldn't find file: %s"), name);
			return TEG_STATUS_ERROR;
		}
	}

	/* Disc for 'no color' */
	filename = load_pixmap_file("disc_grey.png");
	g_color_circles[TEG_MAX_PLAYERS] = gdk_pixbuf_new_from_file(filename, NULL);
	if (filename)
		g_free(filename);

	if (g_color_circles[TEG_MAX_PLAYERS] == NULL) {
		g_warning(_("Error, couldn't find file: %s"), "disc_grey.png");
		return TEG_STATUS_ERROR;
	}

	/* Disc over */
	filename = load_pixmap_file("disc_over.png");
	g_color_circle_over = gdk_pixbuf_new_from_file(filename, NULL);
	if (filename)
		g_free(filename);

	if (g_color_circle_over == NULL) {
		g_warning(_("Error, couldn't find file: %s"), "disc_over.png");
		return TEG_STATUS_ERROR;
	}

	images_loaded = 1;
	return TEG_STATUS_SUCCESS;
}


/**
 * Parsea y almacena los colores
 * En GTK3/4 usamos GdkRGBA en lugar de GdkColor
 */
TEG_STATUS colors_allocate(void)
{
	int i;

	if (allocated)
		return TEG_STATUS_SUCCESS;

	/* Parsear colores de jugadores */
	for (i = 0; i < (int)NR_COLORS; i++) {
		if (!gdk_rgba_parse(&colors_players[i], G_colores[i].ellip_color)) {
			g_warning("Could not parse color: %s", G_colores[i].ellip_color);
			/* Usar gris por defecto */
			gdk_rgba_parse(&colors_players[i], "grey");
		}
	}

	/* Parsear colores comunes */
	for (i = 0; i < NR_COLORS_COMMON; i++) {
		if (!gdk_rgba_parse(&colors_common[i], G_colores_common[i])) {
			g_warning("Could not parse color: %s", G_colores_common[i]);
			gdk_rgba_parse(&colors_common[i], "grey");
		}
	}

	/* Cargar imágenes */
	colors_load_images();

	allocated = 1;
	return TEG_STATUS_SUCCESS;
}


/**
 * Libera recursos de colores
 */
TEG_STATUS colors_free(void)
{
	int i;

	if (!allocated)
		return TEG_STATUS_ERROR;

	for (i = 0; i < TEG_MAX_PLAYERS; i++) {
		if (g_color_players[i]) {
			g_object_unref(g_color_players[i]);
			g_color_players[i] = NULL;
		}
	}

	for (i = 0; i <= TEG_MAX_PLAYERS; i++) {
		if (g_color_circles[i]) {
			g_object_unref(g_color_circles[i]);
			g_color_circles[i] = NULL;
		}
	}

	if (g_color_circle_over) {
		g_object_unref(g_color_circle_over);
		g_color_circle_over = NULL;
	}

	allocated = 0;
	return TEG_STATUS_SUCCESS;
}


/**
 * Obtiene el color de un jugador por su número
 */
GdkRGBA* colors_get_player(int n)
{
	PCPLAYER pJ;

	if (player_whois(n, &pJ) != TEG_STATUS_SUCCESS)
		return &colors_players[NR_COLORS - 1];

	if (pJ->color < 0 || pJ->color >= (int)NR_COLORS)
		return &colors_players[NR_COLORS - 1];

	return &colors_players[pJ->color];
}


/**
 * Obtiene el color directamente por índice de color
 */
GdkRGBA* colors_get_player_from_color(int color)
{
	if (color < 0 || color >= (int)NR_COLORS)
		return &colors_players[NR_COLORS - 1];
	return &colors_players[color];
}


/**
 * Obtiene el color de texto (tinta) para un jugador
 */
GdkRGBA* colors_get_player_ink(int n)
{
	PCPLAYER pJ;

	if (player_whois(n, &pJ) != TEG_STATUS_SUCCESS)
		return &colors_common[COLORS_BLACK];

	if (pJ->color < 0 || pJ->color >= (int)NR_COLORS)
		return &colors_common[COLORS_BLACK];

	return &colors_common[colors_foreground[pJ->color]];
}


/**
 * Obtiene el color de texto por índice de color
 */
GdkRGBA* colors_get_player_ink_from_color(int color)
{
	if (color < 0 || color >= (int)NR_COLORS)
		return &colors_common[COLORS_BLACK];
	return &colors_common[colors_foreground[color]];
}


/**
 * Obtiene color virtual (azul para mí, rojo para otros)
 */
GdkRGBA* colors_get_player_virtual(int n)
{
	if (n == g_game.numjug)
		return &colors_players[COLORS_P_BLUE];
	return &colors_players[COLORS_P_RED];
}


/**
 * Obtiene un color común
 */
GdkRGBA* colors_get_common(int n)
{
	if (n < 0 || n >= NR_COLORS_COMMON)
		return &colors_common[NR_COLORS_COMMON - 1];
	return &colors_common[n];
}


/**
 * Obtiene el nombre del tag para un color (para GtkTextBuffer)
 */
char* get_tag_for_color(int color)
{
	static char buffer[256];
	PCPLAYER pJ;

	strcpy(buffer, "default_tag");

	if (player_whois(color, &pJ) != TEG_STATUS_SUCCESS) {
		return buffer;
	}

	if (pJ->color < 0 || pJ->color >= (int)NR_COLORS)
		return buffer;

	snprintf(buffer, sizeof(buffer) - 1, "%s_tag", g_colores[pJ->color]);
	return buffer;
}


/**
 * Obtiene el color de texto para un índice de color
 */
char* get_foreground_for_color(int c)
{
	if (c < 0 || c >= (int)NR_COLORS)
		return G_colores[NR_COLORS - 1].text_color;
	return G_colores[c].text_color;
}


/**
 * Obtiene el color de fondo para un índice de color
 */
char* get_background_for_color(int c)
{
	if (c < 0 || c >= (int)NR_COLORS)
		return G_colores[NR_COLORS - 1].ellip_color;
	return G_colores[c].ellip_color;
}

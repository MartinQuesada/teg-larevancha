/*	$Id: g_country.c,v 1.3 2003/08/30 04:34:39 riq Exp $	*/
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
 * @file g_country.c
 * Sistema de países del mapa - Migrado a GTK3 con Cairo
 * 
 * En la versión original, cada país era un GnomeCanvasItem con eventos.
 * En esta versión, usamos un único GtkDrawingArea que dibuja todo el mapa
 * y manejamos los eventos con hit-testing basado en las imágenes de países.
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <string.h>
#include <math.h>

#include "gui.h"
#include "client.h"
#include "interface.h"
#include "g_country.h"
#include "g_cont.h"
#include "support.h"
#include "armies.h"
#include "colors.h"
#include "themes.h"
#include "fonts.h"
#include "priv.h"
#include "status.h"
#include "dices.h"
#include "locate_country.h"

/* Datos de los países */
struct _G_countries G_countries[COUNTRIES_CANT];

/* Widget del área de dibujo del mapa */
static GtkWidget *map_drawing_area = NULL;

/* Estado */
static int initialized = 0;
static int hovered_country = -1;  /* País bajo el cursor */

/* Imagen de fondo del mapa (board) */
static GdkPixbuf *map_background = NULL;

/* Función externa para actualizar la barra de estado */
extern void statusbar_set_message(const char *msg);


/**
 * Encuentra qué país está en las coordenadas (x, y)
 * Usa hit-testing basado en el alpha de los pixbufs
 */
static int find_country_at(double x, double y)
{
	int i;
	
	/* Buscar en orden inverso (los últimos dibujados están encima) */
	for (i = COUNTRIES_CANT - 1; i >= 0; i--) {
		GdkPixbuf *pixbuf = G_countries[i].pixbuf;
		if (!pixbuf)
			continue;

		/* Calcular posición absoluta del país */
		int cont = g_countries[i].continente;
		int abs_x = G_conts[cont].x + G_countries[i].x;
		int abs_y = G_conts[cont].y + G_countries[i].y;

		/* Verificar si está dentro del bounding box */
		if (x >= abs_x && x < abs_x + G_countries[i].x_len &&
		    y >= abs_y && y < abs_y + G_countries[i].y_len) {
			
			/* Verificar alpha del pixel */
			int px = (int)(x - abs_x);
			int py = (int)(y - abs_y);
			
			if (gdk_pixbuf_get_has_alpha(pixbuf)) {
				int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
				int n_channels = gdk_pixbuf_get_n_channels(pixbuf);
				guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);
				
				guchar *pixel = pixels + py * rowstride + px * n_channels;
				guchar alpha = pixel[3];  /* Canal alpha */
				
				if (alpha > 128) {  /* Pixel no transparente */
					return i;
				}
			} else {
				/* Sin canal alpha, el bounding box es suficiente */
				return i;
			}
		}
	}
	
	return -1;
}

/*
 * Convierte coordenadas de evento (pantalla/widget) a coordenadas lógicas del mapa.
 * El dibujo aplica canvas_map_zoom, así que el hit-testing debe deshacer ese factor.
 */
static void map_event_to_logical(double event_x, double event_y, double *map_x, double *map_y)
{
	double zoom = canvas_map_zoom;
	if (zoom <= 0.0)
		zoom = 1.0;

	*map_x = event_x / zoom;
	*map_y = event_y / zoom;
}


/**
 * Dibuja la elipse con el número de ejércitos de un país
 */
static void draw_country_armies(cairo_t *cr, int country)
{
	PCPLAYER j;
	int color_idx;
	double size, size_tmp;
	int x, y;
	char size_string[10];

	/* Obtener color del dueño */
	color_idx = TEG_MAX_PLAYERS;  /* Gris por defecto */
	if (player_whois(g_countries[country].numjug, &j) == TEG_STATUS_SUCCESS) {
		if (j->color >= 0 && j->color < TEG_MAX_PLAYERS)
			color_idx = j->color;
	}

	/* Calcular tamaño de la elipse según cantidad de ejércitos */
	switch (g_countries[country].ejercitos) {
		case 0:  size = 4;  size_tmp = 0; break;
		case 1:  size = 6;  size_tmp = 0; break;
		case -1:
		case 2:  size = 6;  size_tmp = 1; break;
		case 3:  size = 7;  size_tmp = 0; break;
		case 4:  size = 7;  size_tmp = 1; break;
		case 5:  size = 8;  size_tmp = 0; break;
		case 6:  size = 8;  size_tmp = 1; break;
		case 7:  size = 9;  size_tmp = 0; break;
		case 8:  size = 9;  size_tmp = 1; break;
		case 9:  size = 10; size_tmp = 0; break;
		case 10: size = 10; size_tmp = 1; break;
		default: size = 11; size_tmp = 0; break;
	}

	/* Calcular posición del centro */
	int cont = g_countries[country].continente;
	int base_x = G_conts[cont].x + G_countries[country].x;
	int base_y = G_conts[cont].y + G_countries[country].y;
	
	x = base_x + G_countries[country].x_len / 2 + G_countries[country].x_center;
	y = base_y + G_countries[country].y_len / 2 + G_countries[country].y_center;

	/* Determinar estilo del borde según selección */
	double line_width = 1.0;
	const char *outline_color = "black";
	
	if (g_countries[country].selected & COUNTRY_SELECT_FICHAS_IN) {
		outline_color = "white";
		line_width = 2.0;
	} else if (g_countries[country].selected & COUNTRY_SELECT_FICHAS_OUT) {
		outline_color = "DarkOrange1";
		line_width = 2.0;
	} else if (g_countries[country].selected & COUNTRY_SELECT_ATTACK_ENTER) {
		outline_color = "white";
		line_width = 2.0;
	} else if (g_countries[country].selected & COUNTRY_SELECT_REGROUP_ENTER) {
		outline_color = "white";
		line_width = 2.0;
	} else if (g_countries[country].selected & COUNTRY_SELECT_ATTACK_SRC) {
		outline_color = "white";
		line_width = 2.0;
	} else if (g_countries[country].selected & COUNTRY_SELECT_ATTACK_DST) {
		outline_color = "white";
		line_width = 2.0;
	}

	/* Dibujar elipse completa (relleno + borde) dentro del mismo contexto */
	GdkRGBA fill_color, border_color;
	gdk_rgba_parse(&fill_color, G_colores[color_idx].ellip_color);
	gdk_rgba_parse(&border_color, outline_color);
	
	cairo_save(cr);
	cairo_translate(cr, x, y);
	cairo_scale(cr, size + size_tmp, size);
	cairo_arc(cr, 0, 0, 1.0, 0, 2 * G_PI);
	cairo_restore(cr);
	
	/* Rellenar la elipse */
	gdk_cairo_set_source_rgba(cr, &fill_color);
	cairo_fill_preserve(cr);
	
	/* Dibujar borde - crear nuevo path para evitar líneas fantasma */
	cairo_new_path(cr);
	cairo_save(cr);
	cairo_translate(cr, x, y);
	cairo_scale(cr, size + size_tmp, size);
	cairo_arc(cr, 0, 0, 1.0, 0, 2 * G_PI);
	cairo_restore(cr);
	
	gdk_cairo_set_source_rgba(cr, &border_color);
	cairo_set_line_width(cr, line_width);
	cairo_stroke(cr);

	/* Dibujar número de ejércitos */
	if (g_countries[country].ejercitos != 0) {
		if (g_countries[country].ejercitos == -1)
			snprintf(size_string, sizeof(size_string), "?");
		else
			snprintf(size_string, sizeof(size_string), "%d", g_countries[country].ejercitos);

		/* Usar Pango para el texto */
		PangoLayout *layout = pango_cairo_create_layout(cr);
		PangoFontDescription *font_desc = pango_font_description_from_string(HELVETICA_10_FONT);
		pango_layout_set_font_description(layout, font_desc);
		pango_layout_set_text(layout, size_string, -1);
		
		int text_width, text_height;
		pango_layout_get_pixel_size(layout, &text_width, &text_height);
		
		GdkRGBA text_color;
		gdk_rgba_parse(&text_color, G_colores[color_idx].text_color);
		gdk_cairo_set_source_rgba(cr, &text_color);
		
		cairo_move_to(cr, x - text_width / 2.0, y - text_height / 2.0);
		pango_cairo_show_layout(cr, layout);
		
		pango_font_description_free(font_desc);
		g_object_unref(layout);
	}
}


/**
 * Función de dibujo del mapa - callback de GtkDrawingArea
 */
static gboolean map_draw_function(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
	int i;

	/* Aplicar zoom */
	cairo_scale(cr, canvas_map_zoom, canvas_map_zoom);

	/* Dibujar fondo del mapa */
	if (map_background) {
		gdk_cairo_set_source_pixbuf(cr, map_background, 0, 0);
		cairo_paint(cr);
	}

	/* Dibujar cada país */
	for (i = 0; i < COUNTRIES_CANT; i++) {
		if (!G_countries[i].pixbuf)
			continue;

		/* No dibujar si está parpadeando (oculto) */
		if (locate_country_is_blinking(i))
			continue;

		int cont = g_countries[i].continente;
		int x = G_conts[cont].x + G_countries[i].x;
		int y = G_conts[cont].y + G_countries[i].y;

		gdk_cairo_set_source_pixbuf(cr, G_countries[i].pixbuf, x, y);
		cairo_paint(cr);
	}

	/* Dibujar ejércitos de cada país */
	for (i = 0; i < COUNTRIES_CANT; i++) {
		if (G_countries[i].pixbuf) {
			draw_country_armies(cr, i);
		}
	}

	/* Destacar país bajo el cursor */
	if (hovered_country >= 0 && G_countries[hovered_country].pixbuf) {
		int cont = g_countries[hovered_country].continente;
		int x = G_conts[cont].x + G_countries[hovered_country].x;
		int y = G_conts[cont].y + G_countries[hovered_country].y;
		int w = G_countries[hovered_country].x_len;
		int h = G_countries[hovered_country].y_len;

		/* Dibujar borde de highlight */
		cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.3);
		cairo_rectangle(cr, x, y, w, h);
		cairo_stroke(cr);
	}

	/* Dibujar dados (si están visibles) */
	dices_draw_on_cairo(cr);

	return FALSE;
}


/**
 * Maneja el evento de movimiento del mouse
 */
static gboolean on_motion_notify(GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
	double map_x, map_y;
	map_event_to_logical(event->x, event->y, &map_x, &map_y);
	int new_hover = find_country_at(map_x, map_y);
	
	if (new_hover != hovered_country) {
		/* Salir del país anterior */
		if (hovered_country >= 0) {
			PCOUNTRY country = &g_countries[hovered_country];
			
			gui_private.country_i_am = -1;
			mainstatus_update();
			
			switch (ESTADO_GET()) {
			case PLAYER_STATUS_FICHAS:
			case PLAYER_STATUS_FICHAS2:
			case PLAYER_STATUS_FICHASC:
				fichas_leave(country);
				break;
			case PLAYER_STATUS_ATAQUE:
			case PLAYER_STATUS_TROPAS:
				attack_leave(country);
				break;
			case PLAYER_STATUS_REAGRUPE:
				reagrupe_leave(country);
				break;
			default:
				break;
			}
		}
		
		hovered_country = new_hover;
		
		/* Entrar al nuevo país */
		if (hovered_country >= 0) {
			PCOUNTRY country = &g_countries[hovered_country];
			char buff[250];
			
			if (country->ejercitos > 0)
				snprintf(buff, sizeof(buff) - 1, _("%s armies: %d"), 
				         countries_get_name(country->id), country->ejercitos);
			else
				snprintf(buff, sizeof(buff) - 1, "%s", countries_get_name(country->id));
			
			statusbar_set_message(buff);
			
			gui_private.country_i_am = country->id;
			mainstatus_update();
			
			switch (ESTADO_GET()) {
			case PLAYER_STATUS_FICHAS:
			case PLAYER_STATUS_FICHAS2:
			case PLAYER_STATUS_FICHASC:
				fichas_enter(country);
				break;
			case PLAYER_STATUS_ATAQUE:
			case PLAYER_STATUS_TROPAS:
				attack_enter(country);
				break;
			case PLAYER_STATUS_REAGRUPE:
				reagrupe_enter(country);
				break;
			default:
				break;
			}
		} else {
			statusbar_set_message("");
		}
		
		gtk_widget_queue_draw(widget);
	}
	
	return FALSE;
}


/**
 * Maneja el evento de click del mouse
 */
static gboolean on_button_press(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	double map_x, map_y;
	map_event_to_logical(event->x, event->y, &map_x, &map_y);
	int country_id = find_country_at(map_x, map_y);
	
	if (country_id < 0)
		return FALSE;
	
	PCOUNTRY country = &g_countries[country_id];
	
	switch (event->button) {
	case 1:  /* Click izquierdo */
		switch (ESTADO_GET()) {
		case PLAYER_STATUS_FICHAS:
		case PLAYER_STATUS_FICHAS2:
		case PLAYER_STATUS_FICHASC:
			if (fichas_add(country) == TEG_STATUS_SUCCESS) {
				G_country_draw_ejer(country->id);
				armies_add(country->id);
			}
			break;
		case PLAYER_STATUS_ATAQUE:
		case PLAYER_STATUS_TROPAS:
			attack_click(country);
			break;
		case PLAYER_STATUS_REAGRUPE:
			reagrupe_click(country);
			break;
		default:
			break;
		}
		break;
		
	case 3:  /* Click derecho */
		switch (ESTADO_GET()) {
		case PLAYER_STATUS_FICHAS:
		case PLAYER_STATUS_FICHAS2:
		case PLAYER_STATUS_FICHASC:
			if (fichas_sub(country) == TEG_STATUS_SUCCESS) {
				G_country_draw_ejer(country->id);
				armies_del(country->id);
			}
			break;
		default:
			break;
		}
		break;
		
	default:
		break;
	}
	
	gtk_widget_queue_draw(widget);
	return FALSE;
}


/**
 * Maneja cuando el mouse sale del widget
 */
static gboolean on_leave_notify(GtkWidget *widget, GdkEventCrossing *event, gpointer data)
{
	if (hovered_country >= 0) {
		PCOUNTRY country = &g_countries[hovered_country];
		
		switch (ESTADO_GET()) {
		case PLAYER_STATUS_FICHAS:
		case PLAYER_STATUS_FICHAS2:
		case PLAYER_STATUS_FICHASC:
			fichas_leave(country);
			break;
		case PLAYER_STATUS_ATAQUE:
		case PLAYER_STATUS_TROPAS:
			attack_leave(country);
			break;
		case PLAYER_STATUS_REAGRUPE:
			reagrupe_leave(country);
			break;
		default:
			break;
		}
		
		hovered_country = -1;
		gui_private.country_i_am = -1;
		mainstatus_update();
		statusbar_set_message("");
		gtk_widget_queue_draw(widget);
	}
	
	return FALSE;
}


/**
 * Inicializa los datos de países desde el tema
 */
void G_country_init(void)
{
	int i;
	TCountry country;

	if (initialized)
		return;

	/* Inicializar continentes primero */
	G_cont_init();

	for (i = 0; i < COUNTRIES_CANT; i++) {
		/* Encontrar continente y número de país dentro del continente */
		int c = g_countries[i].continente;
		int sum = 0, j, p;

		for (j = 0; j < c; j++)
			sum += g_conts[j].cant_countries;

		p = i - sum;

		if (theme_giveme_country(&country, c, p) == TEG_STATUS_SUCCESS) {
			G_countries[i].x = country.pos_x;
			G_countries[i].y = country.pos_y;
			G_countries[i].x_center = country.army_x;
			G_countries[i].y_center = country.army_y;
			G_countries[i].gfx_name = country.file;
		}
		
		G_countries[i].pixbuf = NULL;
		G_countries[i].x_len = 0;
		G_countries[i].y_len = 0;
	}

	initialized = 1;
}


/**
 * Crea y carga los gráficos de un país
 */
void G_country_create(int country)
{
	if (!initialized)
		G_country_init();
	
	/* En GTK3/Cairo no creamos items de canvas,
	 * solo nos aseguramos que los datos estén listos */
}


/**
 * Carga la imagen de un país
 */
void G_country_draw(int country)
{
	char name[200];
	char *filename = NULL;

	if (country < 0 || country >= COUNTRIES_CANT)
		return;

	snprintf(name, sizeof(name) - 1, "%s", G_countries[country].gfx_name);
	name[sizeof(name) - 1] = 0;

	filename = theme_load_file(name);

	/* Liberar imagen anterior si existe */
	if (G_countries[country].pixbuf) {
		g_object_unref(G_countries[country].pixbuf);
		G_countries[country].pixbuf = NULL;
	}

	G_countries[country].pixbuf = gdk_pixbuf_new_from_file(filename, NULL);

	if (G_countries[country].pixbuf == NULL) {
		textmsg(M_ERR, _("Error, couldn't find file: %s"), G_countries[country].gfx_name);
		g_warning("Could not find country %s file", G_countries[country].gfx_name);
	} else {
		G_countries[country].x_len = gdk_pixbuf_get_width(G_countries[country].pixbuf);
		G_countries[country].y_len = gdk_pixbuf_get_height(G_countries[country].pixbuf);
	}
}


/**
 * Actualiza el dibujo de ejércitos de un país
 * En GTK3/Cairo solo solicitamos un redibujado
 */
void G_country_draw_ejer(int country)
{
	if (country < 0 || country >= COUNTRIES_CANT)
		return;

	/* Actualizar tamaño según el zoom */
	if (map_drawing_area && map_background) {
		int w = (int)(gdk_pixbuf_get_width(map_background) * canvas_map_zoom);
		int h = (int)(gdk_pixbuf_get_height(map_background) * canvas_map_zoom);
		gtk_widget_set_size_request(map_drawing_area, w, h);
	}

	/* Solicitar redibujado del mapa */
	if (map_drawing_area) {
		gtk_widget_queue_draw(map_drawing_area);
	}
}


/**
 * Actualiza el estilo de selección de un país
 */
TEG_STATUS gui_country_select(int country)
{
	if (country < 0 || country >= COUNTRIES_CANT)
		return TEG_STATUS_ERROR;

	/* Solicitar redibujado para actualizar el estilo */
	if (map_drawing_area) {
		gtk_widget_queue_draw(map_drawing_area);
	}

	return TEG_STATUS_SUCCESS;
}


/* Variables para auto-ajuste del mapa */
static int map_original_width = 0;
static int map_original_height = 0;
static gboolean auto_fit_enabled = TRUE;

/**
 * Callback para ajustar zoom cuando cambia el tamaño del contenedor
 */
static void on_size_allocate(GtkWidget *widget, GdkRectangle *allocation, gpointer user_data)
{
	if (!auto_fit_enabled || !map_background)
		return;

	if (map_original_width == 0 || map_original_height == 0)
		return;

	/* Calcular el zoom para ajustar al área disponible */
	double zoom_x = (double)allocation->width / map_original_width;
	double zoom_y = (double)allocation->height / map_original_height;
	
	/* Usar el menor para mantener proporción */
	double new_zoom = (zoom_x < zoom_y) ? zoom_x : zoom_y;
	
	/* Solo actualizar si cambió significativamente */
	if (fabs(new_zoom - canvas_map_zoom) > 0.01) {
		canvas_map_zoom = new_zoom;
		gtk_widget_queue_draw(map_drawing_area);
	}
}

/**
 * Habilitar/deshabilitar ajuste automático
 */
void G_country_set_auto_fit(gboolean enabled)
{
	auto_fit_enabled = enabled;
}

/**
 * Crea el widget del mapa
 */
GtkWidget* G_country_create_map_widget(void)
{
	extern TTheme gui_theme;
	char *filename;

	if (!initialized)
		G_country_init();

	/* Crear área de dibujo */
	map_drawing_area = gtk_drawing_area_new();
	
	/* Cargar imagen de fondo */
	filename = theme_load_file(gui_theme.board);
	if (filename) {
		map_background = gdk_pixbuf_new_from_file(filename, NULL);
		if (map_background) {
			map_original_width = gdk_pixbuf_get_width(map_background);
			map_original_height = gdk_pixbuf_get_height(map_background);
			
			/* Establecer tamaño mínimo */
			gtk_widget_set_size_request(map_drawing_area, 400, 300);
		}
	}

	/* Permitir que el widget se expanda */
	gtk_widget_set_hexpand(map_drawing_area, TRUE);
	gtk_widget_set_vexpand(map_drawing_area, TRUE);

	/* Conectar señales */
	g_signal_connect(map_drawing_area, "draw", G_CALLBACK(map_draw_function), NULL);
	g_signal_connect(map_drawing_area, "size-allocate", G_CALLBACK(on_size_allocate), NULL);
	
	/* Habilitar eventos de mouse - todos los necesarios */
	gtk_widget_add_events(map_drawing_area,
	                      GDK_POINTER_MOTION_MASK |
	                      GDK_POINTER_MOTION_HINT_MASK |
	                      GDK_BUTTON_PRESS_MASK |
	                      GDK_BUTTON_RELEASE_MASK |
	                      GDK_ENTER_NOTIFY_MASK |
	                      GDK_LEAVE_NOTIFY_MASK);
	
	g_signal_connect(map_drawing_area, "motion-notify-event", 
	                 G_CALLBACK(on_motion_notify), NULL);
	g_signal_connect(map_drawing_area, "button-press-event",
	                 G_CALLBACK(on_button_press), NULL);
	g_signal_connect(map_drawing_area, "leave-notify-event",
	                 G_CALLBACK(on_leave_notify), NULL);
	
	/* El widget puede recibir focus para eventos */
	gtk_widget_set_can_focus(map_drawing_area, TRUE);

	return map_drawing_area;
}


/**
 * Obtiene el widget del mapa
 */
GtkWidget* G_country_get_map_widget(void)
{
	return map_drawing_area;
}


/**
 * Solicita redibujado del mapa completo
 */
void G_country_redraw(void)
{
	if (map_drawing_area) {
		gtk_widget_queue_draw(map_drawing_area);
	}
}


/**
 * Inicialización del módulo
 */
TEG_STATUS g_country_init(void)
{
	G_country_init();
	return TEG_STATUS_SUCCESS;
}

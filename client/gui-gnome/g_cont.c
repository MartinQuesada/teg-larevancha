/*	$Id: g_cont.c,v 1.9 2002/08/31 17:45:58 riq Exp $	*/
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
 * @file g_cont.c
 * Manejo de continentes - Migrado a GTK3/Cairo
 * 
 * En la versión original, cada continente era un GnomeCanvasGroup.
 * En la versión migrada, los continentes son solo datos de posición
 * que se usan al dibujar el mapa con Cairo.
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>

#include "gui.h"
#include "client.h"
#include "interface.h"
#include "g_country.h"
#include "g_cont.h"
#include "themes.h"

static int initialized = 0;

/* Datos de los continentes */
struct _G_conts G_conts[CONT_CANT];

/**
 * Inicializa los datos de continentes desde el tema
 */
void G_cont_init(void)
{
	int i;
	TContinent cont;

	if (initialized)
		return;

	for (i = 0; i < CONT_CANT; i++) {
		if (theme_giveme_continent(&cont, i) == TEG_STATUS_SUCCESS) {
			G_conts[i].x = cont.pos_x;
			G_conts[i].y = cont.pos_y;
		} else {
			G_conts[i].x = 0;
			G_conts[i].y = 0;
		}
	}

	initialized = 1;
}

/**
 * En GTK3/Cairo ya no creamos grupos de canvas.
 * Esta función existe para compatibilidad pero solo inicializa los datos.
 */
void G_cont_create(int cont)
{
	if (!initialized)
		G_cont_init();
	
	/* En GTK3/Cairo no hay grupos de canvas,
	 * los continentes son solo offsets de posición */
}

/**
 * Obtiene la posición X de un continente
 */
int G_cont_get_x(int cont)
{
	if (cont < 0 || cont >= CONT_CANT)
		return 0;
	return G_conts[cont].x;
}

/**
 * Obtiene la posición Y de un continente
 */
int G_cont_get_y(int cont)
{
	if (cont < 0 || cont >= CONT_CANT)
		return 0;
	return G_conts[cont].y;
}

/**
 * Retorna el número total de continentes
 */
int G_cont_tot(void)
{
	return CONT_CANT;
}

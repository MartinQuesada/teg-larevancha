/*	$Id: g_cont.h,v 1.5 2001/09/03 00:55:40 riq Exp $	*/
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
 * @file g_cont.h
 * Manejo de continentes - Migrado a GTK3/Cairo
 */

#ifndef __TEG_GUI_GNOME_GCONT_H
#define __TEG_GUI_GNOME_GCONT_H

#include <gtk/gtk.h>
#include "cont.h"  /* Para CONT_CANT */

/**
 * Estructura de datos de continente para la GUI
 * En GTK3/Cairo ya no usamos GnomeCanvasGroup
 */
struct _G_conts {
	int x;  /**< Posición X del continente */
	int y;  /**< Posición Y del continente */
};

extern struct _G_conts G_conts[];

/* Funciones */
void G_cont_init(void);
void G_cont_create(int cont);
int G_cont_get_x(int cont);
int G_cont_get_y(int cont);
int G_cont_tot(void);

#endif /* __TEG_GUI_GNOME_GCONT_H */

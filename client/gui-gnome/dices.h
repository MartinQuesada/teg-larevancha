/*	$Id: dices.h,v 1.5 2001/09/03 00:55:40 riq Exp $	*/
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
 * @file dices.h
 */
#ifndef __TEG_GUI_GNOME_DICES_H
#define __TEG_GUI_GNOME_DICES_H

void dices_view(void);
void dices_unview(void);
void dices_init(GtkWidget *parent);

/* Nuevas funciones GTK3 */
GtkWidget *dices_get_widget(void);
void dices_get_offset(double *x, double *y);

/* Función para dibujar dados en un cairo_t (para integrar en el mapa) */
void dices_draw_on_cairo(cairo_t *cr);

/* Función para cargar las imágenes de dados (llamar antes de usarlos) */
void dices_load_images(void);

#endif 

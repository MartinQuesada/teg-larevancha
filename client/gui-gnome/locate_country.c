/*	$Id: locate_country.c,v 1.4 2002/08/31 17:45:59 riq Exp $	*/
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
/* Highlights a country in the map - GTK3 version */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "gnome-compat.h"
#include <assert.h>

#include "gui.h"
#include "client.h"

#include "locate_country.h"
#include "g_country.h"

static guint timeout_id = 0;

struct _struct_countries {
	int country;
	int is_hidden;
	int number_times_refreshed;
};

#define MAX_LOCATE_COUNTRIES (16)
static struct _struct_countries list_locate_countries[MAX_LOCATE_COUNTRIES];
static struct _struct_countries list_locate_armies[MAX_LOCATE_COUNTRIES];

#define MAX_REFRESHES_COUNTRY (5)
#define MAX_REFRESHES_ARMY (7)

/* Array to track which countries are currently blinking */
static int blinking_countries[COUNTRIES_CANT];

static void locate_country_init_entry(struct _struct_countries *s)
{
	s->country = -1;
	s->is_hidden = FALSE;
	s->number_times_refreshed = 0;
}

/* Check if a country is currently in blink (hidden) state */
int locate_country_is_blinking(int country)
{
	int i;
	for (i = 0; i < MAX_LOCATE_COUNTRIES; i++) {
		if (list_locate_countries[i].country == country && 
		    list_locate_countries[i].is_hidden) {
			return 1;
		}
	}
	return 0;
}

static gboolean timeout_cb(gpointer data)
{
	int i;
	int need_redraw = 0;

	/* Iterate over the country's list */
	for (i = 0; i < MAX_LOCATE_COUNTRIES; i++) {
		if (list_locate_countries[i].country == -1)
			continue;

		need_redraw = 1;

		if (list_locate_countries[i].is_hidden) {
			list_locate_countries[i].is_hidden = FALSE;

			if (++list_locate_countries[i].number_times_refreshed >= MAX_REFRESHES_COUNTRY) {
				locate_country_init_entry(&list_locate_countries[i]);
			}
		} else {
			list_locate_countries[i].is_hidden = TRUE;
		}
	}

	/* Iterate over the army's list */
	for (i = 0; i < MAX_LOCATE_COUNTRIES; i++) {
		if (list_locate_armies[i].country == -1)
			continue;

		need_redraw = 1;

		if (list_locate_armies[i].is_hidden) {
			list_locate_armies[i].is_hidden = FALSE;

			if (++list_locate_armies[i].number_times_refreshed >= MAX_REFRESHES_ARMY) {
				locate_country_init_entry(&list_locate_armies[i]);
			}
		} else {
			list_locate_armies[i].is_hidden = TRUE;
		}
	}

	/* Redraw the map if anything changed */
	if (need_redraw) {
		G_country_redraw();
	}

	return TRUE;
}

/* Constructor */
TEG_STATUS locate_country_init()
{
	int i;

	if (timeout_id == 0) {
		timeout_id = g_timeout_add(300, timeout_cb, NULL);
	}

	for (i = 0; i < MAX_LOCATE_COUNTRIES; i++) {
		list_locate_countries[i].country = -1;
		list_locate_armies[i].country = -1;
	}

	for (i = 0; i < COUNTRIES_CANT; i++) {
		blinking_countries[i] = 0;
	}

	return TEG_STATUS_SUCCESS;
}

/* Destructor */
TEG_STATUS locate_country_exit()
{
	if (timeout_id > 0) {
		g_source_remove(timeout_id);
		timeout_id = 0;
	}

	return TEG_STATUS_SUCCESS;
}

/* Add a country to the list of countries to be highlighted */
TEG_STATUS locate_country_add_country(PCOUNTRY p)
{
	int i;

	if (!p)
		return TEG_STATUS_ERROR;

	for (i = 0; i < MAX_LOCATE_COUNTRIES; i++) {
		if (list_locate_countries[i].country == -1) {
			locate_country_init_entry(&list_locate_countries[i]);
			list_locate_countries[i].country = p->id;
			textmsg(M_INF, _("Locating country: %s"), countries_get_name(p->id));
			break;
		}

		if (list_locate_countries[i].country == p->id) {
			list_locate_countries[i].number_times_refreshed = 0;
			break;
		}
	}

	return TEG_STATUS_SUCCESS;
}

/* Add an army to the list of armies to be highlighted */
TEG_STATUS locate_country_add_army(int country)
{
	int i;

	if (country < 0 || country >= COUNTRIES_CANT)
		return TEG_STATUS_ERROR;

	for (i = 0; i < MAX_LOCATE_COUNTRIES; i++) {
		if (list_locate_armies[i].country == -1) {
			locate_country_init_entry(&list_locate_armies[i]);
			list_locate_armies[i].country = country;
			break;
		}

		if (list_locate_armies[i].country == country) {
			list_locate_armies[i].number_times_refreshed = 0;
			break;
		}
	}

	return TEG_STATUS_SUCCESS;
}

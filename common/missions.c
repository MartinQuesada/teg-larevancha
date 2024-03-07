/*	$Id: missions.c,v 1.3 2002/09/09 03:52:06 riq Exp $	*/
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
 * @file missions.c
 */

#include <stdio.h>
#include <string.h>

#include "all.h"

MISSIONS g_missions[]= {
	{ 0,	/* Objetivo cuando se juega sin missions */
		N_("Conquer the world"),
		{CONT_AMERICASUR_CANT, CONT_AMERICANORTE_CANT, CONT_AFRICA_CANT, CONT_OCEANIA_CANT, CONT_EUROPA_CANT, CONT_ASIA_CANT, CONT_AMERICACENTRO_CANT},
		{0, 0, 0, 0, 0, 0, 0},
		0,
		0 },
	{ 0,	/* Objetivo comun */
		N_("Conquer 45 countries"),
		{0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0},
		0,
		45 },

	{ -1,
		N_("Conquer &4, and\n &0"),
		{CONT_AMERICASUR_CANT, 0, 0, 0, CONT_EUROPA_CANT, 0, 0},
		{0, 0, 0, 0, 0, 0, 0},
		0,
		0 },

#ifdef WITH_ALL_MISSIONS
	/*
	 * Estoy viendo como implementarlo.
	 * Por ahora se me ocurre definir un continente bobo que tenga a las 13 islas del mapa,
	 * pero faltaría el chequeo de al menos 3 continentes.
	 * Australia no cuenta como isla.
	 */
	{ -1,
		N_("Conquer &2\n, 4 countries of &4, 4 countries de &5 and\n6 islands distributed in\nat least 3 continents"),
		{0, 0, CONT_AFRICA_CANT, 0, 4, 4, 0},
		{0, 0, 0, 0, 0, 0, 0},
		7,
		0},
#endif /* WITH_ALL_MISSIONS */

	{ -1,
		N_("Conquer &1,\n&3 and\n5 countries of &2"),
		{0, CONT_AMERICANORTE_CANT, 5, CONT_OCEANIA_CANT, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0},
		0,
		0},

	{ -1,
		N_("Conquer &5 and\n&6"),
		{0, 0, 0, 0, 0, CONT_ASIA_CANT, CONT_AMERICACENTRO_CANT},
		{0, 0, 0, 0, 0, 0, 0},
		0,
		0},

	{ -1,
		N_("Conquer &1,\n4 countries of &4 and\n8 countries of &5"),
		{0, CONT_AMERICANORTE_CANT, 0, 0, 4, 8, 0},
		{0, 0, 0, 0, 0, 0, 0},
		0,
		0},

	{ -1,
		N_("Conquer 3 countries of &3,\n3 of &2, 3 of &0,\n4 of &4, 4 of &1,\n4 of &5 and\n3 of &6"),
		{3, 4, 3, 3, 4, 4, 3},
		{0, 0, 0, 0, 0, 0, 0},
		0,
		0},

	{ -1,
		N_("Conquer &3,\n6 countries of &1,\n6 countries of &5 and\n6 countries of &2"),
		{0, 6, 6, CONT_OCEANIA_CANT, 0, 6, 0},
		{0, 0, 0, 0, 0, 0, 0},
		0,
		0},

	{ -1,
		N_("Conquer &6,\n6 countries of &0,\n6 countries of &4 and\n6 countries of &5"),
		{6, 0, 0, 0, 6, 6, CONT_AMERICACENTRO_CANT},
		{0, 0, 0, 0, 0, 0, 0},
		0,
		0},

	{ -1,
		N_("Conquer &0,\n&2 and,\n8 countries of &5"),
		{CONT_AMERICASUR_CANT, 0, CONT_AFRICA_CANT, 0, 8, 0},
		{0, 0, 0, 0, 0, 0, 0},
		0,
		0},

	{ -1,
		N_("Conquer &2,\n&3,\n4 countries of &5 and\n4 countries of &6"),
		{0, 0, CONT_AFRICA_CANT, CONT_OCEANIA_CANT, 0, 4, 4},
		{0, 0, 0, 0, 0, 0, 0},
		0,
		0},

	{ -1,
		N_("Conquer &4,\n4 countries of &0 and\n4 countries of &5"),
		{4, 0, 0, 0, CONT_EUROPA_CANT, 4, 0},
		{0, 0, 0, 0, 0, 0, 0},
		0,
		0},

	{ -1,
		N_("Conquer 35 countries\nin any place of the map"),
		{0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0},
		0,
		35},

	{ -1,
		N_("Destroy the red army.\nIf impossible destroy the\nplayer at your right."),
		{0, 0, 0, 0, 0, 0, 0},
		{1, 0, 0, 0, 0, 0, 0},
		0,
		0},

	{ -1,
		N_("Destroy the yellow army;\nif impossible destroy the\nplayer at your right."),
		{0, 0, 0, 0, 0, 0, 0},
		{0, 1, 0, 0, 0, 0, 0},
		0,
		0},

	{ -1,
		N_("Destroy the blue army;\nif impossible destroy the\nplayer at your right."),
		{0, 0, 0, 0, 0, 0, 0},
		{0, 0, 1, 0, 0, 0, 0},
		0,
		0},

	{ -1,
		N_("Destroy the black army;\nif impossible destroy the\nplayer at your right."),
		{0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 1, 0, 0, 0},
		0,
		0},

	{ -1,
		N_("Destroy the pink army;\nif impossible destroy the\nplayer at your right."),
		{0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 1, 0, 0},
		0,
		0},

	{ -1,
		N_("Destroy the green army;\nif impossible destroy the\nplayer at your right."),
		{0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 1, 0},
		0,
		0},

	{ -1,
		N_("Destroy the player\nat your left"),
		{0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 1},
		0,
		0},
};
#define NRMISSIONS (sizeof(g_missions) /sizeof(g_missions[0]))

int missions_cant()
{
	return NRMISSIONS;
}

/* WARNING: it returns a pointer static buffer */
char * missions_get_name( int number )
{
	static char *_unknown = N_("Unknown");
	static char mission[1024];
	char buf_tmp[1024];
	char old_buffer[1024];
	int i,len;
	char *tmp_ptr;

	if( number < 0 || number >= NRMISSIONS )
		return _(_unknown);

	snprintf( buf_tmp, sizeof(buf_tmp) -1, "%s", _( g_missions[number].name ) );

	/* find &0 - &6 and replace for the correct name */
	len = strlen(buf_tmp);

	memset( mission, 0,  sizeof(mission) );

	tmp_ptr = &buf_tmp[0];

	for(i=0;i<len; )
	{
		char *ptr;

		ptr = strchr( tmp_ptr, '&' );

		if ( ptr ) {
			if( strlen(ptr)>1 && ptr[1] >= '0' && ptr[1] <= '6' ) {
				int real_n;
				char n[4];

				/* XXX: yuck, this is ugly */
				n[0] = ptr[1];
				n[1] = 0;

				real_n = atoi( &n[0] );
				ptr[0] = 0;
				strncpy( old_buffer, mission, sizeof(old_buffer) -1 );
				snprintf( mission, sizeof(mission) -1,  "%s%s%s", old_buffer, tmp_ptr, _(g_conts[real_n].name) );

				tmp_ptr = &ptr[2];
			} else {
				snprintf( mission, sizeof(mission) -1,  "Error: Unknown secret mission.\nPlease check translations!" );
				break;
			}
		} else {
			strncpy( old_buffer, mission, sizeof(old_buffer) -1 );
			snprintf( mission, sizeof(mission) -1,  "%s%s", old_buffer, tmp_ptr );
			break;
		}
	}

	return mission;
}

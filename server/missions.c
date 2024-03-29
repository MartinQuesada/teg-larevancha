/*	$Id: missions.c,v 1.3 2004/08/04 13:03:07 riq Exp $	*/
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
/* checks that the secret missions are accomplished */

#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "server.h"

/* Do player pJ accomplished his secret mission ? */
TEG_STATUS mission_chequear( PSPLAYER pJ )
{
	int i;
	int countries[CONT_CANT];
	
	/********************************************************/

	assert( pJ );

	if( pJ->mission == -1 ) {
		if( mission_asignar( pJ ) != TEG_STATUS_SUCCESS )
			return TEG_STATUS_ERROR;
	}


	memset( countries, 0, sizeof(countries) );

	player_listar_countries( pJ, countries );

	/* 1st part. Check that the player has the correct number of countries */
	if( g_game.cmission && pJ->mission != MISSION_CONQWORLD ) {
		if( pJ->tot_countries >= g_missions[MISSION_COMMON].tot_countries ) {
			pJ->mission = MISSION_COMMON;
			goto gano;
		}
	};

	/* 2da parte. Chequear countries por contienente */
	for(i=0;i<CONT_CANT;i++) {
		if( countries[i] < g_missions[pJ->mission].continentes[i] && g_missions[pJ->mission].continentes[i] > 0 )
			goto no_gano;
	}

	/* TODO: 3ra parte. Chequear si vencio a los otros players */
	int f,g,h;
	int total_countries=0;
	for(g=0;g<TEG_MAX_PLAYERS+1;g++)
	{
		if( g_missions[pJ->mission].jugadores[g] )
		{
			/* Determino el numero de jugador a destruir. Si tiene que destruir al de la izquierda no entro la función */
			if ( g != 6 ) {
				player_verificardestruir( g, pJ, &f );
			} else {
				f=pJ->numjug-1;
				if ( f < 0 )
					f = g_game.players-1;
			}

			//fprintf(stderr,_("Cantidad de jugadores: %i, Jugador: %i, Objetivo: destruir al color %s(%i), jugador numero %i\n"),g_game.players,pJ->numjug,g_colores[g],g,f);
			for(h=0;h<COUNTRIES_CANT;h++) {
				if(g_countries[h].numjug == f) {
					total_countries++;
				}
			}
			
			//g_missions[pJ->mission].jugadores[g]=f;
			
			//fprintf(stderr,_("Paises por destruir: %i\n"), total_countries);
			
			if( total_countries > 0 ){
				goto no_gano;
			}
		}
	}

	/* TODO: 4ta parte. Chequear si tiene los countries limitrofes que se piden */
	if( (i=g_missions[pJ->mission].limitrofes) ) {
		int j,k,i_tmp;
		int salir=0;
		for(j=0;j<COUNTRIES_CANT && salir==0;j++) {
			if( g_countries[j].numjug != pJ->numjug )
				continue;
			i_tmp=0;

			for(k=0;k<COUNTRIES_CANT;k++) {
				if( g_countries[k].numjug != pJ->numjug )
					continue;
				if( countries_eslimitrofe(j,k) ) {
					if( ++i_tmp >= i ) {
						/* mission cumplido, salir */
						salir=1;
						break;
					}
				}

			}
		}
		if( j>=COUNTRIES_CANT)
			goto no_gano;
	}

	/* 5ta parte. Chequear numero correcto de countries para objetivo no comun */
	if( pJ->tot_countries < g_missions[pJ->mission].tot_countries ) {
		goto no_gano;
	}

	/* tiene todo, entonces gano! */
gano:

	return TEG_STATUS_GAMEOVER;

no_gano:
	return TEG_STATUS_ERROR;
}

/* Assigns a secret mission to player pJ */
TEG_STATUS mission_asignar( PSPLAYER pJ )
{
	int i,obj;

	assert( pJ );

	/* tiene ya un mission asignado ? */
	if( pJ->mission != -1 )
		return TEG_STATUS_SUCCESS;

	/* Si se juega sin missions, es a conquistar el mundo */
	if( g_game.mission == FALSE ) {
		pJ->mission = MISSION_CONQWORLD;	/* mission de conquistar al mundo */
		return TEG_STATUS_SUCCESS;
	}

	obj = RANDOM_MAX(0,missions_cant()-1);

	i = obj;

	for( ; i < missions_cant() ; i++) {
		if( g_missions[i].numjug == -1 ) {
			g_missions[i].numjug = pJ->numjug;
			pJ->mission = i;
			fprintf(stderr, _("* Objetivos - Jugador: %s, Objetivo: %s\n"), pJ->name, g_missions[i].name);
			return TEG_STATUS_SUCCESS;
		}
	}
	for( i=0; i < obj ; i++ ) {
		if( g_missions[i].numjug == -1 ) {
			g_missions[i].numjug = pJ->numjug;
			pJ->mission = i;
			return TEG_STATUS_SUCCESS;
		}
	}

	return TEG_STATUS_ERROR;
}

/* Initialize the secret missions */
TEG_STATUS mission_init()
{
	int i;

	for(i=0;i<missions_cant();i++) {
		g_missions[i].numjug = -1;
	}
	g_missions[MISSION_CONQWORLD].numjug = 0;
	g_missions[MISSION_COMMON].numjug = 0;

	return TEG_STATUS_SUCCESS;
}

/* sets the option: play to conquer the world, or with secret missions */
TEG_STATUS mission_set( int a )
{
	if( JUEGO_EMPEZADO )
		return TEG_STATUS_ERROR;

	if( a!=0) a=1;
	g_game.mission = a;
	return TEG_STATUS_SUCCESS;
}

/* Enables/Disables playing with common secret mission */
TEG_STATUS mission_common_mission( int a )
{
	if( JUEGO_EMPEZADO )
		return TEG_STATUS_ERROR;

	if( a!=0) a=1;
	g_game.cmission= a;
	return TEG_STATUS_SUCCESS;
}

/* Recibe un país y un número de jugador y devuelve verdadero si ese país es parte del objetivo del jugador  */
BOOLEAN country_inmission( PCOUNTRY pP, int numjug )
{
	int i;
	PSPLAYER pJ;
	
	for (i=0; i<missions_cant(); i++)
	{
		// Misión común o conquistar el mundo
		if (g_missions[MISSION_COMMON].numjug == numjug || g_missions[MISSION_CONQWORLD].numjug == numjug)
			return TRUE;
		
		// Objetivo de cant. de países
		if (g_missions[i].numjug == numjug && g_missions[i].tot_countries != 0)
			return TRUE;
					
		// Misión de destrucción
		player_whois(pP->numjug, pJ);
		if (g_missions[i].numjug == numjug && g_missions[i].jugadores[pJ->color] == 1)
			return TRUE;
		
		// Por continente
		if (g_missions[i].numjug == numjug && g_missions[i].continentes[pP->continente] == 1)
			return TRUE;
		
		// Por islas
		/*if (g_missions[i].numjug == numjug && g_missions[i].limitrofes > 0 && pP->isIsla == 1)
			return TRUE; */
		
	}
	
	return FALSE;
}

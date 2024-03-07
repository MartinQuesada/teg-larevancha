/*	$Id: country.c,v 1.2 2002/09/09 03:52:06 riq Exp $	*/
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
 * @file country.c
 */

#include "all.h"
#include <stdio.h>

COUNTRY g_countries[] = {
	{ LENTRY_NULL,  0, N_("Argentina"), 0, 0, 0, CONTINENTE_AMERICASUR, TARJETA_NULL_SUPERTARJETA, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL,  1, N_("Brazil"), 0, 0, 0, CONTINENTE_AMERICASUR, TARJETA_NULL_GLOBO, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL,  2, N_("Chile"), 0, 0, 0, CONTINENTE_AMERICASUR, TARJETA_NULL_GLOBO, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL,  3, N_("Colombia"), 0, 0, 0, CONTINENTE_AMERICASUR, TARJETA_NULL_GLOBO, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL,  4, N_("Paraguay"), 0, 0, 0, CONTINENTE_AMERICASUR, TARJETA_NULL_CANION, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL,  5, N_("Uruguay"), 0, 0, 0, CONTINENTE_AMERICASUR, TARJETA_NULL_CANION, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL,  6, N_("Bolivia"), 0, 0, 0, CONTINENTE_AMERICASUR, TARJETA_NULL_GLOBO, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL,  7, N_("Venezuela"), 0, 0, 0, CONTINENTE_AMERICASUR, TARJETA_NULL_CANION, COUNTRY_SELECT_NONE, NULL },

	/* AMERICA DEL NORTE */
	{ LENTRY_NULL,  8, N_("California"), 0, 0, 0, CONTINENTE_AMERICANORTE, TARJETA_NULL_GLOBO, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL,  9, N_("Las Vegas"), 0, 0, 0, CONTINENTE_AMERICANORTE, TARJETA_NULL_SUPERTARJETA, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 10, N_("Oregon"), 0, 0, 0, CONTINENTE_AMERICANORTE, TARJETA_NULL_CANION, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 11, N_("New York"), 0, 0, 0, CONTINENTE_AMERICANORTE, TARJETA_NULL_CANION, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 12, N_("Alaska"), 0, 0, 0, CONTINENTE_AMERICANORTE, TARJETA_NULL_COMODIN, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 13, N_("Florida"), 0, 0, 0, CONTINENTE_AMERICANORTE, TARJETA_NULL_GALEON, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 14, N_("Canada"), 0, 0, 0, CONTINENTE_AMERICANORTE, TARJETA_NULL_GLOBO, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 15, N_("Terranova"), 0, 0, 0, CONTINENTE_AMERICANORTE, TARJETA_NULL_CANION, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 16, N_("Labrador"), 0, 0, 0, CONTINENTE_AMERICANORTE, TARJETA_NULL_GALEON, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 17, N_("Greenland"), 0, 0, 0, CONTINENTE_AMERICANORTE, TARJETA_NULL_GALEON, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 18, N_("Chicago"), 0, 0, 0, CONTINENTE_AMERICANORTE, TARJETA_NULL_GLOBO, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 19, N_("Victoria Island"), 0, 0, 0, CONTINENTE_AMERICANORTE, TARJETA_NULL_GALEON, COUNTRY_SELECT_NONE, NULL },

	/* AFRICA */
	{ LENTRY_NULL, 20, N_("Somalia"), 0, 0, 0, CONTINENTE_AFRICA, TARJETA_NULL_COMODIN, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 21, N_("Sahara"), 0, 0, 0, CONTINENTE_AFRICA, TARJETA_NULL_CANION, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 22, N_("Nigeria"), 0, 0, 0, CONTINENTE_AFRICA, TARJETA_NULL_CANION, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 23, N_("Etiopia"), 0, 0, 0, CONTINENTE_AFRICA, TARJETA_NULL_GLOBO, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 24, N_("Egypt"), 0, 0, 0, CONTINENTE_AFRICA, TARJETA_NULL_GLOBO, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 25, N_("Madagascar"), 0, 0, 0, CONTINENTE_AFRICA, TARJETA_NULL_GALEON, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 26, N_("South Africa"), 0, 0, 0, CONTINENTE_AFRICA, TARJETA_NULL_CANION, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 27, N_("Mozambique"), 0, 0, 0, CONTINENTE_AFRICA, TARJETA_NULL_SUPERTARJETA, COUNTRY_SELECT_NONE, NULL },

	/* OCEANIA */
	{ LENTRY_NULL, 28, N_("Australia"), 0, 0, 0, CONTINENTE_OCEANIA, TARJETA_NULL_GLOBO, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 29, N_("Philippines"), 0, 0, 0, CONTINENTE_OCEANIA, TARJETA_NULL_GLOBO, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 30, N_("Tonga"), 0, 0, 0, CONTINENTE_OCEANIA, TARJETA_NULL_CANION, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 31, N_("Sumatra"), 0, 0, 0, CONTINENTE_OCEANIA, TARJETA_NULL_CANION, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 32, N_("Tasmania"), 0, 0, 0, CONTINENTE_OCEANIA, TARJETA_NULL_CANION, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 33, N_("New Zealand"), 0, 0, 0, CONTINENTE_OCEANIA, TARJETA_NULL_CANION, COUNTRY_SELECT_NONE, NULL },

	/* EUROPA */
	{ LENTRY_NULL, 34, N_("Spain"), 0, 0, 0, CONTINENTE_EUROPA, TARJETA_NULL_GLOBO, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 35, N_("France"), 0, 0, 0, CONTINENTE_EUROPA, TARJETA_NULL_GALEON, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 36, N_("Germany"), 0, 0, 0, CONTINENTE_EUROPA, TARJETA_NULL_COMODIN, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 37, N_("Italy"), 0, 0, 0, CONTINENTE_EUROPA, TARJETA_NULL_GALEON, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 38, N_("Poland"), 0, 0, 0, CONTINENTE_EUROPA, TARJETA_NULL_CANION, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 39, N_("Croatia"), 0, 0, 0, CONTINENTE_EUROPA, TARJETA_NULL_GLOBO, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 40, N_("Serbia"), 0, 0, 0, CONTINENTE_EUROPA, TARJETA_NULL_CANION, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 41, N_("Great Britain"), 0, 0, 0, CONTINENTE_EUROPA, TARJETA_NULL_GALEON, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 42, N_("Iceland"), 0, 0, 0, CONTINENTE_EUROPA, TARJETA_NULL_GALEON, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 43, N_("Ireland"), 0, 0, 0, CONTINENTE_EUROPA, TARJETA_NULL_GLOBO, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 44, N_("Albania"), 0, 0, 0, CONTINENTE_EUROPA, TARJETA_NULL_COMODIN, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 45, N_("Ukraine"), 0, 0, 0, CONTINENTE_EUROPA, TARJETA_NULL_SUPERTARJETA, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 46, N_("Belarus"), 0, 0, 0, CONTINENTE_EUROPA, TARJETA_NULL_GLOBO, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 47, N_("Finland"), 0, 0, 0, CONTINENTE_EUROPA, TARJETA_NULL_GALEON, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 48, N_("Norway"), 0, 0, 0, CONTINENTE_EUROPA, TARJETA_NULL_CANION, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 49, N_("Portugal"), 0, 0, 0, CONTINENTE_EUROPA, TARJETA_NULL_CANION, COUNTRY_SELECT_NONE, NULL },

	/* ASIA */
	{ LENTRY_NULL, 50, N_("Arabia"), 0, 0, 0, CONTINENTE_ASIA, TARJETA_NULL_GLOBO, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 51, N_("Israel"), 0, 0, 0, CONTINENTE_ASIA, TARJETA_NULL_GALEON, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 52, N_("Turkey"), 0, 0, 0, CONTINENTE_ASIA, TARJETA_NULL_CANION, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 53, N_("India"), 0, 0, 0, CONTINENTE_ASIA, TARJETA_NULL_GALEON, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 54, N_("Malaysia"), 0, 0, 0, CONTINENTE_ASIA, TARJETA_NULL_GALEON, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 55, N_("Iran"), 0, 0, 0, CONTINENTE_ASIA, TARJETA_NULL_GALEON, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 56, N_("Irak"), 0, 0, 0, CONTINENTE_ASIA, TARJETA_NULL_GALEON, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 57, N_("China"), 0, 0, 0, CONTINENTE_ASIA, TARJETA_NULL_GLOBO, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 58, N_("Rusia"), 0, 0, 0, CONTINENTE_ASIA, TARJETA_NULL_CANION, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 59, N_("Siberia"), 0, 0, 0, CONTINENTE_ASIA, TARJETA_NULL_CANION, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 60, N_("Chukchi"), 0, 0, 0, CONTINENTE_ASIA, TARJETA_NULL_GLOBO, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 61, N_("Chechenia"), 0, 0, 0, CONTINENTE_ASIA, TARJETA_NULL_SUPERTARJETA, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 62, N_("Vietnam"), 0, 0, 0, CONTINENTE_ASIA, TARJETA_NULL_CANION, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 63, N_("Kamchatka"), 0, 0, 0, CONTINENTE_ASIA, TARJETA_NULL_GALEON, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 64, N_("Japan"), 0, 0, 0, CONTINENTE_ASIA, TARJETA_NULL_GALEON, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 65, N_("Korea"), 0, 0, 0, CONTINENTE_ASIA, TARJETA_NULL_GLOBO, COUNTRY_SELECT_NONE, NULL },

	/* AMERICA CENTRAL */
	{ LENTRY_NULL, 66, N_("Nicaragua"), 0, 0, 0, CONTINENTE_AMERICACENTRO, TARJETA_NULL_GALEON, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 67, N_("El Salvador"), 0, 0, 0, CONTINENTE_AMERICACENTRO, TARJETA_NULL_GLOBO, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 68, N_("Honduras"), 0, 0, 0, CONTINENTE_AMERICACENTRO, TARJETA_NULL_GALEON, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 69, N_("Mexico"), 0, 0, 0, CONTINENTE_AMERICACENTRO, TARJETA_NULL_GALEON, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 70, N_("Cuba"), 0, 0, 0, CONTINENTE_AMERICACENTRO, TARJETA_NULL_GLOBO, COUNTRY_SELECT_NONE, NULL },
	{ LENTRY_NULL, 71, N_("Jamaica"), 0, 0, 0, CONTINENTE_AMERICACENTRO, TARJETA_NULL_GALEON, COUNTRY_SELECT_NONE, NULL },
};
#define COUNTRIES_TOT (sizeof(g_countries) /sizeof(g_countries[0]))

#include "limitrof.h"

/* returns true if country a and b are border */
BOOLEAN countries_eslimitrofe( int a, int b)
{
	if( a > b )
		return( mat_ady[b][a]==1 );
	else
		return( mat_ady[a][b]==1 );
}

/* returns the quantity of border contries that a country has */
int countries_cantlimitrofes( int p )
{
	int i;
	int cant=0;

	if(p < 0 || p >= COUNTRIES_CANT )
		return -1;

	for(i=0; i < COUNTRIES_CANT; i++ ) {
		if(  countries_eslimitrofe( p, i ) )
			cant++;
	}

	return cant;
}

/* Initializes country p */
void countries_initcountry( PCOUNTRY p )
{
	p->numjug=-1;
	p->ejercitos=0;
	InitializeListHead( &p->next );
	InitializeListHead( &p->tarjeta.next );
	tarjeta_inittarj( &p->tarjeta );
	p->tarjeta.numjug = -1;
	p->tarjeta.usada = FALSE;
	p->selected = COUNTRY_SELECT_NONE;
}

/* initializes all the countries */
void countries_init()
{
	int i;
	
	for(i=0;i<COUNTRIES_CANT;i++)
		countries_initcountry( &g_countries[i] );
}

/* says if a country is free or not */
BOOLEAN country_libre( int i )
{
	return( g_countries[i].numjug == -1 );
}

/* returns the name of a country */
char * countries_get_name( int i )
{
	static char *_unknown = N_("Unknown");

	if( i >= 0 && i < COUNTRIES_CANT )
		return _( g_countries[i].name );
	else
		return _( _unknown );
}

/* says if a country is locked or not
   j = jugador que tiene el country */
BOOLEAN country_esbloqueado ( PCOUNTRY p, int j )
{
	int jugador_bloqueo = -1;
	int paises_bloqueantes = 0;
	int i;

	for( i=0; i<COUNTRIES_CANT; i++ )
	{
		if( g_countries[i].ejercitos >=2 && g_countries[i].numjug != j && countries_eslimitrofe( p->id, i ) && p->id != i )
		{
			if( g_countries[i].numjug == jugador_bloqueo )
			{
				//fprintf(stderr, "Pais: %s. Bloqueantes: %i.\n",g_countries[i].name, paises_bloqueantes );
				paises_bloqueantes++;
			}
				
			// Si es el primer país que verifico asigno el usuario

			if( jugador_bloqueo == -1 )
			{
				//fprintf(stderr, "Pais (-1): %s. Bloqueantes: %i.\n",g_countries[i].name, paises_bloqueantes );
				paises_bloqueantes++;
				jugador_bloqueo=g_countries[i].numjug;
			}
		}
	}

	//fprintf(stderr, _("Pais: %s, Bloqueantes: %i, Limitrofes: %i.\n"), p->name, paises_bloqueantes, countries_cantlimitrofes(p->id) );

	return( paises_bloqueantes == countries_cantlimitrofes ( p->id ) && paises_bloqueantes > 2 );
}

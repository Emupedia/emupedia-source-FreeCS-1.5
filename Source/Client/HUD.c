/*
OpenCS Project
Copyright (C) 2015 Marco "eukara" Hladik

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#define HUD_NUMFILE "sprites/640hud7.spr" // We'll precache this
#define HUD_NUMFILE_LAYER "sprites/640hud7.spr_0.tga" // And only use the first frame for drawing (needs precache)

// Sigh
#define NUMSIZE_X 0.09375
#define NUMSIZE_Y 0.09765625

#define HUD_ALPHA 0.6

// Instead of calculating them on demand, just read the offsets here
float vHUDNumPos[10] = {
	0,
	0.09375,
	0.1875,
	0.28125,
	0.375,
	0.46875,
	0.5625,
	0.65625,
	0.75,
	0.84375,
};

// Ditto
vector vHUDCalPos[11] = {
	'0 0 0',
	'0.09375 0.28125 0', 	// 50AE
	'0.28125 0.28125 0', 	// 762MM
	'0 0.375 0', 			// 556MM
	'0 0.375 0', 			// 556MM
	'0.09375 0.375 0', 		// 338MAG
	'0.1875 0.28125 0', 	// 9MM
	'0 0.28125 0', 			// BUCKSHOT
	'0.375 0.28125 0', 		// 45ACP
	'0.46875 0.28125 0', 	// 357SIG
	'0.46875 0.375 0', 		// 57MM
};

/*
=================
HUD_DrawRedNumber

Draws a normal number
=================
*/
void HUD_DrawNumber( int iNumber, vector vPos, float fAlpha, vector vColor ) {
	drawsubpic( vPos, '24 25 0', HUD_NUMFILE_LAYER, [ vHUDNumPos[ iNumber ], 0], [ NUMSIZE_X, NUMSIZE_Y ], vColor, fAlpha, DRAWFLAG_ADDITIVE );
}

/*
=================
HUD_DrawNums

Draws numerals quickly for health, armor etc.
=================
*/
void HUD_DrawNums( float fNumber, vector vPos, float fAlpha, vector vColor ) {
	int iNumber = fNumber;
	if ( iNumber > 0 ) {
		while ( iNumber > 0 ) {
			HUD_DrawNumber( (float)iNumber % 10, vPos, fAlpha, vColor );
			iNumber = iNumber / 10;
			vPos_x -= 24;
		} 
	} else {
		HUD_DrawNumber( 0, vPos, fAlpha, vColor );
	}
}

/*
=================
HUD_DrawHealth

Draw the current amount of health
=================
*/
float fOldHealth;
float fHealthAlpha;
void HUD_DrawHealth( void ) {
	if ( getstatf( STAT_HEALTH ) != fOldHealth ) {
		fHealthAlpha = 1.0;
	}
	
	if ( fHealthAlpha >= HUD_ALPHA ) {
		fHealthAlpha -= frametime * 0.5;
	} else {
		fHealthAlpha = HUD_ALPHA;
	}
	
	vector vHealthPos = [ 16, vVideoResolution_y - 42 ];
	drawsubpic( vHealthPos, '24 24 0', HUD_NUMFILE_LAYER, [ NUMSIZE_X * 2, NUMSIZE_Y], [ NUMSIZE_X, NUMSIZE_X ], VGUI_WINDOW_FGCOLOR, HUD_ALPHA, DRAWFLAG_ADDITIVE );
	HUD_DrawNums( getstatf( STAT_HEALTH ), vHealthPos + '72 0', HUD_ALPHA, VGUI_WINDOW_FGCOLOR );
	fOldHealth = getstatf( STAT_HEALTH );
}

/*
=================
HUD_DrawArmor

Draw the current amount of Kevlar
=================
*/
float fOldArmor;
float fArmorAlpha;
void HUD_DrawArmor( void ) {
	if ( getstatf( STAT_ARMOR ) != fOldArmor ) {
		fArmorAlpha = 1.0;
	}
	
	if ( fArmorAlpha >= HUD_ALPHA ) {
		fArmorAlpha -= frametime * 0.5;
	} else {
		fArmorAlpha = HUD_ALPHA;
	}
	
	vector vArmorPos = [ 112, vVideoResolution_y - 42 ];
	drawsubpic( vArmorPos, '24 24 0', HUD_NUMFILE_LAYER, [ 0, NUMSIZE_Y], [ NUMSIZE_X, NUMSIZE_X ], VGUI_WINDOW_FGCOLOR, fArmorAlpha, DRAWFLAG_ADDITIVE );
	HUD_DrawNums( getstatf( STAT_ARMOR ), vArmorPos + '72 0', fArmorAlpha, VGUI_WINDOW_FGCOLOR);
	fOldArmor = getstatf( STAT_ARMOR );
}

/*
=================
HUD_DrawIcons

Draw icons such as hostage, bomb and buyzones
=================
*/
void HUD_DrawIcons( void ) {
	// BuyZone Icon
	if( getstatf( STAT_BUYZONE ) == TRUE ) {
		vector vBuyIconPos = [ 16, ( vVideoResolution_y / 2 ) - 12 ];
		drawsubpic( vBuyIconPos, '32 32 0', HUD_NUMFILE_LAYER, [ 0.125 * 3, 0.125 * 5 - 0.046875], [ 0.125, 0.125 ], '0 1 0', 1, DRAWFLAG_ADDITIVE );
	}
	
	// Hostage-Rescue Area Icon
	if( getstatf( STAT_HOSTAGEZONE ) == TRUE ) {
		vector vRIconPos = [ 16, ( vVideoResolution_y / 2 ) + 24 ];
		drawsubpic( vRIconPos, '32 32 0', HUD_NUMFILE_LAYER, [ 0.125 * 2, 0.125 * 5 - 0.046875], [ 0.125, 0.125 ], '0 1 0', 1, DRAWFLAG_ADDITIVE );
	}
	
	// Bomb-Area
	if( getstatf( STAT_BOMBZONE ) == TRUE ) {
		vector vBIconPos = [ 16, ( vVideoResolution_y / 2 ) + 24 ];
		
		if ( getstatf( STAT_ACTIVEWEAPON ) == WEAPON_C4BOMB ) {
			float fAlpha = fabs( sin( time * 20 ) );
			drawsubpic( vBIconPos, '32 32 0', HUD_NUMFILE_LAYER, [ 0, 0.125 * 5 - 0.046875], [ 0.125, 0.125 ], '1 0 0', fAlpha, DRAWFLAG_ADDITIVE );
			drawsubpic( vBIconPos, '32 32 0', HUD_NUMFILE_LAYER, [ 0, 0.125 * 5 - 0.046875], [ 0.125, 0.125 ], '0 1 0', 1 - fAlpha, DRAWFLAG_ADDITIVE );
		} else {
			drawsubpic( vBIconPos, '32 32 0', HUD_NUMFILE_LAYER, [ 0, 0.125 * 5 - 0.046875], [ 0.125, 0.125 ], '0 1 0', 1, DRAWFLAG_ADDITIVE );
		}
	}
}

/*
=================
HUD_DrawTimer

Draws the roundtime at the bottom of the screen (always visible)
=================
*/
int iOldUnits;
float fTimerAlpha;
void HUD_DrawTimer( void ) {
	int iMinutes, iSeconds, iTens, iUnits;
	vector vTimePos = [ ( vVideoResolution_x / 2 ) - 62, vVideoResolution_y - 42 ];
	
	if( serverkey( "timelimit" ) ) {
		float fTimeLeft = ( stof(serverkey( "timelimit" )) * 60 ) - getstatf( STAT_GAMETIME );
		if( fTimeLeft < 0 ) {
			iMinutes = iSeconds = iTens = iUnits = 0;
		} else {
			iMinutes = fTimeLeft / 60;
			iSeconds = fTimeLeft - 60 * iMinutes;
			iTens = iSeconds / 10;
			iUnits = iSeconds - 10*iTens;
		}
	} else {
		iMinutes = getstatf( STAT_GAMETIME ) / 60;
		iSeconds = getstatf( STAT_GAMETIME ) - 60*iMinutes;
		iTens = iSeconds / 10;
		iUnits = iSeconds - 10*iTens;
	}

	// Flashing red numbers
	if ( ( iMinutes == 0 ) &&  ( iTens <= 1 ) ) {
		float fAlpha;
		
		// 0:00 is fully red
		if ( ( iTens == 0 ) && ( iUnits == 0 ) ) {
			fAlpha = 1;
		} else {
			fAlpha = fabs( sin( time * 20 ) );
		}
		
		HUD_DrawNumber( iMinutes, vTimePos + '48 0 0', fAlpha, '1 0 0' );
		HUD_DrawNumber( iTens, vTimePos + '75 0 0', fAlpha, '1 0 0' );
		HUD_DrawNumber( iUnits, vTimePos + '99 0 0',fAlpha, '1 0 0' );
		HUD_DrawNumber( iMinutes, vTimePos + '48 0 0', 1 - fAlpha, VGUI_WINDOW_FGCOLOR );
		HUD_DrawNumber( iTens, vTimePos + '75 0 0', 1 - fAlpha, VGUI_WINDOW_FGCOLOR );
		HUD_DrawNumber( iUnits, vTimePos + '99 0 0',1 - fAlpha, VGUI_WINDOW_FGCOLOR );
		
		// : symbol
		drawsubpic( vTimePos + '70 6 0', '3 3 0', HUD_NUMFILE_LAYER, [0.9375, 0], [ 0.01171875, 0.01171875 ], '1 0 0', fAlpha, DRAWFLAG_ADDITIVE );
		drawsubpic( vTimePos + '70 16 0', '3 3 0', HUD_NUMFILE_LAYER, [0.9375, 0], [ 0.01171875, 0.01171875 ], '1 0 0', fAlpha, DRAWFLAG_ADDITIVE );
		drawsubpic( vTimePos + '70 6 0', '3 3 0', HUD_NUMFILE_LAYER, [0.9375, 0], [ 0.01171875, 0.01171875 ], VGUI_WINDOW_FGCOLOR, 1 - fAlpha, DRAWFLAG_ADDITIVE );
		drawsubpic( vTimePos + '70 16 0', '3 3 0', HUD_NUMFILE_LAYER, [0.9375, 0], [ 0.01171875, 0.01171875 ], VGUI_WINDOW_FGCOLOR, 1 - fAlpha, DRAWFLAG_ADDITIVE );
		
		drawsubpic( vTimePos, '24 25 0', HUD_NUMFILE_LAYER, [ NUMSIZE_X * 6, NUMSIZE_Y * 3], [ NUMSIZE_X, NUMSIZE_Y ], '1 0 0', fAlpha, DRAWFLAG_ADDITIVE );
		drawsubpic( vTimePos, '24 25 0', HUD_NUMFILE_LAYER, [ NUMSIZE_X * 6, NUMSIZE_Y * 3], [ NUMSIZE_X, NUMSIZE_Y ], VGUI_WINDOW_FGCOLOR, 1 - fAlpha, DRAWFLAG_ADDITIVE );
	} else {
		if ( iUnits != iOldUnits ) {
			fTimerAlpha = 1.0;
		}
	
		if ( fTimerAlpha >= HUD_ALPHA ) {
			fTimerAlpha -= frametime * 0.5;
		} else {
			fTimerAlpha = HUD_ALPHA;
		}
		HUD_DrawNumber( iMinutes, vTimePos + '48 0 0', fTimerAlpha, VGUI_WINDOW_FGCOLOR );
		HUD_DrawNumber( iTens, vTimePos + '75 0 0', fTimerAlpha, VGUI_WINDOW_FGCOLOR );
		HUD_DrawNumber( iUnits, vTimePos + '99 0 0', fTimerAlpha, VGUI_WINDOW_FGCOLOR );
		
		drawsubpic( vTimePos + '70 6 0', '3 3 0', HUD_NUMFILE_LAYER, [0.9375, 0], [ 0.01171875, 0.01171875 ], VGUI_WINDOW_FGCOLOR, fTimerAlpha, DRAWFLAG_ADDITIVE );
		drawsubpic( vTimePos + '70 16 0', '3 3 0', HUD_NUMFILE_LAYER, [0.9375, 0], [ 0.01171875, 0.01171875 ], VGUI_WINDOW_FGCOLOR, fTimerAlpha, DRAWFLAG_ADDITIVE );
		
		drawsubpic( vTimePos, '24 25 0', HUD_NUMFILE_LAYER, [ NUMSIZE_X * 6, NUMSIZE_Y * 3], [ NUMSIZE_X, NUMSIZE_Y ], VGUI_WINDOW_FGCOLOR, fTimerAlpha, DRAWFLAG_ADDITIVE );
		iOldUnits = iUnits;
	}
}

/*
=================
HUD_DrawMoney

Draws the amount of money (0-16000) with an icon to the screen
=================
*/
float fOldMoneyValue;
float fMoneyAlphaEffect;
vector vMoneyColorEffect;
float fMoneyDifference;
void HUD_DrawMoney( void ) {
	// If the money differs from last frame, paint it appropriately
	if ( getstatf( STAT_MONEY ) > fOldMoneyValue ) {
		// Make it green for a short time
		vMoneyColorEffect = '0 1 0';
		fMoneyAlphaEffect = 1.0;
		fMoneyDifference = fOldMoneyValue - getstatf( STAT_MONEY );
	} else if ( getstatf( STAT_MONEY ) < fOldMoneyValue ) {
		// Make it red
		vMoneyColorEffect = '1 0 0';
		fMoneyAlphaEffect = 1.0;
		fMoneyDifference = fOldMoneyValue - getstatf( STAT_MONEY );
	}
	
	vector vMoneyPos = [ vVideoResolution_x - 160, vVideoResolution_y - 72 ];
	
	// If the alpha/color effect is active, draw the money twice in their varying alphas/colors
	if ( fMoneyAlphaEffect > 0 ) {
		fMoneyAlphaEffect -= frametime * 0.5;
		drawsubpic( vMoneyPos, '18 25 0', HUD_NUMFILE_LAYER, [ NUMSIZE_X * 8, NUMSIZE_Y * 1], [ NUMSIZE_X * 0.75, NUMSIZE_Y ], vMoneyColorEffect, fMoneyAlphaEffect, DRAWFLAG_ADDITIVE );
		drawsubpic( vMoneyPos, '18 25 0', HUD_NUMFILE_LAYER, [ NUMSIZE_X * 8, NUMSIZE_Y * 1], [ NUMSIZE_X * 0.75, NUMSIZE_Y ], VGUI_WINDOW_FGCOLOR, HUD_ALPHA - ( fMoneyAlphaEffect * 0.5 ), DRAWFLAG_ADDITIVE );
		
		// Draw the +/- symbols depending on whether or not we made or lost money
		if ( fMoneyDifference < 0 ) {
			drawsubpic( vMoneyPos + '0 -32 0', '18 23 0', HUD_NUMFILE_LAYER, [ 0.8671875, 0.09765625], [ 0.0703125, 0.08984375 ], vMoneyColorEffect, fMoneyAlphaEffect, DRAWFLAG_ADDITIVE );
		} else {
			drawsubpic( vMoneyPos + '0 -32 0', '13 23 0', HUD_NUMFILE_LAYER, [ 0.8203125, 0.09765625], [ 0.05078125, 0.08984375 ], vMoneyColorEffect, fMoneyAlphaEffect, DRAWFLAG_ADDITIVE );
		}
		
		// Shift the numbers for reverse drawing
		vMoneyPos_x += ( 24 * 5 );
		
		// Draw the regular numbers at their normal positions
		HUD_DrawNums( getstatf( STAT_MONEY ), vMoneyPos, fMoneyAlphaEffect, vMoneyColorEffect );
		HUD_DrawNums( getstatf( STAT_MONEY ), vMoneyPos, HUD_ALPHA - ( fMoneyAlphaEffect * 0.5 ), VGUI_WINDOW_FGCOLOR );
		
		// Draw above how much money we've gotten from all this
		HUD_DrawNums( fabs( fMoneyDifference ), vMoneyPos + '0 -32 0', fMoneyAlphaEffect, vMoneyColorEffect );
	} else {
		drawsubpic( vMoneyPos, '18 25 0', HUD_NUMFILE_LAYER, [ NUMSIZE_X * 8, NUMSIZE_Y * 1], [ NUMSIZE_X * 0.75, NUMSIZE_Y ], VGUI_WINDOW_FGCOLOR, HUD_ALPHA, DRAWFLAG_ADDITIVE );
		vMoneyPos_x += ( 24 * 5 );
		HUD_DrawNums( getstatf( STAT_MONEY ), vMoneyPos, HUD_ALPHA, VGUI_WINDOW_FGCOLOR );
	}
	
	fOldMoneyValue = getstatf( STAT_MONEY );
}

/*
=================
HUD_DrawAmmo

Draws the current clip, the amount of ammo for the caliber and a matching caliber icon
=================
*/
float fOldClip, fOldCal;
float fAmmoAlpha;
void HUD_DrawAmmo( void ) {
	if ( getstatf( STAT_ACTIVEWEAPON ) == WEAPON_KNIFE || getstatf( STAT_ACTIVEWEAPON ) == WEAPON_C4BOMB ) {
		return;
	}
	
	if ( getstatf( STAT_CURRENT_CLIP ) != fOldClip || getstatf( STAT_CURRENT_CALIBER ) != fOldCal ) {
		fAmmoAlpha = 1.0;
	}
	
	if ( fAmmoAlpha >= HUD_ALPHA ) {
		fAmmoAlpha -= frametime * 0.5;
	} else {
		fAmmoAlpha = HUD_ALPHA;
	}
	
	vector vAmmoClipPos = [ vVideoResolution_x - 142, vVideoResolution_y - 42 ];
	HUD_DrawNums( getstatf( STAT_CURRENT_CLIP ), vAmmoClipPos, fAmmoAlpha, VGUI_WINDOW_FGCOLOR );
	
	drawsubpic( [vVideoResolution_x - 118, vVideoResolution_y - 42], '3 25 0', HUD_NUMFILE_LAYER, [0.9375, 0], [ 0.01171875, 0.09765625 ], VGUI_WINDOW_FGCOLOR, fAmmoAlpha, DRAWFLAG_ADDITIVE );
	
	vector vAmmoCalPos = [ vVideoResolution_x - 64, vVideoResolution_y - 42 ];
	HUD_DrawNums( getstatf( STAT_CURRENT_CALIBER ), vAmmoCalPos, fAmmoAlpha, VGUI_WINDOW_FGCOLOR );
	
	// Caliber icon
	drawsubpic( vVideoResolution - '42 42 0', '24 24 0', HUD_NUMFILE_LAYER, vHUDCalPos[ wptTable[ getstatf( STAT_ACTIVEWEAPON ) ].iCaliber ], [ NUMSIZE_X, NUMSIZE_X ], VGUI_WINDOW_FGCOLOR, fAmmoAlpha, DRAWFLAG_ADDITIVE );
	fOldClip = getstatf( STAT_CURRENT_CLIP );
	fOldCal = getstatf( STAT_CURRENT_CALIBER );
}

void HUD_DrawProgressBar( void ) {
	vector vSize = '540 16';
	vector vMainPos;
	vector v1, v2, v3;

	if ( getstatf( STAT_PROGRESS ) > 0 ) {
		vMainPos_x = ( vVideoResolution_x / 2 ) - (vSize_x / 2);
		vMainPos_y = ( vVideoResolution_y / 2 ) - (vSize_y / 2);
	
		// Draw the background
		vector vBar = vSize;
		vBar_x = 536 * getstatf( STAT_PROGRESS );
		vBar_y = 13;
		drawfill( vMainPos + '1 1 0', vBar, VGUI_WINDOW_FGCOLOR, 1, DRAWFLAG_ADDITIVE );
		
		// Draw the outline START
		v1_x = vMainPos_x + vSize_x;
		v1_y = vMainPos_y;
		drawline( 1.0, vMainPos - '1 0 0', v1, VGUI_WINDOW_FGCOLOR, 1, DRAWFLAG_ADDITIVE );
		
		v2_x = vMainPos_x;
		v2_y = vMainPos_y + vSize_y;
		drawline( 1.0, vMainPos, v2, VGUI_WINDOW_FGCOLOR, 1, DRAWFLAG_ADDITIVE );

		v3 = vMainPos + vSize;
		drawline( 1.0, v1, v3, VGUI_WINDOW_FGCOLOR, 1, DRAWFLAG_ADDITIVE );
		drawline( 1.0, v2, v3, VGUI_WINDOW_FGCOLOR, 1, DRAWFLAG_ADDITIVE );
		// Draw the outline END
	}
}

/*
=================
HUD_Draw

Called every frame in Draw.c
=================
*/
void HUD_Draw( void ) {
	
	HUD_DrawTimer();
	
	if( getplayerkeyvalue( player_localnum, "*spectator" ) == "1" ) {
		return;
	}
	
	HUD_DrawHealth();
	HUD_DrawArmor();
	HUD_DrawIcons();
	HUD_DrawMoney();
	HUD_DrawAmmo();
	HUD_DrawOrbituaries();
	HUD_DrawProgressBar();
}

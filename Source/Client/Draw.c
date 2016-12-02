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

void CSQC_UpdateView( float fWinWidth, float fWinHeight, float fGameFocus ) {
	vVideoResolution_x = fWinWidth;
	vVideoResolution_y = fWinHeight;

	clearscene();
	setproperty( VF_DRAWENGINESBAR, 0 );
	setproperty( VF_DRAWCROSSHAIR, 0 );

	addentities( MASK_ENGINE );
	View_DrawViewModel();
	renderscene();
	
	if( fGameFocus == TRUE ) {
		HUD_Draw();
		CSQC_VGUI_Draw();
		drawstring( '320 240 0 ', sprintf( "FRAMETIME: %f", eViewModel.frame1time ) , '8 8 0', '1 1 1', 1, 0 );
	}
}

void CSQC_UpdateViewLoading( float fWinWidth, float fWinHeight, float fGameFocus ) {
	
}

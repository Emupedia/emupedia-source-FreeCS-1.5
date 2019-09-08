/*
 * Copyright (c) 2016-2019 Marco Hladik <marco@icculus.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF MIND, USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

enum {
	ENT_PLAYER = 1,
	ENT_NPC,
	ENT_AMBIENTSOUND,
	ENT_SPRITE,
	ENT_SPRAY,
#ifdef VALVE
	ENT_TRIPMINE,
#endif
	ENT_DECAL
};

/* entity update flags */
enumflags {
	NPC_MODELINDEX,
	NPC_ORIGIN_X,
	NPC_ORIGIN_Y,
	NPC_ORIGIN_Z,
	NPC_ANGLES_X,
	NPC_ANGLES_Y,
	NPC_ANGLES_Z,
	NPC_VELOCITY_X,
	NPC_VELOCITY_Y,
	NPC_VELOCITY_Z,
	NPC_FRAME,
	NPC_SKIN,
	NPC_BODY
};

enumflags {
	PLAYER_KEEPALIVE,
	PLAYER_MODELINDEX,
	PLAYER_ORIGIN,
	PLAYER_ORIGIN_Z,
	PLAYER_ANGLES_X,
	PLAYER_ANGLES_Y,
	PLAYER_ANGLES_Z,
	PLAYER_VELOCITY,
	PLAYER_VELOCITY_Z,
	PLAYER_FLAGS,
	PLAYER_WEAPON,
	PLAYER_ITEMS,
	PLAYER_HEALTH,
	PLAYER_ARMOR,
	PLAYER_MOVETYPE,
	PLAYER_VIEWOFS,
	PLAYER_BASEFRAME,
	PLAYER_FRAME,
	PLAYER_AMMO1,
	PLAYER_AMMO2,
	PLAYER_AMMO3
};

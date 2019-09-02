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

#define PHY_JUMP_CHAINWINDOW	0.5
#define PHY_JUMP_CHAIN		100
#define PHY_JUMP_CHAINDECAY	50

/* FIXME: jumptime should use the time global, as time intervals are not
 * predictable - decrement it based upon input_timelength */
.float waterlevel;
.float watertype;
.float maxspeed;
.vector view_ofs;
int trace_endcontentsi;

#ifdef VALVE
int Items_CheckItem(entity pl, int i) {
    player pm = (player)pl;
	return pm.g_items & i ? TRUE:FALSE;
}
#endif

/* serverinfo keys are the only way both client and server are kept in sync
 * about physics variables. so none of the traditional cvars will work.
 * otherwise we could not have reliable prediction code for player movement.
 */
void
PMove_Init(void) {
#ifdef SSQC
	localcmd("serverinfo phy_stepheight 18\n");
	localcmd("serverinfo phy_airstepheight 18\n");
	localcmd("serverinfo phy_friction 4\n");
	localcmd("serverinfo phy_edgefriction 1\n");
	localcmd("serverinfo phy_stopspeed 75\n");
	localcmd("serverinfo phy_gravity 800\n");
	localcmd("serverinfo phy_airaccelerate 10\n");

#ifdef CSTRIKE
	localcmd("serverinfo phy_accelerate 4\n");
	localcmd("serverinfo phy_maxspeed 240\n");
#endif

#ifdef VALVE
	localcmd("serverinfo phy_accelerate 8\n");
	localcmd("serverinfo phy_maxspeed 270\n");
#endif
#endif
}

/* pointcontents reimplementation, only way we can effectively trace 
 * against ladders and liquids that are defined in the game-logic.
 */
int
PMove_Contents(vector org)
{
	int oldhitcontents = self.hitcontentsmaski;
	self.hitcontentsmaski = -1;
	traceline(org, org, TRUE, self);
	self.hitcontentsmaski = oldhitcontents;
	return trace_endcontentsi;
}

/* used for trigger_gravity type entities */
float
PMove_Gravity(entity ent)
{
	if (ent.gravity) {
		return serverkeyfloat("phy_gravity") * ent.gravity; 
	} else {
		return serverkeyfloat("phy_gravity");
	}
}

/* figure out where we are in the geometry. void, solid, liquid, etc. */
void
PMove_Categorize(void)
{
	int contents;

	if (self.flags & FL_CROUCHING) {
		self.mins = VEC_CHULL_MIN;
		self.maxs = VEC_CHULL_MAX;
		self.view_ofs = VEC_PLAYER_CVIEWPOS;
	} else {
		self.mins = VEC_HULL_MIN;
		self.maxs = VEC_HULL_MAX;
		self.view_ofs = VEC_PLAYER_VIEWPOS;
	}

	tracebox(self.origin, self.mins, self.maxs, self.origin - [0,0,0.25],
		FALSE, self);

	if (!trace_startsolid) {
		if ((trace_fraction < 1) && (trace_plane_normal[2] > 0.7)) {
			self.flags |= FL_ONGROUND;
		} else {
			self.flags &= ~FL_ONGROUND;
		}
	}

	/* ladder content testing */
	int oldhitcontents = self.hitcontentsmaski;
	self.hitcontentsmaski = CONTENTBIT_FTELADDER;
	tracebox(self.origin, self.mins, self.maxs, self.origin, FALSE, self);
	self.hitcontentsmaski = oldhitcontents;

	if (trace_endcontentsi & CONTENTBIT_FTELADDER) {
		self.flags |= FL_ONLADDER;
	} else {
		self.flags &= ~FL_ONLADDER;
	}

	contents = PMove_Contents(self.origin + self.mins + [0,0,1]);

	if (contents & CONTENTBIT_WATER) {
		contents = CONTENT_WATER;
	} else if (contents & CONTENTBIT_SLIME) {
		contents = CONTENT_SLIME;
	} else if (contents & CONTENTBIT_LAVA) {
		contents = CONTENT_LAVA;
	}

	/* how far underwater are we? */
	if (contents < CONTENT_SOLID && !(self.flags & FL_ONLADDER)) {
		self.watertype = contents;
		if (PMove_Contents(self.origin + (self.mins + self.maxs) * 0.5) 
			& CONTENTBITS_FLUID)
		{
			if (PMove_Contents(self.origin + self.maxs - '0 0 1')
				& CONTENTBITS_FLUID)
			{
				self.waterlevel = 3;
			} else {
				self.waterlevel = 2;
			}
		} else {
			self.waterlevel = 1;
		}
	} else {
		self.watertype = CONTENT_EMPTY;
		self.waterlevel = 0;
	}
}

/* this is technically run every frame, not just when we're in water */
void
PMove_WaterMove(void)
{
	if (self.movetype == MOVETYPE_NOCLIP) {
		return;
	}

	/*if (self.health < 0) {
		return;
	}*/

#if 0
	CPlayer plPlayer = (CPlayer)self;
	if (plPlayer.waterlevel != 3) {
		if (plPlayer.m_flAirFinished < time) {
			//sound (plPlayer, CHAN_VOICE, "player/gasp2.wav", 1, ATTN_NORM);
		} else if (plPlayer.m_flAirFinished < time + 9) {
			//sound (plPlayer, CHAN_VOICE, "player/gasp1.wav", 1, ATTN_NORM);
		}
		plPlayer.m_flAirFinished = time + 12;
		plPlayer.dmg = 2;
	} else if (plPlayer.m_flAirFinished < time) {
		if (plPlayer.m_flPainFinished < time) {
			plPlayer.dmg = plPlayer.dmg + 2;
			if (plPlayer.dmg > 15) {
				plPlayer.dmg = 10;
			}

			Damage_Apply(plPlayer, world, plPlayer.dmg, DAMAGE_DROWNING, WEAPON_NONE);
			plPlayer.m_flPainFinished = time + 1;
		}
	}
#endif

	if (!self.waterlevel){
		if (self.flags & FL_INWATER) {
#if 0
			//sound (self, CHAN_BODY, "misc/outwater.wav", 1, ATTN_NORM);
#endif
			self.flags &= ~FL_INWATER;
		}
		return;
	}

#if 0
	if (plPlayer.watertype == CONTENT_LAVA) {
		if (plPlayer.m_flDamageTime < time) {
			plPlayer.m_flDamageTime = time + 0.2;
			Damage_Apply(plPlayer, world, 10*plPlayer.waterlevel, DAMAGE_BURN, WEAPON_NONE);
		}
	} else if (plPlayer.watertype == CONTENT_SLIME) {
		if (plPlayer.m_flDamageTime < time) {
			plPlayer.m_flDamageTime = time + 1;
			Damage_Apply(plPlayer, world, 4*plPlayer.waterlevel, DAMAGE_ACID, WEAPON_NONE);
		}
	}
#endif
	if (!(self.flags & FL_INWATER)) {
#if 0
		sound (self, CHAN_BODY, "player/land/slosh.wav", 1, ATTN_NORM);
		plPlayer.m_flDamageTime = 0;
#endif
		self.flags |= FL_INWATER;
	}

	if (!(self.flags & FL_WATERJUMP)) {
		self.velocity = self.velocity - 0.8 * self.waterlevel * input_timelength * self.velocity;
	}
}

/* spammed whenever we're near a ledge, getting out of a pool or something */
void
PMove_CheckWaterJump(void)
{
	vector vStart;
	vector vEnd;

	makevectors(self.angles);
	vStart = self.origin;
	vStart[2] = vStart[2] + 8; 
	v_forward[2] = 0;
	normalize(v_forward);
	vEnd = vStart + (v_forward * 24);
	traceline(vStart, vEnd, TRUE, self);

	if (trace_fraction < 1) {
		vStart[2] = vStart[2] + self.maxs[2];
		vEnd = vStart + (v_forward * 24);
		//self.movedir = trace_plane_normal * -50;
		traceline(vStart, vEnd, TRUE, self);
		
		if (trace_fraction == 1) {
			//self.flags = self.flags | FL_WATERJUMP;
			self.velocity[2] = 350;
			self.flags &= ~FL_JUMPRELEASED;
			return;
		}
	}
}

/* simple bounds check */
int
QPMove_IsStuck(entity eTarget, vector vOffset, vector vecMins, vector vecMaxs)
{
	vector bound;

	if (eTarget.solid != SOLID_SLIDEBOX) {
		return FALSE;
	}

	bound = eTarget.origin + vOffset;
	tracebox(bound, vecMins, vecMaxs, bound, FALSE, eTarget);
	return trace_startsolid;
}

/* two-pass acceleration */
void
PMove_Run_Acceleration(float move_time, float premove)
{
	vector vecWishVel;
	vector wish_dir;
	vector vecTemp;
	float wish_speed;
	float flFriction;
	float flJumptimeDelta;
	float flChainBonus;
	int iFixCrouch = FALSE;

	PMove_Categorize();

	// Update the timer
	self.jumptime -= move_time;
	self.teleport_time -= move_time;

	// Corpses
	if (self.movetype == MOVETYPE_TOSS) {
		self.velocity[2] = self.velocity[2] - (PMove_Gravity(self) * move_time);
		return;
	}

	if (self.movetype == MOVETYPE_WALK) {
		// Crouching
		if (input_buttons & INPUT_BUTTON8) {
			self.flags |= FL_CROUCHING;
		} else {
			// If we aren't holding down duck anymore and 'attempt' to stand up, prevent it
			if (self.flags & FL_CROUCHING) {
				if (QPMove_IsStuck(self, '0 0 36', VEC_HULL_MIN, VEC_HULL_MAX) == FALSE) {
					self.flags &= ~FL_CROUCHING;
					iFixCrouch = TRUE;
				}
			} else {
				self.flags &= ~FL_CROUCHING;
			}
		}

		if (self.flags & FL_CROUCHING) {
			setsize(self, VEC_CHULL_MIN, VEC_CHULL_MAX);
			self.view_ofs = VEC_PLAYER_CVIEWPOS;
		} else {
			setsize(self, VEC_HULL_MIN, VEC_HULL_MAX);
			if (iFixCrouch && QPMove_IsStuck(self, [0,0,0], VEC_HULL_MIN, VEC_HULL_MAX)) {
				for (int i = 0; i < 36; i++) {
					self.origin[2] += 1;
					if (QPMove_IsStuck(self, [0,0,0], self.mins, self.maxs) == FALSE) {
						break;
					}
				}
			}
			setorigin(self, self.origin);
			self.view_ofs = VEC_PLAYER_VIEWPOS;
		}
	}

	makevectors(input_angles);

	// swim
	if (self.waterlevel >= 2) {
		if (self.movetype != MOVETYPE_NOCLIP) {
			self.flags &= ~FL_ONGROUND;

			if (input_movevalues == [0,0,0]) {
				vecWishVel = [0,0,-60]; // drift towards bottom
			} else {
				vecWishVel = v_forward * input_movevalues[0];
				vecWishVel += v_right * input_movevalues[1];
				vecWishVel += v_up * input_movevalues[2];
			}

			wish_speed = vlen(vecWishVel);

			if (wish_speed > self.maxspeed) {
				wish_speed = self.maxspeed;
			}

			wish_speed = wish_speed * 0.7;

			// water friction
			if (self.velocity != [0,0,0]) {
				flFriction = vlen(self.velocity) * (1 - move_time * serverkeyfloat("phy_friction"));
				if (flFriction > 0) {
					self.velocity = normalize(self.velocity) * flFriction;
				} else {
					self.velocity = [0,0,0];
				}
			} else {
				flFriction = 0;
			}

			// water acceleration
			if (wish_speed <= flFriction) {
				return;
			}

			flFriction = min(wish_speed - flFriction, serverkeyfloat("phy_accelerate") * wish_speed * move_time);
			self.velocity = self.velocity + normalize(vecWishVel) * flFriction;
			return;
		}
	}

	// hack to not let you back into teleporter
	if (self.teleport_time > 0 && input_movevalues[0] < 0) {
		vecWishVel = v_right * input_movevalues[1];
	} else {
		if (self.movetype == MOVETYPE_NOCLIP) {
		} else if (self.flags & FL_ONGROUND) {
			makevectors (input_angles[1] * [0,1,0]);
		}

		vecWishVel = v_forward * input_movevalues[0] + v_right * input_movevalues[1];
	}

	if (self.movetype != MOVETYPE_WALK) {
		vecWishVel[2] += input_movevalues[2];
	} else {
		vecWishVel[2] = 0;
	}

	wish_dir = normalize(vecWishVel);
	wish_speed = vlen(vecWishVel);
	
	if (wish_speed > self.maxspeed) {
		wish_speed = self.maxspeed;
	}

	if (self.movetype == MOVETYPE_NOCLIP) {
		self.flags &= ~FL_ONGROUND;
		self.velocity = wish_dir * wish_speed;
	} else {
		/*FIXME: pogostick*/
		if (self.flags & FL_ONGROUND)
		if (!(self.flags & FL_WATERJUMP))
		if (self.flags & FL_JUMPRELEASED)
		if (input_buttons & INPUT_BUTTON2 && premove) {
			if (self.velocity[2] < 0) {
				self.velocity[2] = 0;
			}

			if (self.waterlevel >= 2) {
				if (self.watertype == CONTENT_WATER) {
					self.velocity[2] = 100;
				} else if (self.watertype == CONTENT_SLIME) {
					self.velocity[2] = 80;
				} else {
					self.velocity[2] = 50;
				}
			} else {
	#ifdef VALVE
				if (self.flags & FL_CROUCHING && Items_CheckItem(self, 0x00008000)) {
					self.velocity = v_forward * 512;
					self.velocity[2] += 100;
				}
	#endif
				self.velocity[2] += 240;
			}

			if (self.jumptime > 0) {
				// time since last jump
				flJumptimeDelta = 0 - (self.jumptime - PHY_JUMP_CHAINWINDOW);
				//self.velocity[2] += PHY_JUMP_CHAIN;
				flChainBonus = PHY_JUMP_CHAIN - (((PHY_JUMP_CHAINWINDOW - (PHY_JUMP_CHAINWINDOW - flJumptimeDelta)) * 2) * PHY_JUMP_CHAINDECAY);
				self.velocity[2] += flChainBonus;
			}
			self.jumptime = PHY_JUMP_CHAINWINDOW;
			self.flags &= ~FL_ONGROUND;
			self.flags &= ~FL_JUMPRELEASED;
		}

		// not pressing jump, set released flag
		if (!(input_buttons & INPUT_BUTTON2)) {
			self.flags |= FL_JUMPRELEASED;
		}

		if (self.flags & FL_ONLADDER) {
			vector vPlayerVector;

			makevectors(input_angles);
			vPlayerVector = v_forward;
			vPlayerVector = (vPlayerVector * 240);

			if (input_movevalues[0] > 0) {
				self.velocity = vPlayerVector;
			} else {
				self.velocity = [0,0,0];
			}
		} else if (self.flags & FL_ONGROUND) {
			// friction
			if (self.velocity[0] || self.velocity[1]) {
				vecTemp = self.velocity;
				vecTemp[2] = 0;
				flFriction = vlen(vecTemp);

				// if the leading edge is over a dropoff, increase friction
				vecTemp = self.origin + normalize(vecTemp) * 16 + '0 0 1' * VEC_HULL_MIN[2];
				traceline(vecTemp, vecTemp + '0 0 -34', TRUE, self);

				// apply friction
				if (trace_fraction == 1.0) {
					if (flFriction < serverkeyfloat("phy_stopspeed")) {
						flFriction = 1 - move_time * (serverkeyfloat("phy_stopspeed") / flFriction) * serverkeyfloat("phy_friction") * serverkeyfloat("phy_edgefriction");
					} else {
						flFriction = 1 - move_time * serverkeyfloat("phy_friction") * serverkeyfloat("phy_edgefriction");
					}
				} else {
					if (flFriction < serverkeyfloat("phy_stopspeed")) {
						flFriction = 1 - move_time * (serverkeyfloat("phy_stopspeed") / flFriction) * serverkeyfloat("phy_friction");
					} else {
						flFriction = 1 - move_time * serverkeyfloat("phy_friction");
					}
				}

				if (flFriction < 0) {
					self.velocity = [0,0,0];
				} else {
					self.velocity = self.velocity * flFriction;
				}
			}

			// acceleration
			flFriction = wish_speed - (self.velocity * wish_dir);
			if (flFriction > 0) {
				self.velocity = self.velocity + wish_dir * min(flFriction, serverkeyfloat("phy_accelerate") * move_time * wish_speed);
			}
		} else {
			/* apply gravity */
			self.velocity[2] = self.velocity[2] - (PMove_Gravity(self) * move_time);

			if (wish_speed < 30) {
				flFriction = wish_speed - (self.velocity * wish_dir);
			} else {
				flFriction = 30 - (self.velocity * wish_dir);
			}

			if (flFriction > 0) {
				float fric;
				fric = min(flFriction, serverkeyfloat("phy_airaccelerate") * wish_speed * move_time);
				self.velocity += wish_dir * fric;
			}
		}
	}
}

void
PMove_DoTouch(entity tother)
{
	entity oself = self;
	if (tother.touch) {
		other = self;
		self = tother;
		self.touch();
	}
	self = oself;
}

/* bounce us back off a place normal */
static void
PMove_Rebound(vector normal)
{
	self.velocity = self.velocity - normal * (self.velocity * normal);
}

/* brute force unstuck function */
float
PMove_Fix_Origin(void)
{
	float x, y, z;
	vector norg, oorg = self.origin;

	for (z = 0; z < 3; z++) {
		norg[2] = oorg[2] + ((z==2)?-1:z)*0.0125;
		for (x = 0; x < 3; x++) {
			norg[0] = oorg[0] + ((x==2)?-1:x)*0.0125;
			for (y = 0; y < 3; y++) {
				norg[1] = oorg[1] + ((y==2)?-1:y)*0.0125;

				tracebox(norg, self.mins, self.maxs, norg, FALSE, self);
				if (!trace_startsolid) {
					self.origin = norg;
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

/* move the player based on the given acceleration */
void
PMove_Run_Move(void)
{
	vector dest;
	vector saved_plane;
	float stepped;
	float move_time;
	int i;

	/* no friction for the deceased */
	if (self.movetype == MOVETYPE_NOCLIP) {
		self.origin = self.origin + self.velocity * input_timelength;
		return;
	}

	/* we need to bounce off surfaces (in order to slide along them), 
	 * so we need at 2 attempts
	 */
	for (i = 3, move_time = input_timelength; move_time > 0 && i; i--) {
		dest = self.origin + (self.velocity * move_time);
		tracebox(self.origin, self.mins, self.maxs, dest, FALSE, self);

		if (trace_startsolid) {
			if (!PMove_Fix_Origin()) {
				return;
			}
			continue;
		}

		self.origin = trace_endpos;

		if (trace_fraction > 1) {
			break;
		}
		saved_plane = trace_plane_normal;
		move_time -= move_time * trace_fraction;

		if (move_time) {
			/* step up if we can */
			trace_endpos = self.origin;

			if (self.flags & FL_ONGROUND) {
				trace_endpos[2] += serverkeyfloat("phy_stepheight");
			} else {
				trace_endpos[2] += serverkeyfloat("phy_airstepheight");
			}

			tracebox(self.origin, self.mins, self.maxs, trace_endpos, FALSE, self);
			stepped = trace_endpos[2] - self.origin[2];

			float roof_fraction = trace_fraction;
			vector roof_plane_normal = trace_plane_normal;

			dest = trace_endpos + self.velocity*move_time;
			dest[2] = trace_endpos[2]; /*only horizontally*/

			/* move forwards */
			tracebox(trace_endpos, self.mins, self.maxs, dest, FALSE, self);

			/* if we got anywhere, make this raised-step move count */
			if (trace_fraction != 0) {
				float fwfrac = trace_fraction;
				vector fwplane = trace_plane_normal;

				/* move down */
				dest = trace_endpos;
				dest[2] -= stepped + 1;
				tracebox(trace_endpos, self.mins, self.maxs, dest, FALSE, self);

				if (trace_fraction < 1 && trace_plane_normal[2] > 0.7f) {
					move_time -= move_time * fwfrac;
					/* bounce off the ceiling */
					if (roof_fraction < 1) {
						PMove_Rebound(roof_plane_normal);
					}
					if (trace_fraction < 1) {
						PMove_Rebound(trace_plane_normal);
					} else if (fwfrac < 1) {
						PMove_Rebound(fwplane);
					}
					self.origin = trace_endpos;
					continue;
				}
			}
		}

		/* stepping failed, just bounce off */
		PMove_Rebound(saved_plane);
		PMove_DoTouch(trace_ent);
	}

	/* touch whatever is below */
	if (self.flags & FL_ONGROUND) {
		dest = self.origin;
		dest[2] -= serverkeyfloat("phy_stepheight");
		tracebox(self.origin, self.mins, self.maxs, dest, FALSE, self);
		if (trace_fraction >= 1) {
			return;
		}
		/*if (trace_startsolid) {
			if (!PMove_Fix_Origin()) {
				return;
			}
		}*/
		PMove_DoTouch(trace_ent);
	}
}

/* it all starts here */
void
PMove_Run(void)
{
#ifdef VALVE
	self.maxspeed = (self.flags & FL_CROUCHING) ? 135 : 270;

	if (input_buttons & INPUT_BUTTON5) {
		input_movevalues *= 0.25;
	}
#endif
	PMove_WaterMove();

	if (self.waterlevel >= 2) {
		PMove_CheckWaterJump();
	}

	if (input_buttons & INPUT_BUTTON2) {
		input_movevalues[2] = 240;
	}	
	if (input_buttons & INPUT_BUTTON8) {
		input_movevalues[2] = -240;
	}

	/* call accelerate before and after the actual move, 
	 * with half the move each time. this reduces framerate dependence. 
	 * and makes controlling jumps slightly easier
	 */
	PMove_Run_Acceleration(input_timelength / 2, TRUE);
	PMove_Run_Move();
	PMove_Run_Acceleration(input_timelength / 2, FALSE);

	/* NOTE: should clip to network precision here if lower than a float */
	self.angles = input_angles;
	self.angles[0] *= -0.333;

	touchtriggers();

	player pl = (player)self;
#ifdef VALVE
	pl.w_attack_next = max(0, pl.w_attack_next - input_timelength);
	pl.w_idle_next = max(0, pl.w_idle_next - input_timelength);
#endif
	pl.weapontime += input_timelength;
	Game_Input();
}
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

/* parse info_node entities and convert them to FTE compatible routing data */

typedef struct node_s {
	vector origin;
	float radius;

	struct neighbour_s
	{
		int node;
		float dist;
		int flags;
	} *nb;
	int nb_count;
} node_t;

static node_t *g_pNodes;
static int g_iNodes;
int g_nodes_present;

/* info_nodes can do a lot more in theory, right now we don't */
class info_node { };

/* write current nodes to disk */
void
Nodes_Save(string filename)
{
	filestream wayfile = fopen(filename, FILE_WRITE);

	if (wayfile < 0) {
		return;
	}

	fputs(wayfile, sprintf("%i\n", g_iNodes));

	for (int i = 0; i < g_iNodes; i++) {
		fputs(
			wayfile,
			sprintf(
				"%v %f %i\n",
				g_pNodes[i].origin,
				g_pNodes[i].radius,
				g_pNodes[i].nb_count
			)
		);

		for(int j = 0; j < g_pNodes[i].nb_count; j++) {
			fputs(
				wayfile,
				sprintf(
					" %i %f %x\n",
					g_pNodes[i].nb[j].node,
					g_pNodes[i].nb[j].dist,
					(float)g_pNodes[i].nb[j].flags
				)
			);
		}
	}

	g_nodes_present = TRUE;
	fclose(wayfile);
}

/* link two nodes together */
static void
Node_Link(node_t *n1, node_t *n2)
{
	int w2n = n2 - g_pNodes;
	for (int i = 0; i < n1->nb_count; i++) {
		if (n1->nb[i].node == w2n) {
			return;
		}
	}

	int idx = n1->nb_count++;
	n1->nb = memrealloc(n1->nb, sizeof(*n1->nb), idx, n1->nb_count);
	local struct neighbour_s *n = n1->nb+idx;
	n->node = w2n;
	n->dist = vlen(n2->origin - n1->origin);
	n->flags = 0;
}

/* loop through already existing nodes, test against them and link */
static void
Node_AutoLink(node_t *new)
{
	int x = new - g_pNodes;

	for (int i = 0; i < g_iNodes; i++) {
		/* don't link to ourselves... */
		if (i == x) {
			continue;
		}

		// TODO: Check distance?

		/* can't use full player size, because steps = messy */
		tracebox(
			new->origin,
			[-16,-16,-8],
			[16,16,32],
			g_pNodes[i].origin,
			TRUE,
			world
		);

		/* line of sight blocked */
		if (trace_fraction < 1) {
			continue;
		}

		Node_Link(new, &g_pNodes[i]);
		Node_Link(&g_pNodes[i], new);
	}
}

/* generate node tree, used for AI movement/navigation */
void
Nodes_Init(void)
{
	g_nodes_present = FALSE;

	/* skip if present. TODO: check if they're out of date? */
	if (whichpack(sprintf("data/%s.way", mapname))) {
		g_nodes_present = TRUE;
		return;
	}

	dprint("[^2NODES^7] Rebuilding node tree...");

	/* run through the ents and rebuild the tree */
	for (entity a = world; a = find(a, ::classname, "info_node");) {
		int iID = g_iNodes++;
		g_pNodes = memrealloc(g_pNodes, sizeof(node_t), iID, g_iNodes);
		node_t *n = g_pNodes + iID;
		n->origin = a.origin;
		n->nb = __NULL__;
		n->nb_count = 0;
		Node_AutoLink(n);
	}

	dprint(" ^2DONE\n");
	dprint(sprintf("[^2NODES^7] %i nodes found.\n", g_iNodes));

	Nodes_Save(sprintf("%s.way", mapname));

#ifndef GS_DEVELOPER
	/* we don't need these any longer */
	for (int i = 0; i < g_iNodes; i++) {
		memfree(g_pNodes[i].nb);
	}
	memfree(g_pNodes);
	g_iNodes = 0;
#endif
}

/* draws debug graphics of our node tree */
void
SV_AddDebugPolygons(void)
{
#ifdef GS_DEVELOPER
	if (!g_iNodes) {
		return;
	}

	makevectors(self.v_angle);

	/* draw the rectangles */
	R_BeginPolygon("", 0, 0);
	for (int i = 0; i < g_iNodes; i++) {
		node_t *w = g_pNodes + i;
		vector org = w->origin;
		vector rgb = [1,1,1];
		R_PolygonVertex(org + v_right * 8 - v_up * 8, [1,1], rgb, 1.0f);
		R_PolygonVertex(org - v_right * 8 - v_up * 8, [0,1], rgb, 1.0f);
		R_PolygonVertex(org - v_right * 8 + v_up * 8, [0,0], rgb, 1.0f);
		R_PolygonVertex(org + v_right * 8 + v_up * 8, [1,0], rgb, 1.0f);
		R_EndPolygon();
	}

	/* draw the radius */
	R_BeginPolygon("", 0, 0);
	for (int i = 0; i < g_iNodes; i++) {
		node_t *w = g_pNodes + i;
		vector org = w->origin;

		for (float j = 0; j < (2 * M_PI); j += (2 * M_PI) / 4) {
			R_PolygonVertex(
				org + [sin(j),cos(j)]*w->radius,
				[1,1],
				[0,0.25,0],
				1.0f
			);
		}
		R_EndPolygon();
	}

	/* draw the lines */
	R_BeginPolygon("", 1, 0);
	for (int i = 0; i < g_iNodes; i++) {
		node_t *w = g_pNodes+i;
		vector org = w->origin;
		vector rgb = [1,1,1];

		for (int j = 0; j < w->nb_count; j++) {
			int k = w->nb[j].node;

			/* check for invalids */
			if (k < 0 || k >= g_iNodes) {
				break;
			}

			node_t *w2 = &g_pNodes[k];

			R_PolygonVertex(org, [0,1], [1,0,1], 1.0f);
			R_PolygonVertex(w2->origin, [1,1], [0,1,0], 1.0f);
			R_EndPolygon();
		}
	}
#endif
}
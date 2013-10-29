/* -*- Mode: C; tab-width: 4 -*- */
/* moebius --- Moebius Strip II, an Escher-like GL scene with ants. */

#if 0
static const char sccsid[] = "@(#)moebius.c	5.01 2001/03/01 xlockmore";
#endif

/*-
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation.
 *
 * This file is provided AS IS with no warranties of any kind.  The author
 * shall have no liability with respect to the infringement of copyrights,
 * trade secrets or any patents by this file or any part thereof.  In no
 * event will the author be liable for any lost revenue or profits or
 * other special, indirect and consequential damages.
 *
 * The RotateAroundU() routine was adapted from the book
 *    "Computer Graphics Principles and Practice
 *     Foley - vanDam - Feiner - Hughes
 *     Second Edition" Pag. 227, exercise 5.15.
 *
 * This mode shows some interesting scenes that are impossible OR very
 * wierd to build in the real universe. Much of the scenes are inspirated
 * on Mauritz Cornelis Escher's works which derivated the mode's name.
 * M.C. Escher (1898-1972) was a dutch artist and many people prefer to
 * say he was a mathematician.
 *
 * Thanks goes to Brian Paul for making it possible and inexpensive to use
 * OpenGL at home.
 *
 * Since I'm not a native English speaker, my apologies for any grammatical
 * mistakes.
 *
 * My e-mail address is
 * mfvianna@centroin.com.br
 *
 * Marcelo F. Vianna (Jun-01-1997)
 *
 * Revision History:
 * 05-Apr-2002: Removed all gllist uses (fix some bug with nvidia driver)
 * 01-Mar-2001: backported from xscreensaver by lassauge@mail.dotcom.fr
 *    Feb-2001: Made motion and rotation be smoother Jamie Zawinski
 *              <jwz@jwz.org>
 * 01-Nov-2000: Allocation checks
 * 01-Jan-1998: Mode separated from escher and renamed
 * 08-Jun-1997: New scene implemented: "Impossible Cage" based in a M.C.
 *              Escher's painting with the same name (quite similar). The
 *              first GL mode to use texture mapping.
 *              The "Impossible Cage" scene doesn't use DEPTH BUFFER, the
 *              wood planks are drawn consistently using GL_CULL_FACE, and
 *              the painter's algorithm is used to sort the planks.
 *              Marcelo F. Vianna.
 * 07-Jun-1997: Speed ups in Moebius Strip using GL_CULL_FACE.
 *              Marcelo F. Vianna.
 * 03-Jun-1997: Initial Release (Only one scene: "Moebius Strip")
 *              The Moebius Strip scene was inspirated in a M.C. Escher's
 *              painting named Moebius Strip II in wich ants walk across a
 *              Moebius Strip path, sometimes meeting each other and sometimes
 *              being in "opposite faces" (note that the moebius strip has
 *              only one face and one edge).
 *              Marcelo F. Vianna.
 */

/*-
 * Texture mapping is only available on RGBA contexts, Mono and color index
 * visuals DO NOT support texture mapping in OpenGL.
 *
 * BUT Mesa do implements RGBA contexts in pseudo color visuals, so texture
 * mapping shuld work on PseudoColor, DirectColor, TrueColor using Mesa. Mono
 * is not officially supported for both OpenGL and Mesa, but seems to not crash
 * Mesa.
 *
 * In real OpenGL, PseudoColor DO NOT support texture map (as far as I know).
 */

#define MODE_moebius
#define refresh_moebius NULL
#define DELAY 20000
#define DEFAULTS			"*delay:		20000   \n"			\
							"*showFPS:      False   \n"

#if 0
	#ifdef STANDALONE
	# include "xlockmore.h"		/* from the xscreensaver distribution */
	#else /* !STANDALONE */
	# include "xlock.h"		/* from the xlockmore distribution */
	#endif /* !STANDALONE */
#endif

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>

#include "win32.h"

#if 0
	#ifdef HAVE_COCOA
	# include "jwxyz.h"
	#else
	# include <X11/Xlib.h>
	# include <GL/gl.h>
	# include <GL/glu.h>
	#endif

	#ifdef HAVE_JWZGLES
	# include "jwzgles.h"
	#endif /* HAVE_JWZGLES */
#endif

#ifdef MODE_moebius

#if 0 /* Hey, this never actually used the texture at all! */
#if 0
#include "e_textures.h"
#else
#include "xpm-ximage.h"
#include "../images/wood.xpm"
#endif
#endif /* 0 */

#include "sphere.h"
#include "tube.h"

#include "rotator.h"
//#include "gltrackball.h"

#define DEF_SOLIDMOEBIUS  "False"
#define DEF_DRAWANTS  "True"

static int  solidmoebius = False;
static int  drawants = True;

#if 0
	static XrmOptionDescRec opts[] =
	{
	  {"-solidmoebius", ".moebius.solidmoebius", XrmoptionNoArg, "on"},
	  {"+solidmoebius", ".moebius.solidmoebius", XrmoptionNoArg, "off"},
	  {"-ants", ".moebius.drawants", XrmoptionNoArg, "on"},
	  {"+ants", ".moebius.drawants", XrmoptionNoArg, "off"}
	};
	static argtype vars[] =
	{
	  {&solidmoebius, "solidmoebius", "Solidmoebius", DEF_SOLIDMOEBIUS, t_Bool},
	  {&drawants, "drawants", "Drawants", DEF_DRAWANTS, t_Bool}

	};
	static OptionStruct desc[] =
	{
		{"-/+solidmoebius", "select between a SOLID or a NET Moebius Strip"},
		{"-/+drawants", "turn on/off walking ants"}
	};

	ENTRYPOINT ModeSpecOpt moebius_opts =
	{sizeof opts / sizeof opts[0], opts, sizeof vars / sizeof vars[0], vars, desc};
#endif

#if 0
	#ifdef USE_MODULES
	ModStruct   moebius_description =
	{"moebius", "init_moebius", "draw_moebius", "release_moebius",
	 "draw_moebius", "change_moebius", (char *) NULL, &moebius_opts,
	 1000, 1, 1, 1, 4, 1.0, "",
	 "Shows Moebius Strip II, an Escher-like GL scene with ants", 0, NULL};
	#endif
#endif

#define Scale4Window               0.3
#define Scale4Iconic               0.4

#define sqr(A)                     ((A)*(A))

#ifndef Pi
#define Pi                         M_PI
#endif

#define ObjMoebiusStrip 0
#define ObjAntBody      1
#define MaxObj          2

/*************************************************************************/

typedef struct {
	GLint       WindH, WindW;
	GLfloat     step;
	GLfloat     ant_position;
	float       ant_step;
	//GLXContext *glx_context;
	HGLRC hglrc;
    rotator    *rot;
    //trackball_state *trackball;
    //Bool        button_down_p;
} moebiusstruct;

static const float front_shininess[] = {60.0f};
static const float front_specular[] = {0.7f, 0.7f, 0.7f, 1.0f};
static const float ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};
static const float diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
static const float position0[] = {1.0f, 1.0f, 1.0f, 0.0f};
static const float position1[] = {-1.0f, -1.0f, 1.0f, 0.0f};
static const float lmodel_ambient[] = {0.5f, 0.5f, 0.5f, 1.0f};
static const float lmodel_twoside[] = {GL_TRUE};

static const float MaterialRed[] = {0.7f, 0.0f, 0.0f, 1.0f};
static const float MaterialGreen[] = {0.1f, 0.5f, 0.2f, 1.0f};
static const float MaterialBlue[] = {0.0f, 0.0f, 0.7f, 1.0f};
static const float MaterialCyan[] = {0.2f, 0.5f, 0.7f, 1.0f};
static const float MaterialYellow[] = {0.7f, 0.7f, 0.0f, 1.0f};
static const float MaterialMagenta[] = {0.6f, 0.2f, 0.5f, 1.0f};
static const float MaterialWhite[] = {0.7f, 0.7f, 0.7f, 1.0f};
static const float MaterialGray[] = {0.2f, 0.2f, 0.2f, 1.0f};
static const float MaterialGray5[] = {0.5f, 0.5f, 0.5f, 1.0f};
static const float MaterialGray6[] = {0.6f, 0.6f, 0.6f, 1.0f};
static const float MaterialGray8[] = {0.8f, 0.8f, 0.8f, 1.0f};

static moebiusstruct *moebius = (moebiusstruct *) NULL;

#define NUM_SCENES      2

static Bool
mySphere(float radius)
{
#if 0
	GLUquadricObj *quadObj;

	if ((quadObj = gluNewQuadric()) == 0)
		return False;
	gluQuadricDrawStyle(quadObj, (GLenum) GLU_FILL);
	gluSphere(quadObj, radius, 16, 16);
	gluDeleteQuadric(quadObj);
#else
    glPushMatrix();
    glScalef (radius, radius, radius);
    unit_sphere (16, 16, False);
    glPopMatrix();
#endif
	return True;
}

static Bool
myCone(float radius)
{
#if 0
	GLUquadricObj *quadObj;

	if ((quadObj = gluNewQuadric()) == 0)
		return False;
	gluQuadricDrawStyle(quadObj, (GLenum) GLU_FILL);
	gluCylinder(quadObj, radius, 0, radius * 3, 8, 1);
	gluDeleteQuadric(quadObj);
#else
    cone (0, 0, 0,
          0, 0, radius * 3,
          radius, 0,
          8, True, True, False);
#endif
	return True;
}

static Bool
draw_moebius_ant(moebiusstruct * mp, const float *Material, int mono)
{
	float       cos1 = COSF(mp->ant_step);
	float       cos2 = COSF(mp->ant_step + 2 * Pi / 3);
	float       cos3 = COSF(mp->ant_step + 4 * Pi / 3);
	float       sin1 = SINF(mp->ant_step);
	float       sin2 = SINF(mp->ant_step + 2 * Pi / 3);
	float       sin3 = SINF(mp->ant_step + 4 * Pi / 3);

	if (mono)
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialGray5);
	else
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, Material);
	glEnable(GL_CULL_FACE);
	glPushMatrix();
	glScalef(1.0f, 1.3f, 1.0f);
	if (!mySphere(0.18f))
		return False;
	glScalef(1.0f, 1.0f / 1.3f, 1.0f);
	glTranslatef(0.00f, 0.30f, 0.00f);
	if (!mySphere(0.2f))
		return False;

	glTranslatef(-0.05f, 0.17f, 0.05f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(-25.0f, 0.0f, 1.0f, 0.0f);
	if (!myCone(0.05f))
		return False;
	glTranslatef(0.00f, 0.10f, 0.00f);
	if (!myCone(0.05f))
		return False;
	glRotatef(25.0f, 0.0f, 1.0f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);

	glScalef(1.0f, 1.3f, 1.0f);
	glTranslatef(0.15f, -0.65f, 0.05f);
	if (!mySphere(0.25f))
		return False;
	glScalef(1.0f, 1.0f / 1.3f, 1.0f);
	glPopMatrix();
	glDisable(GL_CULL_FACE);

	glDisable(GL_LIGHTING);
	/* ANTENNAS */
	glBegin(GL_LINES);
	if (mono)
		glColor3fv(MaterialGray5);
	else
		glColor3fv(Material);
	glVertex3f(0.00f, 0.30f, 0.00f);
	glColor3fv(MaterialGray);
	glVertex3f(0.40f, 0.70f, 0.40f);
	if (mono)
		glColor3fv(MaterialGray5);
	else
		glColor3fv(Material);
	glVertex3f(0.00f, 0.30f, 0.00f);
	glColor3fv(MaterialGray);
	glVertex3f(0.40f, 0.70f, -0.40f);
	glEnd();
	glBegin(GL_POINTS);
	if (mono)
		glColor3fv(MaterialGray6);
	else
		glColor3fv(MaterialRed);
	glVertex3f(0.40f, 0.70f, 0.40f);
	glVertex3f(0.40f, 0.70f, -0.40f);
	glEnd();

	/* LEFT-FRONT ARM */
	glBegin(GL_LINE_STRIP);
	if (mono)
		glColor3fv(MaterialGray5);
	else
		glColor3fv(Material);
	glVertex3f(0.00f, 0.05f, 0.18f);
	glVertex3f(0.35f + 0.05f * cos1, 0.15f, 0.25f);
	glColor3fv(MaterialGray);
	glVertex3f(-0.20f + 0.05f * cos1, 0.25f + 0.1f * sin1, 0.45f);
	glEnd();

	/* LEFT-CENTER ARM */
	glBegin(GL_LINE_STRIP);
	if (mono)
		glColor3fv(MaterialGray5);
	else
		glColor3fv(Material);
	glVertex3f(0.00f, 0.00f, 0.18f);
	glVertex3f(0.35f + 0.05f * cos2, 0.00f, 0.25f);
	glColor3fv(MaterialGray);
	glVertex3f(-0.20f + 0.05f * cos2, 0.00f + 0.1f * sin2, 0.45f);
	glEnd();

	/* LEFT-BACK ARM */
	glBegin(GL_LINE_STRIP);
	if (mono)
		glColor3fv(MaterialGray5);
	else
		glColor3fv(Material);
	glVertex3f(0.00f, -0.05f, 0.18f);
	glVertex3f(0.35f + 0.05f * cos3, -0.15f, 0.25f);
	glColor3fv(MaterialGray);
	glVertex3f(-0.20f + 0.05f * cos3, -0.25f + 0.1f * sin3, 0.45f);
	glEnd();

	/* RIGHT-FRONT ARM */
	glBegin(GL_LINE_STRIP);
	if (mono)
		glColor3fv(MaterialGray5);
	else
		glColor3fv(Material);
	glVertex3f(0.00f, 0.05f, -0.18f);
	glVertex3f(0.35f - 0.05f * sin1, 0.15f, -0.25f);
	glColor3fv(MaterialGray);
	glVertex3f(-0.20f - 0.05f * sin1, 0.25f + 0.1f * cos1, -0.45f);
	glEnd();

	/* RIGHT-CENTER ARM */
	glBegin(GL_LINE_STRIP);
	if (mono)
		glColor3fv(MaterialGray5);
	else
		glColor3fv(Material);
	glVertex3f(0.00f, 0.00f, -0.18f);
	glVertex3f(0.35f - 0.05f * sin2, 0.00f, -0.25f);
	glColor3fv(MaterialGray);
	glVertex3f(-0.20f - 0.05f * sin2, 0.00f + 0.1f * cos2, -0.45f);
	glEnd();

	/* RIGHT-BACK ARM */
	glBegin(GL_LINE_STRIP);
	if (mono)
		glColor3fv(MaterialGray5);
	else
		glColor3fv(Material);
	glVertex3f(0.00f, -0.05f, -0.18f);
	glVertex3f(0.35f - 0.05f * sin3, -0.15f, -0.25f);
	glColor3fv(MaterialGray);
	glVertex3f(-0.20f - 0.05f * sin3, -0.25f + 0.1f * cos3, -0.45f);
	glEnd();

	glBegin(GL_POINTS);
	if (mono)
		glColor3fv(MaterialGray8);
	else
		glColor3fv(MaterialMagenta);
	glVertex3f(-0.20f + 0.05f * cos1, 0.25f + 0.1f * sin1, 0.45f);
	glVertex3f(-0.20f + 0.05f * cos2, 0.00f + 0.1f * sin2, 0.45f);
	glVertex3f(-0.20f + 0.05f * cos3, -0.25f + 0.1f * sin3, 0.45f);
	glVertex3f(-0.20f - 0.05f * sin1, 0.25f + 0.1f * cos1, -0.45f);
	glVertex3f(-0.20f - 0.05f * sin2, 0.00f + 0.1f * cos2, -0.45f);
	glVertex3f(-0.20f - 0.05f * sin3, -0.25f + 0.1f * cos3, -0.45f);
	glEnd();

	glEnable(GL_LIGHTING);

	mp->ant_step += 0.3f;
	return True;
}

static void
RotateAaroundU(float Ax, float Ay, float Az,
	       float Ux, float Uy, float Uz,
	       float *Cx, float *Cy, float *Cz,
	       float Theta)
{
	float       cosO = COSF(Theta);
	float       sinO = SINF(Theta);
	float       one_cosO = 1 - cosO;
	float       Ux2 = sqr(Ux);
	float       Uy2 = sqr(Uy);
	float       Uz2 = sqr(Uz);
	float       UxUy = Ux * Uy;
	float       UxUz = Ux * Uz;
	float       UyUz = Uy * Uz;

	*Cx = (Ux2 + cosO * (1 - Ux2)) * Ax + (UxUy * one_cosO - Uz * sinO) * Ay + (UxUz * one_cosO + Uy * sinO) * Az;
	*Cy = (UxUy * one_cosO + Uz * sinO) * Ax + (Uy2 + cosO * (1 - Uy2)) * Ay + (UyUz * one_cosO - Ux * sinO) * Az;
	*Cz = (UxUz * one_cosO - Uy * sinO) * Ax + (UyUz * one_cosO + Ux * sinO) * Ay + (Uz2 + cosO * (1 - Uz2)) * Az;
}

#define MoebiusDivisions 40
#define MoebiusTransversals 4
static Bool
draw_moebius_strip(ModeInfo * mi)
{
	GLfloat     Phi, Theta;
	GLfloat     cPhi, sPhi;
	moebiusstruct *mp = &moebius[MI_SCREEN(mi)];
	int         i, j;
	int         mono = MI_IS_MONO(mi);

	float       Cx, Cy, Cz;

#ifdef HAVE_JWZGLES /* #### glPolygonMode other than GL_FILL unimplemented */
    solidmoebius = True;
#endif

	if (solidmoebius) {
		glBegin(GL_QUAD_STRIP);
		Phi = 0;
		i = 0;
		while (i < (MoebiusDivisions * 2 + 1)) {
			Theta = Phi / 2;
			cPhi = COSF(Phi);
			sPhi = SINF(Phi);

			i++;
			if (mono)
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialWhite);
			else if (i % 2)
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialRed);
			else
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialGray);

			RotateAaroundU(cPhi, sPhi, 0, -sPhi, cPhi, 0, &Cx, &Cy, &Cz, Theta);
			glNormal3f(Cx, Cy, Cz);
			RotateAaroundU(0, 0, 1, -sPhi, cPhi, 0, &Cx, &Cy, &Cz, Theta);
			glVertex3f(cPhi * 3 + Cx, sPhi * 3 + Cy, +Cz);
			glVertex3f(cPhi * 3 - Cx, sPhi * 3 - Cy, -Cz);

			Phi += (float)(Pi / MoebiusDivisions);
		}
		glEnd();
	} else {
		for (j = -MoebiusTransversals; j < MoebiusTransversals; j++) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glBegin(GL_QUAD_STRIP);
			Phi = 0;
			i = 0;
			while (i < (MoebiusDivisions * 2 + 1)) {
				Theta = Phi / 2;
				cPhi = COSF(Phi);
				sPhi = SINF(Phi);

				RotateAaroundU(cPhi, sPhi, 0, -sPhi, cPhi, 0, &Cx, &Cy, &Cz, Theta);
				glNormal3f(Cx, Cy, Cz);
				RotateAaroundU(0, 0, 1, -sPhi, cPhi, 0, &Cx, &Cy, &Cz, Theta);
				j++;
				if (j == MoebiusTransversals || mono)
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialWhite);
				else if (i % 2)
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialRed);
				else
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialGray);
				glVertex3f(cPhi * 3 + Cx / MoebiusTransversals * j, sPhi * 3 + Cy / MoebiusTransversals * j, +Cz / MoebiusTransversals * j);
				j--;
				if (j == -MoebiusTransversals || mono)
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialWhite);
				else if (i % 2)
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialRed);
				else
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialGray);
				glVertex3f(cPhi * 3 + Cx / MoebiusTransversals * j, sPhi * 3 + Cy / MoebiusTransversals * j, +Cz / MoebiusTransversals * j);

				Phi += (float)(Pi / MoebiusDivisions);
				i++;
			}
			glEnd();
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (drawants) {
		/* DRAW BLUE ANT */
		glPushMatrix();
		glRotatef(mp->ant_position + 180.0f, 0.0f, 0.0f, 1.0f);
		glTranslatef(3.0f, 0.0f, 0.0f);
		glRotatef(mp->ant_position / 2 + 90, 0.0f, 1.0f, 0.0f);
		glTranslatef(0.28f, 0.0f, -0.45f);
		if (!draw_moebius_ant(mp, MaterialYellow, mono))
			return False;
		glPopMatrix();

		/* DRAW YELLOW ANT */
		glPushMatrix();
		glRotatef(mp->ant_position, 0.0f, 0.0f, 1.0f);
		glTranslatef(3, 0, 0);
		glRotatef(mp->ant_position / 2.0f, 0.0f, 1.0f, 0.0f);
		glTranslatef(0.28f, 0.0f, -0.45f);
		if (!draw_moebius_ant(mp, MaterialBlue, mono))
			return False;
		glPopMatrix();

		/* DRAW GREEN ANT */
		glPushMatrix();
		glRotatef(-mp->ant_position, 0.0f, 0.0f, 1.0f);
		glTranslatef(3, 0, 0);
		glRotatef(-mp->ant_position / 2.0f, 0.0f, 1.0f, 0.0f);
		glTranslatef(0.28f, 0.0f, 0.45f);
		glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
		if (!draw_moebius_ant(mp, MaterialGreen, mono))
			return False;
		glPopMatrix();

		/* DRAW CYAN ANT */
		glPushMatrix();
		glRotatef(-mp->ant_position + 180.0f, 0.0f, 0.0f, 1.0f);
		glTranslatef(3.0f, 0.0f, 0.0f);
		glRotatef(-mp->ant_position / 2.0f + 90.0f, 0.0f, 1.0f, 0.0f);
		glTranslatef(0.28f, 0.0f, 0.45f);
		glRotatef(180, 1, 0, 0);
		if (!draw_moebius_ant(mp, MaterialCyan, mono))
			return False;
		glPopMatrix();
	}
	mp->ant_position += 1;
	return True;
}
#undef MoebiusDivisions
#undef MoebiusTransversals

ENTRYPOINT void
reshape_moebius (ModeInfo * mi, int width, int height)
{
	moebiusstruct *mp = &moebius[MI_SCREEN(mi)];

	glViewport(0, 0, mp->WindW = (GLint) width, mp->WindH = (GLint) height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0, 1.0, -1.0, 1.0, 5.0, 15.0);
	glMatrixMode(GL_MODELVIEW);
	if (width >= 1024) {
		glLineWidth(3);
		glPointSize(3);
	} else if (width >= 512) {
		glLineWidth(2);
		glPointSize(2);
	} else {
		glLineWidth(1);
		glPointSize(1);
	}
}

static void
pinit(ModeInfo *mi)
{
  /* int status; */
	glClearDepth(1.0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position0);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, position1);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_NORMALIZE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	/* moebius */
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

#if 0
	glEnable(GL_TEXTURE_2D);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

#if 0
    clear_gl_error();
	status = gluBuild2DMipmaps(GL_TEXTURE_2D, 3,
                               WoodTextureWidth, WoodTextureHeight,
                               GL_RGB, GL_UNSIGNED_BYTE, WoodTextureData);
    if (status)
      {
        const char *s = (char *) gluErrorString (status);
        fprintf (stderr, "%s: error mipmapping %dx%d texture: %s\n",
                 progname, WoodTextureWidth, WoodTextureHeight,
                 (s ? s : "(unknown)"));
        exit (1);
      }
    check_gl_error("mipmapping");
#else
    {
      XImage *img = xpm_to_ximage (mi->dpy,
                                   mi->xgwa.visual,
                                   mi->xgwa.colormap,
                                   wood_texture);
	  glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA,
                    img->width, img->height, 0,
                    GL_RGBA,
                    /* GL_UNSIGNED_BYTE, */
                    GL_UNSIGNED_INT_8_8_8_8_REV,
                    img->data);
      check_gl_error("texture");
      XDestroyImage (img);
    }
#endif

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
#endif

	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, front_shininess);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, front_specular);
}



ENTRYPOINT void
release_moebius (ModeInfo * mi)
{
	if (moebius != NULL) {
		(void) free((void *) moebius);
		moebius = (moebiusstruct *) NULL;
	}
	FreeAllGL(mi);
}

#if 0
	ENTRYPOINT Bool
	moebius_handle_event (ModeInfo *mi, XEvent *event)
	{
	  moebiusstruct *mp = &moebius[MI_SCREEN(mi)];

	  if (event->xany.type == ButtonPress &&
		  event->xbutton.button == Button1)
		{
		  mp->button_down_p = True;
		  gltrackball_start (mp->trackball,
							 event->xbutton.x, event->xbutton.y,
							 MI_WIDTH (mi), MI_HEIGHT (mi));
		  return True;
		}
	  else if (event->xany.type == ButtonRelease &&
			   event->xbutton.button == Button1)
		{
		  mp->button_down_p = False;
		  return True;
		}
	  else if (event->xany.type == ButtonPress &&
			   (event->xbutton.button == Button4 ||
				event->xbutton.button == Button5 ||
				event->xbutton.button == Button6 ||
				event->xbutton.button == Button7))
		{
		  gltrackball_mousewheel (mp->trackball, event->xbutton.button, 10,
								  !!event->xbutton.state);
		  return True;
		}
	  else if (event->xany.type == MotionNotify &&
			   mp->button_down_p)
		{
		  gltrackball_track (mp->trackball,
							 event->xmotion.x, event->xmotion.y,
							 MI_WIDTH (mi), MI_HEIGHT (mi));
		  return True;
		}

	  return False;
	}
#endif

ENTRYPOINT void
init_moebius (ModeInfo * mi)
{
	moebiusstruct *mp;

	if (moebius == NULL) {
		if ((moebius = (moebiusstruct *) calloc(MI_NUM_SCREENS(mi),
					    sizeof (moebiusstruct))) == NULL)
			return;
	}
	mp = &moebius[MI_SCREEN(mi)];
	mp->step = (float)NRAND(90);
	mp->ant_position = (float)NRAND(90);

    {
      double rot_speed = 0.3;
      mp->rot = make_rotator (rot_speed, rot_speed, rot_speed, 1, 0, True);
      //mp->trackball = gltrackball_init ();
    }

	if ((mp->hglrc = init_GL(mi)) != NULL) {

		reshape_moebius(mi, MI_WIDTH(mi), MI_HEIGHT(mi));
		glDrawBuffer(GL_BACK);
		pinit(mi);
	} else {
		MI_CLEARWINDOW(mi);
	}
}

ENTRYPOINT void
draw_moebius (ModeInfo * mi)
{
	moebiusstruct *mp;

	HDC display = MI_DISPLAY(mi);
	HWND window = MI_WINDOW(mi);

        if (moebius == NULL)
	    return;
	mp = &moebius[MI_SCREEN(mi)];

	MI_IS_DRAWN(mi) = True;

	if (!mp->hglrc)
		return;

	wglMakeCurrent(display, mp->hglrc);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();

	glTranslatef(0.0f, 0.0f, -10.0f);

    /* Do it twice because we don't track the device's orientation. */
    //glRotatef( current_device_rotation(), 0, 0, 1);
    glRotatef( 0.0f, 0.0f, 0.0f, 1.0f);
    //gltrackball_rotate (mp->trackball);
    //glRotatef(-current_device_rotation(), 0, 0, 1);
    glRotatef(-0.0f, 0.0f, 0.0f, 1.0f);

	if (!MI_IS_ICONIC(mi)) {
		glScalef((float)(Scale4Window * mp->WindH / mp->WindW), (float)Scale4Window, (float)Scale4Window);
	} else {
		glScalef((float)(Scale4Iconic * mp->WindH / mp->WindW), (float)Scale4Iconic, (float)Scale4Iconic);
	}

    {
      double x, y, z;
      //get_rotation (mp->rot, &x, &y, &z, !mp->button_down_p);
      get_rotation (mp->rot, &x, &y, &z, !False);
      glRotatef((float)(x * 360.0f), 1.0f, 0.0f, 0.0f);
      glRotatef((float)(y * 360.0f), 0.0f, 1.0f, 0.0f);
      glRotatef((float)(z * 360.0f), 0.0f, 0.0f, 1.0f);
    }

	/* moebius */
	if (!draw_moebius_strip(mi)) {
		release_moebius(mi);
		return;
	}

	glPopMatrix();

    if (MI_IS_FPS(mi)) do_fps (mi);
	glFlush();

	SwapBuffers(display);

	mp->step += (float)0.025;
}

#ifndef STANDALONE
ENTRYPOINT void
change_moebius (ModeInfo * mi)
{
	moebiusstruct *mp = &moebius[MI_SCREEN(mi)];

	if (!mp->hglrc)
		return;

	wglMakeCurrent(MI_DISPLAY(mi), mp->hglrc);
	pinit(mi);
}
#endif /* !STANDALONE */


XSCREENSAVER_MODULE ("Moebius", moebius)

#endif
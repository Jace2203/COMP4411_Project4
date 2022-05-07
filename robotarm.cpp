// The sample model.  You should build a file
// very similar to this for when you make your model.
#pragma warning (disable : 4305)
#pragma warning (disable : 4244)
#pragma warning(disable : 4786)

// #include "modelerview.h"
// #include "modelerapp.h"
// #include "modelerdraw.h"
#include "particleSystem.h"

#include <FL/gl.h>
#include <stdlib.h>
#include <cmath>

#define M_DEFAULT 2.0f
#define M_OFFSET 3.0f
#define P_OFFSET 0.3f
#define MAX_VEL 200
#define MIN_STEP 0.1

#include "modelerview.h"
#include "modelerapp.h"
#include "modelerdraw.h"
#include <FL/gl.h>

#include "modelerglobals.h"
#include "complexshape.h"
#include "drawbody.h"
#include "camera.h"
#include "metaball.h"
#include "ik.h"
#include "billboard.h"

#include "cmath"
#include "bitmap.h"

static GLfloat lightPosition0[] = { 4, 2, -4, 0 };
static GLfloat lightDiffuse0[]  = { 1,1,1,1 };


static GLfloat lightPosition1[] = {  5, 5, 5, 0 };
// static GLfloat lightPosition1[] = {  10,  10,  10, 0 };
static GLfloat lightPosition2[] = { -5, -5, -5, 0 };


class Dot;

// To make a SampleModel, we inherit off of ModelerView
class SampleModel : public ModelerView 
{
	double**	limits_L;
	double**	limits_R;
	double*		length;

	double*		angles_L;
	double*		angles_R;
	Vec3d*		last_target_L;
	Vec3d*		last_target_R;

	MetaballContainer* metaball_container[2];

	Dot** draw_pts = nullptr;
	Dot** torus = nullptr;

	Billboard* snow = nullptr;
	Billboard* right_questionmark = nullptr;

	std::vector<unsigned char*> lastimage;
public:
    SampleModel(int x, int y, int w, int h, char *label) 
    : ModelerView(x,y,w,h,label)
	{
		for (int i = 0; i < 2; i++)
			metaball_container[i] = new MetaballContainer(3, 1, 0.02);
		
		metaball_container[0]->append(new Metaball(Vec3d(0, 0, 0), 0.2));
		metaball_container[0]->append(new Metaball(Vec3d(0.12, 0, 0), 0.16, META_INVERSE));
		metaball_container[0]->append(new Metaball(Vec3d(-0.2, 0, 0), 0.28, META_INVERSE));
		
		metaball_container[1]->append(new Metaball(Vec3d(0, 0, 0), 0.2));
		metaball_container[1]->append(new Metaball(Vec3d(-0.12, 0, 0), 0.16, META_INVERSE));
		metaball_container[1]->append(new Metaball(Vec3d(0.2, 0, 0), 0.28, META_INVERSE));
		
		limits_L = new double*[3];
		limits_R = new double*[3];
		for (int i = 0; i < 3; i++)
		{
			limits_L[i] = new double[2];
			limits_R[i] = new double[2];
		}

		limits_L[0][0] = -60;
		limits_L[0][1] = 50;
		limits_R[0][0] = -60;
		limits_R[0][1] = 50;
		
		limits_L[1][0] = -90;
		limits_L[1][1] = 0;
		limits_R[1][0] = 0;
		limits_R[1][1] = 90;
		
		limits_L[2][0] = 0;
		limits_L[2][1] = 140;
		limits_R[2][0] = 0;
		limits_R[2][1] = 140;

		last_target_L = new Vec3d(VAL(L_TARGET_X), VAL(L_TARGET_Y) - 0.9962, VAL(L_TARGET_Z));
		last_target_R = new Vec3d(VAL(R_TARGET_X), VAL(R_TARGET_Y) - 0.9962, VAL(R_TARGET_Z));

		length = new double[2];
		length[0] = 0.5;
		length[1] = 0.5;

		angles_L = new double[3];
		angles_L[0] = VAL(L_THIGH_XROT);
		angles_L[1] = VAL(L_THIGH_YROT);
		angles_L[2] = VAL(L_LEG_XROT);

		angles_R = new double[3];
		angles_R[0] = VAL(R_THIGH_XROT);
		angles_R[1] = VAL(R_THIGH_YROT);
		angles_R[2] = VAL(R_LEG_XROT);

		snow = new Billboard("texture/snow.png");
		right_questionmark = new Billboard("texture/right.png");
	}

	~SampleModel()
	{
		for (int i = 0; i < 3; i++)
		{
			delete[] limits_L[i];
			delete[] limits_R[i];
		}

		for (int i = 0; i < 2; i++)
			delete[] metaball_container[i];

		delete[] limits_L;
		delete[] limits_R;
		delete[] length;

		delete[] angles_L;
		delete[] angles_R;

		delete last_target_L;
		delete last_target_R;

		deleteTexture();

		for(int i = 0; i < 21; ++i)
		{
			if (draw_pts[i]) delete[] draw_pts[i];
			if (torus[i]) delete[] torus[i];
		}

		for(auto it = lastimage.begin(); it != lastimage.end(); ++it)
			delete []*it;
	}

    virtual void draw();
};

// We need to make a creator function, mostly because of
// nasty API stuff that we'd rather stay away from.
ModelerView* createSampleModel(int x, int y, int w, int h, char *label)
{ 
    return new SampleModel(x,y,w,h,label); 
}

double ani_height = -(head_size + torso_height)/5,
	   cur_height = 0,
	   chg_height = 0,
	   ani_theta = acos(1+ani_height)*180/M_PI,
	   cur_theta = 0,
	   chg_theta = 0,
	   ani_angle  = 15,
	   cur_angle  = ani_angle,
	   chg_angle  = 0,
	   ani_wave   = 45,
	   cur_wave   = ani_wave,
	   chg_wave   = 0,
	   time_wave  = 5;

// We are going to override (is that the right word?) the draw()
// method of ModelerView to draw out SampleModel

void SampleModel::draw()
{
	glDisable(GL_STENCIL_TEST);
	Initial(metaball_container[1], metaball_container[0]);

	ModelerView::draw();
	while (particle_spawn.begin() != particle_spawn.end())
	{
		delete *(particle_spawn.begin());
		particle_spawn.erase(particle_spawn.begin());
	}

	lightPosition0[0] = VAL(LIGHTX);
	lightPosition0[1] = VAL(LIGHTY);
	lightPosition0[2] = VAL(LIGHTZ);

	glLightfv( GL_LIGHT0, GL_POSITION, lightPosition1 );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, lightDiffuse0 );

	glLightfv( GL_LIGHT1, GL_POSITION, lightPosition2 );
	glLightfv( GL_LIGHT1, GL_DIFFUSE, lightDiffuse0 );

	lightDiffuse0[0] = VAL(LIGHTR);
	lightDiffuse0[1] = VAL(LIGHTG);
	lightDiffuse0[2] = VAL(LIGHTB);
	
	// glLightfv( GL_LIGHT0, GL_POSITION, lightPosition0 );
    // glLightfv( GL_LIGHT0, GL_DIFFUSE, lightDiffuse0 );

	// glTranslated(10, -10, 10);
	// drawSkyBox(20, 20, 20);

	int size = 70;
	int mirror_x = 7;

	if (VAL(APPLY_IK))
	{
		if (*last_target_L != Vec3d(VAL(L_TARGET_X), VAL(L_TARGET_Y) - 0.9962, VAL(L_TARGET_Z)))
		{
			angles_L[0] = VAL(L_THIGH_XROT);
			angles_L[1] = VAL(L_THIGH_YROT);
			angles_L[2] = VAL(L_LEG_XROT);

			(*last_target_L)[0] = VAL(L_TARGET_X);
			(*last_target_L)[1] = VAL(L_TARGET_Y) - 0.9962;
			(*last_target_L)[2] = VAL(L_TARGET_Z);
			// std::cout << "L " << *last_target_L << std::endl;

			LegIK(length, angles_L, limits_L, *last_target_L);
			angles_L[1] *= -1;
		}

		if (*last_target_R != Vec3d(VAL(R_TARGET_X), VAL(R_TARGET_Y) - 0.9962, VAL(R_TARGET_Z)))
		{
			angles_R[0] = VAL(R_THIGH_XROT);
			angles_R[1] = VAL(R_THIGH_YROT);
			angles_R[2] = VAL(R_LEG_XROT);

			(*last_target_R)[0] = VAL(R_TARGET_X);
			(*last_target_R)[1] = VAL(R_TARGET_Y) - 0.9962;
			(*last_target_R)[2] = VAL(R_TARGET_Z);

			LegIK(length, angles_R, limits_R, *last_target_R);
		}
	}

	if (false)
	{
		if (cur_height <= ani_height)
		{
			chg_height = -ani_height /15;
			chg_theta = -ani_theta /15;
		}
		else if (cur_height >= 0)
		{
			chg_height = ani_height /15;
			chg_theta = ani_theta /15;

			if (chg_angle >= 0)
				chg_angle = -ani_angle /15;
			else
				chg_angle = ani_angle /15;
		}

		// if (cur_height >= 0 && chg_angle >= 0)
		// 	chg_angle = -ani_angle /15;
		// else if (cur_height >= 0 && chg_angle <= 0)
		// 	chg_angle = ani_angle /15;

		if (cur_wave >= ani_wave)
			chg_wave = -ani_wave * time_wave /15;
		else if (cur_wave <= -ani_wave)
			chg_wave = ani_wave * time_wave /15;

		cur_height += chg_height;
		cur_angle += chg_angle;
		cur_wave += chg_wave;
		cur_theta += chg_theta;
	}

	// m_camera->setModelTorso(Vec3f(VAL(XPOS), VAL(YPOS), VAL(ZPOS)));
	// (*metaball_container)[1]->setCenter(Vec3d(VAL(XPOS), 0, 0));
	// metaball_container->render();

	// Real World
	glPushMatrix();
		glTranslated(- size / 2, - size / 2, - size / 2);
		drawSkyBox(size, size, size);
	glPopMatrix();

	float currentColor[4];
    glGetFloatv(GL_CURRENT_COLOR,currentColor);
		setDiffuseColor(0.0, 0.0, 1.0);
		glPushMatrix();
			glScaled(10, 1, 10);
			glTranslated(-0.5, -1.2, -0.5);
			callList(HF_ID);
		glPopMatrix();
    setDiffuseColor(currentColor[0], currentColor[1], currentColor[2]);

	glPushMatrix();
		glTranslated(VAL(XPOS), VAL(YPOS), VAL(ZPOS));
		// int lod = int(VAL(LOD));
		int lod = 4;

		
		// if (ModelerApplication::Instance()->IsAnimating())
		// 	glRotated(cur_angle, 0, 1, 0);
			
		// Torso
		glPushMatrix();
			glRotated(-90.0, 1.0, 0.0, 0.0);
			glTranslated(0, 0, cur_height);

			// drawTorso();
			callList(TORSO_ID);

			if (lod > 0)
			{
				// drawHead();
				callList(HEAD_ID);

				// double LZ = (ModelerApplication::Instance()->IsAnimating()) ? cur_wave : (VAL(MOOD) == 1) ? -45.0: (VAL(MOOD) == 2) ?  45.0: VAL(L_UPPER_ARM_ZROT),
				// 	   RZ = (ModelerApplication::Instance()->IsAnimating()) ? cur_wave : (VAL(MOOD) == 1) ?  45.0: (VAL(MOOD) == 2) ? -45.0: VAL(R_UPPER_ARM_ZROT),
				double LZ = (0) ? cur_wave : (VAL(MOOD) == 1) ? -45.0: (VAL(MOOD) == 2) ?  45.0: VAL(L_UPPER_ARM_ZROT),
					   RZ = (0) ? cur_wave : (VAL(MOOD) == 1) ?  45.0: (VAL(MOOD) == 2) ? -45.0: VAL(R_UPPER_ARM_ZROT),
					   LY = (VAL(MOOD) == 1) ? 135.0: (VAL(MOOD) == 2) ? 50: VAL(L_UPPER_ARM_YROT),
					   RY = (VAL(MOOD) == 1) ? 135.0: (VAL(MOOD) == 2) ? 50: VAL(R_UPPER_ARM_YROT),
					   LUX = (VAL(MOOD) == 1) ? 0 : (VAL(MOOD) == 2) ? -22.5: VAL(L_UPPER_ARM_XROT),
					   RUX = (VAL(MOOD) == 1) ? 0 : (VAL(MOOD) == 2) ? -22.5: VAL(R_UPPER_ARM_XROT),
					   LLAX = (VAL(MOOD) == 2) ? 95.0: VAL(L_LOWER_ARM_XROT),
					   RLAX = (VAL(MOOD) == 2) ? 95.0: VAL(R_LOWER_ARM_XROT);

				// if (VAL(SHOW_TORUS))
				if (true)
					drawtorus(&torus, 21, LY, LUX);

				drawArmL(LY, LZ, LLAX, LUX, metaball_container[1], lod - 1);
				drawArmR(RY, RZ, RLAX, RUX, metaball_container[0], lod - 1);

				// if (ModelerApplication::Instance()->IsAnimating())
				if (false)
				{
					double LTX = -cur_theta,
						   RTX = -cur_theta,
						   LLX = 2*cur_theta, 
						   RLX = 2*cur_theta;

					particle_spawn.push_back(drawLegL(LTX, VAL(L_THIGH_YROT), LLX, lod - 1));
					particle_spawn.push_back(drawLegR(RTX, VAL(R_THIGH_YROT), RLX, lod - 1));
				}
				else if (VAL(APPLY_IK))
				{
					particle_spawn.push_back(drawLegL(angles_L[0], angles_L[1], angles_L[2], lod - 1));
					particle_spawn.push_back(drawLegR(angles_R[0], angles_R[1], angles_R[2], lod - 1));
				}
				else
				{
					double LTX = (VAL(MOOD) == 1) ? 25.0: VAL(L_THIGH_XROT),
						   RTX = (VAL(MOOD) == 1) ? 25.0: VAL(R_THIGH_XROT),
						   LLX = (VAL(MOOD) == 1) ? 75.0: VAL(L_LEG_XROT),
						   RLX = (VAL(MOOD) == 1) ? 75.0: VAL(R_LEG_XROT);

					particle_spawn.push_back(drawLegL(LTX, VAL(L_THIGH_YROT), LLX, lod - 1));
					particle_spawn.push_back(drawLegR(RTX, VAL(R_THIGH_YROT), RLX, lod - 1));
				}

				setDiffuseColor(50.0/255, 75.0/255, 100.0/255);
				drawEquipment(VAL(BACK_YROT), VAL(L_EQUIP_YROT), VAL(R_EQUIP_YROT), VAL(L_TURRET_YROT), VAL(R_TURRET_YROT), VAL(L_TURRET_XROT), VAL(R_TURRET_XROT), VAL(TURRET_NUM), lod);
				setDiffuseColor(1, 1, 1);
			}

			if (lod > 1)
			{
				setDiffuseColor(50.0/255, 75.0/255, 100.0/255);
				drawCurve(&draw_pts, 21, VAL(BACK_YROT));
				if (VAL(DLS))
					drawLsystem(VAL(IT), VAL(DV), VAL(IA), VAL(AOI), VAL(BACK_YROT));
				setDiffuseColor(1, 1, 1);
			}

		glPopMatrix();
	glPopMatrix();
	ModelerView::drawPs();
	
	glPushMatrix();
		glTranslated(-5, 0, 0);
		snow->setTexture();
		snow->draw(1.0f);
	glPopMatrix();
	
	glPushMatrix();
		glTranslated(-5, 3, 0);
		right_questionmark->setTexture();
		right_questionmark->draw(2.0f);
	glPopMatrix();

	// Mirror
	glStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );
	glStencilFunc( GL_ALWAYS, 1, 1 );
	glEnable( GL_STENCIL_TEST );

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glBegin(GL_TRIANGLES);
		glVertex3f(mirror_x, -1.0, -5.0);
		glVertex3f(mirror_x, -1.0,  5.0);
		glVertex3f(mirror_x,  4.0,  5.0);

		glVertex3f(mirror_x,  4.0,  5.0);
		glVertex3f(mirror_x,  4.0, -5.0);
		glVertex3f(mirror_x, -1.0, -5.0);
	glEnd();
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilFunc(GL_NOTEQUAL, 0, 1);
	
	glClear( GL_DEPTH_BUFFER_BIT );
	
	// Mirror World
	glPushMatrix();
		glTranslated(mirror_x, 0.0, 0.0);
		glScaled(-1.0, 1.0, 1.0);
		glTranslated(-mirror_x, 0.0, 0.0);
		
		{
			glPushMatrix();
				glTranslated(- size / 2, - size / 2, - size / 2);
				drawSkyBox(size, size, size);
			glPopMatrix();

			glGetFloatv(GL_CURRENT_COLOR,currentColor);
				setDiffuseColor(0.0, 0.0, 1.0);
				glPushMatrix();
					glScaled(10, 1, 10);
					glTranslated(-0.5, -1.2, -0.5);
					callList(HF_ID);
				glPopMatrix();
			setDiffuseColor(currentColor[0], currentColor[1], currentColor[2]);

			glPushMatrix();
				glTranslated(VAL(XPOS), VAL(YPOS), VAL(ZPOS));
				// int lod = int(VAL(LOD));
				int lod = 4;
				
				// if (ModelerApplication::Instance()->IsAnimating())
				// 	glRotated(cur_angle, 0, 1, 0);
					
				// Torso
				glPushMatrix();
					glRotated(-90.0, 1.0, 0.0, 0.0);
					glTranslated(0, 0, cur_height);

					// drawTorso();
					callList(TORSO_ID);

					if (lod > 0)
					{
						// drawHead();
						callList(HEAD_ID);

						// double LZ = (ModelerApplication::Instance()->IsAnimating()) ? cur_wave : (VAL(MOOD) == 1) ? -45.0: (VAL(MOOD) == 2) ?  45.0: VAL(L_UPPER_ARM_ZROT),
						// 	   RZ = (ModelerApplication::Instance()->IsAnimating()) ? cur_wave : (VAL(MOOD) == 1) ?  45.0: (VAL(MOOD) == 2) ? -45.0: VAL(R_UPPER_ARM_ZROT),
						double LZ = (0) ? cur_wave : (VAL(MOOD) == 1) ? -45.0: (VAL(MOOD) == 2) ?  45.0: VAL(L_UPPER_ARM_ZROT),
							RZ = (0) ? cur_wave : (VAL(MOOD) == 1) ?  45.0: (VAL(MOOD) == 2) ? -45.0: VAL(R_UPPER_ARM_ZROT),
							LY = (VAL(MOOD) == 1) ? 135.0: (VAL(MOOD) == 2) ? 50: VAL(L_UPPER_ARM_YROT),
							RY = (VAL(MOOD) == 1) ? 135.0: (VAL(MOOD) == 2) ? 50: VAL(R_UPPER_ARM_YROT),
							LUX = (VAL(MOOD) == 1) ? 0 : (VAL(MOOD) == 2) ? -22.5: VAL(L_UPPER_ARM_XROT),
							RUX = (VAL(MOOD) == 1) ? 0 : (VAL(MOOD) == 2) ? -22.5: VAL(R_UPPER_ARM_XROT),
							LLAX = (VAL(MOOD) == 2) ? 95.0: VAL(L_LOWER_ARM_XROT),
							RLAX = (VAL(MOOD) == 2) ? 95.0: VAL(R_LOWER_ARM_XROT);

						// if (VAL(SHOW_TORUS))
						if (true)
							drawtorus(&torus, 21, LY, LUX);

						drawArmL(LY, LZ, LLAX, LUX, metaball_container[1], lod - 1);
						drawArmR(RY, RZ, RLAX, RUX, metaball_container[0], lod - 1);

						// if (ModelerApplication::Instance()->IsAnimating())
						if (false)
						{
							double LTX = -cur_theta,
								RTX = -cur_theta,
								LLX = 2*cur_theta, 
								RLX = 2*cur_theta;

							particle_spawn.push_back(drawLegL(LTX, VAL(L_THIGH_YROT), LLX, lod - 1));
							particle_spawn.push_back(drawLegR(RTX, VAL(R_THIGH_YROT), RLX, lod - 1));
						}
						else if (VAL(APPLY_IK))
						{
							particle_spawn.push_back(drawLegL(angles_L[0], angles_L[1], angles_L[2], lod - 1));
							particle_spawn.push_back(drawLegR(angles_R[0], angles_R[1], angles_R[2], lod - 1));
						}
						else
						{
							double LTX = (VAL(MOOD) == 1) ? 25.0: VAL(L_THIGH_XROT),
								RTX = (VAL(MOOD) == 1) ? 25.0: VAL(R_THIGH_XROT),
								LLX = (VAL(MOOD) == 1) ? 75.0: VAL(L_LEG_XROT),
								RLX = (VAL(MOOD) == 1) ? 75.0: VAL(R_LEG_XROT);

							particle_spawn.push_back(drawLegL(LTX, VAL(L_THIGH_YROT), LLX, lod - 1));
							particle_spawn.push_back(drawLegR(RTX, VAL(R_THIGH_YROT), RLX, lod - 1));
						}

						setDiffuseColor(50.0/255, 75.0/255, 100.0/255);
						drawEquipment(VAL(BACK_YROT), VAL(L_EQUIP_YROT), VAL(R_EQUIP_YROT), VAL(L_TURRET_YROT), VAL(R_TURRET_YROT), VAL(L_TURRET_XROT), VAL(R_TURRET_XROT), VAL(TURRET_NUM), lod);
						setDiffuseColor(1, 1, 1);
					}

					if (lod > 1)
					{
						setDiffuseColor(50.0/255, 75.0/255, 100.0/255);
						drawCurve(&draw_pts, 21, VAL(BACK_YROT));
						if (VAL(DLS))
							drawLsystem(VAL(IT), VAL(DV), VAL(IA), VAL(AOI), VAL(BACK_YROT));
						setDiffuseColor(1, 1, 1);
					}

				glPopMatrix();
			glPopMatrix();

			ModelerView::drawPs();
	
			glPushMatrix();
				glTranslated(-5, 0, 0);
				snow->setTexture();
				snow->draw(1.0f);
			glPopMatrix();

			glPushMatrix();
				glTranslated(-5, 3, 0);
				right_questionmark->setTexture();
				right_questionmark->draw(2.0f);
			glPopMatrix();
		}
		glPopMatrix();
	glPopMatrix();

	glDisable(GL_STENCIL_TEST);

	int xx = x();
	int yy = y();
	int ww = w();
	int hh = h();

	unsigned char *temp = new unsigned char[3 * ModelerView::w() * ModelerView::h()];

	glReadBuffer(GL_BACK);

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_ROW_LENGTH, ww);
    
    glReadPixels( 0, 0, ww, hh, 
		GL_RGB, GL_UNSIGNED_BYTE, 
		temp );

	lastimage.push_back(temp);

	int image_num = 5;

	if (lastimage.size() >= image_num)
	{
		while (lastimage.size() > image_num)
		{
			delete []lastimage.front();
			lastimage.erase(lastimage.begin());
		}

		if (ModelerApplication::Instance()->GetMotionBlur())
		{
			temp = new unsigned char[3 * ModelerView::w() * ModelerView::h()]{};
			for(auto it = 0; it != image_num; ++it)
			{
				for(int i = 0; i < 3 * ModelerView::w() * ModelerView::h(); ++i)
					temp[i] += *(lastimage[it] + i) * 1.0 / image_num;
			}
			
			glDrawPixels( ww, 
				hh, 
				GL_RGB, 
				GL_UNSIGNED_BYTE, 
				temp);

			delete []temp;
		}
	}

	endDraw();
}

int main()
{
	// Initialize the controls
	// Constructor is ModelerControl(name, minimumvalue, maximumvalue, 
	// stepsize, defaultvalue)

    ModelerControl controls[NUMCONTROLS];

	// LOD
	controls[LOD] = ModelerControl("Change Level of Detail", 0, 4, 1, 4);
	
	// Light
	controls[LIGHTX] = ModelerControl("Light X", -10, 10, 1, 4);
	controls[LIGHTY] = ModelerControl("Light Y", -10, 10, 1, 2);
	controls[LIGHTZ] = ModelerControl("Light Z", -10, 10, 1, 4);

	controls[LIGHTR] = ModelerControl("Light R", 0, 1, 0.01, 1);
	controls[LIGHTG] = ModelerControl("Light G", 0, 1, 0.01, 1);
	controls[LIGHTB] = ModelerControl("Light B", 0, 1, 0.01, 1);

	// L system
	controls[DLS] = ModelerControl("Display L-system", 0, 1, 1, 0);
	controls[DV] = ModelerControl("D-value", 0, 1, 0.01, 0.1);
	controls[IA] = ModelerControl("Initial Angle", 0, 360, 1, 90);
	controls[AOI] = ModelerControl("Angle of Increment", 0, 360, 1, 30);
	controls[IT] = ModelerControl("Iteration", 0, 6, 1, 4);

	// Whole body
    controls[XPOS] = ModelerControl("X Position", -5, 5, 0.1f, 0);
    controls[YPOS] = ModelerControl("Y Position", 0, 5, 0.1f, 0);
    controls[ZPOS] = ModelerControl("Z Position", -5, 5, 0.1f, 0);

	// Arm
	controls[L_UPPER_ARM_XROT] = ModelerControl("Left Upper Arm X Rotation", -45, 180, 0.1f, 0);
	controls[L_UPPER_ARM_YROT] = ModelerControl("Left Upper Arm Y Rotation", 10, 135, 0.1f, 10);
	controls[L_UPPER_ARM_ZROT] = ModelerControl("Left Upper Arm Z Rotation", -45, 45, 0.1f, 0);
	controls[R_UPPER_ARM_XROT] = ModelerControl("Right Upper Arm X Rotation", -45, 180, 0.1f, 0);
	controls[R_UPPER_ARM_YROT] = ModelerControl("Right Upper Arm Y Rotation", 10, 135, 0.1f, 10);
	controls[R_UPPER_ARM_ZROT] = ModelerControl("Right Upper Arm Z Rotation", -45, 45, 0.1f, 0);

	controls[L_LOWER_ARM_XROT] = ModelerControl("Left Lower Arm X Rotation", 10, 135, 0.1f, 45);
	controls[R_LOWER_ARM_XROT] = ModelerControl("Right Lower Arm X Rotation", 10, 135, 0.1f, 45);

	// Leg
	controls[L_THIGH_XROT] = ModelerControl("Left Thigh X Rotation", -60, 50, 0.1f, -5);
	controls[R_THIGH_XROT] = ModelerControl("Right Thigh X Rotation", -60, 50, 0.1f, -5);
	controls[L_THIGH_YROT] = ModelerControl("Left Thigh Y Rotation", 0, 90, 0.1f, 0);
	controls[R_THIGH_YROT] = ModelerControl("Right Thigh Y Rotation", 0, 90, 0.1f, 0);
	controls[L_LEG_XROT] = ModelerControl("Left Leg X Rotation", 0, 140, 0.1f, 10);
	controls[R_LEG_XROT] = ModelerControl("Right Leg X Rotation", 0, 140, 0.1f, 10);

	// Equipment
	controls[BACK_YROT] = ModelerControl("Back Equipment Y Rotation", -15, 15, 0.1f, 0);
	controls[L_EQUIP_YROT] = ModelerControl("Left Equipment Y Rotation", -15, 15, 0.1f, 0);
	controls[R_EQUIP_YROT] = ModelerControl("Right Equipment Y Rotation", -15, 15, 0.1f, 0);

	// Turret
	controls[L_TURRET_YROT] = ModelerControl("Left Turret Y Rotation", -30, 225, 1, 0);
	controls[R_TURRET_YROT] = ModelerControl("Right Turret Y Rotation", -30, 225, 1, 0);

	controls[L_TURRET_XROT] = ModelerControl("Left Turret X Rotation", 10, 85, 1, 36);
	controls[R_TURRET_XROT] = ModelerControl("Right Turret X Rotation", 10, 85, 1, 36);

	controls[TURRET_NUM] = ModelerControl("Turret Num", 2, 3, 1, 2); 
	
	// Inverse Kinematics
	controls[L_TARGET_X] = ModelerControl("Left Leg Target X", 0, 0.5, 0.01f, 0);
	controls[R_TARGET_X] = ModelerControl("Right Leg Target X", -0.5, 0, 0.01f, 0);
	controls[L_TARGET_Y] = ModelerControl("Left Leg Target Y", 0, 0.5, 0.01f, 0);
	controls[R_TARGET_Y] = ModelerControl("Right Leg Target Y", 0, 0.5, 0.01f, 0);
	controls[L_TARGET_Z] = ModelerControl("Left Leg Target Z", -0.5, 0, 0.01f, 0);
	controls[R_TARGET_Z] = ModelerControl("Right Leg Target Z", -0.5, 0, 0.01f, 0);

	controls[SHOW_TORUS] = ModelerControl("Show Torus", 0, 1, 1, 0);

	controls[MOOD] = ModelerControl("Mood", 0, 2, 1, 0);

	controls[APPLY_IK] = ModelerControl("Apply IK", 0, 1, 1, 0);

	initTexture();

	ParticleSystem* ps = new ParticleSystem();
	ps->appendForce([&](Vec3f p) { return Vec3f(0.0f, -1.0f, 0.0f); });
	ps->appendForce([&](Vec3f p) { return Vec3f(sin(p[0] * 2) + sin(p[1] * 2), 0.0f, sin(p[0] * 2) - sin(p[1] * 2) * 0.1f); });

    ModelerApplication::Instance()->Init(&createSampleModel, controls, NUMCONTROLS);
	ModelerApplication::Instance()->SetParticleSystem(ps);
    return ModelerApplication::Instance()->Run();
}

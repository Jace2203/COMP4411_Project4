#include "modelerdraw.h"
#include <FL/gl.h>
#include <GL/glu.h>
#include <cstdio>

#include "bitmap.h"
#include <cmath>
#define M_PI 3.141592653589793238462643383279502f
// ********************************************************
// Support functions from previous version of modeler
// ********************************************************
void _dump_current_modelview( void )
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    if (mds->m_rayFile == NULL)
    {
        fprintf(stderr, "No .ray file opened for writing, bailing out.\n");
        exit(-1);
    }
    
    GLdouble mv[16];
    glGetDoublev( GL_MODELVIEW_MATRIX, mv );
    fprintf( mds->m_rayFile, 
        "transform(\n    (%f,%f,%f,%f),\n    (%f,%f,%f,%f),\n     (%f,%f,%f,%f),\n    (%f,%f,%f,%f),\n",
        mv[0], mv[4], mv[8], mv[12],
        mv[1], mv[5], mv[9], mv[13],
        mv[2], mv[6], mv[10], mv[14],
        mv[3], mv[7], mv[11], mv[15] );
}

void _dump_current_material( void )
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    if (mds->m_rayFile == NULL)
    {
        fprintf(stderr, "No .ray file opened for writing, bailing out.\n");
        exit(-1);
    }
    
    fprintf( mds->m_rayFile, 
        "material={\n    diffuse=(%f,%f,%f);\n    ambient=(%f,%f,%f);\n}\n",
        mds->m_diffuseColor[0], mds->m_diffuseColor[1], mds->m_diffuseColor[2], 
        mds->m_diffuseColor[0], mds->m_diffuseColor[1], mds->m_diffuseColor[2]);
}

// ****************************************************************************

// Initially assign singleton instance to NULL
ModelerDrawState* ModelerDrawState::m_instance = NULL;

ModelerDrawState::ModelerDrawState() : m_drawMode(NORMAL), m_quality(MEDIUM)
{
    float grey[]  = {.5f, .5f, .5f, 1};
    float white[] = {1,1,1,1};
    float black[] = {0,0,0,1};
    
    memcpy(m_ambientColor, black, 4 * sizeof(float));
    memcpy(m_diffuseColor, grey, 4 * sizeof(float));
    memcpy(m_specularColor, white, 4 * sizeof(float));
    
    m_shininess = 0.5;
    
    m_rayFile = NULL;
}

// CLASS ModelerDrawState METHODS
ModelerDrawState* ModelerDrawState::Instance()
{
    // Return the singleton if it exists, otherwise, create it
    return (m_instance) ? (m_instance) : m_instance = new ModelerDrawState();
}

// ****************************************************************************
// Modeler functions for your use
// ****************************************************************************
// Set the current material properties

void setAmbientColor(float r, float g, float b)
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    mds->m_ambientColor[0] = (GLfloat)r;
    mds->m_ambientColor[1] = (GLfloat)g;
    mds->m_ambientColor[2] = (GLfloat)b;
    mds->m_ambientColor[3] = (GLfloat)1.0;
    
    if (mds->m_drawMode == NORMAL)
        glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, mds->m_ambientColor);
}

void setDiffuseColor(float r, float g, float b)
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    mds->m_diffuseColor[0] = (GLfloat)r;
    mds->m_diffuseColor[1] = (GLfloat)g;
    mds->m_diffuseColor[2] = (GLfloat)b;
    mds->m_diffuseColor[3] = (GLfloat)1.0;
    
    if (mds->m_drawMode == NORMAL)
        glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, mds->m_diffuseColor);
    else
        glColor3f(r,g,b);
}

void setSpecularColor(float r, float g, float b)
{	
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    mds->m_specularColor[0] = (GLfloat)r;
    mds->m_specularColor[1] = (GLfloat)g;
    mds->m_specularColor[2] = (GLfloat)b;
    mds->m_specularColor[3] = (GLfloat)1.0;
    
    if (mds->m_drawMode == NORMAL)
        glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, mds->m_specularColor);
}

void setShininess(float s)
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    mds->m_shininess = (GLfloat)s;
    
    if (mds->m_drawMode == NORMAL)
        glMaterialf( GL_FRONT, GL_SHININESS, mds->m_shininess);
}

void setDrawMode(DrawModeSetting_t drawMode)
{
    ModelerDrawState::Instance()->m_drawMode = drawMode;
}

void setQuality(QualitySetting_t quality)
{
    ModelerDrawState::Instance()->m_quality = quality;
}

bool openRayFile(const char rayFileName[])
{
    ModelerDrawState *mds = ModelerDrawState::Instance();

	fprintf(stderr, "Ray file format output is buggy (ehsu)\n");
    
    if (!rayFileName)
        return false;
    
    if (mds->m_rayFile) 
        closeRayFile();
    
    mds->m_rayFile = fopen(rayFileName, "w");
    
    if (mds->m_rayFile != NULL) 
    {
        fprintf( mds->m_rayFile, "SBT-raytracer 1.0\n\n" );
        fprintf( mds->m_rayFile, "camera { fov=30; }\n\n" );
        fprintf( mds->m_rayFile, 
            "directional_light { direction=(-1,-1,-1); color=(0.7,0.7,0.7); }\n\n" );
        return true;
    }
    else
        return false;
}

void _setupOpenGl()
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
	switch (mds->m_drawMode)
	{
	case NORMAL:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glShadeModel(GL_SMOOTH);
		break;
	case FLATSHADE:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glShadeModel(GL_FLAT);
		break;
	case WIREFRAME:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glShadeModel(GL_FLAT);
	default:
		break;
	}

}

void closeRayFile()
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    if (mds->m_rayFile) 
        fclose(mds->m_rayFile);
    
    mds->m_rayFile = NULL;
}

void drawSphere(double r)
{
    ModelerDrawState *mds = ModelerDrawState::Instance();

	_setupOpenGl();
    
    if (mds->m_rayFile)
    {
        _dump_current_modelview();
        fprintf(mds->m_rayFile, "scale(%f,%f,%f,sphere {\n", r, r, r );
        _dump_current_material();
        fprintf(mds->m_rayFile, "}))\n" );
    }
    else
    {
        int divisions; 
        GLUquadricObj* gluq;
        
        switch(mds->m_quality)
        {
        case HIGH: 
            divisions = 32; break;
        case MEDIUM: 
            divisions = 20; break;
        case LOW:
            divisions = 12; break;
        case POOR:
            divisions = 8; break;
        }
        
        gluq = gluNewQuadric();
        gluQuadricDrawStyle( gluq, GLU_FILL );
        gluQuadricTexture( gluq, GL_TRUE );
        gluSphere(gluq, r, divisions, divisions);
        gluDeleteQuadric( gluq );
    }
}


void drawBox( double x, double y, double z )
{
    ModelerDrawState *mds = ModelerDrawState::Instance();

	_setupOpenGl();
    
    if (mds->m_rayFile)
    {
        _dump_current_modelview();
        fprintf(mds->m_rayFile,  
            "scale(%f,%f,%f,translate(0.5,0.5,0.5,box {\n", x, y, z );
        _dump_current_material();
        fprintf(mds->m_rayFile,  "})))\n" );
    }
    else
    {
        /* remember which matrix mode OpenGL was in. */
        int savemode;
        glGetIntegerv( GL_MATRIX_MODE, &savemode );
        
        /* switch to the model matrix and scale by x,y,z. */
        glMatrixMode( GL_MODELVIEW );
        glPushMatrix();
        glScaled( x, y, z );
        
        glBegin( GL_QUADS );
        
        glNormal3d( 0.0, 0.0, -1.0 );
        glVertex3d( 0.0, 0.0, 0.0 ); glVertex3d( 0.0, 1.0, 0.0 );
        glVertex3d( 1.0, 1.0, 0.0 ); glVertex3d( 1.0, 0.0, 0.0 );
        
        glNormal3d( 0.0, -1.0, 0.0 );
        glVertex3d( 0.0, 0.0, 0.0 ); glVertex3d( 1.0, 0.0, 0.0 );
        glVertex3d( 1.0, 0.0, 1.0 ); glVertex3d( 0.0, 0.0, 1.0 );
        
        glNormal3d( -1.0, 0.0, 0.0 );
        glVertex3d( 0.0, 0.0, 0.0 ); glVertex3d( 0.0, 0.0, 1.0 );
        glVertex3d( 0.0, 1.0, 1.0 ); glVertex3d( 0.0, 1.0, 0.0 );
        
        glNormal3d( 0.0, 0.0, 1.0 );
        glVertex3d( 0.0, 0.0, 1.0 ); glVertex3d( 1.0, 0.0, 1.0 );
        glVertex3d( 1.0, 1.0, 1.0 ); glVertex3d( 0.0, 1.0, 1.0 );
        
        glNormal3d( 0.0, 1.0, 0.0 );
        glVertex3d( 0.0, 1.0, 0.0 ); glVertex3d( 0.0, 1.0, 1.0 );
        glVertex3d( 1.0, 1.0, 1.0 ); glVertex3d( 1.0, 1.0, 0.0 );
        
        glNormal3d( 1.0, 0.0, 0.0 );
        glVertex3d( 1.0, 0.0, 0.0 ); glVertex3d( 1.0, 1.0, 0.0 );
        glVertex3d( 1.0, 1.0, 1.0 ); glVertex3d( 1.0, 0.0, 1.0 );
        
        glEnd();
        
        /* restore the model matrix stack, and switch back to the matrix
        mode we were in. */
        glPopMatrix();
        glMatrixMode( savemode );
    }
}

void drawTextureBox( double x, double y, double z )
{
    // NOT IMPLEMENTED, SORRY (ehsu)
}

void drawCylinder( double h, double r1, double r2 )
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    int divisions;

	_setupOpenGl();
    
    switch(mds->m_quality)
    {
    case HIGH: 
        divisions = 32; break;
    case MEDIUM: 
        divisions = 20; break;
    case LOW:
        divisions = 12; break;
    case POOR:
        divisions = 8; break;
    }
    
    if (mds->m_rayFile)
    {
        _dump_current_modelview();
        fprintf(mds->m_rayFile, 
            "cone { height=%f; bottom_radius=%f; top_radius=%f;\n", h, r1, r2 );
        _dump_current_material();
        fprintf(mds->m_rayFile, "})\n" );
    }
    else
    {
        GLUquadricObj* gluq;
        
        /* GLU will again do the work.  draw the sides of the cylinder. */
        gluq = gluNewQuadric();
        gluQuadricDrawStyle( gluq, GLU_FILL );
        gluQuadricTexture( gluq, GL_TRUE );
        gluCylinder( gluq, r1, r2, h, divisions, divisions);
        gluDeleteQuadric( gluq );
        
        if ( r1 > 0.0 )
        {
        /* if the r1 end does not come to a point, draw a flat disk to
            cover it up. */
            
            gluq = gluNewQuadric();
            gluQuadricDrawStyle( gluq, GLU_FILL );
            gluQuadricTexture( gluq, GL_TRUE );
            gluQuadricOrientation( gluq, GLU_INSIDE );
            gluDisk( gluq, 0.0, r1, divisions, divisions);
            gluDeleteQuadric( gluq );
        }
        
        if ( r2 > 0.0 )
        {
        /* if the r2 end does not come to a point, draw a flat disk to
            cover it up. */
            
            /* save the current matrix mode. */	
            int savemode;
            glGetIntegerv( GL_MATRIX_MODE, &savemode );
            
            /* translate the origin to the other end of the cylinder. */
            glMatrixMode( GL_MODELVIEW );
            glPushMatrix();
            glTranslated( 0.0, 0.0, h );
            
            /* draw a disk centered at the new origin. */
            gluq = gluNewQuadric();
            gluQuadricDrawStyle( gluq, GLU_FILL );
            gluQuadricTexture( gluq, GL_TRUE );
            gluQuadricOrientation( gluq, GLU_OUTSIDE );
            gluDisk( gluq, 0.0, r2, divisions, divisions);
            gluDeleteQuadric( gluq );
            
            /* restore the matrix stack and mode. */
            glPopMatrix();
            glMatrixMode( savemode );
        }
    }
    
}
void drawTriangle( double x1, double y1, double z1,
                   double x2, double y2, double z2,
                   double x3, double y3, double z3 )
{
    ModelerDrawState *mds = ModelerDrawState::Instance();

	_setupOpenGl();

    if (mds->m_rayFile)
    {
        _dump_current_modelview();
        fprintf(mds->m_rayFile, 
            "polymesh { points=((%f,%f,%f),(%f,%f,%f),(%f,%f,%f)); faces=((0,1,2));\n", x1, y1, z1, x2, y2, z2, x3, y3, z3 );
        _dump_current_material();
        fprintf(mds->m_rayFile, "})\n" );
    }
    else
    {
        double a, b, c, d, e, f;
        
        /* the normal to the triangle is the cross product of two of its edges. */
        a = x2-x1;
        b = y2-y1;
        c = z2-z1;
        
        d = x3-x1;
        e = y3-y1;
        f = z3-z1;
        
        glBegin( GL_TRIANGLES );
        glNormal3d( b*f - c*e, c*d - a*f, a*e - b*d );
        glVertex3d( x1, y1, z1 );
        glVertex3d( x2, y2, z2 );
        glVertex3d( x3, y3, z3 );
        glEnd();
    }
}

void drawtorus(Dot*** draw_pts, int num_t, double RY, double RUX)
{
    if (!*draw_pts)
    {
        int num_point = 9;

        double inner = 0.1,
               outer = 0.05;

        Dot* ctrl = new Dot[num_point];
        ctrl[0] = Dot(+0.0 * outer, +0.0 * outer, +0.0);
        ctrl[1] = Dot(-1.0 * outer, +0.0 * outer, +0.0);
        ctrl[2] = Dot(-1.0 * outer, +1.0 * outer, +0.0);
        ctrl[3] = Dot(-1.0 * outer, +2.0 * outer, +0.0);
        ctrl[4] = Dot(+0.0 * outer, +2.0 * outer, +0.0);
        ctrl[5] = Dot(+1.0 * outer, +2.0 * outer, +0.0);
        ctrl[6] = Dot(+1.0 * outer, +1.0 * outer, +0.0);
        ctrl[7] = Dot(+1.0 * outer, +0.0 * outer, +0.0);
        ctrl[8] = ctrl[0];

        int num_ctrl2 = 9;

        Dot* ctrl2 = new Dot[num_ctrl2];
        ctrl2[0] = Dot(+0.0 * inner, +0.0 * inner, +0.0);
        ctrl2[1] = Dot(-1.0 * inner, +0.0 * inner, +0.0);
        ctrl2[2] = Dot(-1.0 * inner, +1.0 * inner, +0.0);
        ctrl2[3] = Dot(-1.0 * inner, +2.0 * inner, +0.0);
        ctrl2[4] = Dot(+0.0 * inner, +2.0 * inner, +0.0);
        ctrl2[5] = Dot(+1.0 * inner, +2.0 * inner, +0.0);
        ctrl2[6] = Dot(+1.0 * inner, +1.0 * inner, +0.0);
        ctrl2[7] = Dot(+1.0 * inner, +0.0 * inner, +0.0);
        ctrl2[8] = Dot(+0.0 * inner, +0.0 * inner, +0.0);

        Dot* pts = nullptr;
        Dot* path = nullptr;

        calpoint(ctrl, &pts, num_point, num_t);
        calpoint(ctrl2, &path, num_ctrl2, num_t);

        *draw_pts = new Dot* [num_t];
        for(int i = 0; i < num_t; ++i)
        {
            (*draw_pts)[i] = new Dot[num_t];
            int previous, next;
            if (i == 0)
            {
                previous = 0;
                next = 1;
            }
            else if (i == num_t)
            {
                previous = num_t - 1;
                next = num_t;
            }
            else
            {
                previous = i - 1;
                next = i + 1;
            }

            double dx = path[next].x - path[previous].x, dy = path[next].y - path[previous].y, dz = path[next].z - path[previous].z;
            double theta, theta2 = 0;

            theta = acos(-dy/(sqrt(pow(dx, 2)+pow(dy, 2))));

            if (dx < 0)
                theta = 2*M_PI - theta;

            theta = theta - M_PI/2;

            //printf("%1f\n", theta*180/M_PI);
            for(int j = 0; j < num_t; ++j)
            {
                double x = pts[j].x, y = pts[j].y, z = pts[j].z;
                double x1 = x, y1 = y*cos(theta) - z*sin(theta), z1 = y*sin(theta)+ z*cos(theta);  // x axis rotate
                double x2 = x1*cos(theta2)+ z1*sin(theta2), y2 = y1, z2 = -x1*sin(theta2)+ z1*cos(theta2);  //y axis rotate
                //x2 = x; y2 = y; z2 = z;
                
                (*draw_pts)[i][j].x = x2+path[i].z;
                (*draw_pts)[i][j].y = y2+path[i].y;
                (*draw_pts)[i][j].z = z2-path[i].x;
            }
        }

        delete []ctrl;
        delete []ctrl2;
        delete path;
        delete pts;
    }

    glPushMatrix();

    glTranslated(0.5, -0.0, 1.2);
    glRotated(90, 0.0, 1.0, 0.0);

    glRotated(-RUX, 0.0, 0.0, 1.0);

    glRotated(-RY, 0.0, 1.0, 0.0);
    glTranslated(0.2, -0.1, 0.0);

    for(int i = 0; i < num_t; ++i)
    {
        for(int j = 0; j < num_t-1; ++j)
        {
            if (i == num_t-1)
            {
                drawTriangle((*draw_pts)[0][j+1].x, (*draw_pts)[0][j+1].y, (*draw_pts)[0][j+1].z,
                            (*draw_pts)[0][j].x, (*draw_pts)[0][j].y, (*draw_pts)[0][j].z,
                            (*draw_pts)[i][j].x, (*draw_pts)[i][j].y, (*draw_pts)[i][j].z);
                drawTriangle((*draw_pts)[i][j+1].x, (*draw_pts)[i][j+1].y, (*draw_pts)[i][j+1].z,
                            (*draw_pts)[0][j+1].x, (*draw_pts)[0][j+1].y, (*draw_pts)[0][j+1].z,
                            (*draw_pts)[i][j].x, (*draw_pts)[i][j].y, (*draw_pts)[i][j].z);
            }
            else
            {
                drawTriangle((*draw_pts)[i+1][j+1].x, (*draw_pts)[i+1][j+1].y, (*draw_pts)[i+1][j+1].z,
                            (*draw_pts)[i+1][j].x, (*draw_pts)[i+1][j].y, (*draw_pts)[i+1][j].z,
                            (*draw_pts)[i][j].x, (*draw_pts)[i][j].y, (*draw_pts)[i][j].z);
                drawTriangle((*draw_pts)[i][j+1].x, (*draw_pts)[i][j+1].y, (*draw_pts)[i][j+1].z,
                            (*draw_pts)[i+1][j+1].x, (*draw_pts)[i+1][j+1].y, (*draw_pts)[i+1][j+1].z,
                            (*draw_pts)[i][j].x, (*draw_pts)[i][j].y, (*draw_pts)[i][j].z);
            }
        }
    }

    glPopMatrix();
}

void calpoint(Dot* ctl, Dot** point, int num_pts, int num_t)
{
	Dot** temp = new Dot*[num_pts];
	for(int i = 0; i < num_pts; ++i)
		temp[i] = new Dot[num_t];

    double t, diff = 1.0 / (num_t -1);
    for(int j = 0; j < num_pts-1; ++j)
    {
        t = 0;
        for(int i = 0; i < num_t; ++i)
        {
            temp[j][i].x = (1-t)*ctl[j].x+t*ctl[j+1].x;
            temp[j][i].y = (1-t)*ctl[j].y+t*ctl[j+1].y;
            temp[j][i].z = (1-t)*ctl[j].z+t*ctl[j+1].z;
            t += diff;
        }
    }

    for(int i = num_pts-2; i > 0; --i)
        for(int j = 0; j < i; ++j)
        {
            t = 0;
            for(int k = 0; k < num_t; ++k)
            {
                temp[j][k].x = (1-t)*temp[j][k].x+t*temp[j+1][k].x;
                temp[j][k].y = (1-t)*temp[j][k].y+t*temp[j+1][k].y;
                temp[j][k].z = (1-t)*temp[j][k].z+t*temp[j+1][k].z;
                t += diff;
            }
        }

    *point = new Dot[num_t];
    for(int i = 0; i < num_t; ++i)
    {
        (*point)[i].x = temp[0][i].x;
        (*point)[i].y = temp[0][i].y;
        (*point)[i].z = temp[0][i].z;
    }
}

void drawCurve(Dot*** draw_pts, int num_t, double back_y)
{
    if (!*draw_pts)
    {
        int num_point = 9;
        double outer = 0.2, scale = 1;

        Dot* ctrl = new Dot[num_point];
        ctrl[0] = Dot(+0.0 * outer, +0.0 * outer * scale, +0.0);
        ctrl[1] = Dot(-1.0 * outer, +0.0 * outer * scale, +0.0);
        ctrl[2] = Dot(-1.0 * outer, +1.0 * outer * scale, +0.0);
        ctrl[3] = Dot(-1.0 * outer, +2.0 * outer * scale, +0.0);
        ctrl[4] = Dot(+0.0 * outer, +2.0 * outer * scale, +0.0);
        ctrl[5] = Dot(+1.0 * outer, +2.0 * outer * scale, +0.0);
        ctrl[6] = Dot(+1.0 * outer, +1.0 * outer * scale, +0.0);
        ctrl[7] = Dot(+1.0 * outer, +0.0 * outer * scale, +0.0);
        ctrl[8] = ctrl[0];

        int num_ctrl2 = 4;

        Dot* ctrl2 = new Dot[num_ctrl2];

        ctrl2[0] = Dot(+0.0 * outer * 2, +1.0 * outer * 2, +0.0);
        ctrl2[1] = Dot(+0.0 * outer * 2, +0.5 * outer * 2, +0.0);
        ctrl2[2] = Dot(+1.0 * outer * 2, +0.5 * outer * 2, +0.0);
        ctrl2[3] = Dot(+1.0 * outer * 2, +0.0 * outer * 2, +0.0);

        Dot* pts = nullptr;
        Dot* path = nullptr;

        calpoint(ctrl, &pts, num_point, num_t);
        calpoint(ctrl2, &path, num_ctrl2, num_t);

        *draw_pts = new Dot* [num_t];
        for(int i = 0; i < num_t; ++i)
        {
            (*draw_pts)[i] = new Dot[num_t];
            int previous, next;
            if (i == 0)
            {
                previous = 0;
                next = 1;
            }
            else if (i == num_t)
            {
                previous = num_t - 1;
                next = num_t;
            }
            else
            {
                previous = i - 1;
                next = i + 1;
            }

            double dx = path[next].x - path[previous].x, dy = path[next].y - path[previous].y, dz = path[next].z - path[previous].z;
            double theta, theta2 = 0;

            theta = acos(-dy/(sqrt(pow(dx, 2)+pow(dy, 2))));

            if (dx < 0)
                theta = 2*M_PI - theta;

            theta = theta - M_PI/2;

            //printf("%1f\n", theta*180/M_PI);
            for(int j = 0; j < num_t; ++j)
            {
                double x = pts[j].x, y = pts[j].y, z = pts[j].z;
                double x1 = x, y1 = y*cos(theta) - z*sin(theta), z1 = y*sin(theta)+ z*cos(theta);  // x axis rotate
                double x2 = x1*cos(theta2)+ z1*sin(theta2), y2 = y1, z2 = -x1*sin(theta2)+ z1*cos(theta2);  //y axis rotate
                //x2 = x; y2 = y; z2 = z;
                
                (*draw_pts)[i][j].x = x2+path[i].z;
                (*draw_pts)[i][j].y = y2+path[i].y;
                (*draw_pts)[i][j].z = z2-path[i].x;
            }
        }
        delete []ctrl;
        delete []ctrl2;
        delete path;
        delete pts;
    }

    glPushMatrix();
    glRotated(90, 1.0, 0.0, 0.0);
    glRotated(180, 0.0, 1.0, 0.0);

    glTranslated(0.0, 0.25, 0.45);

    glRotated(back_y, 0.0, 1.0, 0.0);
    glTranslated(0, 0.5, 1.2);

    for(int i = 0; i < num_t-2; ++i)
    {
        for(int j = 0; j < num_t-1; ++j)
        {
            drawTriangle((*draw_pts)[i + 1][j + 1].x, (*draw_pts)[i + 1][j + 1].y, (*draw_pts)[i + 1][j + 1].z,
                (*draw_pts)[i + 1][j].x, (*draw_pts)[i + 1][j].y, (*draw_pts)[i + 1][j].z,
                (*draw_pts)[i][j].x, (*draw_pts)[i][j].y, (*draw_pts)[i][j].z);
            drawTriangle((*draw_pts)[i][j+1].x, (*draw_pts)[i][j+1].y, (*draw_pts)[i][j+1].z,
                            (*draw_pts)[i+1][j+1].x, (*draw_pts)[i+1][j+1].y, (*draw_pts)[i+1][j+1].z,
                            (*draw_pts)[i][j].x, (*draw_pts)[i][j].y, (*draw_pts)[i][j].z);
        }
    }

    glPopMatrix();
}

void drawSkyBox( double x, double y, double z )
{
    int width, height;
    unsigned char *tex = readBMP("texture/skybox.bmp", width, height);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex );

    ModelerDrawState *mds = ModelerDrawState::Instance();

	_setupOpenGl();
    
    if (mds->m_rayFile)
    {
        _dump_current_modelview();
        fprintf(mds->m_rayFile,  
            "scale(%f,%f,%f,translate(0.5,0.5,0.5,box {\n", x, y, z );
        _dump_current_material();
        fprintf(mds->m_rayFile,  "})))\n" );
    }
    else
    {

        /* remember which matrix mode OpenGL was in. */
        int savemode;
        glGetIntegerv( GL_MATRIX_MODE, &savemode );
        
        /* switch to the model matrix and scale by x,y,z. */
        glMatrixMode( GL_MODELVIEW );
        glPushMatrix();
        glScaled( x, y, z );

        glEnable(GL_TEXTURE_2D);
        
        glBegin( GL_QUADS );
        
        glNormal3d( 0.0, 0.0, -1.0 );
        glTexCoord2f(0.00, 1/3.0); glVertex3d( 0.0, 0.0, 0.0 ); 
        glTexCoord2f(0.00, 2/3.0); glVertex3d( 0.0, 1.0, 0.0 ); 
        glTexCoord2f(0.25, 2/3.0); glVertex3d( 1.0, 1.0, 0.0 ); 
        glTexCoord2f(0.25, 1/3.0); glVertex3d( 1.0, 0.0, 0.0 ); 
        
        glNormal3d( 0.0, -1.0, 0.0 );
        glTexCoord2f(0.50, 1/3.0); glVertex3d( 0.0, 0.0, 0.0 ); 
        glTexCoord2f(0.75, 1/3.0); glVertex3d( 1.0, 0.0, 0.0 );
        glTexCoord2f(0.75, 0/3.0); glVertex3d( 1.0, 0.0, 1.0 ); 
        glTexCoord2f(0.50, 0/3.0); glVertex3d( 0.0, 0.0, 1.0 );
        
        glNormal3d( -1.0, 0.0, 0.0 );
        glTexCoord2f(1.00, 1/3.0); glVertex3d( 0.0, 0.0, 0.0 ); 
        glTexCoord2f(0.75, 1/3.0); glVertex3d( 0.0, 0.0, 1.0 );
        glTexCoord2f(0.75, 2/3.0); glVertex3d( 0.0, 1.0, 1.0 ); 
        glTexCoord2f(1.00, 2/3.0); glVertex3d( 0.0, 1.0, 0.0 );
        
        glNormal3d( 0.0, 0.0, 1.0 );
        glTexCoord2f(0.75, 1/3.0); glVertex3d( 0.0, 0.0, 1.0 ); 
        glTexCoord2f(0.50, 1/3.0); glVertex3d( 1.0, 0.0, 1.0 );
        glTexCoord2f(0.50, 2/3.0); glVertex3d( 1.0, 1.0, 1.0 ); 
        glTexCoord2f(0.75, 2/3.0); glVertex3d( 0.0, 1.0, 1.0 );
        
        glNormal3d( 0.0, 1.0, 0.0 );
        glTexCoord2f(0.50, 2/3.0); glVertex3d( 0.0, 1.0, 0.0 ); 
        glTexCoord2f(0.50, 3/3.0); glVertex3d( 0.0, 1.0, 1.0 );
        glTexCoord2f(0.75, 3/3.0); glVertex3d( 1.0, 1.0, 1.0 ); 
        glTexCoord2f(0.75, 2/3.0); glVertex3d( 1.0, 1.0, 0.0 );
        
        glNormal3d( 1.0, 0.0, 0.0 );
        glTexCoord2f(0.25, 1/3.0); glVertex3d( 1.0, 0.0, 0.0 ); 
        glTexCoord2f(0.25, 2/3.0); glVertex3d( 1.0, 1.0, 0.0 );
        glTexCoord2f(0.50, 2/3.0); glVertex3d( 1.0, 1.0, 1.0 ); 
        glTexCoord2f(0.50, 1/3.0); glVertex3d( 1.0, 0.0, 1.0 );
        
        glEnd();
        
        glDisable(GL_TEXTURE_2D);
        /* restore the model matrix stack, and switch back to the matrix
        mode we were in. */
        glPopMatrix();
        glMatrixMode( savemode );
    }

    delete tex;
}
#ifndef DRAWBODY_H
#define DRAWBODY_H

const double torso_height = 1.2;
const double head_size = 0.36;
const double arm_length = 0.5;
const double leg_length = 0.5;

class MetaballContainer;

enum {
    RIGHT = -1,
    LEFT = 1
};

enum {
    HEAD,
    BLACK, PINK,
    LEG_TOP, LEG_DOWN,
    BODY,
    HAND_LEFT, HAND_RIGHT,
    NUM
};

class bmp
{
public:
    int width;
    int height;

    unsigned char* data;

    bmp();
    bmp(char* in);
};

void drawTorso();

void drawHead();

void drawHair();

void drawArmL(double upper_y, double upper_z, double lower_x, double lower_z, MetaballContainer* hand, int lod);
void drawArmR(double upper_y, double upper_z, double lower_x, double lower_z, MetaballContainer* hand, int lod);

void drawLegL(double thigh_x, double thigh_y, double leg_x, int lod);
void drawLegR(double thigh_x, double thigh_y, double leg_x, int lod);

void drawEquipment(double back_y, double l_equip_y, double r_equip_y, double l_turret_y, double r_turret_y, double l_turret_x, double r_turret_x, int turret_num, int lod);
void drawEquipmentL(double equip_y, double turret_y, double turret_x, int turret_num, int lod);
void drawEquipmentR(double equip_y, double turret_y, double turret_x, int turret_num, int lod);

void drawCompleteTurret(double turret_y, double turret_x, int turret_num, int lod);
void drawTurrets(int dir, double turret_y, double turret_x, int turret_num, int lod);

void initTexture();
void loadTexture(int index);
void deleteTexture();

void drawLsystem(double IT, double DV, double IA, double AOI, double BY);

void Initial(MetaballContainer* left, MetaballContainer* right);
void callList(int index);

#endif
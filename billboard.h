#ifndef BILLBOARD_H
#define BILLBOARD_H

class Billboard
{
public:
    Billboard(char* file);
    ~Billboard();

    void setTexture();
    void drawonce();
    void draw(float scale);

private:
    int width, height, channel;
    unsigned char* texture;
};

#endif
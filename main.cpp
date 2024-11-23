//#define SDL_MAIN_HANDLED  //xxx

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

using namespace std;

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 512
#define PWIDTH 8
#define PHEIGHT 8
#define PI 3.1415926535
#define P2 PI/2
#define P3 3*PI/2
#define DR 0.0174533    //one degree in radians
#define LEFT 0
#define RIGHT 1
#define UP 2
#define DOWN 3


bool running=true;
SDL_Window* window;
SDL_Renderer* renderer;
bool fullscreen = false;
float px,py,pdx,pdy,pa;     //player position
Uint32 lastTicks=0;

SDL_Color bgcolor = {0x30,0x30,0x30,0xFF};
SDL_Color fgcolor = {0xFF,0xFF,0xFF,0xFF};
SDL_Color yellow2 = {255,255,0};
SDL_Color yellow1 = {170,170,0};
SDL_Color yellow0 = {85,85,0};
SDL_Color black3 = {255,255,255};
SDL_Color black2 = {170,170,170};
SDL_Color black1 = {85,85,85};
SDL_Color black0 = {0,0,0};
SDL_Color red0 = {85,0,0};
SDL_Color red1 = {170,0,0};
SDL_Color red2 = {255,0,0};
SDL_Color green0 = {0,85,0};
SDL_Color green1 = {0,170,0};
SDL_Color green2 = {0,255,0};

SDL_Color mapColor[] = {black1,black2,black3,red0,red1,red2,green0,green1,green2,yellow0,yellow1,yellow2};

int mapX=8,mapY=8,mapS=64;
int map[] =
{
1,1,1,1,1,1,1,1,
1,0,0,1,0,0,0,1,
1,0,1,0,0,0,0,1,
1,0,0,0,0,0,0,1,
1,0,0,0,0,1,1,1,
1,0,2,0,0,1,1,1,
1,0,3,0,0,0,0,1,
1,1,1,1,1,1,1,1,
};

void drawRoofFloor();
void drawPlayer();
void drawMap2D();
void drawRays3D();
void init();
void gameLoop();
void processMouseEvent(SDL_Event *event);
void draw();
void update(long ticks);
void box(int x, int y, int w, int h);
void line(int x1,int y1,int x2,int y2);
void color(int r,int g,int b);
void color(SDL_Color c);
void handleKey(SDL_Scancode scancode,int state);
void cls();

int main(int argc, char* argv[]) {

    int numdrivers, i, working;
    const char* drivername;
//    SDL_SetMainReady();   //xxx

    if (SDL_Init(0) != 0) {
        printf("Error initializing SDL:  %s\n", SDL_GetError());
        return 1;
    }
    atexit(SDL_Quit);
    numdrivers = SDL_GetNumVideoDrivers();
    working = 0;

    for (i = 0; i < numdrivers; ++i) {
        drivername = SDL_GetVideoDriver(i);

        if (SDL_VideoInit(drivername) == 0) {
            SDL_VideoQuit();
            ++working;
            printf("Driver %s works.\n", drivername);
        }
        else {
            printf("Driver %s doesn't work.\n", drivername);
        }
    }

    printf("\n%d video drivers total (%d work)\n", numdrivers, working);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return 0;
    }
    window = SDL_CreateWindow(
            "Raycaster",
//            SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,
            50,50,
            SCREEN_WIDTH, SCREEN_HEIGHT,
            fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP:SDL_WINDOW_RESIZABLE);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    renderer = SDL_CreateRenderer(window, -1, 0);
    if(!renderer) {
        printf("m_renderer error %s\n",SDL_GetError());
    }
    SDL_Color color = {0x4B,0x99,0xC5,0xFF};
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    init();
    gameLoop();

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();

    return 0;
}

void init() {
    px=275;py=275;
    pa=4.75;        //point straight up
    pdx=cos(pa)*5;
    pdy=sin(pa)*5;
}
void gameLoop() {
    running=true;
    while (running)
    {
        SDL_Event event;
        if (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                case SDL_MOUSEMOTION:
                    processMouseEvent(&event);
                    break;
                case SDL_QUIT:
                    running=false;
                    break;
                case SDL_KEYDOWN:
                    if(SDL_SCANCODE_ESCAPE==event.key.keysym.scancode) {
                        running=false;
                    } else {
                        handleKey(event.key.keysym.scancode,1);
                    }
                    break;

                case SDL_KEYUP:
                    handleKey(event.key.keysym.scancode,0);
                    break;
                default:
                    break;
            }
        }
        draw();
        SDL_RenderPresent(renderer);
        update(SDL_GetTicks());
    }
}

void processMouseEvent(SDL_Event *event) {
}

int controls[] = {0,0,0,0,0};
void handleKey(SDL_Scancode scancode,int state) {
    switch(scancode) {
        case SDL_SCANCODE_LEFT: controls[LEFT] = state; break;
        case SDL_SCANCODE_RIGHT: controls[RIGHT] = state; break;
        case SDL_SCANCODE_UP: controls[UP] = state; break;
        case SDL_SCANCODE_DOWN: controls[DOWN] = state; break;
    }

    printf("player=%d,%d angle=%f\n",(int)px,(int)py,pa);
}

void draw() {
    cls();
    drawRoofFloor();
    drawMap2D();
    drawRays3D();
    drawPlayer();
}


void update(long ticks) {
    if(ticks-lastTicks<50)
        return;
    lastTicks = ticks;
    if(controls[LEFT]) {
        pa-=0.1;
        if(pa<0) {pa+=2*PI;}
        pdx=cos(pa)*5;
        pdy=sin(pa)*5;
    }
    if(controls[RIGHT]) {
        pa+=0.1;
        if(pa>2*PI) {pa-=2*PI;}
        pdx=cos(pa)*5;
        pdy=sin(pa)*5;
    }
    if(controls[UP]) {
        px+=pdx;
        py+=pdy;
    }
    if(controls[DOWN]) {
        px-=pdx;
        py-=pdy;
    }
}

void cls() {
    color(bgcolor);
    SDL_RenderClear(renderer);
    color(fgcolor);
}
void color(SDL_Color c) {
    color(c.r,c.g,c.b);
}
void color(int r,int g,int b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 0xFF);
}
void box(int x, int y, int w, int h) {
    SDL_Rect r = {x, y, w, h};
    SDL_RenderFillRect(renderer, &r);
}
void rectangle(int x, int y, int w, int h) {
    SDL_Rect r = {x, y, w, h};
    SDL_RenderDrawRect(renderer, &r);
}

void line(int x1,int y1,int x2,int y2) {
    SDL_RenderDrawLine(renderer,x1,y1,x2,y2);
}

void drawPlayer() {
    color(yellow2);
    float w=PWIDTH/2;
    float h=PHEIGHT/2;
    box(px-w, py-h, PWIDTH, PHEIGHT);
    line(px,py,px+pdx*5,py+pdy*5);
}

void drawRoofFloor() {
    color(green1);
    box(530,0,60*8,159);
    color(black1);
    box(530,160,60*8,319);
}
void drawMap2D() {
    int x,y,xo,yo;
    for(y=0; y<mapY; y++) {
        for(x=0; x<mapX; x++) {
            int mp=map[y*mapX+x];
            if(mp==0) {
                color(mapColor[1]);
            } else {
                color(mapColor[mp * 3 + 2]);
            }
            xo=x*mapS;
            yo=y*mapS;
            box(xo + 1, yo + 1, mapS - 2, mapS - 2);
        }
    }
}

float dist(float ax,float ay,float bx,float by,float ang) {
    return (sqrt((bx-ax)*(bx-ax) + (by-ay)*(by-ay)));
}

void drawRays3D() {
    int r,mx,my,mp,dof,mpH,mpV;
    float rx,ry,ra,xo,yo,disT;
    ra=pa-DR*30; if(ra<0) { ra+=2*PI; } if(ra>2*PI) { ra-=2*PI;}
    for(r=0; r<60; r++) {
        //check horz lines
        dof=0;
        float disH=1000000,hx=px,hy=py;
        float aTan = -1/tan(ra);
        if(ra>PI) { ry=(((int)py>>6)<<6)-0.0001; rx=(py-ry)*aTan+px; yo=-64;xo=-yo*aTan; }   //looking down?
        if(ra<PI) { ry=(((int)py>>6)<<6)+64    ; rx=(py-ry)*aTan+px; yo= 64;xo=-yo*aTan; }   //looking up?
        if(ra==0 || ra==PI) {rx=px; ry=py; dof=8;}  //looking straight left or right
        while(dof<8) {
            mx=(int)(rx)>>6; my=(int)(ry)>>6; mp=my*mapX+mx;
            if(mp>0 && mp<mapX*mapY && map[mp]>0) { mpH=map[mp]; hx=rx; hy=ry; disH=dist(px,py,hx,hy,ra); dof=8;} //hit wall
            else { rx+=xo; ry+=yo; dof++; }    //next line
        }

        //check vert lines
        dof=0;
        float disV=1000000,vx=px,vy=py;
        float nTan = -tan(ra);
        if(ra>P2 && ra<P3) { rx=(((int)px>>6)<<6)-0.0001; ry=(px-rx)*nTan+py; xo=-64;yo=-xo*nTan; }   //looking left
        if(ra<P2 || ra>P3) { rx=(((int)px>>6)<<6)+64    ; ry=(px-rx)*nTan+py; xo= 64;yo=-xo*nTan; }   //looking right
        if(ra==0 || ra==PI) {rx=px; ry=py; dof=8;}  //looking straight up or down
        while(dof<8) {
            mx=(int)(rx)>>6; my=(int)(ry)>>6; mp=my*mapX+mx;
            if(mp>0 && mp<mapX*mapY && map[mp]>0) { mpV=map[mp]; vx=rx; vy=ry; disV=dist(px,py,vx,vy,ra); dof=8; } //hit wall
            else { rx+=xo; ry+=yo; dof++; }    //next line
        }
//        if(map[mp]==2) {light=green2; dark=green1;}
        int lightness=1;
        if(disV<disH) { rx=vx; ry=vy; disT=disV; mp=mpV; lightness=1;}          //vertical wall hit
        if(disH<disV) { rx=hx; ry=hy; disT=disH; mp=mpH; lightness=2;}          //horizontal wall hit
        line(px,py,rx,ry);

        //draw 3d walls
        float ca=pa-ra; if(ca<0){ca+=2*PI;} if(ca>2*PI){ca-=2*PI;} disT=disT*cos(ca);   //fix fisheye
        float lineH=(mapS*320)/disT; if(lineH>320){lineH=320;}      //line height
        float lineO=160-lineH/2;
        if(mp>=0 && mp<=3) {
            color(mapColor[mp * 3 + lightness]);
        }
        box(r*8+530,lineO,8,lineH);     //draw a section of wall
        ra+=DR; if(ra<0) { ra+=2*PI; } if(ra>2*PI) { ra-=2*PI;}
    }
    color(yellow2);
    rectangle(530,0,60*8,320);
}
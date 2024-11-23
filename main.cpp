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

bool running=true;
SDL_Window* window;
SDL_Renderer* renderer;
bool fullscreen = false;
float px=300,py=300,pdx,pdy,pa;     //player position

SDL_Color bgcolor = {0x30,0x30,0x30,0xFF};
SDL_Color fgcolor = {0xFF,0xFF,0xFF,0xFF};
SDL_Color yellow = {255,255,0};
SDL_Color white = {255,255,255};
SDL_Color greyMed = {170,170,170};
SDL_Color greyDark = {85,85,85};
SDL_Color black = {0,0,0};
SDL_Color red0 = {85,0,0};
SDL_Color red1 = {170,0,0};
SDL_Color red2 = {255,0,0};
SDL_Color green0 = {0,85,0};
SDL_Color green1 = {0,170,0};
SDL_Color green2 = {0,255,0};

int mapX=8,mapY=8,mapS=64;
int map[] =
{
1,1,1,1,1,1,1,1,
1,0,0,1,0,0,0,1,
1,0,0,1,0,0,0,1,
1,0,0,1,0,0,0,1,
1,0,0,0,0,1,1,1,
1,0,1,0,0,1,1,1,
1,0,0,0,0,0,0,1,
1,1,1,1,1,1,1,1,
};

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
void handleKey(SDL_Scancode scancode);
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
            460,50,
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
    px=300;py=300;
    pa=0;
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
                        handleKey(event.key.keysym.scancode);
                    }
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

void handleKey(SDL_Scancode scancode) {
    switch(scancode) {
        case SDL_SCANCODE_LEFT:
            pa-=0.1;
            if(pa<0) {pa+=2*PI;}
            pdx=cos(pa)*5;
            pdy=sin(pa)*5;
            break;
        case SDL_SCANCODE_RIGHT:
            pa+=0.1;
            if(pa>2*PI) {pa-=2*PI;}
            pdx=cos(pa)*5;
            pdy=sin(pa)*5;
            break;
        case SDL_SCANCODE_UP:
            px+=pdx;
            py+=pdy;
            break;
        case SDL_SCANCODE_DOWN:
            px-=pdx;
            py-=pdy;
            break;
    }
}

void draw() {
    cls();
    drawMap2D();
    drawRays3D();
    drawPlayer();
}


void update(long ticks) {

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
    color(yellow);
    float w=PWIDTH/2;
    float h=PHEIGHT/2;
    box(px-w, py-h, PWIDTH, PHEIGHT);
    line(px,py,px+pdx*5,py+pdy*5);
}

void drawMap2D() {
    int x,y,xo,yo;
    for(y=0; y<mapY; y++) {
        for(x=0; x<mapX; x++) {
            if(map[y*mapX+x]==1) {color(greyMed);} else { color(greyDark);}
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
    int r,mx,my,mp,dof;
    float rx,ry,ra,xo,yo;
    ra=pa;
    for(r=0; r<1; r++) {
        //check horz lines
        dof=0;
        float disH=1000000,hx=px,hy=py;
        float aTan = -1/tan(ra);
        if(ra>PI) { ry=(((int)py>>6)<<6)-0.0001; rx=(py-ry)*aTan+px; yo=-64;xo=-yo*aTan; }   //looking down?
        if(ra<PI) { ry=(((int)py>>6)<<6)+64    ; rx=(py-ry)*aTan+px; yo= 64;xo=-yo*aTan; }   //looking up?
        if(ra==0 || ra==PI) {rx=px; ry=py; dof=8;}  //looking straight left or right
        while(dof<8) {
            mx=(int)(rx)>>6; my=(int)(ry)>>6; mp=my*mapX+mx;
            if(mp>0 && mp<mapX*mapY && map[mp]==1) {dof=8;} //hit wall
            else { rx+=xo; ry+=yo; dof++; }    //next line
        }
//        color(green1); line(px,py,rx,ry);

        //check vert lines
        dof=0;
        float disV=1000000,vx=px,vy=py;
        float nTan = -tan(ra);
        if(ra>P2 && ra<P3) { rx=(((int)px>>6)<<6)-0.0001; ry=(px-rx)*nTan+py; xo=-64;yo=-xo*nTan; }   //looking left
        if(ra<P2 || ra>P3) { rx=(((int)px>>6)<<6)+64    ; ry=(px-rx)*nTan+py; xo= 64;yo=-xo*nTan; }   //looking right
        if(ra==0 || ra==PI) {rx=px; ry=py; dof=8;}  //looking straight up or down
        while(dof<8) {
            mx=(int)(rx)>>6; my=(int)(ry)>>6; mp=my*mapX+mx;
            if(mp>0 && mp<mapX*mapY && map[mp]==1) {dof=8;} //hit wall
            else { rx+=xo; ry+=yo; dof++; }    //next line
        }
        color(red2); line(px,py,rx,ry);
    }
}
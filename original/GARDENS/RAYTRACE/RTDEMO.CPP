
// RTDEMO.CPP Version 1.0
// Simple demonstration of ray tracing (in grey scale)
//
// Written by Christopher Lampton
// for GARDENS OF IMAGINATION (Waite Group Press,1994)

#include  <stdio.h>
#include	<dos.h>
#include	<conio.h>
#include  <stdlib.h>
#include  <math.h>
#include  <time.h>
#include  <values.h>
#include  "screen.h"

const RECTANGLE=1;
const INFINITE_PLANE=2;
const SPHERE=3;

struct vector_type {
  double x,y,z;
};

struct plane_type {
  vector_type surface_normal;
  double distance;
};

struct sphere_type {
  double l,m,n; // Center of sphere
  double r;     // Radius of sphere
};

union object_union {
  plane_type *plane;
  sphere_type *sphere;
};

struct object_type {
  int type_of_object;
  int color;
  object_union obj;
};

const NUMBER_OF_OBJECTS=2;
const BACKGROUND_COLOR=6;
const double AMBIENT_INTENSITY=.14;
const vector_type LIGHT_SOURCE={500,-200,150};
const VIEWER_X=160;
const VIEWER_Y=100;
const VIEWER_DISTANCE=100;

struct plane_type plane={ {0,-1,0}, 	 // Surface normal
                           500			   // Distance from origin
};
struct sphere_type sphere={0,40,300,   // Coordinates of center
                           100};       // Radius
struct sphere_type sphere2={-60,200,300,   // Coordinates of center
                           40};       // Radius
struct sphere_type sphere3={-30,-150,300,   // Coordinates of center
                            70};       // Radius
struct sphere_type sphere4={0,-30,200,   // Coordinates of center
                           150};       // Radius

struct object_type objects[NUMBER_OF_OBJECTS]={
  {SPHERE,56,(plane_type *) &sphere},
  {INFINITE_PLANE,48,(plane_type *) &plane}
};

int trace_ray(vector_type rayvec,vector_type raystart);
void plot(int x,int y,int color);
vector_type getloc(double t,vector_type rayvec,vector_type raystart);
int lightsource(int color,vector_type loc,vector_type norm);

int image=13;
char far *screen;

void main()
{
  char pal[768];
  vector_type rayvec,raystart;

  randomize();

  for (int grey=0; grey<64; grey++) {
    pal[grey*3] = (int)grey;
    pal[grey*3+1] = (int)grey;
    pal[grey*3+2] = (int)grey;
  }

	int oldmode=*(int *)MK_FP(0x40,0x49);	  // Save previous
                                          //  video mode
  screen=(char far *)MK_FP(0xa000,0);
	setmode(0x13);										      // Set mode 13h
	setpalette(pal,0,256);

  cls(screen);

  for (int yscreen=0; yscreen<200; yscreen++) {
    for (int xscreen=0; xscreen<320; xscreen++) {
      raystart.x=xscreen-VIEWER_X;
      raystart.y=(yscreen-VIEWER_Y)*1.2;
      raystart.z=VIEWER_DISTANCE;
      double distance=sqrt(raystart.x*raystart.x+raystart.y*raystart.y
                             +raystart.z*raystart.z);
      rayvec.x = raystart.x/distance;
      rayvec.y = raystart.y/distance;
      rayvec.z = raystart.z/distance;
      plot(xscreen,yscreen,trace_ray(rayvec,raystart));
    }
  }

  // Wait for user to hit a key:

  while (!kbhit());

  setmode(oldmode); // Reset video and exit
}

int trace_ray(vector_type rayvec,vector_type raystart)
{
  int color;
  double a,b,c,d,x1,y1,z1,x,y,z;
  double t,t0,t1,root1,root2,discriminant;
  double X0,Y0,Z0,Xd,Yd,Zd,dot,dot2;
  double prev_t=MAXDOUBLE;
  vector_type norm,loc,plane_loc,pixel,diff,temp,plane;

  color=BACKGROUND_COLOR;
  for (int ob=0; ob<NUMBER_OF_OBJECTS; ob++) {
    switch (objects[ob].type_of_object) {
      case INFINITE_PLANE:
        norm=objects[ob].obj.plane->surface_normal;
        a=norm.x;
        b=norm.y;
        c=norm.z;
        d=objects[ob].obj.plane->distance;
        X0=raystart.x;
        Y0=raystart.y;
        Z0=raystart.z;
        Xd=rayvec.x;
        Yd=rayvec.y;
        Zd=rayvec.z;
        double vd=a*Xd+b*Yd+c*Zd;
        if (vd!=0) {
          double v0=-(a*X0+b*Y0+c*Z0+d);
          t=v0/vd;
          if ((t<prev_t)&&(t>=0)) {
            prev_t=t;
            loc.x=X0+Xd*t;
            loc.y=Y0+Yd*t;
            loc.z=Z0+Zd*t;
            color=lightsource(objects[ob].color,loc,norm);
          }
        }
        break;
      case SPHERE:
        double Xc=objects[ob].obj.sphere->l;
        double Yc=objects[ob].obj.sphere->m;
        double Zc=objects[ob].obj.sphere->n;
        double Sr=objects[ob].obj.sphere->r;
        X0=raystart.x;
        Y0=raystart.y;
        Z0=raystart.z;
        Xd=rayvec.x;
        Yd=rayvec.y;
        Zd=rayvec.z;
        b=2.0*(Xd*(X0-Xc)+Yd*(Y0-Yc)+Zd*(Z0-Zc));
        c=(X0-Xc)*(X0-Xc)+(Y0-Yc)*(Y0-Yc)+(Z0-Zc)*(Z0-Zc)-Sr*Sr;
        if ((discriminant=b*b-4*c)>=0) {
          double sd=sqrt(discriminant);
          t0=(-b-sd)/2;
          t1=(-b+sd)/2;
          if ((t0>0)||(t1>0)) {
            if (t0>0) t=t0;
            if ((t1<t0)&&(t1>=0)) t=t1;
            if (t<prev_t) {
              prev_t=t;
              loc.x=X0+Xd*t;
              loc.y=Y0+Yd*t;
              loc.z=Z0+Zd*t;
              norm.x=(loc.x-Xc)/Sr;
              norm.y=(loc.y-Yc)/Sr;
              norm.z=(loc.z-Zc)/Sr;
              color=lightsource(objects[ob].color,loc,norm);
            }
          }
        }
        break;
    }
  }
  return color;
}

void plot(int x,int y,int color)
{
  int c;

  screen[y*320+x]=color;
}

int lightsource(int color,vector_type loc,vector_type norm)
{
  double newcolor;
  vector_type lightvec;
  double integer;

  lightvec.x=LIGHT_SOURCE.x-loc.x;
  lightvec.y=LIGHT_SOURCE.y-loc.y;
  lightvec.z=LIGHT_SOURCE.z-loc.z;
  long distance=sqrt(lightvec.x*lightvec.x+lightvec.y*lightvec.y+lightvec.z*lightvec.z);
  if (distance==0) distance=1;
  double xlight=(double)lightvec.x/distance;
  double ylight=(double)lightvec.y/distance;
  double zlight=(double)lightvec.z/distance;
  double intensity=norm.x*xlight+norm.y*ylight+norm.z*zlight;
  if (intensity<0.0) intensity=0;
  intensity+=AMBIENT_INTENSITY;
  if (intensity>1.0) intensity=1.0;
  newcolor=color*intensity;
  if (modf(newcolor,&integer) * 10000 > random(10000)) newcolor=ceil(newcolor);
  else newcolor=floor(newcolor);
  if (newcolor>63) newcolor=63;
  return (int) newcolor;
}

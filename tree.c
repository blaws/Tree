#include <stdio.h>
#include <math.h>
#include "gfx.h"

void leafdraw(int x,int y,double thetaDeg,int size,const int baseRGB[3]){
  double theta = thetaDeg * M_PI/180;  // convert angle to radians
  int i,r=0,dir=2,rmax=5,currentcolor[3];

  for(i=0;i<3;i++){  // make sure color values are within range
    currentcolor[i] = baseRGB[i] + pow(-1,rand()%2)*(rand()%20);
    if(currentcolor[i]<0) currentcolor[i] = 0;
    else if(currentcolor[i]>255) currentcolor[i] = 255;
  }
  gfx_color(currentcolor[0],currentcolor[1],currentcolor[2]);
  //gfx_line(x,y,x+size*sin(theta),y+size*cos(theta)); // draw main stem

  for(i=0;i<size;i++){  // draw points
    gfx_line(x+i*sin(theta),y+i*cos(theta),x+i*sin(theta)+r*sin(theta+M_PI/4),y+i*cos(theta)+r*cos(theta+M_PI/4));
    gfx_line(x+i*sin(theta),y+i*cos(theta),x+i*sin(theta)+r*sin(theta-M_PI/4),y+i*cos(theta)+r*cos(theta-M_PI/4));
    r+=dir;
    if(r==0 || r==rmax){
      dir*=-1;
      rmax--;
    }
  }
}

void branchdraw(double x,double y,int xmax,int ymax,int r,int rmax,double thetaDeg,const int branchRGB[3],const int leafRGB[3],int level){
  if(level>3 || y<0 || x>xmax) return;
  else if(r<1){
    leafdraw(x,y,thetaDeg,rmax,leafRGB);
    return;
  }
  int j;
  double i;
  double theta = thetaDeg * M_PI/180;  // convert angle to radians

  while(r>0){
    // draw current "layer" of tree
    for(i=-r;i<r;i+=.5){
      int currentcolor[3];
      for(j=0;j<3;j++){  // make sure color values are within range
	currentcolor[j] = branchRGB[j] + pow(-1,rand()%2)*(rand()%20);
	if(currentcolor[j]<0) currentcolor[j] = 0;
	else if(currentcolor[j]>255) currentcolor[j] = 255;
      }
      gfx_color(currentcolor[0],currentcolor[1],currentcolor[2]);
      gfx_line(x+i*cos(theta),y+i*sin(theta),x+(i+.5)*cos(theta),y+(i+.5)*sin(theta));
    }

    // draw leaf if branch is thin enough
    if(r<3 && rand()%10<1) leafdraw(x,y,thetaDeg+pow(-1,rand()%2)*(45+rand()%90),rmax,leafRGB);

    // move to next layer
    y -= cos(theta);
    x += sin(theta);

    // next layer modifications
    if(rand()%50 < 1) r--;  // decrease branch size
    if(rand()%50 < 1){  // bend branch
      thetaDeg += pow(-1,rand()%2)*(rand()%15);
      theta = thetaDeg * M_PI/180;
      // eliminate gap at bend
      y += 5*cos(theta);
      x -= 5*sin(theta);
    }
    if(rand()%500 < 1){  // fork branch
      double theta1 = thetaDeg+pow(-1,rand()%2)*(rand()%45);
      double theta2 = thetaDeg+pow(-1,rand()%2)*(rand()%45);
      branchdraw(x-r/2*sin(theta),y+5*cos(theta),xmax,ymax,r/2,rmax,theta1<theta2?theta1:theta2,branchRGB,leafRGB,level);
      branchdraw(x+r/2,y+5*cos(theta),xmax,ymax,r/2,rmax,theta1>=theta2?theta1:theta2,branchRGB,leafRGB,level);
      return;
    }

    if((rand()%(4*r*r+1>10?4*r*r+1:10)) < 1){  // make new branch off current branch (modulus by 10 or greater)
      double newTheta = thetaDeg+pow(-1,rand()%2)*(45+rand()%45);
      if(newTheta>125) newTheta-=90;
      else if(newTheta<-125) newTheta+=90;
      branchdraw(x,y,xmax,ymax,r-rand()%(r/2+1),rmax,newTheta,branchRGB,leafRGB,level+1);
    }

    // check for user input
    while(gfx_event_waiting()) if(gfx_wait()=='q') exit(0);
  }
  if(rand()%10<9) leafdraw(x,y,thetaDeg,rmax,leafRGB);  // put leaf at end of branch
}

int main(void){
  srand(time(NULL));
  int xsize=1500,ysize=750,y,r,branchRGB[3],leafRGB[3];
  char c;

  gfx_open(xsize,ysize,"Random Trees");
  gfx_clear_color(200,200,255);

  do{
    gfx_clear();
    gfx_color(10,200,10);
    for(y=.75*ysize;y<=ysize;y++) gfx_line(0,y,xsize,y);  // color ground;

    y = .75 * ysize;
    while(y<ysize){
      y += rand()%(ysize/8);
      r = 25 - (ysize-y)/5;
      if(r<10) r=10;
      branchRGB[0] = 139+pow(-1,rand()%2)*(rand()%20);
      branchRGB[1] = 69+pow(-1,rand()%2)*(rand()%20);
      branchRGB[2] = 19+pow(-1,rand()%2)*(rand()%20); // base color: 139,69,19 (light brown)
      leafRGB[0] = rand()%20;
      leafRGB[1] = 175+pow(-1,rand()%2)*(rand()%20);
      leafRGB[2] = rand()%20; // base color: 0,175,0 (green)

      branchdraw(rand()%xsize,y,xsize,y,r>0?r:1,r>0?r:1,0,branchRGB,leafRGB,1);
    }

    c=gfx_wait();
  }while(c!='q');
}

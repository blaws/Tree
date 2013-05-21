#include <stdio.h>
#include <math.h>
#include "gfx.h"

void leafdraw(int x,int y,double thetaDeg){
  double theta = thetaDeg * M_PI/180;  // convert angle to radians
  int i,r=0,dir=2,rmax=5;
  gfx_color(rand()%20,175+pow(-1,rand()%2)*(rand()%20),rand()%20);
  gfx_line(x,y,x+15*sin(theta),y+15*cos(theta)); // draw main stem

  for(i=0;i<15;i++){  // draw points
    gfx_line(x+i*sin(theta),y+i*cos(theta),x+i*sin(theta)+r*sin(theta+M_PI/4),y+i*cos(theta)+r*cos(theta+M_PI/4));
    gfx_line(x+i*sin(theta),y+i*cos(theta),x+i*sin(theta)+r*sin(theta-M_PI/4),y+i*cos(theta)+r*cos(theta-M_PI/4));
    r+=dir;
    if(r==0 || r==rmax){
      dir*=-1;
      rmax--;
    }
  }
}

void branchdraw(double x,double y,int xmax,int ymax,int r,double thetaDeg,int level){
  if(level>3 || y<0 || y>ymax || x>xmax) return;
  else if(r<1){
    leafdraw(x,y,thetaDeg);
    return;
  }
  double i;
  double theta = thetaDeg * M_PI/180;  // convert angle to radians

  while(r>0){
    // draw current "layer" of tree
    for(i=-r;i<r;i+=.5){
      gfx_color(139+pow(-1,rand()%2)*(rand()%20),69+pow(-1,rand()%2)*(rand()%20),19+pow(-1,rand()%2)*(rand()%20));  // base color: 139,69,19 (light brown)
      gfx_line(x+i*cos(theta),y+i*sin(theta),x+(i+.5)*cos(theta),y+(i+.5)*sin(theta));
    }

    // draw leaf if branch is thin enough
    if(r<3 && rand()%10<1) leafdraw(x,y,thetaDeg+pow(-1,rand()%2)*(45+rand()%90));

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
      branchdraw(x-r/2,y+5*cos(theta),xmax,ymax,r/2,theta1<theta2?theta1:theta2,level);
      branchdraw(x+r/2,y+5*cos(theta),xmax,ymax,r/2,theta1>=theta2?theta1:theta2,level);
      return;
    }

    if((rand()%(4*r*r+1>10?4*r*r+1:10)) < 1){  // make new branch off current branch (modulus by 10 or greater)
      double newTheta = thetaDeg+pow(-1,rand()%2)*(45+rand()%45);
      if(newTheta>125) newTheta-=90;
      else if(newTheta<-125) newTheta+=90;
      branchdraw(x,y,xmax,ymax,r-rand()%(r/2+1),newTheta,level+1);
    }
  }
  if(rand()%10<9) leafdraw(x,y,thetaDeg);  // put leaf at end of branch
}

int main(void){
  srand(time(NULL));
  int wsize = 750;
  char c;

  gfx_open(wsize,wsize,"Random Tree");
  gfx_clear_color(200,200,255);

  do{
    gfx_clear();

    branchdraw(wsize/2,wsize,wsize,wsize,15,0,1);

    c=gfx_wait();
  }while(c!='q');
}

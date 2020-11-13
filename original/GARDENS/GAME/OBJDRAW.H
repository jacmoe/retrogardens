struct objtype {
	int screenx,screeny;
  int mazex,mazey;
  int alignx,aligny;
  int projx,projy;
  int wbit,hbit;
  int wdraw,hdraw;
  int visible;
  unsigned int distance;
	int imagenum;
};

void drawobject(objtype *obj,int lclipx,int lclipy,int rclipx,
			 int rclipy,unsigned int distance,char *screen,
       char *litelevel);

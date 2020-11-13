struct tpolytype {
	int x[4];
	int y[4];
	int wtext;
	int htext;
	char *textmap;
};

void polytext(tpolytype *poly,int leftx,int lefty,int rightx,
			 int righty,char *screen);

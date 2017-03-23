#ifndef __OPTI_H
#define __OPTI_H

extern "C" {
	int fixdiv(char,char);
	int fixmul(char,char);
	void drawwall(char far*,char far*,long,long, char far*);
	void drawfloorrow(int,char far*,char far*,
						char far*,char far*,char far*,
						char far*,int,int,int,int,int);
}

#endif // __OPTI_H

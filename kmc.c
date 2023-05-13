#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <ncurses.h>
#include <unistd.h>

// number of columns NX
#define NX_DEF 64
unsigned char NX;
// number of rows NY
#define NY_DEF 48
unsigned char NY;

//unsigned char pg[NY][NX];
unsigned int** pg;
unsigned char** weight;
unsigned char possible_moves[8];
signed int ix, iy;
unsigned int k, p;
unsigned char mask;
/*
x(0,0)      a b
          h     c
             X
          g     d
            f e
                      a   b   c   d   e   f   g   h */
signed char kx[] = { -1, +1, +2, +2, +1, -1, -2, -2 },
            ky[] = { -2, -2, -1, +1, +2, +2, +1, -1 };

unsigned char check_directions(unsigned char* mask, const signed int x, const signed int y)
{
	if(mask)
		(*mask) = 0;
	unsigned int ways = 0;
	for(int r = 0; r < 8; ++r) {
		if(((x + kx[r]) >= 0) &&
       ((x + kx[r]) < NX) &&
       ((y + ky[r]) >= 0) &&
       ((y + ky[r]) < NY) &&
       (pg[x + kx[r]][y + ky[r]] == 0x00)) {
			if(mask)
				(*mask) |= 1 << r;
			++ways;
      if((x > NX/2) ^ (kx[r] < 0))
        ++ways;
      if((y > NY/2) ^ (ky[r] > 0))
        ++ways;
		}
	}
	return ways;
}

int main(int argc, char *argv[])
{
  initscr();

  //NX = NX_DEF;
  //NY = NY_DEF;

  //getmaxyx(stdscr,row,col);		/* get the number of rows and columns */
  getmaxyx(stdscr,NY,NX);		/* get the number of rows and columns */

//  NX /= 2;
  NX -= 2;
  NY -= 2;

  //mvprintw(0, 0, "NX = %d\n", NX);
  //mvprintw(1, 0, "NY = %d\n", NY);

  //mvprintw(2+iy,2+2*ix,"%02X",p);
  //refresh();

  //getch();
  //NX = NX_DEF;
  //NY = NY_DEF;

  // number of rows NY
  pg = (unsigned int**)malloc(NX*sizeof(unsigned int*));
  weight = (unsigned char**)malloc(NX*sizeof(unsigned char*));
  // number of columns NX
  for(int i = 0; i < NX; ++i)
  {
    pg[i] = (unsigned int*)malloc(NY*sizeof(unsigned int));
    memset(pg[i], 0x00, NY);
    weight[i] = (unsigned char*)malloc(NY*sizeof(unsigned char));
  }

  srand(time(0));
	for(int i = 0; i < NX; ++i) {
		for(int j = 0; j < NY; ++j)
      weight[i][j] = check_directions(NULL, i, j);
	}

	ix = rand() % NX;
	iy = rand() % NY;

	p = pg[ix][iy] = 0x01;
	++p;
	mask = 0;

	unsigned int d = check_directions(&mask, ix, iy);

	//printf("possible steps = %d; mask 0x%02X\n", k, mask);
	//printf("%d / %d", ix, iy);
	while(d) {
		unsigned int min_ways = 65535;
		for(k = 0; k < 8; ++k) {
			if(mask & (1 << k)) {
        signed int dx = ix + kx[k];
        signed int dy = iy + ky[k];
				possible_moves[k] = check_directions(0, dx, dy);
        for(int l = 0; l < 8; ++l) {
          unsigned char msk = 0;
          possible_moves[k] += check_directions(&msk, dx + kx[l], dy + ky[l]);
          if(0) { //msk) {
            signed int gx = dx + kx[k];
            signed int gy = dy + ky[k];
            for(int m = 0; m < 8; ++m) {
              if(msk & (1<<m))
                possible_moves[k] += check_directions(0, gx + kx[m], gy + ky[m]);
              }
            }
        }
				//printf("possible direction %d - > %d\n", k, possible_moves[k]);
				if(min_ways > possible_moves[k])
					min_ways = possible_moves[k];
			}
		}
		unsigned int min_ways_quant = 0;
		for(k = 0; k < 8; ++k) {
			if(mask & (1 << k)) {
				//printf("way %d has %d possible cont.\n", k, possible_moves[k]);
				if(possible_moves[k] > (min_ways))
					mask ^= (1 << k);
				if(possible_moves[k] == min_ways)
					++min_ways_quant;
			}
		}
    d = 0;
    if(min_ways_quant) {
  		//printf("%d ", min_ways_quant);
  		unsigned char selected_direction = rand() % min_ways_quant;
  		for(k = 0; k < 8; ++k) {
  			if(mask & (1 << k)) {
  				if(!selected_direction--) {
  					//printf("selected direction %d\n", k);
  					break;
  				}
  			}
  		}
  		pg[ix + kx[k]][iy + ky[k]] = p++;
      //mvprintw(iy,2*ix,"%02X",p);
      mvprintw(iy,ix,"#");
      refresh();
      //usleep(25);
  		ix += kx[k];
  		iy += ky[k];
  		d = check_directions(&mask, ix, iy);
    }
	}

  mvprintw(NY,0,"Run out of options at %d/%d (%d%%)", p, (NX-1)*(NY-1), (100*p)/((NX-1)*(NY-1)));
  refresh();

  endwin();

  for(int i = 0; i < NY; ++i)
  {
    free(pg[i]);
    free(weight[i]);
  }
  free(pg);
  free(weight);
}

#include"math.h" 
#include"matrixOps.hpp"
#include"sse.hpp"
void mandelbrot(int m,int n,double xmax,double xmin,double ymax,double ymin,void* A,int kmax)
{
	int i;
	int j;
	double cr;
	double ci;
	double zr;
	double zi;
	int k;
	double magnitude;
	int condition;
	double z2r;
	double z2i;
	double *A_data= (*double**) ((char*)A+16)) ;
	long *A_dim = (*(long**) ((char*)A+32)) ;
	for(i=0;i<m;i=i+1)
	{
		for(j=0;j<n;j=j+1)
		{
			cr = (xmin + (((xmax - xmin) * i) / (m - 1)));
			ci = (ymin + (((ymax - ymin) * j) / (n - 1)));
			zr = 0;
			zi = 0;
			k = 0;
			magnitude = ((zr * zr) + (zi * zi));
			condition = 1;
			while((condition > 0))
			{
				z2r = ((zr * zr) - (zi * zi));
				z2i = ((2 * zi) * zr);
				zr = (z2r + cr);
				zi = (z2i + ci);
				magnitude = ((zr * zr) + (zi * zi));
				k = (k + 1);
				if((magnitude > 4))
				{
					condition = 0;
				}
				else
				{
				}
				if((k > kmax))
				{
					condition = 0;
				}
				else
				{
				}
			}
			(*(A_data+i*A_dim[1]+j)) = magnitude;
		}
	}
	return ;
}

void mandelbrotp(int m,int n,double xmax,double xmin,double ymax,double ymin,void* A,int kmax)
{
	int i;
	int j;
	double cr;
	double ci;
	double zr;
	double zi;
	int k;
	double magnitude;
	int condition;
	double z2r;
	double z2i;
	double *A_data= (*double**) ((char*)A+16)) ;
	long *A_dim = (*(long**) ((char*)A+32)) ;
	for(i=0;i<m;i=i+1)
	{
		for(j=0;j<n;j=j+1)
		{
			cr = (xmin + (((xmax - xmin) * i) / (m - 1)));
			ci = (ymin + (((ymax - ymin) * j) / (n - 1)));
			zr = 0;
			zi = 0;
			k = 0;
			magnitude = ((zr * zr) + (zi * zi));
			condition = 1;
			while((condition > 0))
			{
				z2r = ((zr * zr) - (zi * zi));
				z2i = ((2 * zi) * zr);
				zr = (z2r + cr);
				zi = (z2i + ci);
				magnitude = ((zr * zr) + (zi * zi));
				k = (k + 1);
				if((magnitude > 4))
				{
					condition = 0;
				}
				else
				{
				}
				if((k > kmax))
				{
					condition = 0;
				}
				else
				{
				}
			}
			(*(A_data+i*A_dim[1]+j)) = magnitude;
		}
	}
	return ;
}


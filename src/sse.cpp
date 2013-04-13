#include"sse.hpp"
/*int main(){
	
float a[16],b[16],*c;
int i;
for(i=0;i<16;i++){
	b[i]=1;
	a[i]=1;
	printf("\n%f",a[i]);
}

c=sse_add(a,b,16);
for(i=0;i<16;i++){
	printf("%f\n",c[i]);
}
return 0;
}
*/
float* sse_add(float  *a, float *b,int size){
	
	float *c;
	c=new float[size];
	for(int i=0;i<(size/8)*8;i+=8){
		__m128 sse_a =_mm_load_ps(&a[i]);
		__m128 sse_b =_mm_load_ps(&b[i]);
		__m128 sse_c =_mm_add_ps(sse_a,sse_b);
		_mm_store_ps(&c[i],sse_c);
	}
	for(int i=(size/8)*8;i<size;i++){
		c[i]=a[i]+b[i];
	}
	return c;
}

float* sse_sub(float *a,float *b,int size){
	float *c;
	c= new float[size];
	for(int i=0;i<(size/4)*4;i++){
		 __m128 sse_a =_mm_load_ps(&a[i]);
                 __m128 sse_b =_mm_load_ps(&b[i]);
                 __m128 sse_c =_mm_sub_ps(sse_a,sse_b);
                 _mm_store_ps(&c[i],sse_c);
	
	}
	for(int i=(size/4)*4;i<size;i++){
                c[i]=a[i]-b[i];
        }
	return c ;
}

float* sse_mul(float*a,float*b,int size){
	 float *c;
        c= new float[size];
        for(int i=0;i<(size/4)*4;i++){
                 __m128 sse_a =_mm_load_ps(&a[i]);
                 __m128 sse_b =_mm_load_ps(&b[i]);
                 __m128 sse_c =_mm_mul_ps(sse_a,sse_b);
                 _mm_store_ps(&c[i],sse_c);
		
        }
        for(int i=(size/4)*4;i<size;i++){
                c[i]=a[i]*b[i];
        }
	return c;
}


float* sse_div(float*a,float*b,int size){
	  float *c;
        c= new float[size];
        for(int i=0;i<(size/4)*4;i++){
                 __m128 sse_a =_mm_load_ps(&a[i]);
                 __m128 sse_b =_mm_load_ps(&b[i]);
                 __m128 sse_c =_mm_div_ps(sse_a,sse_b);
                 _mm_store_ps(&c[i],sse_c);

        }
        for(int i=(size/4)*4;i<size;i++){
                c[i]=a[i]*b[i];
        }
        return c;

}


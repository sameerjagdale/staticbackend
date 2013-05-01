import mandelbrot
import mmul
from numpy import *
import time
#mandelbrot in serial

def writeFile(data):
	fo=open("output.txt","a")
	fo.write(data)
	fo.close()



n = [1000,4000,6000,8000]
writeFile("name,elements,mean,stdDev\n")
for i in n:
	xmax = 1.0
	xmin = 0.0
	ymax = 1.0
	ymin = 0.0
	kmax = 5
	A = zeros((i,i))
	meanVals=0
	for j in range(3):
		t1 = time.time()
		mandelbrot.mandelbrot(i,i,xmax,xmin,ymax,ymin,A,kmax)
		t2 = time.time()
		print(t2-t1)
		meanVals=meanVals+(t2-t1)
	writeFile("mandelBrotSerial,"+str(i)+","+str(meanVals/4)+"\n")
#mandelbrot in parallel
for i in n:
	xmax = 1.0
	xmin = 0.0
	ymax = 1.0
	ymin = 0.0
	kmax = 5
	A = zeros((i,i))
	meanVals=0;
	for j in range(3):
		t1 = time.time()
		mandelbrot.mandelbrotp(i,i,xmax,xmin,ymax,ymin,A,kmax)
		t2 = time.time()
		print(t2-t1)
		meanVals=meanVals+(t2-t1)
	writeFile("mandelBrotParallel,"+str(i)+","+str(meanVals/4)+"\n")
print("starting matrix operations")
matrxs=zeros((1,3))
matrxp=zeros((1,3))
n=[500,1000,1500,2000,]
#matrix in serial
for i in n :
	A=zeros((i,i))
	B=zeros((i,i))
	C=zeros((i,i))
	matrxs=zeros((1,3))
	meanVals=meanVals=0
	for j in range(3):
		t1=time.time()
		mmul.mmuls(A,B,C,i,i,i)
		t2=time.time()
		print(t2-t1)
		#append(matrxs,t2-t1)
		meanVals=meanVals+(t2-t1)
	#stdDev=std(matrxs)
	#meanVal=mean(matrxs)
	writeFile("matrix serial"+str(i)+","+str(meanVals/4)+"\n" )
# matrix in parallel
for i in n :
	A=zeros((i,i))
	B=zeros((i,i))
	C=zeros((i,i))
	meanVals=0
	matrxp=zeros((1,3))
	for j in range(3):
		t1=time.time()
		mmul.mmulp(A,B,C,i,i,i)
		t2=time.time()
		print(t2-t1)
		meanVals=meanVals+(t2-t1)
	writeFile("matrix parallel"+str(n)+","+str(meanVals/4)+"\n" )

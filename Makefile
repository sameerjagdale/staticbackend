
all : 
	g++ -I include/ -I /home/sameer/ -lPocoXML -lPocoFoundation -L /usr/local/lib/ src/*.cpp  -std=c++0x
clean : 
	rm *.o a.out

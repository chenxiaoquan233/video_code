#include "../../include/encode/Encoder.h"

int main(int argc, char** argv)
{
	Encoder* encoder = new Encoder("../example/pngs/encode/");
	
	argv[1]= encoder->getinfn();
	argv[2]= encoder->getoufn();
	argv[3] = encoder->getlen();
	
	encoder->encode(argv[1],argv[2],argv[3]);
}
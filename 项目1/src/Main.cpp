#include "./../include/Encoder.h"

int main(int argc, char** argv)
{
	Encoder* encoder = new Encoder();
	encoder->encode(argv[1],argv[2],argv[3]);
}
#include "../../include/encode/Encoder.h"

int main(int argc, char** argv)
{
	Encoder* encoder = new Encoder("../example/pngs/");
	encoder->encode(argv[1],argv[2],argv[3]);
}
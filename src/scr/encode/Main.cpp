#include "../../include/encode/Encoder.h"

int main(int argc, char** argv)
{
	Encoder* encoder = new Encoder();
	if (argc != 4)
	{
		std::cout << "The number of Variable is Wrong" << std::endl;
		exit(0);
	}
	encoder->encode(argv[1],argv[2],argv[3]);
}
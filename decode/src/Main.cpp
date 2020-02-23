#include "../include/decoder.h"

int main(int argc, char** argv)
{
	Decoder* decoder = new Decoder();
	decoder->decode(argv[1], argv[2]);
}
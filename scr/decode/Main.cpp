#include "../../include/decode/decoder.h"

using namespace cv;

int main(int argc, char** argv)
{
	Decoder* decoder = new Decoder("../example/decode/pngs/");
	decoder->decode(argv[1],argv[2]);
}
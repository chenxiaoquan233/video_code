#include "../../include/decode/decoder.h"

using namespace cv;

int main(int argc, char** argv)
{
	Decoder* decoder = new Decoder();
	if (argc != 3)
	{
		cout << "Number of parameters should be 3" << endl;
		exit(0);
	}
	decoder->decode(argv[1],argv[2]);
}
#include "../../include/decode/decoder.h"

using namespace cv;

int main(int argc, char** argv)
{
	Decoder* decoder = new Decoder();
	if (argc != 4)
	{
		cout << "Number of parameters should be 4" << endl;
		exit(0);
	}
	decoder->CreateCheckMatrix();
	decoder->decode(argv[1],argv[2],argv[3]);
}
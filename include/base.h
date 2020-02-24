#pragma once
#include <stdio.h>
#include <string.h>
#include <vector>
#include <opencv2/opencv.hpp>

class Code_base
{
protected:
	const int BLOCK_SIZE = 2;//side block number of every single block
	int BLOCK_WIDTH;
	const int BASE_BLOCK_WIDTH = 10;
	const int IMG_X = 720;
	const int IMG_Y = 1280;
	int ANCHOR_AREA; //blocks coverd by anchor
	int MAX_BIN_PER_IMAGE;
	const int ANCHOR_BASE_BLOCKS = 16;
public:
	Code_base()
	{
		BLOCK_WIDTH = BLOCK_SIZE * BASE_BLOCK_WIDTH;
		ANCHOR_AREA = 3 * (ANCHOR_BASE_BLOCKS / BLOCK_SIZE) * (ANCHOR_BASE_BLOCKS / BLOCK_SIZE);
		MAX_BIN_PER_IMAGE = (IMG_X / BLOCK_WIDTH * IMG_Y / BLOCK_WIDTH - ANCHOR_AREA);
	}
	~Code_base() {}
};
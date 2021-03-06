#pragma once
#include <stdio.h>
#include <string.h>
#include <vector>
#include <algorithm>
#include <opencv2/opencv.hpp>

//#define DEBUG

class Code_base
{
protected:
	const int BLOCK_SIZE = 1;//side block number of every single block
	int BLOCK_WIDTH;
	const int BASE_BLOCK_WIDTH = 10;
	const int IMG_X = 880;
	const int IMG_Y = 1580;
	int ANCHOR_AREA; //blocks coverd by anchor
	int MAX_BIN_PER_IMAGE;
	const int ANCHOR_BASE_BLOCKS = 16;
	int BOTTOM_BASE_BLOCKS;
	int ANCHOR_WIDTH;
public:
	Code_base()
	{
		BLOCK_WIDTH = BLOCK_SIZE * BASE_BLOCK_WIDTH;
		ANCHOR_AREA = 4 * (ANCHOR_BASE_BLOCKS / BLOCK_SIZE) * (ANCHOR_BASE_BLOCKS / BLOCK_SIZE);
		MAX_BIN_PER_IMAGE = (IMG_X / BLOCK_WIDTH * IMG_Y / BLOCK_WIDTH - ANCHOR_AREA);
		ANCHOR_WIDTH = ANCHOR_BASE_BLOCKS * BASE_BLOCK_WIDTH;
	}
	~Code_base() {}
};
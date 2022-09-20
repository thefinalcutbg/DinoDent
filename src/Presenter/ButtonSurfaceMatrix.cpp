#include "ButtonSurfaceMatrix.h"

ButtonSurfaceMatrix::ButtonSurfaceMatrix()
{
	map[0] = upRightPost;
	map[1] = upRightPost;
	map[2] = upRightPost;
	map[3] = upRightPost;
	map[4] = upRightPost;
	map[5] = upRightFront;
	map[6] = upRightFront;
	map[7] = upRightFront;
	map[8] = upLeftFront;
	map[9] = upLeftFront;
	map[10] = upLeftFront;
	map[11] = upLeftPost;
	map[12] = upLeftPost;
	map[13] = upLeftPost;
	map[14] = upLeftPost;
	map[15] = upLeftPost;
	map[16] = lowLeftPost;
	map[17] = lowLeftPost;
	map[18] = lowLeftPost;
	map[19] = lowLeftPost;
	map[20] = lowLeftPost;
	map[21] = lowLeftFront;
	map[22] = lowLeftFront;
	map[23] = lowLeftFront;
	map[24] = lowRightFront;
	map[25] = lowRightFront;
	map[26] = lowRightFront;
	map[27] = lowRightPost;
	map[28] = lowRightPost;
	map[29] = lowRightPost;
	map[30] = lowRightPost;
	map[31] = lowRightPost;
}

int ButtonSurfaceMatrix::getSurface(int toothIndex, ButtonPos position){ 
	return map[toothIndex][static_cast<int>(position)];
}

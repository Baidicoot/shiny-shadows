#include "Image.h"

using namespace std;

vector<unsigned char> genSpriteSheetRGBA(unsigned char* data, int subwidth, int subheight, int images_x, int images_y) {
	vector<unsigned char> dataOut;
	int width = subwidth * images_x;
	for (int index_y = 0; index_y < images_y; index_y++) {
		for (int index_x = 0; index_x < images_x; index_x++) {
			for (int local_y = 0; local_y < subheight; local_y++) {
				for (int local_x = 0; local_x < subwidth; local_x++) {
					int global_x = (subwidth * index_x) + local_x;
					int global_y = (subheight * index_y) + local_y;
					int start_index = global_x + (width * global_y);
					int start_loc = start_index * 4;
					dataOut.push_back(data[start_loc]);
					dataOut.push_back(data[start_loc + 1]);
					dataOut.push_back(data[start_loc + 2]);
					dataOut.push_back(data[start_loc + 3]);
				}
			}
		}
	}
	return dataOut;
}
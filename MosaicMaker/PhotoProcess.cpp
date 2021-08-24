//
//  PhotoProcess.cpp
//  OpenCVPractice
//
//  Created by Devin Muzzy on 7/1/21.
//

#include "PhotoProcess.hpp"

using namespace cv;

typedef Vec<uint, 3> VecHSV;

/*
 * takes Mat img to the source image to be created
 * param resolution, the number of x chunks to make and y chunks to make, note that chunks must be square
 */
vector<Vec3b> chunkify(Mat img, int xRes, int yRes){
    int chunkSizeX, chunkSizeY;
    chunkSizeX = img.cols / xRes;
    chunkSizeY = img.rows / yRes;
    if(chunkSizeX != chunkSizeY){
        throw "cannot process non square chunks inside chunkfiy";
    }
    vector<Vec3b> rVec;
    for(int y = img.rows; y > 0; y-=chunkSizeY){
        for(int x = img.cols; x > 0; x-=chunkSizeX){
            rVec.push_back(colorValue(img, Size(img.cols, img.rows), x - chunkSizeX, x, y - chunkSizeY, y));
        }
    }
    return rVec;
}

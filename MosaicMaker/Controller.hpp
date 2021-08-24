//
//  Controller.hpp
//  OpenCVPractice
//
//  Created by Devin Muzzy on 7/1/21.
//

#ifndef Controller_hpp
#define Controller_hpp

#include <stdio.h>
#include "LibraryPrep.hpp"
#include "PhotoProcess.hpp"
#include <stdio.h>
#include <algorithm>
#include <math.h>

using namespace oct;

int distance(ImgObj imO, cv::Vec3b v);
string bestMatch(vector<ImgObj>* iVec, cv::Vec3b cVec);
int gcf(int a, int b);
int commonVector(int factor, int max);
void makeMosaic(cv::Mat* mosaic, Octree<ImgObj>* octree, string path, int clarity, int PIXEL_SIZE);
int main(int argc, char** argv);


#endif /* Controller_hpp */

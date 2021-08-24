//
//  PhotoProcess.hpp
//  OpenCVPractice
//
//  Created by Devin Muzzy on 7/1/21.
//

#ifndef PhotoProcess_hpp
#define PhotoProcess_hpp

#include <stdio.h>
#include "LibraryPrep.hpp"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <map>

vector<cv::Vec3b> chunkify(cv::Mat img, int xRes, int yRes);

#endif /* PhotoProcess_hpp */





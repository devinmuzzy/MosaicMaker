//
//  LibraryPrep.hpp
//  OpenCVPractice
//
//  Created by Devin Muzzy on 7/1/21.
//

#ifndef LibraryPrep_hpp
#define LibraryPrep_hpp

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <math.h>
#include <map>
#include <stdio.h>
#include "Octree.cpp"


vector<cv::String> getImages(string pattern);
struct HSVPos;
bool compareHSV(HSVPos hp1, HSVPos hp2);
cv::Vec3s rgbToHsv(cv::Vec3b rgb);
typedef cv::Vec<uint, 3> VecHSV;
vector<VecHSV> getNineVals(cv::Mat img);
void fillMap(string folder, map<uint, vector<HSVPos>> * hashTable);
cv::Vec3b colorValue(cv::Mat img, cv::Size rsize, uint x1,  uint x2,  uint y1,  uint y2);
cv::Vec3b colorValue(cv::Mat img, cv::Size rsize);
void tile(cv::Mat imgArr[], cv::Mat *dest, int width, int height, cv::Size dsize);
void libMaker();
struct ImgObj{
    string path;
    cv::Vec3b rgb;
    int r;
    int g;
    int b;
    ImgObj(int r, int g, int b, cv::Vec3b rgb, string path);
};
void fillTree(oct::Octree<ImgObj>* ImgTree, vector<string> imagePaths);

#endif /* LibraryPrep_hpp */

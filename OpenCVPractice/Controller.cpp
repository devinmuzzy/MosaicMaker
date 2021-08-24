//
//  Controller.cpp
//  OpenCVPractice
//
//  Created by Devin Muzzy on 7/1/21.
//

#include "Controller.hpp"
#include <sys/stat.h>

using namespace cv;

// calculates "distance" between two points in 3d space
int distance(ImgObj imO, Vec3b v){
    return abs(imO.r - v[0]) + abs(imO.g - v[1]) + abs(imO.b - v[2]);
}

// finds the closest point in the vector to the cVec point
string bestMatch(vector<ImgObj>* iVec, Vec3b cVec){
    string cPath;
    if(iVec->size() == 1){
        return iVec->at(0).path;
    }
    int best, current;
    ImgObj* bestImg;
    bestImg = &(iVec->at(0));
    best = distance(iVec->at(0), cVec);
    for(int i = 1; i < iVec->size(); i++){
        current = distance(iVec->at(i), cVec);
        if(current < best){
            best = current;
            bestImg = &(iVec->at(i));
        }
    }
    return bestImg->path;
}

//return greatest common factor
int gcf(int a, int b){
    if (a == b){
        return a;
    }
    if (a == 0 || b == 0){
        return 0;
    }
    if (a > b){
        return gcf(a - b, b);
    }
    else{
        return gcf(a, b - a);
    }
}

int commonVector(int factor, int max){
    vector<int> rVec;
    for(int i = 1; rVec.size() <= max; i++){
        if(factor % i == 0){
            rVec.push_back(i);
        }
    }
    return rVec.at(rVec.size() - 1);
}


// using the given octree and source image, a mosaic is created and saved to the save location
void makeMosaic(Mat* mosaic, Octree<ImgObj>* octree, string path, int clarity, int PIXEL_SIZE){ // cx is x clarity, cy is y clarity (ish)
    Vec3b cVec;
    string cPath;
    int pixel_count, factor, cx, cy;
    Mat resizedImg;
    vector<ImgObj>* imgVec;
    Mat img = imread(path);
    factor = gcf(img.cols, img.rows);
    cx = img.cols / factor;
    cy = img.rows / factor;
    clarity = commonVector(factor, clarity);
    cx *= clarity;
    cy *= clarity;
    vector<Vec3b> rVec = chunkify(img, cx, cy); // needs to be more automatics
    pixel_count = rVec.size();
    printf("estimated time: %.2f minutes\n", ((double)(rVec.size() * 270)) / 60000.0);
    Mat preTileArr[rVec.size()];
    // for each color subregion find the nearest match
    for(int i = 0; i < rVec.size(); i++){
        cVec = rVec.at(i);
        imgVec = (octree)->nearestNeighbors(cVec[0], cVec[1], cVec[2])->data;
        cPath = bestMatch(imgVec, cVec);
        resizedImg = imread(cPath);
        if(resizedImg.size != 0)
            resize(resizedImg, resizedImg, Size(PIXEL_SIZE,PIXEL_SIZE));
        preTileArr[i] = resizedImg;
        if(i % 100 == 0){
            printf("%d pixels left to fill\n", pixel_count - i);
        }
    }
    printf("tiling pixels...\n");
    tile(preTileArr, mosaic, cx, cy, Size(PIXEL_SIZE,PIXEL_SIZE));
}


/*
 * stores the preferences and octree set during initialization
 */
class Looper{
private:
    Octree<ImgObj>* octree;
    RegionNode* rootPtr;
    string srcPath;
    string savePath;
    int clarity;
    int PIXEL_SIZE = 50;
    
public:
    Looper(string libPath){
        int space[6] = {0, 255, 0, 255, 0, 255};
        rootPtr = new RegionNode(space, 0);
        this->octree = new Octree<ImgObj>((*this->rootPtr));
        this->octree->MAX_HEIGHT = 5;
        fillTree(this->octree, getImages(libPath));
    }
    void setSrc(string srcPath){
        this->srcPath = srcPath;
    }
    void setSave(string savePath){
        this->savePath = savePath;
    }
    void setClarity(int clarity){
        this->clarity = clarity;
    }
    void createMosaic(){
        Mat mosaicImg;
        makeMosaic(&mosaicImg, this->octree, this->srcPath, this->clarity, this->PIXEL_SIZE);
        imwrite(savePath, mosaicImg);
    }
};

/*
 * usage: "path/to/lib" "/path/to/img" "path/to/save" clarity
 */
int main(int argc, char** argv){
    int PIXEL_SIZE = 200;
    int DEFAULT_MAX = 5;
    printf("\n\n");
    printf("MosaicMaker\n");
    printf("\n\n");
    int clarity = 5;
    string libPath;
    string imgPath;
    string savePath;
    while(true){
        printf("\npress [r] to begin the program, or [s] to adjust settings\n");
        string t;
        cin >> t;
        if(t != "s" && t != "r"){
            printf("please try again\n\n");
            continue;
        }
        if(t == "r"){
            break;
        }
        if(t == "s"){
            printf("choose a setting to change:\n");
            printf("\t[c] sub-image clarity: %d\n\t[h] octree height: %d\n\t[e] exit without changes\n", PIXEL_SIZE, DEFAULT_MAX);
            while(true){
                string t;
                cin >> t;
                if(t == "e")
                    break;
                if(t == "c"){
                    printf("\nenter the desired sub-clarity:\n");
                    cin >> PIXEL_SIZE;
                }
                if(t == "h"){
                    printf("\nenter the desired height:\n");
                    cin >> DEFAULT_MAX;
                }
                printf("press [e] to exit, or [h] or [c] to change the other setting\n");
                continue;
            }
            printf("new settings are set to:\n");
            printf("\t[c] sub-image clarity: %d\n\t[h] octree height: %d\n", PIXEL_SIZE, DEFAULT_MAX);
        }
    }
    string trash;
    getline(cin, trash);
    while(true){
        printf("\nenter the path to your image library folder:\n");
        getline(cin, libPath);
        
        // create octree
        struct stat buffer;
        if(stat(libPath.c_str(),  &buffer)){
            printf("\nPath: \"%s\" does not exist\n", &libPath);
            printf("make sure the path ends with \"/\" and is not empty\n");
            continue;
        }
        break;
    }
    printf("\nbuilding tree...\n");
    Octree<ImgObj>* octree;
    int space[6] = {0, 255, 0, 255, 0, 255};
    RegionNode root(space, 0);
    octree = new Octree<ImgObj>(root);
    octree->MAX_HEIGHT = DEFAULT_MAX;
    fillTree(octree, getImages(libPath));
    while(true){
        printf("\nenter the path to the source image:\n");
        getline(cin, imgPath);
        if(imread(imgPath).data == NULL){
            printf("\nImage does not exist\n");
            continue;
        }
        printf("\nenter the desired save location:\n");
        getline(cin, savePath);
        printf("\nenter the desired clarity (maximum around 10 depending on processing power):\n");
        cin >> clarity;
        Mat mosaicImg;
        makeMosaic(&mosaicImg, octree, imgPath, clarity, PIXEL_SIZE);
        imwrite(savePath, mosaicImg);
        printf("\ndone\n");
        printf("\nwould you like to mosaic another image? [y/n] \n");
        string t;
        cin >> t;
        if(t == "y"){
            continue;
        }
        printf("exiting...\n\n");
        exit(0);
    }
}

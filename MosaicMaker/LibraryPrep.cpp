//
//  LibraryPrep.cpp
//  OpenCVPractice
//
//  Created by Devin Muzzy on 7/1/21.
//

#include "LibraryPrep.hpp"

using namespace std;
using namespace cv;
using namespace oct;

typedef Vec<uint, 3> VecHSV;


/*
 * searches for all files matching the pattern
 * returns a vector with their paths
 */
vector<String> getImages(string pattern){
    vector<String> finalResult;
    vector<String> result;    
    glob(pattern + "*.png", result, true);
    finalResult.insert(finalResult.end(), result.begin(), result.end());
    glob(pattern + "*.jpg", result, true);
    finalResult.insert(finalResult.end(), result.begin(), result.end());
    glob(pattern + "*.JPG", result, true);
    finalResult.insert(finalResult.end(), result.begin(), result.end());
    glob(pattern + "*.jpeg", result, true);
    finalResult.insert(finalResult.end(), result.begin(), result.end());

    return finalResult;
}


/*
 * note: should be changed to accept vec3b as constructor in future
 */
struct HSVPos{
    public: uint hue, pos, sat, val;
    public: vector<VecHSV> nine_square;
    public: string path;
    HSVPos(uint hue, uint sat, uint val, uint pos, vector<VecHSV> nine_square, string path){
        this->hue = hue;
        this->pos = pos;
        this->sat = sat;
        this->val = val;
        this->nine_square = nine_square;
        this->path = path;
    }
};

/*
 * comparator used for sorting vectors<HSVPos>
 */
bool compareHSV(HSVPos hp1, HSVPos hp2){
    if(abs(hp1.hue - hp2.hue) < 5){
        return(hp1.val < hp2.val); // should this include saturation??
    }
    else{
        return (hp1.hue < hp2.hue);
    }
}

Vec3s rgbToHsv(Vec3b rgb){
    double r,g,b,r1,g1,b1;
    double Cmax, Cmin;
    r = 0;
    g = 0;
    b = 0;
    r = rgb[0];
    g = rgb[1];
    b = rgb[2];
    r1 = r / 255.0;
    g1 = g / 255.0;
    b1 = b / 255.0;
    Cmax = max({r1,g1,b1});
    Cmin = min({r1,g1,b1});
    double delta = Cmax - Cmin;
    double hue;
    if(Cmax == r1){
        hue = 60.0 * fmod(((g1 - b1) / delta),6);
    }
    else if(Cmax == g1){
        hue = 60.0 * (((b1-r1)/delta) + 2);
    }
    else if(Cmax == b1){
        hue = 60.0 * (((r1-g1)/delta) + 4);
    }
    else{
        hue = 99999980085999;
    }
    double s;
    if(Cmax == 0){
        s = 0;
    }
    else{
        s = delta / Cmax;
    }
    s *= 100;
    Cmax *= 100;
    return Vec3b(hue, s, Cmax);
}


ImgObj::ImgObj(int r, int g, int b, Vec3b rgb, string path){
    this->r = r;
    this->g = g;
    this->b = b;
    this->rgb = rgb;
    this->path = path;
}


// param imagePaths is a vector of strings that for all corrosponding image paths
void fillTree(Octree<ImgObj>* ImgTree, vector<string> imagePaths){
    vector<HSVPos> imageColors;
    // add images to hashMap vectors
    int r,g,b;
    for(int i = 0; i <  imagePaths.size(); i++){
        string path = (string) imagePaths.at(i);
        Vec3b rgb = colorValue(imread(path), Size(20,20)); // does size matter here if resize automatically find s the avg color??
        r = rgb[0];
        g = rgb[1];
        b = rgb[2];
    
        ImgObj* imO = new ImgObj(r, g, b, rgb, path);
        ImgTree->insert(new DataNode<ImgObj>(r,g,b,*imO));
        
    }
}
   
/*
 * @params img, rsize
 * x1, x2, y1, y2 create the rectangle that the color is averaged from after resizing
 * @returns Vec3b of average pixel values in RGB form
 */

Vec3b colorValue(Mat img, Size rsize, uint x1,  uint x2,  uint y1,  uint y2){
    resize(img, img, rsize);
    uint r, g, b;
    r = 0;
    b = 0;
    g = 0;
    for(int y = y1; y < y2; y++){
        for(int x = x1; x < x2; x++){
            Vec3b color = img.at<Vec3b>(Point(x,y));
            b += color[0];
            g += color[1];
            r += color[2];
        }
    }
    int area = (x2-x1) * (y2-y1);
    uchar u_r = r / area;
    uchar u_g = g / area;
    uchar u_b = b / area;
    Vec3b rVec(u_r,u_g,u_b);
    return rVec;
}
    
// overloaded versions for more flexibility
Vec3b colorValue(Mat img, Size rsize){
    return colorValue(img, rsize, 0, rsize.width, 0, rsize.height);
}

/*
 * Width is width of desired dest image ( measured in images of Size  dsize )
 * Height is height of desried dest image
 */
void tile(Mat imgArr[], Mat *dest, int width, int height, Size dsize){
    int index;
    vector<Mat> rowVect;
    vector<Mat> colVect; // array of the concatenated row photos
    Mat row;
    for(int j = height - 1; j >= 0; j--){
        for(int i = width - 1; i >= 0; i--){
            index = j * width + i;
            resize(imgArr[index], imgArr[index], dsize);
            rowVect.push_back(imgArr[index]);
        }
        hconcat(rowVect, row); // row now contians all the images in row 1
        Mat r; // possibly should be here not the top because of scope?
        row.copyTo(r);
        colVect.push_back(r);
        rowVect.clear();
    }
    vconcat(colVect, *dest);
}

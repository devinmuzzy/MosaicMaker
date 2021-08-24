//
//  Octree.cpp
//  OpenCVPractice
//
//  Created by Devin Muzzy on 7/8/21.
//
// control flow for node location
// right branch if conditional is true, left otherwise
// r is the max x,y,z value of the region divided by 2
//
//                                      code increase for right branch:
//                  x < r
//                /       \                 + 4
//          y < r           y < r
//        /      \         /      \         + 2
//    z < r     z < r    z < r    z < r
//    /   \     /   \    /   \    /   \     + 1
//   n0   n1   n2   n3  n4   n5  n6    n7
// codes binary:
//  000  001  010  011  100 101  110  111
// codes in decimal:
//  0     1    2    3    4    5    6    7
// cuboid bounds are in the order: xMin, xMax, yMin, yMax, zMin, int zMax
//                                  0      1     2    3     4      5

#include "Octree.hpp"

using namespace std;
namespace oct{
// base class for polymorphic capabilities
class Node{
public:
    int type = -1;
    Node(){
        this->type = 0;
    }
    virtual int getType(){
        return this->type;
    }
};

// node type 1
class RegionNode: public virtual Node{
public:
    // cuboid bounds are in the order: xMin, xMax, yMin, yMax, zMin, int zMax
    RegionNode(int cuboidBounds[6], int depth){
        loadBound(cuboidBounds);
        this->type = 1;
        this->depth = depth;
        for(int i = 0; i < 8; i++){
            this->subRegion[i] = nullptr;
        }
    }
private:
    void loadBound(int cuboidBounds[6]){
        for(int i = 0; i < 6; i++){
            this->cuboidBounds[i] = cuboidBounds[i];
        }
    }
    virtual int getType(){
        return this->type;
    }
public:
    int depth;
    int level;
    int cuboidBounds[6];
    // points to an array of Node*
    Node* subRegion[8];
};

template <typename D>
// node type 2
class DataNode : public virtual Node {
public:
    DataNode(){
        this->type = -1;
    }
    DataNode(int x, int y, int z){
        this->x = x;
        this->y = y;
        this->z = z;
        this->vec_size = 0;
        this->type = 2;
    }
    DataNode(int x, int y, int z, D data){
        this->x = x;
        this->y = y;
        this->z = z;
        this->vec_size = 1;
        this->type = 2;
        this->data->push_back(data);
    }
    virtual int getType(){
        return this->type;
    }
public:
    int type;
    int x = NULL;
    int y = NULL;
    int z = NULL;
    int vec_size;
    vector<D>* data = new vector<D>;
};

template <typename D>
class Octree{
public:
    int MAX_HEIGHT = def_MAX_HEIGHT; // allows for user control of max height
private:
    RegionNode* root;
    int height;
    int size; // how many nodes
    public:
    Octree(RegionNode root){
        this->root = &root;
        this->height = 0;
        this->size = 0;
    }
private:
    // recursive form of contains at
    vector<D>* containsAt(int x, int y, int z, RegionNode* region){
        int code = getCode(new DataNode<D>(x,y,z), region);
        if((region->subRegion[code]) == NULL){
            return nullptr;
        }
        // if DataNode
        else if(region->subRegion[code]->getType() == 2){
            return dynamic_cast<DataNode<D>*>(region->subRegion[code])->data;
        }
        // if RegionNode
        else if(region->subRegion[code]->getType() == 1){
            return containsAt(x, y, z, dynamic_cast<RegionNode*>(region->subRegion[code]));
        }
        else{
            throw "unreachable code reached in containsAt()";
        }
    }
public:
    //note: if your point falls into a bucket it will return the vector belonging to that bucket even if your point does not specifically exist
    // returns nullptr if no DataNode is found at the point / point bucket
    vector<D>* containsAt(int x, int y, int z){
        return containsAt(x, y, z, this->root);
    }
private:
    // returns the code corrosponding to the cuboid within the region the node belongs in
    int getCode(DataNode<D> *node, RegionNode* region){
        int code = 0;
        code = 0;
        int step;
        step = (((region)->X_MAX - (region)->X_MIN)/2);
        // find the region the node belongs in
        if(node->x >= (((region)->X_MAX) - step)){
            code += 4;
        }
        step = (((region)->Y_MAX - (region)->Y_MIN)/2);
        if(node->y >= (((region)->Y_MAX) - step)){
            code += 2;
        }
        step = (((region)->Z_MAX - (region)->Z_MIN)/2);
        if(node->z >= (((region)->Z_MAX) - step)){
            code += 1;
        }
        return code;
    }
private:
    // creates a copy on the heap of the node for non-local use
    DataNode<D>* nodeCopy(DataNode<D>* oldNode){
        DataNode<D>* rNode = new DataNode<D>(oldNode->x, oldNode->y, oldNode->z, oldNode->data->at(0));
        return rNode;
    }
private:
    // finds the new bounds
    RegionNode* newBounds(RegionNode* region, int code){
        int newCuboidBounds[6] = {-1,-1,-1,-1,-1,-1};
        int step;
        step = (((region)->Z_MAX - (region)->Z_MIN)/2);
        if((code & 1) == 1){
            newCuboidBounds[4] = (region)->Z_MAX - step;
            newCuboidBounds[5] = (region)->Z_MAX;
        }
        else{
            newCuboidBounds[4] = (region)->Z_MIN;
            newCuboidBounds[5] = (region)->Z_MAX - step;
        }
        step = (((region)->Y_MAX - (region)->Y_MIN)/2);
        code = code >> 1;
        if((code & 1) == 1){
            newCuboidBounds[2] = (region)->Y_MAX - step;
            newCuboidBounds[3] = (region)->Y_MAX;
        }
        else{
            newCuboidBounds[2] = (region)->Y_MIN;
            newCuboidBounds[3] = (region)->Y_MAX - step;
        }
        code = code >> 1;
        step = (((region)->X_MAX - (region)->X_MIN) / 2) ;
        if((code & 1) == 1){
            newCuboidBounds[0] = (region)->X_MAX - step;
            newCuboidBounds[1] = (region)->X_MAX;
        }
        else{
            newCuboidBounds[0] = (region)->X_MIN;
            newCuboidBounds[1] = (region)->X_MAX - step;
        }
        for(int i = 0; i < 6; i+=2){
            if(newCuboidBounds[i] == newCuboidBounds[i+1]){
                printf("invalid bounds error\n");
            }
        }
        RegionNode* newRegion = new RegionNode(newCuboidBounds, (region)->depth+1); // new keyword is needed to prevent local only scope
        return newRegion;
    }
private:
    // recursively inserts the node into the tree
    void insert(DataNode<D> *node, RegionNode* region){
        int code;
        Node** destinationRegion;
        code = getCode(node, region);
        destinationRegion = &((region)->subRegion[code]);
        // if region is empty insert node into region
        if(*destinationRegion == NULL){
            *destinationRegion = node;
            this->size += 1;
            return;
        }
        // if region contains RegionNode recursively search that RegionNode
        if((*destinationRegion)->getType() == 1){
            insert(node, (dynamic_cast<RegionNode*>(*destinationRegion))); // type casting from Node to ReginoNode
            return;
        }
        // if correct region contains a DataNode add node
        if((*destinationRegion)->getType() == 2){
            DataNode<D>* accessNode = (dynamic_cast<DataNode<D>*>(*destinationRegion));
            // create and add a new RegionNode to the tree
            // newCuboidBounds are defined by the code obtained above
            if(region->depth < MAX_HEIGHT){
                *destinationRegion = newBounds(region, code);
                insert(nodeCopy(node), dynamic_cast<RegionNode*>(*destinationRegion));
                insert(nodeCopy(accessNode), dynamic_cast<RegionNode*>(*destinationRegion));
                return;
            }
            // allows for tree size control and duplicate points in space
            else{
                accessNode->vec_size = accessNode->vec_size + 1;
                accessNode->data->push_back((node)->data->at(0));
                return;
            }
        }
        throw "impossible to reach code reached in insert()";
    }
public:
    // allows the caller to insert node
    void insert(DataNode<D>* node){
        insert(node, (this->root));
    }
private:
    vector<DataNode<D>*> firstDataNode(RegionNode* region){
        vector<DataNode<D>*> rVec;
        for(int i = 0; i < 8; i++){
            if(region->subRegion[i] != NULL && region->subRegion[i]->getType() == 2)
                rVec.push_back((dynamic_cast<DataNode<D>*>(region->subRegion[i])));
        }
        if(rVec.size() == 0){
            for(int i = 0; i < 8; i++){
                if(region->subRegion[i] != NULL && region->subRegion[i]->getType() == 1)
                    return firstDataNode((dynamic_cast<RegionNode*>(region->subRegion[i])));
            }
        }
        return rVec;
    }
    
    int distance(DataNode<D>* d1, int x, int y, int z){
        return abs(d1->x - x) + abs(d1->y - y) + abs(d1->z - z);
    }
    
    
    // returns all the DataNodes in the region if no exact match is found
    DataNode<D>* nearestNeighbors(int x, int y, int z, RegionNode* region, vector<DataNode<D>*> bestSoFar){
        int code = getCode(new DataNode<D>(x,y,z), region);
        vector<DataNode<D>*> rVec;
        if((region->subRegion[code]) == NULL){
            for(int i = 0; i < 8; i++){
                if(region->subRegion[i] != NULL && region->subRegion[i]->getType() == 2)
                    rVec.push_back((dynamic_cast<DataNode<D>*>(region->subRegion[i])));
            }
            if(rVec.size() == 0){
                for(int i = 0; i < 8; i++){
                    if(region->subRegion[i] != NULL && region->subRegion[i]->getType() == 1)
                        rVec = firstDataNode((dynamic_cast<RegionNode*>(region->subRegion[i])));
                }
            }
            
            // find the best Node in the vector
            if(rVec.size() > 1){
                int best, current;
                DataNode<D>* bestNode;
                best = distance(rVec.at(0), x, y, z);
                bestNode = rVec.at(0);
                for(int i = 1; i < rVec.size(); i++){
                    current = (distance(rVec.at(i), x, y, z));
                    if (current < best){
                        best = current;
                        bestNode = rVec.at(i);
                    }
                }
                return bestNode;
            }
            else{ // rVec only has one element
                return rVec.at(0);
            }
        }
        // if DataNode
        else if(region->subRegion[code]->getType() == 2){
            return dynamic_cast<DataNode<D>*>(region->subRegion[code]);
        }
        // if RegionNode
        else if(region->subRegion[code]->getType() == 1){
            return nearestNeighbors(x, y, z, dynamic_cast<RegionNode*>(region->subRegion[code]), bestSoFar);
        }
        else{
            throw "unreachable code reached in containsAt()";
        }
    }
    
public:
    DataNode<D>* nearestNeighbors(int x, int y, int z){
        vector<DataNode<D>*> v;
        v.push_back(nullptr);
        DataNode<D>* t = nearestNeighbors(x, y, z, this->root, v);
        if (t == nullptr)
            throw "No Neighbor found";
        return t;
    }
    
};
};

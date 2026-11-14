#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here

bool checkPaths(Node*root, int d, int& depthOfLeaf){
     
    if(root == NULL){
        return true;
    }
    if((root->left == NULL)&&(root->right == NULL)){
        if(depthOfLeaf == -1){
            depthOfLeaf = d;
            return true;
        }
        return d == depthOfLeaf;

    }

    return checkPaths(root->left, d + 1, depthOfLeaf) && checkPaths(root->right, d + 1, depthOfLeaf);

}


bool equalPaths(Node * root)
{
    // Add your code below
   int depthOfLeaf = -1;
   return checkPaths(root, 0, depthOfLeaf);

}


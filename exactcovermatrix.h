#ifndef EXACTCOVERMATRIX_H
#define EXACTCOVERMATRIX_H

#include<iostream>
using namespace std;

struct Node
{
    Node* left, *right, *up, *down;
    int col, row;
    Node(){
        left = NULL; right = NULL;
        up = NULL; down = NULL;
        col = 0; row = 0;
    }
    Node( int r, int c )
    {
        left = NULL; right = NULL;
        up = NULL; down  = NULL;
        col = c; row = r;
    }
};

struct ColunmHeader : public Node
{
    int size;
    ColunmHeader(){
        size = 0;
    }
};

class ExactCoverMatrix
{
    public:
        int ROWS, COLS;
        //This is the array where the results are stored
        int* disjointSubet;
        //Receive matrix and its dimensions
        ExactCoverMatrix( int rows, int cols, int** matrix );
        //free memory
        ~ExactCoverMatrix();
        //Insert an element at row r column c position
        void insert( int r, int c );
        //That is, our cover and uncover operations are done
        void cover( int c );
        void uncover( int c );
        //i.e our algorithm X , the realization of X
        int search( int k=0 );
    private:
        ColunmHeader* root;
        ColunmHeader* ColIndex;
        Node* RowIndex;
};

#endif // EXACTCOVERMATRIX_H

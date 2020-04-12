#ifndef ARCHIVATORS_HUFFMAN_H
#define ARCHIVATORS_HUFFMAN_H

#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <map>
#include <utility>

using namespace std;
typedef unsigned char uchar;
typedef unsigned long long ull;


class Huffman {
public:
    void compress(string path);

    void decompress(string path);

private:
    struct Node
    {
        uchar c;
        ull val;
        int index;
        Node* left;
        Node* right;

        Node(ull val, Node* left, Node* right)
        {
            this->val = val;
            this->left = left;
            this->right = right;
            index = -1;
            c = 0;
        }
    };

    void read_original_file(string path);
    void add_nodes();
    void build_tree();
    void set_library(Node* node, string res);
    void write_compressed(string path);

    vector<Node*> nodes;
    Node* root;
    map<uchar, string> library;
    vector<pair<uchar, ull>> freqs;
    ull bytes;
};

#endif //ARCHIVATORS_HUFFMAN_H

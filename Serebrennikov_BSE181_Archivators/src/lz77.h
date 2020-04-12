#ifndef ARCHIVATORS_LZ77_H
#define ARCHIVATORS_LZ77_H

#include <iostream>
#include <fstream>
#include <utility>
#include <vector>

using namespace std;
typedef unsigned long long ull;
typedef unsigned char uchar;
typedef unsigned short ushort;

class LZ77Encoder {
public:
    LZ77Encoder(int buffer, int search)
    {
        this->search = search;
        this->preview = buffer - search;
    }

    void compress(string path);

    void decompress(string path);

private:
    union UN {
        ushort a;
        uchar b[2];
    };

    struct Node
    {
        UN offset;
        UN length;
        uchar chr;

        Node()= default;

        Node(int offset, int length, char chr)
        {
            this->offset.a = (ushort)offset;
            this->length.a = (ushort)length;
            this->chr = chr;
        }
    };

    void read_original_file(string path);
    int find_substring(string& prefix, int currentBufferBorder, int leftBorder, int rightBorder);
    void set_library();
    void write_compressed(string path);

    string input;
    vector<Node> lib;
    ull bytes;

    int search;
    int preview;
};

#endif //ARCHIVATORS_LZ77_H

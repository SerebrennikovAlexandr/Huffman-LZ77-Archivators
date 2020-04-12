#include "huffman.h"

//================= Дополнительные методы =======================

bool cmpfunc(const pair<uchar, ull>& a, const pair<uchar, ull>& b)
{
    return a.second > b.second;
}

//====================== Huffman =============================

void Huffman::read_original_file(string path)
{
    bytes = 0;
    ifstream fin;

    freqs.resize(256, make_pair(0, 0));
    for(uchar i = 1; i != 0; i++)
        freqs[i].first = i;

    fin.open(path, ios::binary);
    if(fin.is_open())
    {
        uchar c;
        c = fin.get();
        while(!fin.eof())
        {
            freqs[c].second++;
            bytes++;
            c = fin.get();
        }
    }
    fin.close();

    sort(freqs.begin(), freqs.end(), cmpfunc);

    while(freqs[freqs.size()-1].second == 0)
        freqs.pop_back();
}

void Huffman::add_nodes()
{
    nodes.clear();
    for(int i = 0; i < freqs.size(); i++)
    {
        Node* node = new Node(freqs[i].second, nullptr, nullptr);
        node->index = i;
        node->c = freqs[i].first;
        nodes.push_back(node);
    }
}

void Huffman::build_tree()
{
    while(nodes.size() > 1)
    {
        Node* right = nodes[nodes.size() - 1];
        nodes.pop_back();

        Node* left = nodes[nodes.size() - 1];
        nodes.pop_back();

        nodes.push_back(new Node(left->val + right->val, left, right));

        for(int i = nodes.size()-1; i > 0; i--)
        {
            if(nodes[i-1]->val >= nodes[i]->val)
                break;
            swap(nodes[i-1], nodes[i]);
        }
    }
    root = nodes[0];
}

void Huffman::set_library(Huffman::Node* node, string res)
{
    if(node == nullptr)
        return;

    if(node->index != -1)
        library[node->c] = res;

    set_library(node->left, res + "1");
    set_library(node->right, res + "0");
}

void Huffman::write_compressed(string path)
{
    ifstream fin;
    ofstream fout;
    string bits;
    string outpath = path.substr(0, path.rfind('.')) + ".haff";

    fin.open(path, ios::binary);
    if(fin.is_open())
    {
        uchar c;
        c = fin.get();
        while(!fin.eof())
        {
            bits += library[c];
            c = fin.get();
        }
    }
    fin.close();

    int size = freqs.size();
    int add = bits.length() % 8;
    for(int i = 0; i < 8 - add; i++)
    {
        bits += "0";
    }

    fout.open(outpath, ios::binary);
    if(fout.is_open())
    {
        fout << size << " " << bytes << "\n";
        for(auto p : freqs)
        {
            fout << p.first << endl;
            fout << p.second << endl;
        }

        for(int i = 0; i < bits.length(); i+=8)
        {
            uchar res = 0;
            uchar temp = 1;
            for(int j = i + 7; j >= i; j--)
            {
                if(bits[j] == '1')
                    res += temp;
                temp *= 2;
            }
            fout << res;
        }
    }
    fout.close();
}

void Huffman::compress(string path)
{
    string filename;
    if(!path.rfind('/'))
        filename = path;
    else
        filename = path.substr(path.rfind('/') + 1);

    cout << "\tCompressing file " + filename + ":\n";

    read_original_file(path);

    add_nodes();
    build_tree();

    library.clear();
    set_library(root, "");

    write_compressed(path);

    cout << "\tFile compressed!\n";
}

void Huffman::decompress(string path)
{
    string filename;
    if(!path.rfind('/'))
        filename = path;
    else
        filename = path.substr(path.rfind('/') + 1);

    cout << "\tDecompressing file " + filename + ":\n";

    freqs.clear();
    library.clear();
    ifstream fin;
    ofstream fout;
    string outpath = path.substr(0, path.rfind('.')) + ".unhaff";
    string bits;

    fin.open(path, ios::binary);
    if(fin.is_open())
    {
        int size;
        uchar c;
        fin >> size >> bytes;
        c = fin.get();
        for(int i = 0; i < size; i++)
        {
            ull amount;
            c = fin.get();
            fin >> amount;
            freqs.emplace_back(c, amount);
            c = fin.get();
        }

        c = fin.get();
        while(!fin.eof())
        {
            string temp;
            uchar t = 128;
            while(t > 0)
            {
                if(c & t)
                    temp += "1";
                else
                    temp += "0";
                t = t >> 1;
            }
            bits += temp;
            c = fin.get();
        }

        add_nodes();
        build_tree();

        fout.open(outpath, ios::binary);
        if(fout.is_open())
        {
            ull count = 0;
            Node* node = root;
            for(ull i = 0; count < bytes; i++)
            {
                if(node->index != -1)
                {
                    fout << node->c;
                    node = root;
                    count++;
                }
                if(bits[i] == '1')
                    node = node->left;
                else
                    node = node->right;
            }
        }
        fout.close();
    }
    fin.close();

    cout << "\tFile decompressed!\n";
    cout << "\n";
}

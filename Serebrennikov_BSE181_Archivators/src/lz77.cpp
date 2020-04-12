#include "lz77.h"

void LZ77Encoder::read_original_file(string path)
{
    input.clear();
    bytes = 0;

    ifstream fin;
    fin.open(path, ios::binary);
    if(fin.is_open())
    {
        char c;
        c = fin.get();
        while(!fin.eof())
        {
            input += c;
            bytes++;
            c = fin.get();
        }
        fin.close();
    }
}

int LZ77Encoder::find_substring(string &prefix, int currentBufferBorder, int leftBorder, int rightBorder)
{
    int res = -1;
    int i;
    for(i = leftBorder; i < currentBufferBorder; i++)
    {
        bool found = false;
        for(int j = 0; j < prefix.length() && i + j < rightBorder; j++)
        {
            if(prefix[j] != input[i + j])
                break;
            if(j == prefix.length() - 1 && prefix[j] == input[i + j])
            {
                res = i;
                found = true;
            }
        }
        if(found)
            break;
    }
    return res;
}

void LZ77Encoder::set_library()
{
    lib.clear();
    int currentBufferBorder = 0;

    // Идём по строке границей между предпросмотром и поиском
    while(currentBufferBorder < input.length())
    {
        // Задаём для буфера граничные левый и правый индексы
        int rightBorder = min((int)input.length(), currentBufferBorder + preview);
        int leftBorder = max(0, currentBufferBorder - search);

        int offset = 0, length = 0;
        char chr = input[currentBufferBorder];
        for(int i = 1; i <= rightBorder; i++)
        {
            string prefix = input.substr(currentBufferBorder, i);
            int find = find_substring(prefix, currentBufferBorder, leftBorder, rightBorder);
            if(find == -1)
                break;

            offset = currentBufferBorder - find;
            length = i;
            chr = input[currentBufferBorder + i];
            if(currentBufferBorder + i > input.length())
            {
                chr = 0;
                break;
            }
        }

        lib.emplace_back(offset, length, chr);
        currentBufferBorder += length + 1;
    }
}

void LZ77Encoder::write_compressed(string path)
{
    ofstream fout;
    string postfix = to_string((search + preview) / 1024);
    string outpath = path.substr(0, path.rfind('.')) + ".lz77" + postfix;

    fout.open(outpath, ios::binary);
    if(fout.is_open())
    {
        fout << lib.size() << " " << bytes << "\n";
        for(auto l : lib)
        {
            fout << l.offset.b[0] << l.offset.b[1] << l.length.b[0] << l.length.b[1] << l.chr;
        }
        fout.close();
    }
}

void LZ77Encoder::compress(string path)
{
    string filename;
    if(!path.rfind('/'))
        filename = path;
    else
        filename = path.substr(path.rfind('/') + 1);

    cout << "\tCompressing file " + filename + ":\n";

    read_original_file(path);
    set_library();
    write_compressed(path);

    cout << "\tFile compressed!\n";
    cout << "\n";
}

void LZ77Encoder::decompress(string path)
{
    string filename;
    if(!path.rfind('/'))
        filename = path;
    else
        filename = path.substr(path.rfind('/') + 1);

    cout << "\tDecompressing file " + filename + ":\n";

    input.clear();
    lib.resize(0);
    ifstream fin;
    ofstream fout;
    string postfix = path.substr(path.rfind('.') + 1);
    string outpath = path.substr(0, path.rfind('.')) + ".un" + postfix;

    fin.open(path, ios::binary);
    if(fin.is_open())
    {
        int num;
        uchar c;
        fin >> num;
        fin >> bytes;
        c = fin.get();
        for(int i = 0; i < num; i++)
        {
            Node node;
            c = fin.get();
            node.offset.b[0] = c;
            c = fin.get();
            node.offset.b[1] = c;
            c = fin.get();
            node.length.b[0] = c;
            c = fin.get();
            node.length.b[1] = c;
            c = fin.get();
            node.chr = c;
            lib.push_back(node);
        }
        fin.close();
    }

    string res;
    for(int i = 0; i < lib.size(); i++)
    {
        int start = res.length() - lib[i].offset.a;
        if(start + lib[i].length.a >= res.length())
        {
            string buffer;
            for(int j = start; j < res.length(); j++)
            {
                buffer += res[j];
            }
            for(int j = 0; j < lib[i].length.a; j++)
            {
                res += buffer[j % buffer.length()];
            }
        }
        else
        {
            for(int j = start; j < start + lib[i].length.a; j++)
            {
                res += res[j];
            }
        }
        res += lib[i].chr;
    }
    res = res.substr(0, bytes);

    fout.open(outpath, ios::binary);
    if(fout.is_open())
    {
        fout << res;
        fout.close();
    }

    cout << "\tFile decompressed!\n";
    cout << "\n";
}



// КДЗ по дисциплине Алгоритмы и структуры данных, 2019-2020 уч.год
// Серебренников Александр Дмитриевич, группа БПИ-181, 31.02.2020
// CLion,
// Состав проекта: main.cpp; huffman.h; huffman.cpp; lz77.h; lz77.cpp;
// Что сделано: сжатие и распаковка методом Хаффмана, сжатие и распаковка методом LZ77,
// проведен вычислительный эксперимент, построены таблицы и графики,
// для измерения времени выполнения использовалась библиотека <chrono>, оформлен отчет
// Что не сделано: сжатие и распаковка методом LZW


#include "huffman.h"
#include "lz77.h"
#include <cmath>
#include <chrono>

using namespace std;
using namespace chrono;

//================= Настройки эксперимента =======================

// расположение обрабатываемых файлов
const string PATH = "../../DATA/";

// директория, в которой будут создаваться сгенерированные таблицы
// с метриками файлов в формате CSV
const string METRICS = "../../../Graphics/";

// имена обрабатываемых файлов
const vector<string> FILENAMES = {"1.txt", "2.docx", "3.pptx", "4.pdf", "5.dll", "6.jpg", "7.jpg", "8.bmp", "9.bmp", "10.fb2"};

// кол-во прогонов (сжатий и разжатий) алгоритмом Хаффмана
const int HUFFMAN_TIMES = 10;

// кол-во прогонов (сжатий и разжатий) алгоритмом LZ77
const int LZ77_TIMES = 10;

//================================================================



//================= Методы для получения метрик файлов =======================

/*
  вычисляет размер файла
  считает целое значение байтов во всем файле
*/
long long get_file_size(const string& path)
{
    uchar c;
    long long res = 0;

    ifstream fin;
    fin.open(path, ios::binary);
    if(fin.is_open())
    {
        c = fin.get();
        while(!fin.eof())
        {
            res++;
            c = fin.get();
        }
        fin.close();
    }

    return res;
}

/*
  вычисляет энтропию всех обрабатываемых файлов
  осуществляет вывод результатов для каждого файла в консоль
  также формирует таблицу и создаёт её в формате csv
  возвращает вектор с энтропиями файлов
*/
vector<double> get_files_entropy()
{
    uchar c;
    vector<vector<ull>> table(FILENAMES.size(), vector<ull>(256, 0));
    vector<double> entropy(FILENAMES.size(), 0);

    for(int i = 0; i < FILENAMES.size(); i++)
    {
        double total_bytes = 0;
        string path = PATH + FILENAMES[i];
        ifstream fin;
        fin.open(path, ios::binary);
        if(fin.is_open())
        {
            c = fin.get();
            while(!fin.eof())
            {
                total_bytes++;
                table[i][c]++;
                c = fin.get();
            }
            fin.close();
        }

        for(int j = 0; j < 256; j++)
        {
            if(table[i][j] != 0)
                entropy[i] -= (table[i][j] / total_bytes)*log2(table[i][j] / total_bytes);
        }
        entropy[i] /= 8;

        cout << "The entropy of the file " + FILENAMES[i] + ": " << entropy[i] << "\n";
    }
    cout << "\n";

    ofstream fout;
    fout.open(METRICS + "entropy.csv");
    if(fout.is_open())
    {
        for(int i = 0; i < 256; i++)
        {
            fout << i;
            for(int j = 0; j < FILENAMES.size(); j++)
            {
                fout << ", " << table[j][i];
            }
            fout << "\n";
        }
        fout << "Entropy (H)";
        for(int i = 0; i < FILENAMES.size(); i++)
        {
            fout << ", " << entropy[i];
        }
        fout.close();
    }

    return entropy;
}

// основное окно проведения эксперимента
int main()
{
    // инициализируем векторы для хранения полученых в ходе эксперимента данных
    vector<long long> size(FILENAMES.size());
    for(int i = 0; i < FILENAMES.size(); i++)
    {
        size[i] = get_file_size(PATH + FILENAMES[i]);
    }

    vector<long long> huffman_size(FILENAMES.size());
    vector<long long> lz775_size(FILENAMES.size());
    vector<long long> lz7710_size(FILENAMES.size());
    vector<long long> lz7720_size(FILENAMES.size());

    vector<double> huffman_compress_sec(FILENAMES.size());
    vector<double> lz775_compress_sec(FILENAMES.size());
    vector<double> lz7710_compress_sec(FILENAMES.size());
    vector<double> lz7720_compress_sec(FILENAMES.size());

    vector<double> huffman_decompress_sec(FILENAMES.size());
    vector<double> lz775_decompress_sec(FILENAMES.size());
    vector<double> lz7710_decompress_sec(FILENAMES.size());
    vector<double> lz7720_decompress_sec(FILENAMES.size());

    time_point<system_clock> start, end;

    cout << "=============== Entropy ===============\n\n";

    vector<double> entropy = get_files_entropy();

    // фактическое начало эксперимента

    cout << "=============== Huffman ===============\n\n";

    for(int i = 0; i < FILENAMES.size(); i++)
    {
        string filename = PATH + FILENAMES[i];
        duration<double> compress_seconds{}, decompress_seconds{};

        for(int j = 0; j < HUFFMAN_TIMES; j++)
        {
            Huffman* huff = new Huffman();

            start = system_clock::now();
            huff->compress(filename);
            end = system_clock::now();
            compress_seconds += end - start;

            start = system_clock::now();
            huff->decompress(filename.substr(0, filename.rfind('.')) + ".haff");
            end = system_clock::now();
            decompress_seconds += end - start;

            delete huff;
        }

        compress_seconds /= HUFFMAN_TIMES;
        decompress_seconds /= HUFFMAN_TIMES;

        cout << "Average time of Huffman compressing " + FILENAMES[i] + ": " << compress_seconds.count() << " sec\n";
        cout << "Average time of Huffman decompressing " + FILENAMES[i] + ": " << decompress_seconds.count() << " sec\n\n";

        huffman_compress_sec[i] = compress_seconds.count();
        huffman_decompress_sec[i] = decompress_seconds.count();
        huffman_size[i] = get_file_size(filename.substr(0, filename.rfind('.')) + ".haff");
    }


    cout << "================ LZ775 ================\n\n";

    for(int i = 0; i < FILENAMES.size(); i++)
    {
        string filename = PATH + FILENAMES[i];
        duration<double> compress_seconds{}, decompress_seconds{};

        for(int j = 0; j < LZ77_TIMES; j++)
        {
            LZ77Encoder* lz77Encoder = new LZ77Encoder(5*1024, 4*1024);

            start = system_clock::now();
            lz77Encoder->compress(filename);
            end = system_clock::now();
            compress_seconds += end - start;

            start = system_clock::now();
            lz77Encoder->decompress(filename.substr(0, filename.rfind('.')) + ".lz775");
            end = system_clock::now();
            decompress_seconds += end - start;

            delete lz77Encoder;
        }

        compress_seconds /= LZ77_TIMES;
        decompress_seconds /= LZ77_TIMES;

        cout << "Average time of LZ775 compressing " + FILENAMES[i] + ": " << compress_seconds.count() << " sec\n";
        cout << "Average time of LZ775 decompressing " + FILENAMES[i] + ": " << decompress_seconds.count() << " sec\n\n";

        lz775_compress_sec[i] = compress_seconds.count();
        lz775_decompress_sec[i] = decompress_seconds.count();
        lz775_size[i] = get_file_size(filename.substr(0, filename.rfind('.')) + ".lz775");
    }

    cout << "================ LZ7710 ================\n\n";

    for(int i = 0; i < FILENAMES.size(); i++)
    {
        string filename = PATH + FILENAMES[i];
        duration<double> compress_seconds{}, decompress_seconds{};

        for(int j = 0; j < LZ77_TIMES; j++)
        {
            LZ77Encoder* lz77Encoder = new LZ77Encoder(10*1024, 8*1024);

            start = system_clock::now();
            lz77Encoder->compress(filename);
            end = system_clock::now();
            compress_seconds += end - start;

            start = system_clock::now();
            lz77Encoder->decompress(filename.substr(0, filename.rfind('.')) + ".lz7710");
            end = system_clock::now();
            decompress_seconds += end - start;

            delete lz77Encoder;
        }

        compress_seconds /= LZ77_TIMES;
        decompress_seconds /= LZ77_TIMES;

        cout << "Average time of LZ7710 compressing " + FILENAMES[i] + ": " << compress_seconds.count() << " sec\n";
        cout << "Average time of LZ7710 decompressing " + FILENAMES[i] + ": " << decompress_seconds.count() << " sec\n\n";

        lz7710_compress_sec[i] = compress_seconds.count();
        lz7710_decompress_sec[i] = decompress_seconds.count();
        lz7710_size[i] = get_file_size(filename.substr(0, filename.rfind('.')) + ".lz7710");
    }

    cout << "================ LZ7720 ================\n\n";

    for(int i = 0; i < FILENAMES.size(); i++)
    {
        string filename = PATH + FILENAMES[i];
        duration<double> compress_seconds{}, decompress_seconds{};
        for(int j = 0; j < LZ77_TIMES; j++)
        {
            LZ77Encoder* lz77Encoder = new LZ77Encoder(20*1024, 16*1024);

            start = system_clock::now();
            lz77Encoder->compress(filename);
            end = system_clock::now();
            compress_seconds += end - start;

            start = system_clock::now();
            lz77Encoder->decompress(filename.substr(0, filename.rfind('.')) + ".lz7720");
            end = system_clock::now();
            decompress_seconds += end - start;

            delete lz77Encoder;
        }
        compress_seconds /= LZ77_TIMES;
        decompress_seconds /= LZ77_TIMES;

        cout << "Average time of LZ7720 compressing " + FILENAMES[i] + ": " << compress_seconds.count() << " sec\n";
        cout << "Average time of LZ7720 decompressing " + FILENAMES[i] + ": " << decompress_seconds.count() << " sec\n\n";

        lz7720_compress_sec[i] = compress_seconds.count();
        lz7720_decompress_sec[i] = decompress_seconds.count();
        lz7720_size[i] = get_file_size(filename.substr(0, filename.rfind('.')) + ".lz7720");
    }

    // окончание эксперимента, вывод результатов в CSV файлы

    ofstream fout;
    fout.open(METRICS + "compress.csv");
    if(fout.is_open())
    {
        for(int i = 0; i < FILENAMES.size(); i++)
        {
            fout << FILENAMES[i] << ", ";
            fout << size[i] << ", " << entropy[i] << ", ";
            fout << huffman_size[i] << ", " << ((double)huffman_size[i]) / size[i] << ", ";
            fout << lz775_size[i] << ", " << ((double)lz775_size[i]) / size[i] << ", ";
            fout << lz7710_size[i] << ", " << ((double)lz7710_size[i]) / size[i] << ", ";
            fout << lz7720_size[i] << ", " << ((double)lz7720_size[i]) / size[i] << "\n";
        }
        fout.close();
    }

    fout.open(METRICS + "times.csv");
    if(fout.is_open())
    {
        for(int i = 0; i < FILENAMES.size(); i++)
        {
            fout << FILENAMES[i] << ", ";
            fout << size[i] << ", " << entropy[i] << ", ";
            fout << huffman_compress_sec[i]*1000 << ", " << huffman_decompress_sec[i]*1000 << ", ";
            fout << lz775_compress_sec[i]*1000 << ", " << lz775_decompress_sec[i]*1000 << ", ";
            fout << lz7710_compress_sec[i]*1000 << ", " << lz7710_decompress_sec[i]*1000 << ", ";
            fout << lz7720_compress_sec[i]*1000 << ", " << lz7720_decompress_sec[i]*1000 << "\n";
        }
        fout.close();
    }

    return 0;
}

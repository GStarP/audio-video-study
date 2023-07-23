#include <string>
#include <fstream>

using namespace std;

/**
 * pcm-16bit to pcm-8bit
 */
void pcm16_to_pcm8()
{
    /**
     * create fp
     */
    // 44.1kHz  16bit  LittleEndian
    ifstream pcm_file("../../resources/NocturneNo2inEflat_44.1k_s16le.pcm", ios::in | ios::binary);
    ofstream pcm8_file("./dist/pcm8.pcm", ios::out | ios::binary | ios::trunc);

    /**
     * read and write audio data
     */
    unsigned char *buffer = new unsigned char[4];
    int flag = 0;
    while (!pcm_file.eof())
    {
        pcm_file.read((char *)buffer, 4);

        unsigned char sample_8bit = 0;
        short *sample_16bit = NULL;
        // step1: [-32768, 32767] => [-128, 127] (>> 8)
        // step2: [-128, 127] => [0, 255]        (+128)
        sample_16bit = (short *)buffer;
        // @FIX test in Audacity, step2 seems to be deprecated
        sample_8bit = ((*sample_16bit) >> 8);
        pcm8_file.write((char *)&sample_8bit, 1);

        sample_16bit = (short *)(buffer + 2);
        sample_8bit = ((*sample_16bit) >> 8);
        pcm8_file.write((char *)&sample_8bit, 1);
    }

    /**
     * clean
     */
    delete buffer;
    pcm_file.close();
    pcm8_file.close();
}

int main()
{
    pcm16_to_pcm8();
}
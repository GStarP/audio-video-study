#include <string>
#include <fstream>

using namespace std;

/**
 * split pcm16le file to left/right channel files
 */
void pcm16le_split()
{
    /**
     * create fp
     */
    // 44.1kHz  16bit  LittleEndian
    ifstream pcm_file("../../resources/NocturneNo2inEflat_44.1k_s16le.pcm", ios::in | ios::binary);
    ofstream l_channel_file("./dist/l_channel.pcm", ios::out | ios::binary | ios::trunc);
    ofstream r_channel_file("./dist/r_channel.pcm", ios::out | ios::binary | ios::trunc);

    /**
     * read and write audio data
     */
    unsigned char *buffer = new unsigned char[4];
    while (!pcm_file.eof())
    {
        pcm_file.read((char *)buffer, 4);
        l_channel_file.write((char *)buffer, 2);
        r_channel_file.write((char *)buffer + 2, 2);
    }

    /**
     * clean
     */
    delete buffer;
    pcm_file.close();
    l_channel_file.close();
    r_channel_file.close();
}

int main()
{
    pcm16le_split();
}
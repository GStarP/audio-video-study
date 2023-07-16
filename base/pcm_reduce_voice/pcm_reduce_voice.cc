#include <string>
#include <fstream>

using namespace std;

/**
 * reduce pcm16le file's left channel voice
 */
void pcm16le_reduce_voice()
{
    /**
     * create fp
     */
    // 44.1kHz  16bit  LittleEndian
    ifstream pcm_file("../../resources/NocturneNo2inEflat_44.1k_s16le.pcm", ios::in | ios::binary);
    ofstream voice_reduced_file("./dist/voice_reduced.pcm", ios::out | ios::binary | ios::trunc);

    /**
     * read and write video data
     */
    unsigned char *buffer = new unsigned char[4];
    while (!pcm_file.eof())
    {
        pcm_file.read((char *)buffer, 4);

        // short takes 2 bytes, which represents the left channel samples
        short *channelBuffer = (short *)buffer;
        // reduce the sample is reduce the voice
        *channelBuffer = *channelBuffer / 4;
        
        voice_reduced_file.write((char *)buffer, 2);
        voice_reduced_file.write((char *)buffer + 2, 2);
    }

    /**
     * clean
     */
    delete buffer;
    pcm_file.close();
    voice_reduced_file.close();
}

int main()
{
    pcm16le_reduce_voice();
}
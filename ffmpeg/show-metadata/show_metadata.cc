extern "C"
{
#include <libavformat/avformat.h>
#include <libavutil/pixfmt.h>
#include <libavutil/samplefmt.h>
}

int show_metadata(const char *filename)
{
  int ret;
  AVFormatContext *fmt_ctx = NULL;

  // open file
  if ((ret = avformat_open_input(&fmt_ctx, filename, NULL, NULL)))
  {
    av_log(NULL, AV_LOG_ERROR, "avformat_open_input\n");
    return ret;
  }

  // read stream info
  if ((ret = avformat_find_stream_info(fmt_ctx, NULL)) < 0)
  {
    av_log(NULL, AV_LOG_ERROR, "avformat_find_stream_info\n");
    return ret;
  }

  // iterate streams
  for (int i = 0; i < fmt_ctx->nb_streams; i++)
  {
    AVStream *stream = fmt_ctx->streams[i];
    AVCodecParameters *codecpar = stream->codecpar;
    printf("Stream #%d:\n", i);
    printf("  Codec type: %s\n", av_get_media_type_string(codecpar->codec_type));
    printf("  Codec ID: %s\n", avcodec_get_name(codecpar->codec_id));

    if (codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
    {
      printf("  Format: %d\n", codecpar->format);
    }
    else if (codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
    {
      printf("  Format: %s\n", av_get_sample_fmt_name((AVSampleFormat)codecpar->format));
    }

    printf("  Bit rate: %lld\n", codecpar->bit_rate);
    printf("  Width: %d\n", codecpar->width);
    printf("  Height: %d\n", codecpar->height);
    printf("  Channels: %d\n", codecpar->channels);
    printf("  Sample rate: %d\n", codecpar->sample_rate);
  }

  avformat_close_input(&fmt_ctx);

  return 0;
}

int main()
{
  show_metadata("../../resources/music.aac");
}

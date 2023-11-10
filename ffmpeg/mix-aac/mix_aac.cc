extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
}
#include <exception>
#include <stdexcept>

AVFormatContext *in_fmt_ctx_1;
AVCodecContext *in_dec_ctx_1;
AVFormatContext *in_fmt_ctx_2;
AVCodecContext *in_dec_ctx_2;

AVFilterGraph *filter_graph;
AVFilterInOut *inputs_1;
AVFilterInOut *inputs_2;
AVFilterInOut *outputs;

AVFormatContext *out_fmt_ctx;
AVCodecContext *out_enc_ctx;

int pkt_count;

int open_input_file_1()
{
    const char *filename = "../../resources/speak.aac";

    printf("===\n");
    printf("[open_input_file] %s\n", filename);
    int ret;
    // open file
    if ((ret = avformat_open_input(&in_fmt_ctx_1, filename, nullptr, nullptr)) < 0)
    {
        av_log(NULL, AV_LOG_ERROR, filename, "avformat_open_input\n");
        return ret;
    }
    printf("[avformat_open_input]\n");

    // read stream info
    if ((ret = avformat_find_stream_info(in_fmt_ctx_1, nullptr)) < 0)
    {
        av_log(NULL, AV_LOG_ERROR, filename, "avformat_find_stream_info\n");
        return ret;
    }
    printf("[avformat_find_stream_info]\n");

    // find the index of our single audio stream (it is just 0)
    AVCodec *dec;
    ret = av_find_best_stream(in_fmt_ctx_1, AVMEDIA_TYPE_AUDIO, -1, -1, &dec, 0);
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, filename, "av_find_best_stream\n");
        return ret;
    }
    printf("[av_find_best_stream] index=%d\n", ret);
    if (ret != 0)
    {

        printf("!!! av_find_best_stream not 0 !!!");
    }

    // create decode context
    in_dec_ctx_1 = avcodec_alloc_context3(dec);
    if (!in_dec_ctx_1)
    {
        av_log(NULL, AV_LOG_ERROR, filename, "avcodec_alloc_context3\n");
        return AVERROR(ENOMEM);
    }
    printf("[avcodec_alloc_context3]\n");

    // set stream's codec params to decode context
    avcodec_parameters_to_context(in_dec_ctx_1, in_fmt_ctx_1->streams[0]->codecpar);
    printf("[avcodec_parameters_to_context]\n");

    // init decode context
    if ((ret = avcodec_open2(in_dec_ctx_1, dec, NULL)) < 0)
    {
        av_log(NULL, AV_LOG_ERROR, filename, "avcodec_open2\n");
        return ret;
    }
    printf("[avcodec_open2]\n");
    printf("===\n");

    return 0;
}

// @TODO duplicate
int open_input_file_2()
{
    const char *filename = "../../resources/music.aac";

    printf("===\n");
    printf("[open_input_file] %s\n", filename);
    int ret;
    // open file
    if ((ret = avformat_open_input(&in_fmt_ctx_2, filename, nullptr, nullptr)) < 0)
    {
        av_log(NULL, AV_LOG_ERROR, filename, "avformat_open_input\n");
        return ret;
    }
    printf("[avformat_open_input]\n");

    // read stream info
    if ((ret = avformat_find_stream_info(in_fmt_ctx_2, nullptr)) < 0)
    {
        av_log(NULL, AV_LOG_ERROR, filename, "avformat_find_stream_info\n");
        return ret;
    }
    printf("[avformat_find_stream_info]\n");

    // find the index of our single audio stream (it is just 0)
    AVCodec *dec;
    ret = av_find_best_stream(in_fmt_ctx_2, AVMEDIA_TYPE_AUDIO, -1, -1, &dec, 0);
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, filename, "av_find_best_stream\n");
        return ret;
    }
    printf("[av_find_best_stream] index=%d\n", ret);
    if (ret != 0)
    {

        printf("!!! av_find_best_stream not 0 !!!");
    }

    // create decode context
    in_dec_ctx_2 = avcodec_alloc_context3(dec);
    if (!in_dec_ctx_2)
    {
        av_log(NULL, AV_LOG_ERROR, filename, "avcodec_alloc_context3\n");
        return AVERROR(ENOMEM);
    }
    printf("[avcodec_alloc_context3]\n");

    // set stream's codec params to decode context
    avcodec_parameters_to_context(in_dec_ctx_2, in_fmt_ctx_2->streams[0]->codecpar);
    printf("[avcodec_parameters_to_context]\n");

    // init decode context
    if ((ret = avcodec_open2(in_dec_ctx_2, dec, NULL)) < 0)
    {
        av_log(NULL, AV_LOG_ERROR, filename, "avcodec_open2\n");
        return ret;
    }
    printf("[avcodec_open2]\n");
    printf("===\n");

    return 0;
}

int open_output_file()
{
    const char *filename = "./dist/mixed.aac";

    printf("===\n");
    printf("[open_output_file] %s\n", filename);
    int ret;

    // alloc context
    ret = avformat_alloc_output_context2(&out_fmt_ctx, nullptr, nullptr, filename);
    if (!out_fmt_ctx)
    {
        av_log(NULL, AV_LOG_ERROR, filename, "avformat_alloc_context\n");
        return ret;
    }
    printf("[avformat_alloc_output_context2] \n");

    // create output audio stream
    AVStream *out_stream = avformat_new_stream(out_fmt_ctx, nullptr);
    if (!out_stream)
    {
        av_log(NULL, AV_LOG_ERROR, filename, "avformat_new_stream\n");
        return -1;
    }
    out_stream->index = 0;
    printf("[avformat_new_stream] \n");

    // create encoder context
    AVCodec *encoder = avcodec_find_encoder(AV_CODEC_ID_AAC);
    out_enc_ctx = avcodec_alloc_context3(encoder);
    if (!out_enc_ctx)
    {
        av_log(NULL, AV_LOG_ERROR, filename, "avcodec_alloc_context3\n");
        return -1;
    }
    printf("[avcodec_alloc_context3] \n");

    // set encode params
    out_enc_ctx->codec_type = AVMEDIA_TYPE_AUDIO;
    out_enc_ctx->sample_rate = 44100;
    out_enc_ctx->channels = 2;
    out_enc_ctx->channel_layout = 3;
    out_enc_ctx->sample_fmt = AV_SAMPLE_FMT_FLTP;
    AVRational ar = {1, out_enc_ctx->sample_rate};
    out_enc_ctx->time_base = ar;

    // opem emcode context
    ret = avcodec_open2(out_enc_ctx, encoder, nullptr);
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, filename, "avcodec_open2\n");
        return ret;
    }
    printf("[avcodec_open2] \n");

    // copy codec params from context to stream
    ret = avcodec_parameters_from_context(out_stream->codecpar, out_enc_ctx);
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, filename, "avcodec_parameters_from_context\n");
        return ret;
    }
    printf("[avcodec_parameters_from_context] \n");

    // @TODO
    if (out_fmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
    {
        out_enc_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    av_dump_format(out_fmt_ctx, 0, filename, 1);
    ret = avio_open(&out_fmt_ctx->pb, filename, AVIO_FLAG_WRITE);
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, filename, "avio_open\n");
        return ret;
    }
    printf("[avio_open] \n");

    ret = avformat_write_header(out_fmt_ctx, nullptr);
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, filename, "avformat_write_header\n");
        return ret;
    }
    printf("[avformat_write_header] \n");

    printf("===\n");

    return 0;
}

int process_frame(AVFrame *frame)
{
    int ret;

    ret = avcodec_send_frame(out_enc_ctx, frame);
    if (ret < 0)
    {
        av_log(NULL, AV_LOG_ERROR, "avcodec_send_frame\n");
        return ret;
    }

    while (ret >= 0)
    {
        AVPacket *packet = av_packet_alloc();
        ret = avcodec_receive_packet(out_enc_ctx, packet);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            break;
        else if (ret < 0)
        {
            av_log(NULL, AV_LOG_ERROR, "avcodec_receive_packet\n");
            return ret;
        }

        av_packet_rescale_ts(packet, out_enc_ctx->time_base, out_fmt_ctx->streams[0]->time_base);
        packet->stream_index = 0;

        packet->pts = pkt_count * out_fmt_ctx->streams[0]->codec->frame_size;
        packet->dts = packet->pts;
        packet->duration = out_fmt_ctx->streams[0]->codec->frame_size;

        packet->pts = av_rescale_q_rnd(
            packet->pts, out_fmt_ctx->streams[0]->codec->time_base, out_fmt_ctx->streams[0]->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        packet->dts = packet->pts;
        packet->duration = av_rescale_q_rnd(
            packet->duration, out_fmt_ctx->streams[0]->codec->time_base, out_fmt_ctx->streams[0]->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));

        ret = av_interleaved_write_frame(out_fmt_ctx, packet);
        av_packet_unref(packet);
        if (ret < 0)
        {
            av_log(NULL, AV_LOG_ERROR, "av_interleaved_write_frame\n");
            return ret;
        }
        ++pkt_count;
    }

    return 0;
}

int print_frame(const AVFrame *frame)
{
    const int n = frame->nb_samples * frame->channels;
    const uint16_t *p = (uint16_t *)frame->data[0];
    const uint16_t *p_end = p + n;

    while (p < p_end)
    {
        fputc(*p & 0xff, stdout);
        fputc(*p >> 8 & 0xff, stdout);
        p++;
    }
    fflush(stdout);
    return 0;
}

int main()
{
    int ret = 0;

    try
    {
        /**
         * inputs
         */
        if (ret = open_input_file_1() < 0)
            throw std::runtime_error("^ err");
        if (ret = open_input_file_2() < 0)
            throw std::runtime_error("^ err");

        /**
         * output
         */
        if (ret = open_output_file() < 0)
        {
            throw std::runtime_error("^ err");
        }

        /**
         * filter
         */
        filter_graph = avfilter_graph_alloc();
        inputs_1 = avfilter_inout_alloc();
        inputs_2 = avfilter_inout_alloc();
        outputs = avfilter_inout_alloc();
        if (!filter_graph || !inputs_1 || !inputs_2 || !outputs)
        {
            throw std::runtime_error("alloc filter_graph/input/output");
        }

        /**
         * input filter (x2)
         */
        // src1
        char args_1[512];
        AVFilterContext *src_filter_ctx_1;
        const AVFilter *abuffersrc_1 = avfilter_get_by_name("abuffer");

        AVRational time_base_1 = in_fmt_ctx_1->streams[0]->time_base;
        if (!in_dec_ctx_1->channel_layout)
        {
            in_dec_ctx_1->channel_layout = av_get_default_channel_layout(in_dec_ctx_1->channels);
        }
        // write format string into args
        snprintf(args_1, sizeof(args_1),
                 "time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=0x%d",
                 time_base_1.num, time_base_1.den, in_dec_ctx_1->sample_rate,
                 av_get_sample_fmt_name(in_dec_ctx_1->sample_fmt),
                 (int)in_dec_ctx_1->channel_layout);
        // time_base=1/28224000:sample_rate=44100:sample_fmt=fltp:channel_layout=0x3
        // printf("arg_1: %s\n", args_1);

        ret = avfilter_graph_create_filter(&src_filter_ctx_1, abuffersrc_1, "in", args_1,
                                           NULL, filter_graph);
        if (ret < 0)
        {
            av_log(NULL, AV_LOG_ERROR, "src1 avfilter_graph_create_filter\n");
            throw std::runtime_error("^ err");
        }
        inputs_1->name = av_strdup("in1");
        inputs_1->filter_ctx = src_filter_ctx_1;
        inputs_1->pad_idx = 0;
        // src2
        char args_2[512];
        AVFilterContext *src_filter_ctx_2;
        const AVFilter *abuffersrc_2 = avfilter_get_by_name("abuffer");

        AVRational time_base_2 = in_fmt_ctx_2->streams[0]->time_base;
        if (!in_dec_ctx_2->channel_layout)
        {
            in_dec_ctx_2->channel_layout = av_get_default_channel_layout(in_dec_ctx_2->channels);
        }
        // write format string into args
        snprintf(args_2, sizeof(args_2),
                 "time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=0x%d",
                 time_base_2.num, time_base_2.den, in_dec_ctx_2->sample_rate,
                 av_get_sample_fmt_name(in_dec_ctx_2->sample_fmt),
                 (int)in_dec_ctx_2->channel_layout);

        ret = avfilter_graph_create_filter(&src_filter_ctx_2, abuffersrc_2, "in", args_2,
                                           NULL, filter_graph);
        if (ret < 0)
        {
            av_log(NULL, AV_LOG_ERROR, "src2 avfilter_graph_create_filter\n");
            throw std::runtime_error("^ err");
        }
        inputs_2->name = av_strdup("in2");
        inputs_2->filter_ctx = src_filter_ctx_2;
        inputs_2->pad_idx = 0;

        // link these inputs
        inputs_1->next = inputs_2;
        inputs_2->next = nullptr;

        /**
         * output filter
         */
        AVFilterContext *sink_filter_ctx;
        const AVFilter *abuffersink = avfilter_get_by_name("abuffersink");
        // create sink filter
        ret = avfilter_graph_create_filter(&sink_filter_ctx, abuffersink, "out", NULL,
                                           NULL, filter_graph);
        if (ret < 0)
        {
            av_log(NULL, AV_LOG_ERROR, "sink avfilter_graph_create_filter\n");
            throw std::runtime_error("^ err");
        }
        // sink: set sample format
        const enum AVSampleFormat out_sample_fmts[] = {
            AV_SAMPLE_FMT_FLTP, (enum AVSampleFormat) - 1};
        ret = av_opt_set_int_list(sink_filter_ctx, "sample_fmts", out_sample_fmts, -1,
                                  AV_OPT_SEARCH_CHILDREN);
        if (ret < 0)
        {
            av_log(NULL, AV_LOG_ERROR, "av_opt_set_int_list\n");
            throw std::runtime_error("^ err");
        }
        // sink: set sink channel layout
        static const int64_t out_channel_layouts[] = {3, -1};
        ret = av_opt_set_int_list(sink_filter_ctx, "channel_layouts",
                                  out_channel_layouts, -1, AV_OPT_SEARCH_CHILDREN);
        if (ret < 0)
        {
            av_log(NULL, AV_LOG_ERROR, "av_opt_set_int_list\n");
            throw std::runtime_error("^ err");
        }
        // sink: set sample rate
        static const int out_sample_rates[] = {44100, -1};
        ret = av_opt_set_int_list(sink_filter_ctx, "sample_rates", out_sample_rates,
                                  -1, AV_OPT_SEARCH_CHILDREN);
        if (ret < 0)
        {
            av_log(NULL, AV_LOG_ERROR, "av_opt_set_int_list\n");
            throw std::runtime_error("^ err");
        }

        outputs->name = av_strdup("out");
        outputs->filter_ctx = sink_filter_ctx;
        outputs->pad_idx = 0;
        outputs->next = nullptr;

        // config filter graph
        // @TRICK speak.aac's volume is realateively low, so *6
        if ((ret = avfilter_graph_parse_ptr(filter_graph, "[in1]volume=6[in1];[in1][in2]amix=inputs=2[out]", &outputs,
                                            &inputs_1, NULL)) < 0)
        {

            av_log(NULL, AV_LOG_ERROR, "avfilter_graph_parse_ptr\n");
            throw std::runtime_error("^ err");
        }

        if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
        {
            av_log(NULL, AV_LOG_ERROR, "avfilter_graph_config\n");
            throw std::runtime_error("^ err");
        }

        // alloc for packet and frame
        int got_frame;
        AVPacket *packet = av_packet_alloc();
        AVPacket *packet_1 = av_packet_alloc();
        AVPacket *packet_2 = av_packet_alloc();
        AVFrame *frame = av_frame_alloc();
        AVFrame *filter_frame = av_frame_alloc();
        if (!packet || !packet_1 || !packet_2 || !frame || !filter_frame)
        {
            av_log(NULL, AV_LOG_ERROR, "packet/frame alloc\n");
            throw;
        }
        packet_1->data = nullptr;
        packet_2->data = nullptr;

        // read frame and process
        printf("=== loop frame ===\n");
        while (1)
        {
            /**
             * read from in1
             */
            if (!packet_1->data)
            {
                // read packet
                if ((ret = av_read_frame(in_fmt_ctx_1, packet_1)) < 0)
                {
                    break;
                }
            }

            // only care about audio stream
            if (packet_1->stream_index == 0)
            {
                got_frame = 0;
                ret = avcodec_decode_audio4(in_dec_ctx_1, frame, &got_frame, packet_1);
                if (ret < 0)
                {
                    av_log(NULL, AV_LOG_ERROR, "in1 avcodec_decode_audio4\n");
                    throw;
                }

                packet_1->size -= ret;
                packet_1->data += ret;

                if (got_frame)
                {
                    if (av_buffersrc_add_frame_flags(src_filter_ctx_1, frame, 0) < 0)
                    {

                        av_log(NULL, AV_LOG_ERROR, "in1 av_buffersrc_add_frame_flags\n");
                        throw;
                    }
                }

                if (packet_1->size <= 0)
                {
                    av_packet_unref(packet_1);
                }
            }
            else
            {
                av_packet_unref(packet_1);
            }

            /**
             * read from in2
             */
            if (!packet_2->data)
            {
                // read packet
                if ((ret = av_read_frame(in_fmt_ctx_2, packet_2)) < 0)
                {
                    break;
                }
            }

            // only care about audio stream
            if (packet_2->stream_index == 0)
            {
                got_frame = 0;
                ret = avcodec_decode_audio4(in_dec_ctx_2, frame, &got_frame, packet_2);
                if (ret < 0)
                {
                    av_log(NULL, AV_LOG_ERROR, "in2 avcodec_decode_audio4\n");
                    throw;
                }

                packet_2->size -= ret;
                packet_2->data += ret;

                if (got_frame)
                {
                    if (av_buffersrc_add_frame_flags(src_filter_ctx_2, frame, 0) < 0)
                    {

                        av_log(NULL, AV_LOG_ERROR,
                               "in2 av_buffersrc_add_frame_flags\n");
                        throw;
                    }
                }

                if (packet->size <= 0)
                {
                    av_packet_unref(packet_2);
                }
            }
            else
            {
                av_packet_unref(packet_2);
            }

            /**
             * write to output
             */
            if (got_frame)
            {
                while (1)
                {
                    // get mixed frame from filter sink
                    ret = av_buffersink_get_frame(sink_filter_ctx, filter_frame);
                    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                        break;
                    else if (ret < 0)
                    {
                        av_log(NULL, AV_LOG_ERROR,
                               "av_buffersink_get_frame\n");
                        throw;
                    }
                    else
                    {
                        // print_frame(filter_frame);
                        process_frame(filter_frame);
                        av_frame_unref(filter_frame);
                    }
                }
            }
        }

        // must execute clean
        // @TODO better use RAII
        throw std::runtime_error("mix finish");
    }
    catch (...)
    {
        std::exception_ptr p = std::current_exception();
        try
        {
            if (p)
            {
                std::rethrow_exception(p);
            }
        }
        catch (const std::exception &e)
        {
            printf("exception: %s\n", e.what());
        }

        printf("=== start clean ===\n");

        if (filter_graph)
            avfilter_graph_free(&filter_graph);

        // if (inputs_2)
        //     avfilter_inout_free(&inputs_2);
        if (inputs_1)
            avfilter_inout_free(&inputs_1);
        if (outputs)
            avfilter_inout_free(&outputs);

        if (out_fmt_ctx)
        {
            av_write_trailer(out_fmt_ctx);
            if (out_fmt_ctx && !(out_fmt_ctx->oformat->flags & AVFMT_NOFILE))
            {
                avio_closep(&out_fmt_ctx->pb);
            }
            avformat_free_context(out_fmt_ctx);
        }
        if (out_enc_ctx)
            avcodec_free_context(&out_enc_ctx);

        if (in_fmt_ctx_1)
            avformat_close_input(&in_fmt_ctx_1);
        if (in_dec_ctx_1)
            avcodec_free_context(&in_dec_ctx_1);
        if (in_fmt_ctx_2)
            avformat_close_input(&in_fmt_ctx_2);
        if (in_dec_ctx_2)
            avcodec_free_context(&in_dec_ctx_2);

        printf("=== finish clean ===\n");
    }
}

#include <stdio.h>
#include "waifu2x.h"
#include "cpu.h"
#include "platform.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include "waifu2x-model-utility.h"

Waifu2xMat* Waifu2x::process(Waifu2xMat* input, int scale, Waifu2xProcessStatus* processStatus) {
    Waifu2xMat* mat = 0;
    process_s(&mat, input, scale, processStatus);
    return mat;
}

int Waifu2x::process_s(Waifu2xMat** output, Waifu2xMat* input, int scale, Waifu2xProcessStatus* processStatus) {
    if (!output || !input) return -1;
    if (scale != 1 && scale % 2) return 0x00000014;

    ncnn::Mat* o_mat;
    auto* source = input->getSource();
    auto recu = 1, rscale = scale == 1 ? 1 : 2;

    if (processStatus) {
        processStatus->target_scale = scale;
        processStatus->current_scale = 0;
    }

    _REP:
    if (scale == 1) wut::createMat(&o_mat, source, 1);
    else {
        wut::createMat(&o_mat, source, 2);
        recu *= 2;
    }

    if (processor->processor == Waifu2xProcessors::Cpu) process_cpu(rscale, processStatus, source->w, source->h, source->elempack, (const byte*)source->data, o_mat->data);
    else process(rscale, processStatus, source->w, source->h, source->elempack, (const byte*)source->data, o_mat->data);

    if (recu > 2) {
        delete source;
    }

    if (processStatus) {
        processStatus->current_scale = recu;
    }

    if (recu < scale) {
        source = o_mat;
        goto _REP;
    }

    *output = new Waifu2xMat(o_mat, true);
    return 0;
}

int Waifu2x::process(int scale, Waifu2xProcessStatus* processStatus, int w, int h, int channels, const byte* pixeldata, void* outdata) {
    int prepadding = model->prePadding;
    bool tta_mode = model->ttaMode;

    const int TILE_SIZE_X = model->tileSize;
    const int TILE_SIZE_Y = model->tileSize;
    auto* vkdev = processor->getDevice();

    ncnn::VkAllocator* blob_vkallocator = vkdev->acquire_blob_allocator();
    ncnn::VkAllocator* staging_vkallocator = vkdev->acquire_staging_allocator();

    ncnn::Option opt = net.opt;
    opt.blob_vkallocator = blob_vkallocator;
    opt.workspace_vkallocator = blob_vkallocator;
    opt.staging_vkallocator = staging_vkallocator;

    // each tile 400x400
    const int xtiles = (w + TILE_SIZE_X - 1) / TILE_SIZE_X;
    const int ytiles = (h + TILE_SIZE_Y - 1) / TILE_SIZE_Y;

    if (processStatus) {
        processStatus->tile_count = xtiles * ytiles;
        processStatus->tile_index = 0;
    }

    const size_t in_out_tile_elemsize = opt.use_fp16_storage ? 2u : 4u;

    for (int yi = 0; yi < ytiles; yi++) {
        const int tile_h_nopad = std::min((yi + 1) * TILE_SIZE_Y, h) - yi * TILE_SIZE_Y;

        int prepadding_bottom = prepadding;
        if (scale == 1) {
            prepadding_bottom += (tile_h_nopad + 3) / 4 * 4 - tile_h_nopad;
        }
        if (scale == 2) {
            prepadding_bottom += (tile_h_nopad + 1) / 2 * 2 - tile_h_nopad;
        }

        #pragma region gpu_mat
        int in_tile_y0 = std::max(yi * TILE_SIZE_Y - prepadding, 0);
        int in_tile_y1 = std::min((yi + 1) * TILE_SIZE_Y + prepadding_bottom, h);

        ncnn::Mat in;
        if (opt.use_fp16_storage && opt.use_int8_storage) {
            in = ncnn::Mat(w, (in_tile_y1 - in_tile_y0), (unsigned char*)pixeldata + in_tile_y0 * w * channels, (size_t)channels, 1);
        } else {
            in = ncnn::Mat::from_pixels(pixeldata + in_tile_y0 * w * channels,
                channels == 4 ? ncnn::Mat::PIXEL_RGBA : ncnn::Mat::PIXEL_RGB,
                w, (in_tile_y1 - in_tile_y0));
        }

        ncnn::VkCompute cmd(vkdev);

        // upload
        ncnn::VkMat in_gpu;
        {
            cmd.record_clone(in, in_gpu, opt);

            if (xtiles > 1) {
                cmd.submit_and_wait();
                cmd.reset();
            }
        }

        int out_tile_y0 = std::max(yi * TILE_SIZE_Y, 0);
        int out_tile_y1 = std::min((yi + 1) * TILE_SIZE_Y, h);

        ncnn::VkMat out_gpu;
        if (opt.use_fp16_storage && opt.use_int8_storage) {
            out_gpu.create(w * scale, (out_tile_y1 - out_tile_y0) * scale, (size_t)channels, 1, blob_vkallocator);
        } else {
            out_gpu.create(w * scale, (out_tile_y1 - out_tile_y0) * scale, channels, (size_t)4u, 1, blob_vkallocator);
        }
        #pragma endregion

        for (int xi = 0; xi < xtiles; xi++) {
            const int tile_w_nopad = std::min((xi + 1) * TILE_SIZE_X, w) - xi * TILE_SIZE_X;

            int prepadding_right = prepadding;
            if (scale == 1) {
                prepadding_right += (tile_w_nopad + 3) / 4 * 4 - tile_w_nopad;
            }
            if (scale == 2) {
                prepadding_right += (tile_w_nopad + 1) / 2 * 2 - tile_w_nopad;
            }

            if (tta_mode) {
                ncnn::VkMat in_tile_gpu[8];
                ncnn::VkMat in_alpha_tile_gpu;
                {
                    #pragma region pre
                    // crop tile
                    int tile_x0 = xi * TILE_SIZE_X - prepadding;
                    int tile_x1 = std::min((xi + 1) * TILE_SIZE_X, w) + prepadding_right;
                    int tile_y0 = yi * TILE_SIZE_Y - prepadding;
                    int tile_y1 = std::min((yi + 1) * TILE_SIZE_Y, h) + prepadding_bottom;

                    in_tile_gpu[0].create(tile_x1 - tile_x0, tile_y1 - tile_y0, 3, in_out_tile_elemsize, 1, blob_vkallocator);
                    in_tile_gpu[1].create(tile_x1 - tile_x0, tile_y1 - tile_y0, 3, in_out_tile_elemsize, 1, blob_vkallocator);
                    in_tile_gpu[2].create(tile_x1 - tile_x0, tile_y1 - tile_y0, 3, in_out_tile_elemsize, 1, blob_vkallocator);
                    in_tile_gpu[3].create(tile_x1 - tile_x0, tile_y1 - tile_y0, 3, in_out_tile_elemsize, 1, blob_vkallocator);
                    in_tile_gpu[4].create(tile_y1 - tile_y0, tile_x1 - tile_x0, 3, in_out_tile_elemsize, 1, blob_vkallocator);
                    in_tile_gpu[5].create(tile_y1 - tile_y0, tile_x1 - tile_x0, 3, in_out_tile_elemsize, 1, blob_vkallocator);
                    in_tile_gpu[6].create(tile_y1 - tile_y0, tile_x1 - tile_x0, 3, in_out_tile_elemsize, 1, blob_vkallocator);
                    in_tile_gpu[7].create(tile_y1 - tile_y0, tile_x1 - tile_x0, 3, in_out_tile_elemsize, 1, blob_vkallocator);

                    if (channels == 4) {
                        in_alpha_tile_gpu.create(tile_w_nopad, tile_h_nopad, 1, in_out_tile_elemsize, 1, blob_vkallocator);
                    }

                    std::vector<ncnn::VkMat> bindings(10);
                    bindings[0] = in_gpu;
                    bindings[1] = in_tile_gpu[0];
                    bindings[2] = in_tile_gpu[1];
                    bindings[3] = in_tile_gpu[2];
                    bindings[4] = in_tile_gpu[3];
                    bindings[5] = in_tile_gpu[4];
                    bindings[6] = in_tile_gpu[5];
                    bindings[7] = in_tile_gpu[6];
                    bindings[8] = in_tile_gpu[7];
                    bindings[9] = in_alpha_tile_gpu;

                    std::vector<ncnn::vk_constant_type> constants(13);
                    constants[0].i = in_gpu.w;
                    constants[1].i = in_gpu.h;
                    constants[2].i = in_gpu.cstep;
                    constants[3].i = in_tile_gpu[0].w;
                    constants[4].i = in_tile_gpu[0].h;
                    constants[5].i = in_tile_gpu[0].cstep;
                    constants[6].i = prepadding;
                    constants[7].i = prepadding;
                    constants[8].i = xi * TILE_SIZE_X;
                    constants[9].i = std::min(yi * TILE_SIZE_Y, prepadding);
                    constants[10].i = channels;
                    constants[11].i = in_alpha_tile_gpu.w;
                    constants[12].i = in_alpha_tile_gpu.h;

                    ncnn::VkMat dispatcher;
                    dispatcher.w = in_tile_gpu[0].w;
                    dispatcher.h = in_tile_gpu[0].h;
                    dispatcher.c = channels;

                    cmd.record_pipeline(this->pre_pipe, bindings, constants, dispatcher);
                    #pragma endregion
                }

                #pragma region waifu2x
                ncnn::VkMat out_tile_gpu[8];
                for (int ti = 0; ti < 8; ti++) {
                    ncnn::Extractor ex = net.create_extractor();

                    ex.set_blob_vkallocator(blob_vkallocator);
                    ex.set_workspace_vkallocator(blob_vkallocator);
                    ex.set_staging_vkallocator(staging_vkallocator);

                    ex.input("Input1", in_tile_gpu[ti]);

                    ex.extract("Eltwise4", out_tile_gpu[ti], cmd);
                }
                #pragma endregion

                #pragma region process_alpha
                ncnn::VkMat out_alpha_tile_gpu;
                if (channels == 4) {
                    if (scale == 1) {
                        out_alpha_tile_gpu = in_alpha_tile_gpu;
                    }
                    if (scale == 2) {
                        bicubic_2x->forward(in_alpha_tile_gpu, out_alpha_tile_gpu, cmd, opt);
                    }
                }
                #pragma endregion

                {
                    #pragma region post
                    std::vector<ncnn::VkMat> bindings(10);
                    bindings[0] = out_tile_gpu[0];
                    bindings[1] = out_tile_gpu[1];
                    bindings[2] = out_tile_gpu[2];
                    bindings[3] = out_tile_gpu[3];
                    bindings[4] = out_tile_gpu[4];
                    bindings[5] = out_tile_gpu[5];
                    bindings[6] = out_tile_gpu[6];
                    bindings[7] = out_tile_gpu[7];
                    bindings[8] = out_alpha_tile_gpu;
                    bindings[9] = out_gpu;

                    std::vector<ncnn::vk_constant_type> constants(11);
                    constants[0].i = out_tile_gpu[0].w;
                    constants[1].i = out_tile_gpu[0].h;
                    constants[2].i = out_tile_gpu[0].cstep;
                    constants[3].i = out_gpu.w;
                    constants[4].i = out_gpu.h;
                    constants[5].i = out_gpu.cstep;
                    constants[6].i = xi * TILE_SIZE_X * scale;
                    constants[7].i = std::min(TILE_SIZE_X * scale, out_gpu.w - xi * TILE_SIZE_X * scale);
                    constants[8].i = channels;
                    constants[9].i = out_alpha_tile_gpu.w;
                    constants[10].i = out_alpha_tile_gpu.h;

                    ncnn::VkMat dispatcher;
                    dispatcher.w = std::min(TILE_SIZE_X * scale, out_gpu.w - xi * TILE_SIZE_X * scale);
                    dispatcher.h = out_gpu.h;
                    dispatcher.c = channels;

                    cmd.record_pipeline(this->post_pipe, bindings, constants, dispatcher);
                    #pragma endregion
                }
            } else {
                // preproc
                ncnn::VkMat in_tile_gpu;
                ncnn::VkMat in_alpha_tile_gpu;
                {
                    #pragma region pre
                    // crop tile
                    int tile_x0 = xi * TILE_SIZE_X - prepadding;
                    int tile_x1 = std::min((xi + 1) * TILE_SIZE_X, w) + prepadding_right;
                    int tile_y0 = yi * TILE_SIZE_Y - prepadding;
                    int tile_y1 = std::min((yi + 1) * TILE_SIZE_Y, h) + prepadding_bottom;

                    in_tile_gpu.create(tile_x1 - tile_x0, tile_y1 - tile_y0, 3, in_out_tile_elemsize, 1, blob_vkallocator);

                    if (channels == 4) {
                        in_alpha_tile_gpu.create(tile_w_nopad, tile_h_nopad, 1, in_out_tile_elemsize, 1, blob_vkallocator);
                    }

                    std::vector<ncnn::VkMat> bindings(3);
                    bindings[0] = in_gpu;
                    bindings[1] = in_tile_gpu;
                    bindings[2] = in_alpha_tile_gpu;

                    std::vector<ncnn::vk_constant_type> constants(13);
                    constants[0].i = in_gpu.w;
                    constants[1].i = in_gpu.h;
                    constants[2].i = in_gpu.cstep;
                    constants[3].i = in_tile_gpu.w;
                    constants[4].i = in_tile_gpu.h;
                    constants[5].i = in_tile_gpu.cstep;
                    constants[6].i = prepadding;
                    constants[7].i = prepadding;
                    constants[8].i = xi * TILE_SIZE_X;
                    constants[9].i = std::min(yi * TILE_SIZE_Y, prepadding);
                    constants[10].i = channels;
                    constants[11].i = in_alpha_tile_gpu.w;
                    constants[12].i = in_alpha_tile_gpu.h;

                    ncnn::VkMat dispatcher;
                    dispatcher.w = in_tile_gpu.w;
                    dispatcher.h = in_tile_gpu.h;
                    dispatcher.c = channels;

                    cmd.record_pipeline(this->pre_pipe, bindings, constants, dispatcher);
                    #pragma endregion
                }

                #pragma region waifu2x
                ncnn::VkMat out_tile_gpu;
                {
                    ncnn::Extractor ex = net.create_extractor();

                    ex.set_blob_vkallocator(blob_vkallocator);
                    ex.set_workspace_vkallocator(blob_vkallocator);
                    ex.set_staging_vkallocator(staging_vkallocator);

                    ex.input("Input1", in_tile_gpu);

                    ex.extract("Eltwise4", out_tile_gpu, cmd);
                }
                #pragma endregion

                #pragma region process_alpha
                ncnn::VkMat out_alpha_tile_gpu;
                if (channels == 4) {
                    if (scale == 1) {
                        out_alpha_tile_gpu = in_alpha_tile_gpu;
                    }
                    if (scale == 2) {
                        bicubic_2x->forward(in_alpha_tile_gpu, out_alpha_tile_gpu, cmd, opt);
                    }
                }
                #pragma endregion

                {
                    #pragma region post
                    std::vector<ncnn::VkMat> bindings(3);
                    bindings[0] = out_tile_gpu;
                    bindings[1] = out_alpha_tile_gpu;
                    bindings[2] = out_gpu;

                    std::vector<ncnn::vk_constant_type> constants(11);
                    constants[0].i = out_tile_gpu.w;
                    constants[1].i = out_tile_gpu.h;
                    constants[2].i = out_tile_gpu.cstep;
                    constants[3].i = out_gpu.w;
                    constants[4].i = out_gpu.h;
                    constants[5].i = out_gpu.cstep;
                    constants[6].i = xi * TILE_SIZE_X * scale;
                    constants[7].i = std::min(TILE_SIZE_X * scale, out_gpu.w - xi * TILE_SIZE_X * scale);
                    constants[8].i = channels;
                    constants[9].i = out_alpha_tile_gpu.w;
                    constants[10].i = out_alpha_tile_gpu.h;

                    ncnn::VkMat dispatcher;
                    dispatcher.w = std::min(TILE_SIZE_X * scale, out_gpu.w - xi * TILE_SIZE_X * scale);
                    dispatcher.h = out_gpu.h;
                    dispatcher.c = channels;

                    cmd.record_pipeline(this->post_pipe, bindings, constants, dispatcher);
                    #pragma endregion
                }
            }

            if (xtiles > 1) {
                cmd.submit_and_wait();
                cmd.reset();
            }
            if (processStatus) {
                processStatus->tile_index = yi * xtiles + xi + 1;
            }
        }

        #pragma region copy
        {
            ncnn::Mat out;

            if (opt.use_fp16_storage && opt.use_int8_storage) {
                out = ncnn::Mat(out_gpu.w, out_gpu.h, (unsigned char*)outdata + yi * scale * TILE_SIZE_Y * w * scale * channels, (size_t)channels, 1);
            }

            cmd.record_clone(out_gpu, out, opt);

            cmd.submit_and_wait();

            if (!(opt.use_fp16_storage && opt.use_int8_storage)) {
                out.to_pixels((unsigned char*)outdata + yi * scale * TILE_SIZE_Y * w * scale * channels,
                    channels == 4 ? ncnn::Mat::PIXEL_RGBA : ncnn::Mat::PIXEL_RGB);
            }
        }
        #pragma endregion
    }

    vkdev->reclaim_blob_allocator(blob_vkallocator);
    vkdev->reclaim_staging_allocator(staging_vkallocator);

    return 0;
}

int Waifu2x::process_cpu(int scale, Waifu2xProcessStatus* processStatus, int w, int h, int channels, const byte* pixeldata, void* outdata) {
    int prepadding = model->prePadding;
    bool tta_mode = model->ttaMode;

    const int TILE_SIZE_X = model->tileSize;
    const int TILE_SIZE_Y = model->tileSize;

    ncnn::Option opt = net.opt;

    // each tile 400x400
    const int xtiles = (w + TILE_SIZE_X - 1) / TILE_SIZE_X;
    const int ytiles = (h + TILE_SIZE_Y - 1) / TILE_SIZE_Y;

    if (processStatus) {
        processStatus->tile_count = xtiles * ytiles;
        processStatus->tile_index = 0;
    }

    for (int yi = 0; yi < ytiles; yi++) {
        const int tile_h_nopad = std::min((yi + 1) * TILE_SIZE_Y, h) - yi * TILE_SIZE_Y;

        int prepadding_bottom = prepadding;
        if (scale == 1) {
            prepadding_bottom += (tile_h_nopad + 3) / 4 * 4 - tile_h_nopad;
        }
        if (scale == 2) {
            prepadding_bottom += (tile_h_nopad + 1) / 2 * 2 - tile_h_nopad;
        }

        int in_tile_y0 = std::max(yi * TILE_SIZE_Y - prepadding, 0);
        int in_tile_y1 = std::min((yi + 1) * TILE_SIZE_Y + prepadding_bottom, h);

        for (int xi = 0; xi < xtiles; xi++) {
            const int tile_w_nopad = std::min((xi + 1) * TILE_SIZE_X, w) - xi * TILE_SIZE_X;

            int prepadding_right = prepadding;
            if (scale == 1) {
                prepadding_right += (tile_w_nopad + 3) / 4 * 4 - tile_w_nopad;
            }
            if (scale == 2) {
                prepadding_right += (tile_w_nopad + 1) / 2 * 2 - tile_w_nopad;
            }

            int in_tile_x0 = std::max(xi * TILE_SIZE_X - prepadding, 0);
            int in_tile_x1 = std::min((xi + 1) * TILE_SIZE_X + prepadding_right, w);
            
            #pragma region mat
            // crop tile
            ncnn::Mat in = ncnn::Mat::from_pixels_roi(pixeldata, 
                    channels == 4 ? ncnn::Mat::PIXEL_RGBA : ncnn::Mat::PIXEL_RGB,
                    w, h, in_tile_x0, in_tile_y0, in_tile_x1 - in_tile_x0, in_tile_y1 - in_tile_y0);

            ncnn::Mat out;
            #pragma endregion

            if (tta_mode) {
                #pragma region TTA
                
                ncnn::Mat in_tile[8];
                ncnn::Mat in_alpha_tile;
                {
                    #pragma region pre
                    in_tile[0].create(in.w, in.h, 3);
                    for (int q = 0; q < 3; q++) {
                        const float* ptr = in.channel(q);
                        float* outptr0 = in_tile[0].channel(q);

                        for (int i = 0; i < in.h; i++) {
                            for (int j = 0; j < in.w; j++) {
                                *outptr0++ = *ptr++ * (1 / 255.f);
                            }
                        }
                    }

                    if (channels == 4) {
                        in_alpha_tile = in.channel_range(3, 1).clone();
                    }
                    #pragma endregion
                }

                // border padding
                {
                    int pad_top = std::max(prepadding - yi * TILE_SIZE_Y, 0);
                    int pad_bottom = std::max(std::min((yi + 1) * TILE_SIZE_Y + prepadding_bottom - h, prepadding_bottom), 0);
                    int pad_left = std::max(prepadding - xi * TILE_SIZE_X, 0);
                    int pad_right = std::max(std::min((xi + 1) * TILE_SIZE_X + prepadding_right - w, prepadding_right), 0);

                    ncnn::Mat in_tile_padded;
                    ncnn::copy_make_border(in_tile[0], in_tile_padded, pad_top, pad_bottom, pad_left, pad_right, ncnn::BORDER_REPLICATE, 0.f, net.opt);
                    in_tile[0] = in_tile_padded;
                }

                // the other 7 directions
                {
                    in_tile[1].create(in_tile[0].w, in_tile[0].h, 3);
                    in_tile[2].create(in_tile[0].w, in_tile[0].h, 3);
                    in_tile[3].create(in_tile[0].w, in_tile[0].h, 3);
                    in_tile[4].create(in_tile[0].h, in_tile[0].w, 3);
                    in_tile[5].create(in_tile[0].h, in_tile[0].w, 3);
                    in_tile[6].create(in_tile[0].h, in_tile[0].w, 3);
                    in_tile[7].create(in_tile[0].h, in_tile[0].w, 3);

                    for (int q = 0; q < 3; q++) {
                        const ncnn::Mat in_tile_0 = in_tile[0].channel(q);
                        ncnn::Mat in_tile_1 = in_tile[1].channel(q);
                        ncnn::Mat in_tile_2 = in_tile[2].channel(q);
                        ncnn::Mat in_tile_3 = in_tile[3].channel(q);
                        ncnn::Mat in_tile_4 = in_tile[4].channel(q);
                        ncnn::Mat in_tile_5 = in_tile[5].channel(q);
                        ncnn::Mat in_tile_6 = in_tile[6].channel(q);
                        ncnn::Mat in_tile_7 = in_tile[7].channel(q);

                        for (int i = 0; i < in_tile[0].h; i++) {
                            const float* outptr0 = in_tile_0.row(i);
                            float* outptr1 = in_tile_1.row(in_tile[0].h - 1 - i);
                            float* outptr2 = in_tile_2.row(i) + in_tile[0].w - 1;
                            float* outptr3 = in_tile_3.row(in_tile[0].h - 1 - i) + in_tile[0].w - 1;

                            for (int j = 0; j < in_tile[0].w; j++) {
                                float* outptr4 = in_tile_4.row(j) + i;
                                float* outptr5 = in_tile_5.row(in_tile[0].w - 1 - j) + i;
                                float* outptr6 = in_tile_6.row(j) + in_tile[0].h - 1 - i;
                                float* outptr7 = in_tile_7.row(in_tile[0].w - 1 - j) + in_tile[0].h - 1 - i;

                                float v = *outptr0++;

                                *outptr1++ = v;
                                *outptr2-- = v;
                                *outptr3-- = v;
                                *outptr4 = v;
                                *outptr5 = v;
                                *outptr6 = v;
                                *outptr7 = v;
                            }
                        }
                    }
                }

                #pragma region waifu2x
                ncnn::Mat out_tile[8];
                for (int ti = 0; ti < 8; ti++) {
                    ncnn::Extractor ex = net.create_extractor();

                    ex.input("Input1", in_tile[ti]);

                    ex.extract("Eltwise4", out_tile[ti]);
                }
                #pragma endregion

                #pragma region process_alpha
                ncnn::Mat out_alpha_tile;
                if (channels == 4) {
                    if (scale == 1) {
                        out_alpha_tile = in_alpha_tile;
                    }
                    if (scale == 2) {
                        bicubic_2x->forward(in_alpha_tile, out_alpha_tile, opt);
                    }
                }
                #pragma endregion

                {
                    #pragma region post
                    out.create(tile_w_nopad * scale, tile_h_nopad * scale, channels);
                    for (int q = 0; q < 3; q++) {
                        const ncnn::Mat out_tile_0 = out_tile[0].channel(q);
                        const ncnn::Mat out_tile_1 = out_tile[1].channel(q);
                        const ncnn::Mat out_tile_2 = out_tile[2].channel(q);
                        const ncnn::Mat out_tile_3 = out_tile[3].channel(q);
                        const ncnn::Mat out_tile_4 = out_tile[4].channel(q);
                        const ncnn::Mat out_tile_5 = out_tile[5].channel(q);
                        const ncnn::Mat out_tile_6 = out_tile[6].channel(q);
                        const ncnn::Mat out_tile_7 = out_tile[7].channel(q);
                        float* outptr = out.channel(q);

                        for (int i = 0; i < out.h; i++) {
                            const float* ptr0 = out_tile_0.row(i);
                            const float* ptr1 = out_tile_1.row(out_tile[0].h - 1 - i);
                            const float* ptr2 = out_tile_2.row(i) + out_tile[0].w - 1;
                            const float* ptr3 = out_tile_3.row(out_tile[0].h - 1 - i) + out_tile[0].w - 1;

                            for (int j = 0; j < out.w; j++) {
                                const float* ptr4 = out_tile_4.row(j) + i;
                                const float* ptr5 = out_tile_5.row(out_tile[0].w - 1 - j) + i;
                                const float* ptr6 = out_tile_6.row(j) + out_tile[0].h - 1 - i;
                                const float* ptr7 = out_tile_7.row(out_tile[0].w - 1 - j) + out_tile[0].h - 1 - i;

                                float v = (*ptr0++ + *ptr1++ + *ptr2-- + *ptr3-- + *ptr4 + *ptr5 + *ptr6 + *ptr7) / 8;

                                *outptr++ = v * 255.f + 0.5f;
                            }
                        }
                    }

                    if (channels == 4) {
                        memcpy(out.channel_range(3, 1), out_alpha_tile, out_alpha_tile.total() * sizeof(float));
                    }
                    #pragma endregion
                }
                #pragma endregion
            } else {
                ncnn::Mat in_tile;
                ncnn::Mat in_alpha_tile;
                {
                    #pragma region pre
                    in_tile.create(in.w, in.h, 3);
                    for (int q = 0; q < 3; q++) {
                        const float* ptr = in.channel(q);
                        float* outptr = in_tile.channel(q);

                        for (int i = 0; i < in.w * in.h; i++) {
                            *outptr++ = *ptr++ * (1 / 255.f);
                        }
                    }

                    if (channels == 4) {
                        in_alpha_tile = in.channel_range(3, 1).clone();
                    }
                    #pragma endregion
                }

                // border padding
                {
                    int pad_top = std::max(prepadding - yi * TILE_SIZE_Y, 0);
                    int pad_bottom = std::max(std::min((yi + 1) * TILE_SIZE_Y + prepadding_bottom - h, prepadding_bottom), 0);
                    int pad_left = std::max(prepadding - xi * TILE_SIZE_X, 0);
                    int pad_right = std::max(std::min((xi + 1) * TILE_SIZE_X + prepadding_right - w, prepadding_right), 0);

                    ncnn::Mat in_tile_padded;
                    ncnn::copy_make_border(in_tile, in_tile_padded, pad_top, pad_bottom, pad_left, pad_right, ncnn::BORDER_REPLICATE, 0.f, net.opt);
                    in_tile = in_tile_padded;
                }

                #pragma region waifu2x
                ncnn::Mat out_tile;
                {
                    ncnn::Extractor ex = net.create_extractor();
                    ex.input("Input1", in_tile);
                    ex.extract("Eltwise4", out_tile);
                }
                #pragma endregion

                #pragma region process_alpha
                ncnn::Mat out_alpha_tile;
                if (channels == 4) {
                    if (scale == 1) {
                        out_alpha_tile = in_alpha_tile;
                    }
                    if (scale == 2) {
                        bicubic_2x->forward(in_alpha_tile, out_alpha_tile, opt);
                    }
                }
                #pragma endregion

                {
                    #pragma region post
                    out.create(tile_w_nopad * scale, tile_h_nopad * scale, channels);
                    for (int q = 0; q < 3; q++) {
                        float* outptr = out.channel(q);

                        for (int i = 0; i < out.h; i++) {
                            const float* ptr = out_tile.channel(q).row(i);

                            for (int j = 0; j < out.w; j++) {
                                *outptr++ = *ptr++ * 255.f + 0.5f;
                            }
                        }
                    }

                    if (channels == 4) {
                        memcpy(out.channel_range(3, 1), out_alpha_tile, out_alpha_tile.total() * sizeof(float));
                    }
                    #pragma endregion
                }
            }

            out.to_pixels((unsigned char*)outdata + yi * scale * TILE_SIZE_Y * w * scale * channels + xi * scale * TILE_SIZE_X * channels,
                channels == 4 ? ncnn::Mat::PIXEL_RGBA : ncnn::Mat::PIXEL_RGB, w * scale * channels);

            if (processStatus) {
                processStatus->tile_index = yi * xtiles + xi + 1;
            }
        }
    }

    return 0;
}
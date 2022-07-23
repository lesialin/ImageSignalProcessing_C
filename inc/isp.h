#pragma once

#if defined(_MSC_VER)
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#include <stdint.h>
#include <iostream>
#include <string>
#include <fstream>
#include "nlohmann/json.hpp"

#define GAIN_FRACTION_BITS 6 //awb gain control
#define BLC_FRACTION 14 //black level compenstation
#define CCM_FRACTION 10 // color conversion matrix
#define GAMMA_STEP 2 // gamma table

#define clip_max(x, value) x > value ? value : x
#define clip_min(x, value) x < value ? value : x

using namespace std;

typedef struct
{
    float r_gain;
    float gr_gain;
    float gb_gain;
    float b_gain;

} awb_gain_t;

typedef struct
{
    uint16_t bl_r;
    uint16_t bl_gr;
    uint16_t bl_gb;
    uint16_t bl_b;
    float alpha;
    float beta;

} blc_cfg_t;

typedef struct
{
    // image resolution
    uint32_t image_width;
    uint32_t image_height;
    uint16_t image_bits;
    // bayer pattern, ex rggb
    string bayer_pattern;
    // black level correction
    blc_cfg_t blc_config;
    // dead pixel correction
    uint16_t dpc_dead_thres;
    string dpc_mode;
    // awb gain
    awb_gain_t awb_gain;
    // color correction matrix
    float ccm_matrix[12];
    //cfa mode
    string cfa_mode;
    // gamma correction parameter
    float gamma;

} isp_config_t;

EXPORT void isp_init();
EXPORT void isp_raw_run(uint16_t *raw_buf, uint8_t *rgb_buf);
EXPORT void isp_load_config(string filename, isp_config_t *isp_config);
EXPORT void isp_blc(uint16_t *raw_buf);
EXPORT void isp_dpc(uint16_t *raw_buf);
EXPORT void isp_aaf(uint16_t *raw_buf);
EXPORT void isp_awb_gain(uint16_t *raw_buf);
EXPORT void isp_ccm(uint16_t *rgb_buf);
EXPORT void isp_cfa(uint16_t *raw_buf, uint16_t *rgb_buf);
EXPORT void isp_gac(uint16_t *rgb_buf, uint8_t *gamma_table, uint16_t table_size, uint8_t *ga_rgb_buf);
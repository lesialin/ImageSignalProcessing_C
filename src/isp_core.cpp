#include "isp.h"
#include <cmath>
#include <vector>

using json = nlohmann::json;
extern isp_config_t g_isp_config;
uint8_t *gamma_table;
uint16_t g_table_size;

void isp_load_config(string filename, isp_config_t *isp_config)
{

    // uint32_t ccm_matrix[12] = {0};

    std::ifstream file(filename);
    json j = json::parse(file);

    isp_config->image_height = j["height"];
    isp_config->image_width = j["width"];
    isp_config->image_bits = j["image_bits"];
    isp_config->bayer_pattern = j["bayer_pattern"];
    isp_config->blc_config.bl_r = j["blc"]["bl_r"];
    isp_config->blc_config.bl_gr = j["blc"]["bl_gr"];
    isp_config->blc_config.bl_gb = j["blc"]["bl_gb"];
    isp_config->blc_config.bl_b = j["blc"]["bl_b"];
    isp_config->blc_config.alpha = j["blc"]["alpha"];
    isp_config->blc_config.beta = j["blc"]["beta"];
    isp_config->dpc_dead_thres = j["dpc"]["dead_thres"];
    isp_config->dpc_mode = j["dpc"]["mode"];
    isp_config->awb_gain.r_gain = j["awb_gain"]["r_gain"];
    isp_config->awb_gain.gr_gain = j["awb_gain"]["gr_gain"];
    isp_config->awb_gain.gb_gain = j["awb_gain"]["gb_gain"];
    isp_config->awb_gain.b_gain = j["awb_gain"]["b_gain"];
    isp_config->cfa_mode = j["cfa_mode"];
    isp_config->gamma = j["gamma"];
    std::vector<std::vector<float>> m;
    m = j["ccm"].get<std::vector<std::vector<float>>>();
    int count = 0;
    for (auto i : m)
    {
        for (auto j : i)
        {

            isp_config->ccm_matrix[count] = (float)j;
            count++;
        }
    }

#if 0
    //log config
    cout << "isp pipeline config:" << endl;
    cout << "image resolution = " << isp_config->image_width << "x" << isp_config->image_height << "," << isp_config->image_bits << " bits/pixels" << endl;
    cout << "bayer_patter = " << isp_config->bayer_pattern << endl;
    cout << "gamma = " << isp_config->gamma << endl;
    cout << "dpc_threshold = " << isp_config->dpc_threshold << endl;
    cout << "awb gain :" << endl;
    cout << "r_gain = " << isp_config->awb_gain.r_gain << endl;
    cout << "gr_gain = " << isp_config->awb_gain.gr_gain << endl;
    cout << "gb_gain = " << isp_config->awb_gain.gb_gain << endl;
    cout << "b_gain = " << isp_config->awb_gain.b_gain << endl;
    cout << "ccm matrix:" << endl;
    cout << isp_config->ccm_matrix[0] << "," << isp_config->ccm_matrix[1] << "," << isp_config->ccm_matrix[2] << "," << isp_config->ccm_matrix[3] << endl;
    cout << isp_config->ccm_matrix[4] << "," << isp_config->ccm_matrix[5] << "," << isp_config->ccm_matrix[6] << "," << isp_config->ccm_matrix[7] << endl;
    cout << isp_config->ccm_matrix[8] << "," << isp_config->ccm_matrix[9] << "," << isp_config->ccm_matrix[10] << "," << isp_config->ccm_matrix[11] << endl;
#endif
}

void isp_init()
{
    uint16_t in_max_value, out_max_value;
    float gamma;

    // init gamma table
    in_max_value = (1 << g_isp_config.image_bits);
    out_max_value = (1 << 8);
    gamma = g_isp_config.gamma;
   

    uint8_t *s_gamma_table;
    
    g_table_size = in_max_value >> GAMMA_STEP;
    
    gamma_table = (uint8_t *)malloc(sizeof(uint8_t) * g_table_size);
    for (int i = 0; i < g_table_size; i++)
    {
        gamma_table[i] = (uint8_t)(pow((float)(i * (1<<GAMMA_STEP)+ (1<<(GAMMA_STEP-1))) / in_max_value, gamma) * out_max_value);
        //printf("gamma_table[%d] = %d\n",i, gamma_table[i]);
    }
}

void isp_raw_run(uint16_t *raw_buf, uint8_t *rgb_buf)
{

    uint32_t image_size;
    uint16_t *rgb_10b_buf;
    uint16_t max_value, table_size;

    image_size = g_isp_config.image_height * g_isp_config.image_width;
    rgb_10b_buf = (uint16_t *)malloc(sizeof(uint16_t) * image_size * 3);
    // dead pixel correction
    isp_dpc(raw_buf);
    // black level correction
    isp_blc(raw_buf);
    //anti-aliasing filter
    isp_aaf(raw_buf);
    // awb gain
    isp_awb_gain(raw_buf);
    // cfa interpolation
    isp_cfa(raw_buf, rgb_10b_buf);
    // color correction matrix
    isp_ccm(rgb_10b_buf);
    // gamma correction 
    max_value = (1 << g_isp_config.image_bits);
    table_size = max_value >> GAMMA_STEP;
    isp_gac(rgb_10b_buf, gamma_table, table_size, rgb_buf);




    free(rgb_10b_buf);
}


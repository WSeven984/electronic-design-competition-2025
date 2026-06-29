/**
 * @file key_app.c
 * @brief 按键应用处理程序 (重构版)
 * @details
 * 该文件实现了按键扫描和相关的应用逻辑。
 * 它使用一个配置数组来管理多个按键，将每个按键的物理定义（GPIO）
 * 与其逻辑功能（回调函数）分离开来，从而提高了代码的可扩展性和可维护性。
 *
 * @author 左岚
 * @date 2025-07-18
 * @version 2.0 (已重构，提高可扩展性)
 */
#include "key_app.h"
#include "ad_measure.h"
#include "my_fft.h"
#include "da_output.h"
#include "AD9959.h"

// *********************************************************************************
// 1. 类型定义和常量
// *********************************************************************************

#define NUM_KEYS 4 // 定义按键的总数

// 定义一个函数指针类型，用于按键的回调函数
typedef void (*key_callback_t)(void);

// 定义一个结构体，用于保存单个按键的所有信息
typedef struct {
    GPIO_TypeDef* port;     // 按键连接的GPIO端口
    uint16_t       pin;      // 按键连接的GPIO引脚
    key_callback_t callback; // 按键按下时要调用的回调函数
} key_info_t;

// *********************************************************************************
// 2. 静态回调函数的原型声明
// *********************************************************************************
static void key1_action_toggle_ask(void);
static void key2_action_toggle_fsk(void);
static void key3_action_cycle_phase(void);
static void key4_action_perform_fft(void);

// *********************************************************************************
// 3. 按键配置数组 <--- 这是设计的核心！
// *********************************************************************************
static const key_info_t key_config[NUM_KEYS] = {
    {KEY1_GPIO_Port, KEY1_Pin, key1_action_toggle_ask}, // 按键1 -> 切换ASK
    {KEY2_GPIO_Port, KEY2_Pin, key2_action_toggle_fsk}, // 按键2 -> 切换FSK
    {KEY3_GPIO_Port, KEY3_Pin, key3_action_cycle_phase},// 按键3 -> 调整相位
    {KEY4_GPIO_Port, KEY4_Pin, key4_action_perform_fft} // 按键4 -> 执行FFT
    // 如果要添加第5个按键，只需在这里加一行即可！
};

// *********************************************************************************
// 4. 全局及静态变量
// *********************************************************************************
// 使用位掩码(bitmask)来保存按键状态，更高效且能处理组合键
uint8_t key_map_now = 0;
uint8_t key_map_old = 0;
uint8_t key_map_down = 0;
// uint8_t key_map_up = 0; // 如果需要弹起检测，可以取消注释

static float current_ad_freq = 2000000.0f;

// *********************************************************************************
// 5. 函数实现
// *********************************************************************************

/**
 * @brief 读取所有物理按键的当前状态，并返回一个位掩码(bitmask)
 * @details 遍历`key_config`数组，扫描每个已定义的按键。如果按键被按下（低电平），
 * 就在返回值的对应位上置1。例如，按键0按下，返回 0b0001；按键1按下，返回 0b0010。
 * @return uint8_t 包含所有按键状态的位掩码。
 */
static uint8_t key_read_map(void)
{
    uint8_t map = 0;
    for (int i = 0; i < NUM_KEYS; i++) {
        if (HAL_GPIO_ReadPin(key_config[i].port, key_config[i].pin) == GPIO_PIN_RESET) {
            map |= (1 << i);
        }
    }
    return map;
}

/**
 * @brief 按键处理主函数 (重构版)
 * @details
 * 1. 调用`key_read_map()`获取所有按键状态的位掩码。
 * 2. 计算哪些按键刚刚被按下。
 * 3. 遍历所有按键，如果检测到某个按键被按下，就调用其在`key_config`数组中注册的回调函数。
 */
void key_proc(void)
{
    key_map_now = key_read_map();
    key_map_down = key_map_now & (key_map_old ^ key_map_now); // 完美的下降沿检测
    // key_map_up = ~key_map_now & (key_map_old ^ key_map_now);
    key_map_old = key_map_now;

    if (key_map_down == 0) {
        return; // 如果没有任何按键被按下，直接返回
    }

    // 遍历所有可能的按键
    for (int i = 0; i < NUM_KEYS; i++) {
        // 检查第i个按键是否被按下
        if (key_map_down & (1 << i)) {
            // 如果是，并且它有关联的回调函数，则调用它
            if (key_config[i].callback != NULL) {
                key_config[i].callback();
            }
        }
    }
}

// --- 外部可调用函数 ---
void set_current_ad_frequency(float freq)
{
    current_ad_freq = freq;
}

float get_current_ad_frequency(void)
{
    return current_ad_freq;
}


// *********************************************************************************
// 6. 静态回调函数的具体实现
// *********************************************************************************

/** @brief 按键1的功能：启动/停止ASK连续发送 */
static void key1_action_toggle_ask(void)
{
//    if (AD9959_Is_ASK_Active()) {
//        AD9959_Stop_ASK_Transmission();
//        my_printf(&huart3, "Button1: ASK transmission stopped\r\n");
//    } else {
//        AD9959_Start_ASK_Transmission();
//        my_printf(&huart3, "Button1: ASK transmission started\r\n");
//    }
}

/** @brief 按键2的功能：启动/停止FSK连续发送 */
static void key2_action_toggle_fsk(void)
{
//    if (AD9959_Is_FSK_Active()) {
//        AD9959_Stop_FSK_Transmission();
//        my_printf(&huart3, "Button2: FSK transmission stopped\r\n");
//    } else {
//        AD9959_Start_FSK_Transmission();
//        my_printf(&huart3, "Button2: FSK transmission started\r\n");
//    }
}

/** @brief 按键3的功能：循环调节DA输出参数（增强版） */
static void key3_action_cycle_phase(void)
{
    // 定义调节模式枚举
    typedef enum {
        MODE_PHASE = 0,     // 相位调节模式
        MODE_FREQUENCY,     // 频率调节模式
        MODE_AMPLITUDE,     // 幅度调节模式
        MODE_WAVEFORM,      // 波形调节模式
        MODE_COUNT          // 模式总数
    } adjust_mode_t;

    static adjust_mode_t current_mode = MODE_FREQUENCY;
    static uint16_t phase_step = 0;
    static uint8_t freq_step = 0;
    static uint8_t amp_step = 0;
    static uint8_t wave_step = 0;

    // 频率调节参数在MODE_FREQUENCY case中定义

    // 预定义的幅度值
    static const uint16_t amp_values[] = {500, 1000, 1500, 2000, 2500};
    static const uint8_t amp_count = sizeof(amp_values) / sizeof(amp_values[0]);

    // 波形类型数组
    static const Waveform_t wave_types[] = {WAVE_SINE, WAVE_SQUARE, WAVE_TRIANGLE, WAVE_SAWTOOTH};
    static const char* wave_names[] = {"SINE", "SQUARE", "TRIANGLE", "SAWTOOTH"};
    static const uint8_t wave_count = sizeof(wave_types) / sizeof(wave_types[0]);

    switch (current_mode) {
        case MODE_PHASE:
            // 相位调节：每次增加30度
            phase_step = (phase_step + 1) % 12; // 12步 = 360度/30度
            uint16_t new_phase = phase_step * 30;
            DA_SetPhase(0, new_phase);
            DA_Apply_Settings();
            my_printf(&huart3, "DA1 Phase: %d degrees\r\n", new_phase);

            // 如果回到0度，切换到下一个模式
            if (phase_step == 0) {
                current_mode = MODE_FREQUENCY;
                my_printf(&huart3, "Switching to FREQUENCY mode\r\n");
            }
            break;

        case MODE_FREQUENCY:
            // 频率调节：从2kHz开始，每次增加100Hz
            #define FREQ_START 2000.0f    // 起始频率2kHz
            #define FREQ_STEP 100.0f      // 频率步进100Hz
            #define FREQ_MAX 10000.0f     // 最大频率10kHz
            #define FREQ_STEPS ((uint8_t)((FREQ_MAX - FREQ_START) / FREQ_STEP + 1))

            freq_step = (freq_step + 1) % FREQ_STEPS;
            float current_freq = FREQ_START + freq_step * FREQ_STEP;
            DA_SetFREQ(0, current_freq);
            DA_Apply_Settings();
            my_printf(&huart3, "DA1 Frequency: %.0f Hz\r\n", current_freq);

            // 如果回到起始频率，切换到下一个模式
            if (freq_step == 0) {
                current_mode = MODE_AMPLITUDE;
                my_printf(&huart3, "Switching to AMPLITUDE mode\r\n");
            }
            break;

        case MODE_AMPLITUDE:
            // 幅度调节：循环预定义的幅度值
            amp_step = (amp_step + 1) % amp_count;
            DA_SetAmp(0, amp_values[amp_step]);
            DA_Apply_Settings();
            my_printf(&huart3, "DA1 Amplitude: %d\r\n", amp_values[amp_step]);

            // 如果回到第一个幅度，切换到下一个模式
            if (amp_step == 0) {
                current_mode = MODE_WAVEFORM;
                my_printf(&huart3, "Switching to WAVEFORM mode\r\n");
            }
            break;

        case MODE_WAVEFORM:
            // 波形调节：循环所有波形类型
            wave_step = (wave_step + 1) % wave_count;
            DA_SetWaveform(0, wave_types[wave_step]);
            DA_Apply_Settings();
            my_printf(&huart3, "DA1 Waveform: %s\r\n", wave_names[wave_step]);

            // 如果回到第一个波形，切换回相位模式
            if (wave_step == 0) {
                current_mode = MODE_PHASE;
                my_printf(&huart3, "Switching to PHASE mode\r\n");
            }
            break;

        default:
            current_mode = MODE_PHASE;
            break;
    }
}

/** @brief 按键4的功能：对AD缓冲区1的数据进行FFT并输出 */
static void key4_action_perform_fft(void)
{
    my_printf(&huart3, "正在计算FFT频谱...\r\n");
    calculate_fft_spectrum(fifo_data1_f, AD_FIFO_SIZE);
    output_fft_spectrum();
}

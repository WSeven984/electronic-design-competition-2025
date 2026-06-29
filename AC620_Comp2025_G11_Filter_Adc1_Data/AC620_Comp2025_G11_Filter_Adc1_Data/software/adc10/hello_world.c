
#include "lcd9341.h"
#include <stdio.h>
#include "system.h"
#include "altera_avalon_pio_regs.h"
#include "sys/alt_irq.h"  //要想编写中断服务程序就必须包含此文件
#include "complex.h"
#include <stdlib.h>
#include <math.h>

#define PLOT_X1 30      // 绘图区域左上角X坐标
#define PLOT_Y1 170     // 绘图区域左上角Y坐标
#define PLOT_X2 220     // 绘图区域右下角X坐标
#define PLOT_Y2 270     // 绘图区域右下角Y坐标
#define PLOT_WIDTH (PLOT_X2 - PLOT_X1)
#define PLOT_HEIGHT (PLOT_Y2 - PLOT_Y1)

#define  PI 3.1415926535898

#define scan_freq_step 200 //扫频率的步进
#define scan_start_freq 1000 //扫频起始频率，1k
#define scan_stop_freq  50000 //扫频起始频率，50k
#define N_Freq ((scan_stop_freq-scan_start_freq)/scan_freq_step+1) //从起始频率扫描到最大频率包含多少个频点。245
#define K_ADC0 9.86  //adc0通道采集的vpp为整数，此整数需要除以256，再乘以此值才能得到准确的电压值，梅雪松不同的acm2108板子的K系数都不同，此值接近10，ccccccccccccccccc
#define K_ADC1 9.86  //adc1通道采集的vpp为整数，此整数需要除以256，再乘以此值才能得到准确的电压值，梅雪松不同的acm2108板子的K系数都不同，此值接近10,cccccccccccccccccc
#define Filter_Order 32 //滤波器的阶数ccccccccccccccccccccccccccccccccccccccccccc
#define Filter_Coef_Width 10 //滤波器系数的位宽

#define fs 50000000L //ad和da原始的采样频率，后面滤波器需要将此采样结果进行降频采样，降后的频率设为200KHz，也就是50k的4倍
#define fs_low 200000 //降频后的采样频率
#define Np_low (fs_low/scan_freq_step) //降频采样后做IDFT变换的点数，200k/200=1000;

#define K_Myself_f1000 1.15   //每个小组需要修改此参数，不同的已知模型电路在1000赫兹时此数的增益基本稳定，cccccccccccccccccccccccccccccccccccccc

float k_myself[4]={0.9,0.89,0.8,0.7};//在question4中，我们的模型电路输出的电压可能与期望的电压有差别，这些系数来校准这些结果，不同频点的系数值不同，需要通过测试进行修改。ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

float am_as_freq[N_Freq];//幅度随着频率编号变化的数组，每个数代表在某个频点的幅度
float filter_coef[Filter_Order];//用于存储滤波器的系数。
float filter_coef_abs_max;//滤波器系数绝对值中的最大值
alt_32 filter_coef_int[Filter_Order];//滤波器系数变成10bit的有符号数。

alt_u32 scan_freq=scan_start_freq;
int n_freq=0;//当前是第几个频点
int if_scaning=0;//表示系统是否在进行question5的扫频操作，=1则正在扫频，=0则不允许扫频，

char lcd_str[50];//在lcd上显示的字符串数组

int i=0;//系统轮回计数器
int j=0;//临时变量
float fai_inc_f;
alt_u32 fai_inc=214748;//nco和的相位累加器控制频率的字。214748能产生1k的正弦波

int n_press=0;    //红外遥控按了多少次，每按一次按键，这个数增加1.
int n_press_old=0;//红外遥控按了多少次的旧次数，只有当新的n_press与旧的n_press_old不相同时才能确定真正被按了
alt_u32 ir_code=0;//红外的状态，从Ir_HT6221_Decode_Irq的ip核的0号地址中读出是8bit的按键编码
alt_u32 freq_inc_step=100;//当ch按钮按下时，freq_inc_step在100，1000，10k，100k之间切换，随后，ch-或者ch+被按下之后，nco输出的正弦波的频率以该步进减少或者增加
alt_u8  question_num=2;//系统处于哪个question的number中，通过按红外遥控中的EQ按钮，能使question_num增加1，当变到6时，再按则返回2.
                       //当系统用来解决基本要求的问题（2）时，question_num=2，解决基本要求的问题（4）时，question_num=4，
                       //解决发挥部分（1）时，question_num=5，等
alt_u32 nco_freq_set=1000; //通过按键、拨码开关、红外等方式设置的令nco核输出的正弦波的频率
float nco_freq_set_float;//将nco_freq_set转成浮点数，以便于调用calculate_input_vpp进行变换。
alt_u32 nco_freq_set_max=2000000;//nco核能够产生的最大的正弦波的频率为2MHz。

alt_u8 mei_sw_state;  //直接获取梅雪松的sw的状态
alt_u8 sw_quest_num_new;//梅雪松扩展板上的拨码开关的低三位新状态，控制question_num
alt_u8 sw_quest_num_old;//梅雪松扩展板上的拨码开关的低三位旧状态，控制question_num
alt_u8 exp10_old;       //梅雪松扩展板上的拨码开关的第3至5位，代表nco频率步进为10的多少次方，旧的状态。
alt_u8 exp10_new;       //梅雪松扩展板上的拨码开关的第3至5位，代表nco频率步进为10的多少次方，新的状态。
alt_u8 btn_new;//板载按键新状态，btn1表示某个变量增加，btn0表示某个变量减少
alt_u8 btn_old;//板载按键旧状态
alt_u8 btn0_new;//btn0的新状态，每按一下，则nco_freq_set减少step
alt_u8 btn0_old;//btn0的旧状态
alt_u8 btn1_new;//btn1的新状态，每按一下，则nco_freq_set增加step
alt_u8 btn1_old;
alt_u8 vpp_adc1_int;//通过pio读取的adc1采集的模型电路输出的电压值的整数，该整数为无符号正整数。
float vpp_adc1_f;//adc1采集的电压转换成浮点数。
alt_u8 vpp_adc0_int;//通过pio读取的adc0采集的dac0输出的电压值的整数，该整数为无符号正整数。
float vpp_adc0_f;//adc0采集的电压转换成浮点数。

float desired_volt=2;//问题3、4中我们期望模型电路输出的电压
alt_u8 adj_volt_new=0;//用来调节上述desired_volt的电压，每按一次sw7则desired_volt增加0.1伏特，达到2v时再返回1伏特。
alt_u8 adj_volt_old=0;//旧的sw7的状态
float dac0_volt_by_hs;//由desired_volt根据hs反算出的dac0应该输出的电压。
float mul_float;//调节nco输出电压的pio对应的mul值的浮点数
alt_u8 mul;//整数的mul由pio输出，再用其乘以nco输出的正弦波的数字序列，得到16bit的序列，再取其高8位送给DA0_Data

alt_u8 filter_type;//判断滤波器类型，0为低通，1为高通，2为带通，3为带阻，4为其他
float cutoff_freq = 0;
float center_freq = 0;
float bandwidth = 0;

float am_as_freq_fill[1000];//由低频的幅频特性填充出来的全频段的幅频特性，共1000个点
float angle_step;
float angle_real[1000];//每个幅频特性数据点需要乘以一个复数的旋转因子
float angle_imag[1000];
float sinp[1000];//傅里叶变换的系数，正弦
float cosp[1000];//傅里叶变换的系数，余弦

void ir_sw_btn_handle(void);//红外遥控，梅雪松的按键以及板载btn控制系统一些变量的函数
void lcd_show_str_pic(void);//lcd显示字符串以及幅频特性曲线图
void clear_lcd_str(void);//清除lcd字符串缓存。
void Ir_Irq_Handler(void);//红外的中断服务程序
float calculate_input_vpp(float f, float vppout);
void Give_Dac0_Vpp(float vpp); //这个函数能够通过调节mul给dac0一个确定vpp的电压值，这个电压值经过adc0进行采样检测，与理想vpp比较后修改mul从而实现了对dac0输出电压进行调整。
void identify_filter_type(void);
void Get_Filter_Coef(void);//根据am_as_freq[]幅频特性计算滤波器系数的数组filter_coef[]
int main()
{
	int jj=0;//临时可用的计数器
	float k1;//question4中校准系数，从k_myself[]数组中选择某一个
 	LCD9341_Init();
	POINT_COLOR=GREEN;
	BACK_COLOR=BLACK;
	LCD_Clear(BLACK);
	alt_irq_register(IR_HT6221_DECODE_IRQ_0_IRQ, NULL, Ir_Irq_Handler);//注册中断服务程序

	IOWR_ALTERA_AVALON_PIO_DATA(PIO6_MUL_BASE, 100);//向mul的pio中写入乘法器的倍率，倍率为1到127之间的整数。
	angle_step = -PI/1000*2;//最基本的旋转因子
	for(i=0;i<1000;i++)
	{
		angle_real[i] = cos(angle_step*i);
		angle_imag[i] = sin(angle_step*i);
	}

	for(i=0;i<1000;i++) //用于计算离散傅里叶反变换，旋转因子
	{
		sinp[i]=sin(-angle_step*i);
		cosp[i]=cos(-angle_step*i);
	}

	i=0;
  	while(1)
	{
  		i++;
  		printf("i = %d\n",i);//在工作台上打印while(1)的轮回数
  		ir_sw_btn_handle();//红外遥控，梅雪松的按键以及板载btn控制系统一些变量的函数
  		lcd_show_str_pic();//lcd显示字符串以及幅频特性曲线图

  		//---------------- Question number=0 用来手动校准adc1和adc0的系数K_ADC1与K_ADC0 --------------//
  		//---------------- 先用函数发生器生成有一个vpp等于已知值x的正弦电压，我们用默认的 K_ADC1得到adc1采集的电压，这个电压不等于x，则通过调整 K_ADC1令其等于x--------------//
		if(question_num==0 || question_num==1)
		{
			IOWR_ALTERA_AVALON_PIO_DATA(PIO7_FILTER_WORK_BASE, 0);//不允许滤波器对adc1采集的数据进行滤波

  			IOWR_ALTERA_AVALON_PIO_DATA(PIO6_MUL_BASE, 100);//向mul的pio中写入乘法器的倍率，让dac0通道输出7点几伏的电压，127对应10伏，ccccccccccccc

			fai_inc_f=4*1024*1024*1024.0/fs*nco_freq_set;//由设置的频率计算dds的相位累加器的fai_inc
			fai_inc=(alt_u32)fai_inc_f;
			IOWR_ALTERA_AVALON_PIO_DATA(PIO0_INC_BASE, fai_inc);//让nco输出设定频率的正弦波，前面可以通过拨码开关以及按钮来调节nco_freq_set的值。

			vpp_adc1_int = IORD_ALTERA_AVALON_PIO_DATA(PIO4_VPP_ADC1_BASE);//获取adc1的采集数据,从而得到vpp
			vpp_adc1_f = vpp_adc1_int/256.0*K_ADC1;//vpp转成浮点数,K_ADC1在前面宏定义中修改ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
			printf("vpp_adc1_f %f \n",vpp_adc1_f);//在工作台上打印adc1采集的电压，

			vpp_adc0_int = IORD_ALTERA_AVALON_PIO_DATA(PIO5_VPP_ADC0_BASE);//获取adc1的采集数据,从而得到vpp
			vpp_adc0_f = vpp_adc0_int/256.0*K_ADC0;//vpp转成浮点数,K_ADC1在前面宏定义中修改ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
			printf("vpp_adc0_f %f \n",vpp_adc0_f);//在工作台上打印adc1采集的电压，

		}

  		//---------------- Question number=2时可输出任意频率的正弦波的扫频过程 --------------//
  		if(question_num==2)
  		{
  			IOWR_ALTERA_AVALON_PIO_DATA(PIO7_FILTER_WORK_BASE, 0);//不允许滤波器对adc1采集的数据进行滤波
  			IOWR_ALTERA_AVALON_PIO_DATA(PIO6_MUL_BASE, 100);//向mul的pio中写入乘法器的倍率，让dac0通道输出7.4几伏的电压，127对应10伏，ccccccccccccccccccccccccccccc
  			//Give_Dac0_Vpp(5.0);//给dac0输出稳定的2伏特的电压，这个电压经过了adc0的测试。ccccccccccccccccccccccccccccccccccccccccccccc

			fai_inc_f=4*1024*1024*1024.0/fs*nco_freq_set;//由设置的频率计算dds的相位累加器的fai_inc
			fai_inc=(alt_u32)fai_inc_f;
			IOWR_ALTERA_AVALON_PIO_DATA(PIO0_INC_BASE, fai_inc);//让nco输出设定频率的正弦波，前面可以通过拨码开关以及按钮来调节nco_freq_set的值。
  		}

  		//---------------- Question number=3或者4时，我们的信号发生器的频率和幅度都可以设置成任意想要的结果，给定模型电路的输出，反算出信号发生器需要的幅度 --------------//
		if(question_num==3)
		{
			IOWR_ALTERA_AVALON_PIO_DATA(PIO7_FILTER_WORK_BASE, 0);//不允许滤波器对adc1采集的数据进行滤波
			fai_inc_f=4*1024*1024*1024.0/fs*nco_freq_set;//由设置的频率计算dds的相位累加器的fai_inc
			fai_inc=(alt_u32)fai_inc_f;
			IOWR_ALTERA_AVALON_PIO_DATA(PIO0_INC_BASE, fai_inc);//让nco输出设定频率的正弦波，前面可以通过拨码开关以及按钮来调节nco_freq_set的值。

			//根据desired_volt反推出dac0需要产生的电压，
			//dac0_volt_by_hs = desired_volt*0.8;//这里由专家指定的desired_volt电压根据hs的表达式反算出来的dac0应该输出的电压。
			nco_freq_set_float = (float)nco_freq_set;//由于下面的函数需要nco_freq_set为浮点数，这里将其装换成浮点数
			dac0_volt_by_hs = calculate_input_vpp(nco_freq_set_float, desired_volt);//程子健写的函数，由H(s)反算出来的我们的模型电路应该输入的电压，或者说dac0应该产生的电压。
			printf("dac0_volt_by_hs = %f\n",dac0_volt_by_hs);//

			mul_float=12.7*dac0_volt_by_hs*K_Myself_f1000;//dac0产生的电压要想可调节，则mul应该乘以127/10=12.7，K_Myself_f1000用于修正，每组的参数各不相同，ccccccccccccccccccccccccccccccc
			mul=(alt_u8)mul_float;//mul必须为8位的有符号整数，且为正数，这样mul在0到127之间。
			IOWR_ALTERA_AVALON_PIO_DATA(PIO6_MUL_BASE, mul);//向mul的pio中写入乘法器的倍率

			//上述三句话可以改成如下的一句话
			//Give_Dac0_Vpp(dac0_volt_by_hs*K_Myself_f1000);//通过adc0的采集确定dac0输出的就是这个理论的的电压ccccccccccccccccccccccccccLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
		}

  		//---------------- Question number=4时，我们的信号发生器的频率和幅度都可以设置成任意想要的结果，给定模型电路的输出，反算出信号发生器需要的幅度 --------------//
		if(question_num==4)
		{
			IOWR_ALTERA_AVALON_PIO_DATA(PIO7_FILTER_WORK_BASE, 0);//不允许滤波器对adc1采集的数据进行滤波
			fai_inc_f=4*1024*1024*1024.0/fs*nco_freq_set;//由设置的频率计算dds的相位累加器的fai_inc
			fai_inc=(alt_u32)fai_inc_f;
			IOWR_ALTERA_AVALON_PIO_DATA(PIO0_INC_BASE, fai_inc);//让nco输出设定频率的正弦波，前面可以通过拨码开关以及按钮来调节nco_freq_set的值。

			//根据desired_volt反推出dac0需要产生的电压，
			//dac0_volt_by_hs = desired_volt*0.8;//这里由专家指定的desired_volt电压根据hs的表达式反算出来的dac0应该输出的电压。
			nco_freq_set_float = (float)nco_freq_set;//由于下面的函数需要nco_freq_set为浮点数，这里将其装换成浮点数
			dac0_volt_by_hs = calculate_input_vpp(nco_freq_set_float, desired_volt);//程子健写的函数，由H(s)反算出来的我们的模型电路应该输入的电压，或者说dac0应该产生的电压。
			printf("dac0_volt_by_hs = %f\n",dac0_volt_by_hs);//

//			if(nco_freq_set<2000)//ccccccccccccccccccccccccccccccccccccccccccccccccccccc
//				k1=k_myself[2];//cccccccccccccccccccccccccccccccccccccccccccccccccccccccc
//
//			if(nco_freq_set>2000)//这里写程序选择从哪个k中选择 ，cccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
//				k1=k_myself[3];//cccccccccccccccccccccccccccccccccccccccccccccccc

			k1=K_Myself_f1000;//如果你的模型电路非常好，在任何频点的幅频响应与Hs理想表达式的误差小于5%，则这里可以直接使用K_Myself_f1000

			mul_float=12.7*dac0_volt_by_hs*k1;//从k_myself[3]系数数组中挑选一个作为当前的系数，ccccccccccccccccccccccccccccccc
			mul=(alt_u8)mul_float;
			IOWR_ALTERA_AVALON_PIO_DATA(PIO6_MUL_BASE, mul);//向mul的pio中写入乘法器的倍率
			//上述三句话可以改成如下的一句话
			//Give_Dac0_Vpp(dac0_volt_by_hs*k1);//通过adc0的采集确定dac0输出的就是这个理论的的电压cccccccccccccccccccccccccccccccccccccccLLLLLLLLLLLLLLLLLLLLLLLLLLL
		}

  		//---------------- Question5的扫频过程 --------------//
  		if(question_num==5 && if_scaning==1)
  		{
  			IOWR_ALTERA_AVALON_PIO_DATA(PIO7_FILTER_WORK_BASE, 0);//不允许滤波器对adc1采集的数据进行滤波
  			Give_Dac0_Vpp(2.0);//给dac0输出稳定的2伏特的电压，这个电压经过了adc0的测试。cccccccccccccccccccccccccccccccccccccccccccccccc
  			//IOWR_ALTERA_AVALON_PIO_DATA(PIO6_MUL_BASE, 200);//向mul的pio中写入乘法器的倍率,ccccccccccccccccccccccccccccccccccccccccccc
			scan_freq=scan_start_freq;//从起始频率1000Hz开始扫
			for(n_freq=0;n_freq<N_Freq;n_freq++)
			{
				fai_inc_f=4*1024*1024*1024.0/fs*scan_freq;//由设置的频率计算dds的相位累加器的fai_inc
				fai_inc=(alt_u32)fai_inc_f;
				IOWR_ALTERA_AVALON_PIO_DATA(PIO0_INC_BASE, fai_inc);//让nco输出设定频率的正弦波，前面可以通过拨码开关以及按钮来调节nco_freq_set的值。
				usleep(50000);//延时0.05秒
				vpp_adc1_int = IORD_ALTERA_AVALON_PIO_DATA(PIO4_VPP_ADC1_BASE);//获取adc1的状态,从而得到vpp
				vpp_adc1_f = vpp_adc1_int/256.0*K_ADC1;//vpp转成浮点数,K_ADC1在前面宏定义中修改ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
				am_as_freq[n_freq]=vpp_adc1_f;//当前电压的vpp浮点数存入幅频特性数组
				printf("am_as_freq %d = %f\n",n_freq,am_as_freq[n_freq]);//在工作台上打印sw状态
				scan_freq = scan_freq + scan_freq_step;//改变扫描的频率
			}
			if_scaning=0;//扫描结束则令扫描的标志位等于0，只有btn0被再次按下才能令if_scaning=1，才能再次进行扫描。

			scan_freq=scan_start_freq;//再令函数dac0通道输出的正弦拨的频率等于起始频率，1kHz。
			fai_inc_f=4*1024*1024*1024.0/fs*scan_freq;//由设置的频率计算dds的相位累加器的fai_inc
			fai_inc=(alt_u32)fai_inc_f;
			IOWR_ALTERA_AVALON_PIO_DATA(PIO0_INC_BASE, fai_inc);//让nco输出设定频率的正弦波，前面可以通过拨码开关以及按钮来调节nco_freq_set的值。

			Draw_AmplitudeFrequency_Curve();//在lcd上画幅频特性曲线。
			identify_filter_type();//判断滤波器种类，
			//-------------------在lcd上显示滤波器种类---------------------------//
            clear_lcd_str();
            char line1[50], line2[50], line3[50]; // 定义三行字符串缓冲区

            switch(filter_type)
            {
                case 0: // 低通滤波器
                    snprintf(line1, sizeof(line1), "Filter: Lowpass");
                    snprintf(line2, sizeof(line2), "Cutoff: %.1f Hz", cutoff_freq);
                    line3[0] = '\0'; // 第三行为空
                    break;

                case 1: // 高通滤波器
                    snprintf(line1, sizeof(line1), "Filter: Highpass");
                    snprintf(line2, sizeof(line2), "Cutoff: %.1f Hz", cutoff_freq);
                    line3[0] = '\0';
                    break;

                case 2: // 带通滤波器
                    snprintf(line1, sizeof(line1), "Filter: Bandpass");
                    snprintf(line2, sizeof(line2), "Center: %.1f Hz", center_freq);
                    snprintf(line3, sizeof(line3), "BW: %.1f Hz", bandwidth);
                    break;

                case 3: // 带阻滤波器
                    snprintf(line1, sizeof(line1), "Filter: Bandstop");
                    snprintf(line2, sizeof(line2), "Center: %.1f Hz", center_freq);
                    snprintf(line3, sizeof(line3), "BW: %.1f Hz", bandwidth);
                    break;

                default: // 未知滤波器
                    snprintf(line1, sizeof(line1), "Filter: Unknown");
                    line2[0] = '\0';
                    line3[0] = '\0';
            }
            // 分三行显示
            LCD_ShowString(10, 80, line1);  // 第一行（类型）
            LCD_ShowString(10, 100, line2); // 第二行（频率参数）
            if (line3[0] != '\0')  // 第三行（带宽参数)
            {
                LCD_ShowString(10, 120, line3);
            }
  		}
  		//---------------- Question6，根据幅频特性曲线数组设计滤波器系数 filter_coef[32]为浮点数系数数组，filter_coef_int[32]为10bit的数组，滤波器为32阶--------------//
		if(question_num==6)
		{
			Get_Filter_Coef();//根据am_as_freq[]幅频特性计算滤波器系数的数组filter_coef_int[32]
			//向coef中写入滤波器系数
			for(jj=0;jj<32;jj++)
			{
				IOWR_ALTERA_AVALON_PIO_DATA(PIO8_FILTER_ADDR_BASE, jj);//写入coef存储器的地址
				IOWR_ALTERA_AVALON_PIO_DATA(PIO9_FILTER_DATA_BASE, filter_coef_int[jj]);//向第jj号coef存储器中写数
				usleep(1);
				IOWR_ALTERA_AVALON_PIO_DATA(PIO10_FILTER_WRITE_BASE, 1);//允许给第jj号存储器写数据
				usleep(1);
				IOWR_ALTERA_AVALON_PIO_DATA(PIO10_FILTER_WRITE_BASE, 0);//允许给第jj号存储器写数据

			}
			IOWR_ALTERA_AVALON_PIO_DATA(PIO7_FILTER_WORK_BASE, 1);//允许滤波器对adc1采集的数据进行滤波，并把数据从dac0中输出，如果该pio输出0则，dac0输出的是原来问题1~5中的数据。
		}

		usleep(100000);//延时0.2秒，这个延迟时间不能太长，最好不要高于0.3秒，太长的话会导致按键、拨码开关反应的速度减慢。
	}
}

//红外的中断服务程序
void Ir_Irq_Handler(void)
{
	n_press++;//每按一次按键，这个数增加1.
	usleep(1);//这里必须延迟一段时间才能使Ir_HT6221_Decode_Irq的ip核的寄存器的数稳定下来
	IOWR_32DIRECT(IR_HT6221_DECODE_IRQ_0_BASE, 0, 0);//清除中断标志，以便于可以执行下次中断
	ir_code=IORD_32DIRECT(IR_HT6221_DECODE_IRQ_0_BASE, 0);//读Ir_HT6221_Decode_Irq的ip核的0号寄存器，里面存的是8bit的按键状态
}

void clear_lcd_str(void)//清除lcd字符串缓存。
{
	int i=0;
	for(i=0;i<50;i++)
	{
		lcd_str[i]=0;
	}
}

// 用于复数运算(double complex, I, cabs)
float calculate_input_vpp(float f, float vppout)
{
    // 1. 计算复频率 s = j * 2 * pi * f
	float complex s = I * 2.0 * PI * f;

    // 2. 计算传递函数 H(s)
    //    H(s) = 5 / (10^-8 * s^2 + 3*10^-4 * s + 1)
	float complex Hf = 5.0 / (1.0e-8 * s * s + 3.0e-4 * s + 1.0);

    // 3. 计算电压增益 Av = |H(s)|
	float Av = cabs(Hf);
    // 4. 反向计算输入峰峰值 vppin = vppout / Av
	float vppin = vppout / Av;

    return vppin;
}

void ir_sw_btn_handle(void)//红外遥控，梅雪松的按键以及板载btn控制系统一些变量的函数
{
	if(n_press != n_press_old) //只有当新的n_press与旧的n_press_old不相同时，才确定红外遥控干什么事情
	{
		//根据红外按键每次按下的编码决定做什么事情
		if(ir_code==9) //如果EQ按键被按下，则令question_num增加1，变成6之后再变成2，表示后续的程序在处理哪个问题。
		{
			question_num++;
			if(question_num>6)
				question_num=0;
		}

		if(ir_code==70) //如果ch按钮被按下，则改变频率的步进值
		{
			freq_inc_step=10*freq_inc_step;//每按一次ch按钮，则频率步进值freq_inc_step增加十倍，达到100k的时候再回到100Hz
			if(freq_inc_step>1000000) //当频率步进大于1M时，再变成100Hz
				freq_inc_step=100;
		}

		if(ir_code==69) //如果ch-按键被按下，则令nco核输出的正弦波的频率nco_freq_set减少freq_inc_step，少于100时，则自动变成100Hz
		{
			nco_freq_set=nco_freq_set-freq_inc_step;
			if(nco_freq_set<100)
				nco_freq_set=100;
		}

		if(ir_code==71) //如果ch+按键被按下，则令nco核输出的正弦波的频率nco_freq_set增加freq_inc_step，高于最大频率nco_freq_set_max时，则自动变成nco_freq_set_max
		{
			nco_freq_set=nco_freq_set+freq_inc_step;
			if(nco_freq_set>nco_freq_set_max)
				nco_freq_set=nco_freq_set_max;
		}

		if(ir_code==21)//如果+被按下，则desired_volt增加0.1，从1增加到2之后再返回1
		{
			desired_volt=desired_volt+0.1;
			if(desired_volt>=2.0)
				desired_volt=2.0;
		}

		if(ir_code==7)//如果-被按下，则desired_volt减少0.1，从1增加到2之后再返回1
		{
			desired_volt=desired_volt-0.1;
			if(desired_volt<=1.0)
				desired_volt=1.0;
		}
	}
	n_press_old=n_press;//旧的按键次数等于现在的按键次数

	//--------------梅雪松的扩展板的八个拨码开关sw、led以及母板上的按键btn[1:0]的功能，它与红外遥控能够平等第修改nco_freq_set,step等变量-------------------//
	//--------------sw[2:0]代表question number，sw[5:3]=exp10代表nco_freq_set_step=10^exp10,btn[0]减少频率，btn1增加nco_freq_set---------------------------//
	mei_sw_state = IORD_ALTERA_AVALON_PIO_DATA(PIO1_SW_BASE);//获取梅雪松的拨码开关的状态

	sw_quest_num_new = mei_sw_state & 0x7;//---------获取梅雪松的sw的低3位,代表question number的第几种。
	if(sw_quest_num_new != sw_quest_num_old)
	{
		question_num = sw_quest_num_new;
	}
	sw_quest_num_old = sw_quest_num_new;

	exp10_new = (mei_sw_state>>3) & 0x7;  //---------获取梅雪松的sw的第3~5位,代表10的多少次方产生nco_freq_set_step
	if(exp10_new != exp10_old)
	{
		freq_inc_step=1;
		for(j=0;j<exp10_new;j++)
		{
			freq_inc_step = freq_inc_step*10;//根据exp10_new计算频率步进值nco_freq_set_step
		}
	}
	exp10_old = exp10_new;

	adj_volt_new = mei_sw_state>>7;
	if(adj_volt_new != adj_volt_old)
	{
		desired_volt=desired_volt+0.1;
		if(desired_volt>2.0)
			desired_volt=1.0;
	}
	adj_volt_old = adj_volt_new;

	btn_new = IORD_ALTERA_AVALON_PIO_DATA(PIO3_BTN_BASE);//-----------获取按键的状态
	btn0_new=btn_new & 0x1;
	btn1_new=(btn_new>>1) & 0x1;
	if(btn0_new == 0 &&  btn0_old ==1) //btn0按下，则nco输出的正弦波的频率要减少step
	{
		nco_freq_set = nco_freq_set-freq_inc_step;
		if(nco_freq_set<100)
			nco_freq_set=100;
		if(question_num==5) //如果解决问题5，要进行扫描
		{
			if_scaning=1;
		}
	}
	btn0_old = btn0_new;

	if(btn1_new == 0 &&  btn1_old ==1) //btn1按下，则nco输出的正弦波的频率要增加step
	{
		nco_freq_set=nco_freq_set+freq_inc_step;
		if(nco_freq_set>nco_freq_set_max)
			nco_freq_set=nco_freq_set_max;
	}
	btn1_old = btn1_new;

	IOWR_ALTERA_AVALON_PIO_DATA(PIO2_LED_BASE, question_num);//某个拨码开关拨上去则对应的led灯会亮。

	//printf("ir_code = %d\n",ir_code);//在工作台上打印出来
	//printf("btn_new = %d\n",btn_new);//在工作台上打印sw状态
	//printf("mei_sw_state = %d\n",mei_sw_state);//在工作台上打印sw状态
}

void lcd_show_str_pic(void)//lcd显示字符串以及幅频特性曲线图
{
	//----------------在lcd上显示一些参数-----------//
	clear_lcd_str();//清除lcd9341显示的字符串缓存
	sprintf(lcd_str,"ir_code=%d,QN=%d ",ir_code,question_num);//显示红外按键的编码以及question number，QN=question number
	LCD_ShowString(10,20,lcd_str);//显示红外按键的编码

	clear_lcd_str();//清除lcd9341显示的字符串缓存
	sprintf(lcd_str,"f_step=%d,f_set=%d",freq_inc_step,nco_freq_set);//将freq_inc_step写进字符串中
	LCD_Fill(130,40,240,60,BLACK);//用黑色填充整个一行
	LCD_ShowString(10,40,lcd_str);//lcd上显示生成的正弦波的频率的步进。

	clear_lcd_str();//清除lcd9341显示的字符串缓存
	sprintf(lcd_str,"U_hope=%2.1f,U_hs=%2.1f",desired_volt,dac0_volt_by_hs);//将question_num写进字符串中
	LCD_ShowString(10,60,lcd_str);//lcd上显示问题编号，dac0_volt_by_hs
}

void Give_Dac0_Vpp(float vpp) //这个函数能够通过调节mul给dac0一个确定vpp的电压值，这个电压值是通过adc0进行检测了的。
{
	int n=0;
	//先让dac0输出一个可能有误差的vpp
	mul_float=12.7*vpp;//dac0产生的电压要想可调节，则mul应该乘以255/10=25.5
	mul=(alt_u8)mul_float;
	IOWR_ALTERA_AVALON_PIO_DATA(PIO6_MUL_BASE, mul);//向mul的pio中写入乘法器的倍率
	usleep(50000);//延时0.05秒,让电压稳定
	vpp_adc0_int = IORD_ALTERA_AVALON_PIO_DATA(PIO5_VPP_ADC0_BASE);//获取adc0采集的dac0输出的vpp
	vpp_adc0_f = vpp_adc0_int/256.0*K_ADC0;//vpp转成浮点数,在前面宏定义中修改ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

	n=0;
	while(abs(vpp_adc0_f-vpp)<vpp*0.05 && n<10)//如果测试的电压与理想的电压之间的差小于0.05，并且循环次数不超过10次，则继续调整mul的值
	{
		n++;
		if(vpp_adc0_f > vpp) //如果dac0输出的电压采集结果比期望结果大，则要降低mul
		{
			mul--;
		}
		else //否则要增加mul
		{
			mul++;
		}
		IOWR_ALTERA_AVALON_PIO_DATA(PIO6_MUL_BASE, mul);//向mul的pio中写入乘法器的倍率
		usleep(50000);//延时0.05秒,让电压稳定
		vpp_adc0_int = IORD_ALTERA_AVALON_PIO_DATA(PIO5_VPP_ADC0_BASE);//获取adc0采集的dac0输出的vpp
		vpp_adc0_f = vpp_adc0_int/256.0*K_ADC0;//vpp转成浮点数,在前面宏定义中修改ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
	}
	if(n<10)
	{
		printf("Give Dac0 volt OK \n");
	}
	else
	{
		printf("Give Dac0 volt Falt \n");
	}
}

// 在LCD上绘制幅频特性曲线
void Draw_AmplitudeFrequency_Curve(void)
{
    // 1. 清空绘图区域
    LCD_Fill(PLOT_X1, PLOT_Y1, PLOT_X2, PLOT_Y2, BLACK);

    // 2. 绘制坐标轴
    POINT_COLOR = WHITE;
    // 横轴 (频率)
    LCD_DrawLine(PLOT_X1, PLOT_Y2, PLOT_X2, PLOT_Y2);
    // 纵轴 (幅度)
    LCD_DrawLine(PLOT_X1, PLOT_Y1, PLOT_X1, PLOT_Y2);

    // 3. 计算幅度最大值（归一化用）
    float max_amp = 0.1;
    for (int k = 0; k < N_Freq; k++)
    {
        if (am_as_freq[k] > max_amp) max_amp = am_as_freq[k];
    }
    if (max_amp < 0.1f) max_amp = 1.0f; // 防止除零

    // 4. 绘制曲线
    int prev_x = -1, prev_y = -1;
    for (int k = 0; k < N_Freq; k++)
    {
        // 计算当前点在屏幕上的坐标
        int x = PLOT_X1 + (int)((float)k / (N_Freq-1) * PLOT_WIDTH);
        float norm_amp = am_as_freq[k] / max_amp;
        norm_amp = (norm_amp > 1.0f) ? 1.0f : norm_amp; // 限制在[0,1]
        int y = PLOT_Y2 - (int)((am_as_freq[k] / max_amp) * PLOT_HEIGHT);

        if (y < PLOT_Y1) y = PLOT_Y1;
        if (y > PLOT_Y2) y = PLOT_Y2;
        // 绘制数据点（小圆点）
        LCD_DrawPoint(x, y);

        // 连接前一个点（从第二个点开始）
        if (prev_x != -1 && prev_y != -1)
        {
            LCD_DrawLine(prev_x, prev_y, x, y);
        }

        prev_x = x;
        prev_y = y;
    }

    //5. 添加坐标轴标签
      char label[20];
	// 横轴标签（频率）
	sprintf(label, "%dkHz", scan_start_freq/1000);
	LCD_ShowString(PLOT_X1, PLOT_Y2 + 5, label);
	sprintf(label, "%dkHz", scan_stop_freq/1000);
	LCD_ShowString(PLOT_X2, PLOT_Y2 + 5, label);

	// 纵轴标签（幅度）
	sprintf(label, "max:%.1fV", max_amp);
	LCD_ShowString(PLOT_X1- 20, PLOT_Y1 - 20, label);
	LCD_ShowString(PLOT_X1 -20, PLOT_Y2, "0V");
}
void identify_filter_type(void)
{
    int i;
    float max_gain = 0;
    float min_gain = 9999;
    int max_index = 0;
    int min_index = 0;
    float db_gain[N_Freq]; // 增益值(dB)

    // 计算增益(dB)并找到最大值
    for(i = 0; i < N_Freq; i++)
    {
        if(am_as_freq[i] <= 0) am_as_freq[i] = 0.0001; // 防止对数计算错误
        db_gain[i] = 20 * log10(am_as_freq[i]);

        if(db_gain[i] > max_gain)
        {
            max_gain = db_gain[i];
            max_index = i;
        }
        if(db_gain[i] < min_gain)
        {
            min_gain = db_gain[i];
            min_index = i;
        }
    }

    // 计算-3dB点
    float threshold_dB = max_gain - 3;
    int low_cutoff = -1;
    int high_cutoff = -1;

    // 寻找低端截止频率
    for(i = 0; i < max_index; i++)
    {
        if(db_gain[i] >= threshold_dB)
        {
            low_cutoff = i;
            break;
        }
    }

    // 寻找高端截止频率
    for(i = max_index; i < N_Freq; i++)
    {
        if(db_gain[i] <= threshold_dB)
        {
            high_cutoff = i;
            break;
        }
    }

    // 判断滤波器类型
    if(low_cutoff == -1 && high_cutoff != -1)
    {
        // 没有低端截止点，只有高端截止点 → 低通滤波器
        filter_type = 0;
        cutoff_freq = scan_start_freq + high_cutoff * scan_freq_step;
    }
    else if(low_cutoff != -1 && high_cutoff == -1)
    {
        // 有低端截止点，没有高端截止点 → 高通滤波器
        filter_type = 1;
        cutoff_freq = scan_start_freq + low_cutoff * scan_freq_step;
    }
    else if(low_cutoff != -1 && high_cutoff != -1 &&
             max_index > low_cutoff && max_index < high_cutoff)
    {
        // 有低高截止点，峰值在中间 → 带通滤波器
        filter_type = 2;
        center_freq = scan_start_freq + max_index * scan_freq_step;
        bandwidth = (high_cutoff - low_cutoff) * scan_freq_step;
    }
    else if(min_index > low_cutoff && min_index < high_cutoff)
    {
        // 有凹陷点 → 带阻滤波器
        filter_type = 3;
        center_freq = scan_start_freq + min_index * scan_freq_step;
        bandwidth = (high_cutoff - low_cutoff) * scan_freq_step;
    }
    else
    {
        filter_type = 4;
    }
}
void Get_Filter_Coef(void)//根据am_as_freq[]幅频特性计算滤波器系数的数组filter_coef[]
{
	float abs_val;//求绝对值
	float coef_10bit_float;//把每个系数归一化后乘以510，由于是10bit的整数。
	int ii=0;
	int n;//对采样点时域进行遍历
	int k;//对谱线的频域进行遍历
	int kn;//k乘以n再对N求余数得到的结果
	float xn_real=0;//第n个滤波器系数的实部
	float xn_imag=0;//第n个滤波器系数的实部
	float am_as_freq_fill_real[1000];//1000个乘以了旋转因子的频域数据的实部
	float am_as_freq_fill_imag[1000];//1000个乘以了旋转因子的频域数据的虚部
	//第一步：把am_as_freq[]中的245个数根据频谱的对称性拼接成1000个点。Np_low=1000          FFFFFFFFDD
	for(ii=0;ii<500;ii++)
	{
		if(ii==0)
		{
			am_as_freq_fill[0]=0;
		}
		if(ii>0 && ii<5)
		{
			am_as_freq_fill[ii]=am_as_freq[0];
		}
		if(ii>=5 && ii<250)
		{
			am_as_freq_fill[ii]=am_as_freq[ii-5];
		}
		if(ii>=250 && ii<500)
		{
			am_as_freq_fill[ii]=0;//中间的500个点赋0
		}
		if(ii != 0)
		{
			am_as_freq_fill[1000-ii]=am_as_freq_fill[ii];
		}
	}
	am_as_freq_fill[500]=0;
	//第二步，提前已经计算好了每个频点的旋转因子，包含实部和虚部两个序列，angle_real[1000],angle_imag[1000]
	//再将am_as_freq_fill[]数组与旋转因子相乘得到复数频域数据的实部与虚部序列
	for(ii=0;ii<1000;ii++)
	{
		am_as_freq_fill_real[ii]=am_as_freq_fill[ii]*angle_real[ii];
		am_as_freq_fill_imag[ii]=am_as_freq_fill[ii]*angle_imag[ii];
	}
	//第三步：对上述夫序列进行傅里叶反变换操作,只求idft的前32个点，就得到滤波器系数filter_coef[32]
	for(n=0;n<Filter_Order;n++)
	{
		xn_real=0;
		xn_imag=0;
		for(k=0;k<1000;k++)
		{
			kn=(k*n)%1000;//余数相位因子
			xn_real = xn_real + am_as_freq_fill_real[k]*cosp[kn]-am_as_freq_fill_imag[k]*sinp[kn];
			xn_imag = xn_imag + am_as_freq_fill_real[k]*sinp[kn]+am_as_freq_fill_imag[k]*cosp[kn];
		}
		filter_coef[n]=xn_real;
//		printf("xn_real=%f,xn_imag=%f \n",xn_real,xn_imag);
	}
	//第四步：对32个系数归一化，得到指定位宽的整数系数，这里位宽设定成10位
	filter_coef_abs_max=0;
	for(n =0; n<32; n++)//求系数的绝对值的最大值
	{
		abs_val=fabs(filter_coef[n]);//得到系数的绝对值
		if(abs_val>filter_coef_abs_max)
		{
			filter_coef_abs_max=abs_val;
		}
	}
	for(n =0; n<32; n++)//求系数的绝对值的最大值
	{

		coef_10bit_float = filter_coef[n]/filter_coef_abs_max*510;//cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
		filter_coef_int[n] = (alt_32)coef_10bit_float;
		printf("filter_coef_int=%d \n",filter_coef_int[n]);
	}
}








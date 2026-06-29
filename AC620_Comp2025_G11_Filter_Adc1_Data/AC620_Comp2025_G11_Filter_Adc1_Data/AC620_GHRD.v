module AC620_GHRD(

	//output reg signed [10+8-1+2-1+3:0] filter_out_prime,//[21:0],共22bit,滤波器原始输出的数据，wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
	//output wire unsigned [7:0] filter_out_cut_unsigned,
	//output wire signed [7:0] filter_out_cut,

	output beep,
	output wire clk200k,//由50m的时钟分频出来的200k的时钟
	
	input [1:0]   btn,     //ac620板子上固有的按钮s0和s1.
	input [7:0]   mei_sw,  //梅雪松的扩展板上的拨码开关
	output [7:0]  mei_led, //梅雪松的扩展板上的led灯

	//ADC 芯片MS9280,可由30M超频到50M
	input [7:0]AD0, AD1,//无符号数
	output AD0_CLK, AD1_CLK,

	
	//125Msps转换速率的高速DAC芯片MS9708（完全兼容AD9708）
	output [7:0] DA0_Data, DA1_Data,
	output DA0_Clk,DA1_Clk,
	


	input  wire        clk,                         //50m
	input  wire        reset_n,                     //                      reset.reset_n
	
	input ir_in,     //红外输入接口
	
	output wire        lcd_rst,                    //                    lcd_rst.export
	output wire        lcd_rd_n,                     //                     lcd_rd.export
	output wire        lcd_bl,                     //                     lcd_bl.export
	output wire        lcd_wr_n,                     //                     lcd_wr.export
	output wire        lcd_rs,                     //                     lcd_rs.export
	output wire        lcd_cs_n,                     //                     lcd_cs.export
	inout  wire [15:0] lcd_data,                     //                     lcd_db.export
	output wire        sdram_clk,                     //                  sdram_clk.clk
	output wire [11:0] sdram_addr,                        //                      sdram.addr
	output wire [1:0]  sdram_ba,                          //                           .ba
	output wire        sdram_cas_n,                       //                           .cas_n
	output wire        sdram_cke,                         //                           .cke
	output wire        sdram_cs_n,                        //                           .cs_n
	inout  wire [15:0] sdram_dq,                          //                           .dq
	output wire [1:0]  sdram_dqm,                         //                           .dqm
	output wire        sdram_ras_n,                       //                           .ras_n
	output wire        sdram_we_n,                         //                           .we_n
	input  wire        uart_0_rxd,                        //                     uart_0.rxd
	output wire        uart_0_txd,                        //                           .txd
	output wire        epcs_dclk,                         //                       epcs.dclk
	output wire        epcs_sce,                          //                           .sce
	output wire        epcs_sdo,                          //                           .sdo
	input  wire        epcs_data0                         //                           .data0
);

wire clk20m; //锁相环输出的20M的时钟，为正弦波的采样频率
wire signed [7:0] mul;//pio6送出的一个8bit的数，该数乘以nco_out_unsigned[9:2],得到16bit的无符号数，再取其高8bit就能得到8bit数，再将此8bit数送给DA0_Data
assign beep=0;
mysystem u0 (

     .filter_work_export   (filter_work),   // 滤波器系数修改所要用的pio，滤波器是否工作
	  .filter_addr_export   (filter_addr),   //  filter_addr.export
	  .filter_data_export   (filter_data),   //  filter_data.export
	  .filter_write_export  (filter_write),   // fi



   .vpp_adc0_export      (vpp_adc0),//8位的输入型pio，用来读取vpp_adc0的值。
   .mul_export           (mul),    //输出型pio产生一个无符号的8bit数，此数不能超过127，让此数乘以nco_out，再取乘积的高8bit送给DA0_Data，从而能够通过pio调节dac0输出的电压
   .vpp_adc1_export      (vpp_adc1),//8位的输入型pio，用来读取vpp_adc1的值。 
   .btn_export           (btn), 
	.meisw_export         (mei_sw),        //    梅雪松的扩展拨码开关
   .meiled_export        (mei_led),         //   梅雪松的led灯。
	.pio0_inc_export      (inc),  //默认产生1k的正弦波
   .clk20m_clk           (clk20m), 
	.clk_clk                           (clk),                           //                        clk.clk
	.reset_reset_n                     (reset_n),                     //                      reset.reset_n

	//红外遥控
	.ir_decoder_clk100k   (),   //  ir_decoder.clk100k
	.ir_decoder_decode_ok (), //            .decode_ok
	.ir_decoder_decoding  (),  //            .decoding
	.ir_decoder_error     (),     //            .error
	.ir_decoder_ir_data   (),   //            .ir_data
	.ir_decoder_ir_addrh  (),  //            .ir_addrh
	.ir_decoder_ir_addrl  (),  //            .ir_addrl
	.ir_decoder_ir_in     (ir_in),     //            .ir_in
	.ir_decoder_ir_out    (),    //            .ir_out

	//串口
	.uart_0_rxd                        (uart_0_rxd),                        //                     uart_0.rxd
	.uart_0_txd                        (uart_0_txd),                        //                           .txd
	.epcs_dclk                         (epcs_dclk),                         //                       epcs.dclk
	.epcs_sce                          (epcs_sce),                          //                           .sce
	.epcs_sdo                          (epcs_sdo),                          //                           .sdo
	.epcs_data0                        (epcs_data0),                        //
	.lcd_rst_export                    (lcd_rst),                    //                    lcd_rst.export
	.lcd_bl_export                     (lcd_bl),                     //                     lcd_bl.export
	.lcd_wr_n                          (lcd_wr_n),                          //                        lcd.wr_n
	.lcd_rd_n                          (lcd_rd_n),                          //                           .rd_n
	.lcd_data                          (lcd_data),                          //                           .data
	.lcd_rs                            (lcd_rs),                            //                           .rs
	.lcd_cs_n                          (lcd_cs_n),                           //  
	.sdram_clk_clk                     (sdram_clk),                     //                  sdram_clk.clk
	.altpll_0_phasedone_conduit_export (), // altpll_0_phasedone_conduit.export
	.altpll_0_locked_conduit_export    (),    //    altpll_0_locked_conduit.export
	.altpll_0_areset_conduit_export    (),    //    altpll_0_areset_conduit.export
	.sdram_addr                        (sdram_addr),                        //                      sdram.addr
	.sdram_ba                          (sdram_ba),                          //                           .ba
	.sdram_cas_n                       (sdram_cas_n),                       //                           .cas_n
	.sdram_cke                         (sdram_cke),                         //                           .cke
	.sdram_cs_n                        (sdram_cs_n),                        //                           .cs_n
	.sdram_dq                          (sdram_dq),                          //                           .dq
	.sdram_dqm                         (sdram_dqm),                         //                           .dqm
	.sdram_ras_n                       (sdram_ras_n),                       //                           .ras_n
	.sdram_we_n                        (sdram_we_n)                         //                           .we_n
);

//调用nco核产生需要的正弦波
wire [31:0] inc;//pio控制inc以产生不同频率的正弦波
wire signed  [9:0] nco_out;
nco_lsj nco_inst
 (
	.clk       (clk),       // clk.clk
	.reset_n   (reset_n),   // rst.reset_n
	.clken     (1),     //  in.clken
	//.phi_inc_i (8589900), //此时产生100K的正弦
	.phi_inc_i (inc), //pio的引脚控制正弦的频率
	.fsin_o    (nco_out),    //输出正弦波序列，有符号数
	.out_valid ()  //    .out_valid
);

//adc测试
reg [7:0] AD0_Data;
reg [7:0] AD1_Data;
assign AD0_CLK = clk;
assign AD1_CLK = clk;

always@(posedge clk)
begin
	AD0_Data <= AD0;
	AD1_Data <= AD1;
end


//dac的代码
assign DA0_Clk=clk;
assign DA1_Clk=clk;

wire signed [17:0] nco_mul;
assign nco_mul=nco_out*mul;
wire signed [7:0] nco_mul_cut;//截取nco_mul中最有效的8bit
assign nco_mul_cut=nco_mul[16:9];
wire unsigned [7:0] nco_mul_cut_unsiged;
assign nco_mul_cut_unsiged=nco_mul_cut+128;


wire unsigned [9:0] nco_out_unsigned;
assign nco_out_unsigned=nco_out+512;//将有符号的nco_out变成无符号数
wire unsigned [7:0] nco_out_unsigned_8bit;//从10bit的nco_out_unsigned中抽取高8bit
assign nco_out_unsigned_8bit = nco_out_unsigned[9:2];//提取高8bit

assign DA0_Data=(filter_work==0)?nco_mul_cut_unsiged:filter_out_cut_unsigned;//如果work=0，则不允许dac0输出滤波器的值，=1则输出滤波器的值。
assign DA1_Data=nco_out_unsigned_8bit;//无符号正数，截取ccccccccccccccccccccccccccccccccccccccccccccccccc


/**********************************************************************************************
    利用梅雪松的acm1028模块上的adc1来采集“未知电路”送出来的信号，求其vpp，50M的采样频率，正弦的频率为1K~50kHz，
也就是一个周期包含1K到50K个采样点，我们求2^16个点中的最大值以及最小值。S
***********************************************************************************************/
reg [15:0] cnt_adc_fs;//记录adc一个轮回共采集多少个点
reg [7:0] max_adc1_temp;//adc1通道采集的数据的临时最大值
reg [7:0] max_adc1;//adc1通道采集的数据的最大值
reg [7:0] min_adc1_temp;//adc1通道采集的数据的临时最小值
reg [7:0] min_adc1;//adc1通道采集的数据的最小值
always@(posedge clk)
	begin
		if(cnt_adc_fs<50000)
			cnt_adc_fs<=cnt_adc_fs+1;
		else
			cnt_adc_fs=0;
	end

always@(posedge clk)
	begin
		if(cnt_adc_fs==0)
			begin
				max_adc1_temp<=0;
				min_adc1_temp<=250;
			end
		else
			begin
				if(AD1_Data > max_adc1_temp)
					max_adc1_temp <= AD1_Data;
				if(AD1_Data < min_adc1_temp)
					min_adc1_temp <= AD1_Data;
			end
	end

always@(posedge clk)
	begin
		if(cnt_adc_fs==49990)
			begin
				max_adc1 <= max_adc1_temp;
				min_adc1 <= min_adc1_temp;
			end
	end
wire [7:0] vpp_adc1;
assign vpp_adc1=max_adc1-min_adc1;


/**********************************************************************************************
利用梅雪松的acm1028模块上的adc0来采集自己的dac0送出的正弦波的vpp，以便调节dac0输出电压的幅度值，
***********************************************************************************************/
reg [7:0] max_adc0_temp;//adc0通道采集的数据的临时最大值
reg [7:0] max_adc0;//adc0通道采集的数据的最大值
reg [7:0] min_adc0_temp;//adc0通道采集的数据的临时最小值
reg [7:0] min_adc0;//adc0通道采集的数据的最小值
always@(posedge clk)
	begin
		if(cnt_adc_fs==0)
			begin
				max_adc0_temp<=0;
				min_adc0_temp<=250;
			end
		else
			begin
				if(AD0_Data > max_adc0_temp)
					max_adc0_temp <= AD0_Data;
				if(AD0_Data < min_adc0_temp)
					min_adc0_temp <= AD0_Data;
			end
	end

always@(posedge clk)
	begin
		if(cnt_adc_fs==49990)
			begin
				max_adc0 <= max_adc0_temp;
				min_adc0 <= min_adc0_temp;
			end
	end
wire [7:0] vpp_adc0;
assign vpp_adc0=max_adc0-min_adc0;


/**********************************************************************************************
对adc1采集的数据进行降采样，将50MHz的采样数据降采样成200KHz的采样频率,50M/200K=250
***********************************************************************************************/
reg unsigned [7:0] adc1_data_down_unsigned;//adc1采集的数据降采样后的结果。AD0_Data,无符号数
wire signed [7:0] adc1_data_down_signed;//adc1采集的数据降采样后的结果。AD0_Data,无符号数

reg [7:0] cnt1;
always@(posedge clk)
	begin
		if(cnt1<249)
			cnt1<=cnt1+1;
		else
			cnt1<=0;
	end

always@(posedge clk)
	begin
		if(cnt1==20)
			adc1_data_down_unsigned<=AD0_Data;
	end

assign clk200k=(cnt1<125)?1:0;
assign adc1_data_down_signed = adc1_data_down_unsigned-128;//降采样后的有符号数


/********************************************************************************
定义滤波器以及系数，对adc1_data_down_signed进行滤波
****************************************************************************/
reg signed [9:0] coef[31:0];//滤波器系数
reg signed [7:0] d_filter[31:0];//滤波器中存的数据
always@(posedge clk200k)
begin
	begin
		d_filter[0]<=adc1_data_down_signed;
		d_filter[1]<=d_filter[0];
		d_filter[2]<=d_filter[1];
		d_filter[3]<=d_filter[2];
		d_filter[4]<=d_filter[3];
		d_filter[5]<=d_filter[4];
		d_filter[6]<=d_filter[5];
		d_filter[7]<=d_filter[6];
		d_filter[8]<=d_filter[7];
		d_filter[9]<=d_filter[8];
		d_filter[10]<=d_filter[9];
		d_filter[11]<=d_filter[10];
		d_filter[12]<=d_filter[11];
		d_filter[13]<=d_filter[12];
		d_filter[14]<=d_filter[13];
		d_filter[15]<=d_filter[14];
		d_filter[16]<=d_filter[15];
		d_filter[17]<=d_filter[16];
		d_filter[18]<=d_filter[17];
		d_filter[19]<=d_filter[18];
		d_filter[20]<=d_filter[19];
		d_filter[21]<=d_filter[20];
		d_filter[22]<=d_filter[21];
		d_filter[23]<=d_filter[22];
		d_filter[24]<=d_filter[23];
		d_filter[25]<=d_filter[24];
		d_filter[26]<=d_filter[25];
		d_filter[27]<=d_filter[26];
		d_filter[28]<=d_filter[27];
		d_filter[29]<=d_filter[28];
		d_filter[30]<=d_filter[29];
		d_filter[31]<=d_filter[30];
	end
end

reg signed [10+8-1+2-1:0] sum1[7:0];//每四个数加到一起
always@(posedge clk200k)
	begin
		sum1[0]<=d_filter[0]*coef[0]+d_filter[1]*coef[1]+d_filter[2]*coef[2]+d_filter[3]*coef[3];
		sum1[1]<=d_filter[4]*coef[4]+d_filter[5]*coef[5]+d_filter[6]*coef[6]+d_filter[7]*coef[7];
		sum1[2]<=d_filter[8]*coef[8]+d_filter[9]*coef[9]+d_filter[10]*coef[10]+d_filter[11]*coef[11];
		sum1[3]<=d_filter[12]*coef[12]+d_filter[13]*coef[13]+d_filter[14]*coef[14]+d_filter[15]*coef[15];
	   sum1[4]<=d_filter[16]*coef[16]+d_filter[17]*coef[17]+d_filter[18]*coef[18]+d_filter[19]*coef[19];
		sum1[5]<=d_filter[20]*coef[20]+d_filter[21]*coef[21]+d_filter[22]*coef[22]+d_filter[23]*coef[23];
		sum1[6]<=d_filter[24]*coef[24]+d_filter[25]*coef[25]+d_filter[26]*coef[26]+d_filter[27]*coef[27];
		sum1[7]<=d_filter[28]*coef[28]+d_filter[29]*coef[29]+d_filter[30]*coef[30]+d_filter[31]*coef[31];
	end

reg signed [10+8-1+2-1+3:0] filter_out_prime;//[21:0],共22bit,滤波器原始输出的数据，wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww

always@(posedge clk200k)
	begin
		filter_out_prime <= sum1[0]+sum1[1]+sum1[2]+sum1[3]+sum1[4]+sum1[5]+sum1[6]+sum1[7];
	end
wire signed [7:0] filter_out_cut;
assign filter_out_cut=filter_out_prime[18:11];//cccccccccccccccccccccccccccccccccccccccccccccccc
	
wire unsigned [7:0] filter_out_cut_unsigned;
assign filter_out_cut_unsigned=filter_out_cut+128;//将滤波器输出的有符号数变成无符号数
	

/**********************************************************************************************
设计数字滤波器，并让滤波器的系数可以由c语言进行修改，
pio7_filter_work:输出型的1位pio，当=0时，表示在做问题1~5的测试，此时dac0输出的是问题1~5的数据，当=1时，dac0输出的是数字滤波器滤出来的波形
                 dac0输出的信号要跟据pio7_filter_work的状态来选择到底是147行的还是下面的    。
pio8_filter_addr:输出型的5位的pio，表示cpu向哪个滤波器系数寄存器中在写数。
pio9_filter_data:输出型的10bit的pio，有符号数，代表cpu向滤波器系数中要写进去的数据
pio10_filter_write:输出型的1bit的pio，为1则向滤波器系数寄存器中写数，为0则不写数
***********************************************************************************************/
wire filter_work; // 滤波器系数修改所要用的pio，滤波器是否工作
wire [4:0] filter_addr; //  filter_addr.export
wire [9:0] filter_data;   //  filter_data.export
wire filter_write;  // fi


always@(posedge clk)
	begin
		if(filter_write==1)
			coef[filter_addr]<=filter_data;
	end

endmodule

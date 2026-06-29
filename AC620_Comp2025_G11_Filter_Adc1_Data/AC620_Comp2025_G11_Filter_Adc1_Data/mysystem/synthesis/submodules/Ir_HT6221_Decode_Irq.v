///作者：刘三军
///时间：2019年6月27日
///功能：对红外解码模块HT6221的接收波形进行解码,并能够产生中断
module Ir_HT6221_Decode_Irq
(
input reset_n,///复位信号
input clk50m,///50兆时钟输入

//cpu read and write
input CPU_CS,//片选
input CPU_RD,//读信号
input CPU_WR,//写信号
input CPU_Addr,//地址信号
input [31:0] CPU_WR_Data,//cpu利用此脚向其中写入数据
output [31:0] CPU_RD_Data,//读数据

//中断请求信号
output reg irq_out,

///export信号
input ir_in,///HT6221的输入电平信号线
output clk100k,///将50MHz的时钟分频成100KHz，目的是让0.56ms的电平脉冲采样点数降低,周期位10us

//测试信号
output reg decoding,///正在解码标志
output reg decode_ok,///持续clk100k的一个周期，表示一次解码完毕
output reg [31:0] ir_out,///解码的数字和地址编成32位的数
output reg [7:0] ir_Data,///解码得到的数字
output reg [7:0] ir_AddrH,///解码得到的地址高位
output reg [7:0] ir_AddrL,///解码得到的地址低位
output reg error ///当高低脉冲的电平计数值不在合理的范围之内时输出错误信号
);
///可调参数申明
parameter T_Sa_Clk=10000;///采样时钟的周期，10000ns
parameter T_Clk50m=20;///50m时钟的周期，20ns

parameter N_Code=32;///解码的个数
parameter N_9ms=9000000/T_Sa_Clk;///9ms中包含多少个采样点,=900
parameter N_4_5ms=4500000/T_Sa_Clk;///9ms中包含多少个采样点,=450
parameter N_560us=560000/T_Sa_Clk;///560us中包含多少个采样点,=56
parameter N_1690us=1690000/T_Sa_Clk;///1690us中包含多少个采样点,=169
parameter N_Dev=20;///当判断ir的脉宽范围时的误差,deviation

reg [31:0] reg_CPU_RD_Data;//cpu读数据

///中间变量
reg [15:0] cnt1_clk100k;///用来产生100kHz时钟的计数器
reg ir_delay;///将ir_in延迟一个clk100k周期
reg up_edge_pulse;////在ir_in的上升沿处会产生一个周期=1的up_edge_pulse脉冲
reg down_edge_pulse;////在ir_in的下降沿处会产生一个周期=1的down_edge_pulse脉冲
reg [15:0] cnt2_IrPulseLen;///用来对ir信号中的每个pulse的长度进行计数,9ms对应计数900个，0.56ms计56个
reg [15:0] num_low;///低电平持续的clk100k时钟数
reg [15:0] num_high;///高电平持续的clk100k时钟数
reg [5:0] code_num;///当前已经解开的码的个数，由于最多解32个，故只需要6bit即可
reg [31:0] ir_out_temp;///ir_out由于在运行过程中不断变化，不太稳定，故先让不稳定的ir_out存于ir_out_temp中

/*******************************************************************
   cpu read 
********************************************************************/ 
always @(posedge clk50m)
	begin
	 if( (CPU_CS==1) && (CPU_RD==1) )
	   begin
			if(CPU_Addr==0)
			  begin
			    reg_CPU_RD_Data[7:0]<= ir_Data;
				 reg_CPU_RD_Data[31:8]<= 24'b0;
				 //reg_CPU_RD_Data <= {(24){1'b0},ir_Data};
			  end
			else if(CPU_Addr==1)
			  reg_CPU_RD_Data<=ir_out;         
			else 
			  reg_CPU_RD_Data<=32'hzzzzzzzz;
		end
	 else 
		reg_CPU_RD_Data<=32'hzzzzzzzz;
	end
          
assign CPU_RD_Data=reg_CPU_RD_Data;

///中断产生信号
always @(posedge clk50m)
	begin
		if((CPU_CS==1)&&(CPU_WR==1))
			 irq_out<=0;
		else
		   if(decode_ok==1)
				irq_out<=1;
	end

always@(posedge clk50m)
	begin
		if(cnt1_clk100k<T_Sa_Clk/T_Clk50m-1)
			cnt1_clk100k<=cnt1_clk100k+1;
		else
		   cnt1_clk100k<=0;
	end

assign clk100k=(cnt1_clk100k<T_Sa_Clk/T_Clk50m/2)?1:0;

///将ir_in延迟一个周期
always@(posedge clk100k)
	begin
		ir_delay<=ir_in;
	end

///判断何时有下降沿、何时有上升沿
always@(posedge clk100k)  ///下降沿到来时down_edge_pulse持续一个时钟周期的高电平，其他时间为低电平
	begin
		if(ir_in==0 && ir_delay==1)
			down_edge_pulse<=1;
		else
			down_edge_pulse<=0;
	end

always@(posedge clk100k)  ///上升沿到来
	begin
		if(ir_in==1 && ir_delay==0)
			up_edge_pulse<=1;
		else
			up_edge_pulse<=0;
	end

///对ir信号中的脉冲长度进行计数
always@(posedge clk100k)  ///上升沿到来
	begin
		if(down_edge_pulse==1 || up_edge_pulse==1)
			cnt2_IrPulseLen<=0;
		else 
			cnt2_IrPulseLen<=cnt2_IrPulseLen+1;
	end

	///在上升沿和下降沿分别获取低电平和高电平持续的clk100k时钟数
always@(posedge clk100k)  
	begin
		if(up_edge_pulse==1)///在上升处捕获9ms的低电平持续的时钟数
			num_low<=cnt2_IrPulseLen;
		if(ir_in==0 && ir_delay==1) ///在下降沿处捕获9ms的低电平持续的时钟数，注意是在down_edge_pulse的前一个时钟周期
			num_high<=cnt2_IrPulseLen;
	end
	
///决定decoding的状态，当ir_in的脉冲的范围合适，down_edge_pulse==1，且没有解码完毕，则认为decoding=1，即正在解码中,如果解码完，则decoding<=0
always@(posedge clk100k)  
	begin	
		if(down_edge_pulse==1 && num_low<N_9ms+N_Dev+80 && num_low>N_9ms-N_Dev-80 && num_high<N_4_5ms+N_Dev+80 && num_high>N_4_5ms-N_Dev-80 && code_num<=N_Code)
			decoding<=1;
		else 
		    begin
				if(decode_ok==1 || reset_n==0)
					decoding<=0;
			end
	end

///对到来的码数进行计数,code_num
always@(posedge clk100k)  
	begin
		if(decoding==0 || decode_ok==1)
			code_num<=0;
		else  ///意思是说decoding==1，且decode_ok==0时
			begin
				if(down_edge_pulse==1) ///ir_in下降沿到来
					code_num<=code_num+1;
			end
	end

///解码期间，即decoding==1时，在每个down_edge_pulse_decode==1时根据num_low和num_high的大小决定当前第code_num个码是1还是0
always@(posedge clk100k)  
	begin
		if(down_edge_pulse==1 && decoding==1)///在解码过程中，如果ir的下降沿到来，
			begin   ///则根据cnt4_low和cnt5_high中存储的低高电平的计数值的范围来确定收到的码是1还是0
				if(num_low>N_560us-N_Dev && num_low<N_560us+N_Dev && num_high>N_1690us-N_Dev && num_high<N_1690us+N_Dev)
					begin
						ir_out_temp[code_num]<=1;
						error<=0;
					end
				else if(num_low>N_560us-N_Dev && num_low<N_560us+N_Dev && num_high>N_560us-N_Dev && num_high<N_560us+N_Dev)
					begin
						ir_out_temp[code_num]<=0;
						error<=0;
					end
				else
					error<=1;
			end
	end

///decode_ok，当32个码都解出来之后就让decode_ok持续一个时钟周期的高电平
always@(posedge clk100k)  
	begin
		if(code_num>=N_Code-1 && down_edge_pulse==1)
			decode_ok<=1;
		else
			decode_ok<=0;
	end

///在decode_ok==1的瞬间将ir_out_temp的值送入ir_out中，同时将其对应位置的8bit数分别送入数据和地址的高低8位。
always@(posedge clk100k)  
	begin
		if(decode_ok==1)
			begin
				ir_out<=ir_out_temp;
				ir_Data<=ir_out_temp[23:16];
				ir_AddrH<=ir_out_temp[15:8];
				ir_AddrL<=ir_out_temp[7:0];
			end
	end	
	
endmodule


module LCD_Control(
	input av_reset_n,
	input clk,
	input av_chipselect_n,
	input av_write_n,
	input av_read_n,
	input av_addr,
	input [15:0]av_writedata,
	output [15:0]av_readdata,
	
	output lcd_wr_n,
	output lcd_rd_n,
	inout [15:0]lcd_data,
	output lcd_cs_n,
	output lcd_rs

);
	
	reg [15:0]r_lcd_data;
	assign lcd_wr_n = av_write_n;
	assign lcd_rd_n = av_read_n;
	assign lcd_rs = av_addr;
	assign lcd_cs_n = av_chipselect_n;
	
	assign av_readdata = r_lcd_data;
	assign lcd_data = av_read_n?av_writedata:16'hzzzz;
	
	always@(posedge clk or negedge av_reset_n)
	if(!av_reset_n)
		r_lcd_data <= 16'd0;
	else
		r_lcd_data <= lcd_data;
	
endmodule

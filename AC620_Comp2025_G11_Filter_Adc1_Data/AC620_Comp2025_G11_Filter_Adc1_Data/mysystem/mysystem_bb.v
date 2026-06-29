
module mysystem (
	btn_export,
	clk_clk,
	clk20m_clk,
	epcs_dclk,
	epcs_sce,
	epcs_sdo,
	epcs_data0,
	ir_decoder_clk100k,
	ir_decoder_decode_ok,
	ir_decoder_decoding,
	ir_decoder_error,
	ir_decoder_ir_data,
	ir_decoder_ir_addrh,
	ir_decoder_ir_addrl,
	ir_decoder_ir_in,
	ir_decoder_ir_out,
	lcd_wr_n,
	lcd_rd_n,
	lcd_data,
	lcd_rs,
	lcd_cs_n,
	lcd_bl_export,
	lcd_rst_export,
	meiled_export,
	meisw_export,
	mul_export,
	pio0_inc_export,
	reset_reset_n,
	sdram_addr,
	sdram_ba,
	sdram_cas_n,
	sdram_cke,
	sdram_cs_n,
	sdram_dq,
	sdram_dqm,
	sdram_ras_n,
	sdram_we_n,
	sdram_clk_clk,
	uart_0_rxd,
	uart_0_txd,
	vpp_adc0_export,
	vpp_adc1_export,
	filter_work_export,
	filter_addr_export,
	filter_data_export,
	filter_write_export);	

	input	[1:0]	btn_export;
	input		clk_clk;
	output		clk20m_clk;
	output		epcs_dclk;
	output		epcs_sce;
	output		epcs_sdo;
	input		epcs_data0;
	output		ir_decoder_clk100k;
	output		ir_decoder_decode_ok;
	output		ir_decoder_decoding;
	output		ir_decoder_error;
	output	[7:0]	ir_decoder_ir_data;
	output	[7:0]	ir_decoder_ir_addrh;
	output	[7:0]	ir_decoder_ir_addrl;
	input		ir_decoder_ir_in;
	output	[31:0]	ir_decoder_ir_out;
	output		lcd_wr_n;
	output		lcd_rd_n;
	inout	[15:0]	lcd_data;
	output		lcd_rs;
	output		lcd_cs_n;
	output		lcd_bl_export;
	output		lcd_rst_export;
	output	[7:0]	meiled_export;
	input	[7:0]	meisw_export;
	output	[7:0]	mul_export;
	output	[31:0]	pio0_inc_export;
	input		reset_reset_n;
	output	[11:0]	sdram_addr;
	output	[1:0]	sdram_ba;
	output		sdram_cas_n;
	output		sdram_cke;
	output		sdram_cs_n;
	inout	[15:0]	sdram_dq;
	output	[1:0]	sdram_dqm;
	output		sdram_ras_n;
	output		sdram_we_n;
	output		sdram_clk_clk;
	input		uart_0_rxd;
	output		uart_0_txd;
	input	[7:0]	vpp_adc0_export;
	input	[7:0]	vpp_adc1_export;
	output		filter_work_export;
	output	[4:0]	filter_addr_export;
	output	[9:0]	filter_data_export;
	output		filter_write_export;
endmodule

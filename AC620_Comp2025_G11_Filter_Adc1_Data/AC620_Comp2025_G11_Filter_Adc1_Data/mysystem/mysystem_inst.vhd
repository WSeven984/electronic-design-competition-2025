	component mysystem is
		port (
			btn_export           : in    std_logic_vector(1 downto 0)  := (others => 'X'); -- export
			clk_clk              : in    std_logic                     := 'X';             -- clk
			clk20m_clk           : out   std_logic;                                        -- clk
			epcs_dclk            : out   std_logic;                                        -- dclk
			epcs_sce             : out   std_logic;                                        -- sce
			epcs_sdo             : out   std_logic;                                        -- sdo
			epcs_data0           : in    std_logic                     := 'X';             -- data0
			ir_decoder_clk100k   : out   std_logic;                                        -- clk100k
			ir_decoder_decode_ok : out   std_logic;                                        -- decode_ok
			ir_decoder_decoding  : out   std_logic;                                        -- decoding
			ir_decoder_error     : out   std_logic;                                        -- error
			ir_decoder_ir_data   : out   std_logic_vector(7 downto 0);                     -- ir_data
			ir_decoder_ir_addrh  : out   std_logic_vector(7 downto 0);                     -- ir_addrh
			ir_decoder_ir_addrl  : out   std_logic_vector(7 downto 0);                     -- ir_addrl
			ir_decoder_ir_in     : in    std_logic                     := 'X';             -- ir_in
			ir_decoder_ir_out    : out   std_logic_vector(31 downto 0);                    -- ir_out
			lcd_wr_n             : out   std_logic;                                        -- wr_n
			lcd_rd_n             : out   std_logic;                                        -- rd_n
			lcd_data             : inout std_logic_vector(15 downto 0) := (others => 'X'); -- data
			lcd_rs               : out   std_logic;                                        -- rs
			lcd_cs_n             : out   std_logic;                                        -- cs_n
			lcd_bl_export        : out   std_logic;                                        -- export
			lcd_rst_export       : out   std_logic;                                        -- export
			meiled_export        : out   std_logic_vector(7 downto 0);                     -- export
			meisw_export         : in    std_logic_vector(7 downto 0)  := (others => 'X'); -- export
			mul_export           : out   std_logic_vector(7 downto 0);                     -- export
			pio0_inc_export      : out   std_logic_vector(31 downto 0);                    -- export
			reset_reset_n        : in    std_logic                     := 'X';             -- reset_n
			sdram_addr           : out   std_logic_vector(11 downto 0);                    -- addr
			sdram_ba             : out   std_logic_vector(1 downto 0);                     -- ba
			sdram_cas_n          : out   std_logic;                                        -- cas_n
			sdram_cke            : out   std_logic;                                        -- cke
			sdram_cs_n           : out   std_logic;                                        -- cs_n
			sdram_dq             : inout std_logic_vector(15 downto 0) := (others => 'X'); -- dq
			sdram_dqm            : out   std_logic_vector(1 downto 0);                     -- dqm
			sdram_ras_n          : out   std_logic;                                        -- ras_n
			sdram_we_n           : out   std_logic;                                        -- we_n
			sdram_clk_clk        : out   std_logic;                                        -- clk
			uart_0_rxd           : in    std_logic                     := 'X';             -- rxd
			uart_0_txd           : out   std_logic;                                        -- txd
			vpp_adc0_export      : in    std_logic_vector(7 downto 0)  := (others => 'X'); -- export
			vpp_adc1_export      : in    std_logic_vector(7 downto 0)  := (others => 'X'); -- export
			filter_work_export   : out   std_logic;                                        -- export
			filter_addr_export   : out   std_logic_vector(4 downto 0);                     -- export
			filter_data_export   : out   std_logic_vector(9 downto 0);                     -- export
			filter_write_export  : out   std_logic                                         -- export
		);
	end component mysystem;

	u0 : component mysystem
		port map (
			btn_export           => CONNECTED_TO_btn_export,           --          btn.export
			clk_clk              => CONNECTED_TO_clk_clk,              --          clk.clk
			clk20m_clk           => CONNECTED_TO_clk20m_clk,           --       clk20m.clk
			epcs_dclk            => CONNECTED_TO_epcs_dclk,            --         epcs.dclk
			epcs_sce             => CONNECTED_TO_epcs_sce,             --             .sce
			epcs_sdo             => CONNECTED_TO_epcs_sdo,             --             .sdo
			epcs_data0           => CONNECTED_TO_epcs_data0,           --             .data0
			ir_decoder_clk100k   => CONNECTED_TO_ir_decoder_clk100k,   --   ir_decoder.clk100k
			ir_decoder_decode_ok => CONNECTED_TO_ir_decoder_decode_ok, --             .decode_ok
			ir_decoder_decoding  => CONNECTED_TO_ir_decoder_decoding,  --             .decoding
			ir_decoder_error     => CONNECTED_TO_ir_decoder_error,     --             .error
			ir_decoder_ir_data   => CONNECTED_TO_ir_decoder_ir_data,   --             .ir_data
			ir_decoder_ir_addrh  => CONNECTED_TO_ir_decoder_ir_addrh,  --             .ir_addrh
			ir_decoder_ir_addrl  => CONNECTED_TO_ir_decoder_ir_addrl,  --             .ir_addrl
			ir_decoder_ir_in     => CONNECTED_TO_ir_decoder_ir_in,     --             .ir_in
			ir_decoder_ir_out    => CONNECTED_TO_ir_decoder_ir_out,    --             .ir_out
			lcd_wr_n             => CONNECTED_TO_lcd_wr_n,             --          lcd.wr_n
			lcd_rd_n             => CONNECTED_TO_lcd_rd_n,             --             .rd_n
			lcd_data             => CONNECTED_TO_lcd_data,             --             .data
			lcd_rs               => CONNECTED_TO_lcd_rs,               --             .rs
			lcd_cs_n             => CONNECTED_TO_lcd_cs_n,             --             .cs_n
			lcd_bl_export        => CONNECTED_TO_lcd_bl_export,        --       lcd_bl.export
			lcd_rst_export       => CONNECTED_TO_lcd_rst_export,       --      lcd_rst.export
			meiled_export        => CONNECTED_TO_meiled_export,        --       meiled.export
			meisw_export         => CONNECTED_TO_meisw_export,         --        meisw.export
			mul_export           => CONNECTED_TO_mul_export,           --          mul.export
			pio0_inc_export      => CONNECTED_TO_pio0_inc_export,      --     pio0_inc.export
			reset_reset_n        => CONNECTED_TO_reset_reset_n,        --        reset.reset_n
			sdram_addr           => CONNECTED_TO_sdram_addr,           --        sdram.addr
			sdram_ba             => CONNECTED_TO_sdram_ba,             --             .ba
			sdram_cas_n          => CONNECTED_TO_sdram_cas_n,          --             .cas_n
			sdram_cke            => CONNECTED_TO_sdram_cke,            --             .cke
			sdram_cs_n           => CONNECTED_TO_sdram_cs_n,           --             .cs_n
			sdram_dq             => CONNECTED_TO_sdram_dq,             --             .dq
			sdram_dqm            => CONNECTED_TO_sdram_dqm,            --             .dqm
			sdram_ras_n          => CONNECTED_TO_sdram_ras_n,          --             .ras_n
			sdram_we_n           => CONNECTED_TO_sdram_we_n,           --             .we_n
			sdram_clk_clk        => CONNECTED_TO_sdram_clk_clk,        --    sdram_clk.clk
			uart_0_rxd           => CONNECTED_TO_uart_0_rxd,           --       uart_0.rxd
			uart_0_txd           => CONNECTED_TO_uart_0_txd,           --             .txd
			vpp_adc0_export      => CONNECTED_TO_vpp_adc0_export,      --     vpp_adc0.export
			vpp_adc1_export      => CONNECTED_TO_vpp_adc1_export,      --     vpp_adc1.export
			filter_work_export   => CONNECTED_TO_filter_work_export,   --  filter_work.export
			filter_addr_export   => CONNECTED_TO_filter_addr_export,   --  filter_addr.export
			filter_data_export   => CONNECTED_TO_filter_data_export,   --  filter_data.export
			filter_write_export  => CONNECTED_TO_filter_write_export   -- filter_write.export
		);


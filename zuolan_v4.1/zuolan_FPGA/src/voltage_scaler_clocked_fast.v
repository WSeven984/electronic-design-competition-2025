module voltage_scaler_clocked_fast(
input wire clk, // ʱ������
input wire [13:0] rom_data, // 14λROM��������
input wire [11:0] voltage_mv, // Ŀ����ֵ��ѹ�������������1550��ʾ1.55V��������3V��Ҳ����3000mV

output reg [13:0] scaled_data // ���ź�����������
);

// ��������
parameter ROM_MAX = 16383; // 2^14 - 1
parameter DEFAULT_PEAK_MV = 3080; // Ĭ�Ϸ�ֵ��ѹ���������3.08V��
parameter HALF_ROM_MAX = ROM_MAX / 2; // ROM���ݵ�����ֵ��һ��

// �Ż����ģ�ʹ��λ�ƴ�������
// DEFAULT_PEAK_MV = 3080 �� 4096 = 2^12
// ʹ��λ�������������ܣ�ͬʱ���־���
parameter SCALE_SHIFT = 12; // 2^12 = 4096
parameter CORRECTION_NUM = 3080; // ԭʼ����
parameter CORRECTION_DEN = 4096; // λ�Ƶ�Ч����

// 内部寄存器 - 流水线优化
reg [13:0] rom_data_reg1, rom_data_reg2;
reg [11:0] voltage_mv_reg1, voltage_mv_reg2;
reg [13:0] offset_data;
reg [25:0] mult_result; // 足够位宽存储乘积
reg sign_bit_reg1, sign_bit_reg2;

// ������������ˮ�ߴ���
always @(posedge clk) begin
    // ��һ����ˮ�ߣ������Ĵ��ͷ����ж�
    rom_data_reg1 <= rom_data;
    voltage_mv_reg1 <= voltage_mv;
    sign_bit_reg1 <= (rom_data >= HALF_ROM_MAX);
    
    // �ڶ�����ˮ�ߣ�ƫ�������
    rom_data_reg2 <= rom_data_reg1;
    voltage_mv_reg2 <= voltage_mv_reg1;
    sign_bit_reg2 <= sign_bit_reg1;
    
    // ����ƫ���������ֵ��
    if (sign_bit_reg1) begin
        offset_data <= rom_data_reg1 - HALF_ROM_MAX;
    end else begin
        offset_data <= HALF_ROM_MAX - rom_data_reg1;
    end
    
    // ��������ˮ�ߣ����ٳ˷���λ�Ƴ���
    mult_result <= offset_data * voltage_mv_reg2;
    
    // 第四级流水线：正确的幅度缩放计算
    // 目标：实现 (offset_data * voltage_mv) / DEFAULT_PEAK_MV
    // mult_result = offset_data * voltage_mv
    // 需要除以 DEFAULT_PEAK_MV = 3080
    //
    // 使用简化的位移近似方法：
    // DEFAULT_PEAK_MV = 3080，接近 3072 = 3 * 1024 = 3 * 2^10
    // 所以 mult_result / 3080 ≈ mult_result / (3 * 2^10) = (mult_result >> 10) / 3
    //
    // 为了避免除法，使用近似：1/3 ≈ 0.333 ≈ 85/256 = 85/2^8
    // 所以最终结果 ≈ (mult_result >> 10) * 85 >> 8 = (mult_result * 85) >> 18
    //
    // 但是 85/256 = 0.332，而实际需要 1/3.08 = 0.325
    // 更精确的近似：1/3.08 ≈ 83/256，所以使用 83 作为乘数

    if (sign_bit_reg2) begin
        // 正半轴：加上缩放后的幅度
        scaled_data <= HALF_ROM_MAX + ((mult_result * 83) >> 18);
    end else begin
        // 负半轴：减去缩放后的幅度
        scaled_data <= HALF_ROM_MAX - ((mult_result * 83) >> 18);
    end
end

endmodule

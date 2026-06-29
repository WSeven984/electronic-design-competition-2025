`timescale 1ns / 1ps

module voltage_scaler_test;

// 测试信号
reg clk;
reg [13:0] rom_data;
reg [11:0] voltage_mv;
wire [13:0] scaled_data;

// 实例化被测模块
voltage_scaler_clocked_fast uut (
    .clk(clk),
    .rom_data(rom_data),
    .voltage_mv(voltage_mv),
    .scaled_data(scaled_data)
);

// 时钟生成
initial begin
    clk = 0;
    forever #5 clk = ~clk; // 100MHz时钟
end

// 测试序列
initial begin
    // 初始化
    rom_data = 0;
    voltage_mv = 0;
    
    // 等待几个时钟周期
    #50;
    
    $display("开始电压缩放模块测试");
    $display("时间\t输入ROM\t输入电压\t输出数据\t期望幅度");
    
    // 测试用例1：满幅度测试
    rom_data = 16383; // 最大ROM值
    voltage_mv = 3080; // 满幅度电压
    #50; // 等待流水线稳定
    $display("%0t\t%d\t%d\t%d\t%d", $time, rom_data, voltage_mv, scaled_data, 16383);
    
    // 测试用例2：半幅度测试
    voltage_mv = 1540; // 半幅度电压
    #50;
    $display("%0t\t%d\t%d\t%d\t%d", $time, rom_data, voltage_mv, scaled_data, 8191);
    
    // 测试用例3：四分之一幅度测试
    voltage_mv = 770; // 四分之一幅度电压
    #50;
    $display("%0t\t%d\t%d\t%d\t%d", $time, rom_data, voltage_mv, scaled_data, 4095);
    
    // 测试用例4：中间值测试
    rom_data = 8191; // ROM中间值
    voltage_mv = 3080; // 满幅度电压
    #50;
    $display("%0t\t%d\t%d\t%d\t%d", $time, rom_data, voltage_mv, scaled_data, 8191);
    
    // 测试用例5：负半轴测试
    rom_data = 0; // 最小ROM值
    voltage_mv = 3080; // 满幅度电压
    #50;
    $display("%0t\t%d\t%d\t%d\t%d", $time, rom_data, voltage_mv, scaled_data, 0);
    
    $display("测试完成");
    $finish;
end

// 监控输出变化
always @(posedge clk) begin
    if (scaled_data !== 14'bx) begin
        // 计算幅度误差
        if (rom_data >= 8191) begin
            // 正半轴
            $display("正半轴: ROM=%d, 电压=%d, 输出=%d", rom_data, voltage_mv, scaled_data);
        end else begin
            // 负半轴
            $display("负半轴: ROM=%d, 电压=%d, 输出=%d", rom_data, voltage_mv, scaled_data);
        end
    end
end

endmodule

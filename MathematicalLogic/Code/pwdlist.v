`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2017/06/19 13:33:31
// Design Name: 
// Module Name: pwdlist
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module pwdlist(
    input [2:0]toselect,        //计数选择
    output reg[1:0]currpwd,      //输出密码
    output reg[2:0]pwdlen,       //密码长度
    input change,             //是否修改密码
    input unlock,             //是否已解锁
    input [1:0]key,           //数据输入
    input keypress            //按键脉冲
    );
    reg [15:0]pwd;            //每两位是一组密码.
    reg [2:0]count;
    
    wire NClk;
    assign NClk = keypress && change && unlock;
    
    initial begin
    count = 0;
    pwd = 0;                  //初始密码
    pwdlen = 4;               //初始密码长度
    end
    
    always @(toselect,pwd)    //密码选择部分
    begin
    case(toselect)
    3'b000 : currpwd = pwd[1:0];
    3'b001 : currpwd = pwd[3:2];
    3'b010 : currpwd = pwd[5:4];
    3'b011 : currpwd = pwd[7:6];
    3'b100 : currpwd = pwd[9:8];
    3'b101 : currpwd = pwd[11:10];
    3'b110 : currpwd = pwd[13:12];
    3'b111 : currpwd = pwd[15:14];
    endcase
    end

    always @(posedge NClk)
    begin
    if(change && unlock) begin            //已经解锁并且给了密码修改使能
    case(count)
    3'b000 : pwd[1:0] = key;
    3'b001 : pwd[3:2] = key;
    3'b010 : pwd[5:4] = key;
    3'b011 : pwd[7:6] = key;
    3'b100 : pwd[9:8] = key;
    3'b101 : pwd[11:10] = key;
    3'b110 : pwd[13:12] = key;
    3'b111 : pwd[15:14] = key;
    endcase
    count = count + 1;
    end
    else count = 0;
    end
    
    always @(negedge change)begin         //密码长度修改
    if(unlock) pwdlen = count;
    end

endmodule

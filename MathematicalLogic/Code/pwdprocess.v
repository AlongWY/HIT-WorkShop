`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2017/06/19 13:19:20
// Design Name: 
// Module Name: pwdprocess
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


module pwdprocess(
    input [1:0] keypwd,             //按键输入密码
    input [1:0] cmppwd,             //正确密码
    input keypress,                 //按键脉冲，上升沿触发
    output res                      //输出计数脉冲
    );
    wire cmp;
    assign cmp = keypwd == cmppwd;
    assign res = cmp && keypress;
endmodule

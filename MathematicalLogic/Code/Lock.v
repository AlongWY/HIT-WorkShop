`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2017/06/19 16:25:30
// Design Name: 
// Module Name: Lock
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


module Lock(
    input [2:0]count,              //��ǰ������ȷ���������
    input [2:0]length,             //���볤��
    output state              //��ǰ����״̬��1Ϊ����
    );
    assign state = (count == length);
endmodule

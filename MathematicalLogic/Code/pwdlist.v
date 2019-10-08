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
    input [2:0]toselect,        //����ѡ��
    output reg[1:0]currpwd,      //�������
    output reg[2:0]pwdlen,       //���볤��
    input change,             //�Ƿ��޸�����
    input unlock,             //�Ƿ��ѽ���
    input [1:0]key,           //��������
    input keypress            //��������
    );
    reg [15:0]pwd;            //ÿ��λ��һ������.
    reg [2:0]count;
    
    wire NClk;
    assign NClk = keypress && change && unlock;
    
    initial begin
    count = 0;
    pwd = 0;                  //��ʼ����
    pwdlen = 4;               //��ʼ���볤��
    end
    
    always @(toselect,pwd)    //����ѡ�񲿷�
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
    if(change && unlock) begin            //�Ѿ��������Ҹ��������޸�ʹ��
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
    
    always @(negedge change)begin         //���볤���޸�
    if(unlock) pwdlen = count;
    end

endmodule

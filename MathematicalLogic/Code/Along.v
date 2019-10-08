`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2017/06/26 17:05:03
// Design Name: 
// Module Name: Along
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


module Along(
    input Change,
    input KeyPress,
    input [1:0]KeyValue,
    input Lock_It,
    output LockState//,
//    output [2:0]pwdlen
    );
    
      wire toLock;
      assign toLock=~Lock_It;
      
      wire [2:0]currlen;
      wire [1:0]currpwd;
      wire [2:0]pwdlen;
      wire isRight;
      
      Lock LockMgr
             (.count(currlen),
              .length(pwdlen),
              .state(LockState));
        counter PwdLen
             (.CLK(isRight),
              .CLR(toLock),
              .EN(LockState),
              .num(currlen));
        pwdlist Pwd_list
             (.change(Change),
              .currpwd(currpwd),
              .key(KeyValue),
              .keypress(KeyPress),
              .pwdlen(pwdlen),
              .toselect(currlen),
              .unlock(LockState));
        pwdprocess Process
             (.cmppwd(currpwd),
              .keypress(KeyPress),
              .keypwd(KeyValue),
              .res(isRight));
endmodule

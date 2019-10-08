module Execute(
  input [31:0] IR,
  input [31:0] NPC,
  input [31:0] A,
  input [31:0] B,
  input [31:0] Imm,
  output cond,
  output [31:0]Res
);

wire [5:0] Op;
assign Op = IR[31:26];

assign cond = Op == 6'b100001 ? 1 :             // JMP
              Op == 6'b100000 ? A == 0 :        // beq
              0;

assign Res = Op == 6'b000000 ? A + B:
             Op == 6'b000001 ? A - B:
             Op == 6'b000010 ? A & B:
             Op == 6'b000011 ? A | B:
             Op == 6'b000100 ? A ^ B:
             Op == 6'b000101 ? A < B:
             Op == 6'b010000 ? A + Imm:         // MEM
             Op == 6'b010001 ? A + Imm:
             Op == 6'b100000 ? NPC + Imm:       // BEQ
             Op == 6'b100001 ? Imm:             // JMP
             0;

endmodule
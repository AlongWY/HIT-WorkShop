module Decode(
  input clk,
  input [31:0] IR,
  output [31:0] A,
  output [31:0] B,
  output [31:0] Imm,
  input  [31:0] wb_IR,
  input  [31:0] wb_Res
);

reg [31:0] Regs [0:31];
wire BEQ, JMP;
wire [4:0] Ri;
wire [4:0] Rj;
wire [4:0] Rk;
wire [5:0] Op;

assign Op = IR[31:26];
assign Ri = IR[25:21];
assign Rj = IR[20:16];
assign Rk = IR[15:11];

wire [5:0]wb_Op;
wire [4:0]wb_Ri;

assign wb_Op = wb_IR[31:26];
assign wb_Ri = wb_IR[25:21];

initial begin
    $readmemh("D:/Arch/Data/reg.txt", Regs);
end

assign BEQ = Op == 6'b100000;
assign JMP = Op == 6'b100001;

assign A = BEQ ? Regs[Ri] ^ Regs[Rj]:
           JMP ? 0:
           Regs[Rj];
assign B = Op[5:4] == 2'b00 ? Regs[Rk] : Regs[Ri];
assign Imm = JMP ?  {{6{IR[25]}}, IR[25:0]}:
                                {{16{IR[15]}}, IR[15:0]};

always @(negedge clk) begin
    if(wb_Ri != 0) begin
        if (wb_Op[5:4] == 2'b00) begin
            Regs[wb_Ri] = wb_Res;
        end else if (wb_Op == 6'b010001) begin // LW
            Regs[wb_Ri] = wb_Res;
        end
    end
end 

endmodule

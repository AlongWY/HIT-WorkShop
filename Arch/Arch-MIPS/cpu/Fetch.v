module Fetch(
  input clk,
  input reset,
  input cond,
  input [31:0]RPC,
  output [31:0]IR,
  output [31:0]NPC
);

reg [31:0] IntMem [0:255];
reg [31:0] PC;
reg [31:0] LIR;
reg [31:0] NIR;
wire lock;

assign NPC = PC + 1;
//assign NIR = IntMem[PC];

assign lock = ((LIR[31:26] == 6'b010001 && NIR[31:30] == 2'b00     && (LIR[25:21] == NIR[20:16] || LIR[25:21] == NIR[15:11])) || // Load --> RR-Alu 
               (LIR[31:26] == 6'b010001 && NIR[31:26] == 6'b010001 &&  LIR[25:21] == NIR[20:16])||                               // Load --> Load
               (LIR[31:26] == 6'b010001 && NIR[31:26] == 6'b010000 && (LIR[25:21] == NIR[25:21])))? LIR != 0 : 0;                // Load --> Store

assign IR = lock ? 0 : NIR;

initial begin
    PC = 0;
	 LIR = 0;
	 NIR = 0;
    $readmemh("D:/Arch/Data/int.txt", IntMem);
end

always @(posedge clk) begin
    if (reset) begin
        PC = 0;
    end else begin
        PC = lock ? PC : cond == 1 ? RPC : NPC;
    end
	 LIR = IR;
	 NIR = lock ? IntMem[PC] : cond == 1 ? IntMem[RPC] : IntMem[NPC];
end

endmodule
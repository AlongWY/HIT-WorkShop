module Memory(
  input clk,
  input [31:0] IR,
  input [31:0] Res,
  input [31:0] Data,
  output [31:0] LMD
);

wire [5:0] Op;
reg [31:0] DatMem [0:255];
assign Op = IR[31:26];
assign LMD = DatMem[Res];

initial begin
    $readmemh("D:/Arch/Data/data.txt", DatMem);
end

always@(posedge clk) begin
  if (Op == 6'b010000) begin
    DatMem[Res] = Data;
  end
end

endmodule
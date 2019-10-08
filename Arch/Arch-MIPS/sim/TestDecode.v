module TestDecode();

reg clk_decode;
reg clk_write;
reg [31:0]IR;
reg [31:0]Data;

wire [5:0]Op;
wire [31:0]A;
wire [31:0]B;
wire [31:0]Imm;

initial
begin
    clk_decode = 1;
	 clk_write = 1;
    IR = 32'h40228001;
    Data = 32'h00000001;
end

Decode decode(clk_decode,clk_write,IR,Data,Op,A,B,Imm);

always
begin
#2 clk_decode =  ~clk_decode;
#2 clk_write =  ~clk_write;
end

endmodule
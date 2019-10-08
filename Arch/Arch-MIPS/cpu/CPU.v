module CPU(
    input clk,
    input reset,
    output [31:0] WB_Reg
    );
	 
    /*----------------------------------------
     *              IF--ID                   *
     *---------------------------------------*/
        
    reg [31:0] IF_ID_IR;
    reg [31:0] IF_ID_NPC;

    /*----------------------------------------
     *              ID--EX                   *
     *---------------------------------------*/
        
    reg [31:0] ID_EX_IR;
    reg [31:0] ID_EX_NPC;
    reg [31:0] ID_EX_A;
    reg [31:0] ID_EX_B;
    reg [31:0] ID_EX_Imm;

    /*----------------------------------------
     *              EX--MEM                  *
     *---------------------------------------*/
        
    reg [31:0] EX_MEM_IR;
    reg [31:0] EX_MEM_B;
    reg [31:0] EX_MEM_Res;
    reg EX_MEM_Cond;

    /*----------------------------------------
     *              MEM-WB                   *
     *---------------------------------------*/
        
    reg [31:0] MEM_WB_IR;
    reg [31:0] MEM_WB_ALU_Res;
    reg [31:0] MEM_WB_LMD;
    reg [31:0] MEM_WB_D;

    initial begin
        IF_ID_IR = 0;
        IF_ID_NPC = 0;
        
        ID_EX_IR = 0;
        ID_EX_NPC = 0;
        ID_EX_A = 0;
        ID_EX_B = 0;
        ID_EX_Imm = 0;
        
        EX_MEM_IR = 0;
        EX_MEM_B = 0;
        EX_MEM_Res = 0;
        EX_MEM_Cond = 0;
        
        MEM_WB_IR = 0;
        MEM_WB_ALU_Res = 0;
        MEM_WB_LMD = 0;
        MEM_WB_D = 0;
    end
    
    wire [31:0] F_IR, F_JPC, F_NPC;
    wire [31:0] D_IR, D_NPC, D_A, D_B, D_Imm;
    wire [31:0] E_IR, NPC_E, E_A, E_B, E_Imm, E_ALU_Res;
    wire [31:0] M_IR, M_ALU_Res, M_B, M_LMD;
    wire [31:0] W_IR, W_LMD, W_ALU_Res;
    wire F_JMP_Cond, E_Cond;
        
    always @ (posedge clk) begin
        IF_ID_IR = F_IR;
        IF_ID_NPC = F_NPC;

        ID_EX_IR = D_IR;
        ID_EX_NPC = D_NPC;
        ID_EX_A = D_A;
        ID_EX_B = D_B;
        ID_EX_Imm = D_Imm;

        EX_MEM_IR = E_IR;
        EX_MEM_B = E_B;
        EX_MEM_Res = E_ALU_Res;
        EX_MEM_Cond = E_Cond;

        MEM_WB_IR = M_IR;
        MEM_WB_ALU_Res = M_ALU_Res;
        MEM_WB_LMD = M_LMD;
    end
        
    assign D_IR = IF_ID_IR;
    assign D_NPC = IF_ID_NPC;
        
    assign E_IR = ID_EX_IR;
    assign NPC_E = ID_EX_NPC;

    assign E_A = M_IR[31:30] == 0         && E_IR[31:30] == 0           && M_IR[25:21] == E_IR[20:16] ? M_ALU_Res :  // RR-ALU --> RR-ALU:Rj
                 M_IR[31:30] == 0         && E_IR[31:26] == 6'b010001   && M_IR[25:21] == E_IR[20:16] ? M_ALU_Res :  // RR-ALU --> Load:Rj
                 W_IR[31:30] == 0         && E_IR[31:30] == 0           && W_IR[25:21] == E_IR[20:16] ? W_ALU_Res :  // RR-ALU --> RR-ALU:Rj
                 W_IR[31:30] == 0         && E_IR[31:26] == 6'b010001   && W_IR[25:21] == E_IR[20:16] ? W_ALU_Res :  // RR-ALU --> Load:Rj
                 W_IR[31:26] == 6'b010001 && W_IR[25:21] == E_IR[20:16] ? W_LMD :                                    // Load --> RR-ALU:Rj
                 ID_EX_A;                                                                                            // Default

    assign E_B = M_IR[31:30] == 0         && E_IR[31:30] == 0         && M_IR[25:21] == E_IR[15:11] ? M_ALU_Res : // RR-ALU --> RR-ALU:Rk
                 W_IR[31:30] == 0         && E_IR[31:30] == 0         && W_IR[25:21] == E_IR[15:11] ? W_ALU_Res : // RR-ALU --> RR-ALU:Rk
                 W_IR[31:26] == 6'b010001 && E_IR[31:30] == 0         && W_IR[25:21] == E_IR[15:11] ? W_LMD :     // Load --> RR-ALU:Rk
                 W_IR[31:26] == 6'b010001 && E_IR[31:26] == 6'b010000 && W_IR[25:21] == E_IR[20:16] ? W_LMD :     // Load --> Stroe:Ri(B)
                 ID_EX_B;                                                                                         // Default
                     
    assign E_Imm = ID_EX_Imm;
        
    assign M_IR = EX_MEM_IR;
    assign M_ALU_Res = EX_MEM_Res;
    assign M_B = EX_MEM_B;
    assign F_JMP_Cond = EX_MEM_Cond;
    assign F_JPC = EX_MEM_Res;
        
    assign W_IR = MEM_WB_IR;
    assign W_ALU_Res = MEM_WB_ALU_Res;
    assign W_LMD = MEM_WB_LMD;
  
    Fetch fetch(clk,reset,F_JMP_Cond,F_JPC,F_IR,F_NPC);
    Decode decode(clk,D_IR,D_A,D_B,D_Imm,W_IR,WB_Reg);
    Execute execute(E_IR,NPC_E,E_A,E_B,E_Imm,E_Cond,E_ALU_Res);
    Memory memory(clk,M_IR,M_ALU_Res,M_B,M_LMD);
    Write write(W_IR,W_LMD,W_ALU_Res,WB_Reg);
endmodule

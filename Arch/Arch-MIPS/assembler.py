#!/usr/bin/env python3
# coding: utf-8

import re

OP_R = {'ADD': '000000', 'SUB': '0000001',
        'AND': '000010', 'OR': '000011',
        'XOR': '000100', 'SLT': '000101'}
OP_I = {'SW': '010000', 'LW': '010001'}
OP_J = {'BEQ': '100000', 'JMP': '100001'}


def parse(cmd: str, pc: int = 0):
    global Binary
    cmds = re.split(r'[,\s]\s*', cmd.upper())
    op = cmds.pop(0)
    if op in OP_R:
        Ri = int(re.findall(r"\d+\.?\d*", cmds[0])[0])
        Rj = int(re.findall(r"\d+\.?\d*", cmds[1])[0])
        Rk = int(re.findall(r"\d+\.?\d*", cmds[2])[0])
        Binary = OP_R[op] + '{:05b}{:05b}{:05b}{:011b}'.format(Ri, Rj, Rk, 0)
    elif op in OP_I:
        Ri = int(re.findall(r"\d+\.?\d*", cmds[0])[0])
        R_IMM = re.findall(r"\d+\.?\d*", cmds[1])
        try:
            Rj, Imm = int(R_IMM[0]), int(R_IMM[1])
        except Exception:
            Rj, Imm = int(re.findall(
                r"\d+\.?\d*", cmds[1])[0]), int(re.findall(r"\d+\.?\d*", cmds[2])[0])
        Binary = OP_I[op] + '{:05b}{:05b}{:016b}'.format(Ri, Rj, Imm)
    elif op in OP_J:
        if op == 'BEQ':
            Ri = int(re.findall(r"\d+\.?\d*", cmds[0])[0])
            Rj = int(re.findall(r"\d+\.?\d*", cmds[1])[0])
            Imm = int(re.findall(r"\d+\.?\d*", cmds[2])[0])
            NPC = '{:016b}'.format(Imm - (pc + 1)).replace('-0', '1')
            Binary = OP_J[op] + '{:05b}{:05b}{}'.format(Ri, Rj, NPC)
        else:
            Imm = int(re.findall(r"\d+\.?\d*", cmds[0])[0])
            Binary = OP_J[op] + '{:026b}'.format(Imm)
    return '{:08x}'.format(int(Binary, 2))


if __name__ == '__main__':
    CMD = '''
        LW 	R10 R11 1
        ADD R12 R13 R14
    '''
    CMDs = [x.strip() for x in CMD.splitlines() if x.strip() != '']
    CMD_Binary = ""
    for index, cmd in enumerate(CMDs):
        Binary = parse(cmd, index)
        print("{:<5} {:<20} {:<30}".format(index, cmd, Binary))
        CMD_Binary += Binary + '\n'
    print(CMD_Binary)

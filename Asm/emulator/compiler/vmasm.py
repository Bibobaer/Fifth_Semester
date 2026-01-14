import re
import struct
import sys

opcodes = {}
counts_operand = {}
optypes = {}
count_opcode = 0
unused_operand = b'\x00' * 5

def init_tables():
    global count_opcode
    
    optypes["none"] = b'\x00'
    optypes["regb"] = b'\x01'
    optypes["regw"] = b'\x02'
    optypes["immed"] = b'\x03'
    optypes["memb"] = b'\x04'
    optypes["memw"] = b'\x05'
    optypes["memrb"] = b'\x06'
    optypes["memrw"] = b'\x07'

    opcodes["nop"] = b'\x00'
    counts_operand["nop"] = 0
    opcodes["in"] = b'\x01'
    counts_operand["in"] = 1
    opcodes["out"] = b'\x02'
    counts_operand["out"] = 1
    opcodes["mov"] = b'\x03'
    counts_operand["mov"] = 2
    opcodes["add"] = b'\x04'
    counts_operand["add"] = 2
    opcodes["sub"] = b'\x05'
    counts_operand["sub"] = 2
    opcodes["xor"] = b'\x06'
    counts_operand["xor"] = 2
    opcodes["and"] = b'\x07'
    counts_operand["and"] = 2
    opcodes["or"] = b'\x08'
    counts_operand["or"] = 2
    opcodes["hlt"] = b'\x09'
    counts_operand["hlt"] = 0
    opcodes["push"] = b'\x0A'
    counts_operand["push"] = 1
    opcodes["pop"] = b'\x0B'
    counts_operand["pop"] = 1
    opcodes["call"] = b'\x0C'
    counts_operand["call"] = 1
    opcodes["ret"] = b'\x0D'
    counts_operand["ret"] = 0
    opcodes["cmp"] = b'\x0E'
    counts_operand["cmp"] = 2
    opcodes["je"] = b'\x0F'
    counts_operand["je"] = 1
    opcodes["jne"] = b'\x10'
    counts_operand["jne"] = 1

    count_opcode = 17

def parse_operand(op):
    if op is None:
        print("undefined operand", file=sys.stderr)
        return None

    if re.match(r'^[0-9]+$', op):
        return optypes["immed"] + struct.pack("<L", int(op))
    
    elif re.match(r'^0x([a-fA-F0-9]+)$', op, re.IGNORECASE):
        hex_match = re.match(r'^0x([a-fA-F0-9]+)$', op, re.IGNORECASE)
        hex_val = hex_match.group(1)
        hex_val = hex_val.zfill(8)
        return optypes["immed"] + bytes.fromhex(hex_val)[::-1]
    
    elif re.match(r'^(byte )* *r([0-9]+)$', op):
        match = re.match(r'^(byte )* *r([0-9]+)$', op)
        reg_num = int(match.group(2))
        if match.group(1) is not None:
            return optypes["regb"] + struct.pack("<L", reg_num)
        else:
            return optypes["regw"] + struct.pack("<L", reg_num)
    
    elif re.match(r'^(byte )* *\[0x([a-fA-F0-9]+)\]$', op):
        match = re.match(r'^(byte )* *\[0x([a-fA-F0-9]+)\]$', op)
        hex_val = match.group(2)
        hex_val = hex_val.zfill(8)
        if match.group(1) is not None:
            return optypes["memb"] + bytes.fromhex(hex_val)[::-1]
        else:
            return optypes["memw"] + bytes.fromhex(hex_val)[::-1]
    
    elif re.match(r'^(byte )* *\[r([0-9]+)\]$', op):
        match = re.match(r'^(byte )* *\[r([0-9]+)\]$', op)
        reg_num = int(match.group(2))
        if match.group(1) is not None:
            return optypes["memrb"] + struct.pack("<L", reg_num)
        else:
            return optypes["memrw"] + struct.pack("<L", reg_num)

    print(f"error in operand {op}", file=sys.stderr)
    return None

def parse_line(line):
    line = line.strip()
    if not line:
        return None

    match = re.match(r'^ *([a-z]{2,4}) *(.*) *$', line)
    if not match:
        print(f"error in line {line}", file=sys.stderr)
        return None

    mnem = match.group(1)
    ops = match.group(2)

    if mnem not in opcodes:
        print(f"error in mnemonic {mnem}", file=sys.stderr)
        return None

    operand_count = counts_operand[mnem]

    if operand_count == 0:
        return opcodes[mnem] + unused_operand + unused_operand
    
    elif operand_count == 1:
        op1 = parse_operand(ops)
        if op1 is None:
            print(f"request operand in {ops}", file=sys.stderr)
            return None
        return opcodes[mnem] + op1 + unused_operand
    
    elif operand_count == 2:
        match_ops = re.match(r'^([0-9a-z \[\]x]+), *([0-9a-z \[\]x]+)$', ops, re.IGNORECASE)
        if not match_ops:
            print(f"error in operands {ops}", file=sys.stderr)
            return None

        op1 = match_ops.group(1)
        op2 = match_ops.group(2)

        op1_parsed = parse_operand(op1)
        if op1_parsed is None:
            print(f"request operand 1 in {op1}", file=sys.stderr)
            return None

        op2_parsed = parse_operand(op2)
        if op2_parsed is None:
            print(f"request operand 2 in {op2}", file=sys.stderr)
            return None

        return opcodes[mnem] + op1_parsed + op2_parsed

    print("unknown error", file=sys.stderr)
    return None

def parse_input_code(input_file=None):
    if input_file is None:
        input_file = sys.stdin
    elif isinstance(input_file, str):
        input_file = open(input_file, 'r')

    result = b''
    
    try:
        for line in input_file:
            line = line.strip()
            if not line:
                continue
            
            line_code = parse_line(line)
            if line_code is None:
                return None
            
            result += line_code
    finally:
        if input_file != sys.stdin:
            input_file.close()

    return result

init_tables()

__all__ = ['opcodes', 'counts_operand', 'optypes', 'count_opcode', 'unused_operand', 
           'parse_line', 'parse_input_code']

if __name__ == "__main__":
    result = parse_input_code()
    if result:
        sys.stdout.buffer.write(result)
import sys
import vmasm

input_file = sys.argv[1] if len(sys.argv) > 1 else None
output_file = sys.argv[2] if len(sys.argv) > 2 else None

if input_file:
    try:
        in_f = open(input_file, 'r')
    except IOError as e:
        print(f"error open input file: {e}", file=sys.stderr)
        sys.exit(1)
else:
    in_f = sys.stdin

if output_file:
    try:
        out_f = open(output_file, 'wb')
    except IOError as e:
        print(f"error open output file: {e}", file=sys.stderr)
        sys.exit(1)
else:
    out_f = sys.stdout

try:
    result = vmasm.parse_input_code(in_f)
    if result is None:
        print("Error during assembly", file=sys.stderr)
        sys.exit(1)
    
    if hasattr(out_f, 'buffer'):
        out_f.buffer.write(result)
    else:
        out_f.write(result)
finally:
    if input_file and in_f != sys.stdin:
        in_f.close()
    if output_file and out_f != sys.stdout:
        out_f.close()
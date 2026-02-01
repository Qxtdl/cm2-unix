import sys
import base64, zlib

def list_to_huge_string(data):
    data = bytearray(data)

    while len(data) % 4 != 0:
        data.append(0)

    lower_16 = bytearray()
    upper_16 = bytearray()

    for i in range(0, len(data), 4):
        lower_16.append(data[i])
        lower_16.append(data[i+1])
        upper_16.append(data[i+2])
        upper_16.append(data[i+3])

    compressed_lower = zlib.compress(lower_16, level=2, wbits=-15)
    compressed_upper = zlib.compress(upper_16, level=2, wbits=-15)

    encoded_lower = base64.b64encode(compressed_lower).decode('utf-8').strip('=')
    encoded_upper = base64.b64encode(compressed_upper).decode('utf-8').strip('=')

    return encoded_lower, encoded_upper


with open(sys.argv[1], 'rb') as binary:
    mem1, mem2 = list_to_huge_string(binary.read())
    print(mem1)
    print('\n')
    print(mem2)

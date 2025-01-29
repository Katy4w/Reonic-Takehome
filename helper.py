# copy-pasted from the pdf
charging_needs_raw = """egnar fo sretemolik ni dnamed gnigrahC ytilibaborP
)egrahc t'nseod( enoN % 13.43
mk 0.5 % 09.4
mk 0.01 % 08.9
mk 0.02 % 67.11
mk 0.03 % 28.8
mk 0.05 % 67.11
mk 0.001 % 87.01
mk 0.002 % 09.4
mk 0.003 % 49.2"""
arrival_probabilities_raw = """seitilibaborp lavirrA :1T
lavirra fo ytilibaborP yad eht fo emiT
% 49.0 0010 - 0000
% 49.0 0020 - 0010
% 49.0 0030 - 0020
% 49.0 0040 - 0030
% 49.0 0050 - 0040
% 49.0 0060 - 0050
% 49.0 0070 - 0060
% 49.0 0080 - 0070
% 38.2 0090 - 0080
% 38.2 0001 - 0090
% 66.5 0011 - 0001
% 66.5 0021 - 0011
% 66.5 0031 - 0021
% 55.7 0041 - 0031
% 55.7 0051 - 0041
% 55.7 0061 - 0051
% 83.01 0071 - 0061
% 83.01 0081 - 0071
% 83.01 0091 - 0081
% 27.4 0002 - 0091
% 27.4 0012 - 0002
% 27.4 0022 - 0012
% 49.0 0032 - 0022
% 49.0 0042 - 0032
"""

# parse into lines and words
pr_arrivals = [(x[::-1].split()[3]) for x in arrival_probabilities_raw.splitlines()[2:]]
charging_needs = [x[::-1].split() for x in charging_needs_raw.splitlines()[1:]]

charging_needs[0][2] = "0.0"

pr_charging_needs =  [x[0] for x in charging_needs]
val_charging_needs = [x[2][:len(x[2])-2] for x in charging_needs]

def as_c_array(li):
    return '{\n'+ ', \n'.join(li) +'}'

lengthcn = len(val_charging_needs)
assert(lengthcn == len(pr_charging_needs))

with open('pr_data.c', 'w') as f:
    f.write("double pr_arrivals[24]" + ' = ' + as_c_array(pr_arrivals) + ';\n')
    f.write(f"double pr_charging_needs[{lengthcn}]" + ' = ' + as_c_array(pr_charging_needs) + ';\n')
    f.write(f"unsigned val_charging_needs[{lengthcn}]" + ' = ' + as_c_array(val_charging_needs) + ';\n')
    f.write(f"size_t len_charging_needs = {lengthcn};\n")

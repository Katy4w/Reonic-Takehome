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

arrival_probabilities = [float(x[::-1].split()[3]) for x in arrival_probabilities_raw.splitlines()[2:]]

charging_needs = [x[::-1].split() for x in charging_needs_raw.splitlines()[1:]]
charging_needs[0][2] = "0"
charging_needs = [(float(x[0]), int(float(x[2]))) for x in charging_needs]
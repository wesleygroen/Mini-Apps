#!/usr/bin/env python
# from math import mean
# import statistics
from numpy import std, mean

# f = open('out4.txt')
# lines = f.readlines()
# n = []
# t = []
# d = {}
# for line in lines:
#     s = line.strip().split(' ')
#     if (len(s)>1):
#         num = float(s[1])
#         t.append(num)
#         x = int(s[0])
#         if (not(x in n)):
#             n.append(x)
#             d[x]=[num]
#         else:
#             d[x].append(num)
# # for key in d.keys():
# print(d)
# k = d.keys()
# k.sort()
# print(k)
# # print(statistics.mean(t))
# for key in k:
#     d[key]=mean(d[key])
#     print("{} {}".format(key, d[key]))
# print("mean: {} std: {}".format(mean(t),  std(t)))

f = open('w2.txt')
lines = f.readlines()
n = []
t = []
d = {}
for line in lines:
    num = float(line)
    t.append(num)
    # s = line.strip().split(' ')
    # if (len(s)>1):
        # x = int(s[0])
        # if (not(x in n)):
        #     n.append(x)
        #     d[x]=[num]
        # else:
        #     d[x].append(num)
# for key in d.keys():
# print(d)
# k = d.keys()
# k.sort()
# print(k)
print(mean(t), std(t), min(t), max(t))
# for key in k:
#     d[key]=mean(d[key])
#     print("{} {}".format(key, d[key]))
# print("mean: {} std: {}".format(mean(t),  std(t)))
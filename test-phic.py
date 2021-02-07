import os
import random

random.seed(1)
tp = 1e-2 # chance to switch states
ep = [(0,2), (2, 4), (4, 8)]

# generate random file
s = random.randint(0, 2)
values = []
states = []
beg = 0
for i in range(1000000): # 1 million values
	values.append(random.randint(ep[s][0], ep[s][1]))
	states.append(s)
	if random.random() < tp:
		beg = i
		s = random.randint(0, 2)

with open('testing.txt', 'w') as fp:
	for v in values:
		fp.write(f'{v}\n')

# decode
os.system('python3 phic.py --file testing.txt --x 3 --classes 1 3 5 > testing.out')

# evaluate
output = []
with open('testing.out') as fp:
	for line in fp.readlines():
		beg, end, state, ave = line.split()
		beg = int(beg)
		end = int(end)
		for i in range(beg-1, end):
			if   state == '1': output.append(0)
			elif state == '3': output.append(1)
			else:              output.append(2)

match = 0
for r, p in zip(states, output):
	if r == p: match += 1
print(f'{match} / {len(states)} = {match/len(states)}')

os.system('rm testing.out testing.txt')

# 0.997227 accuracy
# 6.4s 2017 MacBook Air
# 3.5s 2020 Lenovo IdeaPad 3 running Lubuntu in VirtualBox on Windows
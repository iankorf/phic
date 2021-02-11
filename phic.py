import argparse
import os
import random
import statistics
import sys

if __name__ == '__main__':

	ifile = 'tmp.phic.txt'
	ofile = 'tmp.phic.out'

	parser = argparse.ArgumentParser(
		description='Poission-based HMM Integer Classifier')
	parser.add_argument('--file', required=True, type=str,
		metavar='<file>', help='text file of values')
	parser.add_argument('--classes', required=True, type=int, nargs='*',
		metavar='<int>', help='at least 2 integers that define classes')
	parser.add_argument('--names', required=False, type=str, nargs='*',
		metavar='<str>', help='names corresponding to integers')
	parser.add_argument('--background', required=False, type=str,
		metavar='<file>', help='text file of background values')
	parser.add_argument('--x', required=False, type=int, default=5,
		metavar='<file>', help='switch cost (in log-space) [%(default)i]')
	parser.add_argument('--min', required=False, type=int, default=0,
		metavar='<int>', help='limit minimum value [%(default)i]')
	parser.add_argument('--max', required=False, type=int, default=255,
		metavar='<int>', help='limit maximum [%(default)i]')
	parser.add_argument('--scale', required=False, type=float,
		metavar='<float>', help='scale all values')
	parser.add_argument('--window', required=False, type=int,
		metavar='<int>', help='aggregate values into windows of <int> size')
	parser.add_argument('--gff', required=False, type=str,
		metavar='<str>', help='format as GFF (chr,off)')
	parser.add_argument('--bed', required=False, type=str,
		metavar='<str>', help='format as BED (chr,off)')
	parser.add_argument('--keep', action='store_true',
		help=f'keep temporary files ({ifile}, {ofile})')
	arg = parser.parse_args()
	
	if len(arg.classes) < 2: raise Exception('need at least 2 classes')
	for n in arg.classes:
		if n < 1:    raise Exception('minimum class value exceeded (1)')
		if n >= 255: raise Exception('maximum class value exceeded (255)')
	
	if arg.bed and arg.gff:
		raise Exception('choose --bed or --gff or leave blank')
	if arg.gff:
		chr, off = arg.gff.split(',')
		off = int(off)
	elif arg.bed:
		chr, off = arg.bed.split(',')
		off = int(off)
	
	if arg.background: raise Exception('background not implemented yet')
	
	name = {}
	if arg.names:
		if len(arg.names) != len(arg.classes):
			raise Exception('must be same number of names and classes')
		for i in range(len(arg.classes)):
			name[arg.classes[i]] = arg.names[i]
	
	# collect data and threshold/scale as necessary
	data = []
	raw = []
	window = []
	with open(arg.file) as fp:
		while True:
			line = fp.readline()
			if line == '': break
			val = float(line)
			if arg.scale: val *= arg.scale
			if   val < arg.min: val = arg.min
			elif val > arg.max: val = arg.max
			if arg.window:
				window.append(val)
				if len(window) == arg.window:
					val = round(statistics.mean(window))
					data.append(val)
					raw.append(val)
					window = []
			else:
				data.append(round(val))
				raw.append(val)
	
	# create temporary counts file
	with open(ifile, 'w') as fp:
		for d in data:
			fp.write(f'{d}\n')

	# run phic and create temporary output file
	n = len(data)
	classes = [str(c) for c in arg.classes]
	os.system(f'phic {ifile} {arg.x} {" ".join(classes)} > {ofile}')
	
	# format output
	output = []
	with open(ofile) as fp:
		for line in fp.readlines():
			beg, end, state = line.split()
			output.append((int(beg), int(end), int(state)))
	
	for b, e, s in reversed(output):
		r = statistics.mean(raw[b-1:e])
		d = statistics.mean(data[b-1:e])
		if arg.names: s = name[s]
		if arg.window:
			b = b * arg.window - arg.window + 1
			e *= arg.window
		
		if arg.gff:
			beg = str(b + off)
			end = str(e + off)
			print(f'{chr}\t phic\t{s}\t{beg}\t{end}\t.\t.\t.\t{r:.3f} {d:.3f}')
		elif arg.bed:
			beg = str(b + off)
			end = str(e + off)
			print(f'{chr}\t{beg}\t{end}\t{s}')
		else:
			print(f'{b}\t{e}\t{s}\t{d:.3f}')

	# clean up
	if not arg.keep:
		os.system(f'rm {ifile} {ofile}')




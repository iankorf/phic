PHIC
====

Poisson-based HMM Integer Classifier

## Introduction ##

PHIC is designed to solve a simple recurring problem in bioinformatics.
Sequencing read depth is often used to infer _something_ from
copy-number variants to R-loops, to ChIP-seq peaks. PHIC reads the
sequence of read depths and classifies these into any number of numeric
classes. For example, most of the genome may be 1x coverage, but some
places are 5x, and others are 50x. PHIC can classify coverage into 3
classes of 1, 5, and 50.

PHIC assumes input data are integer values within a limited range. Read
depth files are sometimes normalized into floating point values, and
some values may be out of range. For this reason, there is a python
wrapper `phic.py` that performs a variety of pre- and post-processing
activities on the raw data. It is generally better to use `phic.py` than
`phic` itself.

## Build Instructions ##

In order to compile PHIC you must first build the ik library.

	git clone https://github.com/iankorf/ik.git
	cd ik
	make

Place the ik directory at the same level as the phic directory. This is
necessary because phic references the parent directory to get to ik.

Now you can go to the phic directory and build that too.

	git clone https://github.com/iankorf/phic.git
	cd phic
	make

## Tutorial ##

There is a sample file of integers called `values.txt`. Try running the
programs with that.

	python3 phic.py --file values.txt --x 5 --classes 1 5 20
	phic values.txt 5 1 5 20

There is also `test-phic.py` that creates a random file of 1 million
integers and decodes with `phic.py`.

## HMM Details ##

The PHIC HMM is fully connected, meaning that you can get from any state
to any other state. The transition probabilities are the same for each
state. The chance to leave a state is called X. The chance to stay in a
state should therefore be 1.0 - kX, where k is the number of states. But
since X is generally small, the self-returning state is near 1.0. So,
for simplicity, the self-returning probability is 1.0 and the transition
probabilities don't quite sum to 1.0.

Unlike most HMMs which have tables of discrete emission probabilities,
the emissions in PHIC are drawn from a Poisson distribution. The
emissions are the number of expected counts.


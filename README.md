PHIC
====

Poisson-based HMM Integer Classifier

## Introduction ##

PHIC is designed to solve a simple recurring problem in bioinformatics.
Sequencing read depth is often used to infer _something_ from
copy-number variants to R-loops, to ChIP-seq peaks. PHIC reads the
sequence of read depths and classifies these into any number of numeric
classes. For example, most of the genome may be 0-1x coverage, but some
places are 4-6x, and others are 50 or more. PHIC can classify coverage
into 3 classes of 1, 5, and 50.

PHIC assumes input data are integer values within a limited range
(0-255). Read depth files are sometimes normalized into floating point
values, and some values may be out of range. For this reason, there is a
python wrapper `phic.py` that performs a variety of pre- and
post-processing activities on the raw data. It is generally better to
use `phic.py` than `phic` itself.

## Build Instructions ##

In order to compile `phic` you must first build the `genomikon.a` library.

	git clone https://github.com/KorfLab/genomikon.git
	cd ik
	make

Place the `genomikon` directory at the same level as the `phic` directory. That
is, there should be a parent directory containing both the `genomikon` and
`phic` directories. This is necessary because the phic `Makefile`
references the parent directory to get to `genomikon`.

After you've build `genomikon.a` you can go to the `phic` directory and
build the `phic` executable.

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
since X is generally small, the self-returning transition probability is
near 1.0. So, for simplicity, and because I don't give a phuck, the
self-returning probability is 1.0 and the transition probabilities don't
quite sum to 1.0.

Unlike most HMMs which have tables of discrete emission probabilities,
the emissions in PHIC are drawn from a Poisson distribution. The
emissions are the number of expected counts. All of the calculations are
done in log space, so long sequences of numbers should be fine.


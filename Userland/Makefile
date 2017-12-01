include Makefile.inc

all: sampleCodeModule sampleDataModule

sampleCodeModule:
	cd SampleCodeModule; make

sampleDataModule:
	cd SampleDataModule; make

clean:
	cd SampleCodeModule; make clean
	rm -rf *.bin

.PHONY: sampleCodeModule all clean

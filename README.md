# Burst

The `burst` algorithm is a data-driven and statistically rigorous method to detect and isolate signal bursts in a time series. It is statistically rigorous in the sense that the false-positive and false-negative rates in detecting signal bursts are known, as defined by the user. For example, the algorithm can be used for counting molecules in a single-molecule diffusion experiment. Furthermore, the algorithm isolates signal bursts conservatively to minimize taking in background data points, allowing additional in-depth analysis of information content contained in the data points collected in each signal burst. 

## Getting started

The algorithm is implemented in C. To compile the source code, use the standard C-code compile command, e.g.,

> make

Then under the command-line environment, type

> burst

which will print out instructions. An example, simulated trajectory is included in the /examples/ folder. Output files have extensions with "bst" and ".bak" attached to the input file name and correspond to data-intensity burst regions and background regions, respectively. See the source code for details and output format.

The code has been tested in both Linux (Ubuntu 20.04 LTS) and Mac OSX (macOS Big Sur 11.4 command line) environments.

## References

If you use the `burst` algorithm in your research, please cite:

Kai Zhang and Haw Yang, "Photon-by-Photon Determination of Emission Bursts from Diffusing Single Chromophores," [_J. Phys. Chem. B_, 109, 21930-21937 (2005)](http://dx.doi.org/10.1021/jp0546047).

## License

The `burst` source code is released under a Clear BSD License and is intended for research/academic use only. For commercial use, please contact: Laurie Tzodikov (Assistant Director, Office of Technology Licensing), Princeton University, 609-258-7256.

## Contributors

* Haw Yang, Chemistry Department, Princeton University

The current `burst` is developed in the Yang lab at the Chemistry Department at Princeton University, based on an initial work done at UC Berkeley. It was made possible through funding by Princeton University, the Laboratory Directed Research and Development Program of Lawrence Berkeley National Laboratory under U.S. Department of Energy contract No. DE-AC03-76SF00098, and the University of California at Berkeley.

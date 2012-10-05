About MeSA 
===========

What does it do?
----------------

MeSA is a program for exploring phylogenies and wide expanses of evolutionary history, to analyse evolutionary tempo, mode and trends. To this end it offers tools and functions for:

* analysing the shape, topology and other metrics of phylogenetic trees

* simulating ("growing") trees under complex evolutionary scenarios

* pruning or otherwise manipulating trees, in particular to simulate the results of extinction, conservation schemes or other taxa loss

* repetition and pipelining of the above functions over a population of trees, or over the same tree repeatedly, for statistical purposes 

The following papers give examples of its use: [agapow2005]_  [purvis2002a]_ [purvis2000b]_  [purvis2000]_.


Caveats and limitations
-----------------------

* MeSA does a lot. It is very powerful but conversely can be overwhelming.

* I have tried to make many difficult things easy. But sometimes this has been at the cost of making simpler things harder. This is a conscious trade-off.

* MeSA does not do summary statistics, in the sense of condensing a series of analysis results to an average or confidence limits. There are an infinite number of possible statistics which are best left to a proper statistics program like R. Instead, MeSA concentrates on providing datasets that can be easily analyzed by such programs.

* It does not have a user-friendly GUI interface, but a menu-driven commandline one.

* Documentation is still an ongoing work.

* As it has been recently ported from an old architecture, some bugs may have been introduced.

* MeSA is currently distributed only as source code that requires compiling on a Unix-like system (including OSX), although it is possible to make it to run on Windows and other systems.

* The simulation environment involves some simplifications and assumptions for efficient execution. These are noted later in the appropriate section.

* MeSA allows for non-bifurcating nodes, polytomies and singleton nodes (internal nodes with a single descendant). However it does not handle tree nets or meshes, i.e. situations in which previously seperate taxa merge.

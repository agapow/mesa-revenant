MeSA, menu by menu
==================


The initial File menu
---------------------

When you run MeSA, you'll always be confronted with this menu::

	File Menu:
	   o> Open a Nexus or Caic file
	   n> Create a new tree & data file
	   t> Distill results file
	   x> Experimental
	   q> Quit
	What is your command?                                          :


Open a Nexus or Caic file
~~~~~~~~~~~~~~~~~~~~~~~~~

MeSA can read in data from Nexus (PAUP) and CAIC (or MacroCAIC) files. File type is detected from the file extension (``nex``, ``nexus``, ``nxs`` for Nexus and ``blen`` for CAIC). Note that due to the wide variety of data that is stored in Nexus, not all possible variants can be read. In particular, the trees in BEAST output files are too heavily annotated for MeSA to parse.

Example files are included with this manual.


Create a new tree & data file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This creates a new, empty tree and saves it to disk. This is usually used in preparation for simulation.


Distill results file
~~~~~~~~~~~~~~~~~~~~~

Take a complicated results file and reduce it to columns of interest.


Experimental
~~~~~~~~~~~~

Don't go here.


Quit
~~~~

End the program and flush all data. Make sure you're saved before you do this.



File menu after a file has been opened
---------------------------------------

::
   s> Save the current file
   w> Close the current file
   d> Data Manipulation Menu ...
   a> Instant Analysis Menu ...
   g> Program and Run Queue Menu ...
   i> Summarise file contents
   t> Distill results file
   f> Preferences Menu ...


Save the current file
~~~~~~~~~~~~~~~~~~~~~

Save the current data as a Nexus file.


Close the current file
~~~~~~~~~~~~~~~~~~~~~~

Flush all the current data and return to the state of the program when it first opened.


Data Manipulation Menu
~~~~~~~~~~~~~~~~~~~~~~

Transform the data in a variety of ways. See the relevant section below.


Instant Analysis Menu
~~~~~~~~~~~~~~~~~~~~~

Non-destructively analyse the data in a variety of ways. See the relevant section below.


Program and Run Queue Menu
~~~~~~~~~~~~~~~~~~~~~~~~~~

Run data analysis and manipulations across trees, including tree growth simulation. See the relevant section below.


Summarise file contents
~~~~~~~~~~~~~~~~~~~~~~~

Give a quick textual summary of what data you have loaded in memory: how many trees, the type of trait data etc. The oputput looks like::

	There is no discrete data.
	There is no continuous data.
	There is 1 tree.
	The active tree has 18 nodes (10 leaves, 10 alive)
	There are no actions programmed in the queue.


Preferences Menu
~~~~~~~~~~~~~~~~

Set various operational parameters of MeSA. See the relevant section below.


Data Manipulation Menu
----------------------

   2> Duplicate the active tree
   4> Add a new trait
   6> Set branchlengths
   9> Delete dead taxa
   11> prune (fixed number of taxa)
   12> prune (fixed fraction of taxa)
   13> prune (fixed prob for each taxa)
   15> prune (if taxa meets condition)
   16> Collapse singletons
   17> Summarise file contents
   18> Report on the file contents
   r> Return to main menu


Instant Analysis Menu
---------------------

This allows you to quickly get a result or metric calculated about a tree. Note that nothing in this menu will change any of the data.

   s> Set saving of analyses
   1> Calculate number of extant taxa
   2> Calculate total number of taxa
   3> Calculate genetic diversity
   4> Calculate phylogenetic diversity
   13> Calculate tree information
   14> Calculate information over nodes of tree
   15> Calculate information over nodes (exp)
   16> Calculate Fusco imbalance
   17> Calculate Slowinski-Guyer imbalance
   18> Calculate Shao's N-bar imbalance
   19> Calculate Shao's Sigma-squared imbalance
   20> Calculate Colless' C imbalance
   21> Calculate Shao's B1 balance
   22> Calculate Shao's B2 balance
   23> Calculate stemminess
   24> Calculate resolution
   25> Calculate ultrametricity
   r> Return to main menu


Set saving of analyses
~~~~~~~~~~~~~~~~~~~~~~

Determine how analysis results are displayed or saved. By default they are sent to the screen.


Calculate number of extant taxa
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

How many living tips does the tree have? This will be like the total number of tips, except where extinction has been simulated and taxa have died.


Calculate total number of taxa
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

How many taxa does the tree have? This will be like the number of extant taxa, except where extinction has been simulated and taxa have died.


Calculate genetic diversity
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Calculate GD after the manner of Crozier [REF]__. Note that this requires branch lengths to be allelic (i.e. expressed as a probability of alleles differing on either end of a branch).


Calculate phylogenetic diversity
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Calculate PD after the manner of Faith [REF]__ and May/Nee [REF]__. Note that this is rooted PD.


Calculate tree information
~~~~~~~~~~~~~~~~~~~~~~~~~~

Calculate a series of simple metrics for a tree. This can include the number of nodes, number of leaves (tips), the number of extant taxa, whether the tree is neoontological or paleontological and the tree's phylogenetic age (distance from fartherest tip to root).


Calculate information over nodes of tree
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Calculate simple information for all nodes of the tree, or just the leaves and internal nodes. This can include the ndoe age, the time (branchlength) to parent, the number of immediate children, the number of leaves (tips) ultimately subtended, the size of the total subtree subtended, the number of sibling nodes, the height of the node (i.e. the distance to the nearest subtended tip) and the time to root (i.e. the total distance to the root of the tree).


Calculate information over nodes
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This is an experiemental variant of the above analysis, that allows for a more sophisticated selection of nodes, based on trait value, whether the node is extant or how many tips it subtends.

Calculate Fusco imbalance
~~~~~~~~~~~~~~~~~~~~~~~~~

Calculate Fusco's I metric of tree imbalance for each applicable node in the tree [REF]__. (This metric may only be calcuated over bifurcating nodes.) The analysis may optionally use a column of continuous data as species richness, giving how many taxa each tip node actually represents. It may also return the size of the node with each result and use the extended I' calculation (see [fusco_mod]__).


Calculate Slowinski-Guyer imbalance
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Calculate Slowinski-Guyer's metric of imbalance for each applicable node in the tree [REF]__. (This metric may only be calcuated over bifurcating nodes.) The analysis may optionally use a column of continuous data as species richness, giving how many taxa each tip node actually represents. It may also return the size of the node with each result.


Calculate Shao's N-bar imbalance
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Calculate Shao & Sokal's Nbar metric of imbalance for the tree [REF]__. This returns observed and expected figures.


Calculate Shao's Sigma-squared imbalance
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Calculate Shao & Sokal's Sigma-squared metric of imbalance [REF]__. 


Calculate Colless' C imbalance
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Calculate Colless' C metric of imbalance [REF]__.  This metric can only be calculated for trees with more than 2 tips and no polytomies.


Calculate Shao's B1 balance
~~~~~~~~~~~~~~~~~~~~~~~~~~~

-


Calculate Shao's B2 balance
~~~~~~~~~~~~~~~~~~~~~~~~~~~

-


Calculate stemminess
~~~~~~~~~~~~~~~~~~~~

-


Calculate resolution
~~~~~~~~~~~~~~~~~~~~

-


Calculate ultrametricity
~~~~~~~~~~~~~~~~~~~~~~~~

-




Program and Run Queue Menu
--------------------------

-


Preferences Menu
----------------

-


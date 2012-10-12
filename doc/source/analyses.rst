Analyses
========

Analyses calculate metrics or gather data on trees. This is always non-destructive: an analysis never alters a tree or tree data. Analyses can be carried out as part of the queue or from the top-level Instant Analysis menu. Unless otherwise stated or set by the manner of execution, analyses are always calculated upon the currrent or default tree.


Calculate number of extant taxa
-------------------------------

How many living tips does the tree have? This will be like the total number of tips, except where extinction has been simulated and taxa have died.


Calculate total number of taxa
------------------------------

How many taxa does the tree have? This will be like the number of extant taxa, except where extinction has been simulated and taxa have died.


Calculate genetic diversity
---------------------------

Calculate GD after the manner of Crozier [REF]_. Note that this requires branch lengths to be allelic (i.e. expressed as a probability of alleles differing on either end of a branch).


Calculate phylogenetic diversity
--------------------------------

Calculate PD after the manner of Faith [REF]_ and May/Nee [REF]_. Note that this is rooted PD.


Calculate tree information
--------------------------

Calculate a series of simple metrics for a tree. This can include the number of nodes, number of leaves (tips), the number of extant taxa, whether the tree is neoontological or paleontological and the tree's phylogenetic age (distance from fartherest tip to root).


Calculate information over nodes of tree
-----------------------------------------

Calculate simple information for all nodes of the tree, or just the leaves and internal nodes. This can include the ndoe age, the time (branchlength) to parent, the number of immediate children, the number of leaves (tips) ultimately subtended, the size of the total subtree subtended, the number of sibling nodes, the height of the node (i.e. the distance to the nearest subtended tip) and the time to root (i.e. the total distance to the root of the tree).


Calculate information over nodes
--------------------------------

This is an experiemental variant of the above analysis, that allows for a more sophisticated selection of nodes, based on trait value, whether the node is extant or how many tips it subtends.

Calculate Fusco imbalance
-------------------------

Calculate Fusco's I metric of tree imbalance for each applicable node in the tree [REF]_. (This metric may only be calcuated over bifurcating nodes.) The analysis may optionally use a column of continuous data as species richness, giving how many taxa each tip node actually represents. It may also return the size of the node with each result and use the extended I' calculation (see [fusco_mod]_).


Calculate Slowinski-Guyer imbalance
------------------------------------

Calculate Slowinski-Guyer's metric of imbalance for each applicable node in the tree [REF]_. (This metric may only be calcuated over bifurcating nodes.) The analysis may optionally use a column of continuous data as species richness, giving how many taxa each tip node actually represents. It may also return the size of the node with each result.


Calculate Shao's N-bar imbalance
--------------------------------

Calculate Shao & Sokal's Nbar metric of imbalance for the tree [REF]_. This returns observed and expected figures.


Calculate Shao's Sigma-squared imbalance
-----------------------------------------

Calculate Shao & Sokal's Sigma-squared metric of imbalance [REF]_. 


Calculate Colless' C imbalance
-------------------------------

Calculate Colless' C metric of imbalance [REF]_.  This metric can only be calculated for trees with more than 2 tips and no polytomies.


Calculate Shao's B1 balance
---------------------------

-


Calculate Shao's B2 balance
---------------------------

-


Calculate stemminess
--------------------

-


Calculate resolution
--------------------

-


Calculate ultrametricity
------------------------

-


Overview and concepts
=====================

To explain MeSA and give a better idea of how it works, here I'll sketch out a "mile-high" picture of what it does and how it does it.

Data
----

MeSA loads, generates and manipulates three types of data:

* **Phylogenetic trees:** trees and the taxa (often species) that are within and at the tips of these trees. This includes neontological and paleontological trees. If multiple trees are loaded, at any time one is designated as the **active tree**, upon which analysis takes place. Trees consist of **nodes** and **branches**, with associated branchlengths.

* **Discrete trait data:** categorical (string) data that is associated with taxa on the tree(s). Examples of this include ecosystem type, animal behaviour or 
  
* **Continuous trait data:** real (floating point) data that is associated with taxa on the tree(s). Examples of this include average weight, lifespan or typical number of offspring. This includes **abundance or richness data**, counts of the number of taxa or indivduals represented by 


Files
-----

One dataset can be loaded at any time, but it can contain any number of trees or traits. 
Data can be loaded from or saved to Nexus files. 

In addition, analysis results can be **reported**, logged to files, which is useful when producing metrics across a large number of trees.


Measuring and changing trees
----------------------------

An **analysis** over a tree  returns a metric calculated for the tree or its contents, e.g. the tree size, imbalance, number of nodes subtended for each node. A **quick or instant analysis** calculates a metric on the acive tree. However, analyses can also be batched in the queue.

A **manipulation** is an action that changes a tree, e.g. deleting taxa, transforming branchlengths, altering taxa. Typically such manipulations are not targetted at specific nodes but act on the tree, e.g. delete 30% of tips. As with analyses, a manipulation can be carried out on the spot or batched in the queue.


The queue and simulation
------------------------

MeSA allows you to batch analyses and manipulations and run them as a whole. This serves two purposes: replication and simulation.

In the first case, a queue can be run repeatedly, say over a set of trees or multiple times over a single tree. This can used for example to calculate metrics for a population of trees or the distribution of possible results fior a single tree.

The queue can be used to construct simulations, in which trees can be "grown". A simulation consists of a series of **epochs**. Each epoch lasts until its end condition is met, which might be a time limit or number of taxa. Epochs contain **rules** that are applied to the active tree. Rules have a given **rate** (how often they apply) and an effect, which may be the evolution of taxa traits, speciation, extinction, etc. Analyses and manipulation may be attached to epochs, to trigger when certain conditions apply. 


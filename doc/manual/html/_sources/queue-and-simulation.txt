The Queue and Simulations
=========================

Why batch?
----------

MeSA provides the queue as a way a way of batching analyses and manipulations of trees together, to be run as a single unit. This is to two ends, replication and simulation:

**Replication:** often you want to assess a metric or result across a group of trees or the distribution of possible effects on a single tree. For example: 

* A researcher may wish to know metrics differ across a set of trees, such as looking at imbalance statistics across a set of mammalian phylogenies.

* Given a tree with uncertain topology, represented by a set of possible trees output by a program like MrBayes, metrics can allow for this uncertainty by calculating over all trees and using the resultant distribution to calculate confidence limits.

* The expected effect of a manipulation - such as a random mass extinction - may need to be quantified by repeating that manipulation multiple times on the same tree, taking measurements after each change and then restoring the tree to its original state.

**Simulation:** much of evolutionary research is based on forensic approaches - dissecting available "real" phylogenies for evidence of evolutionary events or modes. This "paleontological" approach can be thwarted by a lack of useful phylogenies, ignorance about what happened deep in the evolutionary history and reconstruction biases. An alternative approach is study evolutionary events in an emergent or synthetic way. That is, recreate the phylogenies that result under particular scenarios and compare these to realworld phylogenies:

* If extinction occurs selectively, say disporportionately to creatures with a given trait, what effect does this have on the phylogenetic topology? Do any "real" phylogenies look like these simulated ones and so may have experienced a similar regime.

* If our "sampling" of paleontological taxa is distorted and we tend to see more of some type of taxa than others, what might the real phylogenies look like?

* If speciation rate is an evolvable characteristics, why isn't the world overwhelmed with "weed" species - taxa that speciate more rapidly than others? 


Using the queue
---------------

You will interact with the queue in number of ways:

* **Programming:** adding analyses, manipulations and simulations to the queue

* **Running:** exexcuting the contents of the queue.


Programming
~~~~~~~~~~~


Running
~~~~~~~

The queue can be executed in a number of ways. Note that none of these alters the content of the queue - a queue may be run and then run again. 

Go! (run the queue)
	Execute the queue. Each elemnt in the queue is executed one-by-one.
Go! (run the queue N times)
	Repeatedly execute the queue a defined number of times, one after the other.
Go! (run the queue & restore)
	Repeatedly execute the queue a defined number of times, one after the other. After every iteration, restore all data (tree, categorical and continuous traits) to their original value.
Go! (run the queue across all trees)
	Repeatedly execute the queue for every tree, one tree after the other.

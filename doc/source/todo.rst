To do
=====

Possible changes and fixes:

* Improve Makefile for better builds across platforms

* Check Prune functions, which don't look like they actually work

* Add to documentation, including recipes and examples

* Better editting of analysis queue

* Embedded scripting and so forth

* Embedded instructions / executions in Nexus files

* Really, this should all be ported to Python ...



.. Here is the queue I set up:

.. There are 22 actions programmed in the queue.
..   1   epoch: evolve while (extant taxa < 1024)
..   2      evol rule: cont. trait-biased speciation (params 2, 2, 2)
..   3      evol rule: markovian extinction (rate 0.0001)
..   4      trait change (symmetrical speciational)
..   5         main scheme set
..   6            cont brownian change scheme (mean 0.1, std dev 0.05 per time)
..   7   analyse: phylogenetic diversity
..   8   reduce tree to neontological core
..   9   set clade label format to phylogenetic
..   10  analyse: Colless' C imbalance
..   11  analyse: tree information
..   12  epoch: evolve until (5 <= time)
..   13     evol rule: cont. trait-biased speciation (params 2, 2, 2)
..   14     evol rule: markovian extinction (rate 0.0001)
..   15     trait change (symmetrical speciational)
..   16        main scheme set
..   17           cont brownian change scheme (mean 0.1, std dev 0.05 per time)
..   18  analyse: phylogenetic diversity
..   19  reduce tree to neontological core
..   20  set clade label format to phylogenetic
..   21  analyse: Colless' C imbalance
..   22  analyse: tree information

.. Note that this doesn't contain all the details that I set e.g. in the
.. trait change model.  (I also couldn't find any way to save the queue
.. settings.)  When I run this, the first epoch runs through, but the
.. second one (the time limit) doesn't return, even after 45 minutes.

.. Save the analysis results in which file [default '']?          : save4
.. Running action queue once ...
.. run once        phylogenetic diversity   99.952934
.. run once        Colless' C imbalance     0.019149
.. run once        tree information         extant taxa:   1024
.. run once        tree information         phylogenetic age:      0.845633
.. run once        tree information         paleo/neo:     neontological

.. Ooh, nice complicated queue. Let me try that.

.. Other problems/questions from my attempts at using it:

.. --evolution of continuous trait doesn't happen even though I specify
.. nonzero/non-unity parameters
..         --OK fixed that, but what is the connection between continuous trait
.. evolution and granularity of time?
.. --how do the ABC parameters actually help specify the rate of trait evolution?
.. --what are the time units in time limit epochs?  Minutes of system time?
.. --no clear way to correct mistakes made in queue setup
.. --don't understand all the options in trait evolution schemes
.. --how to save queue configuration options?  Rather frustrating to have
.. to redo it every time
.. --time limit epochs don't seem to produce a result, program hangs
.. --how to keep tree size fixed during time limit epochs, i.e. to make
.. sure any speciation is exactly balanced by extinction?
.. --no messages on progress of a run even when verbosity is set to loud
.. --got a time limit epoch to follow a size limit epoch (time = 1) but
.. evolution doesn't seem to be happening--MeSA problem or my problem?
.. --when taxa are pruned with the prune commands, is this done randomly?
..         --why does the # of extant taxa go UP after pruning?  (1024 before,
.. asked to prune 512, count extant taxa = 6836)
..         --have to pick "none" when asked about species richness data to use
.. in order to get correct no. of taxa after pruning
.. --no calculation for PHG?
.. --what metric is used for stemminess?  Is it cumulative stemminess?

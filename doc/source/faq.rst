Frequently asked questions
==========================

What units of time does MeSA use?
---------------------------------

The time units are simply that - time units. They are what the rates and tree branch lengths and time are expressed as or against and could potentially be anything. Thousands of years, millions of years? Doesn't matter - it's all just a scale to apply things against. But note that tree-time has little to do with execution time. A short length of tree-time does not necessarily take a short time to simulate. See the following question.


How long will my tree take to grow / simulate?
----------------------------------------------

This is tricky, because time can take a varying amount of time.

Tree-time (branch length units) in MeSA is simulated evolutionary time. Execution time (how long it takes a simulation to run) is only poorly correlated with this. Growing bigger trees (with more tips) over longer periods of time (branch lengths) will generally take longer. But:

* *More rules take longer to assess.* MeSA simulates tree growth by testing every rule in the epoch against every living taxa and seeing which rule will "fire off" next. So this assessment time increases as a product of the number of taxa and the rules.

* *Evolution of continuous traits can be slow.* To ensure a smooth change in continuous traits, their evolution is discretized into a series of smaller, more frequent changes. This can mean that simulations of such scenarios are quite slow as these small changes are repeatedly applied, thousands of times.

* *The rate of tree growth is unpredictable.* Consider a size-limited epoch with only one rule, speciation at a very low rate. The time until the first speciation can vary hugely. But once that first speciation takes place, the subsequent speciations will be much faster, due to there being two possible taxa that can speciate. The rate will exponentially increase with each subsequent speciation. This means that the tree-time can vary widely across simulated trees as those which speciate early race away from the others.

* *Rules can interact in complex ways.* Consider a size-limited epoch with two rules: one speciation and one background extinction that are at nearly equal rates. Your epoch will take a long time to complete as the net rate of your tree growth will be very low. Some examples will even result in total extinction.

In summary, it's hard to predict how long your run will take. Generally it will be consistent between replicates of the same scenario, but even this can vary due to stochastic events early in the history of the tree.


My tree or simulation never finishes. Is MeSA broken?
-----------------------------------------------------

Maybe. But it's very easy to write a simulation that will never finish or run very slowly. Let's consider that possibility first.

Every epoch needs an end-condition and obviously has to fulfil that condition for the epoch to terminate. It is easy to accidentally create an epoch that never finishes, e.g. a time- or size-limited epoch that contains no speciation rules. More complex examples are possible, as are cases where the end condition could be reached but is unlikely to, e.g. a size-limited epoch with periodic mass-extinctions that keep it below that limit, speciation rates depending on a trait value that is unlikely to evolve, nearly balanced speciation and extinction rates.

The variety of possible scenarios is such that there is no way for MeSA to detect whether they are never-ending or ultra-slow. If you think there is a problem, I suggest altering the rate of certain rules or lowering the end-condition for an epoch to see if the problem persists. Periodically dumping out the tree state and examining will also help determine if the tree is actually growing.


I don't understand why I'm getting these simulated trees
--------------------------------------------------------

First, don't panic. This is a common situation, as a set of evolutionary rules can interact in unexpected and emergent ways, resulting in 


What's a singleton node? This error message complains about one
---------------------------------------------------------------

A singleton node has one child. For example, in the Newick code ``(A, (C))``, the parent node of C is a singleton: the root node begets A and B, B begets C. This can be a biologically-true representation: a taxa transforms abruptly or gradually to another. Howver, in MeSA it is more usually the result of incomplete knowledge (i.e. the only descendant of B we know of is C), or pruning (i.e. the only extant decsendant of B is C).

In either case, this can present problems for some algorithms that expect nodes to be at least bifurcating. Also, when a tree is exported to a file, some other programs may refuse to read it, if it contains singletons. To solve this problem, a manipulation is provided that collapses singletons, essentially excising the problematic node and joining the parent and child with a new branch. In the example above, we would end up with ``(A, C)``. The new branch is the length of the two deleted branches.

.. note::
	This is a neologism necessarily coined duing the use of MeSA. There may be a pre-existing term but I am unaware of it.


What's a manipulation? Or an analysis?
--------------------------------------

In MeSA, anything you can do to a tree can be classified as a "manipulation" or "analysis". The former mutates (changes) a tree or trees. The second makes and returns the result of calculations across a tree or trees but does not change them. In practice, these are used the same way, but it is useful to make a distinction between things that change your data and things that give you answers.


What's a paleontological (or neontological) tree?
-------------------------------------------------

A paleontological tree is one within which the tips of interest are of different ages, i.e. different distances from the root, or are considered to be extinct. In contrast, a neontological tree is one in which the tips of interest are all contemporaneous, i.e. the same distance from the root, and considered to be alive. This is largely synonymous with being ultrametric. 


Can MeSA handle unrooted trees?
-------------------------------

Yes. And it also depends.

Trees may be labelled in a Nexus file as rooted or unrooted and are loaded as such. Any loaded from other sources are assumed to be rooted. MeSA contains manipulations for (un)rooting trees, inter-converting these two forms. Certain analyses and manipulations only make sense for rooted trees, usually because they make calculations based on the root of the tree. Should you try to use these on an unrooted tree, MeSA will complain. 

However, most of MeSA's intended purpose concerns rooted trees. 

Any trees that are created in MeSA are rooted. Also, only rooted trees can be used for simulations.


I've been asked to express a rate as three parameters: A, B, C
--------------------------------------------------------------

In various places in MeSA, rates can be set to be calculated dependent on some other variable. This calculation can be expressed as three factors, as so::

	r = a * x^b + c

That is, the rate is equal to the test variable to the power of *b*, multiplied by *a* and added to *c*. Put another way, *c* is the background rate uneffected by the test variable. *a* is the slope of the response to the test variable. *b* is the shape of the response.

Using these, a wide variety of rates can be programmed:

* constant and unresponsive to variable (``a << c``)

* dependent on variable (``a >> c``)

* inversely porportional to variable (``a`` or ``b`` is negative )

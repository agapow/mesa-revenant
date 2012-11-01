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

Manipulations
=============

Manipulations alter trees and tree-associated data. Manipulations can be carried out as part of the queue or from the top-level Data Maniulation menu. Unless otherwise stated or set by the manner of execution, manipulations always act upon the currrent or default tree.


Pruning and extinction
----------------------

A common use-case with MeSA is the deletion of terminal tips to simulate the extinction or other loss of taxa. 

Prune (fixed number of taxa)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This affects an absolute number (e.g. 2, 20, 200) of extant taxa.


Prune (fixed fraction of taxa)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This affects a set fraction (e.g. .02, .2) of currenty extant taxa. This works by calculating the correct number (rounding off to the nearest integer) and then handling it as per the fixed prune. For example, such a prune at 50% (0.5) will kill 8 taxa in a tree with 15 extant taxa.


Prune (fixed prob for each taxa)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This effects extant taxa with a fixed probability (e.g. .02, .2). This works by testing every extant taxa against this probability and then pruning those that are selected. For example, such a prune at 50% (0.5) will usually kill 7 to 8 taxa in a tree with 15 extant taxa.


Prune (if taxa meets condition)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This effects extant taxa with a fixed probability (e.g. .02, .2). This works by testing every extant taxa against this probability and then pruning those that are selected. For example, such a prune at 50% (0.5) will usually kill 7 to 8 taxa in a tree with 15 extant taxa.


Other node deletion
-------------------

Delete dead taxa
~~~~~~~~~~~~~~~~

Collapse singletons
~~~~~~~~~~~~~~~~~~~

Certain algorithms are unable to handle singleton nodes (

Other manipulations
-------------------

Add a new trait
~~~~~~~~~~~~~~~

Set branchlengths
~~~~~~~~~~~~~~~~~

Duplicate the active tree
~~~~~~~~~~~~~~~~~~~~~~~~~




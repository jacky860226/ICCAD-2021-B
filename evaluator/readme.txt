Usage: ./ <input> [<output>] [--verbose] [--dbgrow r] [--dbgcol c] [--dbglay l]
Options:
    -v, --verbose          verbose level          
    -r, --dbgrow           debug ggrid row        
    -c, --dbgcol           debug ggrid col        
    -l, --dbglay           debug ggrid lay        
    -h, --help             Shows this page     

Example: ./evaluator case1.txt case1_example_output.txt
Example: ./evaluator case3.txt --dbgrow 13 --dbgcol 6 --dbglay 5
Example: ./evaluator case1.txt case1_example_output.txt --verbose

The evaluator will first show you the information in <input> it parsed.
For example:

MaxCellMove  2
GGridBoundaryIdx  1155
NumLayer 3
NumNonDefaultSupplyGGrid 3
NumMasterCell 3
NumNeighborCellExtraDemand 2
NumCellInst 8
NumNets 6
NumRoutes 42
NumVoltageAreas 1

If you see difference between these numbers and the cases.
Means that there are some format errors in the input file.
(Only happens when you modify input file,
for example you added a MasterCell but forgot to increase NumMasterCell.
Or you missed some keyword on certain lines.
Or you accidentally move a cell instance to the voltage area group which does not belong to its voltage area group.
We only do rigorous checking on output answer file but not on input file.)

Then it will show the total gGrid length of initial routes.
And show "<---- Input Valid! ---->" as our initial routes will guarantee connected all pins of all nets.


Next it will show you the information of <output>
NumMovedCellInst 2
NumRoutes 40

These numbers should be identical to the file in <output>

Then it will do the evaluate if you see "<---- ReadOutput Success! ---->"

Then, congratulations if you see "<---- Output Valid! ---->"
The "Total gGrid length: XXXXX" shows the final score for this case.
Evaluator will show gGrid length even nets are open or some gGrid overflown or some.
But only the answer with "<---- Output Valid! ---->" is valid.


If you see evaluator shows any line starts with [Error!].
Then it means that you answer is wrong. And the error reason and line number is described subsequently.
And you will either see "<---- ReadInput Failed! ---->" or "<---- Output Invalid! ---->".
Then the score will be 0 for this case.

If you see the evaluator prints the error messages below:
...
...
Error: cell C1 violates voltage area constraint!
       required group name: V1
       placed location: {4 1}
...
...
Which means you have a cell instance C1 placed in an invalid location {4 1}, where location {4 1} is not involved in group V1.


The line starts with [WARN!] shows some warning message but will not make you fail the case.

Add --verbose in the last argument to turn on verbose mode.
In verbose mode, evaluator shows the connectivity of each net.
The gGrid length of each net. And the extra demand information.

The evaluator now automatically generate "input.dmd" and "output.dmd" for you to verify that
if your demand calculation is consistent with evaluator.

If you have any question or find any bugs of the evaluator, please email to cad.contest.iccad@gmail.com


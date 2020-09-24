# Evaluator
Scans, parses, and evaluates user inputed data. All decimal, binary, and hexadecimal numbers are valid up to 32 bits. 
Valid operations are : {+, -, *, /, &, |, <<, >>, >-(Arithmetic shift right), ~, ^}.

Outputs data in either base (-b): 2, 10, or 16.

Outputs data in either width (-w): 4, 8, 16, or 32(default).

Use (-v) to print scanning and parsing trees along with the final output.

Use (-u) to print final output as unsigned.


To run:
make 

./project02 -e (-b) <base> (-w) <width> "<expression to be evaluated>" (-v) (-u)

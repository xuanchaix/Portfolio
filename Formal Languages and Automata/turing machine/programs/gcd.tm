; solve gcd
; input should be 1+01+

; the finite set of states
#Q = {start, accept, copy, back, setpivot, goright, find_small, go_first, go_second, delete_0, delete_first, delete_second}

; the finite set of input symbols
#S = {0,1}

; the complete set of tape symbols
#G = {0,1,_}

; the start state
#q0 = start

; the blank symbol
#B = _

; the set of final states
#F = {accept}

; the number of tapes
#N = 2 

; the transition functions

; State start: begin
start ** ** ll setpivot

; State setpivot: set 0 in front of each number 
setpivot ** 00 rr goright

; State goright: find the second number
goright 1* ** r* goright
goright 0* _* r* copy

; State copy: copy the second number to the second line
copy 1* _1 rr copy
copy _* ** ll back

; State back: back to the start
back 0* ** *l back
back *0 ** l* back
back ** ** ll back
back 00 ** rr find_small

; State find_small: find the smaller number
find_small 11 ** rr find_small
find_small 1_ ** rl go_first
find_small _1 ** lr go_second
find_small __ ** l* delete_0

; State go_first: go to the right most of the first number
go_first 1* ** r* go_first
go_first _* ** l* delete_first

; State delete_first: delete the first number
delete_first 11 _* ll delete_first
delete_first 10 ** l* back

; State go_second: go to the right most of the second number
go_second *1 ** *r go_second
go_second *_ ** *l delete_second

; State delete_second: delete the second number
delete_second 11 *_ ll delete_second
delete_second 01 ** *l back

; State delete_0: delete the pivot 0
delete_0 1* ** l* delete_0
delete_0 0* _* r* accept


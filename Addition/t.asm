ldc 0x1000
a2sp
call main
HALT

main: a2sp
 ldc values
ldnl 0
ldc values
ldnl 1
add
sp2a
return

values: data 8
data 12
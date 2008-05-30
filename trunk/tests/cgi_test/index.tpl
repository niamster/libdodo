index
<(include $main)>

<(print "a=", $a, "b=", $b)>

<(if $a!=$b)>
	a!=b
<(else)>
	a == b
<(fi)>

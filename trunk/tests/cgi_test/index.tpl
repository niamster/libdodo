<(if $main)>
	<(include $main)>
<(else)>
	<(include "index.tpl")>
<(fi)>

<(print "a=", $a, " b=", $b)>

<(if $a!=$b)>
	a != b
<(else)>
	a == b
<(fi)>

<html>
<head>
</head>
<body>

<div>
<(> not )> parsed  <)>
</div>

<(* if statement *)>
<div>
		<(if $test!=hoho )>
			<(print $test)>
		<(else)>
			NULL
		<(fi)>
</div>

<div>
<(if $test )>
	blabla
	<(if $test==hoho )>
		HOHO
	<(fi)>
	<(if $test==hihi )>
		HIHI
	<(fi)>	
	
	<(if $test )>
		blabla
		<(if $test==hoho )>
			<(print $test)>
		<(else)>
			NULL
		<(fi)>
	<(else)>	
		<(if $test!=hihi )>
			HOHO
		<(fi)>	
		<(if false )>
			WTF?
		<(fi)>
	<(fi)>	
	
	<(if 3>2 )>
		3>2
	<(fi)>

	<(if 3<2 )>
		3<2
	<(fi)>
	
	<(if 3!2 )>
		3!2
	<(fi)>
	
	<(
		if
		! `false`
	)>
		OK!
	<(fi)>
	
	<(if!false)>OK!<(fi)>
	
	<(if ! 'false' )>
		OK!
	<(fi)>		
	<(if ! "false" )>
		OK!
	<(fi)>	
<(fi)>
</div>

<div>
<( print bla-bla )>
<( print $show )>
</div>

<div>
<(for $i in $arr)>
	<(for $b in $arr)>
		<div class="test"><(print $b)></div>
	<(rof)>
<(rof)>
</div>

<div>
<(print $test )>
<(print $arr.0.0 )>
<(print $arr1.one )>

<(print $arr2.0.one )>
<(print $arr2.1.one )>
</div>

<div>
<(for $b in $arr2)>
	<(for $c in $b)>
		|<(print $c)>|<(print $b.one)>
	<(rof)>
<(rof)>
</div>

<div>
<(for $c in $arr2.1)>
	|<(print $c)>
<(rof)>
</div>

<div>
<(print {$arr2.{0}.{$one}} )>
<(print $arr2.1.{$arr1.{$one}} )>
</div>

<div>
<(print namespace: )>

<(for $b in $arr2)>
	<(for $b in $b)> <(* HERE : `$b in $b` and it's OK! *)>
		|<(print $b)>
		<(print $dodo.iterator)>
	<(rof)>
	|<(print $b.one)>
	<(print $dodo.iterator)>
<(rof)>
</div>

<div>
<(assign a = bASDA)>

<(if true )>	

	<(assign a = ASDA)>

	<(for $b in $arr2)>
		<(for $b => $c in $b)>
			<(if $b == three)>
				<(break 3)>
			<(fi)>
			<(print $b)> -> <(print $c)>
			
			<(assign a = SDA)>
			<(print $a," must be SDA")>
		<(rof)>
		-------
	<(rof)>	
	
	<(print $a," must be ASDA")>
	
<(fi)>

<(print $a," must be bASDA")>
</div>
	
<(ns)>
		<(ns)>
			<(assign a = in namespace)>
			<(print $a," must be `in namespace`")>
		<(sn)>
	<(print $a," must be bASDA")>
<(sn)>

<(print $arr2.1.{$arr1.{$one}} , !!!! , {$arr2.{0}.{$one}})>

<(print $dodo.version, " ", $dodo)>

</body>
</html>
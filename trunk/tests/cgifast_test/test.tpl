<(> not )> parsed  <)>

<(* if statement *)>

<(if $test!=test )>
	<(print $test)>
<(else)>
	NULL
<(fi)>

<(if $test )>
	$test
	<(if $test==test )>
		test
	<(fi)>
	<(if $test==test1 )>
		test1
	<(fi)>	
	
	<(if $test )>
		test
		<(if $test==test )>
			<(print $test)>
		<(else)>
			NULL
		<(fi)>
	<(else)>	
		<(if $test!=test )>
			test
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

<( print bla-bla )>
<( print $show )>


<(for $i in $arr)>
	<(for $b in $arr)>
		<div class="test"><(print $b)></div>
	<(rof)>
<(rof)>


<(print $test )>
<(print $arr.0.0 )>
<(print $arr1.one )>

<(print $arr2.0.one )>
<(print $arr2.1.one )>


<(for $b in $arr2)>
	<(for $c in $b)>
		|<(print $c)>|<(print $b.one)>
	<(rof)>
<(rof)>

<(for $c in $arr2.1)>
	|<(print $c)>
<(rof)>

<(print {$arr2.{0}.{$one}} )>
<(print $arr2.1.{$arr1.{$one}} )>

<(print namespace: )>

<(for $b in $arr2)>
	<(for $b in $b)> <(* HERE : `$b in $b` and it's OK! *)>
		|<(print $b)>
		<(print $dodo.iterator)>
	<(rof)>
	|<(print $b.one)>
	<(print $dodo.iterator)>
<(rof)>

<(assign a = test1)>

<(if true )>	

	<(assign a = test2)>

	<(for $b in $arr2)>
		<(for $b => $c in $b)>
			<(if $b == three)>
				<(break 3)>
			<(fi)>
			<(print $b)> -> <(print $c)>
			
			<(assign a = test3)>
			<(print $a," must be test3")>
		<(rof)>
		-------
	<(rof)>	
	
	<(print $a," must be test2")>
	
<(fi)>

<(print $a," must be test1")>
	
<(ns)>
		<(ns)>
			<(assign a = in namespace)>
			<(print $a," must be `in namespace`")>
		<(sn)>
	<(print $a," must be test1")>
<(sn)>

<(print $arr2.1.{$arr1.{$one}} , !!!! , {$arr2.{0}.{$one}})>


<(>
<(print $dodo.version, " ", $dodo)>
<)>

<(print $dodo.version, " ", $dodo)>

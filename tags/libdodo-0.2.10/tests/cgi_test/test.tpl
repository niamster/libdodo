blabla <(include "menu.tpl")> HIHI <(include $main)>

<(> not )> parsed  <)>

<(* if statement *)>

		<(if $test!=hoho )>
			<(print $test)>
		<(else)>
			NULL
		<(fi)>

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
	<(rof)>
	|<(print $b.one)>
<(rof)>

<(if true )>	
	<(for $b in $arr2)>
		<(for $b => $c in $b)>
			<(if $b == three)>
				<(break 3)>
			<(fi)>
			<(print $b)> -> <(print $c)>
		<(rof)>
		-------
	<(rof)>	
<(fi)>

<(print $arr2.1.{$arr1.{$one}} , !!!! , {$arr2.{0}.{$one}})>
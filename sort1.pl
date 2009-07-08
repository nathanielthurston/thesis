while (<>) {
	chomp;
	if (/adding (\d+)=([gGnNmM]+)/) {
		$n = $1 + 7;
		$w = $2;
		$word[$n] = $w;
	}
	if (/ELIMINATED (\d+)@(\d+)/) {
		$word = $word[$1];
		$word = $1 if $word eq '';
		print "$2 $word\n";
	}
}
